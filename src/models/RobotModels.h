/*
 * @file RobotModels.h
 * @brief 机械臂型号规格与注册表（型号管理的数据源）。
 *
 * 设计：
 * - 不同机型之间只有"参数差异"，没有"行为差异"，因此用
 *   「数据 + 注册表」而非「每机型一个子类」。
 * - 控制器 Flash 只存型号枚举值（见 RobotModelId.h），完整参数在此处代码侧，
 *   由 RobotModelRegistry::get(id) 按 ID 查表得到。
 * - computeAxisUnits() 用齿轮链公式推导脉冲当量，使改减速比/齿轮比能自动生效：
 *       UNITS = pulsePerRev × reductionRatio × 齿轮比分母 / 360
 *   经验证：R01 复算结果与原始脉冲当量规格完全一致
 *   (J0/J1=555.56, J2/J4=83.33, J3=138.89, J5=27.78)。
 *
 * 当前已归类：R01（即原 Robot_R06 的全部参数）。R02 为新增臂占位，参数待填。
 */
#pragma once

#include <QString>
#include <QStringList>
#include <QList>
#include "src/models/RobotModelId.h"

// 单轴规格：齿轮链 + 运动限制 + 回零/限位传感器
struct AxisSpec {
    int         pulsePerRev = 1000;   // 每转脉冲数
    int         reductionRatio = 1;   // 减速器比（如 50）
    QString     gearRatio = QStringLiteral("1:1");  // 前置齿轮比（如 "1:4"）
    double      maxSpeed = 360;       // 最大速度 (°/s)
    double      maxAccel = 1000;      // 最大加速度 (°/s²)
    double      maxDecel = 1000;      // 最大减速度 (°/s²)
    QStringList sensors;              // 回零/限位传感器映射 (IN0..IN8)

    AxisSpec() = default;   // 默认构造（保持可默认初始化）
    // 构造单轴规格：速度/加减速带默认值，与原配置一致
    AxisSpec(int pr, int rr, const QString& gr, const QStringList& sn,
             double ms = 360, double ma = 1000, double md = 1000)
        : pulsePerRev(pr), reductionRatio(rr), gearRatio(gr),
          maxSpeed(ms), maxAccel(ma), maxDecel(md), sensors(sn) {}
};

// 整机型号规格：原分散在各常量定义中的参数统一收口（FRAME6 几何保留在 RobotKinematics）
struct RobotModelSpec {
    RobotModelId id = RobotModelId::Unknown;
    QString      name;                // 机型名（如 "Robot_R01"）
    int          axisCount = 6;

    QList<AxisSpec> axes;             // 每轴规格，长度 = axisCount
    QList<double>   homePosition;     // 回零后目标位置（6 关节角），Flash 为空时作种子

    double       homeMoveSpeed = 60;  // 回零后移动速度 (°/s)
    double       homeMoveAccel = 80;  // 回零后移动加速度
    double       homeMoveDecel = 100; // 回零后移动减速度

    // FRAME6 几何为固定机械结构，保留在 RobotKinematics 硬编码，此处不收口

    QString      defaultControllerIP = QStringLiteral("192.168.0.123");
    int          stateUpdateIntervalMs = 100;

    // 由齿轮链推导脉冲当量：UNITS = pulsePerRev × reductionRatio × 齿轮比分母 / 360
    QList<double> computeAxisUnits() const;
};

// 型号注册表：枚举 ID -> 具体规格
class RobotModelRegistry {
public:
    // 返回对应机型的规格（Unknown / 未实现机型返回空 spec）
    static const RobotModelSpec& get(RobotModelId id);
    // 所有已知机型（用于 UI 下拉列出）
    static QList<RobotModelId>   allModels();
};
