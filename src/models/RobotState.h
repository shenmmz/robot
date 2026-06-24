/*
 * @file RobotState.h
 * @brief 声明机械臂运行时状态数据模型。
 *
 * 本文件定义 RobotState 类，包含：
 * - 各轴的当前位置、速度、加速度
 * - 机械臂整体运行状态
 * - 错误码与错误信息
 */
#ifndef ROBOTSTATE_H
#define ROBOTSTATE_H

#include <QString>
#include <QList>
#include <QObject>

/**
 * @brief 机械臂状态数据模型
 * 
 * 存储机械臂的实时状态信息，包括：
 * - 各轴位置、速度、加速度
 * - 机械臂总体状态（就绪、运动中、错误）
 * - 各轴错误状态
 * 
 * 使用说明：
 * 1. 此类是"纯数据类"，只存储状态，不进行逻辑操作
 * 2. 各个模块都可以读取此类信息
 * 3. 只有控制器可以修改此类信息
 */
class RobotState : public QObject
{
    Q_OBJECT

public:
    enum RobotStatus {
        StatusIdle = 0,        // 空闲状态
        StatusMoving = 1,      // 运动中
        StatusError = 2,       // 错误状态
        StatusEmergencyStop = 3, // 紧急停止
        StatusCartesianMode = 4, // 笛卡尔空间控制模式（逆解模式）
        StatusJointMode = 5      // 关节空间控制模式（正解模式）
    };

    explicit RobotState(int axisCount = 6, QObject *parent = nullptr);

    // ===== Getter 方法 =====
    RobotStatus getStatus() const { return m_status; }
    QString getStatusStr() const;
    
    double getAxisPosition(int axisIndex) const;
    double getAxisVelocity(int axisIndex) const;
    double getAxisAccel(int axisIndex) const;
    
    QList<double> getAllAxisPositions() const { return m_axisPositions; }
    QList<double> getAllAxisVelocities() const { return m_axisVelocities; }
    
    // 笛卡尔坐标相关
    double getCartesianPosition(int index) const;
    QList<double> getAllCartesianPositions() const { return m_cartesianPositions; }
    
    QString getErrorMessage() const { return m_errorMessage; }
    int getErrorCode() const { return m_errorCode; }
    
    int getAxisCount() const { return m_axisCount; }

    // ===== Setter 方法 =====
    void setStatus(RobotStatus status);
    void setAxisPosition(int axisIndex, double position);
    void setAxisVelocity(int axisIndex, double velocity);
    void setAxisAccel(int axisIndex, double accel);
    
    void setAllAxisPositions(const QList<double>& positions);
    void setAllAxisVelocities(const QList<double>& velocities);
    
    // 笛卡尔坐标相关
    void setCartesianPosition(int index, double value);
    void setAllCartesianPositions(const QList<double>& positions);
    
    void setError(int errorCode, const QString& errorMsg);
    void clearError();

    // ===== 工具方法 =====
    QString toString() const; // 调试用：打印整个状态

signals:
    // 状态改变信号
    void statusChanged(RobotStatus oldStatus, RobotStatus newStatus);
    void axisPositionChanged(int axisIndex, double position);
    void errorOccurred(int errorCode, const QString& errorMsg);

private:
    int m_axisCount;                    // 轴数（通常是6）
    RobotStatus m_status;               // 机械臂总体状态
    
    QList<double> m_axisPositions;      // 各轴当前位置
    QList<double> m_axisVelocities;     // 各轴当前速度
    QList<double> m_axisAccels;         // 各轴当前加速度
    
    QList<double> m_cartesianPositions; // 笛卡尔坐标 [X, Y, Z, A, B, C]
    
    int m_errorCode;                    // 最后一次错误码
    QString m_errorMessage;             // 最后一次错误消息
};

#endif // ROBOTSTATE_H
