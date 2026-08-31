/*
 * @file RobotConfig.cpp
 * @brief RobotConfig 实现：按机型规格（RobotModelRegistry）构建机械臂配置。
 *
 * 数据单一来源在 RobotModels.cpp
 */
#include "RobotConfig.h"
#include "RobotModels.h"
#include <QDebug>          // qInfo / qWarning 流式输出

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
 * @brief 设置指定轴配置；索引越界则忽略
 */
void RobotConfig::setAxisConfig(int axisIndex, const RobotConfig::AxisConfig& config)
{
    if (axisIndex >= 0 && axisIndex < m_axisConfigs.size()) {
        m_axisConfigs[axisIndex] = config;
    }
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
        result += QString("  传感器: %1\n").arg(config.sensors.join(", "));
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

    // 回零后移动速度参数
    m_homeMoveSpeed = src.homeMoveSpeed;
    m_homeMoveAccel = src.homeMoveAccel;
    m_homeMoveDecel = src.homeMoveDecel;

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
        config.sensors = a.sensors;
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

