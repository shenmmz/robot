/*
 * @file TaskExecutor.h
 * @brief 任务执行器——管理脚本指令队列的逐步执行、暂停、恢复与停止。
 *
 * 支持 MOVEJ/MOVEL/WAIT_IDLE/DELAY/SET_DO/WAIT_DI/PRINT/CALL 等指令。
 * 每步执行完成后通过信号通知进度，支持暂停（急停+保留队列）、恢复（继续）、停止（清空）。
 */
#ifndef TASKEXECUTOR_H
#define TASKEXECUTOR_H

#include <QObject>
#include <QList>
#include <QTimer>
#include <QStack>
#include <QHash>
#include <QMap>
#include "ScriptCommand.h"
#include "TargetDef.h"   // MotionParams:点位级 speed/accel/decel/timeoutMs

class RobotController;
class ServoController;

class TaskExecutor : public QObject
{
    Q_OBJECT

public:
    enum State { Idle, Running, Paused };

    explicit TaskExecutor(RobotController* robotCtrl, QObject* parent = nullptr);
    ~TaskExecutor();

    // ===== 任务管理 =====
    /// 设置指令列表（替代旧的 setTaskList）
    void setCommands(const QList<ScriptCommand>& commands);
    /// 单步模式首次进入时设置起点（仅 m_currentIdx）
    void setStartIndex(int idx) { m_currentIdx = idx; }
    void start();
    void pause();
    void resume();
    void stop();

    /// 单步执行：先准备好任务（设指令列表），仅执行一条命令后停在 Idle。
    /// 等价于内部把 m_state 设为 Running，但 executeNext 在执行完一条后清掉 Running 标志，不再续行。
    /// 重复调用 stepOnce() 会从当前指令后继续往下走一条。
    void stepOnce();

    /// 重置到起始位置：m_currentIdx=0、清变量表、清循环栈。用于单步模式走完末尾后"从头再来"。
    void reset();

    // ===== 参数 =====
    void setSpeed(double s)  { m_speed = s; }
    void setAccel(double a)  { m_accel = a; }
    void setDecel(double d)  { m_decel = d; }
    void setTimeoutMs(int ms) { m_timeoutMs = ms; }
    void setTargets(const QHash<QString, QList<double>>& targets) { m_targets = targets; }
    /// @brief 设置点位级运动参数(speed/accel/decel/timeoutMs)
    /// 运行时优先级:点位 motion > 全局 SPEED/ACCEL/DECEL/TIMEOUT > 行级 cmd.speed > 自动估算
    void setTargetMotion(const QHash<QString, MotionParams>& motion) { m_targetMotion = motion; }
    void setScriptName(const QString& name) { m_scriptName = name; }
    /// 设置主脚本完整路径，CALL 子脚本时优先在主脚本同目录查找
    void setScriptPath(const QString& path) { m_scriptPath = path; }

    // ===== 状态查询 =====
    State state() const { return m_state; }
    bool isRunning() const { return m_state == Running; }
    bool isPaused() const { return m_state == Paused; }
    int currentIndex() const { return m_currentIdx; }
    int totalCount() const { return m_commands.size(); }

signals:
    void taskStarted(int total);
    void taskProgress(int current, int total);
    void taskPaused(int current, int total);
    void taskResumed(int current, int total);
    void taskFinished();
    void taskError(const QString& msg);
    /// 控制台输出（PRINT 指令等）
    void consoleOutput(const QString& text);
    /// 当前正在执行的源代码行号（0-based，编辑器用于高亮+绿色圆点）
    void currentLineChanged(int line);

private slots:
    void executeNext();

private:
    /// 执行单条指令，返回是否成功
    bool executeCommand(const ScriptCommand& cmd);

    /// 预计算循环跳转表（FOR→END_FOR, WHILE_DI→END_WHILE）
    void buildLoopMap();

    /// 解析变量值（5 → 5.0, a → m_variables["a"]）
    static double resolveValue(const QString& raw, const QMap<QString, double>& vars);

    /// 求值表达式（支持 + - * / 四则运算和变量替换）
    static double evaluateExpr(const QString& expr, const QMap<QString, double>& vars);

    /// 单步/正常模式统一的续行调度：单步则停在 Idle，正常则 singleShot 排队 executeNext
    void scheduleNext();

    /// 阻塞等待一组舵机通道全部到位（支持暂停 / 超时）
    void waitForServoIdle(ServoController* servo, const QList<int>& channels, int timeoutMs = 30000);

    /// 解析 TARGET 行 '}' 之后的 #speed=N acc=N dec=N tmo=N 运动参数后缀
    /// （与 UiController::parseMotionSuffix 语义一致，供 CALL 子脚本解析复用）
    MotionParams parseMotionSuffix(const QString& tail);

    /// 解析 MOVE 指令内联运动参数（逗号分隔 key=value 格式，如 speed=50, acc=50, dec=60）
    /// 写入 ScriptCommand 的 speed/accel/decel/timeoutMs，供 CALL 子脚本引用式写法复用
    void parseMotionSuffixInto(const QString& inner, ScriptCommand& cmd);

    /// 打印单条 MOVE 指令的 TARGET 点位 + SPD/ACC/DEC + 源码行 + ▶ 前缀行。
    /// 主脚本 executeNext() 与 CALL 子脚本执行循环共用，保证两套路径日志格式一致。
    /// @param cmd   待打印的指令（MoveJ/MoveL 才输出 TARGET/SPD/ACC/DEC）
    /// @param idx        当前指令序号（从 1 开始）
    /// @param total      指令总数
    /// @param scriptName 当前脚本文件名（主脚本或 CALL 子脚本名），追加到 ▶ 行末尾
    void printMoveCommand(const ScriptCommand& cmd, int idx, int total, const QString& scriptName);

    RobotController* m_robotController;
    State m_state = Idle;

    QList<ScriptCommand> m_commands;
    int m_currentIdx = 0;

    /// 变量表: 变量名 → 值
    QMap<QString, double> m_variables;

    /// 运行时全局默认参数（SPEED/ACCEL/DECEL/TIMEOUT 指令设置）
    double m_globalSpeed = -1;
    double m_globalAccel = -1;
    double m_globalDecel = -1;
    int    m_globalTimeout = -1;

    /// 循环跳转表: key=循环头索引, value=循环尾索引
    QHash<int, int> m_loopEndMap;    // FOR→END_FOR / WHILE_DI→END_WHILE
    QHash<int, int> m_loopStartMap;  // END_FOR→FOR / END_WHILE→WHILE_DI

    /// 循环栈: {循环头索引, 剩余次数}
    struct LoopFrame { int headIdx; int remaining; };
    QStack<LoopFrame> m_loopStack;

    double m_speed   = 25.0;
    double m_accel   = 50.0;
    double m_decel   = 60.0;
    int m_timeoutMs  = 10000;

    /// 点位表: 目标名 → 6个坐标值（来自 UiController）
    QHash<QString, QList<double>> m_targets;
    /// 点位级运动参数表: 目标名 → MotionParams（来自 UiController）
    /// 运行时在 MoveJ / MoveL 分支查询,优先级高于 m_globalXxx
    QHash<QString, MotionParams> m_targetMotion;
    QString m_scriptName;
    /// 主脚本完整路径（用于 CALL 子脚本时在同目录查找）
    QString m_scriptPath;

    /// 单步模式：executeNext 在执行完一条命令后清掉 Running，不再续行
    bool m_stepOnce = false;
};

#endif // TASKEXECUTOR_H
