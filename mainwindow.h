/*
 * @file mainwindow.h
 * @brief 声明原始主窗口类 MainWindow。
 *
 * 本文件定义 MainWindow 类，作为应用的主界面入口。
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QButtonGroup>
#include <QTimer>
#include <QByteArray>
#include <QSpinBox>

class RobotController;
class UiController;
class Rs485Controller;
class ServoController;
class VisionController;
class QPlainTextEdit;
class ServoTestWidget;  // 舵机测试界面

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onScanClicked();
    void onConnectClicked();
    void onDisconnectClicked();
    void onHardwareConnected();
    void onHardwareDisconnected();
    void onHardwareConnectionFailed(const QString& errorMsg);
    void onModelInitSuccess();
    void onModelInitFailed(int errorCode);

    void on_pushButton_clicked();

    // 正逆解模式切换槽函数
    void onCartesianModeClicked();  // 切换到笛卡尔模式
    void onJointModeClicked();      // 切换到关节模式

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    // 位置显示更新
    void updatePositionDisplay();
    void onSendJointMove();
    void onSendCartMove();

    // 运行页面
    void onBtnStop();
    void onRadioModeChanged();

    // 编程页面
    void onGetJoints();
    void onGetCart();

    // 页面导航
    void onNavButtonClicked(int index);

    // 老化测试
    void onAgingTick();
    void finishAgingTest(bool success, const QString& message);

    // RS485 测试（ZMC306X 直连 PWM 模块）
    void initRs485UI();
    void onRs485Send();
    void onRs485ClearSend();
    void onRs485ClearRecv();
    void onRs485DataReceived(const QByteArray& data);
    void onRs485DataSent(qint64 bytes);
    
    // RS485 定时发送
    void onRs485AutoSendToggled(bool checked);
    void onRs485AutoSendTick();

    // PWM 测试按钮
    void onTestPwmDuty();

    // 视觉系统控制
    void onVisionStarted();
    void onVisionStopped(int exitCode);
    void onVisionError(const QString &error);
    void onVisionMessage(const QString &channel, const QString &line);
    void onVisionMotorCommand(double angle);

private:
    Ui::MainWindow *ui;
    RobotController* m_robotController;
    UiController* m_uiController;
    Rs485Controller* m_rs485 = nullptr;
    ServoController* m_servo = nullptr;
    VisionController* m_vision = nullptr;
    QElapsedTimer m_visionThrottleTimer;
    double m_lastVisionAngle = 0;
    int m_visionConsecutiveCount = 0;
    static constexpr int kVisionConfirmCount = 10;
    ServoTestWidget* m_servoTestWidget = nullptr;  // 舵机测试界面
    QList<QSpinBox*> m_servoSpins;   // 6通道舵机角度 spinbox
    QList<QDoubleSpinBox*> m_servoSpeedSpins;  // 6通道舵机速度 spinbox (度/秒)
    QList<QLabel*> m_servoAngleLabels;  // 手动页舵机板块 6 通道只读角度显示（被动显示）

    QButtonGroup* m_navGroup;
    QTimer* m_jogResetTimerJoint = nullptr;   // 关节点动5s自动复位
    QTimer* m_jogResetTimerCart  = nullptr;   // 笛卡尔点动5s自动复位
    bool m_jogJointUserInput = false;          // 用户正在手动输入关节值
    bool m_jogCartUserInput  = false;          // 用户正在手动输入笛卡尔值
    QPlainTextEdit* m_lastHighlightedEditor = nullptr;  // 单步高亮编辑器

    // RS485 定时发送
    QTimer* m_rs485AutoSendTimer = nullptr;

    // 老化测试状态
    QTimer* m_agingTimer = nullptr;
    int m_agingCycle = 0;
    int m_agingTargetIdx = 0;
    enum class AgingStep { MoveJ, MoveL };
    AgingStep m_agingStep = AgingStep::MoveJ;
    bool m_agingSimMode = false;  // 检测到模拟器（轴不动）后启用快速模式
    QList<QList<double>> m_agingJointTargets;

    void initConnectionUI();
    void initPositionDisplay();
    void initManualProgUI();
    void updateConnectionUI(bool connected);
    void setConnectionStatus(const QString& text, const QString& color);
    void validateKinematicsModel();
    void appendRs485RecvText(const QString& text);
    QByteArray parseHexString(const QString& hex, bool* ok) const;
    QString bytesToHex(const QByteArray& data) const;
    QString decimalToHex(const QString& decimalStr) const;
    static quint16 calcModbusCRC16(const QByteArray& data);
    void showPointAdjustDialog(bool isJoint);
};
#endif // MAINWINDOW_H
