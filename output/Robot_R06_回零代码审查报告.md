---
AIGC:
    Label: "1"
    ContentProducer: 001191440300708461136T1XGW3
    ProduceID: 7fd4f4eb60674eb6db7e422f0766d751_35c357d2a69f11f1891f525400f8a581
    ReservedCode1: a5QchsONtpHhieK/vctTnLUiNX1dW7rwSMDo7EavY+Odjztg1LKArO4O9NPzxtM8QK2Qo4CTPLBozrAouIgbH8F1SRcm0thu9Z5ViKCIb84PUWC5UJIMrI2xzF/6XDReul0NBWt7dOwsVHNG6uiKP2Q3+eL4A8WCFY0F+FeAdp+Tu6lxISJ+hcmpYKs=
    ContentPropagator: 001191440300708461136T1XGW3
    PropagateID: 7fd4f4eb60674eb6db7e422f0766d751_35c357d2a69f11f1891f525400f8a581
    ReservedCode2: a5QchsONtpHhieK/vctTnLUiNX1dW7rwSMDo7EavY+Odjztg1LKArO4O9NPzxtM8QK2Qo4CTPLBozrAouIgbH8F1SRcm0thu9Z5ViKCIb84PUWC5UJIMrI2xzF/6XDReul0NBWt7dOwsVHNG6uiKP2Q3+eL4A8WCFY0F+FeAdp+Tu6lxISJ+hcmpYKs=
---



# Robot_R06 回原点（回零）代码审查报告

- 审查日期：2026-09-02
- 项目路径：`C:\Users\111\Desktop\Robot_R06`
- 审查范围：
  1. `src/logic/HomingController.cpp/.h`（回零状态机：串行预处理 J3→J2→J5 移开 + 6 轴并行回零）
  2. `src/logic/RobotController.cpp`（executeHoming / stopHoming / executeSafeStop / stopMotion / homingCompleted 链路）
  3. `src/models/RobotModels.cpp/.h`、`src/models/RobotConfig.cpp/.h`（HomeSpec / RobotModelRegistry 回零规格）
  4. `src/hardware/` 相关 ZmotionAPI 调用（setAxisSingleDatum / getAxisSingleHomeStatus / getDI / getIfIdle2 / setInvertIn / clearAxisPosition / waitForAxesIdle）
  5. `mainwindow.cpp`（onHomingClicked 一键链路 / onBtnOpEStop / onBtnStop / 急停与原点设置）

---

## 一、正常回零链路简述（供对照）

1. `MainWindow::onHomingClicked()`（mainwindow.cpp:734）确认后调 `RobotController::executeHoming()`（RobotController.cpp:322）→ `HomingController::startHoming()`（HomingController.cpp:158）：`rapidStop` → 启动 200ms 轮询定时器。
2. `step()`（HomingController.cpp:213）先 `stepSerial()`：依次对索引 2(J2/机械臂第3轴)→1(J1/机械臂第2轴)→4(J4/机械臂第5轴) 做"参数初始化→查传感器→移开原点→单轴 DATUM"；三个轴都 `movedAway` 后进入 `stepParallel()`：6 轴并行"查传感器(含反向限位)→移开→DATUM→轮询 homeStatus→成功/超时"。
3. 全部轴 HOME_SUCCESS → `onPollTimeout`（HomingController.cpp:254）`emit homingCompleted()` → `RobotController.cpp:821` lambda：`getOriginPosition()`（Flash 优先，config 兜底）→ `moveAllAxesToPositions()` 绝对移动到原点位 → `waitForAxesIdle(15s)` → `clearAxisPosition()` 清 DPOS/MPOS → `emit operationCompleted("回零")`。
4. mainwindow.cpp conn1 收到 `operationCompleted("回零")` 后依次：`initRobotModel()` → `setAxisDpos(全轴清0)` → `switchToJointMode()` → `switchToCartesianMode()` → `switchToJointMode()` → `m_homingDone=true`。

---

## 二、问题清单

### P0-1　回零失败/超时路径不停轴（轴可能继续运行）
- 位置：`HomingController.cpp` `onPollTimeout()` 254-286 行；`processPrehomeAxis()` HOME_WAIT_DATUM 超时分支 ~646-653 行；`stepParallel()` HOME_WAIT_DATUM 超时分支 ~830 行。
- 问题：任何轴 HOME_FAIL（DATUM 超时/移开超时）都只把该轴置 `HOME_FAIL`；`onPollTimeout` 检测到 anyFailed 后仅 `stop timer + m_isHoming=false + setVerbose(true) + emit homingFailed`，**没有调用 `singleAxisCancel` / `rapidStop` 停止仍在运行的轴**。控制器端 DATUM 是硬件级找零过程，上位机宣布"失败"后轴仍可能继续朝原点/限位运动；若原点传感器丢失/卡滞，轴可能一直运动直到控制器 DATUM 超时或撞限位。
- 影响：失败后机械臂不受控持续运动；若操作者认为"已失败"而靠近设备，有安全隐患。
- 建议：所有 FAIL 出口统一先逐轴 `singleAxisCancel` 或 `rapidStop`，再停止轮询；或将 `onPollTimeout` 的失败分支收敛到 `stopHoming()`（已含 rapidStop）。

### P0-2　回零完成链路"静默成功"：轴未到位/清零失败仍宣布完成并进入就绪
- 位置：`RobotController.cpp:821-848` homingCompleted lambda。
- 问题：
  1. `moveAllAxesToPositions()`（HomingController.cpp:862-881）内部对每轴 `startAxisSingleMoveAbs` **不检查返回结果**，fire-and-forget；
  2. `api->waitForAxesIdle({0..5}, 15000)` 的返回值被忽略——若 15s 内某轴未到位（卡住/超行程），函数返回失败后代码**仍继续清零坐标并 emit operationCompleted("回零")**；
  3. `clearAxisPosition()` 失败只 `qWarning`，仍 emit 成功信号；
  4. `getOriginPosition()` 若返回 <6 个值（Flash/配置异常），整段移动+清零被跳过，**直接 emit operationCompleted**。
- 影响：UI 显示"回零+建模+清零+正解完成，机械臂已就绪"，但实际坐标基准错误（未到原点就清零 / 清零失败），后续逆解/笛卡尔操作全部基于错误基准，属于最危险的"假成功"。
- 建议：
  - `moveAllAxesToPositions` 增加发送结果检查并返回 bool；
  - `waitForAxesIdle` 失败时 emit `operationFailed("回零", "移动到原点超时/失败")` 并**禁止清零**；
  - `clearAxisPosition` 失败同样发失败信号；
  - `originPos.size()<6` 时显式报错而非静默完成。

### P0-3　急停/安全停止不与回零联动，急停后回零会自动继续
- 位置：`RobotController.cpp:359-374` executeSafeStop、`745-765` stopMotion（两者都只 `rapidStop`）；`RobotController.cpp:345-350` stopHoming 存在但未被急停路径调用；`mainwindow.cpp:1219` onBtnStop、`1268` onRequestStopMotion、`1389` onBtnOpEStop。
- 问题：回零过程中用户按急停/停止 → 只发 `rapidStop` 停掉当前运动，`HomingController` 的 pollTimer 仍在跑、`m_isHoming` 仍 true；下一轮轮询发现轴已 idle（如正处于 HOME_WAIT_MOVE_AWAY_ORIGIN / HOME_WAIT_DATUM），状态机认为"移开完成/回零完成"并自动启动下一步 DATUM → **急停后机械臂自动重新运动**。
- 影响：严重违背急停语义；急停后设备自行重启运动，可能伤人/撞机构。
- 建议：`executeSafeStop` / `stopMotion` 中调用 `stopHoming()`（或新增 halted 状态）；急停解除后要求用户显式重新点击回零，禁止状态机在急停期间推进。

### P1-1　串行预处理实际与"严格串行"注释不符：前一轴 DATUM 与后一轴移开重叠
- 位置：`HomingController.cpp` `stepSerial()` 291-319 行（`movedAway` 为放行条件）；`processPrehomeAxis()` 535-673 行。
- 问题：`movedAway` 在轴"移开完成"（HOME_WAIT_MOVE_AWAY_ORIGIN → idle，~590-596 行）或"传感器未触发直接放行"时就置 true，而该轴真正 DATUM（可能持续数十秒）远未结束；`step()` 213-239 行据此放行下一轴移开 → **前轴 DATUM 与后轴移开/回零重叠并行**。这与 startHoming/step 头注释"串行预处理(J3→J2→J5)依次移开避免机械干涉"的意图不完全一致；若设计目标只是"移开阶段互斥"则注释误导，若目标是"三轴依次完成回零再放行"则实现错误。
- 影响：J2/J3/J5 关节在移开/找零过程中产生不可预期的联动位形，可能引入干涉（注释本意正是防干涉）。
- 建议：明确设计意图后二选一——严格串行则把放行条件改为 `m_isHomed[i]==true`（HOME_SUCCESS 后）；仅移开互斥则在 `step()` 头注释中如实说明，并保留"移开未完成禁止并行 DATUM"的闸门。

### P1-2　串行预处理移开后不二次确认原点传感器已释放（与并行行为不一致）
- 位置：`HomingController.cpp` `processPrehomeAxis()` HOME_WAIT_MOVE_AWAY_ORIGIN→HOME_START_DATUM（~590-600 行）vs `stepParallel()` HOME_WAIT_MOVE_AWAY_ORIGIN→HOME_CHECK_SENSOR（~737-740 行）。
- 问题：并行阶段移开后回 CHECK_SENSOR 确认已脱离原点传感器再启动 DATUM；串行阶段移开一停就立即 START_DATUM。若 `moveAwayOrigin` 距离不足/方向偏差导致仍压着传感器，DATUM 将在错误起点上执行。
- 建议：串行 WAIT_MOVE_AWAY 完成后也回到 CHECK_SENSOR 确认 `sensorState==0` 再 DATUM。

### P1-3　反向限位回退无次数上限，且可能打断已完成 DATUM
- 位置：`HomingController.cpp` `stepParallel()` HOME_WAIT_DATUM ~800-815 行。
- 问题：等待 DATUM 期间只要读到 negIn 低电平（反向限位触发）就 `singleAxisCancel(mode=2)` + 转 `HOME_MOVE_AWAY_REVERSE`（J0 回退 -155°、J5 回退 -220°，行程极大）。存在两个风险：
  1. **无回退次数上限**：若反向限位传感器故障/常触发，轴会反复"cancel→大幅回退→重查→再 DATUM→再 cancel"，大幅摆动直到 180s 超时；
  2. **误打断**：未先查 `homeStatus` 是否已置成功，若 DATUM 实际已到原点、仅因轮询滞后读到限位瞬时信号，会把已完成回零打断重来。
- 建议：进入回退前先读 homeStatus，成功则不处理；为每轴加反向回退计数上限（如 2~3 次），超限直接 FAIL + rapidStop；回退行程建议按实际机械边界缩小或分段确认。

### P1-4　MainWindow 成功路径残留 conn2、失败分支不恢复 Verbose 日志
- 位置：`mainwindow.cpp:818-876` conn1/conn2。
- 问题：
  1. conn1 成功分支只 `disconnect(*conn1)`，**conn2 一直残留连接**；后续任意 `operationFailed("回零")`（如用户再次回零失败、手动停止）会触发旧 conn2 副作用（重复显示失败、`updateConnectionUI(false)` 禁用连接按钮、重复 setVerbose(true)）；第二次回零失败时新旧两个 conn2 会先后触发。
  2. conn1 中建模失败/模式切换失败等提前 return 分支只做 UI 提示，**不 `ZmotionAPI::setVerbose(true)`**——而 `HomingController::startHoming()` 开头 `setVerbose(false)`，一旦走到这些分支，全局详细日志永久关闭。
- 建议：conn1 成功分支同时断开 conn1+conn2；所有提前 return 出口统一恢复 verbose（或改用 RAII/作用域退出钩子）。

### P1-5　`setVerbose(false)` 未在 HomingController 成功路径恢复（日志静默泄漏）
- 位置：`HomingController.cpp` startHoming 173 行 `setVerbose(false)`；`onPollTimeout()` isAllHomed 成功分支 259-267 行只 stop+emit，**未恢复**；而 anyFailed 分支和 `stopHoming()` 都恢复了。
- 影响：若 homingCompleted 后 MainWindow 链路（P1-4）没有走到恢复点，后续所有 ZmotionAPI 详细日志静默丢失，排障困难。
- 建议：成功路径同样恢复 verbose，或把 verbose 开关收敛为 RAII 管理。

### P1-6　急停后解除依赖"重跑流程脚本"，回零流程不被重跑
- 位置：`mainwindow.cpp:1365-1392、1506-1508`。
- 问题：操作界面急停按钮 `onBtnOpEStop` 为"急停→解除急停并重新执行 rbt 流程脚本"的语义；急停只影响流程脚本（UiController），不影响 HomingController（见 P0-3）。在回零/建模/清零这类非脚本链路上，急停按钮的"解除重跑"语义完全不生效。
- 建议：把"回零"也纳入急停态管理，或急停发生时明确提示"回零已被取消，请重新点击回零"。

### P1-7　`waitForAxesIdle(15000)` 在 UI 线程同步阻塞
- 位置：`RobotController.cpp:836`（在 `homingCompleted` 同步回调中执行，而 homingCompleted 由主线程 pollTimer 触发）。
- 问题：回调里 `waitForAxesIdle` 若为阻塞等待 15s，主线程/UI 会冻结最长 15s；期间急停按钮等事件无法及时处理（若 P0-3 已修复则此处是下一个卡 UI 点）。
- 建议：移动到位也纳入 HomingController 的轮询/异步流程，或把 wait 改为事件驱动的到达检测 + 超时回调，避免主线程阻塞。

### P2-1　回零参数双数据源，机型化未落地（RobotModels 与 HomingController 各存一份）
- 位置：`HomingController.cpp:19-50`（static `g_axis_hw` / `g_home_strategy`，且 323-324/500/538-539/691-692/877 各处直接消费）；`RobotModels.cpp:40-84`（buildR01 中 HomeSpec h0~h5，注释 L40 却写"原 HomingController.cpp 的 g_axis_hw + g_home_strategy（已删除）"——实际**并未删除**）。
- 问题：同一套回零硬件参数在 `HomingController.cpp` 和 `RobotModels.cpp` 各维护一份，前者是唯一实际生效源，后者声称已删除却仍保留；R02 机型 buildR02 注释"home 字段留默认（homeIn=-1），R02 实装时补全"——HomingController 写死 6 轴且从不读机型参数，即使 R02 补全也不生效。任何一边改动不同步即产生"回零方向/IO 错误 → 撞限位"级隐患。
- 建议：删除 HomingController.cpp 内 static 数组，统一从 `RobotConfig/RobotModelRegistry`（经机型下发）读取 homeIn/negIn/datumMode/moveAway 等；HomingController 参数化（axisCount/机型），让 R02 等机型真正可回零。

### P2-2　`g_axis_hw[].dir` 字段零消费（方向配置是死代码）
- 位置：`HomingController.cpp:19-42`（字段含 dir）；全文无任何 `hw.dir` 读取。
- 影响：移开/回零方向完全依赖 `moveAwayOrigin/moveAwayReverse` 的数值符号与硬件接线一致；`dir` 字段被定义却不消费，维护者会误以为方向已处理，一旦实际运动方向与假设相反，移开即撞限位。
- 建议：要么在 moveAxisRelative/startAxisDatum 中显式消费 dir，要么删除该字段并在注释中说明方向由硬件/数值决定。

### P2-3　HOME_CHECK_SENSOR 变量命名语义反置（可读性/易错）
- 位置：`HomingController.cpp` processPrehomeAxis ~559-573 行：`homeTriggered = (diVal != 0)`，而按 `initAxisParams` 的 `setInvertIn(homeIn,1)` + `checkAxisSensors`（~429-432 行 `if (!homeTriggered) return 1;`）语义，**低电平 0 才是触发**，非 0 是未触发——变量名 `homeTriggered` 实际表示"未触发/已离开原点"。当前分支行为恰好正确（0→移开；非0→直接 DATUM），但命名与真值相反，极易被后续维护改错。
- 建议：改名（如 `atOrigin / homeSensorActive` 反向定义）或加注释明示"0=触发"。

### P2-4　串行阶段 MOVE_AWAY 前不等待轴真正空闲（急停/重试时序风险）
- 位置：`HomingController.cpp` processPrehomeAxis HOME_MOVE_AWAY_ORIGIN ~576-586 行；对比 stepParallel 对 MOVE_AWAY/START_DATUM 有 `needsIdle` 保护（~706-717 行），串行路径没有。
- 问题：`startHoming()` 先 `rapidStop()`，200ms 后首轮轮询若判"未触发"直接发相对运动；若上一轮运动（含 stopHoming 后快速重试）尚未停稳，叠加运动命令可能被控制器拒绝或产生异常。串行路径同样应在 MOVE_AWAY/START_DATUM 前检查 isAxisIdle。
- 建议：串行各运动发射点统一加 idle 闸门；`startHoming()` 中 rapidStop 后先等待全部轴 idle 再启动轮询。

### P2-5　`step()` 无 `m_isHoming` 保护 + 失败后状态机残留
- 位置：`HomingController.cpp:213-239`。
- 问题：`step()` 为 public，若在非回零状态被调用（例如未来定时器复用、误连）会直接发起运动指令；且轴 FAIL 后 `m_homeState[i]` 停留在 HOME_FAIL，下一次 startHoming 的 reset 会覆盖（OK），但 FAIL→重新 start 时若上一轮某轴仍在运动（P0-1 未修复），reset 后立刻发新运动存在竞态。
- 建议：step 改为 private（仅 timer 调）或入口加 `m_isHoming` 保护；startHoming 开头先 rapidStop + 等待全部轴 idle。

### P2-6　FAIL 信号重复发射（无沿检测）
- 位置：`HomingController.cpp` onPollTimeout ~270-283 行。
- 问题：每轮 poll 对每个 HOME_FAIL 轴都 `emit homingAxisFailed`，无上升沿去重；若 UI/其他模块连接该信号弹窗/告警，会重复弹出直至流程终止。
- 建议：仅在轴首次转入 HOME_FAIL 时 emit 一次。

### P2-7　超时参数与用户可操作性
- 位置：`HomingController.cpp` m_homingTimeoutMs=180000（单轴 DATUM 180s）；kMoveTimeoutMs=30000。
- 问题：全流程最坏耗时 = 串行 3×(移开 30s+DATUM 180s) + 并行 180s ≈ 10 分钟级；UI 上回零期间无"取消回零"按钮（onBtnStop 只 stopMotion，不 stopHoming），用户只能干等或依赖总流程失败。
- 建议：缩短/分级超时（如 DATUM 60s 内应能完成）；UI 提供"取消回零"（调 stopHoming），并展示各轴进度。

### P2-8　回零后绝对移动依赖 DATUM 结束时的 DPOS 语义，未显式核对
- 位置：`RobotController.cpp:821-847`（DATUM 完成→直接 `moveAllAxesToPositions(originPos)` 绝对定位）。
- 问题：绝对移动的目标（Flash 原点/出厂默认，如 J0≈85°）只有当 DATUM 结束后控制器轴坐标 DPOS 已归零（或为已知基准）时才正确；代码未在 DATUM 后先清零/确认 DPOS 语义，而是移动完才 `clearAxisPosition`。若 ZMC DATUM 完成不自动归零 DPOS，此次绝对移动目标会叠加历史 DPOS 残留，落点错误，随后又被整体清零 → 产生错误坐标基准但 UI 显示成功。
- 建议：按 ZMC 手册核实 DATUM 结束后的 DPOS 行为；稳妥做法是 DATUM 成功后先显式清零 DPOS/MPOS 再绝对移动至 originPos（保持"清零后落位 Flash 原点"的不变量），并在移动完成后二次校验实际 DPOS≈originPos。

### P2-9　`DATUM mode` 语义/注释与控制器手册需核对
- 位置：`RobotModels.h:16-35` HomeSpec（datumMode 注释"3=正向限位,4=反向限位"）；`HomingController.cpp:497-533` startAxisDatum 使用 st.datumMode；`ZmotionAPI` setAxisSingleDatum（内部 UserDatum）封装注释为"自定义二次回零"。
- 问题：ZMC 各型号 DATUM mode 编号含义（正/负限位、是否含原点、找零方向）存在差异，代码注释"3=正向、4=反向"与常见手册定义未必一致；且 `initAxisParams` 对 homeIn 统一 `setInvertIn(homeIn,1)`，反转电平后控制器 DATUM 的触发沿与上位机轮询判据（低电平触发）是否一致需现场验证。任何一边理解错，找零方向/触发沿错误都表现为"回零越走越远/撞限位"。
- 建议：对照实际控制器手册逐一核对 datumMode 3/4、moveAwayOrigin/Reverse 符号与 setInvertIn 的组合；建议以控制器 DATUM 状态机为准，上位机只读 homeStatus，不自行按电平推断找零方向。

### P2-10　关机解除路径与回零基准混用
- 位置：`mainwindow.cpp:761-776`（m_isShutdown 分支用 `moveJ({0,0,0,0,0,0})` 当"回到原点"）。
- 问题：该"回原点"依赖当前 DPOS 基准（绝对 0），若此前从未回零/DPOS 非 0，落点并非机械原点，却提示"已回到原点"；与真正的回零语义混淆。
- 建议：关机解除应复用回零流程或明确提示"回到软件零点而非机械原点"。

---

## 三、汇总与建议修复顺序

| 优先级 | 问题 | 关键位置 | 类型 |
|---|---|---|---|
| P0 | 失败/超时不停轴 | HomingController.cpp:254-286 | 安全/失控 |
| P0 | 移动未到位/清零失败仍报成功 | RobotController.cpp:821-848 | 坐标基准错误 |
| P0 | 急停不回零联动，急停后自动续跑 | RobotController.cpp:359-374/745-765, mainwindow.cpp:1389 | 安全 |
| P1 | 串行预处理名实不符（DATUM 与移开重叠） | HomingController.cpp:291-319/535-673 | 时序/干涉 |
| P1 | 串行移开后不确认传感器释放 | HomingController.cpp:590-600 | 逻辑 |
| P1 | 反向限位回退无上限、可能误打断 | HomingController.cpp:800-815 | 机械冲击 |
| P1 | conn2 残留 + verbose 不恢复 | mainwindow.cpp:818-876 | UI/日志 |
| P1 | setVerbose 成功路径不恢复 | HomingController.cpp:259-267 | 日志 |
| P1 | UI 线程阻塞 15s | RobotController.cpp:836 | 卡 UI |
| P1 | 急停解除不重跑回零 | mainwindow.cpp:1365-1392 | 语义 |
| P2 | 参数双数据源/机型化未落地 | HomingController.cpp:19-50 vs RobotModels.cpp:40-84 | 维护性 |
| P2 | dir 死代码 / 命名反置 / 缺 idle 闸门 / FAIL 重复 emit 等 | HomingController.cpp 多处 | 维护性 |

**建议修复顺序**：先处理 3 个 P0（停止语义 + 假成功 + 急停联动），再做 P1-3（反向限位回退保护）与 P1-4/5（信号/日志收敛），随后统一参数源（P2-1）并核对 DATUM 模式（P2-9），最后清理串行语义与命名。

> 说明：本报告基于静态代码审查，涉及控制器（ZMC）DATUM 模式、INVERT_IN 电平语义、DATUM 结束 DPOS 行为等需结合实际控制器手册与现场联调验证。
*（内容由AI生成，仅供参考）*
*（内容由AI生成，仅供参考）*
