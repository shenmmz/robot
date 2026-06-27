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

/**
 * @brief 构造 VisionController,创建并初始化内部的 QTcpServer。
 *
 * 负责:
 * - 创建 QTcpServer 实例并将其父子对象设为 this,保证生命周期随本对象释放。
 * - 将 QTcpServer::newConnection 信号连接至本对象的 onNewConnection 槽。
 *
 * @param parent Qt 父对象指针,默认 nullptr。
 */
VisionController::VisionController(QObject *parent)
    : QObject(parent)
{
    m_tcpServer = new QTcpServer(this);
    connect(m_tcpServer, &QTcpServer::newConnection, this, &VisionController::onNewConnection);
}

/**
 * @brief 析构函数,负责安全释放子进程与 TCP Server 资源。
 *
 * 释放顺序:
 * - 若 Python 子进程仍在运行,先 terminate(),等待 1 秒后若仍未退出则 kill() 强杀。
 * - 关闭 QTcpServer,断开所有 TCP 连接。
 */
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

/**
 * @brief 启动 TCP Server 并拉起 Python 视觉程序。
 *
 * 流程:
 * - 若已经在运行,直接返回 true(幂等)。
 * - 从 exe 所在目录向上查找 external/VisualRoboticArm 子目录,作为 Python 项目根。
 * - QTcpServer 监听 127.0.0.1:5001;失败时 emit errorOccurred 并返回 false。
 * - 构造 QProcess,使用项目自带的 .venv/Scripts/python.exe,设置工作目录和
 *   VISUAL_ARM_COMM_PORT=5001 环境变量,然后 start() 并 waitForStarted(5 秒)。
 * - 失败时负责回滚:deleteLater、置空 m_process、关闭 TCP Server。
 *
 * @return true 表示 TCP 监听成功且 QProcess 已成功拉起。
 */
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

/**
 * @brief 停止 Python 子进程并关闭 TCP Server。
 *
 * 流程:
 * - 关闭 QTcpServer,清空 m_clientSocket 和 m_tcpBuffer(连接会在子进程结束前被强制断开)。
 * - 若 QProcess 不存在或已退出,直接返回。
 * - 否则先 terminate(),等待 1.5 秒;超时则 kill() 强杀,再 waitForFinished(1 秒)。
 */
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

/**
 * @brief 判断 Python 视觉子进程是否仍在运行。
 * @return true 表示 m_process 非空且状态不是 QProcess::NotRunning。
 */
bool VisionController::isRunning() const
{
    return m_process != nullptr && m_process->state() != QProcess::NotRunning;
}

/**
 * @brief 获取 Python 项目目录路径。
 *
 * 即 start() 中向上搜索得到的 external/VisualRoboticArm 绝对路径。
 * 若尚未 start,返回空字符串。
 */
QString VisionController::projectPath() const
{
    return m_projectPath;
}

// ==================== QProcess 信号槽 ====================

/**
 * @brief QProcess::started 信号槽,Python 子进程成功启动后立即触发。
 *
 * 透传:emit started() 通知外部 UI / 业务层。
 */
void VisionController::onStarted()
{
    emit started();
}

/**
 * @brief QProcess::readyReadStandardOutput 信号槽。
 *
 * 读取 stdout 全部可用字节并交给 drainLines 按 '\n' 切行。
 */
void VisionController::onReadyReadStdout()
{
    if (!m_process) return;
    drainLines(m_process->readAllStandardOutput(), QStringLiteral("stdout"));
}

/**
 * @brief QProcess::readyReadStandardError 信号槽。
 *
 * 读取 stderr 全部可用字节并交给 drainLines 按 '\n' 切行。
 */
void VisionController::onReadyReadStderr()
{
    if (!m_process) return;
    drainLines(m_process->readAllStandardError(), QStringLiteral("stderr"));
}

/**
 * @brief QProcess::finished 信号槽,Python 子进程退出时触发。
 *
 * 流程:
 * - 先把 stdout/stderr 缓冲区中残留的最后一行(没有 '\n' 结尾)冲刷出去。
 * - qDebug 输出 exitCode,emit stopped(exitCode) 通知业务层。
 * - 通过 deleteLater 释放 m_process 并置空指针。
 *
 * @param exitCode   Python 进程退出码。
 * @param exitStatus 退出状态(正常 / 崩溃),本函数暂不区分。
 */
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

/**
 * @brief QProcess::errorOccurred 信号槽。
 *
 * 把 QProcess::errorString 透传给 errorOccurred 信号,并 qWarning 记录日志。
 *
 * @param error QProcess 错误枚举值,本函数未使用。
 */
void VisionController::onProcessError(int /*error*/)
{
    const QString err = m_process ? m_process->errorString() : QStringLiteral("unknown");
    qWarning() << "[Vision ERROR]" << err;
    emit errorOccurred(err);
}

/**
 * @brief 将 QByteArray 数据按 '\n' 切行,并逐行 emit outputMessage。
 *
 * 根据 channel 字符串选择累加缓冲:
 * - "stderr" → m_stderrBuffer
 * - 其它    → m_stdoutBuffer
 *
 * 切分策略: 在缓冲区中查找第一个 '\n',取出该行 trim 后 emit;剩余字节继续保留在缓冲区等待下次数据。
 *
 * @param data    本次 readyRead 读到的字节片段。
 * @param channel "stdout" 或 "stderr",决定使用哪个累加缓冲。
 */
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

/**
 * @brief QTcpServer::newConnection 信号槽。
 *
 * 单连接策略: 已存在 m_clientSocket 时,取到 nextPendingConnection 后立即断开并 deleteLater,
 * qWarning 记录日志后返回。否则接受该连接作为 m_clientSocket,绑定 readyRead / disconnected 信号,
 * 清空 m_tcpBuffer,qDebug 打印对端地址和端口。
 */
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

/**
 * @brief QTcpSocket::readyRead 信号槽。
 *
 * 把当前可读字节追加进 m_tcpBuffer,然后按 '\n' 切包,每得到完整一行(已 trim)就交给 parseAndDispatch 解析。
 * 不完整的尾部字节保留在 m_tcpBuffer 中,等待下次数据到达时继续拼接。
 */
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

/**
 * @brief QTcpSocket::disconnected 信号槽。
 *
 * Python 端断开 TCP 连接后触发:qDebug 记录日志,通过 deleteLater 释放 m_clientSocket 并置空指针。
 */
void VisionController::onClientDisconnected()
{
    qDebug() << "[Vision] Python 已断开 TCP 连接";
    if (m_clientSocket) {
        m_clientSocket->deleteLater();
        m_clientSocket = nullptr;
    }
}

// ==================== JSON 解析 ====================

/**
 * @brief 解析一帧 JSON 并下发对应的业务信号。
 *
 * 协议约定(只关心 type=="pose" 的消息):
 * @code
 * {
 *   "type": "pose",
 *   "payload": {
 *     "motor_6": { "angle_deg": <double> }
 *   }
 * }
 * @endcode
 *
 * 解析失败时 qWarning 输出错误信息和原始字节;
 * type 非 "pose"、payload / motor_6 缺失时静默忽略;
 * 解析成功后 emit motorCommand(angle)。
 *
 * @param line 单行 UTF-8 JSON 文本(已去除 '\n' 两侧空白)。
 */
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