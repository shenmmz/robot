/*
 * @file RobotConfig.cpp
 * @brief RobotConfig 实现：按机型规格（RobotModelRegistry）构建机械臂配置。
 *
 * 数据单一来源在 RobotModels.cpp
 */
#include "RobotConfig.h"
#include "RobotModels.h"
#include <QDebug>          // qInfo / qWarning 流式输出
#include <QSettings>        // 回零覆盖表 ini 读写

/**
 * @brief 构造函数：默认按 R01 机型规格构建（数据源自 RobotModelRegistry）
 */
RobotConfig::RobotConfig()
{
    // 字段全部由 initializeFromModel(R01) 填充，不要在初始化列表写死默认值
    initializeFromModel(RobotModelId::R01);
}

/**
 * @brief 取指定轴配置；索引越界返回默认空配置
 */
RobotConfig::AxisConfig RobotConfig::getAxisConfig(int axisIndex) const
{
    if (axisIndex >= 0 && axisIndex < m_axisConfigs.size()) {
        return m_axisConfigs.at(axisIndex);
    }
    return AxisConfig();
}

/**
 * @brief 取关节轴号列表 (0~5)，供 CONNFRAME 建模
 */
QList<int> RobotConfig::getJointAxisList() const
{
    QList<int> list;
    for (int i = 0; i < m_axisCount; ++i) {
        list.append(i);
    }
    return list;
}

/**
 * @brief 取虚拟轴号列表 (6~11)，供 CONNFRAME 建模
 */
QList<int> RobotConfig::getVirtualAxisList() const
{
    QList<int> list;
    for (int i = 0; i < m_axisCount; ++i) {
        list.append(m_axisCount + i);
    }
    return list;
}

/**
 * @brief 取各轴脉冲当量列表（关节端每度脉冲数）
 *
 * 实时调用机型注册表公式推导，数据源在 RobotModels.cpp（computeAxisUnits），
 * 不缓存（改减速比/齿轮比自动生效）。
 */
QList<double> RobotConfig::getAxisUnitsList() const
{
    return RobotModelRegistry::get(m_modelId).computeAxisUnits();
}

/**
 * @brief 取指定轴的型号规格（含 home 回零配置）
 *
 * 直接转发到 RobotModelRegistry；不缓存（机型切换时自动跟随）。
 */
const AxisSpec& RobotConfig::getAxisSpec(int axisIndex) const
{
    return RobotModelRegistry::get(m_modelId).axes.at(axisIndex);
}

// ==================== 运行时回零参数覆盖（HomeOverride） ====================

namespace {
// 覆盖表键：(modelId 高16位) << 16 | axisIndex 低16位
quint64 homeKeyOf(RobotModelId id, int axisIndex)
{
    return (static_cast<quint64>(static_cast<int>(id)) << 16)
           | static_cast<quint64>(static_cast<unsigned>(axisIndex) & 0xFFFFu);
}

// 把 HomeSpec 序列化为 ini 组键值（字段与 HomeSpec 成员一一对应）
void writeHomeSpec(QSettings& s, const HomeSpec& h)
{
    s.setValue(QStringLiteral("homeIn"), h.homeIn);
    s.setValue(QStringLiteral("negIn"), h.negIn);
    s.setValue(QStringLiteral("datumMode"), h.datumMode);
    s.setValue(QStringLiteral("moveAwayOrigin"), h.moveAwayOrigin);
    s.setValue(QStringLiteral("moveAwayReverse"), h.moveAwayReverse);
    s.setValue(QStringLiteral("homeSpeed"), h.homeSpeed);
    s.setValue(QStringLiteral("homeAccel"), h.homeAccel);
    s.setValue(QStringLiteral("homeDecel"), h.homeDecel);
    s.setValue(QStringLiteral("homeLspeed"), h.homeLspeed);
    s.setValue(QStringLiteral("homeSramp"), h.homeSramp);
    s.setValue(QStringLiteral("homeCreep"), h.homeCreep);
}

HomeSpec readHomeSpec(QSettings& s)
{
    HomeSpec h;
    h.homeIn           = s.value(QStringLiteral("homeIn"), h.homeIn).toInt();
    h.negIn            = s.value(QStringLiteral("negIn"), h.negIn).toInt();
    h.datumMode        = s.value(QStringLiteral("datumMode"), h.datumMode).toInt();
    h.moveAwayOrigin   = s.value(QStringLiteral("moveAwayOrigin"), h.moveAwayOrigin).toDouble();
    h.moveAwayReverse  = s.value(QStringLiteral("moveAwayReverse"), h.moveAwayReverse).toDouble();
    h.homeSpeed        = s.value(QStringLiteral("homeSpeed"), h.homeSpeed).toDouble();
    h.homeAccel        = s.value(QStringLiteral("homeAccel"), h.homeAccel).toDouble();
    h.homeDecel        = s.value(QStringLiteral("homeDecel"), h.homeDecel).toDouble();
    h.homeLspeed       = s.value(QStringLiteral("homeLspeed"), h.homeLspeed).toDouble();
    h.homeSramp        = s.value(QStringLiteral("homeSramp"), h.homeSramp).toDouble();
    h.homeCreep        = s.value(QStringLiteral("homeCreep"), h.homeCreep).toDouble();
    return h;
}
} // namespace

/**
 * @brief 当前机型下轴的回零参数：覆盖优先，否则注册表默认。
 *
 * 返回 const 引用：覆盖值存于 m_homeOverrides（QHash 节点稳定），
 * 默认值存于 RobotModelRegistry 静态单例，均可跨调用安全引用。
 */
const HomeSpec& RobotConfig::homeSpecFor(int axisIndex) const
{
    const quint64 key = homeKeyOf(m_modelId, axisIndex);
    const auto it = m_homeOverrides.constFind(key);
    if (it != m_homeOverrides.constEnd())
        return it.value();
    return RobotModelRegistry::get(m_modelId).axes.at(axisIndex).home;
}

/**
 * @brief 指定机型轴的回零覆盖值（无覆盖时回注册表默认）。面板按机型预览用。
 */
const HomeSpec& RobotConfig::homeOverrideFor(RobotModelId id, int axisIndex) const
{
    const quint64 key = homeKeyOf(id, axisIndex);
    const auto it = m_homeOverrides.constFind(key);
    if (it != m_homeOverrides.constEnd())
        return it.value();
    const RobotModelSpec& spec = RobotModelRegistry::get(id);
    return (spec.id == RobotModelId::Unknown)
               ? RobotModelRegistry::get(RobotModelId::R01).axes.at(axisIndex).home
               : spec.axes.at(axisIndex).home;
}

bool RobotConfig::hasHomeOverride(RobotModelId id, int axisIndex) const
{
    return m_homeOverrides.contains(homeKeyOf(id, axisIndex));
}

void RobotConfig::setHomeOverride(RobotModelId id, int axisIndex, const HomeSpec& h)
{
    m_homeOverrides.insert(homeKeyOf(id, axisIndex), h);
}

bool RobotConfig::clearHomeOverride(RobotModelId id, int axisIndex)
{
    return m_homeOverrides.remove(homeKeyOf(id, axisIndex)) > 0;
}

bool RobotConfig::clearAllHomeOverrides(RobotModelId id)
{
    const quint64 high = static_cast<quint64>(static_cast<int>(id)) << 16;
    bool any = false;
    auto it = m_homeOverrides.begin();
    while (it != m_homeOverrides.end()) {
        if ((it.key() >> 16) == high) {
            it = m_homeOverrides.erase(it);
            any = true;
        } else {
            ++it;
        }
    }
    return any;
}

bool RobotConfig::hasAnyHomeOverrides() const
{
    return !m_homeOverrides.isEmpty();
}

/**
 * @brief 从 ini 载入全部机型覆盖（HomeOverride/<机型>/axis<轴> 组）。
 *
 * 全量载入语义：先清空再读，保证与文件一致（启动空表 / 面板"载入已保存"均可用）。
 */
bool RobotConfig::loadHomeOverrides(const QString& iniPath)
{
    m_homeOverrides.clear();
    QSettings settings(iniPath, QSettings::IniFormat);
    const QStringList groups = settings.childGroups();
    for (const QString& g : groups) {
        const QStringList parts = g.split(QLatin1Char('/'));
        if (parts.size() != 3 || parts.at(0) != QStringLiteral("HomeOverride"))
            continue;
        const RobotModelId id = robotModelFromName(parts.at(1));
        bool ok = false;
        const int axis = parts.at(2).mid(4).toInt(&ok);   // "axis0" -> 0
        if (id == RobotModelId::Unknown || !ok || axis < 0 || axis >= 6)
            continue;
        settings.beginGroup(g);
        m_homeOverrides.insert(homeKeyOf(id, axis), readHomeSpec(settings));
        settings.endGroup();
    }
    return true;
}

/**
 * @brief 全量重写 ini（QSettings 写模式先 clear 再写各覆盖组）。
 */
bool RobotConfig::saveHomeOverrides(const QString& iniPath) const
{
    QSettings settings(iniPath, QSettings::IniFormat);
    settings.clear();
    auto it = m_homeOverrides.constBegin();
    for (; it != m_homeOverrides.constEnd(); ++it) {
        const RobotModelId id = static_cast<RobotModelId>(static_cast<int>(it.key() >> 16));
        const int axis = static_cast<int>(it.key() & 0xFFFFu);
        const QString group = QStringLiteral("HomeOverride/%1/axis%2")
                                  .arg(robotModelName(id))
                                  .arg(axis);
        settings.beginGroup(group);
        writeHomeSpec(settings, it.value());
        settings.endGroup();
    }
    settings.sync();
    return settings.status() == QSettings::NoError;
}

/**
 * @brief 输出完整配置的可读字符串
 */
QString RobotConfig::toString() const
{
    QString result = QString("=== RobotConfig ===\n");
    result += QString("机械臂名称: %1\n").arg(m_robotName);
    result += QString("控制器地址: %1\n").arg(m_controllerIP);
    result += QString("轴数: %1\n\n").arg(m_axisCount);

    for (const auto& config : m_axisConfigs) {
        result += QString("轴%1配置:\n").arg(config.axisIndex);
        result += QString("  减速比: %1\n").arg(config.reductionRatio);
        result += QString("  齿轮比: %1\n").arg(config.gearRatio);
        result += QString("  脉冲数: %1\n").arg(config.pulsePerRev);
        result += QString("  最大速度: %.2f\n").arg(config.maxSpeed);
        result += QString("  最大加速度: %.2f\n").arg(config.maxAccel);
        result += QString("  最大减速度: %.2f\n").arg(config.maxDecel);
        result += QString("  回零位置: %.2f\n\n").arg(config.homePosition);
    }

    return result;
}

/**
 * @brief 按机型规格构建配置（单一数据源：RobotModelRegistry）
 *
 * 不读 JSON；Unknown/未实现机型回退 R01，保证启动有有效配置。
 */
void RobotConfig::initializeFromModel(RobotModelId id)
{
    const RobotModelSpec& spec = RobotModelRegistry::get(id);
    const RobotModelSpec& src =
        (spec.id == RobotModelId::Unknown) ? RobotModelRegistry::get(RobotModelId::R01)
                                            : spec;

    m_robotName = src.name;
    m_controllerIP = src.defaultControllerIP;
    m_axisCount = src.axisCount;
    m_stateUpdateIntervalMs = src.stateUpdateIntervalMs;

    // 回零后目标位置
    m_homePosition = src.homePosition;

    // 关机安全位姿（随机型走）
    m_shutdownPosition = src.shutdownPosition;

    // 回零后移动速度参数
    m_homeMoveSpeed = src.homeMoveSpeed;
    m_homeMoveAccel = src.homeMoveAccel;
    m_homeMoveDecel = src.homeMoveDecel;
    m_moveToHomeAfterHoming = src.moveToHomeAfterHoming; // 回零后是否移动到 homePosition

    // 当前机型 ID（存回退后的 src.id：Unknown 机型回退 R01 后存 R01）
    m_modelId = src.id;

    // 每轴配置
    m_axisConfigs.clear();
    const int n = src.axes.size();
    for (int i = 0; i < n; ++i) {
        const AxisSpec& a = src.axes.at(i);
        AxisConfig config;
        config.axisIndex = i;
        config.maxSpeed = a.maxSpeed;
        config.maxAccel = a.maxAccel;
        config.maxDecel = a.maxDecel;
        config.homePosition = (i < src.homePosition.size()) ? src.homePosition.at(i) : 0.0;
        config.reductionRatio = a.reductionRatio;
        config.gearRatio = a.gearRatio;
        config.pulsePerRev = a.pulsePerRev;
        m_axisConfigs.append(config);
    }

    if (spec.id == RobotModelId::Unknown) {
        qWarning() << "[RobotConfig] 机型" << static_cast<int>(id)
                   << "无效/未实现，已回退到 R01 默认规格";
    } else {
        qInfo() << "[RobotConfig] 已按机型" << src.name << "构建配置（"
                << n << "轴）";
    }
}

