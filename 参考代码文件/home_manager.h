/**
 * @file home_manager.h
 * @brief 回零管理器头文件
 * @details 定义回零管理器相关的结构体、枚举和类，用于控制机械臂的回零流程
 */
#pragma once
#ifndef HOME_MANAGER_H
#define HOME_MANAGER_H

#include <array>
#include <cstdint>

#include "motion_driver.h"
#include "../../Utils/robot_config.h"

/**
 * @brief 轴硬件配置结构体
 * @details 存储每个轴的硬件配置信息，包括轴号、类型、方向、原点输入、反向限位输入等
 */
struct HomeAxisHWConfig
{
    int axis;              ///< 轴号
    int atype;             ///< 轴类型
    int dir;               ///< 轴方向
    int homeIn;            ///< 原点输入IO编号
    int negIn;             ///< 反向限位输入IO编号（-1表示无）
};

/**
 * @brief 回零策略结构体
 * @details 存储每个轴的回零策略参数，包括移开原点距离、移开反向限位距离、回零速度、加速度等
 */
struct HomeStrategy
{
    float moveAwayOrigin;   ///< 移开原点的距离
    float moveAwayReverse;  ///< 移开反向限位的距离
    float homeSpeed;        ///< 回零速度
    float homeAccel;        ///< 回零加速度
    float homeDecel;        ///< 回零减速度
    float homeLspeed;       ///< 回零慢速阈值
    float homeSramp;        ///< 回零S曲线过渡时间
    float homeCreep;        ///< 回零爬行速度
    int datumMode;          ///< 回零模式
};

/**
 * @brief 回零轴状态枚举
 * @details 定义回零过程中轴的各种状态
 */
enum HomeAxisState
{
    HOME_INIT = 0,                 ///< 回零初始化状态
    HOME_CHECK_SENSOR,             ///< 检查传感器状态
    HOME_MOVE_AWAY_ORIGIN,         ///< 移开原点状态
    HOME_WAIT_MOVE_AWAY_ORIGIN,    ///< 等待移开原点完成状态
    HOME_MOVE_AWAY_REVERSE,        ///< 移开反向限位状态
    HOME_WAIT_MOVE_AWAY_REVERSE,   ///< 等待移开反向限位完成状态
    HOME_START_DATUM,              ///< 开始回零状态
    HOME_WAIT_DATUM,               ///< 等待回零完成状态
    HOME_SUCCESS,                  ///< 回零成功状态
    HOME_FAIL                      ///< 回零失败状态
};

/**
 * @brief 回零运行时参数结构体
 * @details 存储回零过程中的运行时参数，使用 GetTickCount64() 进行 3 分钟超时控制
 */
struct HomeRuntime
{
    uint64_t start_time_ms = 0; ///< 进入等待回零状态时的起始时间（毫秒），用于 3 分钟超时判断
};

/**
 * @brief 回零管理器类
 * @details 管理机械臂的回零流程，包括串行预处理和并行回零
 */
class HomeManager
{
public:
    static constexpr int kAxisCount = ROBOT_AXIS_NUM;  ///< 轴数量

    /**
     * @brief 绑定运动驱动
     * @param driver 运动驱动指针
     * @details 将运动驱动与回零管理器绑定，以便后续控制机械臂运动
     */
    void bind_driver(MotionDriver* driver);

    /**
     * @brief 重置回零状态
     * @details 将所有轴的回零状态重置为初始状态，包括回零标志、运行时参数等
     */
    void reset();

    /**
     * @brief 执行回零步骤
     * @details 先执行串行预处理，然后执行并行回零
     */
    void step();

    /**
     * @brief 检查是否所有轴都已回零
     * @return 所有轴都已回零返回true，否则返回false
     * @details 遍历所有轴的回零标志，只要有一个轴未回零就返回false
     */
    bool is_all_homed() const;

    /**
     * @brief 获取回零标志数组
     * @return 回零标志数组的常量引用
     * @details 返回每个轴的回零状态，true表示已回零，false表示未回零
     */
    const std::array<bool, kAxisCount>& homed_flags() const;

    /**
     * @brief 移动所有轴到指定位置
     * @param positions 各轴目标位置数组
     * @details 控制每个轴移动到指定的绝对位置
     */
    void move_all_axes_to_positions(const std::array<float, kAxisCount>& positions);

    /**
     * @brief 多轴直线插补
     * @param positions 各轴目标位置数组
     * @details 控制多个轴同时移动到指定位置，实现直线插补运动
     */
    void multi_axis_interpolation(const std::array<float, kAxisCount>& positions);

    // /**
    //  * @brief 测试IO状态
    //  * @details 循环读取并显示每个轴的原点输入和反向输入状态
    //  */
    // void test_io();

private:
    /**
     * @brief 执行串行预处理步骤
     * @details 按优先级处理J3、J2、J5轴的预处理，确保它们移开原点
     */
    void step_serial();

    /**
     * @brief 处理单个轴的预处理
     * @param i 轴索引
     * @details 处理指定轴的预处理，包括初始化参数、检查传感器状态、移开原点等
     */
    void process_prehome_axis(int i);

    /**
     * @brief 执行并行回零步骤
     * @details 所有轴同时执行回零流程，包括初始化、检查传感器、移开原点、回零等
     */
    void step_parallel();

private:
    MotionDriver* driver_ = nullptr;           ///< 运动驱动指针
    bool serial_prehome_completed_ = false;    ///< 串行预处理完成标志

    std::array<HomeAxisState, kAxisCount> home_state_{};  ///< 各轴回零状态数组
    std::array<HomeRuntime, kAxisCount> home_runtime_{};  ///< 各轴回零运行时参数数组
    std::array<bool, kAxisCount> is_homed_{};             ///< 各轴回零标志数组

    bool j2_moved_away_ = false;  ///< J2轴移开原点标志
    bool j3_moved_away_ = false;  ///< J3轴移开原点标志
    bool j5_moved_away_ = false;  ///< J5轴移开原点标志
};

#endif // HOME_MANAGER_H
