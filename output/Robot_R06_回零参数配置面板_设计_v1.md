---
AIGC:
    Label: "1"
    ContentProducer: 001191440300708461136T1XGW3
    ProduceID: 7fd4f4eb60674eb6db7e422f0766d751_c84aa238a6b511f1891f525400f8a581
    ReservedCode1: RcgFzCJR7bxtzRp20hldxktix3JjjH1JUv+2+LNwuH4mEPJSulvSmm6XyMaGLdyeqCYEpQt9Nu03I/qqXqPEszgqiboGI3kelkzK0ttQTpnUYTekYoKbCfsTQjIP3rucXPryrt/6ATGZoN+zwhnjHiji2VjwdF8N5MelXhbpgCAcUyK6jW4Qzq/o4yY=
    ContentPropagator: 001191440300708461136T1XGW3
    PropagateID: 7fd4f4eb60674eb6db7e422f0766d751_c84aa238a6b511f1891f525400f8a581
    ReservedCode2: RcgFzCJR7bxtzRp20hldxktix3JjjH1JUv+2+LNwuH4mEPJSulvSmm6XyMaGLdyeqCYEpQt9Nu03I/qqXqPEszgqiboGI3kelkzK0ttQTpnUYTekYoKbCfsTQjIP3rucXPryrt/6ATGZoN+zwhnjHiji2VjwdF8N5MelXhbpgCAcUyK6jW4Qzq/o4yY=
---



# Robot_R06 回零参数配置面板 — 设计方案 v1

日期：2026-09-02
状态：待评审

## 1. 目标

为上位机新增"回零参数"配置面板（按机型 R01/R02、按轴 0~5），运行时填写 6 轴 × 10 个回零字段并保存；回零执行时优先读取界面保存的运行时值，未配置的轴回退到代码注册表默认值。后续调参不再改代码重编译。

## 2. 现状盘点（已核对源码）

### 2.1 数据流

```
HomingController.cpp  (7 处读取)
  m_config->getAxisSpec(i).home
        │  RobotConfig::getAxisSpec(int)   —— 实时转发，不缓存（RobotConfig.cpp:117）
        ▼
  RobotModelRegistry::get(m_modelId).axes.at(i)
        │  RobotModels.cpp buildR01()/buildR02() —— 编译期静态单例，无 setter
        ▼
  代码内硬编码的 HomeSpec（R02 目前为 R01 同款占位）
```

关键结论：**HomeSpec 目前没有运行时写入通道**。`RobotConfig` 只搬运了 speed/accel/decel/减速比等字段，`home` 只通过 `getAxisSpec()` 转发只读注册表。HomingController 所有读点都走 `m_config->getAxisSpec(i).home`（第 113/279/431/605/703/778 行等共 7 处）。

### 2.2 UI 现状

- 主窗口 `MainWindow`，导航页 `contentStack`，设置页 = 索引 4（`btnNavSettings`），`.ui` 内 page 名为 `pageSettings`（mainwindow.ui:5531），内含"回零位置 + btnSaveHome"等板块。
- `UiController` 负责设置页逻辑：全部用 `m_mainWindow->findChild<...>("控件名")` 查找，不依赖 ui_mainwindow.h（UiController.cpp:5），`init()` 里绑定信号；`onLoadSettings`(3660) / `onSaveSettings`(3681) 已存在，操作 `RobotConfig` 的 homePosition/homeMove* 字段。
- 设置页保存按钮 `btnSaveHome` 已连接 `onSaveSettings`。

### 2.3 机型切换

连接后从控制器 Flash 读机型 ID → `RobotController::applyModel(readId)` → `RobotConfig` 更新 `m_modelId`。`getAxisSpec` 实时跟随注册表。机型切换不重建 `HomingController`/`RobotConfig` 对象（`initializeFromModel` 只更新字段）。

### 2.4 持久化现状

- 上位机配置：`config/` 目录（`MainWindow::findConfigDir()` 向上查找 exe 的 config 目录，mainwindow.cpp:75），账号配置用 `QSettings::IniFormat`。
- 控制器 Flash：只存机型 ID（块 100, 4 float）与原点位置；不存回零参数。

## 3. 总体设计

三层，均为增量改动：

```
┌─────────────────────────────────────────────┐
│ UI 层（新增）                               │
│  pageSettings 内新增 "回零参数" GroupBox     │
│  机型下拉 + 6轴×10字段表格 + 按钮区          │
│  （代码构建，走 UiController findChild 风格） │
└──────────────┬──────────────────────────────┘
               │ 保存/载入/恢复默认
┌──────────────▼──────────────────────────────┐
│ 数据层（RobotConfig 扩展）                  │
│  m_homeOverrides: (modelId, axis) → HomeSpec│
│  homeSpecFor(axis)：覆盖优先，否则注册表     │
│  load/saveHomeOverrides(Ini)                │
└──────────────┬──────────────────────────────┘
               │ 回零时实时查询
┌──────────────▼──────────────────────────────┐
│ 消费层（HomingController 7 处替换）         │
│  m_config->getAxisSpec(i).home              │
│      →  m_config->homeSpecFor(i)            │
└─────────────────────────────────────────────┘
```

- 代码注册表（RobotModels.cpp）保留为**默认兜底**，语义不变。
- 运行时覆盖表以 `(modelId, axisIndex)` 为键，**各机型独立**，机型切换互不污染。
- 持久化用上位机 `config/robot_home.ini`（QSettings IniFormat，见决策 D1）。

## 4. 数据层改动

### 4.1 RobotConfig 新增 API（RobotConfig.h/.cpp）

```cpp
// 运行时回零参数覆盖：覆盖优先于机型注册表默认
const HomeSpec& homeSpecFor(int axisIndex) const;      // 回零消费点统一入口
bool hasHomeOverride(RobotModelId id, int axisIndex) const;
void setHomeOverride(RobotModelId id, int axisIndex, const HomeSpec& h);
bool clearHomeOverride(RobotModelId id, int axisIndex);
bool clearAllHomeOverrides(RobotModelId id);           // "恢复默认"用
bool hasAnyHomeOverrides() const;                      // 面板状态显示用

bool loadHomeOverrides(const QString& iniPath);        // 启动时调用
bool saveHomeOverrides(const QString& iniPath) const;
```

头文件依赖：`HomeSpec` 目前定义在 `RobotModels.h`；`RobotConfig.h` 只前置声明 `AxisSpec`。需要把 `RobotConfig.h` 增加 `#include "src/models/RobotModels.h"`（或把 HomeSpec 拆到独立头，实现时二选一，倾向直接 include，改动最小）。

### 4.2 ini 键布局（config/robot_home.ini）

```ini
[HomeOverride/R01/axis0]
homeIn=0
negIn=1
datumMode=3
moveAwayOrigin=-10
moveAwayReverse=-155
homeSpeed=40
homeCreep=1
homeAccel=80
homeDecel=100
homeSramp=200
; ... axis1 ~ axis5
[HomeOverride/R02/axis0]
...
```

字段名与 `HomeSpec` 成员一一对应，便于读写与排障。

### 4.3 生效时机

- **保存**：写入内存覆盖表 + 写 ini，下次回零立即生效（HomingController 每次点击回零时实时查 `homeSpecFor`），无需重启、无需重新建模。
- **启动**：MainWindow/UiController 初始化时 `loadHomeOverrides(findConfigDir()+"/robot_home.ini")`。
- **恢复默认**：清该机型全部覆盖并删除/置空 ini 对应组 → 回退注册表默认。

## 5. 消费层改动（HomingController）

替换 HomingController.cpp 中 7 处 `m_config->getAxisSpec(i).home` 为 `m_config->homeSpecFor(i)`：

| 行号 | 用途 | 替换注意 |
|------|------|---------|
| 113 | 回零预检 homeIn<0 | 值比较，直接换 |
| 279 | stepSerial 传感器检查/移开 | 局部 `const HomeSpec&` 改值语义 |
| 431 | stepParallel 同左 | 同上 |
| 605 | initAxisParams 运动参数 | 同上 |
| 703 | checkAxisSensors | 同上 |
| 778 | startAxisDatum | 同上 |

注意：`getAxisSpec()` 仍被其它模块使用（建模/单位/限速），**不要改其返回语义**；只有回零读点切到覆盖优先入口。覆盖表在 RobotConfig 内以 `QHash<quint64, HomeSpec>` 存值，`homeSpecFor` 返回引用前需确保覆盖值可稳定寻址（用 QHash 值节点或拷贝返回，实现时定为返回 const 引用 + 无覆盖时返回注册表静态引用）。

## 6. UI 层改动

### 6.1 布局（UiController 代码构建，注入 pageSettings）

在 `pageSettings` 现有内容下方新增 `QGroupBox "回零参数（机型化，运行时覆盖）"`：

```
[机型: R01 ▾]  [载入已保存] [保存到配置] [恢复代码默认]
┌──────┬─────┬──────┬───────┬──────┬───────┬────┬────┬────┬────┬─────┐
│ 轴   │homeIn│negIn│datumMode│moveAwayOrigin│moveAwayReverse│homeSpeed│homeCreep│homeAccel│homeDecel│homeSramp│
├──────┼─────┼──────┼───────┼──────┼───────┼────┼────┼────┼────┼─────┤
│ J0   │ 0   │  1  │   3   │  -10 │  -155 │ 40 │ 1  │ 80 │ 100│ 200 │
│ ...  │ ... │     │       │      │       │    │    │    │    │     │
│ J5   │ ... │     │       │      │       │    │    │    │    │     │
└──────┴─────┴──────┴───────┴──────┴───────┴────┴────┴────┴────┴─────┘
[未保存修改提示 *]  [回零执行时提示：正在使用界面保存值/代码默认值]
```

- 控件类型：`homeIn/negIn/datumMode` 用 `QSpinBox`；速度/距离类用 `QDoubleSpinBox`（小数位 1~3，范围放宽到 ±10000）。
- 行为：
  - 机型下拉变化 → 从 `RobotModelRegistry::get(id)` 填充表格为"代码默认值"，再叠加 ini 已保存的覆盖值（覆盖值格以高亮底色标识）。
  - "保存到配置" → 收集表格 → 校验（`datumMode ∈ {3,4}`、保存前提示"轴 N homeIn=-1 将跳过原点检测"）→ `setHomeOverride` + `saveHomeOverrides`。
  - "载入已保存" → 重读 ini 覆盖回填。
  - "恢复代码默认" → `clearAllHomeOverrides(id)` + ini 清组 + 表格回填注册表默认。
  - 面板不依赖连接状态（断连也可编辑，保存仅写上位机 ini）；但若已连接且轴在运动，保存前提示先停止（与现有 btnSaveHome 同风格）。

### 6.2 与现有代码的关系

- `UiController.h/.cpp`：新增 `buildHomeParamPanel()`、`onLoadHomeOverrides()`、`onSaveHomeOverrides()`、`onResetHomeOverrides()`；`init()` 中调用构建与绑定。
- 不动 `onLoadSettings/onSaveSettings` 现有逻辑（它们管 homePosition/homeMove*，与本文的每轴 HomeSpec 是两回事）。
- 面板默认数据来自 `RobotModelRegistry::get(当前下拉机型)`。

## 7. 字段对照与默认值

### 7.1 makeHome 形参与 HomeSpec 成员对照

你贴的 makeHome 形参顺序：`homeIn, negIn, datumMode, awayOrigin, awayReverse, speed, creep, accel, decel, sramp`。对应 HomeSpec 成员：

| # | makeHome 形参 | HomeSpec 成员 | 类型 |
|---|--------------|---------------|------|
| 1 | homeIn | homeIn | int |
| 2 | negIn | negIn | int |
| 3 | datumMode | datumMode | int |
| 4 | awayOrigin | moveAwayOrigin | double |
| 5 | awayReverse | moveAwayReverse | double |
| 6 | speed | homeSpeed | double |
| 7 | creep | homeCreep | double |
| 8 | accel | homeAccel | double |
| 9 | decel | homeDecel | double |
| 10 | sramp | homeSramp | double |

### 7.2 ⚠️ 需要你确认：这组数据与现有占位完全一致

我按上述映射展开你给的 6 行，并与 `buildR02()` 当前代码（R01 同款占位）逐字段比对：

| 轴 | homeIn | negIn | datumMode | awayOrigin | awayReverse | speed | creep | accel | decel | sramp | 与 R02 现占位 |
|----|-------|-------|-----------|------------|-------------|-------|-------|-------|-------|-------|--------------|
| J0 | 0 | 1 | 3 | -10 | -155 | 40 | 1 | 80 | 100 | 200 | 完全一致 |
| J1 | 2 | -1 | 4 | 35 | 0 | 40 | 1 | 80 | 200 | 200 | 完全一致 |
| J2 | 3 | -1 | 3 | -35 | 0 | 20 | 1 | 50 | 100 | 200 | 完全一致 |
| J3 | 4 | 5 | 3 | -10 | -200 | 40 | 1 | 80 | 100 | 200 | 完全一致 |
| J4 | 6 | -1 | 4 | 20 | 0 | 40 | 1 | 80 | 100 | 200 | 完全一致 |
| J5 | 7 | 8 | 3 | -10 | -220 | 60 | 2 | 100 | 200 | 200 | 完全一致 |

即：你贴的 R02 数据 = `buildR02()` 当前占位值 = R01 同款回零 IO/参数（R01/R02 的差异目前只在减速比 50/30/10 vs 28）。如果 R02 实机确实沿用同一套原点/限位 IO 与速度，则代码默认无需改动，只需做"界面 + 运行时覆盖"落地，把这组值作为两机型共用默认展示；如果 R02 实测另有数值，请替换后再进入实现。

## 8. 待决策

| # | 决策点 | 选项 | 建议 |
|---|--------|------|------|
| D1 | 持久化载体 | A. 上位机 config/robot_home.ini；B. 控制器 Flash（需设计 60 float 块布局与读写协议） | A（v1 范围小、与现有 config 目录惯例一致；B 留作后续"参数下发控制器"扩展） |
| D2 | UI 宿主 | A. 设置页 pageSettings 内加分组（推荐）；B. 独立新导航页 | A |
| D3 | 登录门控 | 面板是否受开发者账号登录限制（现账号管理独立） | 跟随现有 btnSaveHome 行为，暂不设门控 |
| D4 | buildR02 同步 | 是否把你贴的 6 行作为"R02 实装确认值"写回 buildR02 并移除"占位待填"注释 | 建议做（与 D1 不冲突：注册表 = 出厂默认，ini = 现场覆盖） |

## 9. 实施步骤与文件清单

阶段 P0 数据层：
- `src/models/RobotConfig.h/.cpp`：include RobotModels.h、新增 override 表 + API + ini load/save。
- （可选 D4）`src/models/RobotModels.cpp`：buildR02 注释与数值落定。

阶段 P1 消费层：
- `src/logic/HomingController.cpp`：7 处读点替换为 `homeSpecFor(i)`。

阶段 P2 UI 层：
- `src/logic/UiController.h/.cpp`：面板构建 + 槽函数 + init 绑定。
- （宿主页已有，无需改 .ui；如 D2 选独立页再议）

阶段 P3 验证：
- VS2026 DevShell 增量编译（cmake --build，遇 LNK1168 先结束调试版进程）。
- 断连状态功能自测：默认值来自注册表 → 改值保存 → 重启载入 → 恢复默认。
- 真机回零对照：R01 用保存值回零与代码默认行为一致；R02 装新臂后按实测调。

## 10. 风险与注意

1. `RobotConfig.h` include `RobotModels.h` 增加耦合，但两文件同属 models 层，可接受。
2. `homeSpecFor` 引用生命周期：覆盖值存在 QHash 内，注册表默认值存在静态单例内，均稳定；不要在覆盖删除后持引用跨调用。
3. HomingController 其余 `getAxisSpec()` 用法（若有）不动，避免影响建模/单位换算。
4. 保存不校验控制器连接（v1 上位机侧配置）；若后续要"一键下发到 Flash"，按 D1 扩展，不回退本设计。
*（内容由AI生成，仅供参考）*
*（内容由AI生成，仅供参考）*
