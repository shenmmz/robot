/*
 * @file TargetDef.h
 * @brief TARGET 点位扩展数据模型
 *
 * 每个 TARGET 点位除原有的 6 轴坐标外,还可保存:
 *   1) motion:该点位作为 MOVEJ/MOVEL 默认参数时使用的 speed/acc/dec/tmo
 *   2) servos[6]:6 路 PWM 舵机的目标位置快照(角度+速度+时间)
 *   全部 0 / -1 表示"未设",运行时会落到下一层(全局/默认值)
 *
 * 这个头文件独立于 UiController / TaskExecutor,避免循环 include。
 */
#ifndef TARGETDEF_H
#define TARGETDEF_H

#include <QList>
#include <QString>
#include <QStringList>

/// 单个 TARGET 的运动参数默认值(0 = 未设)
struct MotionParams {
    double speed    = 0.0;   // °/s
    double accel    = 0.0;   // °/s²
    double decel    = 0.0;   // °/s²
    int    timeoutMs = 0;    // ms
    bool   hasAny() const { return speed > 0 || accel > 0 || decel > 0 || timeoutMs > 0; }
};

/// 6 路 PWM 舵机槽位(channel = -1 表示禁用)
struct ServoSlot {
    int    channel = -1;     // 0..5
    double angle   = 0.0;    // 0..180
    double speed   = 0.0;    // °/s,1..360
    int    timeMs  = 0;      // >0 = 时间模式;0 = 速度模式
    bool   enabled() const { return channel >= 0; }
};

/// 完整 TARGET 定义(替换原来的 QList<double> 6 轴)
struct TargetDef {
    QList<double> joints;        // 6 个坐标(J0..J5 或 X..C)
    QStringList   jointLabels;   // 轴标签,顺序与 joints 一致(用于回写时保持)
    MotionParams  motion;        // 运动默认值
    ServoSlot     servos[6];     // 6 路舵机快照
    QString       comment;       // 行尾 // 注释

    int servoCount() const {
        int n = 0;
        for (const auto& s : servos) if (s.enabled()) ++n;
        return n;
    }
};

#endif // TARGETDEF_H