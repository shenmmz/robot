#pragma once

#include <QElapsedTimer>
#include <QCloseEvent>
#include <QJsonArray>
#include <QMainWindow>
#include <QPointer>
#include <QTimer>
#include <QJsonObject>
#include <QTextEdit>

class QCheckBox;
class QGroupBox;
class QLineEdit;
class QLabel;
class QPlainTextEdit;
class QPushButton;
class QSpinBox;
class QSplitter;

class LocalCommBridge;
class PythonProcessController;
struct PythonLaunchConfig;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    // 测试主控退出时停止 Python 并关闭 TCP 服务。
    // 正式项目中可把这段关闭流程移入主程序生命周期管理。
    void closeEvent(QCloseEvent *event) override;

private slots:
    // 测试界面使用的 Python 启停控制。
    // 正式项目中可改为由配置或主程序状态机自动启动。
    void startPython();
    void stopPython();
    void restartPython();
    void browsePythonExecutable();
    void browseProjectRoot();
    void sendCommand();
    void clearLog();

    void onClientConnected(const QString &peer);
    void onClientDisconnected();

    // 协议消息统一入口。当前测试阶段只消费 payload["motor_6"] / payload["motor_commands"]；
    // 正式项目应在这里或独立服务类中，将 motor_6 的 pwm_index / angle_deg 交给电机映射模块。
    void onMessageReceived(const QJsonObject &message);
    void onBridgeError(const QString &error);
    void onBridgeStatus(const QString &status);

    void onPythonStarted(const QString &status);
    void onPythonFinished(int exitCode);
    void onPythonCrashed(int exitCode);
    void onPythonStdout(const QString &line);
    void onPythonStderr(const QString &line);
    void onPythonError(const QString &error);

    void updateWatchdog();

private:
    // 以下函数只负责搭建测试 UI。
    // 若把通讯类复制进已有 Qt 主程序，这些 UI 代码不是必需项。
    void buildUi();
    void bindSignals();
    void appendLog(const QString &tag, const QString &text);
    void setServerStatus(const QString &text);
    void setPythonStatus(const QString &text);
    void setLinkStatus(const QString &text);
    void setLastMessage(const QString &text);
    void loadLaunchSettings();
    void saveLaunchSettings() const;
    bool ensureBridgeListening();
    PythonLaunchConfig currentLaunchConfig() const;
    QString defaultProjectRoot() const;
    QString defaultPythonExecutable() const;

    // 当前 pose JSON 的显示辅助函数。
    // 这里刻意不写 RS485/ZMC 指令，电机控制应由正式主项目负责。
    // 其中 formatMotor6* 只用于当前 6 号电机验证阶段；formatPose* 系列仅保留为历史显示工具。
    static QString summarizePayload(const QJsonObject &payload);
    static QString summarizeRotation(const QJsonObject &rotation);
    static QJsonObject selectPrimaryArm(const QJsonArray &angles);
    static QString formatPoseSummary(const QJsonArray &angles);
    static QString formatMotor6Summary(const QJsonObject &command);
    static QString formatMotor6Html(const QJsonObject &command, const QJsonArray &angles);
    static QString formatPoseHtml(const QJsonArray &angles);
    static QString formatArmColumn(const QString &title, const QJsonObject &arm);
    static QString formatJointRow(const QString &label, const QJsonObject &rotation);
    static QString formatWristExperimentSummary(const QJsonObject &arm);
    static QString formatWristExperimentHtml(const QJsonObject &arm);
    static QString formatAxisMotion(const QJsonObject &rotation, const QString &directionKey, const QString &degKey);
    static QString formatDirection(const QJsonObject &rotation, const QString &key);
    static QString formatDeg(const QJsonObject &rotation, const QString &key);

    LocalCommBridge *m_bridge = nullptr;
    PythonProcessController *m_python = nullptr;

    QLineEdit *m_pythonExeEdit = nullptr;
    QLineEdit *m_projectRootEdit = nullptr;
    QLineEdit *m_moduleEdit = nullptr;
    QLineEdit *m_hostEdit = nullptr;
    QSpinBox *m_portSpin = nullptr;
    QLineEdit *m_clientNameEdit = nullptr;
    QCheckBox *m_autoStartCheck = nullptr;
    QPushButton *m_browsePythonButton = nullptr;
    QPushButton *m_browseProjectButton = nullptr;
    QPushButton *m_startButton = nullptr;
    QPushButton *m_stopButton = nullptr;
    QPushButton *m_restartButton = nullptr;
    QPushButton *m_sendButton = nullptr;
    QPushButton *m_clearButton = nullptr;
    QLineEdit *m_commandEdit = nullptr;
    QLabel *m_serverStatusLabel = nullptr;
    QLabel *m_pythonStatusLabel = nullptr;
    QLabel *m_linkStatusLabel = nullptr;
    QLabel *m_lastMessageLabel = nullptr;
    QLabel *m_connectionLabel = nullptr;
    QLabel *m_poseLabel = nullptr;
    QTextEdit *m_poseView = nullptr;
    QPlainTextEdit *m_logView = nullptr;

    QTimer m_watchdog;
    QElapsedTimer m_lastMessageTimer;
};
