#pragma once

#include <QObject>
#include <QList>

#include "vmtypes.h"

/**
 * @brief 视觉像素 → 机械臂笛卡尔坐标 数据转换器
 *
 * 接收 VisionMasterBridge 的 VISION_POINT 数据，经过两级滤波（预热丢弃 + 移动平均），
 * 通过固定偏移将视觉 XY 映射为机械臂笛卡尔坐标，以信号形式输出目标 (X, Y)。
 * 本类不操作机械臂，运动下发由调用方自行决定。
 *
 * 滤波管线：
 *   1. 预热：   启动后丢弃前 N 帧（视觉进程启动初期数据不稳定）
 *   2. 均值滤波：对最近 N 帧取算术平均，抑制抖动
 *
 * 用法：
 *   VisionRobotLink* link = new VisionRobotLink(this);
 *   connect(vmBridge, &VisionMasterBridge::visionDataReceived,
 *           link, &VisionRobotLink::onVisionData);
 *   connect(link, &VisionRobotLink::targetReady,
 *           this, [](double x, double y) { qDebug() << "目标:" << x << y; });
 */
class VisionRobotLink : public QObject
{
    Q_OBJECT

public:
    explicit VisionRobotLink(QObject* parent = nullptr);

    /**
     * @brief 设置视觉 → 机械臂 XY 偏移量
     *
     * 公式：robot_xy = vision_xy + offset
     * 默认偏移 (84.077, -494.404)：视觉像素 (~398, ~292) 映射到机械臂笛卡尔 (~482, -202)
     */
    void setOffset(double offsetX, double offsetY);

    /**
     * @brief 设置启动预热帧数
     *
     * 视觉进程启动后前几帧数据通常不稳定，这些帧会被丢弃不输出信号。
     * 默认 3 帧。
     */
    void setWarmupFrames(int count);

    /**
     * @brief 设置移动平均窗口大小
     *
     * 预热结束后，对最近 N 帧原始 (x, y) 取算术平均后再计算目标坐标。
     * 窗口越大越平滑但响应越慢。设为 1 则关闭平均，直接使用原始值。
     * 默认 3。
     */
    void setFilterWindow(int windowSize);

    /**
     * @brief 重置帧计数与滤波缓冲区
     *
     * 每次开始新一轮多帧采样前调用，清除历史帧的累积状态。
     */
    void resetFrameCount();

signals:
    /**
     * @brief 数据转换完成，发射目标坐标
     * @param robotX 机械臂笛卡尔 X 坐标
     * @param robotY 机械臂笛卡尔 Y 坐标
     */
    void targetReady(double robotX, double robotY);

public slots:
    /**
     * @brief 处理一行视觉数据（字符串格式，兼容 QProcess 桥接）
     *
     * 预期格式（竖线分隔）：VISION_POINT|<流程名>|<x>|<y>|<r>
     * 非 VISION_POINT 开头的行会被忽略。
     * 滤波 + 映射后的坐标通过 targetReady 信号发射。
     */
    void onVisionData(const QString& line);

    /**
     * @brief 处理结构化视觉数据事件（静态库内嵌模式）
     *
     * 接收 VMControl::dataReady 信号，提取 PointData，
     * 经滤波和坐标映射后通过 targetReady 信号发射。
     */
    void onVisionDataEvent(const VisionDataEvent& event);

private:
    // --- 滤波状态 ---
    int m_warmupFrames = 3;       ///< 启动时丢弃的帧数
    int m_frameCount   = 0;       ///< 已接收的总帧数
    int m_filterWindow = 3;       ///< 移动平均窗口大小
    QList<double> m_xBuf;         ///< 最近 N 帧原始 X（环形缓冲区）
    QList<double> m_yBuf;         ///< 最近 N 帧原始 Y（环形缓冲区）
};
