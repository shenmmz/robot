---
AIGC:
    Label: "1"
    ContentProducer: 001191440300708461136T1XGW3
    ProduceID: 7fd4f4eb60674eb6db7e422f0766d751_102e13b3a6a511f199d2525400287e28
    ReservedCode1: 2PlR7all29ncm3k50NtKYWExv4Cc3EJ6rXLYyLgMCpxPKxY3F5lMIlyqjfwWJWwZxNYz6E2ksZqh+J0wYDX+FrpvIjnzarPjvl1ql1Lg6SVp07uim/iwSG3DMB70GVXFfFOJ+4cj3lTJxpghhY5RLL7CwXe6vZn/Ha/2XArhPo9pgPUrFKYqcmTOF0A=
    ContentPropagator: 001191440300708461136T1XGW3
    PropagateID: 7fd4f4eb60674eb6db7e422f0766d751_102e13b3a6a511f199d2525400287e28
    ReservedCode2: 2PlR7all29ncm3k50NtKYWExv4Cc3EJ6rXLYyLgMCpxPKxY3F5lMIlyqjfwWJWwZxNYz6E2ksZqh+J0wYDX+FrpvIjnzarPjvl1ql1Lg6SVp07uim/iwSG3DMB70GVXFfFOJ+4cj3lTJxpghhY5RLL7CwXe6vZn/Ha/2XArhPo9pgPUrFKYqcmTOF0A=
---

# Robot_R06 R02 回零修改方案（讨论稿）

> 日期：2026-09-02
> 范围：R02 机型回原点（回零）异常修复 —— HomingController 机型化改造 + RobotModels R02 参数补全
> 状态：讨论稿，未落盘源码改动

---

## 一、问题定位回顾

R02 回零异常不是单点 bug，是三条独立缺陷叠加：

| # | 缺陷 | 位置 | 后果 |
|---|------|------|------|
| A | 运行时从不按 Flash 识别结果重载机型，全程按 R01 跑 | mainwindow.cpp:111 / 475-488 | R02 整机参数（脉冲当量、回零速度）全是 R01 的 |
| B | HomingController 回零 IO/策略只读顶部 static 数组（R01 值），机型化未落地 | HomingController.cpp:19-50 + 8 个函数消费点 | 即使补全 R02 HomeSpec 也不生效 |
| C | buildR02 的 HomeSpec 全部留默认（homeIn=-1），无可下发回零参数 | RobotModels.cpp:86-102 | R02 自身无回零配置 |

**关键事实（已核代码）**：
- `RobotConfig::getAxisSpec(i)`（RobotConfig.cpp:146-151）已实现为"实时转发 `RobotModelRegistry::get(m_modelId).axes.at(i)`，不缓存，机型切换自动跟随"——基础设施已就绪，只差 HomingController 去用它。
- HomingController.cpp 中 `g_axis_hw[]`/`g_home_strategy[]` 内容与 buildR01 的 HomeSpec 逐字节一致（双数据源漂移）。
- `g_axis_hw[].axis` 恒等于数组下标 i；`atype` 恒为 1；`dir` 从未被消费（死代码）。
- RobotModels.cpp:40 与 RobotModels.h:18 注释称 static 数组"已删除"，实际从未删除——注释误导。

---

## 二、目标架构

**单一数据源**：回零参数只存于机型注册表（RobotModels.cpp 的 buildR01/buildR02）→ `RobotConfig`（按机型 initializeFromModel）→ HomingController 运行时实时读取。

```
buildR01 / buildR02 (HomeSpec 每轴唯一)
        │
        ▼
RobotModelRegistry::get(m_modelId)
        │
        ▼
RobotConfig::getAxisSpec(i).home   ← HomingController 所有消费点（8 个函数）
```

新增机型只需在注册表加一个 buildRxx，回零代码零改动。

---

## 三、修改方案分块

### 改动 1：HomingController.cpp 数据源替换（核心，必做）

1. 删除文件顶部 `g_axis_hw[]` / `g_home_strategy[]` 两个 static 数组（第 19-50 行，含注释）。
2. 新增 include：`#include "../models/RobotModels.h"`（RobotConfig.h 只有 AxisSpec 前向声明，访问 .home 需要完整定义）。
3. 每个消费函数首行替换为 `const HomeSpec& hm = m_config->getAxisSpec(i).home;`
4. 函数体内字段引用按映射替换（见下表）。

**涉及 8 个函数**：initAxisParams / checkAxisSensors / isAxisIdle / moveAxisRelative / startAxisDatum / processPrehomeAxis / stepParallel / moveAllAxesToPositions

**字段映射表**：

| 原写法（static 数组） | 替换为 |
|---|---|
| `hw.axis` | `i`（恒等） |
| `hw.atype` | `1`（恒等常量） |
| `hw.homeIn` | `hm.homeIn` |
| `hw.negIn` | `hm.negIn` |
| `st.datumMode` | `hm.datumMode` |
| `st.moveAwayOrigin` | `hm.moveAwayOrigin` |
| `st.moveAwayReverse` | `hm.moveAwayReverse` |
| `st.homeSpeed` | `hm.homeSpeed` |
| `st.homeAccel` | `hm.homeAccel` |
| `st.homeDecel` | `hm.homeDecel` |
| `st.homeLspeed` | `hm.homeLspeed` |
| `st.homeSramp` | `hm.homeSramp` |
| `st.homeCreep` | `hm.homeCreep` |

> 注：R01 数值在迁移前后不变（static 数组值 = R01 HomeSpec 值），R01 行为等价，作为回归基准。

### 改动 2：startHoming() 增加机型参数预检（安全护栏）

R02 当前 homeIn 全为 -1，改造后若直接跑，`setAxisDatumOrigin({i},{-1})` / `getDI(-1)` 是危险调用。在 `startHoming()` 的 reset() 之后加：

```cpp
for (int i = 0; i < kAxisCount; ++i) {
    const HomeSpec& hm = m_config->getAxisSpec(i).home;
    if (hm.homeIn < 0) {
        emit homingFailed(QStringLiteral("当前机型回零参数未配置（轴%1 homeIn=-1），请在 RobotModels.cpp 补全 HomeSpec").arg(i));
        return false;
    }
}
```

> 作用：参数没补全前 R02 点回零会明确报错，而不是按默认值乱跑撞限位。

### 改动 3：RobotModels.cpp buildR02 补全 HomeSpec（参数待定）

现结构（RobotModels.cpp:86-102）只有 3 个机械字段。需按 R01 模板为 6 轴补全：

```cpp
HomeSpec h0;
h0.homeIn = /* R02 J0 原点DI */;   h0.negIn = /* 反向限位DI，无则-1 */;
h0.datumMode = /* 回零模式 */;
h0.moveAwayOrigin = /* 移开量(°) */;   h0.moveAwayReverse = /* 反向回退量(°) */;
h0.homeSpeed = /* (°/s) */;  h0.homeAccel = ...;  h0.homeDecel = ...;
h0.homeSramp = 200;  h0.homeCreep = ...;
// ... h1~h5 同结构

s.axes = QList<AxisSpec>{
    {10000, 28, QStringLiteral("1:4"), h0},  // J0
    {10000, 28, QStringLiteral("1:4"), h1},  // J1
    {10000, 28, QStringLiteral("1:1"), h2},  // J2
    {10000, 28, QStringLiteral("1:1"), h3},  // J3
    {10000, 28, QStringLiteral("1:1"), h4},  // J4
    {10000, 28, QStringLiteral("1:1"), h5},  // J5
};
```

### 改动 4：mainwindow 机型重载（前提条件，必做）

mainwindow.cpp:475-488 Flash 识别到机型 readId 后，当前只更新 UI 变量/标签，未重建配置。需在识别后（或启动初始化时改为先读 Flash）调用：

```cpp
if (readId != m_robotController->getCurrentModelId())  // 以实际接口为准
    m_robotController->initialize(readId);
```

> 不做这步，HomingController 改读 m_config 后 R02 仍套 R01 配置，全部白改。

### 改动 5：注释与冗余清理

1. RobotModels.cpp:40、RobotModels.h:18 的"已删除"误导注释更正为"HomingController 经 RobotConfig 实时读取机型注册表，唯一数据源"。
2. HomingController.h 删除已无消费方的 `HomeAxisHWConfig` / `HomeStrategy` 结构体。
3. buildR02 注释"R02 实装时补全"在补全后同步删除/更新。

---

## 四、需要用户确认/提供的 R02 参数

补全 buildR02 之前，需要 R02 每轴的真实硬件参数（与 R01 未必相同）：

| 参数 | 说明 | 涉及轴 |
|---|---|---|
| homeIn | 原点传感器接的 DI 编号 | 6 轴 |
| negIn | 反向限位 DI 编号（无则 -1） | 6 轴 |
| datumMode | 找零模式（对照 ZMC 手册 3/4 语义） | 6 轴 |
| moveAwayOrigin | 压住原点时的移开量 (°) | 6 轴 |
| moveAwayReverse | 反向限位触发时的回退量 (°) | 有 negIn 的轴 |
| homeSpeed / Accel / Decel / Sramp / Creep | 回零运动参数 | 6 轴 |

- 若 R02 与 R01 传感器接线/机构完全一致，可先复制 R01 值联调；
- 若不一致，必须按实测填写，否则方向/IO 错误会撞限位。

---

## 五、验证步骤

1. **编译**：改动后先编译通过（Qt Creator Release，MSVC）。
2. **R01 回归**（关键）：R01 参数值迁移前后不变，回零行为应完全一致——验证数据源替换无副作用。
3. **R02 未补参数时**：点回零应弹出"回零参数未配置"明确报错（改动 2 护栏生效）。
4. **R02 补参数后**：
   - 启动确认控制台打印的是 R02 识别/配置信息（改动 4 生效）；
   - 手动点动小角度，确认脉冲当量换算正确（R02 应为 10000×28×4/360 ≈ 3111.1 pulse/°）；
   - 执行回零，逐轴观察：移开方向 → 原点传感器触发 → 找零停稳 → 坐标归零。

---

## 六、待讨论问题（本次讨论重点）

1. **R02 回零参数**：你是否已有每轴 DI/方向/速度实测值？没有的话是否先用 R01 值复制试机，还是先只改代码链路、参数等实测后再填？
2. **改动范围**：本次只做"机型化数据源 + 参数补全"，还是顺带修之前审查报告中的 P0（失败/超时不停轴、假成功、急停不回零联动）？建议先只做数据源改造（风险最小、可回归），P0 另行一轮。
3. **dir 字段处理**：HomeAxisHWConfig.dir 从未消费，机型化后方向完全依赖 moveAwayOrigin/Reverse 数值符号 + 硬件接线。是否接受删除，还是要在 AxisSpec/HomeSpec 中显式补方向字段？
4. **homeIn<0 的语义**：HomeSpec 注释写"homeIn=-1 未配置跳过原点检测"，但原实现没有跳过逻辑。预检护栏采用"直接报错拒绝回零"，是否接受？还是希望实现"跳过该轴"语义？
5. **mainwindow 重载触发时机**：启动时先读 Flash 再 initialize，还是启动默认 R01、识别后异步重载？（涉及连接/建模时序，需确认 RobotController.initialize 的调用时机与副作用）
6. **回零后移动目标**：buildSharedDefaults 的 homePosition/homeMoveSpeed 两机型共享同一值，R02 是否需要独立回零后目标位姿？
*（内容由AI生成，仅供参考）*
