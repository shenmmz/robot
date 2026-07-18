/*
 * @file VisionController.cpp
 * @brief 实现 VisionController：以 QTcpServer 监听 5001，接收 Python 视觉 pose 数据。
 *
 * 架构：
 * - Qt 端：QTcpServer listen(127.0.0.1, 5001) → 等待 Python PoseCommClient connect
 * - Python 端：QTcpSocket connectToHost(127.0.0.1, 5001) → 主动连接 Qt
 * - 这样才符合 pose_comm.py 现有设计与此前 Qt 的 Server 期望
 */
#include "VisionController.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHostAddress>
#include <QProcess>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcessEnvironment>

VisionController::VisionController(QObject *parent)
    : QObject(parent)
    , m_tcpServer(nullptr)
    , m_tcpSocket(nullptr)
{
    // m_tcpSocket 在 onNewConnection 里由 server 取出赋值，不再构造函数里 new
}

VisionController::~VisionController()
{
    if (m_tcpServer) {
        m_tcpServer->close();
    }
    if (m_tcpSocket) {
        m_tcpSocket->disconnectFromHost();
    }
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->terminate();
        m_process->waitForFinished(1000);
        if (m_process->state() != QProcess::NotRunning) {
            m_process->kill();
            m_process->waitForFinished(500);
        }
    }
}

bool VisionController::start()
{
    if (isRunning()) {
//         qDebug() << "[Vision] 已在运行中，跳过 start";
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

    // 2) 先起 TCP Server 监听 5001（必须先 listen，Python 才能 connectToHost 进来）
    if (!m_tcpServer) {
        m_tcpServer = new QTcpServer(this);
        connect(m_tcpServer, &QTcpServer::newConnection,
                this, &VisionController::onNewConnection);
    }
    if (!m_tcpServer->listen(QHostAddress::LocalHost, kServerPort)) {
        emit errorOccurred(QStringLiteral("Vision TCP Server 启动失败: %1")
                           .arg(m_tcpServer->errorString()));
        return false;
    }
//     qDebug() << "[Vision] TCP Server 已监听 127.0.0.1:" << kServerPort
//              << " (等待 Python 连接)";

    // 3) 启动 Python 进程（使用项目自带的 .venv）
    const QString pythonExe = projectDir + "/.venv/Scripts/python.exe";
    m_process = new QProcess(this);
    m_process->setWorkingDirectory(projectDir);
    m_process->setProgram(pythonExe);
    m_process->setArguments({QStringLiteral("-m"), QStringLiteral("main")});
    // 环境变量: 让 Python 连接到本进程监听的 5001
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert(QStringLiteral("VISUAL_ARM_COMM_HOST"), QStringLiteral("127.0.0.1"));
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

//     qDebug() << "[Vision] Python 已启动 PID=" << m_process->processId();
//     qDebug() << "[Vision] 等待 Python TCP 客户端连入 127.0.0.1:" << kServerPort << " ...";
    return true;
}

void VisionController::stop()
{
    // 先断 TCP socket（让 server 安静下来，再关 server）
    if (m_tcpSocket) {
        m_tcpSocket->disconnectFromHost();
        m_tcpSocket->deleteLater();
        m_tcpSocket = nullptr;
    }
    m_tcpBuffer.clear();

    if (m_tcpServer) {
        m_tcpServer->close();
    }

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

//     qDebug() << "[Vision] Python 已退出，exitCode=" << exitCode;
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
    if (!m_tcpServer) return;

    // 如果已有 socket，先释放旧的（保持单连接语义）
    if (m_tcpSocket) {
//         qDebug() << "[Vision] 已有 socket，断开旧连接换新连接";
        m_tcpSocket->disconnectFromHost();
        m_tcpSocket->deleteLater();
        m_tcpSocket = nullptr;
    }

    m_tcpSocket = m_tcpServer->nextPendingConnection();
    if (!m_tcpSocket) return;
    m_tcpBuffer.clear();

    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &VisionController::onReadyRead);
    connect(m_tcpSocket, &QTcpSocket::disconnected, this, &VisionController::onClientDisconnected);

//     qDebug() << "[Vision] Python TCP 客户端已连接 from"
//              << m_tcpSocket->peerAddress().toString()
//              << ":" << m_tcpSocket->peerPort()
//              << " state=" << m_tcpSocket->state();
}

void VisionController::onReadyRead()
{
    if (!m_tcpSocket) return;

    const QByteArray chunk = m_tcpSocket->readAll();
//     qDebug() << "[Vision] tcp-receive bytes=" << chunk.size()
//              << " preview=" << QString::fromUtf8(chunk.left(80));
    m_tcpBuffer.append(chunk);

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
//     qDebug() << "[Vision] Python 已断开 TCP 连接 errorString=" << m_tcpSocket->errorString()
//              << " state=" << m_tcpSocket->state();
}

// ==================== JSON 解析 ====================

void VisionController::parseAndDispatch(const QByteArray &line)
{
//     qDebug() << "[Vision] recv-line len=" << line.size()
//              << " preview=" << QString::fromUtf8(line.left(120));

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(line, &err);
    if (doc.isNull()) {
        qWarning() << "[Vision] JSON 解析失败:" << err.errorString()
                   << " raw(末尾200)=" << QString::fromUtf8(line.right(200));
        return;
    }

    const QJsonObject root = doc.object();

    // 只处理 type="pose" 消息 (其它 type 之前会被静默丢弃, 现在改成 qDebug 便于排查)
    const QString type = root.value(QStringLiteral("type")).toString();
    if (type.isEmpty()) {
//         qDebug() << "[Vision] 跳过: msg 无 type 字段";
        return;
    }
    if (type != QStringLiteral("pose")) {
//         qDebug() << "[Vision] skip non-pose type=" << type;
        return;
    }

    // 协议路径: payload.motor_commands (首选, 多轴列表) 或 payload.motor_6 (兼容兜底, 单轴)
    const QJsonObject payload = root.value(QStringLiteral("payload")).toObject();
    if (payload.isEmpty()) return;

    // 1) 首选：motor_commands 数组（每条 {motor_id, signed_angle_deg, ...}）
    const QJsonArray cmds = payload.value(QStringLiteral("motor_commands")).toArray();
    if (!cmds.isEmpty()) {
        QList<VisionMotorCommand> list;
        list.reserve(cmds.size());
        for (const QJsonValue& v : cmds) {
            if (!v.isObject()) continue;
            const QJsonObject cmd = v.toObject();
            const int motorId = cmd.value(QStringLiteral("motor_id")).toInt(-1);
            const double signedAngle = cmd.value(QStringLiteral("signed_angle_deg")).toDouble();
            if (motorId < 1 || motorId > 6) {
                qWarning() << "[Vision] motor_commands 中 motor_id 越界:" << motorId;
                continue;
            }
            // 兼容性：有的 Python 版本在 valid==false 时不写 signed_angle_deg，
            // 此时退回读 angle_deg（仍是正值，需结合 direction 推断符号；
            // 这里保守做法：仅当 direction 字段是 "out"/"in" 才赋符号，其它保持原值）。
            double angle = signedAngle;
            if (qIsNaN(angle) || !cmd.contains(QStringLiteral("signed_angle_deg"))) {
                const double raw = cmd.value(QStringLiteral("angle_deg")).toDouble();
                const QString dir = cmd.value(QStringLiteral("direction")).toString();
                if (dir == QStringLiteral("in")) {
                    angle = -qAbs(raw);
                } else if (dir == QStringLiteral("out")) {
                    angle = qAbs(raw);
                } else {
                    angle = raw;
                }
            }
            list.append({motorId - 1, angle});  // motor_id 1..6 → axis 0..5
        }
        if (!list.isEmpty()) {
//             qDebug() << "[Vision] 收到多轴指令" << list.size() << "条";
            emit motorCommands(list);
        }
        return;
    }

    // 2) 兼容老协议：motor_6 单轴
    const QJsonObject motor6 = payload.value(QStringLiteral("motor_6")).toObject();
    if (motor6.isEmpty()) return;

    const double angle = motor6.value(QStringLiteral("angle_deg")).toDouble();
//     qDebug() << "[Vision] 收到 motor_6 兼容指令 angle_deg =" << angle;
    emit motorCommand(angle);
}