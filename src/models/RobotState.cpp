/*
 * @file RobotState.cpp
 * @brief 实现 RobotState 类，用于管理机械臂运行时状态。
 *
 * 本文件负责：
 * - 初始化轴状态数据
 * - 提供状态访问与修改方法
 * - 触发状态变化信号
 */
#include "RobotState.h"

/**
 * @brief RobotState 构造函数
 *
 * 初始化机械臂状态，设置轴数、各轴位置/速度/加速度初始值为0，状态为空闲。
 *
 * @param axisCount 轴数量
 * @param parent 父对象指针
 */
RobotState::RobotState(int axisCount, QObject *parent)
    : QObject(parent)
    , m_axisCount(axisCount)
    , m_status(StatusIdle)
    , m_errorCode(0)
{
    // 初始化所有轴的位置、速度、加速度为0
    for (int i = 0; i < axisCount; ++i) {
        m_axisPositions.append(0.0);
        m_axisVelocities.append(0.0);
        m_axisAccels.append(0.0);
        m_cartesianPositions.append(0.0); // 初始化笛卡尔坐标
    }
}

/**
 * @brief 获取状态描述字符串
 *
 * @return QString 状态对应的中文描述："空闲"、"运动中"、"错误"、"紧急停止"或"未知"
 */
QString RobotState::getStatusStr() const
{
    switch (m_status) {
        case StatusIdle: return "空闲";
        case StatusMoving: return "运动中";
        case StatusError: return "错误";
        case StatusEmergencyStop: return "紧急停止";
        case StatusCartesianMode: return "笛卡尔模式";
        case StatusJointMode: return "关节模式";
        default: return "未知";
    }
}

/**
 * @brief 获取单轴位置
 *
 * @param axisIndex 轴索引
 * @return double 当前角度位置（度），索引无效返回0.0
 */
double RobotState::getAxisPosition(int axisIndex) const
{
    if (axisIndex >= 0 && axisIndex < m_axisPositions.size()) {
        return m_axisPositions.at(axisIndex);
    }
    return 0.0;
}

/**
 * @brief 获取单轴速度
 *
 * @param axisIndex 轴索引
 * @return double 当前速度（度/秒），索引无效返回0.0
 */
double RobotState::getAxisVelocity(int axisIndex) const
{
    if (axisIndex >= 0 && axisIndex < m_axisVelocities.size()) {
        return m_axisVelocities.at(axisIndex);
    }
    return 0.0;
}

/**
 * @brief 获取单轴加速度
 *
 * @param axisIndex 轴索引
 * @return double 当前加速度（度/秒²），索引无效返回0.0
 */
double RobotState::getAxisAccel(int axisIndex) const
{
    if (axisIndex >= 0 && axisIndex < m_axisAccels.size()) {
        return m_axisAccels.at(axisIndex);
    }
    return 0.0;
}

/**
 * @brief 设置机器人状态
 *
 * 更改状态并发射statusChanged信号（仅当状态实际改变时）。
 *
 * @param status 新的机器人状态
 */
void RobotState::setStatus(RobotStatus status)
{
    if (m_status != status) {
        RobotStatus oldStatus = m_status;  // 保存旧状态
        m_status = status;                  // 更新为新状态
        emit statusChanged(oldStatus, status);  // 发射状态变更信号
    }
}

/**
 * @brief 设置单轴位置
 *
 * 更新指定轴的位置，若值改变则发射axisPositionChanged信号。
 *
 * @param axisIndex 轴索引
 * @param position 位置值（度）
 */
void RobotState::setAxisPosition(int axisIndex, double position)
{
    if (axisIndex >= 0 && axisIndex < m_axisPositions.size()) {
        if (m_axisPositions[axisIndex] != position) {
            m_axisPositions[axisIndex] = position;
            emit axisPositionChanged(axisIndex, position);
        }
    }
}

/**
 * @brief 设置单轴速度
 *
 * @param axisIndex 轴索引
 * @param velocity 速度值（度/秒）
 */
void RobotState::setAxisVelocity(int axisIndex, double velocity)
{
    if (axisIndex >= 0 && axisIndex < m_axisVelocities.size()) {
        m_axisVelocities[axisIndex] = velocity;
    }
}

/**
 * @brief 设置单轴加速度
 *
 * @param axisIndex 轴索引
 * @param accel 加速度值（度/秒²）
 */
void RobotState::setAxisAccel(int axisIndex, double accel)
{
    if (axisIndex >= 0 && axisIndex < m_axisAccels.size()) {
        m_axisAccels[axisIndex] = accel;
    }
}

/**
 * @brief 批量设置所有轴位置
 *
 * @param positions 位置值列表，个数必须与轴数一致
 */
void RobotState::setAllAxisPositions(const QList<double>& positions)
{
    if (positions.size() == m_axisPositions.size()) {
        m_axisPositions = positions;
    }
}

/**
 * @brief 批量设置所有轴速度
 *
 * @param velocities 速度值列表，个数必须与轴数一致
 */
void RobotState::setAllAxisVelocities(const QList<double>& velocities)
{
    if (velocities.size() == m_axisVelocities.size()) {
        m_axisVelocities = velocities;
    }
}

/**
 * @brief 获取笛卡尔坐标值
 *
 * @param index 坐标索引：0=X, 1=Y, 2=Z, 3=A, 4=B, 5=C
 * @return double 笛卡尔坐标值，索引无效返回0.0
 */
double RobotState::getCartesianPosition(int index) const
{
    if (index >= 0 && index < m_cartesianPositions.size()) {
        return m_cartesianPositions.at(index);
    }
    return 0.0;
}

/**
 * @brief 设置单个笛卡尔坐标值
 *
 * @param index 坐标索引：0=X, 1=Y, 2=Z, 3=A, 4=B, 5=C
 * @param value 坐标值
 */
void RobotState::setCartesianPosition(int index, double value)
{
    if (index >= 0 && index < m_cartesianPositions.size()) {
        m_cartesianPositions[index] = value;
    }
}

/**
 * @brief 批量设置所有笛卡尔坐标
 *
 * @param positions 笛卡尔坐标值列表 [X, Y, Z, A, B, C]
 */
void RobotState::setAllCartesianPositions(const QList<double>& positions)
{
    if (positions.size() == m_cartesianPositions.size()) {
        m_cartesianPositions = positions;
    }
}

/**
 * @brief 设置错误状态
 *
 * 记录错误码和错误信息，将状态切换为StatusError，发射errorOccurred信号。
 *
 * @param errorCode 错误码
 * @param errorMsg 错误信息
 */
void RobotState::setError(int errorCode, const QString& errorMsg)
{
    m_errorCode = errorCode;
    m_errorMessage = errorMsg;
    setStatus(StatusError);
    emit errorOccurred(errorCode, errorMsg);
}

/**
 * @brief 清除错误状态
 *
 * 将错误码重置为0，清空错误信息。
 */
void RobotState::clearError()
{
    m_errorCode = 0;
    m_errorMessage = "";
}

/**
 * @brief 转换为字符串描述
 *
 * 以人类可读的格式输出当前状态的完整信息。
 *
 * @return QString 格式化的状态信息字符串
 */
QString RobotState::toString() const
{
    QString result = QString("=== RobotState ===\n");
    result += QString("状态: %1\n").arg(getStatusStr());
    result += QString("轴数: %1\n").arg(m_axisCount);
    
    for (int i = 0; i < m_axisCount; ++i) {
        result += QString("轴%1: 位置=%.2f, 速度=%.2f, 加速度=%.2f\n")
                .arg(i)
                .arg(m_axisPositions[i])
                .arg(m_axisVelocities[i])
                .arg(m_axisAccels[i]);
    }
    
    if (m_errorCode != 0) {
        result += QString("错误: [%1] %2\n").arg(m_errorCode).arg(m_errorMessage);
    }
    
    return result;
}
