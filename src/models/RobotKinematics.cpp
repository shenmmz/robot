#include "RobotKinematics.h"
#include <QDebug>

/**
 * @brief 构建 FRAME6 CONNFRAME 的 25 项 TableList
 *
 * 几何尺寸固定硬编码，脉冲数 = axisUnits[i] * 360。
 */
QVector<float> RobotKinematics::buildFrame6Table(const QList<double>& axisUnits)
{
    QVector<float> table(25, 0.0f);

    if (axisUnits.size() != 6) {
        qWarning() << QStringLiteral("RobotKinematics: 轴单位数量不等于6，已忽略");
        return table;
    }

    // ===== FRAME6 几何尺寸 =====
    table[0] = 307.5f;                 // LargeZ  (基座垂直高度)
    table[1] = -80.87f;                // L1      (1轴到2轴X偏移)
    table[2] = 312.0f;                 // L2      (大臂长度)
    table[3] = 85.0f;                  // L3      (3轴到4轴中心距离)
    table[4] = 80.92f + 306.36f;       // L4      (4轴到5轴距离, 小臂总长 = 387.18)
    table[5] = 0.0f;                   // D5      (5轴转一圈时6轴转动圈数, 0=不关联)

    // ===== 脉冲数 (Units * 360) =====
    table[6] = static_cast<float>(axisUnits[0] * 360.0);   // J0 脉冲数
    table[7] = static_cast<float>(axisUnits[1] * 360.0);   // J1 脉冲数
    table[8] = static_cast<float>(axisUnits[2] * 360.0);   // J2 脉冲数
    table[9] = static_cast<float>(axisUnits[3] * 360.0);   // J3 脉冲数
    table[10] = static_cast<float>(axisUnits[4] * 360.0);  // J4 脉冲数
    table[11] = static_cast<float>(axisUnits[5] * 360.0);  // J5 脉冲数
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

