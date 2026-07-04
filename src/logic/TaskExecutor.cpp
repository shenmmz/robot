/*
 * @file TaskExecutor.cpp
 * @brief 实现 TaskExecutor 的指令队列管理与逐步执行逻辑。
 *
 * 支持 MOVEJ/MOVEL/WAIT_IDLE/DELAY/SET_DO/WAIT_DI/PRINT/CALL 等指令。
 */
#include "TaskExecutor.h"
#include "RobotController.h"
#include "../hardware/MotionController.h"
#include "../hardware/ServoController.h"
#include "../models/RobotState.h"
#include "../../ZmotionAPI/ZmotionAPI.h"
#include "../utils/MathUtils.h"
#include <QDebug>
#include <QCoreApplication>
#include <QThread>
#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QDir>

/**
 * @brief 构造函数：初始化任务执行器
 * @param robotCtrl 机器人控制器指针（用于执行运动指令）
 * @param parent    父对象
 */
TaskExecutor::TaskExecutor(RobotController* robotCtrl, QObject* parent)
    : QObject(parent)
    , m_robotController(robotCtrl)
{
}

/**
 * @brief 析构函数：停止正在执行的任务
 */
TaskExecutor::~TaskExecutor()
{
    stop();
}

/**
 * @brief 设置待执行的指令列表（准备阶段）
 *
 * 调用此函数后，指令列表被加载到内部队列，同时：
 *   - 清空变量表 m_variables
 *   - 重置当前执行索引 m_currentIdx = 0
 *   - 状态置为 Idle
 *   - 清空循环栈并重置全局参数
 *   - 调用 buildLoopMap() 构建循环跳转映射表
 *
 * @param commands 解析好的指令列表（由 UiController::parseScript() 生成）
 */
void TaskExecutor::setCommands(const QList<ScriptCommand>& commands)
{
    m_variables.clear();
    m_commands = commands;
    m_currentIdx = 0;
    m_state = Idle;
    m_stepOnce = false;
    m_loopStack.clear();
    // 重置运行时全局参数（-1 表示使用指令自身参数）
    m_globalSpeed = -1;
    m_globalAccel = -1;
    m_globalDecel = -1;
    m_globalTimeout = -1;
    buildLoopMap();
}

/**
 * @brief 开始执行任务（主入口）
 *
 * 检查条件：
 *   - 指令列表不能为空
 *   - 机器人控制器必须已连接
 *
 * 满足条件后：
 *   - 重置当前索引为 0
 *   - 状态置为 Running
 *   - 发出 taskStarted 信号
 *   - 调用 executeNext() 开始逐条执行
 */
void TaskExecutor::start()
{
    if (m_commands.isEmpty()) {
        emit taskError(QStringLiteral("指令列表为空"));
        return;
    }
    if (!m_robotController || !m_robotController->isConnected()) {
        emit taskError(QStringLiteral("控制器未连接"));
        return;
    }

    m_currentIdx = 0;
    m_state = Running;
    m_stepOnce = false;
    emit taskStarted(m_commands.size());
    qDebug() << "[TaskExecutor] 开始执行" << m_commands.size() << "条指令";

    executeNext();
}

/**
 * @brief 单步执行：仅执行当前 m_currentIdx 处的指令，结束后停在 Idle。
 *
 * 前提：已通过 setCommands() 加载指令列表（指令在 m_commands 中）。
 * 每次调用 stepOnce() 会从 m_currentIdx 推进一条；连续点击即"走一步停一步"。
 * 如果 m_currentIdx 已到达末尾，会 emit taskFinished 并保持 Idle。
 */
void TaskExecutor::stepOnce()
{
    if (m_commands.isEmpty()) {
        emit taskError(QStringLiteral("指令列表为空"));
        return;
    }
    if (!m_robotController || !m_robotController->isConnected()) {
        emit taskError(QStringLiteral("控制器未连接"));
        return;
    }
    if (m_state == Running) {
        // 已有任务在跑，不允许并发单步
        return;
    }

    // 全部走完则结束
    if (m_currentIdx >= m_commands.size()) {
        m_state = Idle;
        emit taskFinished();
        return;
    }

    // 第一次调用时发 taskStarted（与 start() 保持一致的事件语义）
    if (m_currentIdx == 0) {
        emit taskStarted(m_commands.size());
    }

    m_state = Running;
    m_stepOnce = true;
    qDebug() << "[TaskExecutor] 单步执行" << (m_currentIdx + 1) << "/" << m_commands.size();

    executeNext();
}

/**
 * @brief 重置到起始位置：清 m_currentIdx / 变量表 / 循环栈 / 全局参数。
 *
 * 单步模式走完末尾后，UI 调用此函数让"单步"按钮可继续从头运行。
 */
void TaskExecutor::reset()
{
    m_variables.clear();
    m_currentIdx = 0;
    m_state = Idle;
    m_stepOnce = false;
    m_loopStack.clear();
    m_globalSpeed = -1;
    m_globalAccel = -1;
    m_globalDecel = -1;
    m_globalTimeout = -1;
}

/**
 * @brief 暂停任务执行
 *
 * 将状态从 Running 变为 Paused，同时：
 *   - 调用 stopMotion() 停止当前运动
 *   - 等待所有轴停止（最多 2000ms）
 *   - 发出 taskPaused 信号通知 UI 更新
 */
void TaskExecutor::pause()
{
    if (m_state != Running) return;
    m_state = Paused;
    m_robotController->stopMotion();

    ZmotionAPI* api = m_robotController->getMotionController() &&
                      m_robotController->getMotionController()->getZmotionAPI()
                          ? m_robotController->getMotionController()->getZmotionAPI()
                          : nullptr;
    if (api) {
        api->waitForAxesIdle({0, 1, 2, 3, 4, 5}, 2000);
    }

    // 取消所有进行中的舵机软插值（不取消 m_state，resume() 后不重发 SERVO）
    if (ServoController* servo = m_robotController->getServoController()) {
        servo->cancelAllMovements();
    }

    qDebug() << "[TaskExecutor] 已暂停，当前进度:" << m_currentIdx << "/" << m_commands.size();
    emit taskPaused(m_currentIdx, m_commands.size());
}

/**
 * @brief 恢复暂停的任务
 *
 * 恢复前会根据下一条指令类型设置运动学模式：
 *   - MoveJ → 正向运动学模式
 *   - MoveL → 逆向运动学模式
 *
 * 然后将状态改为 Running，发出 taskResumed 信号，继续执行。
 */
void TaskExecutor::resume()
{
    if (m_state != Paused) return;

    if (m_currentIdx < m_commands.size()) {
        if (m_commands[m_currentIdx].type == ScriptCmdType::MoveJ) {
            m_robotController->getMotionController()->setForwardKinematicsMode();
        } else if (m_commands[m_currentIdx].type == ScriptCmdType::MoveL) {
            m_robotController->getMotionController()->setInverseKinematicsMode();
        }
    }

    m_state = Running;
    qDebug() << "[TaskExecutor] 恢复执行，从第" << (m_currentIdx) << "条指令继续";
    emit taskResumed(m_currentIdx, m_commands.size());
    executeNext();
}

/**
 * @brief 强制停止任务（彻底终止）
 *
 * 无论当前状态如何：
 *   - 若正在运行则先调用 stopMotion()
 *   - 状态置为 Idle
 *   - 清空指令列表和索引
 */
void TaskExecutor::stop()
{
    if (m_state == Running) {
        m_robotController->stopMotion();
        // 取消所有进行中的舵机软插值（100ms tick 推进），让 isMoving 立即复位，
        // waitForServoIdle 可在下一轮询退出
        if (ServoController* servo = m_robotController->getServoController()) {
            servo->cancelAllMovements();
        }
    }
    m_state = Idle;
    m_commands.clear();
    m_currentIdx = 0;
}

/**
 * @brief 续行调度：根据单步/正常模式决定是立即执行下一条还是停在 Idle
 *
 * 单步模式下：清掉 m_stepOnce 标志、把 m_state 置回 Idle，UI 可再次调用 stepOnce() 续走下一条。
 * 正常模式下：用 QTimer::singleShot(0) 排队 executeNext()，避免栈溢出。
 */
void TaskExecutor::scheduleNext()
{
    if (m_stepOnce) {
        m_stepOnce = false;
        m_state = Idle;
        return;
    }
    QTimer::singleShot(0, this, &TaskExecutor::executeNext);
}

/**
 * @brief 指令执行主循环（递归调用）
 *
 * 核心执行流程：
 *   1. 检查状态是否为 Running（非运行状态则返回）
 *   2. 检查是否已执行完所有指令（是则发出 taskFinished 信号）
 *   3. 发出 taskProgress 信号更新进度
 *   4. 处理控制流指令（FOR/IF_DI/ELSE/END_IF/WHILE/END_FOR/END_WHILE）
 *   5. 调用 executeCommand() 执行普通指令
 *   6. 通过 QTimer::singleShot(0) 递归调用自身执行下一条
 */
void TaskExecutor::executeNext()
{
    if (m_state != Running) return;

    if (m_currentIdx >= m_commands.size()) {
        m_state = Idle;
        emit taskFinished();
        return;
    }

    const ScriptCommand& cmd = m_commands[m_currentIdx];
    emit taskProgress(m_currentIdx + 1, m_commands.size());
    // 通知 UI 高亮当前源代码行（-1 表示空行/注释，无需高亮）
    emit currentLineChanged(cmd.sourceLineNo);

    // 控制台输出: 显示正在执行的指令
    if (cmd.type != ScriptCmdType::TargetDef) {
        QString prefix;
        switch (cmd.type) {
        case ScriptCmdType::MoveJ:   prefix = QStringLiteral("MOVEJ"); break;
        case ScriptCmdType::MoveL:   prefix = QStringLiteral("MOVEL"); break;
        case ScriptCmdType::MoveOff: prefix = QStringLiteral("MOVEJOFF"); break;
        case ScriptCmdType::MoveLOff:prefix = QStringLiteral("MOVELOFF"); break;
        case ScriptCmdType::Delay:   prefix = QStringLiteral("DELAY"); break;
        case ScriptCmdType::SetDO:   prefix = QStringLiteral("SET_DO"); break;
        case ScriptCmdType::WaitDI:  prefix = QStringLiteral("WAIT_DI"); break;
        case ScriptCmdType::WaitIdle:prefix = QStringLiteral("WAIT_IDLE"); break;
        case ScriptCmdType::Print:   prefix = QStringLiteral("PRINT"); break;
        case ScriptCmdType::ReadPos: prefix = QStringLiteral("READ_POS"); break;
        case ScriptCmdType::SpeedSet:prefix = QStringLiteral("SPEED"); break;
        case ScriptCmdType::AccelSet:prefix = QStringLiteral("ACCEL"); break;
        case ScriptCmdType::DecelSet:prefix = QStringLiteral("DECEL"); break;
        case ScriptCmdType::TimeoutSet:prefix=QStringLiteral("TIMEOUT"); break;
        case ScriptCmdType::SetVar:  prefix = QStringLiteral("SET"); break;
        case ScriptCmdType::ForLoop: prefix = QStringLiteral("FOR"); break;
        case ScriptCmdType::EndFor:  prefix = QStringLiteral("END_FOR"); break;
        case ScriptCmdType::WhileTrue:prefix=QStringLiteral("WHILE"); break;
        case ScriptCmdType::EndWhile:prefix=QStringLiteral("END_WHILE"); break;
        case ScriptCmdType::ServoMove: prefix=QStringLiteral("SERVO"); break;
        default: prefix = QStringLiteral("●"); break;
        }
        // 输出 TARGET 定义 + 源码行
        if ((cmd.type == ScriptCmdType::MoveJ || cmd.type == ScriptCmdType::MoveL)
            && !cmd.targetName.isEmpty() && m_targets.contains(cmd.targetName)) {
            const QList<double>& tgt = m_targets[cmd.targetName];
            if (cmd.type == ScriptCmdType::MoveJ)
                emit consoleOutput(QStringLiteral("TARGET %1 = {J0: %2, J1: %3, J2: %4, J3: %5, J4: %6, J5: %7}")
                    .arg(cmd.targetName).arg(tgt.value(0),0,'f',3).arg(tgt.value(1),0,'f',3)
                    .arg(tgt.value(2),0,'f',3).arg(tgt.value(3),0,'f',3)
                    .arg(tgt.value(4),0,'f',3).arg(tgt.value(5),0,'f',3));
            else
                emit consoleOutput(QStringLiteral("TARGET %1 = {X: %2, Y: %3, Z: %4, A: %5, B: %6, C: %7}")
                    .arg(cmd.targetName).arg(tgt.value(0),0,'f',3).arg(tgt.value(1),0,'f',3)
                    .arg(tgt.value(2),0,'f',3).arg(tgt.value(3),0,'f',3)
                    .arg(tgt.value(4),0,'f',3).arg(tgt.value(5),0,'f',3));
            emit consoleOutput(cmd.sourceLine);
        }
        emit consoleOutput(QStringLiteral("▶ %1  [%2/%3]").arg(prefix).arg(m_currentIdx + 1).arg(m_commands.size()));
    }

    // === 循环入口: FOR(n) ===
    if (cmd.type == ScriptCmdType::ForLoop) {
        if (cmd.loopCount == -1 && !cmd.varName.isEmpty()) {
            // 新语法: FOR(var, start, end, step)
            if (cmd.loopStep == 0) { m_currentIdx++; scheduleNext(); return; }
            bool forward = (cmd.loopStep > 0);
            if ((forward && cmd.loopStart > cmd.loopEnd) || (!forward && cmd.loopStart < cmd.loopEnd)) {
                // 没有可迭代的 → 跳转 END_FOR
                m_currentIdx = m_loopEndMap.value(m_currentIdx, m_currentIdx) + 1;
            } else {
                LoopFrame frame;
                frame.headIdx = m_currentIdx;
                frame.remaining = -1;  // 标记新语法
                m_loopStack.push(frame);
                m_variables[cmd.varName] = cmd.loopStart;
                m_currentIdx++;  // 进入循环体
            }
        } else if (cmd.loopCount > 0) {
            // 旧语法: FOR(n)
            LoopFrame frame;
            frame.headIdx = m_currentIdx;
            frame.remaining = cmd.loopCount;
            m_loopStack.push(frame);
            m_variables[QStringLiteral("$i")] = 0;
            m_variables[QStringLiteral("$n")] = cmd.loopCount;
            m_currentIdx++;
        } else {
            m_currentIdx = m_loopEndMap.value(m_currentIdx, m_currentIdx) + 1;
        }
        scheduleNext();
        return;
    }

    // === IF_DI 分支入口 ===
    if (cmd.type == ScriptCmdType::IfDI) {
        int diVal = 0;
        ZmotionAPI* api = m_robotController->getMotionController()->getZmotionAPI();
        diVal = api->getDI(cmd.ioIndex);
        if (diVal == cmd.ioValue) {
            m_currentIdx++;
        } else {
            int j = m_currentIdx + 1;
            while (j < m_commands.size()) {
                auto t = m_commands[j].type;
                if (t == ScriptCmdType::Else) { m_currentIdx = j + 1; break; }
                if (t == ScriptCmdType::EndIf) { m_currentIdx = j + 1; break; }
                j++;
            }
            if (j >= m_commands.size()) m_currentIdx++;
        }
        scheduleNext();
        return;
    }

    // === ELSE 跳过 ===
    if (cmd.type == ScriptCmdType::Else) {
        int j = m_currentIdx + 1;
        while (j < m_commands.size() && m_commands[j].type != ScriptCmdType::EndIf) j++;
        m_currentIdx = (j < m_commands.size()) ? j + 1 : j;
        scheduleNext();
        return;
    }

    // === END_IF ===
    if (cmd.type == ScriptCmdType::EndIf) {
        m_currentIdx++;
        scheduleNext();
        return;
    }

    // === 循环入口: WHILE TRUE ===
    if (cmd.type == ScriptCmdType::WhileTrue) {
        LoopFrame frame;
        frame.headIdx = m_currentIdx;
        frame.remaining = -1;  // 无条件无限循环
        m_loopStack.push(frame);
        m_currentIdx++;
        scheduleNext();
        return;
    }

    // === 循环入口: WHILE_DI(di, val) ===
    if (cmd.type == ScriptCmdType::WhileDI) {
        int diVal = 0;
        ZmotionAPI* api = m_robotController->getMotionController()->getZmotionAPI();
        diVal = api->getDI(cmd.ioIndex);
        if (diVal == cmd.ioValue) {
            LoopFrame frame;
            frame.headIdx = m_currentIdx;
            frame.remaining = -1;  // -1 表示 WHILE 无限循环
            m_loopStack.push(frame);
            m_currentIdx++;  // 进入循环体
        } else {
            // 条件不满足 → 跳到 END_WHILE 后
            m_currentIdx = m_loopEndMap.value(m_currentIdx, m_currentIdx) + 1;
        }
        scheduleNext();
        return;
    }

    // === 循环出口: END_FOR ===
    if (cmd.type == ScriptCmdType::EndFor) {
        if (!m_loopStack.isEmpty()) {
            auto& frame = m_loopStack.top();
            if (frame.remaining == -1) {
                // 新语法: FOR(var, start, end, step) — 递增变量并判断
                int headIdx = frame.headIdx;
                if (headIdx >= 0 && headIdx < m_commands.size()) {
                    const auto& forCmd = m_commands[headIdx];
                    double& val = m_variables[forCmd.varName];
                    val += forCmd.loopStep;
                    bool forward = (forCmd.loopStep > 0);
                    if ((forward && val <= forCmd.loopEnd) || (!forward && val >= forCmd.loopEnd)) {
                        m_currentIdx = headIdx + 1;  // 继续循环
                        scheduleNext();
                        return;
                    }
                }
                m_loopStack.pop();
            } else {
                // 旧语法: FOR(n)
                frame.remaining--;
                if (frame.remaining > 0) {
                    m_variables[QStringLiteral("$i")] = static_cast<int>(m_variables[QStringLiteral("$n")]) - frame.remaining;
                    m_currentIdx = m_loopStartMap.value(m_currentIdx, frame.headIdx) + 1;
                    scheduleNext();
                    return;
                }
                m_loopStack.pop();
            }
        }
        m_currentIdx++;
        scheduleNext();
        return;
    }

    // === 循环出口: END_WHILE ===
    if (cmd.type == ScriptCmdType::EndWhile) {
        if (!m_loopStack.isEmpty()) {
            int headIdx = m_loopStartMap.value(m_currentIdx, -1);
            if (headIdx >= 0 && headIdx < m_commands.size()) {
                const auto& whileCmd = m_commands[headIdx];
                if (whileCmd.type == ScriptCmdType::WhileTrue) {
                    m_currentIdx = headIdx + 1;  // 无条件继续循环
                    scheduleNext();
                    return;
                }
                ZmotionAPI* api = m_robotController->getMotionController()->getZmotionAPI();
                int diVal = api->getDI(whileCmd.ioIndex);
                if (diVal == whileCmd.ioValue) {
                    m_currentIdx = headIdx + 1;
                    scheduleNext();
                    return;
                }
            }
            m_loopStack.pop();
        }
        m_currentIdx++;
        scheduleNext();
        return;
    }

    // === 普通指令执行 ===
    bool ok = executeCommand(cmd);

    if (m_state != Running) return;

    if (!ok) {
        emit taskError(QStringLiteral("指令执行失败: %1").arg(cmd.sourceLine));
        m_state = Idle;
        return;
    }

    m_currentIdx++;
    m_robotController->getMotionController()->updateState();
    scheduleNext();
}

/**
 * @brief 执行单条指令
 *
 * 根据指令类型调用相应的执行逻辑，支持的指令包括：
 *   - MoveJ/MoveL:      关节/笛卡尔运动
 *   - MoveOff/MoveLOff: 增量运动（基于当前位置偏移）
 *   - WaitIdle:         等待所有轴停止
 *   - Delay:            延时等待
 *   - SetDO:            设置数字输出
 *   - WaitDI:           等待数字输入变为期望值
 *   - Print:            打印文本到控制台
 *   - ReadPos:          读取并显示当前位置
 *   - SetVar:           变量赋值
 *   - SpeedSet/AccelSet/DecelSet/TimeoutSet: 设置全局运动参数
 *   - Call:             调用子脚本
 *
 * @param cmd 待执行的指令
 * @return true=执行成功，false=执行失败（会导致任务中止）
 */
bool TaskExecutor::executeCommand(const ScriptCommand& cmd)
{
    ZmotionAPI* api = m_robotController->getMotionController()->getZmotionAPI();

    switch (cmd.type) {
    case ScriptCmdType::MoveJ: {
        QList<double> targets;
        // 动态解析：优先从 m_targets 实时取最新点位，改点位即生效
        if (!cmd.targetName.isEmpty() && m_targets.contains(cmd.targetName)) {
            targets = m_targets[cmd.targetName];
        } else if (!cmd.targetRaw.isEmpty()) {
            for (int i = 0; i < cmd.targetRaw.size(); ++i)
                targets.append(resolveValue(cmd.targetRaw[i], m_variables));
        } else {
            targets = cmd.targets;
        }
        double spd = (m_globalSpeed > 0) ? m_globalSpeed : cmd.speed;
        double acc = (m_globalAccel > 0) ? m_globalAccel : cmd.accel;
        double dec = (m_globalDecel > 0) ? m_globalDecel : cmd.decel;
        int tmo = cmd.timeoutMs;
        if (tmo <= 0 || (m_globalTimeout <= 0 && tmo == 5000)) {
            QList<double> cur = m_robotController->getState()->getAllAxisPositions();
            double maxDist = 0.0;
            for (int i = 0; i < qMin(targets.size(), cur.size()); ++i)
                maxDist = qMax(maxDist, qAbs(targets[i] - cur[i]));
            tmo = qMax(5000, static_cast<int>(maxDist / qMax(spd, 0.1) * 2000.0));
        }
        if (m_globalTimeout > 0) tmo = m_globalTimeout;
        // 执行后同步全局参数，后续 MOVEJOFF/MOVELOFF 可继承速度
        m_globalSpeed = spd;
        m_globalAccel = acc;
        m_globalDecel = dec;
        return m_robotController->moveJ(targets, spd, acc, dec, tmo);
    }
    case ScriptCmdType::MoveL: {
        QList<double> targets;
        // 动态解析：优先从 m_targets 实时取最新点位，改点位即生效
        if (!cmd.targetName.isEmpty() && m_targets.contains(cmd.targetName)) {
            targets = m_targets[cmd.targetName];
        } else if (!cmd.targetRaw.isEmpty()) {
            for (int i = 0; i < cmd.targetRaw.size(); ++i)
                targets.append(resolveValue(cmd.targetRaw[i], m_variables));
        } else {
            targets = cmd.targets;
        }
        double spd = (m_globalSpeed > 0) ? m_globalSpeed : cmd.speed;
        double acc = (m_globalAccel > 0) ? m_globalAccel : cmd.accel;
        double dec = (m_globalDecel > 0) ? m_globalDecel : cmd.decel;
        int tmo = cmd.timeoutMs;
        if (tmo <= 0 || (m_globalTimeout <= 0 && tmo == 5000)) {
            QList<double> cur = m_robotController->getState()->getAllCartesianPositions();
            double maxDist = 0.0;
            for (int i = 0; i < qMin(targets.size(), cur.size()); ++i)
                maxDist = qMax(maxDist, qAbs(targets[i] - cur[i]));
            tmo = qMax(5000, static_cast<int>(maxDist / qMax(spd, 0.1) * 2000.0));
        }
        if (m_globalTimeout > 0) tmo = m_globalTimeout;
        m_globalSpeed = spd;
        m_globalAccel = acc;
        m_globalDecel = dec;
        return m_robotController->moveL(targets, spd, acc, dec, tmo);
    }
    case ScriptCmdType::WaitIdle: {
        ZmotionResult r = api->waitForAxesIdle({0, 1, 2, 3, 4, 5}, 30000);
        if (!r.success) {
            emit consoleOutput(QStringLiteral("[WARN] WAIT_IDLE 超时或不可用，跳过"));
        }
        return true;  // 超时不中止任务
    }
    case ScriptCmdType::Delay: {
        int elapsed = 0;
        while (elapsed < cmd.delayMs) {
            if (m_state != Running) return false;
            QCoreApplication::processEvents();
            QThread::msleep(30);
            elapsed += 30;
        }
        return true;
    }
    case ScriptCmdType::SetDO: {
        int ret = ZAux_Direct_SetOp(api->getHandle(), cmd.ioIndex, cmd.ioValue);
        if (ret == 0) {
            emit consoleOutput(QStringLiteral("[INFO] SET_DO %1 → %2").arg(cmd.ioIndex).arg(cmd.ioValue));
            return true;
        }
        return false;
    }
    case ScriptCmdType::WaitDI: {
        int waited = 0;
        while (waited < cmd.ioTimeout) {
            if (m_state != Running) return false;
            int val = api->getDI(cmd.ioIndex);
            if (val == cmd.ioValue) {
                emit consoleOutput(QStringLiteral("[INFO] WAIT_DI %1 → %2 匹配 (%3ms)")
                    .arg(cmd.ioIndex).arg(cmd.ioValue).arg(waited));
                return true;
            }
            QCoreApplication::processEvents();
            QThread::msleep(30);
            waited += 30;
        }
        emit consoleOutput(QStringLiteral("[WARN] WAIT_DI %1 超时 (%2ms)")
            .arg(cmd.ioIndex).arg(cmd.ioTimeout));
        return true;  // 超时不算失败，继续执行
    }
    case ScriptCmdType::Print: {
        emit consoleOutput(QStringLiteral("[PRINT] %1").arg(cmd.printText));
        return true;
    }
    case ScriptCmdType::ReadPos: {
        RobotState* state = m_robotController->getState();
        if (!state) return true;
        QList<double> joints = state->getAllAxisPositions();
        QList<double> carts  = state->getAllCartesianPositions();
        emit consoleOutput(QStringLiteral("--- 关节 (°) ---"));
        emit consoleOutput(QStringLiteral("J0=%1 J1=%2 J2=%3 J3=%4 J4=%5 J5=%6")
            .arg(joints.value(0,0), 0, 'f', 3).arg(joints.value(1,0), 0, 'f', 3).arg(joints.value(2,0), 0, 'f', 3)
            .arg(joints.value(3,0), 0, 'f', 3).arg(joints.value(4,0), 0, 'f', 3).arg(joints.value(5,0), 0, 'f', 3));
        emit consoleOutput(QStringLiteral("--- 笛卡尔 (mm/°) ---"));
        emit consoleOutput(QStringLiteral("X=%1 Y=%2 Z=%3 RX=%4 RY=%5 RZ=%6")
            .arg(carts.value(0,0), 0, 'f', 3).arg(carts.value(1,0), 0, 'f', 3).arg(carts.value(2,0), 0, 'f', 3)
            .arg(carts.value(3,0), 0, 'f', 3).arg(carts.value(4,0), 0, 'f', 3).arg(carts.value(5,0), 0, 'f', 3));
        return true;
    }
    case ScriptCmdType::SetVar: {
        double val;
        if (!cmd.varExpr.isEmpty()) {
            val = evaluateExpr(cmd.varExpr, m_variables);
        } else {
            val = cmd.varValue;
        }
        m_variables[cmd.varName] = val;
        emit consoleOutput(QStringLiteral("[SET] %1 = %2").arg(cmd.varName).arg(val));
        return true;
    }
    case ScriptCmdType::SpeedSet:
        m_globalSpeed = cmd.targets.value(0, 25.0);
        emit consoleOutput(QStringLiteral("[SPEED] %1 mm/s").arg(m_globalSpeed));
        return true;
    case ScriptCmdType::AccelSet:
        m_globalAccel = cmd.targets.value(0, 50.0);
        emit consoleOutput(QStringLiteral("[ACCEL] %1 mm/s²").arg(m_globalAccel));
        return true;
    case ScriptCmdType::DecelSet:
        m_globalDecel = cmd.targets.value(0, 60.0);
        emit consoleOutput(QStringLiteral("[DECEL] %1 mm/s²").arg(m_globalDecel));
        return true;
    case ScriptCmdType::TimeoutSet:
        m_globalTimeout = static_cast<int>(cmd.targets.value(0, 10000));
        emit consoleOutput(QStringLiteral("[TIMEOUT] %1 ms").arg(m_globalTimeout));
        return true;
    case ScriptCmdType::ServoMove: {
        // 修复（2026-06-15）：原 for 循环串行调 setAngleWithSpeed，每通道走 10 次
        // 同步 ZMC RPC（PUTCHAR 逐字节），SETCOM 抖动 + UART 排空不及时导致多通道
        // 丢帧。改为单次 setAnglesForScript → setAllPwm 单帧下发 6 路。
        // 解析时已把所有通道填入 cmd.servoChannels/Angles/Speeds；单路时 size=1。
        // 2026-06-16：增加时间模式支持，若 cmd.servoTimes 中任意一项 > 0 则走混合分支
        // （仿 KPZ32 出厂协议 #xxxPxxxT<ms>! 的 T<ms> 字段语义）。
        ServoController* servo = m_robotController->getServoController();
        if (!servo) {
            emit taskError(QStringLiteral("舵机控制器未初始化"));
            return false;
        }

        // 判断是否走混合模式（时间模式）
        bool useTimeMode = false;
        if (cmd.servoTimes.size() == cmd.servoChannels.size()) {
            for (int t : cmd.servoTimes) {
                if (t > 0) { useTimeMode = true; break; }
            }
        }

        bool ok = false;
        if (useTimeMode) {
            // 时间模式（仿 KPZ32）
            ok = servo->setAnglesForScriptMixed(cmd.servoChannels, cmd.servoAngles,
                                                cmd.servoSpeeds, cmd.servoTimes);
        } else {
            // 速度模式（兼容旧 SERVO 指令）
            ok = servo->setAnglesForScript(cmd.servoChannels, cmd.servoAngles, cmd.servoSpeeds);
        }
        if (!ok) {
            emit taskError(QStringLiteral("SERVO 批量下发失败: %1").arg(cmd.sourceLine));
            return false;
        }
        // 汇总日志（避免 N 行日志刷屏）；时间模式下显示 Tms，速度模式下显示 °/s
        QStringList parts;
        for (int i = 0; i < cmd.servoChannels.size(); ++i) {
            int tMs = cmd.servoTimes.value(i, 0);
            if (useTimeMode && tMs > 0) {
                parts << QStringLiteral("PWM%1→%2°@T%3ms")
                             .arg(cmd.servoChannels[i] + 1)
                             .arg(cmd.servoAngles.value(i, 90.0f), 0, 'f', 1)
                             .arg(tMs);
            } else {
                parts << QStringLiteral("PWM%1→%2°@%3°/s")
                             .arg(cmd.servoChannels[i] + 1)
                             .arg(cmd.servoAngles.value(i, 90.0f), 0, 'f', 1)
                             .arg(cmd.servoSpeeds.value(i, 10.0f), 0, 'f', 0);
            }
        }
        emit consoleOutput(QStringLiteral("[SERVO%1] %2")
                               .arg(useTimeMode ? QStringLiteral("-T") : QString())
                               .arg(parts.join(", ")));
        // 脚本模式：单帧直发时 isMoving 立即 false；时间模式可能启动 100ms tick 软插值
        // 通过超时 5000ms 等待到位（也覆盖运行中切换为 STOP/PAUSE 的情况）
        waitForServoIdle(servo, cmd.servoChannels, 5000);
        return true;
    }
    case ScriptCmdType::MoveOff: {
        QList<double> joints = m_robotController->getState()->getAllAxisPositions();
        QList<double> targets;
        for (int i = 0; i < qMin(joints.size(), cmd.targets.size()); ++i)
            targets.append(joints[i] + cmd.targets[i]);
        while (targets.size() < 6) targets.append(0);
        double spd = (m_globalSpeed > 0) ? m_globalSpeed : cmd.speed;
        double acc = (m_globalAccel > 0) ? m_globalAccel : cmd.accel;
        double dec = (m_globalDecel > 0) ? m_globalDecel : cmd.decel;
        int    tmo = (m_globalTimeout > 0) ? m_globalTimeout : cmd.timeoutMs;
        return m_robotController->moveJ(targets, spd, acc, dec, tmo);
    }
    case ScriptCmdType::MoveLOff: {
        QList<double> cart = m_robotController->getState()->getAllCartesianPositions();
        QList<double> targets;
        for (int i = 0; i < qMin(cart.size(), cmd.targets.size()); ++i)
            targets.append(cart[i] + cmd.targets[i]);
        while (targets.size() < 6) targets.append(0);
        double spd = (m_globalSpeed > 0) ? m_globalSpeed : cmd.speed;
        double acc = (m_globalAccel > 0) ? m_globalAccel : cmd.accel;
        double dec = (m_globalDecel > 0) ? m_globalDecel : cmd.decel;
        int    tmo = (m_globalTimeout > 0) ? m_globalTimeout : cmd.timeoutMs;
        return m_robotController->moveL(targets, spd, acc, dec, tmo);
    }
    case ScriptCmdType::IfDI:
    case ScriptCmdType::Else:
    case ScriptCmdType::EndIf:
        return true;  // 由 executeNext 处理分支逻辑
    case ScriptCmdType::Call: {
        // 加载子脚本并逐行执行
        // 路径查找顺序：
        //   1) 绝对路径或 CWD 相对路径（cmd.callFile 本身）
        //   2) 主脚本所在目录（m_scriptPath 的目录 + cmd.callFile）
        //   3) 程序目录的 config/scripts/（兼容历史部署）
        //   4) 当前进程工作目录下的相对路径
        const QString fileName = cmd.callFile;
        QStringList candidates;
        candidates << fileName; // 1) 自身/绝对路径/CWD
        if (!m_scriptPath.isEmpty()) {
            QFileInfo scriptFi(m_scriptPath);
            const QString scriptDir = scriptFi.absolutePath();
            if (!scriptDir.isEmpty())
                candidates << (scriptDir + QLatin1Char('/') + fileName); // 2) 同目录
        }
        candidates << (QCoreApplication::applicationDirPath()
                       + QStringLiteral("/config/scripts/") + fileName); // 3) 历史部署目录
        candidates << (QStringLiteral("./") + fileName); // 4) CWD 相对路径

        QString path;
        for (const QString& c : candidates) {
            if (QFileInfo::exists(c)) { path = c; break; }
        }
        if (path.isEmpty()) {
            // 都找不到——回退第一个候选（用于错误信息更友好）
            path = fileName;
        }

        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            emit consoleOutput(QStringLiteral("[ERROR] CALL 无法打开: %1").arg(cmd.callFile));
            return false;
        }
        QString content = QString::fromUtf8(file.readAll());
        file.close();

        // 简易解析子脚本（只支持 MOVEJ/MOVEL/WAIT_IDLE/DELAY/SET_DO/WAIT_DI/PRINT）
        QStringList lines = content.split(QChar::fromLatin1('\n'), Qt::SkipEmptyParts);
        for (const QString& line : lines) {
            if (m_state != Running) return false;
            QString trimmed = line.trimmed();
            if (trimmed.isEmpty() || trimmed.startsWith(QChar::fromLatin1('#')))
                continue;

            // 快速解析单行
            ScriptCommand sub;
            sub.sourceLine = trimmed;
            if (trimmed.startsWith(QStringLiteral("MOVEJ("), Qt::CaseInsensitive)) {
                sub.type = ScriptCmdType::MoveJ;
                int s = trimmed.indexOf(QChar::fromLatin1('{')); int e = trimmed.indexOf(QChar::fromLatin1('}'));
                if (s >= 0 && e >= 0) {
                    for (const auto& p : trimmed.mid(s+1, e-s-1).split(QChar::fromLatin1(','), Qt::SkipEmptyParts))
                        sub.targets.append(p.trimmed().toDouble());
                }
            } else if (trimmed.startsWith(QStringLiteral("MOVEL("), Qt::CaseInsensitive)) {
                sub.type = ScriptCmdType::MoveL;
                int s = trimmed.indexOf(QChar::fromLatin1('{')); int e = trimmed.indexOf(QChar::fromLatin1('}'));
                if (s >= 0 && e >= 0) {
                    for (const auto& p : trimmed.mid(s+1, e-s-1).split(QChar::fromLatin1(','), Qt::SkipEmptyParts))
                        sub.targets.append(p.trimmed().toDouble());
                }
            } else if (trimmed.startsWith(QStringLiteral("WAIT_IDLE"), Qt::CaseInsensitive)) {
                sub.type = ScriptCmdType::WaitIdle;
            } else if (trimmed.startsWith(QStringLiteral("READ_POS"), Qt::CaseInsensitive)) {
                sub.type = ScriptCmdType::ReadPos;
            } else if (trimmed.startsWith(QStringLiteral("DELAY("), Qt::CaseInsensitive)) {
                sub.type = ScriptCmdType::Delay;
                sub.delayMs = trimmed.mid(6, trimmed.indexOf(QChar::fromLatin1(')')) - 6).toInt();
            } else if (trimmed.startsWith(QStringLiteral("SERVO("), Qt::CaseInsensitive)) {
                // 统一格式（2026-06-15）：SERVO(ch=ang:spd, ...) 单/多路
                // 2026-06-16：扩展支持 SERVO(ch=ang:T<ms>) 时间模式（仿 KPZ32 出厂协议）
                // 已废弃：速度模式 SERVO(ch=ang:spd) 仍可解析但报警告
                int pe = trimmed.indexOf(QChar::fromLatin1(')'));
                QString inner = trimmed.mid(6, pe - 6).trimmed();
                if (inner.isEmpty() || !inner.contains(QChar::fromLatin1('='))) break;
                sub.type = ScriptCmdType::ServoMove;
                auto groups = inner.split(QChar::fromLatin1(','));
                for (const QString& g : groups) {
                    auto chanPart = g.split(QChar::fromLatin1('='));
                    if (chanPart.size() >= 2) {
                        int ch = chanPart[0].trimmed().toInt();
                        auto angSpd = chanPart[1].split(QChar::fromLatin1(':'));
                        float ang = angSpd.value(0, QString()).trimmed().toFloat();
                        QString paramStr = (angSpd.size() >= 2) ? angSpd[1].trimmed() : QString();
                        if (paramStr.startsWith(QLatin1Char('T'), Qt::CaseInsensitive)
                            || paramStr.startsWith(QLatin1Char('t'), Qt::CaseInsensitive)) {
                            // 时间模式
                            int ms = paramStr.mid(1).trimmed().toInt();
                            if (ms < 0) ms = 0;
                            sub.servoChannels.append(ch);
                            sub.servoAngles.append(ang);
                            sub.servoSpeeds.append(10.0f);
                            sub.servoTimes.append(ms);
                        } else {
                            // 速度模式（已废弃，保留解析但报警告）
                            emit consoleOutput(QStringLiteral("[警告] SERVO 速度模式已废弃: '%1' 应改写为 SERVO(ch=ang:T<ms>) 格式，例如 T2000。原文: %2")
                                .arg(paramStr, trimmed));
                            float spd = paramStr.isEmpty() ? 10.0f : paramStr.toFloat();
                            sub.servoChannels.append(ch);
                            sub.servoAngles.append(ang);
                            sub.servoSpeeds.append(spd);
                            sub.servoTimes.append(0);
                        }
                    }
                }
                if (!sub.servoChannels.isEmpty()) {
                    sub.servoChannel = sub.servoChannels.first();
                    sub.servoAngle   = sub.servoAngles.first();
                    sub.servoSpeed   = sub.servoSpeeds.first();
                }
            } else if (trimmed.startsWith(QStringLiteral("SET_DO("), Qt::CaseInsensitive)) {
                sub.type = ScriptCmdType::SetDO;
                auto parts = trimmed.mid(7, trimmed.indexOf(QChar::fromLatin1(')')) - 7).split(QChar::fromLatin1(','));
                if (parts.size() >= 2) { sub.ioIndex = parts[0].trimmed().toInt(); sub.ioValue = parts[1].trimmed().toInt(); }
            } else if (trimmed.startsWith(QStringLiteral("WAIT_DI("), Qt::CaseInsensitive)) {
                sub.type = ScriptCmdType::WaitDI;
                auto parts = trimmed.mid(8, trimmed.indexOf(QChar::fromLatin1(')')) - 8).split(QChar::fromLatin1(','));
                if (parts.size() >= 3) { sub.ioIndex = parts[0].trimmed().toInt(); sub.ioValue = parts[1].trimmed().toInt(); sub.ioTimeout = parts[2].trimmed().toInt(); }
            } else if (trimmed.startsWith(QStringLiteral("PRINT(\""), Qt::CaseInsensitive)) {
                sub.type = ScriptCmdType::Print;
                int q1 = trimmed.indexOf(QChar::fromLatin1('"'));
                int q2 = trimmed.lastIndexOf(QChar::fromLatin1('"'));
                if (q1 >= 0 && q2 > q1) sub.printText = trimmed.mid(q1+1, q2-q1-1);
        } else if (trimmed.startsWith(QStringLiteral("READ_POS"), Qt::CaseInsensitive)) {
            sub.type = ScriptCmdType::ReadPos;
        } else if (trimmed.startsWith(QStringLiteral("FOR("), Qt::CaseInsensitive)) {
            sub.type = ScriptCmdType::ForLoop;
            int pe = trimmed.indexOf(QChar::fromLatin1(')'));
            QString inner = trimmed.mid(4, pe - 4).trimmed();
            QStringList parts = inner.split(QChar::fromLatin1(','));
            if (parts.size() >= 3) {
                sub.varName   = parts[0].trimmed();
                sub.loopStart = parts[1].trimmed().toDouble();
                sub.loopEnd   = parts[2].trimmed().toDouble();
                sub.loopStep  = 1.0;
                sub.loopCount = -1;
            } else {
                sub.loopCount = inner.toInt();
            }
        } else if (trimmed.startsWith(QStringLiteral("END_FOR"), Qt::CaseInsensitive)) {
            sub.type = ScriptCmdType::EndFor;
        } else if (trimmed.startsWith(QStringLiteral("WHILE TRUE"), Qt::CaseInsensitive)) {
            sub.type = ScriptCmdType::WhileTrue;
        } else if (trimmed.startsWith(QStringLiteral("WHILE_DI("), Qt::CaseInsensitive)) {
            sub.type = ScriptCmdType::WhileDI;
            int pe = trimmed.indexOf(QChar::fromLatin1(')'));
            auto parts = trimmed.mid(9, pe - 9).split(QChar::fromLatin1(','));
            if (parts.size() >= 2) { sub.ioIndex = parts[0].trimmed().toInt(); sub.ioValue = parts[1].trimmed().toInt(); }
        } else if (trimmed.startsWith(QStringLiteral("END_WHILE"), Qt::CaseInsensitive)) {
            sub.type = ScriptCmdType::EndWhile;
            } else if (trimmed.contains(QChar::fromLatin1('=')) && !trimmed.startsWith(QChar::fromLatin1('#'))
                       && !trimmed.startsWith(QStringLiteral("SERVO("), Qt::CaseInsensitive)) {
                // 子块 SetVar 通用赋值（排除 SERVO(，原因同 parseScript 主表）
                sub.type = ScriptCmdType::SetVar;
                int eq = trimmed.indexOf(QChar::fromLatin1('='));
                sub.varName = trimmed.left(eq).trimmed();
                QString rhs = trimmed.mid(eq + 1).trimmed();
                bool isNum;
                sub.varValue = rhs.toDouble(&isNum);
                if (!isNum) sub.varExpr = rhs;
            } else {
                continue;
            }
            if (!executeCommand(sub)) return false;
        }
        return true;
    }
    case ScriptCmdType::ForLoop:
    case ScriptCmdType::EndFor:
    case ScriptCmdType::WhileDI:
    case ScriptCmdType::WhileTrue:
    case ScriptCmdType::EndWhile:
        return true;  // 由 executeNext 处理循环逻辑
    }
    return false;
}

/**
 * @brief 构建循环/分支的跳转映射表
 *
 * 预处理阶段调用，建立 FOR/END_FOR、WHILE/END_WHILE、IF_DI/END_IF 之间的索引映射，
 * 用于执行时快速跳转，避免重复搜索。
 *
 * 映射表说明：
 *   - m_loopEndMap:  循环/分支入口 → 对应结束位置
 *   - m_loopStartMap: 循环/分支结束 → 对应入口位置
 */
void TaskExecutor::buildLoopMap()
{
    m_loopEndMap.clear();
    m_loopStartMap.clear();
    QStack<int> forStack;
    QStack<int> whileStack;

    for (int i = 0; i < m_commands.size(); ++i) {
        switch (m_commands[i].type) {
        case ScriptCmdType::ForLoop:
            forStack.push(i);
            break;
        case ScriptCmdType::EndFor:
            if (!forStack.isEmpty()) {
                int head = forStack.pop();
                m_loopEndMap[head] = i;
                m_loopStartMap[i] = head;
            }
            break;
        case ScriptCmdType::WhileDI:
        case ScriptCmdType::WhileTrue:
            whileStack.push(i);
            break;
        case ScriptCmdType::EndWhile:
            if (!whileStack.isEmpty()) {
                int head = whileStack.pop();
                m_loopEndMap[head] = i;
                m_loopStartMap[i] = head;
            }
            break;
        case ScriptCmdType::IfDI: {
            // IF_DI → ELSE or END_IF（支持嵌套）
            int j = i + 1, depth = 1;
            while (j < m_commands.size() && depth > 0) {
                auto t = m_commands[j].type;
                if (t == ScriptCmdType::IfDI) depth++;
                else if (t == ScriptCmdType::EndIf) depth--;
                j++;
            }
            if (j < m_commands.size()) m_loopEndMap[i] = j - 1;
            break;
        }
        default: break;
        }
    }
}

/**
 * @brief 阻塞等待一组舵机通道全部到位（或超时）
 *
 * 在 SERVO / SERVO_S 指令中调用，避免连续下发时上一条还没转到位就被覆盖。
 * - 通道集合可指定（SERVO 1 个，SERVO_S 多个）
 * - 等待期间每 20ms 轮询一次；期间支持暂停（m_state 变非 Running 时立即返回）
 * - 超时后发出 [WARN] 日志并继续执行（不视为失败）
 *
 * @param servo     舵机控制器指针
 * @param channels  目标通道列表（0-5）
 * @param timeoutMs 超时毫秒（默认 30 秒）
 */
void TaskExecutor::waitForServoIdle(ServoController* servo,
                                    const QList<int>& channels,
                                    int timeoutMs)
{
    if (!servo || channels.isEmpty()) return;

    QElapsedTimer timer;
    timer.start();

    while (true) {
        // 暂停/停止：立即返回
        if (m_state != Running) {
            emit consoleOutput(QStringLiteral("[SERVO] 等待被中断（状态已变更）"));
            return;
        }
        // 检查所有通道是否都已不再移动
        bool allIdle = true;
        for (int ch : channels) {
            if (servo->isMoving(ch)) { allIdle = false; break; }
        }
        if (allIdle) return;
        // 超时
        if (timer.elapsed() > timeoutMs) {
            emit consoleOutput(QStringLiteral("[WARN] 舵机等待超时 (%1ms)，继续执行")
                .arg(timeoutMs));
            return;
        }
        QThread::msleep(20);
        QCoreApplication::processEvents();
    }
}

/**
 * @brief 解析值：尝试将字符串转换为数字，失败则从变量表查找
 *
 * @param raw  原始字符串（可能是数字或变量名）
 * @param vars 变量表
 * @return 解析后的数值（未找到则返回 0.0）
 */
double TaskExecutor::resolveValue(const QString& raw, const QMap<QString, double>& vars)
{
    bool ok;
    double val = raw.toDouble(&ok);
    if (ok) return val;
    return vars.value(raw, 0.0);
}

/**
 * @brief 简易表达式求值器（支持 +、-、*、/）
 *
 * 运算优先级：先乘除后加减
 * 支持的操作数：数字常量或变量名
 *
 * 示例：
 *   "x+5"      → 变量 x 的值 + 5
 *   "count*2"  → 变量 count 的值 * 2
 *   "100/5"    → 100 / 5 = 20
 *
 * @param expr 表达式字符串
 * @param vars 变量表
 * @return 计算结果
 */
double TaskExecutor::evaluateExpr(const QString& expr, const QMap<QString, double>& vars)
{
    QString s = expr.trimmed();
    // 去掉所有空格
    s.replace(QChar::fromLatin1(' '), QString());

    // 检查乘除运算符（优先计算）
    for (int i = 0; i < s.size(); ++i) {
        QChar ch = s[i];
        if (ch == QChar::fromLatin1('*') || ch == QChar::fromLatin1('/')) {
            double left  = resolveValue(s.left(i), vars);
            double right = resolveValue(s.mid(i + 1), vars);
            return (ch == QChar::fromLatin1('*')) ? (left * right) : (left / right);
        }
    }

    // 检查加减运算符
    for (int i = 1; i < s.size(); ++i) {
        QChar ch = s[i];
        if (ch == QChar::fromLatin1('+') || ch == QChar::fromLatin1('-')) {
            // 减号前面是数字或变量（不是负号）
            if (!s[i-1].isDigit() && !s[i-1].isLetter() && s[i-1] != QChar::fromLatin1('_') && s[i-1] != QChar::fromLatin1('$'))
                continue;
            double left  = resolveValue(s.left(i), vars);
            double right = resolveValue(s.mid(i + 1), vars);
            return (ch == QChar::fromLatin1('+')) ? (left + right) : (left - right);
        }
    }

    // 没有运算符 → 单个数字或变量
    return resolveValue(s, vars);
}
