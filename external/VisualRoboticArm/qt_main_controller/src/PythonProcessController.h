#pragma once

#include <QByteArray>
#include <QObject>
#include <QProcess>
#include <QStringList>
#include <functional>

struct PythonLaunchConfig
{
    // 可保持为 "python" 并依赖系统 PATH，也可由界面/配置文件填入目标电脑上的 python.exe 绝对路径。
    QString pythonExecutable = QStringLiteral("python");

    // 使用 "python -m <module>" 启动模块。当前视觉程序入口为 main.py，
    // 命令行参数支持 --host / --port / --name。
    QString module = QStringLiteral("main");

    // Python 项目根目录，会加入 PYTHONPATH，同时作为 QProcess 工作目录。
    // 测试当前工程时应指向 VisualRoboticArm 根目录。
    QString projectRoot;
    QString host = QStringLiteral("127.0.0.1");
    quint16 port = 5001;
    QString clientName = QStringLiteral("qt-main");
    QStringList extraArguments;
};

// QProcess 的轻量封装，只负责生命周期隔离：
// Qt 可以启动/停止/重启 Python，并读取 stdout/stderr；
// Python 崩溃不会拖垮 Qt 主控。电机控制必须保留在正式 C++ 主项目中。
class PythonProcessController : public QObject
{
    Q_OBJECT

public:
    explicit PythonProcessController(QObject *parent = nullptr);

    bool start(const PythonLaunchConfig &config);
    void stop();
    bool restart(const PythonLaunchConfig &config);

    bool isRunning() const;
    QString workingDirectory() const;

signals:
    void started(const QString &status);
    void finished(int exitCode);
    void crashed(int exitCode);
    void stdoutLine(const QString &line);
    void stderrLine(const QString &line);
    void errorOccurred(const QString &error);

private slots:
    void onStarted();
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onErrorOccurred(QProcess::ProcessError error);
    void onStdoutReady();
    void onStderrReady();

private:
    static QString guessProjectRoot();
    static QString appendPythonPath(const QString &projectRoot);

    void drainLines(QByteArray &buffer, const QByteArray &chunk, const std::function<void(const QString &)> &emitLine);

    QProcess m_process;
    PythonLaunchConfig m_config;
    QByteArray m_stdoutBuffer;
    QByteArray m_stderrBuffer;
};
