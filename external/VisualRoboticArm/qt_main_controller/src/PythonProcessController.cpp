#include "PythonProcessController.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QStandardPaths>

#include <functional>

PythonProcessController::PythonProcessController(QObject *parent)
    : QObject(parent)
{
    m_process.setProcessChannelMode(QProcess::SeparateChannels);

    connect(&m_process, &QProcess::started, this, &PythonProcessController::onStarted);
    connect(&m_process, &QProcess::finished, this, &PythonProcessController::onFinished);
    connect(&m_process, &QProcess::errorOccurred, this, &PythonProcessController::onErrorOccurred);
    connect(&m_process, &QProcess::readyReadStandardOutput, this, &PythonProcessController::onStdoutReady);
    connect(&m_process, &QProcess::readyReadStandardError, this, &PythonProcessController::onStderrReady);
}

bool PythonProcessController::start(const PythonLaunchConfig &config)
{
    if (isRunning()) {
        return true;
    }

    m_config = config;
    m_stdoutBuffer.clear();
    m_stderrBuffer.clear();

    QString pythonExe = m_config.pythonExecutable.trimmed();
    if (pythonExe.isEmpty()) {
        pythonExe = QStringLiteral("python");
    }

    // Python 项目根目录通过界面配置，避免写死电脑路径。
    // 如果界面留空，则按可执行文件位置向上推断，方便本地调试。
    QString projectRoot = m_config.projectRoot.trimmed();
    if (projectRoot.isEmpty()) {
        projectRoot = guessProjectRoot();
    }
    projectRoot = QDir::cleanPath(projectRoot);

    QString module = m_config.module.trimmed();
    if (module.isEmpty()) {
        module = QStringLiteral("main");
    }

    QString host = m_config.host.trimmed();
    if (host.isEmpty()) {
        host = QStringLiteral("127.0.0.1");
    }

    QString clientName = m_config.clientName.trimmed();
    if (clientName.isEmpty()) {
        clientName = QStringLiteral("qt-main");
    }

    // 使用模块方式启动，而不是直接写脚本路径。
    // 只要 projectRoot 已加入 PYTHONPATH，项目移动后启动命令仍可复用。
    QStringList arguments;
    arguments << QStringLiteral("-u")
              << QStringLiteral("-m")
              << module
              << QStringLiteral("--host")
              << host
              << QStringLiteral("--port")
              << QString::number(m_config.port)
              << QStringLiteral("--name")
              << clientName;
    arguments.append(m_config.extraArguments);

    // ===== 启动前自检：把每个关键变量和错误都打到 stderrLine，方便 UI 日志诊断 =====
    // Windows 上 "目录名称无效" 通常意味着 CWD 不存在或 Python 可执行路径解析失败。
    const QFileInfo exeInfo(pythonExe);
    if (!exeInfo.exists()) {
        emit stderrLine(QStringLiteral("[diag] Python 解释器路径不存在: %1").arg(pythonExe));
    } else if (exeInfo.isDir()) {
        emit stderrLine(QStringLiteral("[diag] Python 解释器路径是一个目录，不是可执行文件: %1").arg(pythonExe));
    }

    const QFileInfo rootInfo(projectRoot);
    if (!rootInfo.exists()) {
        emit stderrLine(QStringLiteral("[diag] 项目根目录不存在: %1").arg(projectRoot));
    } else if (!rootInfo.isDir()) {
        emit stderrLine(QStringLiteral("[diag] 项目根不是目录: %1").arg(projectRoot));
    } else {
        // 入口模块应当存在于 projectRoot/<module>.py，否则 -m 启动会失败。
        const QString entryScript = QDir(projectRoot).filePath(module + QStringLiteral(".py"));
        if (!QFileInfo::exists(entryScript)) {
            emit stderrLine(QStringLiteral("[diag] 找不到入口脚本: %1").arg(entryScript));
        }
    }

    emit stderrLine(QStringLiteral("[diag] python=%1").arg(pythonExe));
    emit stderrLine(QStringLiteral("[diag] cwd=%1").arg(projectRoot));
    emit stderrLine(QStringLiteral("[diag] cmd=%1 %2").arg(pythonExe, arguments.join(QLatin1Char(' '))));

    // PYTHONUNBUFFERED 让 Qt 能及时收到 Python 日志。
    // PYTHONPATH 保证 app.*、stage1_comm.* 等导入不依赖固定路径。
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    environment.insert(QStringLiteral("PYTHONPATH"), appendPythonPath(projectRoot));
    environment.insert(QStringLiteral("PYTHONUNBUFFERED"), QStringLiteral("1"));

    m_process.setProcessEnvironment(environment);
    m_process.setWorkingDirectory(projectRoot);
    m_process.setProgram(pythonExe);
    m_process.setArguments(arguments);
    m_process.start();

    if (!m_process.waitForStarted(3000)) {
        // waitForStarted 失败时，把 Qt 给出的 native 错误也透传出来。
        emit stderrLine(QStringLiteral("[diag] QProcess.error=%1, errorString=%2")
                            .arg(m_process.error())
                            .arg(m_process.errorString()));
        emit errorOccurred(QStringLiteral("Python 启动失败: %1").arg(m_process.errorString()));
        return false;
    }

    m_config.projectRoot = projectRoot;
    return true;
}

void PythonProcessController::stop()
{
    if (!isRunning()) {
        return;
    }

    // 先尝试温和结束；若 Python UI 没有及时退出，再强制结束进程。
    m_process.terminate();
    if (!m_process.waitForFinished(1500)) {
        m_process.kill();
        m_process.waitForFinished(1500);
    }
}

bool PythonProcessController::restart(const PythonLaunchConfig &config)
{
    stop();
    return start(config);
}

bool PythonProcessController::isRunning() const
{
    return m_process.state() != QProcess::NotRunning;
}

QString PythonProcessController::workingDirectory() const
{
    return m_process.workingDirectory();
}

void PythonProcessController::onStarted()
{
    emit started(QStringLiteral("Python 进程已启动: %1").arg(m_process.program()));
}

void PythonProcessController::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::CrashExit || exitCode != 0) {
        emit crashed(exitCode);
    }
    emit finished(exitCode);
}

void PythonProcessController::onErrorOccurred(QProcess::ProcessError error)
{
    const QString message = [error]() -> QString {
        switch (error) {
        case QProcess::FailedToStart:
            return QStringLiteral("Python 进程启动失败");
        case QProcess::Crashed:
            return QStringLiteral("Python 进程崩溃");
        case QProcess::Timedout:
            return QStringLiteral("Python 进程超时");
        case QProcess::WriteError:
            return QStringLiteral("Python 进程写入失败");
        case QProcess::ReadError:
            return QStringLiteral("Python 进程读取失败");
        case QProcess::UnknownError:
        default:
            return QStringLiteral("Python 进程未知错误");
        }
    }();

    emit errorOccurred(QStringLiteral("%1: %2").arg(message, m_process.errorString()));
}

void PythonProcessController::onStdoutReady()
{
    const QByteArray chunk = m_process.readAllStandardOutput();
    drainLines(m_stdoutBuffer, chunk, [this](const QString &line) {
        emit stdoutLine(line);
    });
}

void PythonProcessController::onStderrReady()
{
    const QByteArray chunk = m_process.readAllStandardError();
    drainLines(m_stderrBuffer, chunk, [this](const QString &line) {
        emit stderrLine(line);
    });
}

QString PythonProcessController::guessProjectRoot()
{
    // 调试输出目录通常位于 qt_main_controller/build/Debug，
    // 但也可能直接是 build/，因此向上多看几级，找到第一个含 stage1_comm 或 app 的目录。
    QDir dir(QCoreApplication::applicationDirPath());
    for (int i = 0; i < 6; ++i) {
        const QString candidate = dir.absolutePath();
        const bool looksLikeProjectRoot =
            QFileInfo::exists(QDir(candidate).filePath(QStringLiteral("stage1_comm"))) ||
            QFileInfo::exists(QDir(candidate).filePath(QStringLiteral("app")));
        if (looksLikeProjectRoot) {
            return candidate;
        }
        if (!dir.cdUp()) {
            break;
        }
    }

    // 没找到项目根目录时，回退到可执行文件所在目录；记到日志里方便用户察觉。
    return QCoreApplication::applicationDirPath();
}

QString PythonProcessController::appendPythonPath(const QString &projectRoot)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const QString current = env.value(QStringLiteral("PYTHONPATH"));
    if (current.isEmpty()) {
        return projectRoot;
    }
    return projectRoot + QLatin1Char(';') + current;
}

void PythonProcessController::drainLines(QByteArray &buffer, const QByteArray &chunk, const std::function<void(const QString &)> &emitLine)
{
    buffer.append(chunk);
    while (true) {
        const int newlineIndex = buffer.indexOf('\n');
        if (newlineIndex < 0) {
            break;
        }

        QByteArray line = buffer.left(newlineIndex);
        buffer.remove(0, newlineIndex + 1);
        line = line.trimmed();
        if (!line.isEmpty()) {
            emitLine(QString::fromUtf8(line));
        }
    }
}
