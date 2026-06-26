/*
 * @file VisionController.cpp
 * @brief 实现 VisionController 类：管理外部 VisualRoboticArmQtController.exe 子进程。
 */
#include "VisionController.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QProcess>

VisionController::VisionController(QObject *parent)
    : QObject(parent)
{
}

VisionController::~VisionController()
{
    // 析构时确保子进程被清理（QProcess 父对象机制也会处理）
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
        qDebug() << "[Vision] 已在运行中，跳过 start";
        return true;
    }

    // 1) 解析 exe 路径
    const QString path = m_exePath.isEmpty() ? resolveDefaultExePath() : m_exePath;
    if (path.isEmpty() || !QFile::exists(path)) {
        emit errorOccurred(QStringLiteral("找不到 VisualRoboticArmQtController.exe，请检查 external/VisualRoboticArm 是否已编译"));
        return false;
    }

    // 2) 创建 QProcess 并配置
    m_process = new QProcess(this);
    m_process->setProgram(path);
    // 工作目录设为 exe 所在目录（让外部程序能找到其依赖的 Qt DLL/插件）
    m_process->setWorkingDirectory(QFileInfo(path).absolutePath());
    m_resolvedExePath = path;

    // 3) 信号连接
    connect(m_process, &QProcess::started, this, &VisionController::onStarted);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &VisionController::onReadyReadStdout);
    connect(m_process, &QProcess::readyReadStandardError, this, &VisionController::onReadyReadStderr);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &VisionController::onFinished);
    connect(m_process, &QProcess::errorOccurred, this, &VisionController::onProcessError);

    // 4) 启动并等待（最多 3 秒）
    m_process->start();
    if (!m_process->waitForStarted(3000)) {
        const QString err = m_process->errorString();
        emit errorOccurred(QStringLiteral("启动失败: ") + err);
        m_process->deleteLater();
        m_process = nullptr;
        return false;
    }

    qDebug() << "[Vision] 已启动:" << path << "PID=" << m_process->processId();
    return true;
}

void VisionController::stop()
{
    if (!m_process || m_process->state() == QProcess::NotRunning) {
        return;
    }

    // 先温和结束（让外部程序走 closeEvent 优雅退出）
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

QString VisionController::exePath() const
{
    return m_resolvedExePath;
}

void VisionController::setExePath(const QString &path)
{
    m_exePath = path;
}

QString VisionController::resolveDefaultExePath()
{
    // 优先 Release 配置（与开发环境一致），回退到 Debug
    const QStringList candidates = {
        QStringLiteral("C:/Users/31422/Desktop/Robot_R06/external/VisualRoboticArm/qt_main_controller/"
                       "build/Desktop_Qt_6_9_0_MSVC2022_64bit-Release/VisualRoboticArmQtController.exe"),
        QStringLiteral("C:/Users/31422/Desktop/Robot_R06/external/VisualRoboticArm/qt_main_controller/"
                       "build/Debug/VisualRoboticArmQtController.exe"),
    };

    for (const QString &candidate : candidates) {
        if (QFile::exists(candidate)) {
            return candidate;
        }
    }
    return QString();
}

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
    // 进程退出，清理缓冲并 emit
    if (!m_stdoutBuffer.isEmpty()) {
        emit outputMessage(QStringLiteral("stdout"), QString::fromUtf8(m_stdoutBuffer).trimmed());
        m_stdoutBuffer.clear();
    }
    if (!m_stderrBuffer.isEmpty()) {
        emit outputMessage(QStringLiteral("stderr"), QString::fromUtf8(m_stderrBuffer).trimmed());
        m_stderrBuffer.clear();
    }

    qDebug() << "[Vision] 已退出，exitCode=" << exitCode;
    emit stopped(exitCode);

    // 子进程退出后 QProcess 仍可查询最终状态，但 state 会变回 NotRunning
    m_process->deleteLater();
    m_process = nullptr;
}

void VisionController::onProcessError(int /*error*/)
{
    // errorOccurred 信号只携带 ProcessError 枚举，错误细节用 errorString()
    const QString err = m_process ? m_process->errorString() : QStringLiteral("unknown");
    qWarning() << "[Vision ERROR]" << err;
    emit errorOccurred(err);
}

void VisionController::drainLines(const QByteArray &data, const QString &channel)
{
    if (data.isEmpty()) {
        return;
    }

    QByteArray &buffer = (channel == QStringLiteral("stderr")) ? m_stderrBuffer : m_stdoutBuffer;
    buffer.append(data);

    // 按 '\n' 切分，处理半行/多行情况
    while (true) {
        const int newlineIndex = buffer.indexOf('\n');
        if (newlineIndex < 0) {
            break;
        }
        QByteArray line = buffer.left(newlineIndex);
        buffer.remove(0, newlineIndex + 1);
        line = line.trimmed();
        if (!line.isEmpty()) {
            emit outputMessage(channel, QString::fromUtf8(line));
        }
    }
}