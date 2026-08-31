/*
 * @file RobotController.h
 * @brief 声明业务逻辑层的核心控制器类。
 *
 * 本文件定义 RobotController 类，用来协调：
 * - 配置（RobotConfig）
 * - 状态（RobotState）
 * - 硬件控制（MotionController）
 * - UI 层的请求和事件
 */
/**
 * @brief 机械臂控制器 - 业务逻辑层
 * 
 * 这是应用的核心业务逻辑层，负责：
 * 1. 协调各个模块（配置、状态、硬件）
 * 2. 实现高级的机械臂操作（如回零、示教、重放）
 * 3. 错误处理和恢复
 * 4. 与UI层通信（通过信号）
 * 
 * 架构图：
 * MainWindow (UI)
 *     |
 *     v
 * RobotController (业务逻辑)
 *     |
 *     +----> MotionController (硬件控制)
 *     |
 *     +----> RobotState (状态)
 *     |
 *     +----> RobotConfig (配置)
 * 
 * 使用说明：
 * 1. UI层调用RobotController提供的公共方法
 * 2. RobotController协调各个模块完成任务
 * 3. 通过信号向UI层报告结果
 */
#ifndef ROBOTCONTROLLER_H
#define ROBOTCONTROLLER_H

#include <QObject>
#include <QString>
#include "RobotState.h"
#include "RobotConfig.h"
#include "HomingController.h"
#include "../hardware/MotionController.h"
#include "../hardware/ServoController.h"
#include "../hardware/Rs485Controller.h"
#include "../../ZmotionAPI/ZmotionAPI.h"


class RobotController : public QObject
{
    Q_OBJECT

public:
    explicit RobotController(QObject *parent = nullptr);
    ~RobotController();

    // ===== 初始化 =====
    // 按机型规格构建配置（不再依赖 robot_config.json，单一数据源在 RobotModelRegistry）。
    // 默认 R01；连上控制器后由 Flash 读取的型号 ID 可经此重载切换（当前 R01 为唯一实装机型）。
    bool initialize(RobotModelId modelId = RobotModelId::R01);
    bool initialize(RobotConfig* config);

    // ===== 连接管理 =====
    bool connectHardware(const QString& ipAddr);
    bool disconnectHardware();
    bool isConnected() const;

    // ===== 网络扫描 =====
    QStringList searchControllers();

    // ===== 机器人模型初始化 =====
    bool initRobotModel();

    // ===== 获取状态和配置 =====
    RobotState* getState() const { return m_state; }
    RobotConfig* getConfig() const { return m_config; }
    MotionController* getMotionController() const { return m_motionController; }
    ServoController* getServoController() const { return m_servoController; }
    Rs485Controller* getRs485Controller() const { return m_rs485Controller; }

    // ===== 高级运动控制 =====
    bool executeHoming();
    void stopHoming();
    HomingController* getHomingController() const { return m_homingController; }
    
    /**
     * @brief 执行安全停止
     * 先减速停止，然后禁用轴
     */
    bool executeSafeStop();

    /**
     * @brief 移动到预设位置
     * 支持如"初始位置"、"工作位置"等预设位置
     */
    bool moveToPreset(const QString& presetName);

    // ===== 轴级控制 =====
    bool moveAxis(int axisIndex, double targetPos, double speed = -1, double accel = -1, double decel = -1);
    bool stopMotion();

    // ===== 关节空间运动 =====
    // moveJ: 正解模式下多轴直线插补，等待到位（阻塞）
    bool moveJ(const QList<double>& jointPos, double speed, double accel, double decel, int timeoutMs = 30000);

    // jogJoint: 关节空间点动，Cancel → Move → 立即返回，不等待
    bool jogJoint(int axis, double targetPos, double speed, double accel, double decel);

    // jogJointBatch: 批量多轴点动，一次读取当前位置，叠加目标后一次 directMoveAbsolute 下发
    bool jogJointBatch(const QHash<int, double>& axisTargets, double speed, double accel, double decel);

    // ===== 笛卡尔空间运动 =====
    // moveL: 逆解模式下笛卡尔直线运动，等待到位（阻塞）
    bool moveL(const QList<double>& cartPos, double speed, double accel, double decel, int timeoutMs = 30000);

    // jogCart: 笛卡尔空间点动，Cancel → Move → 立即返回，不等待
    bool jogCart(const QList<double>& cartTarget, double speed, double accel, double decel);

    // ===== 原点管理（Flash 断电保持）=====
    /**
     * @brief 将当前6轴DPOS设为自定义原点，写入控制器Flash
     *
     * 用户在点动控制中手动调整好机械臂位置后调用，
     * 将当前各轴DPOS作为"新原点"持久化到Flash，断电不丢失。
     *
     * @return true=成功，false=失败
     */
    bool setOriginFromCurrentPos();

    /**
     * @brief 获取原点位置（Flash优先，config兜底）
     *
     * 从控制器Flash读取自定义原点位置；
     * 如果Flash中无数据，返回 RobotConfig 的 homePosition 默认值（来自机型规格）。
     *
     * @return 6轴原点位置列表
     */
    QList<double> getOriginPosition();

    // 注意：清除自定义原点的功能已移除。如需恢复出厂默认原点，
    // 请重新执行"回零"流程，会自动走 kDefaultHomePosition 作为初始位置。

    // ===== 正逆解模式控制 =====
    bool switchToCartesianMode();      // 切换到笛卡尔空间控制（逆解模式）
    bool switchToJointMode();          // 切换到关节空间控制（正解模式）
    bool isCartesianMode() const;      // 获取当前是否为笛卡尔模式

    // ===== 状态获取 =====
    QString getRobotStatusStr() const;
    double getAxisPosition(int axisIndex) const;
    QList<double> getAllAxisPositions() const;

signals:
    // 初始化信号
    void initialized();
    void initializationFailed(const QString& errorMsg);

    // 连接信号
    void hardwareConnected();
    void hardwareDisconnected();
    void hardwareConnectionFailed(const QString& errorMsg);

    // 建模信号
    void modelInitSuccess();
    void modelInitFailed(int errorCode);

    // 操作信号
    void operationStarted(const QString& operationName);
    void operationCompleted(const QString& operationName);
    void operationFailed(const QString& operationName, const QString& errorMsg);

    // 状态信号
    void robotStatusChanged(const QString& statusStr);
    void errorOccurred(int errorCode, const QString& errorMsg);

public slots:
    // 来自硬件层的信号处理
    void onHardwareConnected();
    void onHardwareDisconnected();
    void onHardwareError(int errorCode, const QString& errorMsg);
    void onMotionStarted(int axisIndex);
    void onMotionCompleted(int axisIndex);
    void onStatusUpdated();

private:
    RobotConfig* m_config;              // 配置
    RobotState* m_state;                // 状态
    MotionController* m_motionController; // 硬件控制器
    HomingController* m_homingController; // 回原点控制器
    Rs485Controller* m_rs485Controller;  // RS485控制器（外部PWM模块）
    ServoController* m_servoController;  // 舵机控制器
    bool m_initialized;                 // 初始化标志

    // 内部辅助方法
    void connectSignals();

    /**
     * @brief 确保Flash中已有出厂默认原点
     *
     * 在首次连接成功后调用：若Flash为空（无有效原点），
     * 将出厂默认原点 [84.999, 68.62, -51.944, 4.229, 71, 90] 写入Flash，
     * 使Flash永远持有一个有效home；已有原点则不覆盖（保留用户最新设置）。
     */
    void ensureOriginInitialized();
};

#endif // ROBOTCONTROLLER_H
