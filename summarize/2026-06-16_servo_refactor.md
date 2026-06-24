# 2026-06-16 舵机 UI 重构工作汇总

> Commit: `52e5fb8`
> 类型: `refactor(servo)`
> 改动规模: 8 个文件 / 179 行新增 / 983 行删除

## 背景与目标

将项目里分散在三处的舵机控制 UI（顶部导航的"舵机调试"页 + 手动页舵机板块 + 弹窗式 `ServoTestWidget`）整合成**单入口的弹窗控制**模式：

- **入口唯一化**：所有控制都通过"程序编程"页底部的"舵机设置"按钮弹模态对话框完成
- **手动页舵机板块**改为"仪表盘"——只显示 6 路当前角度，被动接收 `ServoController::angleChanged` 信号实时刷新
- **删除冗余页面**：`pageServo`（顶部导航第 6 项）整页移除

## 改动清单

### 1. CMake 三件套（新增 `robot_ui` 静态库）

| 文件 | 改动 |
|---|---|
| `Robot_R06/src/CMakeLists.txt` | 追加 `add_subdirectory(ui)` |
| `Robot_R06/src/ui/CMakeLists.txt` | **新建** — `robot_ui` 静态库，包含 `ServoTestWidget.cpp/.h` |
| `Robot_R06/CMakeLists.txt` | `target_link_libraries(Robot_R06)` 加 `robot_ui` |

> 项目里 `ServoTestWidget.cpp` 之前**未被编译进任何库**（`src/ui/` 缺 CMakeLists.txt），本次把链路补上。

### 2. mainwindow.ui：删页 + 导航重命名

- **删除** `btnNavServo` 导航按钮（顶部导航第 6 项的 `<item>` 块）
- **删除** `pageServo` 整个页面（含 OUT0/OUT1 滑块、初始化/停止/应用 4 个按钮、breadcrumbServo 标签）
- **重命名** 顶部导航 `手动&编程` → `程序编程`
- **重命名** 面包屑 `🎮 手动 & 编程 > 点动控制 / 脚本编辑器` → `📝 程序编程 > 点动控制 / 脚本编辑器`

### 3. mainwindow.cpp：核心重构

| 行号 | 改动 |
|---|---|
| 31 | 加 `#include <QDialog>` |
| 98 | 删 `m_navGroup->addButton(ui->btnNavServo, 6);` |
| 113 | 删 `initServoTestUI();` 调用 |
| 1025-1026 | 状态栏 names 数组 7 项→6 项；`"手动 & 编程"`→`"程序编程"`；`index < 7`→`index < 6` |
| 1370 附近 | 删 `// 舵机调试` 注释块 |
| 1402-1687 | **整个 `initServoTestUI()` 函数体删除**（约 320 行） |
| 1817-1973 | **手动页舵机板块重写**：5 控件行（±/角度/速度/±/运行）→ 2×3 QGridLayout + 6 个 `QLabel` 只读显示 |
| 1817 后 | 新增 `ServoController::angleChanged` 订阅 → 实时刷新 6 路 Label |
| 1950 后 | 新增"舵机设置"按钮 → 弹模态 `QDialog` 复用 `ServoTestWidget` |

### 4. mainwindow.h：追加成员

```cpp
QList<QLabel*> m_servoAngleLabels;  // 手动页舵机板块 6 通道只读角度显示（被动显示）
```

`m_servoTestWidget` 此前已 forward-declared 并初始化为 `nullptr`，本次实装使用。

### 5. ServoTestWidget.cpp/.h：功能精简 + 体验优化

#### 删除部分

- 底部 3 个按钮：`m_initBtn`/`m_stopBtn`/`m_applyBtn`（初始化/停止/应用）
- 3 个对应槽函数：`onInitClicked`/`onStopClicked`/`onApplyClicked`
- 3 个空槽函数：`onSliderChanged`/`onAngleSpinChanged`/`onCheckChanged`
- 私有方法 `syncSliderAndSpin`（与"调试按钮应用"配合的临时桥，删按钮后无意义）

#### 改进部分

| 改进点 | 实现 |
|---|---|
| 滑块拖动真正驱动舵机 | `sliderMoved` → `m_servo->setAngleDrag(i, value)`（50ms timer 批量下发 6 路，**顺滑不卡**） |
| 拖动结束兜底 | `sliderReleased` → `setAngleWithSpeed`（带速度控制到位） |
| 角度输入框联动 | `valueChanged` 同步滑块（不驱动舵机，避免拖动时死循环） |
| 速度输入框生效 | `editingFinished` 调 `setDefaultSpeed` 同步给控制器 |
| **当前角度 Label 实时刷新** | `onAngleChanged` 始终更新 Label（即使 isMoving 也刷新） |
| 勾选=解锁机制 | 新增 `updateChannelLockState(i)`：`m_checkedChannels[i]` true 才启用滑块/角度输入 |
| 未勾选时灰显 | 角度输入框 `setEnabled(false)` + 灰底样式 |
| **未勾选时拖动无效** | 4 个 connect 都加 `m_checkedChannels[i]` 守卫 |
| 默认速度 | `m_speedSpins[i]->setValue(30)` → `setValue(10)`（与 `ServoController` 内部默认对齐） |

## 关键技术点

### 拖动顺滑的奥秘

`ServoController::setAngleDrag(channel, angle)` 是项目专为拖拽场景设计的接口：
- 不单独发帧
- 只更新内部 `currentAngle/targetAngle` + 标记 `isMoving=true`
- 由 50ms `m_timer` 一次 `setAllPwm` 批量下发 6 路
- **同帧下发 → P05/P06 不滞后**

`onTimerTick` 已通过 `angleChanged` 信号 emit 各通道中间角度，弹窗和手动页都订阅这个信号做"当前角度"显示。

### 防重复弹窗

```cpp
if (m_servoTestWidget && m_servoTestWidget->isVisible()) {
    m_servoTestWidget->raise();
    m_servoTestWidget->activateWindow();
    return;
}
```

`Qt::WA_DeleteOnClose` 让 dialog 关闭时自动销毁子 widget；`exec()` 返回后 `m_servoTestWidget = nullptr` 防野指针。

## 排查的 1 个关键 Bug

### PowerShell 写文件破坏 UTF-8 编码

**症状**：MSVC 编译报几十个 C2001（"常量中有换行符"）

**根因**：之前用 `Set-Content -Encoding utf8` 整体重写 `mainwindow.cpp`（删除 `initServoTestUI` 函数体时），PowerShell 5.1 读入时把 UTF-8 字节误当 ANSI（GBK）解码，写出时虽然加了 BOM，但中文 UTF-8 多字节序列**已被错误转换**。结果是 MSVC 报几十个 C2001 错。

**修复**：
- `git checkout HEAD -- mainwindow.cpp` 恢复到干净状态
- 之后**只用 `Edit` 工具按字节精确匹配做小段修改**，不重写整个文件
- 大段删除用 `head -c + tail -c + cat` 字节级拼接（不经过任何字符解码）

**教训**：
- 涉及 UTF-8 中文源码的修改，**优先用 Edit 工具**（按字节精确匹配）
- **不要用 PowerShell `Set-Content` 整体重写**含中文的源码
- 大段删除可用 `head -c N | tail -c +M | cat` 字节级拼接
- 头字节验证：`head -c 4 file.cpp | xxd` 应该是 `2f 2a 0d 0a`（`/*\r\n`，无 BOM）或 `ef bb bf ...`（带 BOM）

## 现在的用户体验

### 顶部导航（6 项）

```
[运行] [程序编程] [IO 监控] [报警日志] [设置] [通信]
```

### 程序编程页

左侧舵机板块 2×3 紧凑布局：

```
舵机
─────────────────────
CH0:  90.0°    CH1:  90.0°
CH2:  90.0°    CH3:  90.0°
CH4:  90.0°    CH5:  90.0°
        [ 舵机设置 ]
```

- 6 路 Label 只读，**实时刷新**（弹窗里改角度，Label 跟手）
- 底部"舵机设置"按钮弹出模态 dialog

### 弹窗（舵机调试）

6 行（CH0-CH5），每行：
- 勾选框
- 通道标签
- 角度滑块（拖动实时驱动）
- 角度输入
- 当前角度 Label（实时）
- 速度输入（10°/s 默认）

**必须勾选才能控制**（未勾选时滑块/角度输入灰显禁用）。

## 文件结构

```
src/ui/
├── CMakeLists.txt           # 新建
├── ServoTestWidget.h
└── ServoTestWidget.cpp
```

之前 `ServoTestWidget.cpp` 是"死代码"（没人调用也没人编译），现在正式接入构建。

## 下一步建议

1. **重新编译验证**（建议先清 build 目录，避免 cmake 缓存）
2. 验证弹窗拖动时 6 路舵机是否真顺滑（特别是 P05/P06 不滞后）
3. 验证"勾选才能控制"机制是否符合预期
4. 如果"勾选"也不需要了，告诉我，我可以把勾选框也去掉，弹窗就只剩纯滑块控制
