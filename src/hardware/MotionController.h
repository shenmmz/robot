/*
 * @file MotionController.h
 * @brief 声明硬件驱动层控制器，用于机械臂运动命令和硬件通信管理。
 *
 * 本文件定义 MotionController 类，该类负责：
 * - 与 ZmotionAPI 通信
 * - 管理连接状态
 * - 更新机械臂状态
 * - 错误处理和异常恢复
 * 
 * 注意：运动控制方法（如moveAxis、stopAxis等）已移至 ZmotionAPI，
 * MotionController 仅负责连接管理、状态更新和信号转发。
 */
#ifndef MOTIONCONTROLLER_H
#define MOTIONCONTROLLER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QTimer>
#include "../models/RobotState.h"
#include "../models/RobotConfig.h"
#include "../../ZmotionAPI/ZmotionAPI.h"

/**
 * @brief 运动控制器
 * 
 * 这是硬件驱动层的核心类，负责：
 * 1. 与硬件通信（通过ZmotionAPI）
 * 2. 管理连接状态
 * 3. 实时更新机械臂状态（RobotState）
 * 4. 错误处理和异常恢复
 * 5. 信号转发
 * 
 * 工作流程：
 * MainWindow -> RobotController -> MotionController -> ZmotionAPI -> 硬件
 * 
 * 使用说明：
 * 1. 先调用 connect() 连接到硬件
 * 2. 通过 getZmotionAPI() 获取 ZmotionAPI 实例进行运动控制
 * 3. 通过信号获取实时状态更新
 */
class MotionController : public QObject
{
    Q_OBJECT

public:
    explicit MotionController(RobotConfig* config, RobotState* state, QObject *parent = nullptr);
    ~MotionController();

    // ===== 连接管理 =====
    bool connect(const QString& ipAddr);
    bool disconnect();
    bool isConnected() const { return m_isConnected; }

    // ===== 获取 ZmotionAPI 实例 =====
    ZmotionAPI* getZmotionAPI() const { return m_zmotionAPI; }

    // ===== 状态管理 =====
    void updateState();  // 从硬件读取最新状态并更新RobotState
    
    // ===== 更新频率配置 =====
    void setStateUpdateInterval(int intervalMs);  // 设置状态更新间隔（毫秒）
    int getStateUpdateInterval() const { return m_updateIntervalMs; }  // 获取更新间隔

    // ===== 正逆解模式控制 =====
    bool setInverseKinematicsMode();    // 切换到逆解模式（笛卡尔空间控制）
    bool setForwardKinematicsMode();    // 切换到正解模式（关节空间控制）
    bool isInverseKinematicsMode() const; // 获取当前是否为逆解模式

signals:
    // 连接相关信号
    void connected();
    void disconnected();
    void connectionError(const QString& errorMsg);

    // 运动相关信号
    void motionStarted(int axisIndex);
    void motionStopped(int axisIndex);
    void motionCompleted(int axisIndex);
    
    // 错误相关信号
    void hardwareError(int errorCode, const QString& errorMsg);
    void statusUpdated(); // 状态更新完成
    
    // 正逆解模式切换信号
    void kinematicsModeChanged(bool isInverseMode); // true表示逆解模式，false表示正解模式

private:
    // ===== 内部成员 =====
    RobotConfig* m_config;              // 配置指针
    RobotState* m_state;                // 状态指针
    ZmotionAPI* m_zmotionAPI;           // 下层硬件API
    bool m_isConnected;                 // 连接状态标志
    
    // ===== 状态更新定时器 =====
    QTimer* m_updateTimer;              // 状态更新定时器
    int m_updateIntervalMs;             // 更新间隔（毫秒），默认100ms

    // ===== 内部辅助方法 =====
    bool checkConnected();              // 检查连接状态
    void handleZmotionError(const ZmotionResult& result); // 处理ZmotionAPI的错误
    void updateAxisState(int axisIndex); // 更新单轴状态
};

#endif // MOTIONCONTROLLER_H
