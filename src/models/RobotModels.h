/*
 * @file RobotModels.h
 * @brief 机械臂型号规格与注册表。
 *
 * 数据驱动：不同机型只有参数差异，没有行为差异；Flash 只存 ID，整机参数按 ID 查表。
 * 公式：UNITS = pulsePerRev × reductionRatio × 齿轮比分母 / 360
 * 当前：R01（原 Robot_R06）；R02 占位，参数待填。
 */
#pragma once

#include <QString>
#include <QList>
#include "src/models/RobotModelId.h"
struct HomeSpec {
    // ---- IO 配置 ----
    int    homeIn = -1;          // 原点传感器输入 IO 编号（-1=未配置，跳过原点检测）
    int    negIn  = -1;          // 反向限位传感器输入 IO 编号（-1=该轴无反向限位）

    // ---- 回零模式 ----
    int    datumMode = 3;        // 回零模式（3=正向限位回零，4=反向限位回零）

    // ---- 移开距离：回零前先离开限位，避免轴已停在传感器上导致找原点失败 ----
    double moveAwayOrigin  = 0;  // 原点触发时相对移开距离 (°)：正=正向移，负=反向移
    double moveAwayReverse = 0;  // 反向限位触发时相对移开距离 (°)；0=无反向限位时保持 0

    // ---- 回零运动参数 ----
    double homeSpeed  = 30;      // 回零高速 (°/s)：寻找原点时的主运行速度
    double homeAccel  = 80;      // 回零加速度 (°/s²)
    double homeDecel  = 100;     // 回零减速度 (°/s²)
    double homeLspeed = 0;       // 起始速度 (°/s)：启动瞬间的低速（抑制起步抖动/过冲）
    double homeSramp  = 200;     // S 曲线过渡时间 (ms)：加减速更平滑
    double homeCreep  = 1;       // 爬行速度 (°/s)：压住原点后的低速寻点速度
};

// 单轴规格
struct AxisSpec {
    int         pulsePerRev = 1000;
    int         reductionRatio = 1;
    QString     gearRatio = QStringLiteral("1:1");
    HomeSpec    home;                // 回零配置（嵌套）
    double      maxSpeed = 360;       // °/s
    double      maxAccel = 1000;      // °/s²
    double      maxDecel = 1000;      // °/s²

    AxisSpec() = default;
    AxisSpec(int pr, int rr, const QString& gr, const HomeSpec& h = HomeSpec{},
             double ms = 360, double ma = 1000, double md = 1000)
        : pulsePerRev(pr), reductionRatio(rr), gearRatio(gr), home(h),
          maxSpeed(ms), maxAccel(ma), maxDecel(md) {}
};

// 整机型号规格（FRAME6 几何保留在 RobotKinematics）
// 字段默认值集中在 RobotModels.cpp::buildSharedDefaults()，避免双定义漂移
struct RobotModelSpec {
    RobotModelId id = RobotModelId::Unknown;  // emptySpec() 用此默认
    QString      name;
    int          axisCount;                   // 通常 6

    QList<AxisSpec> axes;
    QList<double>   homePosition;             // Flash 为空时作种子
    QList<double>   shutdownPosition;         // 关机安全位姿（6轴关节角，°），纯 spec 不写 Flash

    double       homeMoveSpeed;               //回原点速度 °/s
    double       homeMoveAccel;              //加速度 °/s²
    double       homeMoveDecel;             // °减速度 /s²
    bool         moveToHomeAfterHoming;     // 回零成功后是否移动到 homePosition（false=停 DATUM 位只清坐标）

    QString      defaultControllerIP;      // 默认控制器 IP
    int          stateUpdateIntervalMs;     // 状态更新周期，单位 ms

    QList<double> computeAxisUnits() const;  // 计算每个轴的单位数
};

// 型号注册表
class RobotModelRegistry {
public:
    static const RobotModelSpec& get(RobotModelId id);   // Unknown 返回空 spec
    static QList<RobotModelId>   allModels();            // 用于 UI 下拉
};

// 出厂原点：单一数据源，委托 R01.homePosition（Flash 为空时 seed 一次）
inline QList<double> initialDeviceOrigin()
{
    return RobotModelRegistry::get(RobotModelId::R01).homePosition;
}
