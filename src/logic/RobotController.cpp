/*
 * @file RobotController.cpp
 * @brief 实现 RobotController 类，负责机械臂业务逻辑的协调与控制。
 *
 * 本文件包含：
 * - 初始化流程
 * - 硬件连接管理
 * - 高级运动控制流程
 * - 信号与槽的连接
 */
#include "RobotController.h"
#include "../hardware/MotionController.h"
#include "../hardware/ServoController.h"
#include "../hardware/Rs485Controller.h"
#include "../models/RobotKinematics.h"
#include "../utils/MathUtils.h"
#include "../../ZmotionAPI/ZmotionAPI.h"
#include <QDebug>
#include <QTimer>
#include <cmath>

/**
 * @brief RobotController 构造函数
 *
 * 初始化机器人控制器，创建空的成员变量指针。
 *
 * @param parent 父对象指针
 */
// 构造函数，初始化所有成员指针为nullptr
RobotController::RobotController(QObject *parent)
    : QObject(parent)
    , m_config(nullptr)
    , m_state(nullptr)
    , m_motionController(nullptr)
    , m_homingController(nullptr)
    , m_rs485Controller(nullptr)
    , m_servoController(nullptr)
    , m_initialized(false)
{
}

/**
 * @brief RobotController 析构函数
 *
 * 释放所有资源：断开硬件连接、删除运动控制器、状态对象和配置对象。
 */
RobotController::~RobotController()
{
    if (m_servoController) {
        delete m_servoController;
    }
    if (m_rs485Controller) {
        delete m_rs485Controller;
    }
    if (m_homingController) {
        delete m_homingController;
    }
    if (m_motionController) {
        m_motionController->disconnect();
        delete m_motionController;
    }
    if (m_state) {
        delete m_state;
    }
    if (m_config) {
        delete m_config;
    }
}

/**
 * @brief 按机型规格初始化
 *
 * 按指定机型枚举从代码侧注册表（RobotModelRegistry）构建 RobotConfig，
 * 不再从 robot_config.json 读取配置。随后调用 initialize(RobotConfig*) 完成
 * 状态对象、运动控制器等子模块的创建与信号槽连接。
 *
 * @param modelId 机型枚举，缺省 R01（当前唯一实装机型）
 * @return true 初始化成功，false 初始化失败
 */
bool RobotController::initialize(RobotModelId modelId)
{
    if (m_config) {
        delete m_config;
        m_config = nullptr;
    }
    m_config = new RobotConfig();

    // 按机型规格填充配置（替代 JSON 加载，单一数据源在 RobotModels.cpp）
    m_config->initializeFromModel(modelId);

    return initialize(m_config);
}

/**
 * @brief 通过配置对象初始化
 *
 * 使用已加载的RobotConfig对象初始化控制器，创建状态对象和运动控制器，
 * 并连接信号槽。
 *
 * @param config 已配置的RobotConfig指针
 * @return true 初始化成功，false 配置对象为空
 */
bool RobotController::initialize(RobotConfig* config)
{
    if (!config) {
        emit initializationFailed("配置对象为空");
        return false;
    }

    m_config = config;
    
    m_state = new RobotState(m_config->getAxisCount(), this);
    
    m_motionController = new MotionController(m_config, m_state, this);
    
    m_homingController = new HomingController(
        m_motionController->getZmotionAPI(),
        m_config,
        m_state,
        this
    );
    
    // 初始化RS485控制器（用于外部6路PWM模块）
    m_rs485Controller = new Rs485Controller(this);
    
    // 初始化舵机控制器（通过RS485控制外部PWM模块）
    m_servoController = new ServoController(m_rs485Controller, this);
    
    // 设置ZMotion API指针（RS485通过ZMC的Modbus转RS485功能通信）
    m_rs485Controller->setZmotionAPI(m_motionController->getZmotionAPI());

    // 初始化舵机到初始角度，同步软件 currentAngle 与硬件实际位置
    m_servoController->initAll();

    connectSignals();
    
    m_initialized = true;
    emit initialized();
    
//     qDebug() << "RobotController初始化成功";
    return true;
}

/**
 * @brief 连接硬件
 *
 * 与机械臂硬件建立连接。需先完成initialize()初始化。
 *
 * @return true 连接成功，false 连接失败或未初始化
 */
bool RobotController::connectHardware(const QString& ipAddr)
{
    if (!m_initialized) {
        emit hardwareConnectionFailed("RobotController未初始化");
        return false;
    }

    if (m_config) {
        m_config->setControllerIP(ipAddr);
    }

    bool ok = m_motionController->connect(ipAddr);
    if (ok) {
        // 首次连接成功：若Flash为空，写入出厂默认原点，使Flash始终持有一个有效home
        ensureOriginInitialized();
    }
    return ok;
    // 注意：失败时 MotionController::connect() 会发射 connectionError 信号，
    // connectSignals() 中已将其路由到 hardwareConnectionFailed
}

/**
 * @brief 断开硬件连接
 *
 * 断开与机械臂硬件的连接。
 *
 * @return true 断开成功，false 未初始化
 */
bool RobotController::disconnectHardware()
{
    if (!m_initialized) {
        return false;
    }

    return m_motionController->disconnect();
}

/**
 * @brief 搜索网络中的控制器
 *
 * 通过 ZmotionAPI 搜索以太网上的 ZMotion 控制器，返回 IP 地址列表。
 *
 * @return QStringList 搜索到的控制器 IP 地址列表，失败时返回空列表
 */
QStringList RobotController::searchControllers()
{
    if (!m_initialized || !m_motionController) {
        return QStringList();
    }

    ZmotionResult result = m_motionController->getZmotionAPI()->searchEthlist();
    return result.success ? result.ipList : QStringList();
}

/**
 * @brief 初始化机器人运动学模型
 *
 * 将六轴串联机器人模型写入 ZMotion 控制器：
 * 1. 设置物理关节轴（旋转类型、脉冲当量、初始位置、加减速）
 * 2. 生成 25 元素 TABLE 并写入控制器
 * 3. 设置虚拟笛卡尔轴
 * 4. 调用 CONNFRAME 关联关节轴与虚拟轴
 *
 * @return true 模型初始化成功，false 失败
 */
bool RobotController::initRobotModel()
{
    if (!isConnected()) {
        qWarning() << "[initRobotModel] 控制器未连接，无法初始化模型";
        emit modelInitFailed(-1);
        return false;
    }

    ZmotionAPI* api = m_motionController->getZmotionAPI();
    ZmotionResult ret;
    const int axisNum = m_config->getAxisCount();
    const QList<int> jointAxes = m_config->getJointAxisList();
    const QList<int> virtualAxes = m_config->getVirtualAxisList();
    const QList<double> axisUnits = m_config->getAxisUnitsList();
//     qDebug() << "[initRobotModel] 配置机器人模型...";

    // ===== 1. 物理关节轴初始化 =====
    for (int i = 0; i < axisNum; ++i) {
        int axis = jointAxes[i];

        ret = api->setAtype(axis, 1);
        if (!ret.success) { emit modelInitFailed(ret.errorCode); return false; }

        ret = api->setAxisUnits({axis}, {axisUnits[i]});
        if (!ret.success) { emit modelInitFailed(ret.errorCode); return false; }

        ret = api->setAxisDpos({axis}, {0.0});
        if (!ret.success) { emit modelInitFailed(ret.errorCode); return false; }

        ret = api->setAxisAccel({axis}, {100.0});
        if (!ret.success) { emit modelInitFailed(ret.errorCode); return false; }

        ret = api->setAxisDecel({axis}, {100.0});
        if (!ret.success) { emit modelInitFailed(ret.errorCode); return false; }
    }

    // ===== 2. 生成并写入 TABLE =====
    QVector<float> tableData = RobotKinematics::buildFrame6Table(axisUnits);

    ret = api->setTable(0, 25, tableData);
    if (!ret.success) {
        qWarning() << "[initRobotModel] TABLE 写入失败";
        emit modelInitFailed(ret.errorCode);
        return false;
    }

    // ===== 3. 虚拟轴配置（必须在 CONNFRAME 之前） =====
    QList<double> virtSpeed, virtAccel, virtDecel;
    for (int i = 0; i < axisNum; ++i) {
        ret = api->setAtype(virtualAxes[i], 0);
        if (!ret.success) { emit modelInitFailed(ret.errorCode); return false; }

        virtSpeed.append(100.0);
        virtAccel.append(100.0);
        virtDecel.append(150.0);
    }

    QList<double> virtUnits;
    for (int i = 0; i < axisNum; ++i) { virtUnits.append(10000.0); }

    ret = api->setAxisUnits(virtualAxes, virtUnits);
    if (!ret.success) { emit modelInitFailed(ret.errorCode); return false; }

    ret = api->setAxisSpeed(virtualAxes, virtSpeed);
    if (!ret.success) { emit modelInitFailed(ret.errorCode); return false; }

    ret = api->setAxisAccel(virtualAxes, virtAccel);
    if (!ret.success) { emit modelInitFailed(ret.errorCode); return false; }

    ret = api->setAxisDecel(virtualAxes, virtDecel);
    if (!ret.success) { emit modelInitFailed(ret.errorCode); return false; }

    // 清零虚拟轴 DPOS，避免姿态值累积偏大
    {
        QList<double> zeroDpos;
        for (int i = 0; i < axisNum; ++i) zeroDpos.append(0.0);
        ret = api->setAxisDpos(virtualAxes, zeroDpos);
        if (!ret.success) { emit modelInitFailed(ret.errorCode); return false; }
    }
//     qDebug() << "[initRobotModel] 六轴串联机器人模型配置完成";
    emit modelInitSuccess();
    return true;
}

/**
 * @brief 查询硬件连接状态
 *
 * @return true 已连接，false 未连接或未初始化
 */
bool RobotController::isConnected() const
{
    if (!m_initialized) {
        return false;
    }
    return m_motionController->isConnected();
}

/**
 * @brief 执行回零操作
 *
 * 对所有轴依次执行回零操作，将机械臂移动到机械原点位置。
 *
 * @return true 回零命令发送成功，false 硬件未连接
 */
bool RobotController::executeHoming()
{
    if (!isConnected()) {
        emit operationFailed(QStringLiteral("回零"), QStringLiteral("硬件未连接"));
        return false;
    }

    if (!m_homingController) {
        emit operationFailed(QStringLiteral("回零"), QStringLiteral("回零控制器未初始化"));
        return false;
    }

    emit operationStarted(QStringLiteral("回零"));

    bool result = m_homingController->startHoming();// 启动回零操作

    if (!result) {
        emit operationFailed(QStringLiteral("回零"), QStringLiteral("回零启动失败"));
    }

    return result;
}

void RobotController::stopHoming()
{
    if (m_homingController) {
        m_homingController->stopHoming();
    }
}

/**
 * @brief 执行安全停止
 *
 * 停止所有轴运动并将状态设置为空闲。
 *
 * @return true 操作成功，false 硬件未连接
 */
bool RobotController::executeSafeStop()
{
    if (!isConnected()) {
        return false;
    }

    // 安全停止：减速停止 -> 禁用轴
    ZmotionAPI* zmotionAPI = m_motionController->getZmotionAPI();
    zmotionAPI->rapidStop();
    m_state->setStatus(RobotState::StatusIdle);
    
    return true;
}

/**
 * @brief 移动到预设位置
 *
 * 将机械臂移动到预定义的位置，如"初始位置"、"工作位置"等。
 *
 * @param presetName 预设位置名称
 * @return true 命令发送成功，false 硬件未连接
 */
bool RobotController::moveToPreset(const QString& presetName)
{
    if (!isConnected()) {
        emit operationFailed("移动到预设位置", "硬件未连接");
        return false;
    }

    // TODO: 实现预设位置的移动
    // 例如："初始位置"、"工作位置"等

//     qDebug() << "移动到预设位置:" << presetName;
    return true;
}

/**
 * @brief 单轴绝对位置移动
 *
 * 将指定轴移动到目标绝对位置。若参数为负值，使用该轴配置默认值的一半。
 *
 * @param axisIndex 轴索引
 * @param targetPos 目标位置（度）
 * @param speed 速度（度/秒），负值使用默认值
 * @param accel 加速度（度/秒²），负值使用默认值
 * @param decel 减速度（度/秒²），负值使用默认值
 * @return true 移动命令发送成功，false 硬件未连接
 */
bool RobotController::moveAxis(int axisIndex, double targetPos, double speed, double accel, double decel)
{
    if (!isConnected()) {
        emit operationFailed(QString(QStringLiteral("轴%1移动")).arg(axisIndex), QStringLiteral("硬件未连接"));
        return false;
    }

    // 使用默认值
    if (speed < 0) {
        speed = m_config->getAxisConfig(axisIndex).maxSpeed * 0.5;
    }
    if (accel < 0) {
        accel = m_config->getAxisConfig(axisIndex).maxAccel * 0.5;
    }
    if (decel < 0) {
        decel = m_config->getAxisConfig(axisIndex).maxDecel * 0.5;
    }

    ZmotionAPI* zmotionAPI = m_motionController->getZmotionAPI();
    ZmotionResult result = zmotionAPI->startAxisSingleMoveAbs(axisIndex, speed, accel, decel, targetPos);
    
    if (result.success) {
        m_state->setStatus(RobotState::StatusMoving);
        return true;
    } else {
        emit errorOccurred(result.errorCode, result.errorMessage);
        return false;
    }
}


/**
 * 关节空间运动控制函数
 *
 * 在关节空间中控制机器人移动到指定位置，支持自定义运动速度、加速度、减速度和超时时间。
 * 运动前会自动切换到正解模式，并确保硬件已连接且关节数量匹配。
 *
 * @param jointPos 目标关节位置列表，包含各轴的目标位置
 * @param speed 运动速度
 * @param accel 加速度
 * @param decel 减速度
 * @param timeoutMs 超时时间（毫秒）
 * @return 返回true表示运动成功完成，false表示运动失败
 */
bool RobotController::moveJ(const QList<double>& jointPos, double speed, double accel, double decel, int timeoutMs)
{
    if (!isConnected()) {
        qCritical() << "[moveJ] 硬件未连接";
        emit operationFailed(QStringLiteral("moveJ"), QStringLiteral("硬件未连接"));
        return false;
    }

    if (jointPos.size() != m_config->getAxisCount()) {
        qCritical() << "[moveJ] 关节数量不匹配: 传入" << jointPos.size()
                     << "期望" << m_config->getAxisCount();
        emit operationFailed(QStringLiteral("moveJ"), QStringLiteral("关节数量不匹配"));
        return false;
    }

    // 确保在正解模式
    switchToJointMode();

    ZmotionAPI* api = m_motionController->getZmotionAPI();
    QList<int> axes = m_config->getJointAxisList();
    QList<float> targets;
    for (double pos : jointPos) targets.append(static_cast<float>(pos));

    // 取消 CONNREFRAME 绑定，使关节轴回到独立轴模式
    api->executeCommand("CANCEL");
    // CANCEL 后必须等待轴停止，对应 RTBasic 的 WAIT IDLE
    api->waitForAxesIdle(axes, 2000);// 等待轴停止，防止后续指令出错

    ZmotionResult result = api->directMoveAbsolute(axes, targets, static_cast<float>(speed), static_cast<float>(accel), static_cast<float>(decel));// 直接绝对移动，触发正解模式
    if (!result.success) {
        qCritical() << "[moveJ] directMoveAbsolute 失败:" << result.errorMessage;
        emit operationFailed(QStringLiteral("moveJ"), result.errorMessage);
        return false;
    }

    m_state->setStatus(RobotState::StatusMoving);
//     qDebug() << "[moveJ] 运动已启动, targets=" << targets << "speed=" << speed << "timeout=" << timeoutMs;
    // 等待关节轴物理停止（IDLE检测，CONNREFRAME 下 GetDpos 不可靠）
    result = api->waitForAxesIdle(axes, timeoutMs);
    m_state->setStatus(RobotState::StatusIdle);

    if (!result.success) {
        qCritical() << "[moveJ] 等待运动到位超时:" << result.errorMessage << "超时=" << timeoutMs << "ms";
        return false;
    }
    return true;
}

/**
 * @brief 执行笛卡尔空间直线运动
 *
 * 切换到逆解模式(CONNFRAME: virtual→joint)，
 * 直接指令虚拟轴(6-11)的笛卡尔位置，由控制器自动计算IK驱动关节轴。
 *
 * @param cartPos 笛卡尔目标坐标，包含6个值(X, Y, Z, Rx, Ry, Rz)
 * @param speed 运动速度
 * @param accel 加速度
 * @param decel 减速度
 * @param timeoutMs 等待运动完成的最大超时时间，单位毫秒
 * @return 操作是否成功，true表示命令已发送且运动完成，false表示失败
 */
bool RobotController::moveL(const QList<double>& cartPos, double speed, double accel, double decel, int timeoutMs)
{
    if (!isConnected()) {
        emit operationFailed(QStringLiteral("moveL"), QStringLiteral("硬件未连接"));
        return false;
    }

    if (cartPos.size() != 6) {
        emit operationFailed(QStringLiteral("moveL"), QStringLiteral("笛卡尔坐标需要6个值(X,Y,Z,Rx,Ry,Rz)"));
        return false;
    }

    // 确保在逆解模式（已在逆解时守卫跳过 500ms）
    switchToCartesianMode();

    ZmotionAPI* api = m_motionController->getZmotionAPI();

    // 逆解模式(CONNFRAME): 指令虚拟轴(6-11)，控制器通过DH自动解算关节轴
    QList<int> virtualAxes = m_config->getVirtualAxisList();

    QList<double> curRaw;
    for (int i = 0; i < 6; ++i) {
        ZmotionResult r = api->getAxisPosition(virtualAxes[i]);
        curRaw.append(r.success ? r.axisPosition : 0.0);
    }

    QList<float> targets;
    for (int i = 0; i < cartPos.size(); ++i) {
        double v = cartPos[i];
        if (i >= 3) {
            double normCur = normalizeAngle180(curRaw[i]);
            double delta = v - normCur;
            v = curRaw[i] + delta;
        }
        targets.append(static_cast<float>(v));
    }

//     qDebug().noquote() << QStringLiteral("[moveL] 当前DPOS: X=%1 Y=%2 Z=%3 RX=%4 RY=%5 RZ=%6")
//         .arg(curRaw[0],0,'f',3).arg(curRaw[1],0,'f',3).arg(curRaw[2],0,'f',3)
//         .arg(curRaw[3],0,'f',3).arg(curRaw[4],0,'f',3).arg(curRaw[5],0,'f',3);
//     qDebug().noquote() << QStringLiteral("[moveL] 目标DPOS: X=%1 Y=%2 Z=%3 RX=%4 RY=%5 RZ=%6")
//         .arg(targets[0],0,'f',3).arg(targets[1],0,'f',3).arg(targets[2],0,'f',3)
//         .arg(targets[3],0,'f',3).arg(targets[4],0,'f',3).arg(targets[5],0,'f',3);

    // 虚拟轴规划运动:MoveAbs 触发 CONNFRAME IK,关节轴按 SPEED/ACCEL/DECEL 跟随
    // (SetDpos 是瞬间赋值,会让机械臂"瞬移"到目标位置,看不到连续运动)
    QList<float> dposTargets;
    for (float v : targets) dposTargets.append(v);

    // 给虚拟轴设速度/加速度/减速度(虚拟轴 UNITS=10000 对应 mm)
    {
        QList<double> vSpeeds(virtualAxes.size(), static_cast<double>(speed));
        QList<double> vAccels(virtualAxes.size(), static_cast<double>(accel));
        QList<double> vDecels(virtualAxes.size(), static_cast<double>(decel));
        api->setAxisSpeed(virtualAxes, vSpeeds);
        api->setAxisAccel(virtualAxes, vAccels);
        api->setAxisDecel(virtualAxes, vDecels);
    }

    // 同时给关节轴设运动参数(CONNFRAME 模式下关节轴也要遵守)
    {
        QList<int> jointAxes = m_config->getJointAxisList();
        QList<double> jSpeeds(jointAxes.size(), static_cast<double>(speed));
        QList<double> jAccels(jointAxes.size(), static_cast<double>(accel));
        QList<double> jDecels(jointAxes.size(), static_cast<double>(decel));
        api->setAxisSpeed(jointAxes, jSpeeds);
        api->setAxisAccel(jointAxes, jAccels);
        api->setAxisDecel(jointAxes, jDecels);
    }

    ZmotionResult result = api->directMoveAbsolute(virtualAxes, dposTargets,
        static_cast<float>(speed), static_cast<float>(accel), static_cast<float>(decel));
    if (!result.success) {
        emit operationFailed(QStringLiteral("moveL"), result.errorMessage);
        return false;
    }

    m_state->setStatus(RobotState::StatusMoving);
    // 等待虚拟轴运动停止(CONNFRAME 模式下虚拟轴先到位后关节轴跟随到位)
    QList<int> jointAxes = m_config->getJointAxisList();
    result = api->waitForAxesIdle(virtualAxes, timeoutMs);
    m_state->setStatus(RobotState::StatusIdle);// 运动完成后设置状态为空闲

    if (!result.success) {
        qWarning() << "[moveL] 等待运动到位超时,命令已发出";
        if (result.errorCode == -4)
            return false;
    }
    return true;
}

/**
 * @brief 关节空间点动——Move → 立即返回
 *
 * 确保正解模式后直接发绝对位置指令。控制器 CONNFRAME 跟踪模式下，
 * 每次新目标自动覆盖旧目标，无需 Cancel。
 * 适用于 ± 按钮快速连续点击场景。
 *
 * @param axis 目标关节轴索引 (0~5)
 * @param targetAngle 目标角度
 * @param speed 运动速度
 * @param accel 加速度
 * @param decel 减速度
 * @return 指令是否成功发送
 */
bool RobotController::jogJoint(int axis, double targetAngle,
                                double speed, double accel, double decel)
{
    if (!isConnected()) return false;

    switchToJointMode();

    ZmotionAPI* api = m_motionController->getZmotionAPI();
    QList<int> axes = m_config->getJointAxisList();

    QList<double> joints = m_state->getAllAxisPositions();
    if (axis >= 0 && axis < joints.size())
        joints[axis] = targetAngle;

    QList<float> targets;
    for (double pos : joints) targets.append(static_cast<float>(pos));

    api->directMoveAbsolute(axes, targets, static_cast<float>(speed),
                            static_cast<float>(accel), static_cast<float>(decel));
    return true;
}

/**
 * @brief 批量多轴关节点动——一次读取当前位置、叠加多个轴目标后单次下发
 *
 * 解决逐轴调用 jogJoint 时后一轴的"当前位置快照"覆盖前一轴尚未到达的目标的问题。
 * 适用于视觉跟随等需要每帧下发多轴指令的场景。
 *
 * @param axisTargets 要更新的轴索引(0~5) → 目标角度（°）
 * @param speed 速度
 * @param accel 加速度
 * @param decel 减速度
 * @return 指令是否成功发送
 */
bool RobotController::jogJointBatch(const QHash<int, double>& axisTargets,
                                     double speed, double accel, double decel)
{
    if (!isConnected()) return false;

    switchToJointMode();

    ZmotionAPI* api = m_motionController->getZmotionAPI();
    QList<int> axes = m_config->getJointAxisList();

    // 取消当前运动，确保新指令立即生效（视觉追踪场景最新目标优先）
    api->singleAxisCancel(axes, 0);
    api->waitForAxesIdle(axes, 200);

    // 一次读取所有轴当前真实位置
    QList<double> joints = m_state->getAllAxisPositions();

    // 叠加本帧所有目标轴的新角度（未在 axisTargets 中的轴保留当前值）
    for (auto it = axisTargets.begin(); it != axisTargets.end(); ++it) {
        const int axis = it.key();
        const double targetAngle = it.value();
        if (axis >= 0 && axis < joints.size())
            joints[axis] = targetAngle;
    }

    QList<float> targets;
    for (double pos : joints) targets.append(static_cast<float>(pos));

    ZmotionResult result = api->directMoveAbsolute(axes, targets, static_cast<float>(speed),
                            static_cast<float>(accel), static_cast<float>(decel));
    return result.success;
}

/**
 * @brief 笛卡尔空间点动——Move → 立即返回
 *
 * 确保逆解模式后直接发笛卡尔绝对位置指令，不等待到位。
 * CONNFRAME 跟踪模式下每次新目标自动覆盖旧目标。
 *
 * @param cartTarget 笛卡尔目标坐标 (X,Y,Z,A,B,C)
 * @param speed 运动速度
 * @param accel 加速度
 * @param decel 减速度
 * @return 指令是否成功发送
 */
bool RobotController::jogCart(const QList<double>& cartTarget,
                               double speed, double accel, double decel)
{
    if (!isConnected()) return false;
    if (cartTarget.size() != 6) return false;

//     qDebug() << "[RobotController::jogCart] cart=" << cartTarget
//              << "wasCartesian=" << isCartesianMode();

    switchToCartesianMode();

    ZmotionAPI* api = m_motionController->getZmotionAPI();
    QList<int> virtualAxes = m_config->getVirtualAxisList();
    QList<int> jointAxes   = m_config->getJointAxisList();

    // 给虚拟轴设速度/加速度/减速度（虚拟轴 UNITS=10000 对应 mm）
    {
        QList<double> vSpeeds(virtualAxes.size(), static_cast<double>(speed));
        QList<double> vAccels(virtualAxes.size(), static_cast<double>(accel));
        QList<double> vDecels(virtualAxes.size(), static_cast<double>(decel));
        api->setAxisSpeed(virtualAxes, vSpeeds);
        api->setAxisAccel(virtualAxes, vAccels);
        api->setAxisDecel(virtualAxes, vDecels);
    }

    // 同时给关节轴设运动参数（CONNFRAME 模式下关节轴也要遵守）
    {
        QList<double> speeds(jointAxes.size(), static_cast<double>(speed));
        QList<double> accels(jointAxes.size(), static_cast<double>(accel));
        QList<double> decels(jointAxes.size(), static_cast<double>(decel));
        api->setAxisSpeed(jointAxes, speeds);
        api->setAxisAccel(jointAxes, accels);
        api->setAxisDecel(jointAxes, decels);
    }

    // 用 directMoveAbsolute 替代 setAxisDpos，带运动规划平滑加减速
    QList<float> targets;
    for (double pos : cartTarget) targets.append(static_cast<float>(pos));
    api->directMoveAbsolute(virtualAxes, targets, static_cast<float>(speed),
                            static_cast<float>(accel), static_cast<float>(decel));
    return true;
}

/**
 * @brief 停止机器人运动
 *
 * 调用zmotionAPI的rapidStop接口快速停止所有轴运动
 * @return bool 停止成功返回true，未连接返回false
 */
bool RobotController::stopMotion()
{
    if (!isConnected()) {
        return false;
    }

    ZmotionAPI* zmotionAPI = m_motionController->getZmotionAPI();
    return zmotionAPI->rapidStop().success;
}

/**
 * @brief 获取机器人状态字符串
 *
 * @return QString 状态描述字符串，如"空闲"、"运动中"等，未初始化返回"未初始化"
 */
QString RobotController::getRobotStatusStr() const
{
    if (!m_state) {
        return "未初始化";
    }
    return m_state->getStatusStr();
}

/**
 * @brief 获取单轴当前位置
 *
 * @param axisIndex 轴索引
 * @return double 当前角度位置（度），未初始化返回0.0
 */
double RobotController::getAxisPosition(int axisIndex) const
{
    if (!m_state) {
        return 0.0;
    }
    return m_state->getAxisPosition(axisIndex);
}

/**
 * @brief 获取所有轴位置
 *
 * @return QList<double> 所有轴位置的列表，未初始化返回空列表
 */
QList<double> RobotController::getAllAxisPositions() const
{
    if (!m_state) {
        return QList<double>();
    }
    return m_state->getAllAxisPositions();
}

/**
 * @brief 连接信号槽
 *
 * 将MotionController和RobotState的信号连接到RobotController的响应槽函数，
 * 实现状态变化的统一管理和事件转发。
 */
void RobotController::connectSignals()
{
    // 连接MotionController的信号
    connect(m_motionController, &MotionController::connected, this, &RobotController::onHardwareConnected);
    connect(m_motionController, &MotionController::disconnected, this, &RobotController::onHardwareDisconnected);
    connect(m_motionController, &MotionController::connectionError, this, [this](const QString& errorMsg) {
        emit hardwareConnectionFailed(errorMsg);
    });
    connect(m_motionController, &MotionController::hardwareError, this, &RobotController::onHardwareError);
    connect(m_motionController, &MotionController::motionStarted, this, &RobotController::onMotionStarted);
    connect(m_motionController, &MotionController::motionCompleted, this, &RobotController::onMotionCompleted);
    connect(m_motionController, &MotionController::statusUpdated, this, &RobotController::onStatusUpdated);

    // 连接RobotState的信号
    connect(m_state, &RobotState::statusChanged, this, [this]() {
        emit robotStatusChanged(getRobotStatusStr());
    });
    connect(m_state, &RobotState::errorOccurred, this, &RobotController::errorOccurred);

    if (m_homingController) {
        connect(m_homingController, &HomingController::homingCompleted, this, [this]() {
            // 回零完成后，移动到原点位置（Flash优先，config兜底）
            QList<double> originPos = getOriginPosition();
            if (originPos.size() >= 6) {
                QList<float> positions;
                for (int i = 0; i < 6; ++i) {
                    positions.append(static_cast<float>(originPos[i]));
                }
                float speed = static_cast<float>(m_config->getHomeMoveSpeed());
                float accel = static_cast<float>(m_config->getHomeMoveAccel());
                float decel = static_cast<float>(m_config->getHomeMoveDecel());
                m_homingController->moveAllAxesToPositions(positions, speed, accel, decel);

                // 等待所有轴到达原点位置后再继续后续流程
                ZmotionAPI* api = getMotionController()->getZmotionAPI();
                api->waitForAxesIdle({0, 1, 2, 3, 4, 5}, 15000);

                // 物理已移动到原点位置 P，把 DPOS 与 MPOS 同时清 0，
                // 使 DPOS=0 ↔ 处于 Flash 原点。维持"原点设置"公式所需的
                // "DPOS 是相对当前 Flash 原点的偏移"不变量，避免先回零再设原点时双倍计数。
                // 必须同时清 DPOS+MPOS（闭环伺服两者不一致会瞬冲）。
                ZmotionResult zeroRet = api->clearAxisPosition({0, 1, 2, 3, 4, 5});
                if (!zeroRet.success) {
                    qWarning() << "[RobotController] 回零后清零 DPOS/MPOS 失败:" << zeroRet.errorMessage;
                }
            }
            emit operationCompleted(QStringLiteral("回零"));
        });
        connect(m_homingController, &HomingController::homingFailed, this, [this](const QString& errorMsg) {
            emit operationFailed(QStringLiteral("回零"), errorMsg);
        });
    }
}

/**
 * @brief 硬件连接响应槽
 *
 * 接收MotionController的connected信号，发射hardwareConnected信号。
 */
void RobotController::onHardwareConnected()
{
    emit hardwareConnected();
}

/**
 * @brief 硬件断开响应槽
 *
 * 接收MotionController的disconnected信号，发射hardwareDisconnected信号。
 */
void RobotController::onHardwareDisconnected()
{
    emit hardwareDisconnected();
}

/**
 * @brief 硬件错误响应槽
 *
 * 接收MotionController的hardwareError信号，发射errorOccurred信号。
 *
 * @param errorCode 错误码
 * @param errorMsg 错误信息
 */
void RobotController::onHardwareError(int errorCode, const QString& errorMsg)
{
    emit errorOccurred(errorCode, errorMsg);
}

/**
 * @brief 运动开始响应槽
 *
 * 接收轴运动开始信号，输出调试信息。
 *
 * @param axisIndex 开始的轴索引
 */
void RobotController::onMotionStarted(int axisIndex)
{
//     qDebug() << "轴" << axisIndex << "开始运动";
}

/**
 * @brief 运动完成响应槽
 *
 * 接收轴运动完成信号，输出调试信息。
 *
 * @param axisIndex 完成的轴索引
 */
void RobotController::onMotionCompleted(int axisIndex)
{
//     qDebug() << "轴" << axisIndex << "运动完成";
}

/**
 * @brief 状态更新响应槽
 *
 * 接收状态更新信号，发射robotStatusChanged信号。
 */
void RobotController::onStatusUpdated()
{
    emit robotStatusChanged(getRobotStatusStr());
}

// ==================== 原点管理（Flash 断电保持）====================

/**
 * @brief 将当前姿态设为自定义原点，写入控制器Flash
 *
 * 新原点绝对姿态 = "Flash 中当前已保存的原点" + 当前 6 轴 DPOS。
 *
 * 语义：以**当前生效的原点**为基线，加上本次示教产生的 DPOS 增量。
 * 这样用户多次设置原点时，第二次保存的是"在第一次基础上又调整了多少"，
 * 不会因为基线是出厂 homePosition 而被反复覆盖回滚。
 *
 * - 首次保存：Flash 为空（或 ensureOriginInitialized 刚 seed 的默认 home），
 *   等价于 "homePosition + DPOS"，符合出厂基准。
 * - 后续保存：基线 = Flash 已有值，DPOS 是相对该基线的偏移，因此是真正的
 *   "在原基础上做调整"。
 *
 * 写入前把每个角度 round 到 3 位小数，避免 float 存储把不必要的位占满。
 *
 * 仅在用户主动点击"原点设置"按钮时调用，避免 Flash 写入寿命消耗。
 *
 * @return true=写入成功，false=未连接/读取失败/写入失败
 */
bool RobotController::setOriginFromCurrentPos()
{
    if (!isConnected()) {
        emit operationFailed(QStringLiteral("原点设置"), QStringLiteral("控制器未连接"));
        return false;
    }

    ZmotionAPI* api = m_motionController->getZmotionAPI();

    // 读取当前 6 轴 DPOS（相对"当前生效原点"的偏移）
    QList<double> dpos;
    for (int i = 0; i < 6; ++i) {
        ZmotionResult ret = api->getAxisPosition(i);
        if (!ret.success) {
            emit operationFailed(QStringLiteral("原点设置"),
                QStringLiteral("读取轴%1位置失败: %2").arg(i).arg(ret.errorMessage));
            return false;
        }
        dpos.append(static_cast<double>(ret.axisPosition));
    }

    // 取"当前生效的原点"作为基线：
    //   - 优先读 Flash 中上一次保存的原点
    //   - Flash 为空时回退到 config 的 homePosition（与 ensureOriginInitialized 一致）
    QList<double> baseOrigin = getOriginPosition();

    // 在基线上叠加本次 DPOS 增量，并 round 到 3 位小数
    QList<double> newOrigin(6, 0.0);
    for (int i = 0; i < 6; ++i) {
        const double base = (i < baseOrigin.size()) ? baseOrigin[i] : 0.0;
        double v = base + dpos[i];
        v = std::round(v * 1000.0) / 1000.0;
        newOrigin[i] = v;
    }

    qInfo() << "[RobotController] 原点设置，baseOrigin =" << baseOrigin
            << "+ DPOS =" << dpos << "= newOrigin" << newOrigin;

    ZmotionResult flashRet = api->saveOriginToFlash(newOrigin);
    if (!flashRet.success) {
        emit operationFailed(QStringLiteral("原点设置"), flashRet.errorMessage);
        return false;
    }

    emit operationCompleted(QStringLiteral("原点设置"));
    return true;
}

/**
 * @brief 获取原点位置（Flash优先，config兜底）
 *
 * 从控制器Flash读取用户自定义的原点绝对位置；
 * Flash中无数据时返回 RobotConfig 的 homePosition 默认值（来自机型规格）。
 *
 * @return 原点位置列表（6个元素）
 */
QList<double> RobotController::getOriginPosition()
{
    if (isConnected()) {
        ZmotionAPI* api = m_motionController->getZmotionAPI();
        QList<double> flashPos;
        ZmotionResult readRet = api->readOriginFromFlash(flashPos);
        if (readRet.success && flashPos.size() == 6) {
            qInfo() << "[RobotController] 从Flash读取原点位置:" << flashPos;
            return flashPos;
        }
    }

    // Flash为空或未连接，使用配置文件默认值
    QList<double> homePos = m_config->getHomePosition();
    qInfo() << "[RobotController] Flash无数据，使用配置homePosition:" << homePos;
    return homePos;
}

/**
 * @brief 确保Flash中已有出厂默认原点
 *
 * 首次连接成功后调用。若Flash中无有效原点（空块/魔数校验失败），
 * 将出厂默认原点写入Flash；若已有原点则直接返回，绝不覆盖用户设置。
 */
void RobotController::ensureOriginInitialized()
{
    if (!isConnected()) {
        return;
    }
    ZmotionAPI* api = m_motionController->getZmotionAPI();
    QList<double> dummy;
    ZmotionResult readRet = api->readOriginFromFlash(dummy);
    if (readRet.success && dummy.size() == 6) {
        // Flash中已有有效原点（出厂默认或用户自定义），无需写入
        qInfo() << "[RobotController] Flash中已有原点，跳过默认seed:" << dummy;
        return;
    }
    // Flash为空，把配置文件中的出厂默认原点直接写入Flash；之后一律以Flash为基准
    QList<double> seedHome = m_config->getHomePosition();
    ZmotionResult writeRet = api->saveOriginToFlash(seedHome);
    if (writeRet.success) {
        qInfo() << "[RobotController] Flash为空，已写入出厂默认原点:" << seedHome;
    } else {
        qWarning() << "[RobotController] 写入出厂默认原点失败:" << writeRet.errorMessage;
    }
}

/**
 * @brief 切换到笛卡尔空间控制（逆解模式）
 *
 * 通过MotionController切换到逆解模式，允许通过笛卡尔坐标控制机器人。
 *
 * @return true 切换成功，false 切换失败或硬件未连接
 */
bool RobotController::switchToCartesianMode()
{
    if (!isConnected()) {
        emit operationFailed(QStringLiteral("切换笛卡尔模式"), QStringLiteral("硬件未连接"));
        return false;
    }

    // 已在逆解模式，跳过
    if (isCartesianMode()) {
        return true;
    }

    emit operationStarted(QStringLiteral("切换笛卡尔模式"));

    bool result = m_motionController->setInverseKinematicsMode();

    if (result) {
        m_state->setStatus(RobotState::StatusCartesianMode);
        emit operationCompleted(QStringLiteral("切换笛卡尔模式"));
    } else {
        emit operationFailed(QStringLiteral("切换笛卡尔模式"), QStringLiteral("切换失败"));
    }

    return result;
}

/**
 * @brief 切换到关节空间控制（正解模式）
 *
 * 通过MotionController切换到正解模式，允许通过关节角度控制机器人。
 *
 * @return true 切换成功，false 切换失败或硬件未连接
 */
bool RobotController::switchToJointMode()
{
    if (!isConnected()) {
        emit operationFailed(QStringLiteral("切换关节模式"), QStringLiteral("硬件未连接"));
        return false;
    }

    // 已在正解模式，跳过（精确判断模式值，-1 未知态不跳过，需重建管线）
    if (m_motionController->getZmotionAPI()->getCurrentMotionMode() == 0) return true;

    emit operationStarted(QStringLiteral("切换关节模式"));

    bool result = m_motionController->setForwardKinematicsMode();

    if (result) {
        m_state->setStatus(RobotState::StatusJointMode);
        emit operationCompleted(QStringLiteral("切换关节模式"));
    } else {
        emit operationFailed(QStringLiteral("切换关节模式"), QStringLiteral("切换失败"));
    }

    return result;
}

/**
 * @brief 获取当前是否为笛卡尔模式
 *
 * 查询当前的运动模式，判断是否为笛卡尔空间控制模式。
 *
 * @return true 当前为笛卡尔模式，false 当前为关节模式或未连接
 */
bool RobotController::isCartesianMode() const
{
    if (!isConnected()) {
        return false;
    }
    return m_motionController->isInverseKinematicsMode();
}
