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
#include <QLineEdit>
#include <QHash>
#include <QList>
#include <QPair>
#include <QSet>
#include <QPlainTextEdit>
#include <QVariantMap>
#include "src/draw/drawing_window.h"

class RobotController;   // 机器人运动控制器（正逆解、关节/笛卡尔运动）
class UiController;      // UI 逻辑控制器（界面状态与交互逻辑）
class VisionRobotLink;   // 视觉→机械臂联动控制器
class Rs485Controller;   // RS485 通信控制器（直连 ZMC306X PWM 模块）
class ServoController;   // 舵机控制器（舵机驱动与角度控制）
class VisionController;  // 视觉系统控制器（识别与电机指令下发）
class VMControl;           // VisionMaster SDK 静态库控制层
class VmWindow;            // 视觉方案操作窗口（从 CS-VMC-Dome 集成）
class QCheckBox;         // 复选框控件（如六轴使能勾选）
class QDoubleSpinBox;    // 双精度数值输入框（角度/速度等参数）
class ServoTestWidget;   // 舵机设置界面

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
    void onScanClicked();                           // 扫描控制器
    void onConnectClicked();                        // 连接控制器
    void onDisconnectClicked();                     // 断开控制器
    void onHardwareConnected();                     // 硬件连接成功回调
    void onHardwareDisconnected();                  // 硬件断开回调
    void onHardwareConnectionFailed(const QString& errorMsg);  // 连接失败回调
    void onModelInitSuccess();                      // 建模成功回调
    void onModelInitFailed(int errorCode);          // 建模失败回调
    void on_pushButton_clicked();    // 机器人建模（运动学验证→写入控制器）

    // 正逆解模式切换槽函数
    void onCartesianModeClicked();  // 切换到笛卡尔模式
    void onJointModeClicked();      // 切换到关节模式

    void on_pushButton_2_clicked();  // 正解模式（关节空间控制）
    void on_pushButton_3_clicked();  // 逆解模式（笛卡尔空间控制）
    void onHomingClicked();          // 机械回原点（回零→建模→逆解模式）
    void on_pushButton_6_clicked();  // 视觉启停

    // 视觉取放水
    void on_btnGetWater_clicked();//拿水按钮点击事件

    // VisionMaster 验证（CS-VMC-Dome 桥接）
    void onBtnVerifyVision();                     // 执行验证按钮：启动/关闭 CS-VMC-Dome.exe

    // 开发者账号管理
    void onBtnLogin();                           // 登录按钮（开发者账号验证）

    // 位置显示更新
    void updatePositionDisplay();                // 刷新位置显示（定时更新关节/笛卡尔坐标）
    void onSendJointMove();                      // 发送关节运动指令（关节点动）
    void onSendCartMove();                       // 发送笛卡尔运动指令（笛卡尔点动）

    // 运行页面
    void onBtnStop();                            // 急停按钮（立即停止所有运动）
    void onBtnShutdown();                        // 关机按钮（安全停止 + 禁用轴）
    void onRadioModeChanged();                   // 运行模式切换（关节/笛卡尔/视觉等）

    // 编程页面
    void onGetJoints();                          // 获取当前关节角度（写入编程块）
    void onGetCart();                            // 获取当前笛卡尔坐标（写入编程块）

    // 页面导航
    void onNavButtonClicked(int index);          // 导航栏按钮点击（切换功能页面）

    // RS485 测试（ZMC306X 直连 PWM 模块）
    void initRs485UI();                          // 初始化 RS485 调试界面控件
    void onRs485Send();                          // RS485 手动发送数据
    void onRs485ClearSend();                     // 清空发送区记录
    void onRs485ClearRecv();                     // 清空接收区记录
    void onRs485DataReceived(const QByteArray& data);  // RS485 数据接收回调（追加到接收区）    
    // RS485 定时发送
    void onRs485AutoSendToggled(bool checked);   // 定时发送开关切换
    void onRs485AutoSendTick();                  // 定时发送计时器回调

    // PWM 测试按钮
    void onTestPwmDuty();                        // 测试 PWM 占空比输出

    // 视觉系统控制
    void onVisionStarted();                      // 视觉进程启动回调
    void onVisionStopped(int exitCode);          // 视觉进程停止回调（exitCode 为退出码）
    void onVisionError(const QString &error);    // 视觉进程错误回调
    void onVisionMessage(const QString &channel, const QString &line);  // 视觉进程标准输出/错误消息
    void onVisionMotorCommands(const QList<QPair<int, double>>& commands);  // 视觉下发电机指令（axis(0-5)+signed_angle_deg）
    void onAxisEnableToggled(int axis, bool enabled);  // 单轴视觉使能开关切换
    void initVisionControlUI();                  // 初始化视觉控制页面控件
    void initVisionAutoLoad();                     // 程序启动时自动加载视觉方案并连续执行
    void onApplyGlobalSpeedClicked();            // 把总速度广播到 6 个轴的转速输入框

private:
    Ui::MainWindow *ui;
    RobotController* m_robotController;         // 机器人运动控制器（正逆解、关节/笛卡尔运动）
    UiController* m_uiController;               // UI 逻辑控制器（界面状态与交互逻辑）
    Rs485Controller* m_rs485 = nullptr;         // RS485 通信控制器（ZMC306X PWM 控制）
    ServoController* m_servo = nullptr;         // 舵机控制器（舵机驱动与角度控制）
    VisionController* m_vision = nullptr;       // 视觉系统控制器（识别与电机指令下发）
    VmWindow* m_vmWindow = nullptr;              // 视觉方案操作窗口（延迟创建）
    VisionRobotLink* m_visionLink = nullptr;     // 视觉→机械臂联动控制器
    DrawingWindow* m_drawingWindow = nullptr;      // 画板窗口（非模态）
    bool m_homingDone = false;                     // 回零是否已完成（避免重复回零）
    bool m_isShutdown = false;                     // 是否处于关机状态（回到关机位置）
    bool m_visionControlActive = false;             // 视觉跟随是否激活（停止时先置false再杀进程）
    QElapsedTimer m_visionThrottleTimer;
    QHash<int, qint64> m_lastVisionSendMsPerAxis;           // axis → 上次下发时间戳(ms since epoch)
    QHash<int, double> m_lastVisionLoggedAnglePerAxis;      // axis → 上次写入日志的角度
    QHash<int, QList<double>> m_axisValueBuffer;            // axis → 滑动窗口缓冲
    QHash<int, double> m_lastSentAnglePerAxis;              // axis → 上次实际下发给机械臂的角度
    static constexpr int kWindowSize = 10;                   // 滑动窗口大小
    static constexpr double kStdDevThreshold = 0.3;          // 标准差阈值(°)：低于此值认为姿态稳定
    static constexpr double kMinAngleDeadband = 0.5;         // 死区阈值(°)：与上次下发角度差异小于此值不下发
    ServoTestWidget* m_servoTestWidget = nullptr;  // 舵机设置界面
    QList<QLabel*> m_servoAngleLabels;  // 手动页舵机板块 6 通道只读角度显示（被动显示）

    // ----- 视觉运动页面（pageVision）-----
    QCheckBox* m_axisEnableCks[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    QLabel*    m_axisAngleLabels[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    QLabel*    m_axisNameLabels[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    QLineEdit* m_axisSpeedSpins[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    bool       m_axisEnabled[6] = {false, false, false, false, false, false};
    double     m_axisSpeed[6] = {25.0, 25.0, 25.0, 25.0, 25.0, 25.0};
    QLabel*    m_visionStatusLabel = nullptr;
    QPlainTextEdit* m_visionLogView = nullptr;

    QButtonGroup* m_navGroup;
    QTimer* m_jogResetTimerJoint = nullptr;   // 关节点动5s自动复位
    QTimer* m_jogResetTimerCart  = nullptr;   // 笛卡尔点动5s自动复位
    bool m_jogJointUserInput = false;          // 用户正在手动输入关节值
    bool m_jogCartUserInput  = false;          // 用户正在手动输入笛卡尔值
    QPlainTextEdit* m_lastHighlightedEditor = nullptr;  // 单步高亮编辑器

    // RS485 定时发送
    QTimer* m_rs485AutoSendTimer = nullptr;

    void initConnectionUI();                     // 初始化连接页面控件与信号槽
    void initPositionDisplay();                  // 初始化位置显示相关控件
    void initManualProgUI();                     // 初始化手动/编程页面控件
    void updateConnectionUI(bool connected);     // 根据连接状态更新 UI 控件外观
    void setConnectionStatus(const QString& text, const QString& color);  // 设置连接状态文本与颜色
    void appendRs485RecvText(const QString& text);  // 追加文本到 RS485 接收显示区
    QByteArray parseHexString(const QString& hex, bool* ok) const;  // 十六进制字符串转字节数组
    QString bytesToHex(const QByteArray& data) const;               // 字节数组转十六进制字符串
    QString decimalToHex(const QString& decimalStr) const;          // 十进制字符串转十六进制字符串
    static quint16 calcModbusCRC16(const QByteArray& data);         // 计算 Modbus CRC16 校验码
    void showPointAdjustDialog(bool isJoint);    // 弹出点位调整对话框（关节/笛卡尔）

    // 开发者账号管理
    void initLoginUI();                          // 初始化登录界面控件
    void updateLoginUI();                        // 根据登录状态更新界面
    bool checkLoginGate();                       // 检查登录状态（返回 true 表示已登录或弹窗已处理）
    QString getAccountConfigPath() const;        // 获取账号配置文件路径
    bool m_loggedIn = false;                     // 是否已登录开发者账号
};
#endif // MAINWINDOW_H
