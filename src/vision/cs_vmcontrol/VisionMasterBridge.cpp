#include "VisionMasterBridge.h"

#include <QProcess>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

VisionMasterBridge::VisionMasterBridge(QObject *parent)
    : QObject(parent)
{
    qDebug() << "[Bridge] constructor (QProcess mode)";
    m_exePath = findExePath();
    qDebug() << "[Bridge] CS-VMC-Dome.exe path:" << m_exePath;
}

VisionMasterBridge::~VisionMasterBridge()
{
    terminate();
}

QString VisionMasterBridge::findExePath() const
{
    const QString appDir = QCoreApplication::applicationDirPath();

    // 查找顺序：
    // 1. 可执行文件目录/CS-VMC-Dome.exe （POST_BUILD 复制后）
    // 2. 源码构建目录 external/CS-VMC-Dome/build/.../CS-VMC-Dome.exe
    const QStringList candidates = {
        appDir + QStringLiteral("/CS-VMC-Dome.exe"),
        appDir + QStringLiteral("/../../../external/CS-VMC-Dome/build/Desktop_Qt_6_9_0_MSVC2022_64bit-Release/CS-VMC-Dome.exe"),
        appDir + QStringLiteral("/../../external/CS-VMC-Dome/build/Desktop_Qt_6_9_0_MSVC2022_64bit-Release/CS-VMC-Dome.exe"),
    };

    for (const auto &p : candidates) {
        if (QFileInfo::exists(p)) {
            return QDir(p).absolutePath();
        }
    }

    qWarning() << "[Bridge] CS-VMC-Dome.exe not found! Searched:" << candidates;
    return QString();
}

bool VisionMasterBridge::launch()
{
    if (m_process && m_process->state() != QProcess::NotRunning) {
        emit logMessage(QStringLiteral("视觉程序已在运行中"));
        return true;
    }

    if (m_exePath.isEmpty()) {
        m_exePath = findExePath();
    }

    if (m_exePath.isEmpty() || !QFileInfo::exists(m_exePath)) {
        const QString msg = QStringLiteral("未找到 CS-VMC-Dome.exe，请先构建 CS-VMC-Dome 项目");
        qWarning() << "[Bridge]" << msg;
        emit logMessage(msg);
        return false;
    }

    if (!m_process) {
        m_process = new QProcess(this);
        connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &VisionMasterBridge::onProcessFinished);
        connect(m_process, &QProcess::errorOccurred,
                this, &VisionMasterBridge::onProcessErrorOccurred);
        connect(m_process, &QProcess::readyReadStandardOutput,
                this, &VisionMasterBridge::onReadyReadStandardOutput);
    }

    // 设置工作目录为 exe 所在目录（CS-VMC-Dome 需要访问同目录下的 DLL、方案文件等）
    m_process->setWorkingDirectory(QFileInfo(m_exePath).absolutePath());

    qDebug() << "[Bridge] launching:" << m_exePath;
    m_process->start(m_exePath, QStringList());

    if (!m_process->waitForStarted(5000)) {
        const QString msg = QStringLiteral("启动 CS-VMC-Dome.exe 失败: %1")
                                .arg(m_process->errorString());
        qWarning() << "[Bridge]" << msg;
        emit logMessage(msg);
        return false;
    }

    qDebug() << "[Bridge] CS-VMC-Dome.exe started, PID:" << m_process->processId();
    emit processStarted();
    emit logMessage(QStringLiteral("视觉程序已启动 (PID: %1)")
                        .arg(m_process->processId()));
    return true;
}

void VisionMasterBridge::terminate()
{
    if (!m_process || m_process->state() == QProcess::NotRunning) {
        return;
    }

    qDebug() << "[Bridge] terminating CS-VMC-Dome.exe...";

    // 先尝试优雅关闭
    m_process->terminate();

    // 等待 3 秒，超时则强制杀死
    if (!m_process->waitForFinished(3000)) {
        qDebug() << "[Bridge] graceful terminate failed, killing...";
        m_process->kill();
        m_process->waitForFinished(2000);
    }

    emit logMessage(QStringLiteral("视觉程序已关闭"));
}

bool VisionMasterBridge::isRunning() const
{
    return m_process && m_process->state() != QProcess::NotRunning;
}

void VisionMasterBridge::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "[Bridge] CS-VMC-Dome.exe finished, exitCode:" << exitCode
             << "exitStatus:" << exitStatus;

    QString msg;
    if (exitStatus == QProcess::CrashExit) {
        msg = QStringLiteral("视觉程序异常退出 (崩溃)");
    } else {
        msg = QStringLiteral("视觉程序已退出 (代码: %1)").arg(exitCode);
    }

    emit logMessage(msg);
    emit processFinished(exitCode);
}

void VisionMasterBridge::onProcessErrorOccurred(QProcess::ProcessError error)
{
    qWarning() << "[Bridge] process error:" << error
             << "-" << (m_process ? m_process->errorString() : QString());

    emit logMessage(QStringLiteral("视觉程序错误: %1")
                        .arg(m_process ? m_process->errorString() : QStringLiteral("未知错误")));
}

void VisionMasterBridge::onReadyReadStandardOutput()
{
    if (!m_process) return;

    // 读取所有可用数据，追加到缓冲区
    m_stdoutBuffer += m_process->readAllStandardOutput();

    // 按行拆分处理（CS-VMC-Dome 输出的是 UTF-8 编码，每行一个 VISION_* 消息）
    int idx;
    while ((idx = m_stdoutBuffer.indexOf('\n')) >= 0) {
        QByteArray lineData = m_stdoutBuffer.left(idx).trimmed();
        m_stdoutBuffer = m_stdoutBuffer.mid(idx + 1);

        if (lineData.isEmpty()) continue;

        // UTF-8 解码
        const QString line = QString::fromUtf8(lineData);

        // 发射信号供 MainWindow 处理
        emit visionDataReceived(line);

        // 同时输出到日志
        qDebug() << "[Bridge] vision data:" << line;
    }
}
