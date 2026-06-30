#ifndef ROBOTCONSTANTS_H
#define ROBOTCONSTANTS_H

#include <QList>

/**
 * @file RobotConstants.h
 * @brief 机械臂硬件固定参数常量定义
 * 
 * 本文件定义机械臂的固定硬件参数，这些参数在机械臂生命周期内保持不变：
 * - 脉冲当量（轴单位）
 * - 轴类型配置
 * 
 * 所有模块应从这里读取这些参数，避免重复定义。
 */

namespace RobotConstants {

/**
 * @brief 轴脉冲当量数组（脉冲/度）
 * 
 * 定义每个轴的脉冲当量，用于将角度转换为脉冲数。
 * 这是机械臂的硬件固定参数，取决于电机、减速器和编码器配置。
 */
constexpr double kAxisUnits[6] = {
    555.56,  // J1: 旋转轴，减速比50 + 齿轮比1:4
    555.56,  // J2: 旋转轴，减速比50 + 齿轮比1:4
    83.33,   // J3: 旋转轴，减速比30 + 齿轮比1:1
    138.89,  // J4: 旋转轴，减速比50 + 齿轮比1:1
    83.33,   // J5: 旋转轴，减速比30 + 齿轮比1:1
    27.78    // J6: 旋转轴，减速比10 + 齿轮比1:1
};

/**
 * @brief 获取轴脉冲当量列表
 * @return QList<double> 包含6个轴的脉冲当量
 */
inline QList<double> getAxisUnitsList() {
    return QList<double>{
        kAxisUnits[0], kAxisUnits[1], kAxisUnits[2],
        kAxisUnits[3], kAxisUnits[4], kAxisUnits[5]
    };
}

/**
 * @brief 获取指定轴的脉冲当量
 * @param axisIndex 轴索引（0-5）
 * @return double 脉冲当量值
 */
inline double getAxisUnit(int axisIndex) {
    if (axisIndex >= 0 && axisIndex < 6) {
        return kAxisUnits[axisIndex];
    }
    return 1.0;  // 默认值
}

/**
 * @brief 轴类型数组
 * 0: 直线轴, 1: 旋转轴
 */
constexpr int kAxisTypes[6] = {
    1,  // J1: 旋转轴
    1,  // J2: 旋转轴
    1,  // J3: 旋转轴
    1,  // J4: 旋转轴
    1,  // J5: 旋转轴
    1   // J6: 旋转轴
};

/**
 * @brief 获取轴类型
 * @param axisIndex 轴索引（0-5）
 * @return int 轴类型（0=直线轴，1=旋转轴）
 */
inline int getAxisType(int axisIndex) {
    if (axisIndex >= 0 && axisIndex < 6) {
        return kAxisTypes[axisIndex];
    }
    return 1;  // 默认旋转轴
}

} // namespace RobotConstants

#endif // ROBOTCONSTANTS_H
