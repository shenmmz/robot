/*
 * @file RobotConfig.h
 * @brief 声明机械臂配置数据模型及其配置文件读写接口。
 *
 * 本文件定义 RobotConfig 类，用于存储机械臂的:
 * - 轴数、命名、控制器地址
 * - 各轴速度/加速度/限位参数
 * - 配置文件加载与保存方法
 */
#ifndef ROBOTCONFIG_H
#define ROBOTCONFIG_H

#include <QString>
#include <QList>
#include <QJsonObject>

/**
 * @brief 机械臂配置数据模型
 * 
 * 存储机械臂的配置参数，包括：
 * - 轴数
 * - 各轴的速度、加速度上限
 * - 各轴的软件限位
 * - 机械臂的DH参数
 * 
 * 使用说明：
 * 1. 这些参数通常从config.json文件加载
 * 2. 运行过程中可以动态修改，但需要重新校准
 * 3. 修改后应该保存到配置文件
 */
class RobotConfig
{
public:
    struct AxisConfig {
        int axisIndex;
        double maxSpeed;        // 最大速度（度/秒或mm/秒）
        double maxAccel;        // 最大加速度
        double maxDecel;        // 最大减速度
        double homePosition;    // 回零位置

        QList<QString> sensors; // 轴对应的传感器输入
        int reductionRatio;     // 减速比
        QString gearRatio;      // 齿轮比（例如 "1:4"）
        int pulsePerRev;        // 每转脉冲数
        
        AxisConfig() 
            : axisIndex(0), maxSpeed(360), maxAccel(1000), maxDecel(1000),
              homePosition(0),
              reductionRatio(1), gearRatio("1:1"), pulsePerRev(1000) {}
    };

    RobotConfig();

    // ===== Getter =====
    int getAxisCount() const { return m_axisCount; }    // 获取轴数
    QString getRobotName() const { return m_robotName; }    // 获取机械臂名称
    QString getControllerIP() const { return m_controllerIP; }    // 获取控制器IP地址
    int getStateUpdateInterval() const { return m_stateUpdateIntervalMs; }    // 获取状态更新间隔(ms)
    QList<double> getHomePosition() const { return m_homePosition; }    // 获取回零后目标位置
    double getHomeMoveSpeed() const { return m_homeMoveSpeed; }         // 获取回零后移动速度(°/s)
    double getHomeMoveAccel() const { return m_homeMoveAccel; }         // 获取回零后移动加速度(°/s²)
    double getHomeMoveDecel() const { return m_homeMoveDecel; }         // 获取回零后移动减速度(°/s²)

    AxisConfig getAxisConfig(int axisIndex) const;    // 获取指定轴的配置
    QList<AxisConfig> getAllAxisConfigs() const { return m_axisConfigs; }    // 获取所有轴的配置列表

    // ===== 机器人模型参数 =====
    QList<int> getJointAxisList() const;            // 获取关节轴列表
    QList<int> getVirtualAxisList() const;          // 获取虚拟轴列表
    QList<double> getAxisUnitsList() const;        // 获取轴单位列表

    // ===== Setter =====
    void setAxisCount(int count);    // 设置轴数
    void setRobotName(const QString& name) { m_robotName = name; }    // 设置机械臂名称
    void setControllerIP(const QString& ip) { m_controllerIP = ip; }    // 设置控制器IP地址
    void setStateUpdateInterval(int intervalMs) { m_stateUpdateIntervalMs = intervalMs; }    // 设置状态更新间隔(ms)
    void setAxisConfig(int axisIndex, const AxisConfig& config);    // 设置指定轴的配置

    // ===== 文件操作 =====
    bool loadFromJson(const QString& filePath);    // 从 JSON 文件加载配置
    bool saveToJson(const QString& filePath) const;    // 保存配置到 JSON 文件

    // ===== 工具方法 =====
    QString toString() const;    // 保存配置到 JSON 字符串表示

private:
    int m_axisCount;                    // 轴数
    QString m_robotName;                // 机械臂名称
    QString m_controllerIP;             // 控制器IP地址
    int m_stateUpdateIntervalMs;        // 状态更新间隔（毫秒）

    QList<AxisConfig> m_axisConfigs;    // 各轴配置
    QList<double> m_axisUnits;          // 各轴脉冲当量（脉冲/度）
    QList<double> m_homePosition;       // 回零后目标位置（6轴关节角度）
    double m_homeMoveSpeed;             // 回零后移动速度(°/s)
    double m_homeMoveAccel;             // 回零后移动加速度(°/s²)
    double m_homeMoveDecel;             // 回零后移动减速度(°/s²)

    void initializeDefaultConfigs();    // 初始化默认配置
};

#endif // ROBOTCONFIG_H
