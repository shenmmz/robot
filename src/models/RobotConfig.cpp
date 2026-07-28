/*
 * @file RobotConfig.cpp
 * @brief 实现 RobotConfig 类的配置加载、保存和默认参数初始化。
 *
 * 本文件负责：
 * - 从 JSON 配置文件加载机械臂参数
 * - 将配置写回 JSON 文件
 * - 初始化默认配置参数
 */
#include "RobotConfig.h"
#include <QDir>
#include <QFileInfo>
#include "../../library/RobotConstants.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>

/**
 * @brief RobotConfig 构造函数
 *
 * 初始化机械臂控制器的默认配置参数。将轴数设置为6，
 * 机械臂名称为 "Robot_R06"，控制器IP为 "192.168.1.100"，
 * 端口号为 8888，并生成各轴的默认配置。
 *
 * @note 此构造函数将自动调用 initializeDefaultConfigs() 来初始化所有轴的配置
 */
RobotConfig::RobotConfig()
    : m_axisCount(6)
    , m_robotName("Robot_R06")
    , m_controllerIP("192.168.1.100")
    , m_stateUpdateIntervalMs(100)  // 默认状态更新间隔100ms（10Hz）
{
    initializeDefaultConfigs();
}

/**
 * @brief 获取指定轴的配置信息
 *
 * @param axisIndex 轴索引，从0开始
 * @return AxisConfig 如果索引有效则返回对应的轴配置，否则返回默认的空配置
 */
RobotConfig::AxisConfig RobotConfig::getAxisConfig(int axisIndex) const
{
    if (axisIndex >= 0 && axisIndex < m_axisConfigs.size()) {
        return m_axisConfigs.at(axisIndex);
    }
    return AxisConfig();
}

/**
 * @brief 设置机械臂的轴数
 *
 * 更改机械臂轴数时，将清空现有的轴配置列表，
 * 并根据新的轴数重新初始化所有轴的默认配置。
 *
 * @param count 新的轴数
 *
 * @note 调用此函数后，所有轴的配置将被重置为默认值。
 *       如需保留现有配置，请在调用前使用 saveToJson() 保存
 */
void RobotConfig::setAxisCount(int count)
{
    m_axisCount = count;  // 设置轴数
    m_axisConfigs.clear();  // 清空现有轴配置
    initializeDefaultConfigs();  // 重新初始化默认配置
}

/**
 * @brief 设置指定轴的配置参数
 *
 * 更新指定轴的完整配置信息，包括运动参数（速度、加速度等）
 * 和硬件参数（传感器、减速比、齿轮比等）。
 *
 * @param axisIndex 轴索引，从0开始 (0-5 对应 J0-J5)
 * @param config    包含轴配置信息的 AxisConfig 结构体
 *
 * @note 若 axisIndex 超出有效范围 [0, m_axisCount)，则操作被忽略
 *       配置更改不会自动保存到文件，需要手动调用 saveToJson() 保存
 */
void RobotConfig::setAxisConfig(int axisIndex, const RobotConfig::AxisConfig& config)
{
    if (axisIndex >= 0 && axisIndex < m_axisConfigs.size()) {
        m_axisConfigs[axisIndex] = config;
    }
}

/**
 * @brief 从 JSON 文件加载机械臂配置
 *
 * 读取指定路径的 JSON 配置文件，解析以下内容：
 * 1. 基本配置：机械臂名称、控制器IP地址、端口号、轴数
 * 2. 轴配置：每个轴的运动参数和硬件参数
 *
 * 轴配置包括：
 * - 运动参数：maxSpeed (最大速度), maxAccel (最大加速度), maxDecel (最大减速度)
 * - 软限位：softLimitMax (正向极限), softLimitMin (负向极限)
 * - 硬件参数：sensors (传感器列表), reductionRatio (减速比), 
 *             gearRatio (齿轮比), pulsePerRev (每转脉冲数)
 * - 其他：homePosition (回零位置，可选)
 *
 * @param filePath 配置文件的完整路径
 * @return true 文件加载并解析成功，false 表示文件打开失败或JSON格式错误
 *
 * @note 若文件不存在或格式错误，原有配置保持不变
 * @note 传感器数组支持多个传感器，如 ["IN0", "IN1"]
 * @note 齿轮比格式为字符串，如 "1:4"，表示小轮转4圈，大轮转1圈
 */
bool RobotConfig::loadFromJson(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        return false;
    }

    QJsonObject obj = doc.object();

    /* 解析基本配置 */
    m_robotName = obj["robotName"].toString("Robot_R06");
    m_controllerIP = obj["controllerIP"].toString("192.168.1.100");
    m_axisCount = obj["axisCount"].toInt(6);
    
    // 解析状态更新间隔，限制范围 10ms ~ 5000ms
    int interval = obj["stateUpdateIntervalMs"].toInt(100);
    m_stateUpdateIntervalMs = qBound(10, interval, 5000);

    // 解析回零后目标位置
    m_homePosition.clear();
    QJsonArray homePosArray = obj["homePosition"].toArray();
    for (int i = 0; i < homePosArray.size(); ++i) {
        m_homePosition.append(homePosArray[i].toDouble(0.0));
    }

    // 解析回零后移动速度参数
    m_homeMoveSpeed = obj["homeMoveSpeed"].toDouble(60.0);
    m_homeMoveAccel = obj["homeMoveAccel"].toDouble(80.0);
    m_homeMoveDecel = obj["homeMoveDecel"].toDouble(100.0);

    // 解析脉冲当量
    m_axisUnits.clear();
    QJsonArray unitsArray = obj["axisUnits"].toArray();
    if (!unitsArray.isEmpty()) {
        for (int i = 0; i < unitsArray.size(); ++i) {
            m_axisUnits.append(unitsArray[i].toDouble(0.0));
        }
    } else {
        m_axisUnits = RobotConstants::getAxisUnitsList();
    }

    /* 解析轴配置数组 */
    m_axisConfigs.clear();
    QJsonArray axisArray = obj["axes"].toArray();
    for (int i = 0; i < axisArray.size(); ++i) {
        QJsonObject axisObj = axisArray[i].toObject();
        AxisConfig config;
        config.axisIndex = axisObj["index"].toInt(i);
        config.maxSpeed = axisObj["maxSpeed"].toDouble(360);
        config.maxAccel = axisObj["maxAccel"].toDouble(1000);
        config.maxDecel = axisObj["maxDecel"].toDouble(1000);
        config.homePosition = axisObj["homePosition"].toDouble(0);

        /* 解析传感器映射数组 */
        config.sensors.clear();
        QJsonArray sensorArray = axisObj["sensors"].toArray();
        for (const auto& sensorValue : sensorArray) {
            config.sensors.append(sensorValue.toString());
        }

        /* 解析硬件参数 */
        config.reductionRatio = axisObj["reductionRatio"].toInt(1);
        config.gearRatio = axisObj["gearRatio"].toString("1:1");
        config.pulsePerRev = axisObj["pulsePerRev"].toInt(1000);
        
        m_axisConfigs.append(config);
    }

    return true;
}

// 保存机器人配置到JSON文件
// @param filePath 文件路径
// @return 是否保存成功
bool RobotConfig::saveToJson(const QString& filePath) const
{
    QJsonObject obj;

    obj["robotName"] = m_robotName;
    obj["controllerIP"] = m_controllerIP;
    obj["axisCount"] = m_axisCount;
    obj["stateUpdateIntervalMs"] = m_stateUpdateIntervalMs;

    QJsonArray homePosArray;
    for (double pos : m_homePosition) {
        homePosArray.append(pos);
    }
    obj["homePosition"] = homePosArray;
    obj["homeMoveSpeed"] = m_homeMoveSpeed;
    obj["homeMoveAccel"] = m_homeMoveAccel;
    obj["homeMoveDecel"] = m_homeMoveDecel;

    QJsonArray unitsArray;
    for (double unit : m_axisUnits) {
        unitsArray.append(unit);
    }
    obj["axisUnits"] = unitsArray;

    QJsonArray axisArray;
    for (const auto& config : m_axisConfigs) {
        QJsonObject axisObj;
        axisObj["index"] = config.axisIndex;
        axisObj["maxSpeed"] = config.maxSpeed;
        axisObj["maxAccel"] = config.maxAccel;
        axisObj["maxDecel"] = config.maxDecel;
        axisObj["homePosition"] = config.homePosition;

        QJsonArray sensorArray;
        for (const auto& sensor : config.sensors) {
            sensorArray.append(sensor);
        }
        axisObj["sensors"] = sensorArray;

        axisObj["reductionRatio"] = config.reductionRatio;
        axisObj["gearRatio"] = config.gearRatio;
        axisObj["pulsePerRev"] = config.pulsePerRev;
        
        axisArray.append(axisObj);
    }
    obj["axes"] = axisArray;

    QJsonDocument doc(obj);
    
    // 确保目标目录存在
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        dir.mkpath(QStringLiteral("."));
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    file.write(doc.toJson());
    file.close();

    return true;
}

/**
 * @brief 获取关节轴号列表
 *
 * 返回物理关节轴号列表 (0~5)，用于 CONNFRAME 建模。
 *
 * @return QList<int> 关节轴号列表
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
 * @brief 获取虚拟轴号列表
 *
 * 返回笛卡尔空间虚拟轴号列表 (6~11)，用于 CONNFRAME 建模。
 *
 * @return QList<int> 虚拟轴号列表
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
 * @brief 获取各轴脉冲当量
 *
 * 脉冲当量 = pulsePerRev × reductionRatio × gearRatioDenominator / 360.0
 * 表示关节端每度对应的脉冲数，考虑了减速比和齿轮比。
 *
 * @return QList<double> 各轴脉冲当量列表
 */
QList<double> RobotConfig::getAxisUnitsList() const
{
    return m_axisUnits;
}

/**
 * @brief 将配置转换为字符串描述
 *
 * 以人类可读的格式输出当前RobotConfig的完整配置信息，
 * 包括机械臂名称、控制器地址、轴数以及所有轴的运动参数和硬件参数。
 *
 * @return QString 格式化的配置信息字符串
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
 * @brief 初始化默认轴配置
 *
 * 清空现有的轴配置列表，并根据当前的轴数（m_axisCount）
 * 重新生成所有轴的基础配置。基础配置仅包含轴索引，
 * 其他参数使用AxisConfig结构体的默认值。
 */
void RobotConfig::initializeDefaultConfigs()
{
    m_axisConfigs.clear();
    
    // 使用全局常量设置脉冲当量
    m_axisUnits = RobotConstants::getAxisUnitsList();

    // 默认回零后目标位置（6轴全0）
    m_homePosition.clear();
    for (int i = 0; i < m_axisCount; ++i) {
        m_homePosition.append(0.0);
    }

    // 默认回零后移动速度参数
    m_homeMoveSpeed = 60.0;
    m_homeMoveAccel = 80.0;
    m_homeMoveDecel = 100.0;

    for (int i = 0; i < m_axisCount; ++i) {
        AxisConfig config;
        config.axisIndex = i;
        m_axisConfigs.append(config);
    }
}
