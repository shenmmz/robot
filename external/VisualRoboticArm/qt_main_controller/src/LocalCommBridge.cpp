#include "LocalCommBridge.h"

#include "JsonProtocol.h"

#include <QHostAddress>

LocalCommBridge::LocalCommBridge(QObject *parent)
    : QObject(parent)
{
    connect(&m_server, &QTcpServer::newConnection, this, &LocalCommBridge::onNewConnection);

    m_watchdog.setInterval(1000);
    connect(&m_watchdog, &QTimer::timeout, this, &LocalCommBridge::emitStatus);
}

bool LocalCommBridge::startServer(const QString &host, quint16 port)
{
    // host 为空时默认监听本机。验证阶段保持 127.0.0.1 更安全；
    // 若后续开放到局域网，应先增加访问控制或白名单。
    const QString normalizedHost = host.trimmed().isEmpty() ? QStringLiteral("127.0.0.1") : host.trimmed();
    if (m_server.isListening() && m_host == normalizedHost && m_port == port) {
        return true;
    }

    if (m_server.isListening()) {
        shutdownServer(false);
    }

    m_host = normalizedHost;
    m_port = port;

    const QHostAddress address = resolveAddress(m_host);
    if (address.isNull()) {
        emit errorOccurred(QStringLiteral("Invalid host address: %1").arg(m_host));
        return false;
    }

    if (!m_server.listen(address, m_port)) {
        emit errorOccurred(QStringLiteral("Failed to listen on %1:%2 (%3)")
                               .arg(m_host)
                               .arg(m_port)
                               .arg(m_server.errorString()));
        return false;
    }

    m_watchdog.start();
    emit statusChanged(QStringLiteral("Qt 服务已启动: %1:%2").arg(m_host).arg(m_port));
    return true;
}

void LocalCommBridge::stopServer()
{
    shutdownServer(true);
}

bool LocalCommBridge::isListening() const
{
    return m_server.isListening();
}

bool LocalCommBridge::isConnected() const
{
    return m_socket != nullptr && m_socket->state() == QAbstractSocket::ConnectedState;
}

QString LocalCommBridge::host() const
{
    return m_host;
}

quint16 LocalCommBridge::port() const
{
    return m_port;
}

QString LocalCommBridge::peerDescription() const
{
    return m_peer;
}

bool LocalCommBridge::sendMessage(const QString &type,
                                  const QJsonObject &payload,
                                  const QString &msg,
                                  const QJsonObject &extra)
{
    if (!isConnected() || m_socket == nullptr) {
        emit errorOccurred(QStringLiteral("发送失败，当前没有已连接的 Python 客户端"));
        return false;
    }

    ++m_seq;
    const QJsonObject message = armcomm::buildMessage(type, m_seq, QStringLiteral("qt"), payload, msg, extra);
    const QByteArray bytes = armcomm::encodeMessage(message);
    const qint64 written = m_socket->write(bytes);
    m_socket->flush();
    if (written < 0) {
        emit errorOccurred(m_socket->errorString());
        return false;
    }
    return true;
}

bool LocalCommBridge::sendCommand(const QString &text)
{
    return sendMessage(QStringLiteral("cmd"),
                       QJsonObject{{QStringLiteral("text"), text}},
                       text);
}

void LocalCommBridge::onNewConnection()
{
    QTcpSocket *socket = m_server.nextPendingConnection();
    if (socket == nullptr) {
        return;
    }

    // 当前测试框架只保留一个 Python 客户端连接。
    // 新连接会替换旧连接，避免调试重启后残留无效 socket。
    if (m_socket != nullptr) {
        closeSocket(false);
    }

    m_socket = socket;
    m_buffer.clear();
    m_peer = QStringLiteral("%1:%2")
                 .arg(socket->peerAddress().toString())
                 .arg(socket->peerPort());

    connect(m_socket, &QTcpSocket::readyRead, this, &LocalCommBridge::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &LocalCommBridge::onDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &LocalCommBridge::onSocketError);

    emit clientConnected(m_peer);
    emit statusChanged(QStringLiteral("Python 客户端已连接: %1").arg(m_peer));
}

void LocalCommBridge::onReadyRead()
{
    if (m_socket == nullptr) {
        return;
    }

    // TCP 是字节流，一次 readyRead 可能只有半条消息，也可能包含多条消息。
    // 当前协议使用 '\n' 作为一条 JSON 消息的边界。
    m_buffer.append(m_socket->readAll());
    while (true) {
        const int newlineIndex = m_buffer.indexOf('\n');
        if (newlineIndex < 0) {
            break;
        }

        QByteArray line = m_buffer.left(newlineIndex);
        m_buffer.remove(0, newlineIndex + 1);
        line = line.trimmed();
        if (line.isEmpty()) {
            continue;
        }

        QJsonObject message;
        QString error;
        if (!armcomm::decodeMessage(line, &message, &error)) {
            emit errorOccurred(QStringLiteral("JSON 解析失败: %1").arg(error));
            continue;
        }

        emit messageReceived(message);
    }
}

void LocalCommBridge::onDisconnected()
{
    closeSocket(false);
    emit clientDisconnected();
    emit statusChanged(QStringLiteral("Python 客户端已断开"));
}

void LocalCommBridge::onSocketError(QAbstractSocket::SocketError error)
{
    if (error == QAbstractSocket::RemoteHostClosedError) {
        return;
    }
    if (m_socket == nullptr) {
        return;
    }
    emit errorOccurred(m_socket->errorString());
}

void LocalCommBridge::emitStatus()
{
    if (isConnected()) {
        emit statusChanged(QStringLiteral("Qt 监听中，Python 已连接: %1").arg(m_peer));
    } else if (m_server.isListening()) {
        emit statusChanged(QStringLiteral("Qt 监听中，等待 Python 连接: %1:%2").arg(m_host).arg(m_port));
    }
}

void LocalCommBridge::shutdownServer(bool emitStatusSignal)
{
    m_watchdog.stop();
    closeSocket(false);
    if (m_server.isListening()) {
        m_server.close();
    }
    if (emitStatusSignal) {
        emit statusChanged(QStringLiteral("Qt 服务已停止"));
    }
}

void LocalCommBridge::closeSocket(bool emitSignal)
{
    if (m_socket == nullptr) {
        return;
    }

    disconnect(m_socket, &QTcpSocket::readyRead, this, &LocalCommBridge::onReadyRead);
    disconnect(m_socket, &QTcpSocket::disconnected, this, &LocalCommBridge::onDisconnected);
    disconnect(m_socket, &QTcpSocket::errorOccurred, this, &LocalCommBridge::onSocketError);

    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
    }
    m_socket->deleteLater();
    m_socket = nullptr;
    m_buffer.clear();
    m_peer.clear();

    if (emitSignal) {
        emit clientDisconnected();
    }
}

QHostAddress LocalCommBridge::resolveAddress(const QString &host) const
{
    if (host.compare(QStringLiteral("localhost"), Qt::CaseInsensitive) == 0 ||
        host == QStringLiteral("127.0.0.1")) {
        return QHostAddress::LocalHost;
    }

    QHostAddress address;
    if (!address.setAddress(host)) {
        return QHostAddress();
    }
    return address;
}
