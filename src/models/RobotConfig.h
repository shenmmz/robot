/*
 * @file RobotConfig.h
 * @brief 声明机械臂配置数据模型。配置由机型规格（RobotModelRegistry）构建，
 *        不再从外部 JSON 文件读取/保存。
 *
 * 本文件定义 RobotConfig 类，用于存储机械臂的:
 * - 轴数、命名、控制器地址
 * - 各轴速度/加速度/限位参数
 * - 由机型规格初始化的全部字段
 */
#ifndef ROBOTCONFIG_H
#define ROBOTCONFIG_H

#include <QString>
#include <QList>
#include <QHash>
#include "src/models/RobotModelId.h"   // 机型枚举（initializeFromModel 参数）
#include "src/models/RobotModels.h"   // HomeSpec/RobotModelSpec（回零覆盖表值类型）

/**
 * @brief 机械臂配置数据模型
 * 
 * 存储机械臂的配置参数，包括：
 * - 轴数
 * - 各轴的速度、加速度上限
 * - 各轴的软件限位
 * - 机械臂的DH参数
 * 
 * 配置来源：通过 initializeFromModel(RobotModelId) 从代码侧注册表
 * （RobotModels.cpp）按机型规格填充，单一数据源，不依赖外部 JSON 文件。
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
    void setHomePosition(const QList<double>& pos) { m_homePosition = pos; }    // 设置回零后目标位置
    double getHomeMoveSpeed() const { return m_homeMoveSpeed; }         // 获取回零后移动速度(°/s)
    double getHomeMoveAccel() const { return m_homeMoveAccel; }         // 获取回零后移动加速度(°/s²)
    double getHomeMoveDecel() const { return m_homeMoveDecel; }         // 获取回零后移动减速度(°/s²)
    bool moveToHomeAfterHoming() const { return m_moveToHomeAfterHoming; } // 回零成功后是否移动到 homePosition（false=停 DATUM 位只清坐标）

    // 关机安全位姿（6轴关节角，°）：来自机型 spec，不写 Flash
    QList<double> getShutdownPosition() const { return m_shutdownPosition; }
    void setShutdownPosition(const QList<double>& pos) { m_shutdownPosition = pos; }

    AxisConfig getAxisConfig(int axisIndex) const;    // 获取指定轴的配置

    // ===== 机器人模型参数 =====
    RobotModelId modelId() const { return m_modelId; }  // 当前生效机型 ID
    const AxisSpec& getAxisSpec(int axisIndex) const;  // 获取指定轴的型号规格（含 home 配置）
    QList<int> getJointAxisList() const;            // 获取关节轴列表
    QList<int> getVirtualAxisList() const;          // 获取虚拟轴列表
    QList<double> getAxisUnitsList() const;        // 获取轴单位列表

    // ===== 运行时回零参数覆盖（HomeOverride） =====
    // 覆盖优先于机型注册表默认；键 = (modelId, axisIndex)，各机型独立
    const HomeSpec& homeSpecFor(int axisIndex) const;                    // 回零消费点统一入口（按当前机型）
    const HomeSpec& homeOverrideFor(RobotModelId id, int axisIndex) const; // 指定机型覆盖值（无覆盖回注册表默认）
    bool hasHomeOverride(RobotModelId id, int axisIndex) const;
    void setHomeOverride(RobotModelId id, int axisIndex, const HomeSpec& h);
    bool clearHomeOverride(RobotModelId id, int axisIndex);
    bool clearAllHomeOverrides(RobotModelId id);                         // "恢复默认"用
    bool hasAnyHomeOverrides() const;                                    // 面板状态显示用
    bool loadHomeOverrides(const QString& iniPath);                      // 启动时调用
    bool saveHomeOverrides(const QString& iniPath) const;

    // ===== Setter =====
    void setRobotName(const QString& name) { m_robotName = name; }    // 设置机械臂名称
    void setControllerIP(const QString& ip) { m_controllerIP = ip; }    // 设置控制器IP地址
    void setStateUpdateInterval(int intervalMs) { m_stateUpdateIntervalMs = intervalMs; }    // 设置状态更新间隔(ms)

    // ===== 机型规格初始化 =====
    // 由 RobotModelRegistry 的指定机型规格填充全部字段（单一数据源，不再依赖 JSON 文件）
    void initializeFromModel(RobotModelId id);

    // ===== 工具方法 =====
    QString toString() const;    // 配置信息字符串表示（调试用）

private:
    int m_axisCount;                    // 轴数
    QString m_robotName;                // 机械臂名称
    QString m_controllerIP;             // 控制器IP地址
    int m_stateUpdateIntervalMs;        // 状态更新间隔（毫秒）

    QList<AxisConfig> m_axisConfigs;    // 各轴配置
    RobotModelId m_modelId = RobotModelId::Unknown;  // 当前机型 ID（Unknown 回退后存 R01）
    QHash<quint64, HomeSpec> m_homeOverrides;  // 运行时回零参数覆盖表：(modelId<<16)|axis -> HomeSpec
    QList<double> m_homePosition;       // 回零后目标位置（6轴关节角度）
    QList<double> m_shutdownPosition;   // 关机安全位姿（6轴关节角度，来自机型 spec）
    double m_homeMoveSpeed;             // 回零后移动速度(°/s)
    double m_homeMoveAccel;             // 回零后移动加速度(°/s²)
    double m_homeMoveDecel;             // 回零后移动减速度(°/s²)
    bool m_moveToHomeAfterHoming = true; // 回零成功后是否移动到 homePosition（initializeFromModel 按机型填充）
};

#endif // ROBOTCONFIG_H
