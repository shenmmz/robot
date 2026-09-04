---
AIGC:
    Label: "1"
    ContentProducer: 001191440300708461136T1XGW3
    ProduceID: 7fd4f4eb60674eb6db7e422f0766d751_38dce37da6ae11f1891f525400f8a581
    ReservedCode1: r1KjnXMOSbqMZyrbRUSRd6lrFil6SxQvlveVdqrCnCbv1iT7jydI7sxYT2j7sHHebHWIMK4BxqKwcU6E4//KLK8AtIoacfapJZofDPDPVN6rn0jq/+2VKUWz7dcq7y+RF7jV8Vi3C2x7vtzUvYiIrgVEVLzssRotQqb7mpCU/9VWRQelCkioXaTaSFQ=
    ContentPropagator: 001191440300708461136T1XGW3
    PropagateID: 7fd4f4eb60674eb6db7e422f0766d751_38dce37da6ae11f1891f525400f8a581
    ReservedCode2: r1KjnXMOSbqMZyrbRUSRd6lrFil6SxQvlveVdqrCnCbv1iT7jydI7sxYT2j7sHHebHWIMK4BxqKwcU6E4//KLK8AtIoacfapJZofDPDPVN6rn0jq/+2VKUWz7dcq7y+RF7jV8Vi3C2x7vtzUvYiIrgVEVLzssRotQqb7mpCU/9VWRQelCkioXaTaSFQ=
---

# Robot_R06 回零链路死代码审查报告

- 审查日期：2026-09-02
- 项目路径：`C:\Users\111\Desktop\Robot_R06`
- 审查目标：确认回零链路是否存在"多余无用代码"，输出死代码清单（不做任何代码修改）
- 审查范围：
  1. `src/logic/HomingController.cpp/.h`（审查主体：未用成员变量/函数、注释掉的死代码、声明无调用的接口、冗余 include、错位注释）
  2. `src/logic/RobotController.cpp/.h`（executeHoming / stopHoming / getHomingController / homingCompleted 信号链）
  3. `mainwindow.cpp`（回零按钮 onHomingClicked 与完成/失败链式回调）
  4. `src/models/RobotModels.cpp/.h`（HomeSpec 各字段消费核查）
  5. `src/models/RobotConfig.cpp/.h`（AxisConfig 各字段消费核查）
- 核实方法：对每个疑似项在全项目（`library/`、`ZmotionAPI/`、`src/`、`main.cpp`、`mainwindow.cpp`，排除 `build/`、`external/`、`参考代码文件/`）grep 真实引用，不轻信注释"已删除/已废弃"
- 特别说明：`参考代码文件/home_manager.cpp/.h` 虽含 `g_axis_hw` / `g_home_strategy` / `HomeManager` 全套旧实现，但根 `CMakeLists.txt` 仅 `add_subdirectory(library|ZmotionAPI|src)`，该目录**不参与编译**，属归档参考，不计入死代码

---

## 一、结论摘要

回零主链（startHoming → step → 6 轴并行回零 → moveAllAxesToPositions）**没有逻辑死代码**：上一轮机型化改造后，旧静态数组 `g_axis_hw`/`g_home_strategy` 已在业务代码物理删除（grep 证实仅剩 `参考代码文件/` 归档与 RobotModels 注释溯源），HomeSpec 全部字段均被状态机消费。

真正问题集中在**接口层残留**：HomingController / RobotController / RobotConfig 上存在一批"声明/定义齐全但从无调用"的公开接口、信号、成员字段与 include/注释残留，共 **14 项死代码/冗余** + **6 项疑似未用待确认**（详见清单）。它们不影响编译与运行，但增加维护面与双数据源漂移风险。

---

## 二、死代码清单

### 2.1 HomingController.cpp / HomingController.h（主体）

| 文件:行号 | 代码 | 判定 | 依据 | 建议 |
|---|---|---|---|---|
| HomingController.h:90；HomingController.cpp:37 | `RobotState* m_state;` / 构造 `m_state(state)` | 死代码（未使用成员变量） | grep `m_state` 仅命中声明与构造初始化 1 处，.cpp 全文再无任何 `m_state->` 消费；无对应 getter 暴露 | 删除成员与构造参数；连带清理构造签名、`#include "../models/RobotState.h"` 及不必要的类型依赖 |
| HomingController.h:57；HomingController.cpp:75-88 | `void bindZmotionAPI(ZmotionAPI* api)` | 死代码（声明无调用的接口） | grep 全项目仅声明+定义两处命中；`m_zmotionAPI` 实际经构造函数注入 | 删除接口与定义 |
| HomingController.h:68-71 | `setHomingTimeoutMs/getHomingTimeoutMs/setPollInterval/getPollInterval`（4 个 inline） | 死代码（声明无调用的接口） | 全项目无任何调用；`m_homingTimeoutMs`、`m_pollIntervalMs` 在构造函数/startHoming 内直接用默认值，超时/轮询间隔无动态调节需求 | 删除 4 个接口；如需可调再暴露 |
| HomingController.h:66 | `const QList<bool>& homedFlags() const` | 死代码（声明无调用的接口） | 全项目无调用；`isAllHomed()` 已覆盖"是否全部回零"的判断职责 | 删除 |
| HomingController.h:65 | `bool isHoming() const { return m_isHoming; }` | 疑似未用待确认 | 全项目无调用方（上层 RobotController/mainwindow 均不查询回零中状态） | 当前无消费；若后续 UI 要显示"回零中"可保留，否则删除 |
| HomingController.h:77-80；cpp emit 点（homingStarted:147；homingProgress:307 等 20+ 处；homingAxisCompleted:386/589；homingAxisFailed:237/306 等 10+ 处） | 信号 `homingStarted / homingProgress / homingAxisCompleted / homingAxisFailed` | 疑似未用待确认（只发不收） | grep connect 全项目仅 HomingController 的 `homingCompleted`(RobotController.cpp:845) 与 `homingFailed`(:873) 有连接；上述 4 个信号只被 emit、无任何 connect 消费者 | 产品无"回零进度/单轴结果"展示需求则删除整组；否则保留供未来 UI 订阅（保留需在头文件注明意图）。若删除，`homingProgress` 各 emit 行可随状态机一并清理，且与 `homingAxisFailed` 在失败分支的重复 emit 问题随之消失 |
| HomingController.cpp:148/190/227 | `// qDebug() << ...`（3 行注释掉的调试日志） | 仅注释残留 | 整行为注释、无执行路径 | 删除 3 行注释；删除后 `.cpp:6 #include <QDebug>` 无其他引用，可一并移除 |
| HomingController.h:25/26 | `#include <QElapsedTimer>` / `#include <QCoreApplication>` | 冗余（无用 include） | grep `.cpp/.h` 全文无 `QElapsedTimer`、`QCoreApplication`（含 `qApp`）符号引用 | 删除两个 include |
| HomingController.h:94 | 孤立注释 `// 仅执行串行预处理（不进入并行回零）` | 仅注释残留（错位） | 该注释孤立悬挂在 `m_serialPrehomeCompleted`(:93) 声明后、无对应代码语句；语义对应旧实现的"仅串行预处理模式"标志，现无此逻辑，且易让读者误以为成员带此含义 | 删除，或若想保留设计说明请移动到函数注释处并改写 |

### 2.2 调用方：RobotController.cpp / RobotController.h

| 文件:行号 | 代码 | 判定 | 依据 | 建议 |
|---|---|---|---|---|
| RobotController.h:86；RobotController.cpp:369-374 | `void stopHoming()` | 疑似未用待确认（无调用接口） | grep 全项目无调用方：mainwindow 回零失败/取消路径与急停路径均未调用（急停走 executeSafeStop，回零自动继续的问题已在前次审查 P0-3 记录） | 建议将急停/安全停止路径接入本方法（与回零状态机关联），接入后即为有效代码；若确认产品不需要"停止回零"能力则删除 |
| RobotController.h:87 | `HomingController* getHomingController() const` | 死代码（声明无调用的接口） | grep 全项目仅声明行命中，无任何调用者 | 删除；后续如需让 UI 直接订阅 HomingController 信号再暴露 |

### 2.3 调用方：mainwindow.cpp

| 文件:行号 | 代码 | 判定 | 依据 | 建议 |
|---|---|---|---|---|
| mainwindow.cpp:274/750-887/1631；mainwindow.h:73/161 | 回零按钮链 `onHomingClicked`、`operationCompleted/Failed("回零")` 链式回调、`m_homingDone` | 全部有效，未发现死代码 | pushButton_4(:274)、btnOpHome(:1631) 均 connect 到 onHomingClicked；executeHoming(:822) 被调用；完成/失败 lambda 均挂在 operationCompleted/Failed 上并被触发；m_homingDone 在 :796/:867/:1809 读写 | 无需改动 |

### 2.4 调用方：RobotModels.h / RobotModels.cpp

| 文件:行号 | 代码 | 判定 | 依据 | 建议 |
|---|---|---|---|---|
| RobotModels.h:23 起 HomeSpec 全字段（homeIn/negIn/datumMode/moveAwayOrigin/moveAwayReverse/homeSpeed/homeAccel/homeDecel/homeLspeed/homeCreep/homeSramp） | 机型 HomeSpec 结构体 | 全部有效，未发现死字段 | grep 每个字段在 HomingController.cpp 均有消费（homeIn:127-131 等；negIn:571-572 等；datumMode:805；moveAwayOrigin:337/495；moveAwayReverse:524；homeSpeed:643/805；homeAccel:649；homeDecel:655；homeLspeed:661；homeCreep:667/805；homeSramp:673） | 无需改动 |
| RobotModels.cpp:40、RobotModels.h:18 注释（"数据来源：原 HomingController.cpp 的 g_axis_hw + g_home_strategy（已删除）"） | 溯源注释 | 属实（非死代码） | grep 证实业务代码中 `g_axis_hw/g_home_strategy/HomeAxisHWConfig/HomeStrategy` 零残留，注释与事实一致 | 保留作数据溯源 |

### 2.5 调用方：RobotConfig.h / RobotConfig.cpp（AxisConfig）

| 文件:行号 | 代码 | 判定 | 依据 | 建议 |
|---|---|---|---|---|
| RobotConfig.h:48；RobotConfig.cpp:98 | `QList<QString> sensors;` | 死代码（无消费字段） | grep 全项目除 `toString()` 打印外**无任何赋值点、无任何业务消费**（恒为空列表）；回零 IO 实际取 HomeSpec 的 homeIn/negIn | 删除字段及 toString 打印行 |
| RobotConfig.h:46；RobotConfig.cpp:105/149 | `double homePosition;`（AxisConfig 内） | 冗余（双数据源副本） | 业务读回零位置走 `getHomePosition()`（返回 `m_homePosition`，源为 RobotModels.homePosition，RobotConfig.cpp:129），不消费 AxisConfig.homePosition；后者仅被 toString(:105) 打印 + initializeFromModel(:149) 填充 | 删除字段与 :149 填充（或保留并注明仅供 toString 调试，避免与 m_homePosition 双份漂移——与历史"双数据源"隐患同模式） |
| RobotConfig.h:49-51；RobotConfig.cpp:99-101/150-152 | `int reductionRatio; QString gearRatio; int pulsePerRev;`（AxisConfig 内） | 冗余（双数据源副本） | 真实运动学换算消费的是 AxisSpec 同名字段（RobotModels.cpp:182-183 `a.pulsePerRev*a.reductionRatio…`，a 为 AxisSpec）；AxisConfig 三字段仅 toString 打印 + initializeFromModel 填充，无业务读 | 删除三字段与 :150-152 填充，或保留仅作调试视图（需注明，防止与 AxisSpec 漂移） |
| RobotConfig.h:67 | `QList<AxisConfig> getAllAxisConfigs() const` | 死代码（声明无调用的接口） | grep 全项目仅声明行命中 | 删除 |
| RobotConfig.h:80；RobotConfig.cpp:34-41 | `void setAxisConfig(int, const AxisConfig&)` | 死代码（声明无调用的接口） | grep 全项目仅声明+定义命中，无任何调用；配置现统一由 initializeFromModel 从机型注册表填充 | 删除 |
| RobotConfig.h:87；RobotConfig.cpp:89-111 | `QString toString() const` | 疑似未用待确认（无调用） | grep 全项目无外部调用点（仅定义/声明命中）；注释标注"调试用" | 确认无人依赖调试输出后可删除；若保留，上述 AxisConfig 冗余字段的打印可随之保留（toString 是它们唯一消费者） |
| RobotConfig.h:42 | `int axisIndex;`（AxisConfig 内） | 冗余（自描述字段） | 除 toString 打印外无业务消费；getAxisConfig(i) 本身按下标返回，无需读该字段 | 可与 toString 一并保留或删除（低优先，随 toString 处置） |

---

## 三、建议删除清单汇总

按"删除后不影响编译与行为"的可执行度排序：

**A 组：高置信死代码（全项目无引用，可直接删除）**

| 文件 | 删除项 |
|---|---|
| HomingController.h:90 / .cpp:37 | `m_state` 成员 + 构造参数（连带 `RobotState.h` include） |
| HomingController.h:57 / .cpp:75-88 | `bindZmotionAPI()` |
| HomingController.h:68-71 | `set/getHomingTimeoutMs`、`set/getPollInterval` 4 个接口 |
| HomingController.h:66 | `homedFlags()` |
| HomingController.cpp:148/190/227 | 3 行注释掉的 qDebug |
| HomingController.cpp:6 | `#include <QDebug>`（注释删除后冗余） |
| HomingController.h:25/26 | `QElapsedTimer` / `QCoreApplication` 两个 include |
| HomingController.h:94 | 错位孤立注释 |
| RobotController.h:87 | `getHomingController()` |
| RobotConfig.h:67 | `getAllAxisConfigs()` |
| RobotConfig.h:80 / .cpp:34-41 | `setAxisConfig()` |
| RobotConfig.h:48 / .cpp:98 | `AxisConfig::sensors` 字段及打印 |
| RobotConfig.cpp:149-152 | AxisConfig 冗余四字段（homePosition/reductionRatio/gearRatio/pulsePerRev）的 initializeFromModel 填充（若保留字段调试则跳过） |

**B 组：疑似未用待确认（删除前需产品决策）**

| 文件 | 项 | 决策点 |
|---|---|---|
| HomingController.h:65 | `isHoming()` | UI 是否展示"回零中"状态 |
| HomingController.h:77-80 | 4 个进度/单轴信号 | 产品是否要回零进度展示；不要则随其 emit 行整体删除 |
| RobotController.h:86 / .cpp:369-374 | `stopHoming()` | 建议改为接入急停/停止路径（功能需求，见前次审查 P0-3），否则删除 |
| RobotConfig.h:87 / .cpp:89-111 | `toString()` 及 AxisConfig 调试字段 | 确认无调试依赖后整体删除 |

**C 组：顺带发现（超出回零主链，仅提示，未列入主清单）**

- `RobotConfig.h:77 setRobotName()`：grep 全项目无调用，死接口；同文件 `getRobotName()`（:57）也无外部消费。与回零无关，属配置模型层整体清理范畴，建议另开任务处理。

---

## 四、排查过、确认非死代码（供复核对照）

- `HomingController::reset()`（.h:59/.cpp:90）：有内部调用（startHoming .cpp:136），非死代码。
- `HomingController::stopHoming()`（.h:61/.cpp:157）：析构（.cpp:65）与 RobotController::stopHoming（.cpp:372）调用，非死代码。
- `HomingController::moveAllAxesToPositions()`：RobotController.cpp:856 调用，回零完成后移动到原点位，有效。
- `HomingController::step() / isAllHomed() / onPollTimeout()`：状态机内部互相调用，有效。
- `axisName` / `kMoveTimeoutMs`（.cpp 匿名 static）：step/onPollTimeout/moveAllAxesToPositions 使用，有效。
- `m_serialPrehomeCompleted / m_j2MovedAway / m_j3MovedAway / m_j5MovedAway`：串行预处理放行条件，均被 step/stepSerial 消费，有效。
- `RobotConfig::getAxisConfig`（RobotController.cpp:440-446 消费）、`getHomeMoveSpeed/Accel/Decel`（RobotController.cpp:853-855 消费）、`getJointAxisList/getVirtualAxisList/getAxisCount`（RobotController/MotionController 广泛消费）：有效。
- `mainwindow.cpp` 回零链：全部有消费（见 2.3）。
- `参考代码文件/home_manager.cpp/.h`：不参与编译（CMake 未引用），归档参考，不判死代码。
*（内容由AI生成，仅供参考）*
