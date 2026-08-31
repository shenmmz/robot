#ifndef ROBOTKINEMATICS_H
#define ROBOTKINEMATICS_H

#include <QList>
#include <QVector>

/**
 * @brief 六轴机械臂建模工具
 *
 * 用于构建 Zmotion 控制器 FRAME6 类型 CONNFRAME 所需的 25 项 TableList。
 * 运动学计算（正解/逆解）由控制器内部完成，PC 端不需要重复实现。
 *
 * FRAME6 TableList 映射（25 项 float）：
 *   [0]      = LargeZ    (基座垂直高度 / J0.a)
 *   [1]      = L1        (1轴到2轴X偏移 / J0.alpha)
 *   [2]      = L2        (大臂长度 / J0.d)
 *   [3]      = L3        (3轴到4轴中心距离 / J0.theta)
 *   [4]      = L4        (4轴到5轴距离 / J1.a)
 *   [5]      = D5        (5轴转一圈时6轴转动圈数，0=不关联 / J1.alpha)
 *   [6..11]  = u_j1..u_j6  (关节轴一圈脉冲数 = axisUnits[i] * 360)
 *   [12]     = PulesVROneCircle (虚拟姿态轴一圈脉冲数)
 *   [13]     = SmallX    (末端到转盘中心X偏移)
 *   [14]     = SmallY    (末端到转盘中心Y偏移)
 *   [15]     = SmallZ    (末端到5轴垂直距离)
 *   [16]     = InitRx    (初始绕X姿态角)
 *   [17]     = InitRy    (初始绕Y姿态角)
 *   [18]     = InitRz    (初始绕Z姿态角)
 *   [19..24] = 保留 (0)
 */
class RobotKinematics
{
public:
    // 按 6 轴脉冲当量构建 FRAME6 CONNFRAME 的 25 项 TableList（几何为固定硬编码）
    static QVector<float> buildFrame6Table(const QList<double>& axisUnits);
};

#endif // ROBOTKINEMATICS_H
