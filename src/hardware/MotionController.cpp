/*
 * @file MotionController.cpp
 * @brief 实现 MotionController 类，负责机械臂的硬件控制逻辑。
 *
 * 本文件包含：
 * - 连接和断开硬件接口
 * - 状态管理
 * - 错误处理与状态更新
 * - 软限位检查
 * 
 * 注意：运动控制方法（如moveAxis、stopAxis等）已移至 ZmotionAPI，
 * MotionController 仅负责连接管理、状态更新和信号转发。
 */
#include "MotionController.h"
#include <QDebug>

/**
 * @brief MotionController 构造函数
 *
 * 初始化运动控制器，关联机器人配置和状态对象，并创建ZmotionAPI实例。
 *
 * @param config 机器人配置指针
 * @param state 机器人状态指针
 * @param parent 父对象指针
 */
MotionController::MotionController(RobotConfig* config, RobotState* state, QObject *parent)
    : QObject(parent)
    , m_config(config)
    , m_state(state)
    , m_zmotionAPI(nullptr)
    , m_isConnected(false)
{
    // 从配置中读取状态更新间隔，如果配置中没有则使用默认值100ms
    m_updateIntervalMs = m_config->getStateUpdateInterval();
    
    // 创建ZmotionAPI实例（传入轴数）
    m_zmotionAPI = new ZmotionAPI(m_config->getAxisCount() * 2, this);  // 关节轴 + 虚拟轴
    
    // 创建状态更新定时器
    m_updateTimer = new QTimer(this);
    QObject::connect(m_updateTimer, &QTimer::timeout, this, &MotionController::updateState);
    // 定时器初始为停止状态，连接成功后再启动
}

/**
 * @brief MotionController 析构函数
 *
 * 断开硬件连接，释放资源。
 */
MotionController::~MotionController()
{
    if (m_isConnected) {
        disconnect();
    }
}

/**
 * @brief 连接到硬件控制器
 *
 * 根据配置中的IP地址和端口号建立与机械臂控制器的连接。
 * 连接成功后设置状态为空闲，并发射connected()信号。
 *
 * @return true 连接成功或已连接，false 连接失败
 */
bool MotionController::connect(const QString& ipAddr)
{
    // 如果已经连接，先断开现有连接
    if (m_isConnected) {
        disconnect();
    }

    // 调用底层API建立以太网连接
    ZmotionResult result = m_zmotionAPI->openEth(ipAddr);

    if (result.success) {
        // 连接成功，更新状态
        m_isConnected = true;
        m_state->setStatus(RobotState::StatusIdle);
        qDebug() << "成功连接到控制器:" << ipAddr;
        
        // 启动状态更新定时器
        m_updateTimer->start(m_updateIntervalMs);
        qDebug() << "状态更新定时器已启动，间隔:" << m_updateIntervalMs << "ms";
        
        emit connected();
        return true;
    } else {
        // 连接失败，发出错误信号
        emit connectionError(result.errorMessage);
        return false;
    }
}

/**
 * @brief 断开与硬件控制器的连接
 *
 * 断开与机械臂控制器的连接，释放硬件资源。
 * 断开后设置状态为空闲，并发射disconnected()信号。
 *
 * @return true 断开成功或已断开，false 断开失败
 */
bool MotionController::disconnect()
{
    if (!m_isConnected) {
        return true;
    }

    // 停止状态更新定时器
    m_updateTimer->stop();
    qDebug() << "状态更新定时器已停止";
    
    m_zmotionAPI->close();
    m_isConnected = false;
    m_state->setStatus(RobotState::StatusIdle);

    qDebug() << "已断开控制器连接";
    emit disconnected();
    return true;
}

/**
 * @brief 更新所有轴状态
 *
 * 遍历所有轴，读取当前位置并更新到状态对象。
 * 更新完成后发射statusUpdated()信号。
 */
void MotionController::updateState()
{
    if (!checkConnected()) return;

    // 读取所有关节轴的位置并更新状态
    for (int i = 0; i < m_config->getAxisCount(); ++i) {
        updateAxisState(i);
    }

    // 读取虚拟轴位置作为笛卡尔坐标（存储原始值，不做归一化）
    // - 正解模式(CONNREFRAME): 关节轴驱动虚拟轴(FK)
    // - 逆解模式(CONNFRAME): 指令虚拟轴位置，控制器自动IK解算驱动关节轴
    {
        QList<int> virtualAxes = m_config->getVirtualAxisList();
        for (int i = 0; i < virtualAxes.size(); ++i) {
            ZmotionResult result = m_zmotionAPI->getAxisPosition(virtualAxes[i]);
            if (result.success) {
                double pos = result.axisPosition;
                m_state->setCartesianPosition(i, pos);
            }
        }
    }

    emit statusUpdated();
}


/**
 * @brief 检查连接状态
 *
 * 验证与硬件控制器的连接是否正常。若未连接，发射connectionError信号。
 *
 * @return true 已连接，false 未连接
 */
bool MotionController::checkConnected()
{
    if (!m_isConnected) {
        emit connectionError(QStringLiteral("未连接到硬件"));
        return false;
    }
    return true;
}

/**
 * @brief 处理Zmotion错误
 *
 * 将ZmotionAPI返回的错误结果转换并更新到机器人状态，
 * 同时发射hardwareError信号。
 *
 * @param result Zmotion操作结果结构体
 */
void MotionController::handleZmotionError(const ZmotionResult& result)
{
    m_state->setError(result.errorCode, result.errorMessage);
    emit hardwareError(result.errorCode, result.errorMessage);
}

/**
 * @brief 更新单轴状态
 *
 * 读取指定轴的当前位置并更新到状态对象。
 *
 * @param axisIndex 要更新的轴索引
 */
void MotionController::updateAxisState(int axisIndex)
{
    // 从ZmotionAPI读取该轴的状态并更新
    ZmotionResult result = m_zmotionAPI->getAxisPosition(axisIndex);
    if (result.success) {
        m_state->setAxisPosition(axisIndex, result.axisPosition);
    }
}

/**
 * @brief 设置状态更新间隔
 *
 * 设置定时更新状态的时间间隔。如果定时器正在运行，会自动重启。
 *
 * @param intervalMs 更新间隔（毫秒），范围 10ms ~ 5000ms
 */
void MotionController::setStateUpdateInterval(int intervalMs)
{
    // 限制范围：10ms ~ 5000ms（0.01秒 ~ 5秒）
    // 避免更新过于频繁（CPU占用过高）或过于稀疏（状态更新不及时）
    m_updateIntervalMs = qBound(1, intervalMs, 5000);
    
    // 如果定时器正在运行，更新间隔并重启
    if (m_updateTimer->isActive()) {
        m_updateTimer->stop();
        m_updateTimer->start(m_updateIntervalMs);
        qDebug() << "状态更新间隔已更新为:" << m_updateIntervalMs << "ms";
    }
}

/**
 * @brief 切换到逆解模式（笛卡尔空间控制）
 *
 * 通过调用ZmotionAPI的startInverseKinematics方法，将机器人切换到逆解模式。
 * 切换成功后更新状态并发射kinematicsModeChanged信号。
 *
 * @return true 切换成功，false 切换失败
 */
bool MotionController::setInverseKinematicsMode()
{
    if (!m_isConnected) {
        qWarning() << "[MotionController] 未连接控制器，无法切换到逆解模式";
        return false;
    }

    ZmotionResult result = m_zmotionAPI->startInverseKinematics(
        m_config->getJointAxisList(), m_config->getVirtualAxisList());
    if (result.success) {
        qDebug() << "[MotionController] 已切换到逆解模式（笛卡尔空间控制）";
        emit kinematicsModeChanged(true);
    } else {
        qWarning() << "[MotionController] 切换逆解模式失败:" << result.errorMessage;
        handleZmotionError(result);
    }
    return result.success;
}

/**
 * @brief 切换到正解模式（关节空间控制）
 *
 * 通过调用ZmotionAPI的startForwardKinematics方法，将机器人切换到正解模式。
 * 切换成功后更新状态并发射kinematicsModeChanged信号。
 *
 * @return true 切换成功，false 切换失败
 */
bool MotionController::setForwardKinematicsMode()
{
    if (!m_isConnected) {
        qWarning() << "[MotionController] 未连接控制器，无法切换到正解模式";
        return false;
    }

    ZmotionResult result = m_zmotionAPI->startForwardKinematics(
        m_config->getJointAxisList(), m_config->getVirtualAxisList());
    if (result.success) {
        qDebug() << "[MotionController] 已切换到正解模式（关节空间控制）";
        emit kinematicsModeChanged(false);
    } else {
        qWarning() << "[MotionController] 切换正解模式失败:" << result.errorMessage;
        handleZmotionError(result);
    }
    return result.success;
}

/**
 * @brief 获取当前是否为逆解模式
 *
 * 查询ZmotionAPI的当前运动模式，判断是否为逆解模式。
 *
 * @return true 当前为逆解模式，false 当前为正解模式或未初始化
 */
bool MotionController::isInverseKinematicsMode() const
{
    if (!m_isConnected || !m_zmotionAPI) {
        return false;
    }
    // 0 表示正解模式（关节空间），1 表示逆解模式（笛卡尔空间），-1 表示未设置
    return m_zmotionAPI->getCurrentMotionMode() == 1;
}
