/*
 * @file VisionController.cpp
 * @brief 实现 VisionController：以 TCP Server 接收 Python 视觉 pose 数据。
 */
#include "VisionController.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcessEnvironment>

VisionController::VisionController(QObject *parent)
    : QObject(parent)
{
    m_tcpServer = new QTcpServer(this);
    connect(m_tcpServer, &QTcpServer::newConnection, this, &VisionController::onNewConnection);
}

VisionController::~VisionController()
{
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->terminate();
        m_process->waitForFinished(1000);
        if (m_process->state() != QProcess::NotRunning) {
            m_process->kill();
            m_process->waitForFinished(500);
        }
    }
    if (m_tcpServer) {
        m_tcpServer->close();
    }
}

bool VisionController::start()
{
    if (isRunning()) {
        qDebug() << "[Vision] 已在运行中，跳过 start";
        return true;
    }

    // 1) 确定 Python 项目目录（从 exe 所在目录向上查找）
    QDir dir(QCoreApplication::applicationDirPath());
    QString projectDir;
    while (!dir.isRoot()) {
        const QString candidate = dir.absolutePath() + "/external/VisualRoboticArm";
        if (QDir(candidate).exists()) {
            projectDir = candidate;
            break;
        }
        if (!dir.cdUp()) break;
    }
    if (projectDir.isEmpty()) {
        emit errorOccurred(QStringLiteral("找不到 Python 视觉项目 external/VisualRoboticArm"));
        return false;
    }
    m_projectPath = projectDir;

    // 2) 启动 TCP Server（监听 5001）
    if (!m_tcpServer->listen(QHostAddress::LocalHost, kServerPort)) {
        emit errorOccurred(QStringLiteral("TCP Server 监听 5001 失败: %1").arg(m_tcpServer->errorString()));
        return false;
    }
    qDebug() << "[Vision] TCP Server 已监听 127.0.0.1:5001";

    // 3) 启动 Python 进程（使用项目自带的 .venv）
    const QString pythonExe = projectDir + "/.venv/Scripts/python.exe";
    m_process = new QProcess(this);
    m_process->setWorkingDirectory(projectDir);
    m_process->setProgram(pythonExe);
    m_process->setArguments({QStringLiteral("-m"), QStringLiteral("main")});
    // 环境变量: 让 Python 连接到本进程监听的 5001
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert(QStringLiteral("VISUAL_ARM_COMM_PORT"), QString::number(kServerPort));
    m_process->setProcessEnvironment(env);

    connect(m_process, &QProcess::started, this, &VisionController::onStarted);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &VisionController::onReadyReadStdout);
    connect(m_process, &QProcess::readyReadStandardError, this, &VisionController::onReadyReadStderr);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &VisionController::onFinished);
    connect(m_process, &QProcess::errorOccurred, this, &VisionController::onProcessError);

    m_process->start();
    if (!m_process->waitForStarted(5000)) {
        const QString err = m_process->errorString();
        emit errorOccurred(QStringLiteral("Python 启动失败: ") + err);
        m_process->deleteLater();
        m_process = nullptr;
        m_tcpServer->close();
        return false;
    }

    qDebug() << "[Vision] Python 已启动 PID=" << m_process->processId();
    return true;
}

void VisionController::stop()
{
    // 关闭 TCP Server
    if (m_tcpServer) {
        m_tcpServer->close();
    }
    m_clientSocket = nullptr;
    m_tcpBuffer.clear();

    if (!m_process || m_process->state() == QProcess::NotRunning) {
        return;
    }

    m_process->terminate();
    if (!m_process->waitForFinished(1500)) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }
}

bool VisionController::isRunning() const
{
    return m_process != nullptr && m_process->state() != QProcess::NotRunning;
}

QString VisionController::projectPath() const
{
    return m_projectPath;
}

// ==================== QProcess 信号槽 ====================

void VisionController::onStarted()
{
    emit started();
}

void VisionController::onReadyReadStdout()
{
    if (!m_process) return;
    drainLines(m_process->readAllStandardOutput(), QStringLiteral("stdout"));
}

void VisionController::onReadyReadStderr()
{
    if (!m_process) return;
    drainLines(m_process->readAllStandardError(), QStringLiteral("stderr"));
}

void VisionController::onFinished(int exitCode, int /*exitStatus*/)
{
    if (!m_stdoutBuffer.isEmpty()) {
        emit outputMessage(QStringLiteral("stdout"), QString::fromUtf8(m_stdoutBuffer).trimmed());
        m_stdoutBuffer.clear();
    }
    if (!m_stderrBuffer.isEmpty()) {
        emit outputMessage(QStringLiteral("stderr"), QString::fromUtf8(m_stderrBuffer).trimmed());
        m_stderrBuffer.clear();
    }

    qDebug() << "[Vision] Python 已退出，exitCode=" << exitCode;
    emit stopped(exitCode);

    m_process->deleteLater();
    m_process = nullptr;
}

void VisionController::onProcessError(int /*error*/)
{
    const QString err = m_process ? m_process->errorString() : QStringLiteral("unknown");
    qWarning() << "[Vision ERROR]" << err;
    emit errorOccurred(err);
}

void VisionController::drainLines(const QByteArray &data, const QString &channel)
{
    if (data.isEmpty()) return;

    QByteArray &buffer = (channel == QStringLiteral("stderr")) ? m_stderrBuffer : m_stdoutBuffer;
    buffer.append(data);

    while (true) {
        const int newlineIndex = buffer.indexOf('\n');
        if (newlineIndex < 0) break;
        QByteArray line = buffer.left(newlineIndex);
        buffer.remove(0, newlineIndex + 1);
        line = line.trimmed();
        if (!line.isEmpty()) {
            emit outputMessage(channel, QString::fromUtf8(line));
        }
    }
}

// ==================== TCP Server ====================

void VisionController::onNewConnection()
{
    // 只保留一个客户端连接（与 Python 一一对应）
    if (m_clientSocket) {
        QTcpSocket *extra = m_tcpServer->nextPendingConnection();
        qWarning() << "[Vision] 已有客户端连接，拒绝新连接";
        extra->disconnectFromHost();
        extra->deleteLater();
        return;
    }

    m_clientSocket = m_tcpServer->nextPendingConnection();
    connect(m_clientSocket, &QTcpSocket::readyRead, this, &VisionController::onClientReadyRead);
    connect(m_clientSocket, &QTcpSocket::disconnected, this, &VisionController::onClientDisconnected);
    m_tcpBuffer.clear();

    qDebug() << "[Vision] Python 已连接 TCP Server"
             << m_clientSocket->peerAddress().toString()
             << m_clientSocket->peerPort();
}

void VisionController::onClientReadyRead()
{
    if (!m_clientSocket) return;

    m_tcpBuffer.append(m_clientSocket->readAll());

    while (true) {
        const int idx = m_tcpBuffer.indexOf('\n');
        if (idx < 0) break;

        const QByteArray line = m_tcpBuffer.left(idx).trimmed();
        m_tcpBuffer.remove(0, idx + 1);

        if (!line.isEmpty()) {
            parseAndDispatch(line);
        }
    }
}

void VisionController::onClientDisconnected()
{
    qDebug() << "[Vision] Python 已断开 TCP 连接";
    if (m_clientSocket) {
        m_clientSocket->deleteLater();
        m_clientSocket = nullptr;
    }
}

// ==================== JSON 解析 ====================

void VisionController::parseAndDispatch(const QByteArray &line)
{
    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(line, &err);
    if (doc.isNull()) {
        qWarning() << "[Vision] JSON 解析失败:" << err.errorString() << "raw:" << line;
        return;
    }

    const QJsonObject root = doc.object();

    // 只处理 type="pose" 消息
    const QString type = root.value(QStringLiteral("type")).toString();
    if (type != QStringLiteral("pose")) return;

    // 协议路径: payload.motor_6.angle_deg
    const QJsonObject payload = root.value(QStringLiteral("payload")).toObject();
    if (payload.isEmpty()) return;

    const QJsonObject motor6 = payload.value(QStringLiteral("motor_6")).toObject();
    if (motor6.isEmpty()) return;

    const double angle = motor6.value(QStringLiteral("angle_deg")).toDouble();
    qDebug() << "[Vision] 收到视觉指令 motor_6.angle_deg =" << angle;
    emit motorCommand(angle);
}