#include "VisionRobotLink.h"

// ---------------------------------------------------------------------------
//  构造
// ---------------------------------------------------------------------------

VisionRobotLink::VisionRobotLink(QObject* parent)
    : QObject(parent)
{
}

// ---------------------------------------------------------------------------
//  配置接口
// ---------------------------------------------------------------------------

void VisionRobotLink::setWarmupFrames(int count)
{
    m_warmupFrames = count;
}

/**
 * @brief 设置移动平均滤波的窗口大小
 *
 * 用于控制视觉数据平滑处理的滑动窗口大小。窗口内数据取算术平均值以抑制噪声。
 * 窗口大小为 1 时关闭滤波，直接使用原始值。
 *
 * @param windowSize 滤波窗口大小，最小值为 1
 */
void VisionRobotLink::setFilterWindow(int windowSize)
{
    // 最小为 1，设为 1 时关闭平均，直接使用原始值
    m_filterWindow = qMax(1, windowSize);
}

// ---------------------------------------------------------------------------
//  核心：视觉数据 → 笛卡尔坐标转换
// ---------------------------------------------------------------------------

void VisionRobotLink::onVisionData(const QString& line)
{
    // ---- 1. 解析输入 ----
    //
    // 预期格式：VISION_POINT|<流程名>|<x>|<y>|<r>
    // 示例：    VISION_POINT|flow1|398.275|292.404|0

    const QStringList parts = line.split(QLatin1Char('|'));
    if (parts.isEmpty()) return;

    const QString& type = parts[0];
    if (type != QStringLiteral("VISION_POINT") || parts.size() < 5)
        return;

    double vx = parts[2].toDouble();
    double vy = parts[3].toDouble();

    // ---- 2. 数值范围校验 ----
    //
    // 过滤视觉进程启动瞬间产生的天文数字噪点（如 6e+13）。
    // 画面分辨率有限，超过 10000 的像素坐标视为无效。

    constexpr double kMaxValid = 10000.0;
    if (vx > kMaxValid || vy > kMaxValid) {
        return;
    }

    // ---- 3. 预热过滤 ----
    //
    // 视觉进程启动后前几帧数据通常不稳定（跳变/噪点），
    // 丢弃前 N 帧，不输出信号。

    if (m_frameCount < m_warmupFrames) {
        ++m_frameCount;
        return;
    }

    // ---- 4. 移动平均滤波 ----
    //
    // 维护最近 N 帧原始 (x, y) 的滑动窗口，取算术平均。
    // 用于平滑传感器噪声和单帧离群点。

    m_xBuf.append(vx);
    m_yBuf.append(vy);
    if (m_xBuf.size() > m_filterWindow) {
        m_xBuf.removeFirst();
        m_yBuf.removeFirst();
    }

    double avgX = 0.0, avgY = 0.0;
    for (const double& x : m_xBuf) avgX += x;
    for (const double& x : m_yBuf) avgY += x;
    avgX /= m_xBuf.size();
    avgY /= m_yBuf.size();

    // ---- 5. 坐标映射 ----
    //
    // 视觉坐标已标定对齐机械臂坐标系，直接透传无需偏移。

    double robotX = avgX;
    double robotY = avgY;

    // ---- 6. 发射目标坐标 ----
    emit targetReady(robotX, robotY);
}

// ---------------------------------------------------------------------------
//  结构化数据接收（VMControl 静态库模式）
// ---------------------------------------------------------------------------

void VisionRobotLink::onVisionDataEvent(const VisionDataEvent& event)
{
    // 当前仅处理单点视觉定位数据
    if (event.DataType != VisionDataType::PointData)
        return;

    const PointData& point = event.pack["PointData"].value<PointData>();

    double vx = point.x;
    double vy = point.y;

    // ---- 数值范围校验 ----
    constexpr double kMaxValid = 10000.0;
    if (vx > kMaxValid || vy > kMaxValid) {
        return;
    }

    // ---- 预热过滤 ----
    if (m_frameCount < m_warmupFrames) {
        ++m_frameCount;
        return;
    }

    // ---- 移动平均滤波 ----
    m_xBuf.append(vx);
    m_yBuf.append(vy);
    if (m_xBuf.size() > m_filterWindow) {
        m_xBuf.removeFirst();
        m_yBuf.removeFirst();
    }

    double avgX = 0.0, avgY = 0.0;
    for (const double& x : m_xBuf) avgX += x;
    for (const double& x : m_yBuf) avgY += x;
    avgX /= m_xBuf.size();
    avgY /= m_yBuf.size();

    // ---- 坐标映射 ----
    double robotX = avgX;
    double robotY = avgY;

    // ---- 发射目标坐标 ----
    emit targetReady(robotX, robotY);
}

// ---------------------------------------------------------------------------
//  帧计数重置
// ---------------------------------------------------------------------------

void VisionRobotLink::resetFrameCount()
{
    m_frameCount = 0;
    m_xBuf.clear();
    m_yBuf.clear();
}
