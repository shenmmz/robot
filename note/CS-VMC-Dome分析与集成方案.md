# CS-VMC-Dome 代码分析与集成方案

## 一、CS-VMC-Dome 代码架构分析

### 1.1 文件结构

```
external/CS-VMC-Dome/
├── main.cpp                    # 示例程序入口
├── mainwindow.h/cpp/ui         # 示例主窗口（演示层）
├── CMakeLists.txt              # 独立构建配置
├── VisionController/           # 核心：可复用的视觉模块
│   ├── vmtypes.h               # 类型定义（枚举、结构体、Q_DECLARE_METATYPE）
│   ├── vmbase.h/cpp            # VisionMaster SDK 底层封装
│   ├── vmcontrol.h/cpp         # 模块对外入口（命令分发 + 信号输出）
│   └── VisionController_Usage.md
├── EventBus/                   # 历史遗留（不参与构建）
│   ├── eventbus.h/cpp
│   └── eventTypes.h
└── 视觉方案/
    └── 水瓶识别.sol             # VM 视觉方案文件
```

### 1.2 三层架构

| 层级 | 类名 | 职责 |
|------|------|------|
| **上层调用者** | MainWindow | 发送命令、接收结果（可替换为任意调用者） |
| **模块入口** | VMControl | 命令分发、回调处理、超时管理、数据格式转换 |
| **SDK 封装** | VMbase | 直接调用 VisionMaster SDK，处理回调 |

### 1.3 命令入口（3 个 slot）

VMControl 提供三个统一的命令槽，上层通过信号槽调用：

```cpp
// 1. 方案级命令：Load/Close/Save/SaveAs/Run/Runing/Stop/Switching
void executeSolutionCommand(SolMotionType command, const QVariantMap& params = QVariantMap());

// 2. 流程级命令：Run/Runing/Stop/Set
void executeProcedureCommand(ProcMotionType command, const QString& processName);

// 3. 模块参数页命令：Set/GroupSet
void executeModuleCommand(ModuMotionType command, const QVariantMap& params);
```

### 1.4 输出信号（3 个 signal）

```cpp
// 方案状态变化：NotLoaded → Loaded / FailToLoad
void statusChanged(const VisionStatusEvent &event);

// 数据就绪：流程列表、渲染句柄、点位数据、超时信息
void dataReady(const VisionDataEvent &event);

// 运行日志
void messageReady(const VisionMessageEvent &event);
```

### 1.5 数据类型（vmtypes.h）

**数据类型枚举 VisionDataType：**
- `TargetData` — 多目标数据，pack 中包含 `TargetDataList`
- `PointData` — 单点数据，pack 中包含 `PointData`
- `SolData` — 方案加载后的流程列表
- `RenderData` — 渲染控件句柄
- `SetData` — 参数控件句柄
- `Timeout` — 流程执行超时

**核心数据结构：**
```cpp
struct TargetData {
    int id;       // 目标编号
    int status;   // 目标状态
    double x;     // X 坐标
    double y;     // Y 坐标
    double r;     // 角度
};

struct PointData {
    double x, y, r;  // 单点坐标
};
```

### 1.6 完整数据流

```
1. 加载方案
   MainWindow → emit requestSolutionCommand(Load, {solPath, password})
   → VMControl::loadSolution() → VMbase::loadSol() → SDK LoadSolution()
   → VMControl 发送 statusChanged(Loaded) + dataReady(SolData, 流程列表)

2. 执行流程
   MainWindow → emit requestProcedureCommand(Run, "流程名")
   → VMControl::runProcedure() → VMbase::runPrc() → SDK pVmPrc->Run()
   → VMControl 发送 dataReady(RenderData, 渲染句柄)

3. SDK 回调（异步）
   SDK 执行完成 → VMbase::OnWorkEndStatusCallBack()
   → 读取 results->GetOutputString("out_string") 判断 NG
   → 读取 results->GetOutputString("out_dataStatus") 判断数据类型
   → 读取 results->GetOutputFloat("out_X"/"out_Y") 获取坐标
   → emit processEndCallBack(backData)

4. 数据分发
   VMControl::proEndReceive() 收到回调
   → 按 dataStatus 分发：
     "多组数据" → hanleTargetData() → emit dataReady(TargetData, {processName, TargetDataList})
     "单点数据" → hanlePointData() → emit dataReady(PointData, {processName, PointData})
     其他      → hanleprocData()  → 停止流程 + 停止定时器
```

### 1.7 关键设计细节

- **超时保护**：连续执行流程时启动 5 秒单次定时器，超时后停止流程并发送 Timeout 事件
- **去重机制**：`m_procSuccessFlags` 标记每个流程是否已收到有效数据，防止重复回调
- **自动停止**：收到有效数据后自动调用 `procedure->StopRun()` 停止连续执行
- **NG 过滤**：SDK 回调中如果 `out_string == "NG"` 则直接返回不处理
- **渲染控件**：通过 `GetControlInfo()` 获取句柄，传递给 QAxWidget 显示画面
- **参数页**：通过 `ParamBaseEx` 结构体和 `IMVS_PF_GetModuleIDByFullName` 获取模块参数页句柄

---

## 二、集成方案

### 2.1 核心思路

CS-VMC-Dome 的 `VisionController/` 目录设计为可复用模块（Usage.md 明确说明）。方案：
- **不修改** CS-VMC-Dome 源码
- 在 `src/vision/cs_vmcontrol/` 新建 `VisionMasterBridge` 适配类
- 直接编译 CS-VMC-Dome 的 `vmcontrol.cpp` + `vmbase.cpp` 作为项目的一部分
- 通过信号槽桥接 VMControl 与 MainWindow

### 2.2 VisionMaster SDK 路径（已确认安装）

SDK 安装路径：`C:\Program Files\VisionMaster4.4.0\`

| 用途 | 路径 |
|------|------|
| 头文件 | `Development\V4.x\Includes\` |
| 静态库 (x64) | `Development\V4.x\Libraries\win64\C\iMVS-6000PlatformSDK.lib` |
| 运行时 DLL | `Development\V4.x\Libraries\win64\C\my_cross_platform.dll` |
| QT 控件头文件 | `Development\V4.x\ComControls\Includes\QT\VmControlBaseInfo.h` |

### 2.3 新建文件清单

```
src/vision/cs_vmcontrol/
├── VisionMasterBridge.h      # 适配层头文件
├── VisionMasterBridge.cpp    # 适配层实现
└── CMakeLists.txt            # 构建配置
```

### 2.4 VisionMasterBridge 接口设计

```cpp
class VisionMasterBridge : public QObject {
    Q_OBJECT
public:
    explicit VisionMasterBridge(QObject *parent = nullptr);
    ~VisionMasterBridge();

    // 方案操作
    bool loadSolution(const QString& solPath, const QString& password = "");
    void closeSolution();
    bool isLoaded() const;

    // 流程操作
    void runProcedure(const QString& processName);
    void runProcedureContinuously(const QString& processName);
    void stopProcedure(const QString& processName);

    // 获取流程列表
    QStringList processNames() const;

signals:
    // 状态信号
    void solutionLoaded(bool success, const QString& message);
    void solutionClosed();

    // 数据信号
    void targetDataReady(const QString& processName, const QList<TargetData>& targets);
    void pointDataReady(const QString& processName, const PointData& point);
    void procedureTimeout(const QString& processName);

    // 日志
    void logMessage(const QString& message);

private:
    VMControl* m_vmControl;
    QStringList m_processNames;
    bool m_loaded = false;

    void onStatusChanged(const VisionStatusEvent& event);
    void onDataReady(const VisionDataEvent& event);
    void onMessageReady(const VisionMessageEvent& event);
};
```

### 2.5 "执行验证" 按钮流程

```
用户点击 btnVerifyVision
  ↓
MainWindow::onVerifyVisionClicked()
  ↓
if (!m_vmBridge->isLoaded())
    m_vmBridge->loadSolution("视觉方案/水瓶识别.sol");
else
    m_vmBridge->runProcedure(m_processNames.first());  // 执行第一个流程
  ↓
等待 targetDataReady / pointDataReady 信号
  ↓
收到数据 → 更新 UI / 驱动机械臂运动
```

### 2.6 CMake 集成

在 `src/vision/CMakeLists.txt` 中添加：

```cmake
# VisionMaster SDK 路径
set(VM_SDK_DIR "C:/Program Files/VisionMaster4.4.0/Development/V4.x")
set(VM_INCLUDE_DIR ${VM_SDK_DIR}/Includes)
set(VM_LIB_DIR ${VM_SDK_DIR}/Libraries/win64/C)
set(VM_QT_INCLUDE_DIR ${VM_SDK_DIR}/ComControls/Includes/QT)

# cs_vmcontrol 适配层 + CS-VMC-Dome 源码（不修改原文件）
add_library(cs_vmcontrol STATIC
    cs_vmcontrol/VisionMasterBridge.h
    cs_vmcontrol/VisionMasterBridge.cpp
    # 直接编译 CS-VMC-Dome 的源文件
    ${CMAKE_SOURCE_DIR}/external/CS-VMC-Dome/VisionController/vmcontrol.cpp
    ${CMAKE_SOURCE_DIR}/external/CS-VMC-Dome/VisionController/vmbase.cpp
)

target_include_directories(cs_vmcontrol PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cs_vmcontrol
    ${CMAKE_SOURCE_DIR}/external/CS-VMC-Dome/VisionController
    ${VM_INCLUDE_DIR}
    ${VM_QT_INCLUDE_DIR}
)

target_link_libraries(cs_vmcontrol PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::AxContainer
    ${VM_LIB_DIR}/iMVS-6000PlatformSDK.lib
)

# SDK 头文件为 GBK 编码，需要特殊处理
if(MSVC)
    target_compile_options(cs_vmcontrol PRIVATE /source-charset:gbk)
endif()
```

### 2.7 注意事项

1. **编码问题**：VM SDK 头文件为 GBK 编码，编译 cs_vmcontrol 时需要 `/source-charset:gbk`
2. **AxContainer 依赖**：VMbase 使用 QAxWidget，需要链接 Qt::AxContainer
3. **DLL 部署**：需要将 `my_cross_platform.dll` 复制到构建输出目录
4. **.sol 文件路径**：方案文件路径需要与运行时工作目录匹配
5. **Qt 版本**：CS-VMC-Dome 使用 Qt6，Robot_R06 也使用 Qt6，兼容
6. **注册元类型**：vmtypes.h 中的 Q_DECLARE_METATYPE 需要在 main.cpp 中 qRegisterMetaType
