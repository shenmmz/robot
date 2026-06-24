/**
 * @file home_manager.cpp
 * @brief 回零管理器实现文件
 * @details 实现机械臂回零相关的功能，包括回零流程控制、轴运动控制、IO测试等
 */
#include "home_manager.h"
#include "motion_driver.h"

#include <QDebug>
#include <windows.h>

namespace {

/**
 * @brief 轴硬件配置数组
 * @details 存储每个轴的硬件配置信息，包括轴号、类型、方向、原点输入、反向限位输入等
 * 轴类型使用 AxisConfig::AXIS_TYPE 常量定义
 */
const HomeAxisHWConfig g_axis_hw[HomeManager::kAxisCount] =
{
    {0, AxisConfig::AXIS_TYPE,  1, 0,  1},   // J1
    {1, AxisConfig::AXIS_TYPE,  1, 2, -1},   // J2
    {2, AxisConfig::AXIS_TYPE, -1, 3, -1},   // J3
    {3, AxisConfig::AXIS_TYPE,  1, 4,  5},   // J4
    {4, AxisConfig::AXIS_TYPE,  1, 6, -1},   // J5
    {5, AxisConfig::AXIS_TYPE,  1, 7,  8}    // J6
};

/**
 * @brief 回零策略数组
 * @details 存储每个轴的回零策略，包括移开原点的距离 移开反向限位的距离 回零速度 回零加速度 回零减速度 回零慢速阈值 回零S曲线过渡时间 回零爬行速度 回零模式
 */
const HomeStrategy g_home_strategy[HomeManager::kAxisCount] =
{//移开原点的距离 移开反向限位的距离 回零速度 回零加速度 回零减速度 回零慢速阈值 回零S曲线过渡时间 回零爬行速度 回零模式
    { -10.0f, -85.0f, 20.0f,  30.0f,  50.0f, 0.0f, 200.0f, 0.5f, 3 },
    {  38.0f,    0.0f,  20.0f,  30.0f,  80.0f, 0.0f, 200.0f, 0.5f, 4 },
    { -50.0f,    0.0f, 20.0f,  30.0f,  80.0f, 0.0f, 200.0f, 0.5f, 3 }, 
    { -10.0f, -200.0f, 20.0f, 30.0f, 50.0f, 0.0f, 200.0f, 0.5f, 3 },
    { 20.0f,    0.0f, 20.0f, 30.0f, 50.0f, 0.0f, 200.0f, 0.5f, 4 }, 
    { -10.0f, -220.0f, 30.0f, 50.0f, 80.0f, 0.0f, 200.0f, 2.0f, 3 }
};

} // namespace

/**
 * @brief 绑定运动驱动
 * @param driver 运动驱动指针
 * @details 将运动驱动与回零管理器绑定，以便后续控制机械臂运动
 */
void HomeManager::bind_driver(MotionDriver* driver)
{
    driver_ = driver;
}

/**
 * @brief 重置回零状态
 * @details 将所有轴的回零状态重置为初始状态，包括回零标志、运行时参数等
 */
void HomeManager::reset()
{
    // 重置每个轴的回零状态
    for (int i = 0; i < kAxisCount; ++i) {
        home_state_[i] = HOME_INIT;      // 设置为初始状态
        home_runtime_[i].start_time_ms = 0; // 清除起始时间
        is_homed_[i] = false;             // 回零标志设为false
    }
    
    // 重置预处理标志
    j2_moved_away_ = false;     // J2轴未移开原点
    j3_moved_away_ = false;     // J3轴未移开原点
    j5_moved_away_ = false;     // J5轴未移开原点
    serial_prehome_completed_ = false;  // 串行预处理未完成
}

/**
 * @brief 获取回零标志数组
 * @return 回零标志数组的常量引用
 * @details 返回每个轴的回零状态，true表示已回零，false表示未回零
 */
const std::array<bool, HomeManager::kAxisCount>& HomeManager::homed_flags() const
{
    return is_homed_;
}

/**
 * @brief 检查是否所有轴都已回零
 * @return 所有轴都已回零返回true，否则返回false
 * @details 遍历所有轴的回零标志，只要有一个轴未回零就返回false
 */
bool HomeManager::is_all_homed() const
{
    for (bool ok : is_homed_) {
        if (!ok) {
            return false;
        }
    }
    return true;
}

/**
 * @brief 执行回零步骤
 * @details 先执行串行预处理，然后执行并行回零
 */
void HomeManager::step()
{
    // 检查驱动是否绑定
    if (!driver_) {
        return;
    }

    // 执行串行预处理
    step_serial();
    
    // 检查串行预处理是否完成
    if (!serial_prehome_completed_) {
        step_serial();  // 再次执行串行预处理
        
        // 检查J3、J2、J5轴是否都已移开原点
        if (j3_moved_away_ && j2_moved_away_ && j5_moved_away_) {
            qDebug() << "[Home] 串行预处理完成，切换到并行回零阶段";
            serial_prehome_completed_ = true;  // 标记串行预处理完成
        } else {
            return;  // 串行预处理未完成，返回
        }
    }
    
    // 执行并行回零
    step_parallel();
}

/**
 * @brief 执行串行预处理步骤
 * @details 按优先级处理J3、J2、J5轴的预处理，确保它们移开原点
 */
void HomeManager::step_serial()
{
    // 优先级：J3 -> J2 -> J5
    // 先检查J3轴是否需要处理
    if (!j3_moved_away_ && !is_homed_[2]) {
        process_prehome_axis(2);  // 处理J3轴（索引2）
        return;
    }
    
    // J3轴处理完成，检查J2轴
    if (!j2_moved_away_ && !is_homed_[1]) {
        process_prehome_axis(1);  // 处理J2轴（索引1）
        return;
    }
    
    // J2轴处理完成，检查J5轴
    if (!j5_moved_away_ && !is_homed_[4]) {
        process_prehome_axis(4);  // 处理J5轴（索引4）
        return;
    }
}

/**
 * @brief 处理单个轴的预处理
 * @param i 轴索引
 * @details 处理指定轴的预处理，包括初始化参数、检查传感器状态、移开原点等
 */
void HomeManager::process_prehome_axis(int i)
{
    HomeAxisState& state = home_state_[i];         // 获取当前轴的回零状态
    const HomeAxisHWConfig& hw = g_axis_hw[i];     // 获取轴的硬件配置
    const HomeStrategy& st = g_home_strategy[i];   // 获取轴的回零策略
    bool& moved_away = (i == 2 ? j3_moved_away_ : (i == 1 ? j2_moved_away_ : j5_moved_away_));  // 获取对应的移开原点标志

    qDebug() << "[Home] 处理轴" << i << "(J" << i+1 << ")，当前状态:" << state;

    switch (state) {
    case HOME_INIT:
        qDebug() << "[Home] 轴" << i << "初始化参数...";
        // 初始化轴参数（轴类型已在连接时设置）
        driver_->set_axis_speed(hw.axis, st.homeSpeed);       // 设置回零速度
        driver_->set_axis_accel(hw.axis, st.homeAccel);       // 设置回零加速度
        driver_->set_axis_decel(hw.axis, st.homeDecel);       // 设置回零减速度
        driver_->set_axis_lspeed(hw.axis, st.homeLspeed);     // 设置回零慢速阈值
        driver_->set_axis_creep(hw.axis, st.homeCreep);       // 设置回零爬行速度
        driver_->set_axis_sramp(hw.axis, st.homeSramp);       // 设置回零S曲线过渡时间
        driver_->set_datum_in(hw.axis, hw.homeIn);            // 设置原点输入
        driver_->set_invert_in(hw.homeIn, 1);                 // 设置原点输入反向
        if (hw.negIn >= 0) {
            driver_->set_invert_in(hw.negIn, 1);              // 设置反向输入反向
        }
        driver_->set_axis_enable(hw.axis, true);              // 使能轴
        state = HOME_CHECK_SENSOR;                             // 切换到检查传感器状态
        qDebug() << "[Home] 轴" << i << "初始化完成，切换到检查传感器状态";
        break;

    case HOME_CHECK_SENSOR:
    {
        qDebug() << "[Home] 轴" << i << "检查传感器状态...";
        // 读取原点传感器状态
        bool home_triggered = false;
        driver_->read_in(hw.homeIn, home_triggered);
        
        qDebug() << "[Home] 轴" << i << "原点传感器状态:" << (home_triggered ? "未触发" : "已触发");
        
        if (!home_triggered) {  // 原点传感器被触发
            state = HOME_MOVE_AWAY_ORIGIN;  // 切换到移开原点状态
            qDebug() << "[Home] 轴" << i << "原点传感器被触发，需要移开原点";
        } else {  // 原点传感器未被触发
            moved_away = true;              // 标记已移开原点
            state = HOME_START_DATUM;       // 切换到开始回零状态
            qDebug() << "[Home] 轴" << i << "原点传感器未被触发，直接开始回零";
        }
        break;
    }

    case HOME_MOVE_AWAY_ORIGIN:
        qDebug() << "[Home] 轴" << i << "移开原点，移动距离:" << st.moveAwayOrigin;
        // 移开原点
        driver_->axis_move_relative(hw.axis, st.moveAwayOrigin);  // 相对移动到移开原点位置
        state = HOME_WAIT_MOVE_AWAY_ORIGIN;                        // 切换到等待移开原点完成状态
        qDebug() << "[Home] 轴" << i << "开始移开原点，等待完成...";
        break;

    case HOME_WAIT_MOVE_AWAY_ORIGIN:
        // 等待移开原点完成
        if (driver_->is_axis_idle(hw.axis)) {  // 轴空闲表示移动完成
            moved_away = true;              // 标记已移开原点
            state = HOME_START_DATUM;       // 切换到开始回零状态
            qDebug() << "[Home] 轴" << i << "移开原点完成，切换到开始回零状态";
        } else {
            qDebug() << "[Home] 轴" << i << "正在移开原点中...";
        }
        break;

    case HOME_START_DATUM:
        qDebug() << "[Home] 轴" << i << "开始回零...";
        driver_->axis_home(hw.axis, st.datumMode);  // 开始回零
        state = HOME_WAIT_DATUM;         // 切换到等待回零完成状态
        qDebug() << "[Home] 轴" << i << "回零已启动，等待完成...";
        break;

    case HOME_WAIT_DATUM:
        // 检查回零是否完成
        if (driver_->is_axis_idle(hw.axis)) {  // 轴空闲表示回零完成
            is_homed_[i] = true;         // 标记轴已回零
            state = HOME_SUCCESS;            // 切换到回零完成状态
            qDebug() << "[Home] 轴" << i << "回零完成！";
        } else {
            qDebug() << "[Home] 轴" << i << "正在回零中...";
        }
        break;

    case HOME_SUCCESS:
        qDebug() << "[Home] 轴" << i << "回零已完成";
        break;

    default:
        break;
    }
}

/**
 * @brief 执行并行回零步骤
 * @details 所有轴同时执行回零流程，包括初始化、检查传感器、移开原点、回零等
 */
void HomeManager::step_parallel()
{
    qDebug() << "[Home] 执行并行回零步骤";
    
    // 遍历所有轴
    for (int i = 0; i < kAxisCount; ++i) {
        // 跳过已回零的轴
        if (is_homed_[i]) {
            qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 已回零，跳过";
            continue;
        }

        // 跳过未完成预处理的轴
        if ((i == 1 && !j2_moved_away_) ||
            (i == 2 && !j3_moved_away_) ||
            (i == 4 && !j5_moved_away_)) {
            qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 预处理未完成，跳过";
            continue;
        }

        const HomeAxisHWConfig& hw = g_axis_hw[i];     // 获取轴的硬件配置
        HomeAxisState& state = home_state_[i];         // 获取轴的回零状态
        HomeRuntime& rt = home_runtime_[i];            // 获取轴的运行时参数
        const HomeStrategy& st = g_home_strategy[i];   // 获取轴的回零策略

        qDebug() << "[Home] 处理轴" << i << "(J" << i+1 << ")，当前状态:" << state;

        // 检查轴是否空闲（除了等待回零完成状态）
        if (!driver_->is_axis_idle(hw.axis) && state != HOME_WAIT_DATUM) {
            qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 正在运动中，跳过";
            continue;
        }

        switch (state) {
        case HOME_INIT:
            qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 初始化参数...";
            // 初始化轴参数（轴类型已在连接时设置）
            driver_->set_axis_speed(hw.axis, st.homeSpeed);       // 设置回零速度
            driver_->set_axis_accel(hw.axis, st.homeAccel);       // 设置回零加速度
            driver_->set_axis_decel(hw.axis, st.homeDecel);       // 设置回零减速度
            driver_->set_axis_lspeed(hw.axis, st.homeLspeed);     // 设置回零慢速阈值
            driver_->set_axis_creep(hw.axis, st.homeCreep);       // 设置回零爬行速度
            driver_->set_axis_sramp(hw.axis, st.homeSramp);       // 设置回零S曲线过渡时间
            driver_->set_datum_in(hw.axis, hw.homeIn);            // 设置原点输入
            driver_->set_invert_in(hw.homeIn, 1);                 // 设置原点输入反向
            if (hw.negIn >= 0) {
                driver_->set_invert_in(hw.negIn, 1);              // 设置反向输入反向
            }
            driver_->set_axis_enable(hw.axis, true);              // 使能轴
            rt.start_time_ms = 0;                                 // 清除起始时间
            state = HOME_CHECK_SENSOR;                             // 切换到检查传感器状态
            qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 初始化完成，切换到检查传感器状态";
            break;

        case HOME_CHECK_SENSOR:
        {
            qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 检查传感器状态...";
            // 读取原点传感器状态
            bool home_triggered = false;
            if (hw.homeIn >= 0) {
                driver_->read_in(hw.homeIn, home_triggered);
                qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 原点传感器状态:" << (home_triggered ? "未触发" : "已触发");
            }
            
            // 读取反向限位传感器状态
            bool neg_triggered = false;
            if (hw.negIn >= 0) {
                driver_->read_in(hw.negIn, neg_triggered);
                qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 反向限位传感器状态:" << (neg_triggered ? "未触发" : "已触发");
            }

            if (!home_triggered) {  // 原点传感器被触发
                state = HOME_MOVE_AWAY_ORIGIN;  // 切换到移开原点状态
                qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 原点传感器被触发，需要移开原点";
            } else if (hw.negIn >= 0 && !neg_triggered) {  // 反向限位传感器被触发
                state = HOME_MOVE_AWAY_REVERSE;  // 切换到移开反向限位状态
                qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 反向限位传感器被触发，需要移开反向限位";
            } else {  // 传感器均未被触发
                state = HOME_START_DATUM;  // 切换到开始回零状态
                qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 传感器均未被触发，直接开始回零";
            }
            break;
        }

        case HOME_MOVE_AWAY_ORIGIN:
            qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 移开原点，移动距离:" << st.moveAwayOrigin;
            // 移开原点
            driver_->axis_move_relative(hw.axis, st.moveAwayOrigin);  // 相对移动到移开原点位置
            state = HOME_WAIT_MOVE_AWAY_ORIGIN;                        // 切换到等待移开原点完成状态
            qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 开始移开原点，等待完成...";
            break;

        case HOME_WAIT_MOVE_AWAY_ORIGIN:
            // 等待移开原点完成
            if (driver_->is_axis_idle(hw.axis)) {  // 轴空闲表示移动完成
                state = HOME_CHECK_SENSOR;  // 切换到检查传感器状态
                qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 移开原点完成，切换到检查传感器状态";
            } else {
                qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 正在移开原点中...";
            }
            break;

        case HOME_MOVE_AWAY_REVERSE:
            qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 移开反向限位，移动距离:" << st.moveAwayReverse;
            // 移开反向限位
            driver_->axis_move_relative(hw.axis, st.moveAwayReverse);  // 相对移动到移开反向限位位置
            state = HOME_WAIT_MOVE_AWAY_REVERSE;                        // 切换到等待移开反向限位完成状态
            qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 开始移开反向限位，等待完成...";
            break;

        case HOME_WAIT_MOVE_AWAY_REVERSE:
            // 等待移开反向限位完成
            if (driver_->is_axis_idle(hw.axis)) {  // 轴空闲表示移动完成
                state = HOME_CHECK_SENSOR;  // 切换到检查传感器状态
                qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 移开反向限位完成，切换到检查传感器状态";
            } else {
                qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 正在移开反向限位中...";
            }
            break;

        case HOME_START_DATUM:
            qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 开始回零...";
            // 开始回零
            if (driver_->is_axis_idle(hw.axis)) {  // 轴空闲
                driver_->set_axis_enable(hw.axis, true);  // 确保轴使能
                driver_->axis_home(hw.axis, st.datumMode);  // 执行回零
                state = HOME_WAIT_DATUM;  // 切换到等待回零完成状态
                rt.start_time_ms = GetTickCount64(); // 记录起始时间（ms）
                qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 回零已启动，等待完成...";
            } else {
                qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 未空闲，无法启动回零";
            }
            break;

        case HOME_WAIT_DATUM:
        {
            // 使用实时时钟判断超时（3 分钟）
            const uint64_t now = GetTickCount64();
            
            // 读取回零状态
            bool homed = false;
            bool get_homed_result = driver_->get_axis_homed(hw.axis, homed);
            
            qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 回零状态:" << (homed ? "已完成" : "进行中") << "，获取状态结果:" << (get_homed_result ? "成功" : "失败");
            
            // 检查反向限位
            if (hw.negIn >= 0) {
                bool neg_triggered = false;
                driver_->read_in(hw.negIn, neg_triggered);
                if (!neg_triggered) {  // 反向限位被触发
                    driver_->axis_cancel(hw.axis, 2);  // 取消当前运动
                    rt.start_time_ms = 0;
                    state = HOME_MOVE_AWAY_REVERSE;  // 切换到移开反向限位状态
                    qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 反向限位被触发，取消回零，移开反向限位";
                    break;
                }
            }

            // 检查回零是否完成
            if (get_homed_result && homed) {
                is_homed_[i] = true;  // 标记轴已回零
                rt.start_time_ms = 0;
                state = HOME_SUCCESS;  // 切换到回零成功状态
                qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 回零完成！";
            } else if (rt.start_time_ms != 0 && (now - rt.start_time_ms) > (2 * 60 * 1000)) {  // 回零超时（3分钟）
                rt.start_time_ms = 0;
                state = HOME_FAIL;  // 切换到回零失败状态
                qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 回零超时（2分钟），切换到失败状态";
            } else {
                qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 正在回零中...已等待" << (rt.start_time_ms > 0 ? (now - rt.start_time_ms)/1000 : 0) << "秒";
            }
            break;
        }

        case HOME_SUCCESS:
            qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 回零成功！";
            break;

        case HOME_FAIL:
            qDebug() << "[Home] 轴" << i << "(J" << i+1 << ") 回零失败！";
            break;

        default:
            break;
        }
    }
    
    // 检查所有轴是否都已回零
    bool all_homed = true;
    for (int i = 0; i < kAxisCount; ++i) {
        if (!is_homed_[i]) {
            all_homed = false;
            break;
        }
    }
    
    if (all_homed) {
        qDebug() << "[Home] 所有轴回零完成！";
    } else {
        qDebug() << "[Home] 部分轴仍在回零中...";
    }
}

/**
 * @brief 移动所有轴到指定位置
 * @param positions 各轴目标位置数组
 * @details 控制每个轴移动到指定的绝对位置
 */
void HomeManager::move_all_axes_to_positions(const std::array<float, kAxisCount>& positions)
{
    // 检查驱动是否绑定
    if (!driver_) {
        return;
    }

    // 控制每个轴移动到指定位置
    for (int i = 0; i < kAxisCount; ++i) {
        const HomeAxisHWConfig& hw = g_axis_hw[i];
        driver_->axis_move_absolute(hw.axis, positions[i]);  // 绝对移动到目标位置
        qDebug() << "[Move] J" << (i + 1) << "->" << positions[i];  // 输出移动信息
    }
}

/**
 * @brief 多轴直线插补
 * @param positions 各轴目标位置数组
 * @details 控制多个轴同时移动到指定位置，实现直线插补运动
 */
void HomeManager::multi_axis_interpolation(const std::array<float, kAxisCount>& positions)
{
    // 检查驱动是否绑定
    if (!driver_) {
        return;
    }

    // 构建轴号和目标位置列表
    std::vector<int> axes;
    std::vector<double> target_positions;// 目标位置列表
    axes.reserve(kAxisCount);         // 预分配空间
    target_positions.reserve(kAxisCount);  // 预分配空间

    // 填充轴号和目标位置
    for (int i = 0; i < kAxisCount; ++i) {
        const HomeAxisHWConfig& hw = g_axis_hw[i];
        axes.push_back(hw.axis);              // 添加轴号
        target_positions.push_back(positions[i]);  // 添加目标位置
    }

    // 执行多轴直线插补
    const bool success = driver_->multi_axis_move_absolute(axes, target_positions);
    qDebug() << "[Interp]" << (success ? "成功" : "失败");  // 输出执行结果
}
