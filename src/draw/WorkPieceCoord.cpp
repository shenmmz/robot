/*
 * @file WorkPieceCoord.cpp
 * @brief 工件坐标系类实现
 * 
 * 该文件实现了六轴机械臂工件坐标系的核心功能：
 * 1. 通过三个点计算工件坐标系平面（原点、X轴、Y轴方向）
 * 2. 提供2D坐标到3D坐标的转换（用于画板轨迹点转换）
 * 3. 支持姿态信息（RX/RY/RZ）的存储和管理
 * 4. 提供角度归一化功能（将角度转换到-180°~180°范围）
 * 5. 生成坐标变换矩阵用于坐标转换
 * 
 * 工件坐标系允许用户在工件上定义自己的局部坐标系，方便在不同工件上执行轨迹任务。
 */
#include "WorkPieceCoord.h"
#include <QMath.h>

WorkPieceCoord::WorkPieceCoord(QObject *parent) : QObject(parent) {
}

QVector3D WorkPieceCoord::getZAxis() const {
    return QVector3D::crossProduct(m_xAxis, m_yAxis).normalized();
}

bool WorkPieceCoord::isValid() const {
    return m_valid && !m_xAxis.isNull() && !m_yAxis.isNull();
}

QVector3D WorkPieceCoord::transform2DTo3D(double x, double y, double z) const {
    if (!isValid()) {
        return QVector3D(x, y, z);
    }
    return m_origin + m_xAxis * x + m_yAxis * y + getZAxis() * z;
}

void WorkPieceCoord::calculateFromThreePoints(const QVector3D &p1, const QVector3D &p2, const QVector3D &p3) {
    // 保存原始定义点
    m_p1 = p1;
    m_p2 = p2;
    m_p3 = p3;
    
    m_origin = p1;

    QVector3D xDir = p2 - p1;
    double xLen = xDir.length();
    if (xLen > 0.001) {
        m_xAxis = xDir / xLen;
    } else {
        m_xAxis = QVector3D(1, 0, 0);
    }

    QVector3D yDir = p3 - p1;
    QVector3D zAxis = QVector3D::crossProduct(m_xAxis, yDir);
    m_yAxis = QVector3D::crossProduct(zAxis, m_xAxis).normalized();

    m_valid = true;
    emit coordChanged();
}

QMatrix4x4 WorkPieceCoord::getTransformationMatrix() const {
    QMatrix4x4 matrix;
    if (!isValid()) {
        return matrix;
    }

    QVector3D zAxis = getZAxis();

    matrix.setColumn(0, QVector4D(m_xAxis, 0));
    matrix.setColumn(1, QVector4D(m_yAxis, 0));
    matrix.setColumn(2, QVector4D(zAxis, 0));
    matrix.setColumn(3, QVector4D(m_origin, 1));

    return matrix;
}
