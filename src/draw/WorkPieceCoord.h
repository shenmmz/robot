#pragma once

#include <QObject>
#include <QVector3D>
#include <QMatrix4x4>
#include <QString>
#include <cmath>

class WorkPieceCoord : public QObject {
    Q_OBJECT

public:
    explicit WorkPieceCoord(QObject *parent = nullptr);

    void setName(const QString &name) { m_name = name; }
    void setOrigin(const QVector3D &origin) { m_origin = origin; }
    void setXAxis(const QVector3D &xAxis) { m_xAxis = xAxis; }
    void setYAxis(const QVector3D &yAxis) { m_yAxis = yAxis; }
    
    // 添加原始定义点的设置和获取方法（包含姿态）
    void setOriginalPoints(const QVector3D &p1, const QVector3D &p2, const QVector3D &p3,
                          double rx = 0.0, double ry = 0.0, double rz = 0.0) {
        m_p1 = p1;
        m_p2 = p2;
        m_p3 = p3;
        m_rx = normalizeAngle(rx);
        m_ry = normalizeAngle(ry);
        m_rz = normalizeAngle(rz);
        m_rxOriginal = rx;  // 保存原始值
        m_ryOriginal = ry;
        m_rzOriginal = rz;
    }
    QVector3D getPoint1() const { return m_p1; }
    QVector3D getPoint2() const { return m_p2; }
    QVector3D getPoint3() const { return m_p3; }
    
    // 角度归一化函数（将角度转换到 -180°~180° 范围）
    static double normalizeAngle(double angle) {
        angle = fmod(angle, 360.0);
        if (angle > 180.0) {
            angle -= 360.0;
        } else if (angle <= -180.0) {
            angle += 360.0;
        }
        return angle;
    }
    
    // 姿态信息（归一化后的值，用于计算）
    void setPose(double rx, double ry, double rz) { 
        m_rx = normalizeAngle(rx); 
        m_ry = normalizeAngle(ry); 
        m_rz = normalizeAngle(rz); 
        m_rxOriginal = rx;  // 保存原始值
        m_ryOriginal = ry;
        m_rzOriginal = rz;
    }
    double getRX() const { return m_rx; }
    double getRY() const { return m_ry; }
    double getRZ() const { return m_rz; }
    
    // 获取原始姿态值（用于显示）
    double getRXOriginal() const { return m_rxOriginal; }
    double getRYOriginal() const { return m_ryOriginal; }
    double getRZOriginal() const { return m_rzOriginal; }

    QString getName() const { return m_name; }
    QVector3D getOrigin() const { return m_origin; }
    QVector3D getXAxis() const { return m_xAxis; }
    QVector3D getYAxis() const { return m_yAxis; }

    QVector3D getZAxis() const;

    bool isValid() const;

    QVector3D transform2DTo3D(double x, double y, double z = 0.0) const;

    void calculateFromThreePoints(const QVector3D &p1, const QVector3D &p2, const QVector3D &p3);

    QMatrix4x4 getTransformationMatrix() const;

signals:
    void coordChanged();

private:
    QString m_name;
    QVector3D m_origin;
    QVector3D m_xAxis;
    QVector3D m_yAxis;
    QVector3D m_p1;  // 原始定义点1（原点）
    QVector3D m_p2;  // 原始定义点2（X轴方向）
    QVector3D m_p3;  // 原始定义点3（XY平面）
    double m_rx = 0.0;  // 姿态RX（归一化后）
    double m_ry = 0.0;  // 姿态RY（归一化后）
    double m_rz = 0.0;  // 姿态RZ（归一化后）
    double m_rxOriginal = 0.0;  // 原始姿态RX
    double m_ryOriginal = 0.0;  // 原始姿态RY
    double m_rzOriginal = 0.0;  // 原始姿态RZ
    bool m_valid = false;
};