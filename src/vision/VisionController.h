/*
 * @file VisionController.h
 * @brief 启动 Python 视觉程序并以 TCP Server 接收姿态数据。
 *
 * 职责：
 * - 用 QProcess 启动 Python 视觉程序（python -m main，POS 端 PoseCommClient 是 TCP Client）
 * - 用 QTcpServer 监听 5001，等待 Python 端 connect 进来
 * - 解析 JSON 提取 motor_commands 数组，通过 motorCommands 信号下发
 * - 监控子进程的 stdout / stderr / 退出事件
 * - 提供 isRunning() / stop() 业务接口和 started / stopped / errorOccurred / outputMessage 信号
 *
 * 架构：
 *   Python (QTcpSocket client, 主动 connectToHost 127.0.0.1:5001)
 *        ↓
 *   Qt    (QTcpServer, listen 5001) ← 本类
 *
 * 之前错误的设计是 Qt 做 client 连 Python 的 5001 → 两边都是 client 永远连不上，
 * 现在改为 Qt 做 server 等 Python 主动连过来（与 pose_comm.py 的设计一致）。
 */
#ifndef VISIONCONTROLLER_H
#define VISIONCONTROLLER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QPair>

class QProcess;
class QTcpServer;
class QTcpSocket;

/**
 * @brief 视觉控制器一对一下行数据类型
 *
 * 由 payload.motor_commands 数组中每条命令抽象得来：
 * - axis：关节轴下标 0-5（来自 Python 端 motor_id 1-6，编程时 -1 对齐）
 * - signed_angle_deg：带方向符号的角度（度），正值=正方向，负值=反方向
 */
using VisionMotorCommand = QPair<int, double>;

class VisionController : public QObject
{
    Q_OBJECT

public:
    explicit VisionController(QObject *parent = nullptr);
    ~VisionController() override;

    /**
     * @brief 启动 TCP Server 监听 5001 并拉起 Python 视觉程序
     * @return true 表示 listen 成功且 QProcess 启动成功
     */
    bool start();

    /**
     * @brief 停止 Python 子进程并关闭 TCP Server
     */
    void stop();

    bool isRunning() const;

    /**
     * @brief 获取 Python 项目目录路径
     */
    QString projectPath() const;

signals:
    void started();
    void stopped(int exitCode);
    void errorOccurred(const QString &error);
    void outputMessage(const QString &channel, const QString &line);

    /**
     * @brief 视觉系统下发多轴运动指令（首选信号，订阅它即可拿到全部电机命令）
     * @param commands 每条 (axis 0~5, signed_angle_deg)
     */
    void motorCommands(const QList<VisionMotorCommand>& commands);

    /**
     * @brief 视觉系统下发单轴指令（兼容老代码：motor_commands 数组缺失时回退到此信号）
     * @param angle 目标角度（度，仅 motor_id=6）
     */
    void motorCommand(double angle);

private slots:
    void onStarted();
    void onReadyReadStdout();
    void onReadyReadStderr();
    void onFinished(int exitCode, int exitStatus);
    void onProcessError(int error);

    // --- TCP Server ---
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();

private:
    void drainLines(const QByteArray &data, const QString &channel);
    void parseAndDispatch(const QByteArray &line);

    QProcess *m_process = nullptr;
    QString m_projectPath;
    QByteArray m_stdoutBuffer;
    QByteArray m_stderrBuffer;

    QTcpServer *m_tcpServer = nullptr;       // 新: Qt 做 server 监听 5001
    QTcpSocket *m_tcpSocket = nullptr;       // Python 连上来的 socket（保留指针便于 stop 时 disconnect）
    QByteArray m_tcpBuffer;
    static constexpr quint16 kServerPort = 5001;
};

#endif // VISIONCONTROLLER_H