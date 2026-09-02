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

// 提取共享默认字段：axisCount / homePosition / homeMove / 默认 IP / 状态间隔
RobotModelSpec buildSharedDefaults()
{
    RobotModelSpec s;
    s.axisCount = 6; // 6 轴
    s.homePosition = QList<double>{84.999, 68.620, -51.944, 4.229, 71.000, 90.000};
    s.homeMoveSpeed = 60; // 回零后移动速度 (°/s)
    s.homeMoveAccel = 80; // 回零后移动加速度
    s.homeMoveDecel = 100; // 回零后移动减速度
    s.defaultControllerIP = QStringLiteral("192.168.0.123");
    s.stateUpdateIntervalMs = 100; // 状态更新间隔(ms)
    return s;
}

// 构建 R01 机型（FRAME6 几何由 RobotKinematics 硬编码，此处不含）
RobotModelSpec buildR01()
{
    RobotModelSpec s = buildSharedDefaults();
    s.id = RobotModelId::R01;
    s.name = QStringLiteral("Robot_R01");
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

// 构建 R02 机型（同 FRAME6 几何，电机脉冲与减速比由新机型指定）
RobotModelSpec buildR02()
{
    RobotModelSpec s = buildSharedDefaults();
    s.id = RobotModelId::R02;
    s.name = QStringLiteral("Robot_R02");

    // 每轴齿轮链 / 传感器
    // 列：pulsePerRev, reductionRatio, gearRatio, sensors
    // 按 R02 要求：电机脉冲为 10000，减速比固定为 28
    s.axes = QList<AxisSpec>{
        {10000, 28, QStringLiteral("1:4"), QStringList{QStringLiteral("IN0"), QStringLiteral("IN1")}},  // J0
        {10000, 28, QStringLiteral("1:4"), QStringList{QStringLiteral("IN2")}},                           // J1
        {10000, 28, QStringLiteral("1:1"), QStringList{QStringLiteral("IN3")}},                           // J2
        {10000, 28, QStringLiteral("1:1"), QStringList{QStringLiteral("IN4"), QStringLiteral("IN5")}},  // J3
        {10000, 28, QStringLiteral("1:1"), QStringList{QStringLiteral("IN6")}},                           // J4
        {10000, 28, QStringLiteral("1:1"), QStringList{QStringLiteral("IN7"), QStringLiteral("IN8")}},  // J5
    };
    return s;
}

// R01 规格单例（静态缓存，全程唯一）
const RobotModelSpec& r01Spec()
{
    static const RobotModelSpec s = buildR01();
    return s;
}

// R02 规格单例（静态缓存，全程唯一）
const RobotModelSpec& r02Spec()
{
    static const RobotModelSpec s = buildR02();
    return s;
}

// 空规格（R02/Unknown 占位，参数待填）
// 注：const 聚合对象必须显式初始化（值初始化），字段为 0/空，
//   但实际不会读这些字段 —— 见 RobotConfig::initializeFromModel 对 Unknown 的 R01 回退
const RobotModelSpec& emptySpec()
{
    static const RobotModelSpec s{};
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
        return r02Spec();
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
