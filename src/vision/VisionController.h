/*
 * @file VisionController.h
 * @brief 启动 Python 视觉程序并以 TCP Client 连接接收姿态数据。
 *
 * 职责：
 * - 用 QProcess 启动 Python 视觉程序（python -m main，默认监听 5001）
 * - 以 QTcpSocket 连接到 Python 的 5001 端口，接收 pose 数据
 * - 解析 JSON 提取 motor_6.angle_deg，通过 motorCommand 信号下发
 * - 监控子进程的 stdout / stderr / 退出事件
 * - 提供 isRunning() / stop() 业务接口和 started / stopped / errorOccurred / outputMessage 信号
 *
 * 不再依赖外部 VisualRoboticArmQtController.exe，绕过其单连接限制。
 */
#ifndef VISIONCONTROLLER_H
#define VISIONCONTROLLER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QJsonObject>

class QProcess;
class QTcpSocket;

class VisionController : public QObject
{
    Q_OBJECT

public:
    explicit VisionController(QObject *parent = nullptr);
    ~VisionController() override;

    /**
     * @brief 连接 Python 的 TCP 5001 并拉起 Python 视觉程序
     * @return true 表示连接成功且 QProcess 启动成功
     */
    bool start();

    /**
     * @brief 停止 Python 子进程并断开 TCP 连接
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
     * @brief 视觉系统下发 J0 旋转角度指令
     * @param angle 目标角度（度）
     */
    void motorCommand(double angle);

private slots:
    void onStarted();
    void onReadyReadStdout();
    void onReadyReadStderr();
    void onFinished(int exitCode, int exitStatus);
    void onProcessError(int error);

    // --- TCP Client ---
    void onConnected();
    void onReadyRead();
    void onClientDisconnected();

private:
    void drainLines(const QByteArray &data, const QString &channel);
    void parseAndDispatch(const QByteArray &line);

    QProcess *m_process = nullptr;
    QString m_projectPath;
    QByteArray m_stdoutBuffer;
    QByteArray m_stderrBuffer;

    QTcpSocket *m_tcpSocket = nullptr;
    QByteArray m_tcpBuffer;
    static constexpr quint16 kServerPort = 5001;
};

#endif // VISIONCONTROLLER_H