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
#include <QStringList>
#include <QList>
#include "src/models/RobotModelId.h"

// 单轴规格
struct AxisSpec {
    int         pulsePerRev = 1000;
    int         reductionRatio = 1;
    QString     gearRatio = QStringLiteral("1:1");
    double      maxSpeed = 360;       
    double      maxAccel = 1000;     
    double      maxDecel = 1000;      
    QStringList sensors;              // IN0..IN8
/* 单轴传感器配置 */
    AxisSpec() = default;
    AxisSpec(int pr, int rr, const QString& gr, const QStringList& sn,
             double ms = 360, double ma = 1000, double md = 1000)
        : pulsePerRev(pr), reductionRatio(rr), gearRatio(gr),
          maxSpeed(ms), maxAccel(ma), maxDecel(md), sensors(sn) {}
};

// 整机型号规格（FRAME6 几何保留在 RobotKinematics）
// 字段默认值集中在 RobotModels.cpp::buildSharedDefaults()，避免双定义漂移
struct RobotModelSpec {
    RobotModelId id = RobotModelId::Unknown;  // emptySpec() 用此默认
    QString      name;
    int          axisCount;                   // 通常 6

    QList<AxisSpec> axes;
    QList<double>   homePosition;             // Flash 为空时作种子

    double       homeMoveSpeed;               //回原点速度 °/s
    double       homeMoveAccel;              //加速度 °/s²
    double       homeMoveDecel;             // °减速度 /s²

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
