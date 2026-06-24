/*
 * @file ScriptCommand.h
 * @brief 定义脚本指令数据结构，支持 MOVEJ/MOVEL/WAIT_IDLE/DELAY/IO/PRINT/CALL 等指令类型。
 *
 * 每个 ScriptCommand 代表脚本中的一行指令，包含类型和该类型所需的所有参数。
 * TaskExecutor 通过 QList<ScriptCommand> 统一管理并逐步执行。
 */
#ifndef SCRIPTCOMMAND_H
#define SCRIPTCOMMAND_H

#include <QString>
#include <QList>

/// 脚本指令类型
enum class ScriptCmdType {
    MoveJ,      // 关节空间运动
    MoveL,      // 笛卡尔直线运动
    WaitIdle,   // 等待所有轴停止
    Delay,      // 延时毫秒
    SetDO,      // 设置数字输出
    WaitDI,     // 等待数字输入
    ReadPos,    // 读取当前位置到控制台
    Print,      // 打印到控制台
    Call,       // 调用子脚本文件
    SetVar,     // 变量赋值: a = 100
    ForLoop,    // FOR 循环开始
    EndFor,     // FOR 循环结束
    WhileDI,    // WHILE_DI 循环开始（等待 DI 条件）
    WhileTrue,  // WHILE TRUE 无条件循环
    EndWhile,   // WHILE 循环结束
    SpeedSet,   // SPEED(v)  设定全局默认速度
    AccelSet,   // ACCEL(v) 设定全局默认加速度
    DecelSet,   // DECEL(v) 设定全局默认减速度
    TimeoutSet, // TIMEOUT(ms) 设定全局默认超时
    MoveOff,    // MOVEOFF(j0..j5) 关节相对偏移
    MoveLOff,   // MOVELOFF(x..rz) 笛卡尔相对偏移
    IfDI,       // IF_DI(n, val) 条件判断
    Else,       // ELSE 分支
    EndIf,      // END_IF 结束
    TargetDef,  // TARGET P1 = {...} 点位定义
    ServoMove,  // SERVO(ch=ang:spd, ch=ang:spd, ...) 单/多路统一（2026-06-15 合并）
};

/// 脚本单行指令，包含类型和参数
struct ScriptCommand {
    ScriptCmdType type = ScriptCmdType::MoveJ;

    // ===== 运动指令参数 (MoveJ / MoveL) =====
    QList<double> targets;       // 6个目标位置（关节角度 或 笛卡尔坐标）
    QStringList targetRaw;       // targets的原始数值字符串（用于执行时解析变量）
    QString targetName;          // 目标名称（如"P1"，非空时从m_targets查找）
    double speed   = 25.0;      // 速度
    double accel   = 50.0;      // 加速度
    double decel   = 60.0;      // 减速度
    int    timeoutMs = 10000;   // 等待到位超时（毫秒）

    // ===== IO 指令参数 (SetDO / WaitDI) =====
    int ioIndex   = 0;          // IO 编号 (0-19)
    int ioValue   = 0;          // IO 值 (0/1)
    int ioTimeout = 5000;       // WaitDI 超时

    // ===== Delay 参数 =====
    int delayMs   = 500;        // 延时毫秒

    // ===== Print 参数 =====
    QString printText;          // 打印文本

    // ===== Call 参数 =====
    QString callFile;           // 子脚本文件名

    // ===== 循环参数 (ForLoop / WhileDI) =====
    int loopCount = 0;          // FOR 循环次数（兼容旧 FOR(n) 语法）
    double loopStart = 0;       // FOR(var, start, end, step) 起始值
    double loopEnd   = 0;       // 结束值
    double loopStep  = 1;       // 步长（默认 1）

    // ===== SetVar 参数 =====
    QString varName;            // 变量名
    double  varValue = 0;       // 常量值（纯数字时）
    QString varExpr;            // 表达式字符串（含变量时，如 "x + 10"）

    /// 原始脚本文本行（用于控制台回显）
    QString sourceLine;

    /// 源文件中的行号（0-based，对应编辑器行号；非指令行如空行/注释的指令不发出高亮）
    int sourceLineNo = -1;

    // ===== 舵机参数 (ServoMove) =====
    // 2026-06-15 语法合并：单/多路统一用 ServoMove，单路时 servoChannels.size()==1。
    // 旧单字段 servoChannel/Angle/Speed 保留为冗余（解析时填入首通道等价值），
    // 避免删除时引发未预期的引用错误。
    int servoChannel = 0;               // [冗余] 旧单舵机字段；首通道的等价值
    float servoAngle = 90.0f;           // [冗余] 首通道角度
    float servoSpeed = 10.0f;           // [冗余] 首通道速度
    QList<int> servoChannels;           // 通道列表（单路时 size=1）
    QList<float> servoAngles;           // 角度列表
    QList<float> servoSpeeds;           // 速度列表（与通道一一对应，°/s）
    QList<int>   servoTimes;            // 时间列表（毫秒；与通道一一对应，0 表示走速度分支；新语法 SERVO(ch=ang:Tms) 使用）
};

#endif // SCRIPTCOMMAND_H
