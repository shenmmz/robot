/*
 * @file RobotModels.cpp
 * @brief RobotModelSpec / RobotModelRegistry 的实现，及 R01 机型参数定义。
 *
 * R01 即原 Robot_R06 的参数，齿轮链/速度/传感器/回零/IP 等归总，
 * 作为型号管理的唯一代码侧数据源；FRAME6 几何为固定结构，仍保留在 RobotKinematics 硬编码。
 */
#include "RobotModels.h"

namespace {

// 齿轮比 "1:4" -> 分母 4；解析失败返回 1
int gearDenominator(const QString& g)
{
    QStringList parts = g.split(QLatin1Char(':'));
    if (parts.size() == 2) {
        bool ok = false;
        int denom = parts.at(1).toInt(&ok);
        if (ok && denom > 0)
            return denom;
    }
    return 1;
}

// 构建 R01 机型（FRAME6 几何由 RobotKinematics 硬编码，此处不含）
RobotModelSpec buildR01()
{
    RobotModelSpec s;
    s.id = RobotModelId::R01;
    s.name = QStringLiteral("Robot_R01");
    s.axisCount = 6;
    s.homePosition = QList<double>{84.999, 68.620, -51.944, 4.229, 71.000, 90.000};
    s.homeMoveSpeed = 60;
    s.homeMoveAccel = 80;
    s.homeMoveDecel = 100;
    s.defaultControllerIP = QStringLiteral("192.168.0.123");
    s.stateUpdateIntervalMs = 100;

    // 每轴齿轮链 / 传感器
    // 列：pulsePerRev, reductionRatio, gearRatio, sensors
    s.axes = QList<AxisSpec>{
        {1000, 50, QStringLiteral("1:4"), QStringList{QStringLiteral("IN0"), QStringLiteral("IN1")}},  // J0
        {1000, 50, QStringLiteral("1:4"), QStringList{QStringLiteral("IN2")}},                           // J1
        {1000, 30, QStringLiteral("1:1"), QStringList{QStringLiteral("IN3")}},                           // J2
        {1000, 50, QStringLiteral("1:1"), QStringList{QStringLiteral("IN4"), QStringLiteral("IN5")}},  // J3
        {1000, 30, QStringLiteral("1:1"), QStringList{QStringLiteral("IN6")}},                           // J4
        {1000, 10, QStringLiteral("1:1"), QStringList{QStringLiteral("IN7"), QStringLiteral("IN8")}},  // J5
    };
    return s;
}

// R01 规格单例（静态缓存，全程唯一）
const RobotModelSpec& r01Spec()
{
    static const RobotModelSpec s = buildR01();
    return s;
}

// 空规格（R02/Unknown 占位，参数待填）
const RobotModelSpec& emptySpec()
{
    static const RobotModelSpec s;
    return s;
}

} // namespace

// 由齿轮链推导脉冲当量：pulsePerRev × reductionRatio × 齿轮比分母 / 360
QList<double> RobotModelSpec::computeAxisUnits() const
{
    QList<double> u;
    u.reserve(axes.size());
    for (const AxisSpec& a : axes) {
        double v = a.pulsePerRev * a.reductionRatio
                   * gearDenominator(a.gearRatio) / 360.0;
        u.append(v);
    }
    return u;
}

// 按机型 ID 取规格（R02/Unknown 返回空规格）
const RobotModelSpec& RobotModelRegistry::get(RobotModelId id)
{
    switch (id) {
    case RobotModelId::R01:
        return r01Spec();
    case RobotModelId::R02:
        // 新臂占位：参数待填，暂返回空 spec（后续在此填充 buildR02()）
        return emptySpec();
    case RobotModelId::Unknown:
    default:
        return emptySpec();
    }
}

// 所有已知机型（用于 UI 下拉）
QList<RobotModelId> RobotModelRegistry::allModels()
{
    return QList<RobotModelId>{RobotModelId::R01, RobotModelId::R02};
}
