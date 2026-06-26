#pragma once

#include <QAbstractSocket>
#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>

class LocalCommBridge : public QObject
{
    Q_OBJECT

public:
    explicit LocalCommBridge(QObject *parent = nullptr);

    // 启动本地 TCP 服务端，等待 Python 视觉进程连接。
    // 正式项目中建议继续让此类只负责通讯：这里完成收包和解析，
    // 再由 MainWindow::onMessageReceived 或业务服务层把 pose 数据交给运动控制模块。
    bool startServer(const QString &host = QStringLiteral("127.0.0.1"), quint16 port = 5001);
    void stopServer();

    bool isListening() const;
    bool isConnected() const;

    QString host() const;
    quint16 port() const;
    QString peerDescription() const;

    // 向 Python 发送一条 JSON 行消息。当前 Qt 主要使用 cmd/shutdown，
    // 方法本身保持通用，便于正式项目复用为诊断或调试命令通道。
    bool sendMessage(const QString &type,
                     const QJsonObject &payload = QJsonObject(),
                     const QString &msg = QString(),
                     const QJsonObject &extra = QJsonObject());
    bool sendCommand(const QString &text);

signals:
    void clientConnected(const QString &peer);
    void clientDisconnected();
    void messageReceived(const QJsonObject &message);
    void errorOccurred(const QString &error);
    void statusChanged(const QString &status);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();
    void onSocketError(QAbstractSocket::SocketError error);
    void emitStatus();

private:
    void shutdownServer(bool emitStatus);
    void closeSocket(bool emitSignal);

    QHostAddress resolveAddress(const QString &host) const;

    QTcpServer m_server;
    QTcpSocket *m_socket = nullptr;
    QByteArray m_buffer;
    QTimer m_watchdog;
    QString m_host = QStringLiteral("127.0.0.1");
    quint16 m_port = 5001;
    qint64 m_seq = 0;
    QString m_peer;
};
