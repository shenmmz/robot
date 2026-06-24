#include "RobotKinematics.h"
#include "../../library/RobotConstants.h"
#include <QDebug>

/**
 * @brief 默认构造函数
 *
 * 使用全局常量中的脉冲当量配置：
 * - 轴0:555.56 轴1:333.33 轴2:250.00 轴3:138.89 轴4:83.33 轴5:27.78
 */
RobotKinematics::RobotKinematics()
    : m_axisUnits(RobotConstants::getAxisUnitsList())
{
}

/**
 * @brief 设置 6 轴脉冲当量
 *
 * 脉冲当量影响 toControllerTable() 中 table[6..12] 的转换因子。
 *
 * @param units 6 个轴的脉冲当量（每度对应的脉冲数），长度不为 6 时忽略
 */
void RobotKinematics::setAxisUnits(const QList<double>& units)
{
    if (units.size() == 6) {
        m_axisUnits = units;
    } else {
        qWarning() << QStringLiteral("RobotKinematics: 轴单位数量不等于6，已忽略");
    }
}

/**
 * @brief 获取 6 轴脉冲当量
 *
 * @return 6 个轴的脉冲当量列表
 */
QList<double> RobotKinematics::getAxisUnits() const
{
    return m_axisUnits;
}

/**
 * @brief 生成控制器 FRAME6 CONNFRAME 所需的 25 项 TableList
 *
 * 按 FRAME6 类型规格填充几何参数与脉冲数，默认值需根据实际机械臂校准后替换。
 * 映射规则详见头文件类注释。
 *
 * @return 长度为 25 的浮点向量，全部初始化为 0
 */
QVector<float> RobotKinematics::toControllerTable() const
{
    QVector<float> table(25, 0.0f);

    // ===== FRAME6 几何尺寸 =====
    table[0] = 307.5f;                 // LargeZ  (基座垂直高度)
    table[1] = -80.87f;                // L1      (1轴到2轴X偏移)
    table[2] = 312.0f;                 // L2      (大臂长度)
    table[3] = 85.0f;                  // L3      (3轴到4轴中心距离)
    table[4] = 80.92f + 306.36f;       // L4      (4轴到5轴距离, 小臂总长 = 387.18)
    table[5] = 0.0f;                   // D5      (5轴转一圈时6轴转动圈数, 0=不关联)

    // ===== 脉冲数 (Units * 360) =====
    table[6] = static_cast<float>(m_axisUnits[0] * 360.0);   // J0 脉冲数
    table[7] = static_cast<float>(m_axisUnits[1] * 360.0);   // J1 脉冲数
    table[8] = static_cast<float>(m_axisUnits[2] * 360.0);   // J2 脉冲数
    table[9] = static_cast<float>(m_axisUnits[3] * 360.0);   // J3 脉冲数
    table[10] = static_cast<float>(m_axisUnits[4] * 360.0);  // J4 脉冲数
    table[11] = static_cast<float>(m_axisUnits[5] * 360.0);  // J5 脉冲数
    table[12] = 3600000.0f;              // PulesVR (10000*360, 虚拟轴一圈脉冲)

    // ===== 末端参数 =====
    table[13] = 0.0f;                  // SmallLX
    table[14] = 0.0f;                  // SmallLY
    table[15] = 53.18f;                // SmallLZ (轴5到TCP距离)

    // ===== 初始姿态 =====
    table[16] = 0.0f;                  // InitRx
    table[17] = 0.0f;                  // InitRy
    table[18] = 0.0f;                  // InitRz
    // table[19..24] 保持为 0

    return table;
}

/**
 * @brief 从控制器的 25 项 TableList 解析脉冲当量
 *
 * 按 toControllerTable() 的逆向映射解析 table[6..11] 恢复 axisUnits。
 * 输入长度不足 25 时忽略并打印警告。
 *
 * @param table25 从控制器读回的 25 项浮点数组
 */
void RobotKinematics::loadFromControllerTable(const QVector<float>& table25)
{
    if (table25.size() < 25) {
        qWarning() << QStringLiteral("RobotKinematics: 表格大小小于25，已忽略");
        return;
    }

    m_axisUnits.clear();
    for (int i = 0; i < 6; ++i) {
        m_axisUnits.append(static_cast<double>(table25[6 + i]) / 360.0);
    }
}
