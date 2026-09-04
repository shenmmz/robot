#include "HomingController.h"
#include "../models/RobotConfig.h"
#include "../models/RobotModels.h"
#include "../../ZmotionAPI/ZmotionAPI.h"
#include <QDateTime>
namespace {

/**
 * @brief 移动超时常量
 * 
 * 用于检测移开原点/反向限位操作是否超时
 * 默认值：30000毫秒（30秒）
 */
constexpr int kMoveTimeoutMs = 30000;
constexpr int kPollIntervalMs = 200;    // 轮询间隔200ms
constexpr int kHomingTimeoutMs = 180000; // 回零超时180s

// 轴名称辅助函数，统一格式：机械臂第X轴(J?)
inline QString axisName(int i) { return QString("机械臂第%1轴(J%2)").arg(i + 1).arg(i); }

}

/**
 * @brief 构造函数
 * 
 * 初始化回零控制器的成员变量，创建轮询定时器并连接超时信号
 * 
 * @param zmotionAPI 运动控制API指针，用于控制硬件
 * @param config 机器人配置指针，存储机器人参数
 * @param parent 父对象指针，用于Qt对象树管理
 */
HomingController::HomingController(ZmotionAPI* zmotionAPI, RobotConfig* config, QObject *parent)
    : QObject(parent)
    , m_zmotionAPI(zmotionAPI)   // ZMotion API实例
    , m_config(config)           // 机器人配置
    , m_isHoming(false)         // 回零是否进行中
    , m_serialPrehomeCompleted(false)  // 串口预回零是否完成
    , m_j2MovedAway(false)      // 索引1(机械臂第2轴)已移开原点
    , m_j3MovedAway(false)      // 索引2(机械臂第3轴)已移开原点
    , m_j5MovedAway(false)      // 索引4(机械臂第5轴)已移开原点
    , m_pollTimer(nullptr)      // 轮询定时器
{
    // 初始化各轴状态数组
    m_homeState.resize(kAxisCount, HOME_INIT);  // 各轴回零状态
    m_homeStartTimeMs.resize(kAxisCount, 0);     // 各轴回零开始时间
    m_isHomed.resize(kAxisCount, false);         // 各轴回零完成标志
    m_moveStartTimeMs.resize(kAxisCount, 0);     // 各轴移动开始时间

    // 创建并连接轮询定时器
    m_pollTimer = new QTimer(this);     //轮询定时器
    connect(m_pollTimer, &QTimer::timeout, this, &HomingController::onPollTimeout);  // 连接轮询定时器超时信号到槽函数
}

/**
 * @brief 析构函数
 * 
 * 停止回零操作，释放资源
 */
HomingController::~HomingController()
{
    stopHoming();// 停止回零操作
}

/**
 * @brief 重置回零状态
 * 
 * 将所有轴的回零状态重置为初始状态，包括：
 * - 各轴回零状态重置为 HOME_INIT
 * - 各轴回零开始时间重置为0
 * - 各轴回零完成标志重置为false
 * - 各轴移动开始时间重置为0
 * - 串行预处理移开原点标志（对应索引1/2/4，机械臂第2/3/5轴）重置为false
 */
void HomingController::reset()
{
    for (int i = 0; i < kAxisCount; ++i) {
        m_homeState[i] = HOME_INIT;
        m_homeStartTimeMs[i] = 0;
        m_isHomed[i] = false;
        m_moveStartTimeMs[i] = 0;
    }

    m_j2MovedAway = false;
    m_j3MovedAway = false;
    m_j5MovedAway = false;
    m_serialPrehomeCompleted = false;
}

/**
 * @brief 启动回零流程
 * 
 * 重置所有状态后启动轮询定时器，回零流程由定时器驱动
 * 
 * 流程：串行预处理(索引2/1/4，即机械臂第3/2/5轴) → 并行回零(6轴同时)
 * 
 * @return true 启动成功
 * @return false 启动失败（回零正在进行中或控制器未连接）
 */
bool HomingController::startHoming()
{
    if (m_isHoming) {
        qWarning() << "[HomingController] 回零正在进行中，请勿重复操作";
        return false;
    }

    if (!m_zmotionAPI || !m_zmotionAPI->isConnected()) {
        emit homingFailed(QStringLiteral("控制器未连接"));
        return false;
    }

    // 回零前预检：任一轴未配置原点IO（homeIn<0）时拒绝回零，避免用默认参数空跑
    for (int i = 0; i < kAxisCount; ++i) {
        if (m_config->homeSpecFor(i).homeIn < 0) {
            qWarning() << "[HomingController]" << axisName(i) << "未配置原点IO(homeIn=-1)，该轴被跳过（回零整体拒绝执行）";
            emit homingFailed(QStringLiteral("轴%1 未配置原点IO(homeIn=-1)，请先在机型规格中补全回零参数").arg(i + 1));
            return false;
        }
    }

    reset();
    m_isHoming = true;

    // 关闭ZmotionAPI详细日志，减少回零过程刷屏
    ZmotionAPI::setVerbose(false);

    // 回零前先停止所有轴，确保每个轴都处于空闲状态
    m_zmotionAPI->rapidStop();

    m_pollTimer->start(kPollIntervalMs);

    emit homingStarted();
    return true;
}

/**
 * @brief 停止回零操作
 * 
 * 停止轮询定时器，向控制器发送急停命令，发射回零失败信号
 */
void HomingController::stopHoming()
{
    if (m_pollTimer->isActive()) {
        m_pollTimer->stop();
    }

    if (m_isHoming) {
        if (m_zmotionAPI) {
            m_zmotionAPI->rapidStop();
        }
        m_isHoming = false;
        ZmotionAPI::setVerbose(true);
        emit homingFailed(QStringLiteral("回零已被手动停止"));
    }
}

/**
 * @brief 执行回零步骤（主循环入口）
 * 
 * 先执行串行预处理，确保索引2/1/4（机械臂第3/2/5轴）依次移开原点（避免机械干涉），
 * 串行预处理完成后再执行并行回零（6轴同时回零）
 */
void HomingController::step()
{
    if (!m_zmotionAPI) {
        qWarning() << "[HomingController] step: ZmotionAPI 未初始化";
        return;
    }

    stepSerial();

    if (!m_serialPrehomeCompleted) {
        if (m_j3MovedAway && m_j2MovedAway && m_j5MovedAway) {
            m_serialPrehomeCompleted = true;
        } else {
            return;
        }
    }

    stepParallel();
}

/**
 * @brief 检查是否所有轴都已回零
 * 
 * @return true 所有轴都已回零
 * @return false 至少有一个轴未回零
 */
bool HomingController::isAllHomed() const
{
    for (int i = 0; i < kAxisCount; ++i) {
        if (m_homeState[i] != HOME_SUCCESS) return false;
    }
    return true;
}

/**
 * @brief 轮询定时器超时槽函数
 * 
 * 每次超时调用step()推进回零状态机，
 * 完成后检查是否全部回零成功或是否有轴回零失败
 */
void HomingController::onPollTimeout()
{
    step();

    if (isAllHomed()) {
        m_pollTimer->stop();
        m_isHoming = false;
        qInfo() << QStringLiteral("[回零] 全部轴回零完成汇总：成功 %1 根、失败 0 根").arg(kAxisCount);
        emit homingCompleted();
        return;
    }

    bool anyFailed = false;
    int successCount = 0;
    int failCount = 0;
    for (int i = 0; i < kAxisCount; ++i) {
        if (m_homeState[i] == HOME_SUCCESS) {
            ++successCount;
        } else if (m_homeState[i] == HOME_FAIL) {
            anyFailed = true;
            ++failCount;
            qWarning() << QStringLiteral("[回零] J%1 回零失败!").arg(i + 1);
            emit homingAxisFailed(i, QStringLiteral("轴%1回零失败").arg(i));
        }
    }

    if (anyFailed) {
        qWarning() << QStringLiteral("[回零] 回零结束汇总：成功 %1 根、失败 %2 根").arg(successCount).arg(failCount);
        m_pollTimer->stop();
        m_isHoming = false;
        ZmotionAPI::setVerbose(true);
        emit homingFailed(QStringLiteral("部分轴回零失败"));
    }
}

/**
 * @brief 执行串行预处理步骤
 * 
 * 按优先级依次处理索引2/1/4（机械臂第3/2/5轴），
 * 确保这三个轴先移开原点，避免后续回零时发生机械干涉
 * 
 * 每次只处理一个轴，处理完一个才进入下一个
 */
void HomingController::stepSerial()
{
    // 串行预处理顺序: 索引2(机械臂第3轴) → 索引1(机械臂第2轴) → 索引4(机械臂第5轴)

    if (!m_j3MovedAway && !m_isHomed[2]) {
        processPrehomeAxis(2);
        return;
    }

    if (!m_j2MovedAway && !m_isHomed[1]) {
        processPrehomeAxis(1);
        return;
    }

    if (!m_j5MovedAway && !m_isHomed[4]) {
        processPrehomeAxis(4);
        return;
    }
}

/**
 * @brief 处理单个轴的串行预处理
 * 
 * 对指定轴执行简化的回零状态机：初始化→检查传感器→移开原点→回零
 * 串行预处理阶段不检查反向限位，只关注原点传感器
 * 
 * @param i 轴索引（0-5），串行预处理时为1/2/4（机械臂第2/3/5轴）
 * 
 * 状态机流程：
 * - HOME_INIT: 设置运动参数、映射原点IO、反转IO、使能轴
 * - HOME_CHECK_SENSOR: 读取原点传感器，判断是否需要移开
 * - HOME_MOVE_AWAY_ORIGIN: 发送相对运动命令移开原点
 * - HOME_WAIT_MOVE_AWAY_ORIGIN: 等待轴空闲，超时检测
 * - HOME_START_DATUM: 发送回零命令
 * - HOME_WAIT_DATUM: 等待轴空闲，超时检测
 * - HOME_SUCCESS: 回零成功
 * - HOME_FAIL: 回零失败
 */
void HomingController::processPrehomeAxis(int i)
{
    HomeAxisState& state = m_homeState[i];
    const HomeSpec& hm = m_config->homeSpecFor(i);
    bool& movedAway = (i == 2 ? m_j3MovedAway : (i == 1 ? m_j2MovedAway : m_j5MovedAway));

    switch (state) {
    case HOME_INIT:
        if (!initAxisParams(i)) {
            qWarning() << "[HomingController]" << axisName(i) << "初始化参数失败";
            state = HOME_FAIL;
            emit homingAxisFailed(i, QStringLiteral("初始化参数失败"));
            emit homingProgress(i, state);
            return;
        }
        state = HOME_CHECK_SENSOR;
        emit homingProgress(i, state);
        break;

    case HOME_CHECK_SENSOR:
    {
        bool homeTriggered = false;
        if (hm.homeIn >= 0) {
            int diVal = m_zmotionAPI->getDI(hm.homeIn);
            if (diVal < 0) {
                qWarning() << "[HomingController]" << axisName(i) << "串行读取原点IO失败";
                break;  // 下一轮重试
            }
            homeTriggered = (diVal != 0);
        }

        if (!homeTriggered) {
            state = HOME_MOVE_AWAY_ORIGIN;
        } else {
            movedAway = true;
            state = HOME_START_DATUM;
        }
        emit homingProgress(i, state);
        break;
    }

    case HOME_MOVE_AWAY_ORIGIN:
        if (!moveAxisRelative(i, hm.moveAwayOrigin)) {
            qWarning() << "[HomingController]" << axisName(i) << "移开原点运动失败";
            state = HOME_FAIL;
            emit homingAxisFailed(i, QStringLiteral("移开原点运动失败"));
            emit homingProgress(i, state);
            return;
        }
        m_moveStartTimeMs[i] = QDateTime::currentMSecsSinceEpoch();
        state = HOME_WAIT_MOVE_AWAY_ORIGIN;
        emit homingProgress(i, state);
        break;

    case HOME_WAIT_MOVE_AWAY_ORIGIN:
        if (isAxisIdle(i)) {
            movedAway = true;
            m_moveStartTimeMs[i] = 0;
            state = HOME_START_DATUM;
            emit homingProgress(i, state);
        } else if (m_moveStartTimeMs[i] > 0 &&
                   QDateTime::currentMSecsSinceEpoch() - m_moveStartTimeMs[i] > kMoveTimeoutMs) {
            qWarning() << "[HomingController]" << axisName(i) << "移开原点超时";
            state = HOME_FAIL;
            m_moveStartTimeMs[i] = 0;
            emit homingAxisFailed(i, QStringLiteral("串行移开原点超时"));
            emit homingProgress(i, state);
        }
        break;

    case HOME_START_DATUM:
        if (!startAxisDatum(i)) {
            qWarning() << "[HomingController]" << axisName(i) << "启动回零失败";
            state = HOME_FAIL;
            emit homingAxisFailed(i, QStringLiteral("启动回零失败"));
            emit homingProgress(i, state);
            return;
        }
        state = HOME_WAIT_DATUM;
        m_homeStartTimeMs[i] = QDateTime::currentMSecsSinceEpoch();
        emit homingProgress(i, state);
        break;

    case HOME_WAIT_DATUM:
    {
        // 使用getAxisSingleHomeStatus验证DATUM真正成功，而非仅判空闲（与并行阶段一致）
        ZmotionResult homeResult = m_zmotionAPI->getAxisSingleHomeStatus(i);
        bool homed = homeResult.success && homeResult.axisHomeStatus != 0;
        if (homed) {
            m_isHomed[i] = true;
            state = HOME_SUCCESS;
            emit homingAxisCompleted(i);
            emit homingProgress(i, state);
            qInfo() << QStringLiteral("[回零] %1 回零成功（串行阶段，原点IO已触发）").arg(axisName(i));
          
        } else {
            // 超时检测
            qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - m_homeStartTimeMs[i];
            if (m_homeStartTimeMs[i] != 0 && elapsed > kHomingTimeoutMs) {
                qWarning() << QStringLiteral("[回零-串行] J%1 回零超时(%2s)")
                    .arg(i + 1).arg(kHomingTimeoutMs / 1000);
                state = HOME_FAIL;
                emit homingAxisFailed(i, QStringLiteral("串行回零超时"));
                emit homingProgress(i, state);
            }
        }
        break;
    }

    case HOME_SUCCESS:
    case HOME_FAIL:
        break;

    default:
        break;
    }
}

/**
 * @brief 执行并行回零步骤
 * 
 * 所有轴同时执行回零流程，跳过已回零的轴和未完成串行预处理的轴
 * 
 * 与串行预处理的区别：
 * 1. 同时处理所有轴（而非一次一个）
 * 2. 增加反向限位传感器检测和移开逻辑
 * 3. 回零等待阶段增加超时检测和反向限位触发回退
 * 
 * 状态机流程：
 * - HOME_INIT: 设置运动参数、映射原点IO、反转IO、使能轴
 * - HOME_CHECK_SENSOR: 读取原点和反向限位传感器，判断需要移开哪个
 * - HOME_MOVE_AWAY_ORIGIN: 发送相对运动命令移开原点
 * - HOME_WAIT_MOVE_AWAY_ORIGIN: 等待轴空闲，超时检测
 * - HOME_MOVE_AWAY_REVERSE: 发送相对运动命令移开反向限位
 * - HOME_WAIT_MOVE_AWAY_REVERSE: 等待轴空闲，超时检测
 * - HOME_START_DATUM: 确认轴空闲后发送回零命令，记录起始时间
 * - HOME_WAIT_DATUM: 检查回零状态，检测反向限位触发回退，超时判断
 * - HOME_SUCCESS: 回零成功
 * - HOME_FAIL: 回零失败（超时）
 */
void HomingController::stepParallel()
{
    for (int i = 0; i < kAxisCount; ++i) {
        if (m_isHomed[i]) {
            continue;
        }

        if ((i == 1 && !m_j2MovedAway) ||
            (i == 2 && !m_j3MovedAway) ||
            (i == 4 && !m_j5MovedAway)) {
            continue;
        }

        HomeAxisState& state = m_homeState[i];
        qint64& startTime = m_homeStartTimeMs[i];
        qint64& moveStartTime = m_moveStartTimeMs[i];
        const HomeSpec& hm = m_config->homeSpecFor(i);

        // HOME_INIT和HOME_CHECK_SENSOR不检查IDLE（只设置参数/读传感器，不发运动）
        // HOME_WAIT_* 也不检查（正在等待中，需要继续轮询）
        // 只有MOVE_AWAY_*/START_DATUM需要等轴空闲再发送运动命令
        bool needsIdle = (state == HOME_MOVE_AWAY_ORIGIN ||
                          state == HOME_MOVE_AWAY_REVERSE ||
                          state == HOME_START_DATUM);
        if (needsIdle && !isAxisIdle(i)) {
            continue;
        }

        switch (state) {
        case HOME_INIT:
            if (!initAxisParams(i)) {
                qWarning() << "[HomingController]" << axisName(i) << "初始化参数失败";
                state = HOME_FAIL;
                emit homingAxisFailed(i, QStringLiteral("初始化参数失败"));
                emit homingProgress(i, state);
                continue;
            }
            startTime = 0;
            moveStartTime = 0;
            state = HOME_CHECK_SENSOR;
            emit homingProgress(i, state);
            break;

        case HOME_CHECK_SENSOR:
        {
            int sensorState = checkAxisSensors(i, true);
            if (sensorState < 0) {
                continue;  // IO读取失败，下一轮重试
            }
            if (sensorState == 1) {
                state = HOME_MOVE_AWAY_ORIGIN;
            } else if (sensorState == 2) {
                state = HOME_MOVE_AWAY_REVERSE;
            } else {
                state = HOME_START_DATUM;
            }
            emit homingProgress(i, state);
            break;
        }

        case HOME_MOVE_AWAY_ORIGIN:
            if (!moveAxisRelative(i, hm.moveAwayOrigin)) {
                qWarning() << "[HomingController]" << axisName(i) << "移开原点运动失败";
                state = HOME_FAIL;
                emit homingAxisFailed(i, QStringLiteral("移开原点运动失败"));
                emit homingProgress(i, state);
                continue;
            }
            moveStartTime = QDateTime::currentMSecsSinceEpoch();
            state = HOME_WAIT_MOVE_AWAY_ORIGIN;
            emit homingProgress(i, state);
            break;

        case HOME_WAIT_MOVE_AWAY_ORIGIN:
            if (isAxisIdle(i)) {
                moveStartTime = 0;
                state = HOME_CHECK_SENSOR;
                emit homingProgress(i, state);
            } else if (moveStartTime > 0) {
                if (QDateTime::currentMSecsSinceEpoch() - moveStartTime > kMoveTimeoutMs) {
                    qWarning() << "[HomingController]" << axisName(i) << "移开原点超时";
                    state = HOME_FAIL;
                    moveStartTime = 0;
                    emit homingAxisFailed(i, QStringLiteral("移开原点超时"));
                    emit homingProgress(i, state);
                }
            }
            break;

        case HOME_MOVE_AWAY_REVERSE:
            if (!moveAxisRelative(i, hm.moveAwayReverse)) {
                qWarning() << "[HomingController]" << axisName(i) << "移开反向限位运动失败";
                state = HOME_FAIL;
                emit homingAxisFailed(i, QStringLiteral("移开反向限位运动失败"));
                emit homingProgress(i, state);
                continue;
            }
            moveStartTime = QDateTime::currentMSecsSinceEpoch();
            state = HOME_WAIT_MOVE_AWAY_REVERSE;
            emit homingProgress(i, state);
            break;

        case HOME_WAIT_MOVE_AWAY_REVERSE:
            if (isAxisIdle(i)) {
                moveStartTime = 0;
                state = HOME_CHECK_SENSOR;
                emit homingProgress(i, state);
            } else if (moveStartTime > 0) {
                if (QDateTime::currentMSecsSinceEpoch() - moveStartTime > kMoveTimeoutMs) {
                    qWarning() << "[HomingController]" << axisName(i) << "移开反向限位超时";
                    state = HOME_FAIL;
                    moveStartTime = 0;
                    emit homingAxisFailed(i, QStringLiteral("移开反向限位超时"));
                    emit homingProgress(i, state);
                }
            }
            break;

        case HOME_START_DATUM:
            if (isAxisIdle(i)) {
                if (!startAxisDatum(i)) {
                    qWarning() << "[HomingController]" << axisName(i) << "启动回零失败";
                    state = HOME_FAIL;
                    emit homingAxisFailed(i, QStringLiteral("启动回零失败"));
                    emit homingProgress(i, state);
                    continue;
                }
                state = HOME_WAIT_DATUM;
                startTime = QDateTime::currentMSecsSinceEpoch();
                emit homingProgress(i, state);
            }
            break;

        case HOME_WAIT_DATUM:
        {
            qint64 now = QDateTime::currentMSecsSinceEpoch();

            if (hm.negIn >= 0) {
                int negTriggered = m_zmotionAPI->getDI(hm.negIn);
                if (!negTriggered) {
                    m_zmotionAPI->singleAxisCancel({i}, 2);
                    startTime = 0;
                    state = HOME_MOVE_AWAY_REVERSE;
                    emit homingProgress(i, state);
                    break;
                }
            }

            ZmotionResult homeResult = m_zmotionAPI->getAxisSingleHomeStatus(i);
            bool homed = homeResult.success && homeResult.axisHomeStatus != 0;

            if (homed) {
                m_isHomed[i] = true;
                state = HOME_SUCCESS;
                startTime = 0;
                emit homingAxisCompleted(i);
                emit homingProgress(i, state);
                qInfo() << QStringLiteral("[回零] %1 回零成功（原点IO已触发，DATUM完成）").arg(axisName(i));
            } else if (startTime != 0 && (now - startTime) > kHomingTimeoutMs) {
                qWarning() << QStringLiteral("[回零] J%1 回零超时(%2s)")
                    .arg(i + 1).arg(kHomingTimeoutMs / 1000);
                startTime = 0;
                state = HOME_FAIL;
                emit homingAxisFailed(i, QStringLiteral("轴%1回零超时").arg(i));
                emit homingProgress(i, state);
            }
            break;
        }

        case HOME_SUCCESS:
        case HOME_FAIL:
            break;

        default:
            break;
        }
    }
}

/**
 * @brief 初始化轴参数
 * 
 * 设置指定轴的运动参数、映射原点IO、反转IO电平、使能轴
 * 所有操作都有错误检查，失败时返回false
 * 
 * @param i 轴索引（0-5）
 * @return true 初始化成功
 * @return false 初始化失败（API调用失败）
 */
bool HomingController::initAxisParams(int i)
{
    const HomeSpec& hm = m_config->homeSpecFor(i);

    ZmotionResult result;

    // 首先设置轴类型为旋转轴（1）
    result = m_zmotionAPI->setAtype(i, 1);
    if (!result.success) {
        qWarning() << "[HomingController]" << axisName(i) << "设置轴类型失败:" << result.errorMessage;
        return false;
    }

    // 设置脉冲当量（从 RobotConfig 获取）
    double axisUnit = m_config->getAxisUnitsList().at(i);
    result = m_zmotionAPI->setAxisUnits({i}, {axisUnit});
    if (!result.success) {
        qWarning() << "[HomingController]" << axisName(i) << "设置脉冲当量失败:" << result.errorMessage;
        return false;
    }

    result = m_zmotionAPI->setAxisSpeed({i}, {hm.homeSpeed});
    if (!result.success) {
        qWarning() << "[HomingController]" << axisName(i) << "设置速度失败:" << result.errorMessage;
        return false;
    }

    result = m_zmotionAPI->setAxisAccel({i}, {hm.homeAccel});
    if (!result.success) {
        qWarning() << "[HomingController]" << axisName(i) << "设置加速度失败:" << result.errorMessage;
        return false;
    }

    result = m_zmotionAPI->setAxisDecel({i}, {hm.homeDecel});
    if (!result.success) {
        qWarning() << "[HomingController]" << axisName(i) << "设置减速度失败:" << result.errorMessage;
        return false;
    }

    result = m_zmotionAPI->setAxisLspeed({i}, {hm.homeLspeed});
    if (!result.success) {
        qWarning() << "[HomingController]" << axisName(i) << "设置起始速度失败:" << result.errorMessage;
        return false;
    }

    result = m_zmotionAPI->setAxisCreepSpeed({i}, {hm.homeCreep});
    if (!result.success) {
        qWarning() << "[HomingController]" << axisName(i) << "设置爬行速度失败:" << result.errorMessage;
        return false;
    }

    result = m_zmotionAPI->setAxisSramp({i}, {hm.homeSramp});
    if (!result.success) {
        qWarning() << "[HomingController]" << axisName(i) << "设置S曲线失败:" << result.errorMessage;
        return false;
    }

    result = m_zmotionAPI->setAxisDatumOrigin({i}, {hm.homeIn});
    if (!result.success) {
        qWarning() << "[HomingController]" << axisName(i) << "映射原点IO失败:" << result.errorMessage;
        return false;
    }

    result = m_zmotionAPI->setInvertIn(hm.homeIn, 1);
    if (!result.success) {
        qWarning() << "[HomingController]" << axisName(i) << "反转原点IO失败:" << result.errorMessage;
        return false;
    }

    if (hm.negIn >= 0) {
        result = m_zmotionAPI->setInvertIn(hm.negIn, 1);
        if (!result.success) {
            qWarning() << "[HomingController]" << axisName(i) << "反转反向限位IO失败:" << result.errorMessage;
            return false;
        }
    }

    result = m_zmotionAPI->setAxisEnable({i}, 1);
    if (!result.success) {
        qWarning() << "[HomingController]" << axisName(i) << "使能失败:" << result.errorMessage;
        return false;
    }

    return true;
}

/**
 * @brief 检查轴传感器状态
 * 
 * 读取指定轴的原点传感器和反向限位传感器状态
 * 
 * @param i 轴索引（0-5）
 * @param checkNegIn 是否检查反向限位传感器（串行预处理时为false，并行回零时为true）
 * @return int 传感器状态码：
 *         - 0: 传感器均未触发，可以开始回零
 *         - 1: 原点传感器被触发，需要移开原点
 *         - 2: 反向限位传感器被触发，需要移开反向限位
 */
int HomingController::checkAxisSensors(int i, bool checkNegIn)
{
    const HomeSpec& hm = m_config->homeSpecFor(i);

    int homeTriggered = m_zmotionAPI->getDI(hm.homeIn);
    if (homeTriggered < 0) {
        qWarning() << "[HomingController]" << axisName(i) << "读取原点IO失败，返回码:" << homeTriggered;
        return -1;
    }

    if (!homeTriggered) {  // 低电平触发（0表示触发）
        return 1;
    }

    if (checkNegIn && hm.negIn >= 0) {
        int negTriggered = m_zmotionAPI->getDI(hm.negIn);
        if (negTriggered < 0) {
            qWarning() << "[HomingController]" << axisName(i) << "读取反向限位IO失败，返回码:" << negTriggered;
            return -1;
        }
        if (!negTriggered) {  // 低电平触发（0表示触发）
            return 2;
        }
    }

    return 0;
}

/**
 * @brief 检查轴是否空闲
 * 
 * 通过读取轴的IDLE参数判断轴是否处于空闲状态
 * 
 * @param i 轴索引（0-5）
 * @return true 轴空闲（可以执行新的运动命令）
 * @return false 轴正在运动中
 */
bool HomingController::isAxisIdle(int i)
{
    // 使用 GetIfIdle2 而不是直接读 IDLE 参数。
    // GetIfIdle2 会额外判断编码器速度(MSPEED)，当 IDLE=-1 但 MSPEED=0 时也能正确返回"已停止"。
    ZmotionResult idleResult = m_zmotionAPI->getIfIdle2(i);
    // axisHomeStatus: 0=运动中, 非0=已停止
    return idleResult.success && idleResult.axisHomeStatus != 0;
}

/**
 * @brief 执行轴相对运动
 * 
 * 让指定轴执行相对运动（从当前位置移动指定距离）
 * 
 * @param i 轴索引（0-5）
 * @param distance 相对移动距离（正值正向移动，负值反向移动）
 * @return true 运动命令发送成功
 * @return false 运动命令发送失败
 */
bool HomingController::moveAxisRelative(int i, float distance)
{
    ZmotionResult result = m_zmotionAPI->setAxisSingleMove({i}, {distance});
    if (!result.success) {
        qWarning() << "[HomingController]" << axisName(i) << "相对运动失败:" << result.errorMessage;
        return false;
    }
    return true;
}

/**
 * @brief 启动轴回零
 * 
 * 先使能轴，然后发送回零命令
 * 
 * @param i 轴索引（0-5）
 * @return true 回零命令发送成功
 * @return false 回零命令发送失败
 */
bool HomingController::startAxisDatum(int i)
{
    const HomeSpec& hm = m_config->homeSpecFor(i);

    ZmotionResult result = m_zmotionAPI->setAxisEnable({i}, 1);
    if (!result.success) {
        qWarning() << "[HomingController]" << axisName(i) << "使能失败:" << result.errorMessage;
        return false;
    }

    result = m_zmotionAPI->setAxisSingleDatum(i, hm.datumMode, hm.homeSpeed, hm.homeCreep, 0.0f);
    if (!result.success) {
        qWarning() << "[HomingController]" << axisName(i) << "启动回零失败:" << result.errorMessage;
        return false;
    }

    return true;
}

/**
 * @brief 移动所有轴到指定位置
 * 
 * 回零完成后，将每个轴移动到指定的绝对位置
 * 
 * @param positions 各轴目标位置列表，长度必须不小于kAxisCount
 */
void HomingController::moveAllAxesToPositions(const QList<float>& positions, float speed, float accel, float decel)
{
    if (!m_zmotionAPI) {
        return;
    }

    if (positions.size() < kAxisCount) {
        qWarning() << "[HomingController] 位置列表长度不足";
        return;
    }
    for (int i = 0; i < kAxisCount; ++i) {
        m_zmotionAPI->startAxisSingleMoveAbs(i, speed, accel, decel, positions[i]);
    }
}

