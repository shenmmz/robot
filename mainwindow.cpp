/*
 * @file mainwindow.cpp
 * @brief 实现原始主窗口类 MainWindow 的构造和析构。
 *
 * 本文件负责：
 * - 初始化 UI 界面
 * - 控制器连接管理（扫描、连接、断开）
 * - 验证建模功能
 */
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "ui_PointAdjustDialog.h"
#include "src/logic/RobotController.h"
#include "src/logic/UiController.h"
#include "src/hardware/Rs485Controller.h"
#include "src/hardware/ServoController.h"
#include "src/vision/VisionController.h"
#include "src/models/RobotConfig.h"
#include "src/models/RobotKinematics.h"
#include "library/zmotion.h"
#include "src/utils/MathUtils.h"
#include "src/utils/CodeEditor.h"
#include "library/zauxdll2.h"
#include <QElapsedTimer>
#include <QThread>
#include <QTimer>
#include <QLineEdit>
#include <functional>
#include "ZmotionAPI/ZmotionAPI.h"
#include "src/hardware/ServoController.h"
#include "src/ui/ServoTestWidget.h"
#include <QDebug>
#include <QCoreApplication>
#include <QMessageBox>
#include <QCheckBox>
#include <QDialog>
#include <QScrollBar>
#include <QTextCursor>
#include <QDoubleSpinBox>
#include <QSerialPort>
#include <QTabWidget>
#include <QGridLayout>
#include <QSplitter>
#include <QStackedWidget>
#include <QTextEdit>
#include <QTextDocument>
#include <QTextBlock>
#include <memory>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QThread>
#include <QApplication>

static QString findConfigFile()
{
    // 从 exe 所在目录向上查找 config/robot_config.json
    QDir dir(QCoreApplication::applicationDirPath());
    while (!dir.isRoot()) {
        QString candidate = dir.absolutePath() + "/config/robot_config.json";
        if (QFileInfo::exists(candidate)) {
            qDebug() << "[MainWindow] 找到配置文件:" << candidate;
            return candidate;
        }
        if (!dir.cdUp()) break;
    }
    qWarning() << "[MainWindow] 未找到配置文件，使用默认路径";
    return "config/robot_config.json";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_robotController(nullptr)
{
    ui->setupUi(this);

    // 初始化机器人控制器（从JSON配置文件加载机械参数）
    m_robotController = new RobotController(this);
    m_robotController->initialize(findConfigFile());

    // 初始化 UI 辅助控制器（IO监控/报警日志/编程/设置）
    m_uiController = new UiController(this, m_robotController, this);
    m_uiController->init();

    // 点动完成时立即刷新位置标签
    connect(m_uiController, &UiController::jogCompleted,
            this, &MainWindow::updatePositionDisplay);

    // 初始化连接面板
    initConnectionUI();

    // 初始化位置显示面板
    initPositionDisplay();

    // 初始化导航栏
    m_navGroup = new QButtonGroup(this);
    m_navGroup->setExclusive(true);
    m_navGroup->addButton(ui->btnNavRun, 0);
    m_navGroup->addButton(ui->btnNavManual, 1);
    m_navGroup->addButton(ui->btnNavIO, 2);
    m_navGroup->addButton(ui->btnNavAlarm, 3);
    m_navGroup->addButton(ui->btnNavSettings, 4);
    m_navGroup->addButton(ui->btnNavRs485, 5);

    // 默认显示"运行"页 (UI文件设置的是index=6，需纠正)
    ui->contentStack->setCurrentIndex(0);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(m_navGroup, &QButtonGroup::idClicked, this, &MainWindow::onNavButtonClicked);
#else
    connect(m_navGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            this, &MainWindow::onNavButtonClicked);
#endif

    // 初始化 RS485 测试页
    m_rs485 = new Rs485Controller(this);
    initRs485UI();

    // 重构手动编程页布局（含舵机控制）
    initManualProgUI();

    // --- 视觉系统：管理外部 VisualRoboticArmQtController.exe ---
    m_vision = new VisionController(this);
    connect(m_vision, &VisionController::started,        this, &MainWindow::onVisionStarted);
    connect(m_vision, &VisionController::stopped,        this, &MainWindow::onVisionStopped);
    connect(m_vision, &VisionController::errorOccurred,  this, &MainWindow::onVisionError);
    connect(m_vision, &VisionController::outputMessage,  this, &MainWindow::onVisionMessage);
    connect(m_vision, &VisionController::motorCommand,   this, &MainWindow::onVisionMotorCommand);

    // 切换到手动编程页时强制刷新布局
    connect(ui->contentStack, &QStackedWidget::currentChanged, this, [this](int idx) {
        if (idx == 1) {  // 手动 & 编程页
            ui->pageManual->updateGeometry();
            ui->pageManualLayout->activate();
        }
    });
}

/**
 * @brief 主窗口析构函数
 *
 * 停止老化测试定时器，释放 UI 资源。
 */
MainWindow::~MainWindow()
{
    delete ui;
}

// ==================== UI 初始化 ====================

/**
 * @brief 初始化连接面板的 UI 状态和信号槽连接
 *
 * 设置初始断开状态，预填虚拟IP，注册按钮点击信号
 * 和 RobotController 的连接状态信号。
 */
void MainWindow::initConnectionUI()
{
    // 初始状态：断开，预填虚拟IP
    ui->btnDisconnect->setEnabled(false);
    ui->comboIpList->addItem(QStringLiteral("127.0.0.1"));
    setConnectionStatus(QStringLiteral("未连接"), QStringLiteral("red"));

    // 按钮点击信号
    connect(ui->btnScan, &QPushButton::clicked,this, &MainWindow::onScanClicked);
    connect(ui->btnConnect, &QPushButton::clicked,this, &MainWindow::onConnectClicked);
    connect(ui->btnDisconnect, &QPushButton::clicked,this, &MainWindow::onDisconnectClicked);

    // RobotController 信号
    connect(m_robotController, &RobotController::hardwareConnected,this, &MainWindow::onHardwareConnected);
    connect(m_robotController, &RobotController::hardwareDisconnected,this, &MainWindow::onHardwareDisconnected);
    connect(m_robotController, &RobotController::hardwareConnectionFailed,this, &MainWindow::onHardwareConnectionFailed);
    connect(m_robotController, &RobotController::modelInitSuccess,this, &MainWindow::onModelInitSuccess);
    connect(m_robotController, &RobotController::modelInitFailed,this, &MainWindow::onModelInitFailed);
    connect(m_robotController, &RobotController::robotStatusChanged,
            this, &MainWindow::updatePositionDisplay);
}

/**
 * @brief 根据连接状态更新 UI 控件的可用性
 *
 * 连接时禁用扫描、IP选择、连接按钮，启用断开按钮；
 * 断开时则相反。
 *
 * @param connected 当前连接状态，true 表示已连接，false 表示未连接
 */
void MainWindow::updateConnectionUI(bool connected)
{
    ui->btnScan->setEnabled(!connected); // 连接时禁用扫描按钮
    ui->comboIpList->setEnabled(!connected); // 连接时禁用IP选择
    ui->btnConnect->setEnabled(!connected); // 连接时禁用连接按钮
    ui->btnDisconnect->setEnabled(connected); // 连接时启用断开按钮
}

/**
 * @brief 设置连接状态显示
 *
 * 更新状态标签文本和颜色，同时更新状态栏消息。
 *
 * @param text  状态文本
 * @param color 颜色名称（"red"/"green"/"orange"）
 */
void MainWindow::setConnectionStatus(const QString& text, const QString& color)
{
    ui->labelStatus->setText(text);
    ui->labelStatus->setStyleSheet(
        QStringLiteral("color: %1;").arg(color));
    ui->statusIndicator->setStyleSheet(
        QStringLiteral("background: %1; border-radius: 6px; min-width: 12px; min-height: 12px; max-width: 12px; max-height: 12px;").arg(color));
    ui->statusbar->showMessage(text);
}

// ==================== 按钮槽函数 ====================

/**
 * @brief 扫描可用控制器
 *
 * 调用 RobotController 搜索局域网内的控制器IP，
 * 扫描不到物理控制器时自动添加虚拟IP作为默认选项。
 */
void MainWindow::onScanClicked()
{
    // 禁用扫描按钮，防止重复点击
    ui->btnScan->setEnabled(false);
    // 更新状态为扫描中
    setConnectionStatus(QStringLiteral("扫描中..."), QStringLiteral("orange"));

    // 搜索网络中的控制器
    QStringList ipList = m_robotController->searchControllers();

    // 清空IP列表下拉框
    ui->comboIpList->clear();
    // 127.0.0.1永远保留在列表中
    ui->comboIpList->addItem(QStringLiteral("127.0.0.1"));
    if (ipList.isEmpty()) {
        setConnectionStatus(QStringLiteral("未找到物理控制器，已添加虚拟IP"), QStringLiteral("orange"));
    } else {
        // 添加扫描到的IP地址到下拉框
        ui->comboIpList->addItems(ipList);
        setConnectionStatus(
            QStringLiteral("找到 %1 个控制器").arg(ipList.size()), QStringLiteral("green"));
    }

    // 重新启用扫描按钮
    ui->btnScan->setEnabled(true);
}

/**
 * @brief 连接选中的控制器
 *
 * 获取用户选择的IP地址，调用 RobotController 建立连接，
 * 连接结果通过异步信号返回。
 */
void MainWindow::onConnectClicked()
{
    QString ip = ui->comboIpList->currentText().trimmed();
    // 去除可能的后缀说明文字，只保留IP地址
    ip = ip.split(QStringLiteral(" ")).first();
    if (ip.isEmpty()) {
        setConnectionStatus(QStringLiteral("请输入或选择 IP 地址"), QStringLiteral("orange"));
        return;
    }

    ui->btnConnect->setEnabled(false);
    ui->btnScan->setEnabled(false);
    setConnectionStatus(QStringLiteral("正在连接 %1...").arg(ip), QStringLiteral("orange"));

    m_robotController->connectHardware(ip);
}

/**
 * @brief 断开当前连接
 *
 * 调用 RobotController 断开硬件连接。
 */
void MainWindow::onDisconnectClicked()
{
    setConnectionStatus(QStringLiteral("正在断开..."), QStringLiteral("orange"));
    m_robotController->disconnectHardware();
}

// ==================== 连接状态槽函数 ====================

/**
 * @brief 连接成功回调
 *
 * 更新状态为"已连接"，启用断开按钮并禁用其他操作按钮。
 */
void MainWindow::onHardwareConnected()
{
    setConnectionStatus(QStringLiteral("已连接"), QStringLiteral("green"));
    updateConnectionUI(true);

    // TODO: 暂时注释掉舵机控制器，专注于 RS485 PWM 模块通信
    // // 初始化舵机控制器（使用 IP 连接的 ZMC 句柄）
    // ZMC_HANDLE ethHandle = m_robotController->getMotionController()->getZmotionAPI()->getHandle();
    // if (!m_servo) {
    //     m_servo = new ServoController(ethHandle, 6, this);
    //     m_servo->initAll();
    // } else {
    //     m_servo->setHandle(ethHandle);
    //     m_servo->initAll();
    // }

    // 自动配置 RS485 串口（通道1）为 Modbus主端模式，用于 PWM 模块通信
    // SETCOM(波特率,数据位,停止位,校验,端口,模式,变量映射,超时ms)
    //   mode=14: Modbus主端(16位整数)
    //   variable=2: 系统MODBUS寄存器
    //   timeout=800ms: ZMC手册建议值（默认仅8ms）
    ZmotionAPI* api = m_robotController->getMotionController()->getZmotionAPI();
    if (api) {
        ZmotionResult result = api->executeCommand(
            QStringLiteral("SETCOM(9600,8,1,0,1,14,2,800)"));
        if (result.success) {
            qInfo() << "[RS485] 串口(通道1)已配置: 9600,8,N,1 Modbus主端, 超时800ms";
        } else {
            qWarning() << "[RS485] 串口配置失败:" << result.errorMessage;
        }
    }

    // 重连后刷新舵机角度显示：读取 ServoController 中已保存的当前角度，避免显示默认90°
    ServoController* servo = m_robotController->getServoController();
    if (servo) {
        for (int i = 0; i < 6 && i < m_servoAngleLabels.size(); ++i) {
            float angle = servo->getCurrentAngle(i);
            m_servoAngleLabels[i]->setText(
                QString("CH%1 : %2°").arg(i).arg(angle, 0, 'f', 1));
        }
    }
}

/**
 * @brief 断开连接回调
 *
 * 更新状态为"未连接"，恢复所有操作按钮的可用性。
 */
void MainWindow::onHardwareDisconnected()
{
    setConnectionStatus(QStringLiteral("未连接"), QStringLiteral("red"));
    updateConnectionUI(false);
}

/**
 * @brief 连接失败回调
 *
 * 显示错误信息，恢复按钮状态。
 *
 * @param errorMsg 错误描述信息
 */
void MainWindow::onHardwareConnectionFailed(const QString& errorMsg)
{
    setConnectionStatus(QStringLiteral("连接失败: %1").arg(errorMsg), QStringLiteral("red"));
    updateConnectionUI(false);
}

// ==================== 建模验证（调试用） ====================

/**
 * @brief 验证机器人运动学建模功能
 *
 * 测试 RobotKinematics 类的各项功能：
 * - 创建运动学对象
 * - 设置脉冲当量
 * - 生成控制器DH参数表
 * - 解析控制器参数表
 */
void MainWindow::validateKinematicsModel()
{
    qDebug() << QStringLiteral("=== 开始验证机器人建模功能 ===");

    // 1. 创建运动学对象（使用默认构造函数）
    RobotKinematics kinematics;
    qDebug() << QStringLiteral("1. 创建运动学对象成功");

    // 2. 获取默认脉冲当量（从构造函数初始化的值）
    QList<double> initialUnits = kinematics.getAxisUnits();
    qDebug() << QStringLiteral("2. 构造函数默认脉冲当量:") << initialUnits;

    // 3. 重新设置相同的默认脉冲当量（验证 setAxisUnits 功能）
    QList<double> testUnits = {555.56, 333.33, 250.00, 138.89, 83.33, 27.78};
    kinematics.setAxisUnits(testUnits);
    qDebug() << QStringLiteral("3. 设置测试脉冲当量:") << testUnits;

    // 4. 生成控制器DH参数表
    QVector<float> dhTable = kinematics.toControllerTable();
    qDebug() << QStringLiteral("4. 生成DH参数表成功，表长度:") << dhTable.size();

    // 5. 输出关键DH参数
    qDebug() << QStringLiteral("   LargeZ =") << dhTable[0] << QStringLiteral("mm");
    qDebug() << QStringLiteral("   L1 =") << dhTable[1] << QStringLiteral("mm");
    qDebug() << QStringLiteral("   L2 =") << dhTable[2] << QStringLiteral("mm");
    qDebug() << QStringLiteral("   L3 =") << dhTable[3] << QStringLiteral("mm");
    qDebug() << QStringLiteral("   L4 =") << dhTable[4] << QStringLiteral("mm");
    qDebug() << QStringLiteral("   SmallLZ =") << dhTable[15] << QStringLiteral("mm");

    // 6. 测试从控制器表解析脉冲当量
    kinematics.loadFromControllerTable(dhTable);
    QList<double> parsedUnits = kinematics.getAxisUnits();
    qDebug() << QStringLiteral("5. 从DH表解析脉冲当量:") << parsedUnits;

    // 7. 测试无效输入（边界条件）
    qDebug() << QStringLiteral("6. 测试无效输入...");
    QList<double> invalidUnits = {1.0, 2.0, 3.0};  // 不足6个
    kinematics.setAxisUnits(invalidUnits);  // 应该输出警告

    QVector<float> shortTable(10, 0.0f);  // 不足25项
    kinematics.loadFromControllerTable(shortTable);  // 应该输出警告

    qDebug() << QStringLiteral("=== 机器人建模功能验证完成 ===");
}

/**
 * @brief 建模验证按钮点击处理
 *
 * 机器人建模流程：
 * 1. 执行运动学模型功能验证（输出到调试控制台）
 * 2. 将机器人模型写入控制器（DH参数、脉冲当量等）
 * 注意：建模后需要手动点击正解/逆解按钮切换控制模式
 */
void MainWindow::on_pushButton_clicked()
{
    // 1. 执行运动学模型功能验证（仅输出到调试控制台）
    validateKinematicsModel();
    
    // 2. 初始化机器人模型（写入控制器）
    if (m_robotController && m_robotController->isConnected()) {
        bool modelSuccess = m_robotController->initRobotModel();
        
        if (modelSuccess) {
            setConnectionStatus(QStringLiteral("建模完成"), QStringLiteral("green"));
            ui->statusbar->showMessage(QStringLiteral("机器人建模完成，请选择控制模式"));
        } else {
            setConnectionStatus(QStringLiteral("建模失败"), QStringLiteral("red"));
            ui->statusbar->showMessage(QStringLiteral("机器人建模失败"));
        }
    } else {
        setConnectionStatus(QStringLiteral("未连接控制器"), QStringLiteral("red"));
        ui->statusbar->showMessage(QStringLiteral("请先连接控制器再进行建模"));
    }
}

/**
 * @brief 模型初始化成功回调
 *
 * 更新状态显示"已连接 - 模型初始化成功"。
 */
void MainWindow::onModelInitSuccess()
{
    setConnectionStatus(QStringLiteral("已连接 - 模型初始化成功"), QStringLiteral("green"));
}

/**
 * @brief 模型初始化失败回调
 *
 * 显示错误码和提示信息。
 *
 * @param errorCode 错误码
 */
void MainWindow::onModelInitFailed(int errorCode)
{
    setConnectionStatus(
        QStringLiteral("模型初始化失败，错误码: %1").arg(errorCode), QStringLiteral("red"));
    ui->statusbar->showMessage(QStringLiteral("模型初始化失败！请检查控制器连接和参数"));
}

/**
 * @brief 切换到笛卡尔模式（逆解模式）
 *
 * 用户点击"笛卡尔模式"按钮时触发，调用 RobotController 的 switchToCartesianMode() 方法。
 */
void MainWindow::onCartesianModeClicked()
{
    if (!m_robotController) {
        ui->statusbar->showMessage(QStringLiteral("机器人控制器未初始化"));
        return;
    }

    bool success = m_robotController->switchToCartesianMode();
    if (success) {
        ui->statusbar->showMessage(QStringLiteral("已切换到笛卡尔空间控制模式"));
        qDebug() << "[MainWindow] 已切换到笛卡尔模式";
    } else {
        ui->statusbar->showMessage(QStringLiteral("切换笛卡尔模式失败"));
        qDebug() << "[MainWindow] 切换笛卡尔模式失败";
    }
    updatePositionDisplay();
}

/**
 * @brief 切换到关节模式（正解模式）
 *
 * 用户点击"关节模式"按钮时触发，调用 RobotController 的 switchToJointMode() 方法。
 */
void MainWindow::onJointModeClicked()
{
    if (!m_robotController) {
        ui->statusbar->showMessage(QStringLiteral("机器人控制器未初始化"));
        return;
    }

    bool success = m_robotController->switchToJointMode();
    if (success) {
        ui->statusbar->showMessage(QStringLiteral("已切换到关节空间控制模式"));
        qDebug() << "[MainWindow] 已切换到关节模式";
    } else {
        ui->statusbar->showMessage(QStringLiteral("切换关节模式失败"));
        qDebug() << "[MainWindow] 切换关节模式失败";
    }
    updatePositionDisplay();
}


/**
 * @brief 正解模式按钮点击处理
 * 
 * 切换到正解模式（关节空间控制）：
 * - 使用 CONNREFRAME 建立正运动学映射
 * - 控制器根据关节角度计算笛卡尔坐标
 * - 用户通过关节角度控制机器人
 */
void MainWindow::on_pushButton_2_clicked()
{
    if (m_robotController && m_robotController->isConnected()) {
        bool success = m_robotController->switchToJointMode();
        
        if (success) {
            setConnectionStatus(QStringLiteral("正解模式"), QStringLiteral("green"));
            ui->statusbar->showMessage(QStringLiteral("已切换到正解模式（关节空间控制）"));
            ui->radioJoint->blockSignals(true);
            ui->radioJoint->setChecked(true);
            ui->radioJoint->blockSignals(false);
        } else {
            setConnectionStatus(QStringLiteral("正解模式切换失败"), QStringLiteral("orange"));
            ui->statusbar->showMessage(QStringLiteral("正解模式切换失败"));
        }
        updatePositionDisplay();
    } else {
        setConnectionStatus(QStringLiteral("未连接控制器"), QStringLiteral("red"));
        ui->statusbar->showMessage(QStringLiteral("请先连接控制器"));
    }
}


/**
 * @brief 逆解模式按钮点击处理
 * 
 * 切换到逆解模式（笛卡尔空间控制）：
 * - 使用 CONNFRAME 建立逆运动学映射
 * - 控制器根据笛卡尔坐标计算关节角度
 * - 用户通过笛卡尔坐标控制机器人
 */
void MainWindow::on_pushButton_3_clicked()
{
    if (m_robotController && m_robotController->isConnected()) {
        bool success = m_robotController->switchToCartesianMode();
        
        if (success) {
            setConnectionStatus(QStringLiteral("逆解模式"), QStringLiteral("green"));
            ui->statusbar->showMessage(QStringLiteral("已切换到逆解模式（笛卡尔空间控制）"));
            ui->radioCart->blockSignals(true);
            ui->radioCart->setChecked(true);
            ui->radioCart->blockSignals(false);
        } else {
            setConnectionStatus(QStringLiteral("逆解模式切换失败"), QStringLiteral("orange"));
            ui->statusbar->showMessage(QStringLiteral("逆解模式切换失败"));
        }
        updatePositionDisplay();
    } else {
        setConnectionStatus(QStringLiteral("未连接控制器"), QStringLiteral("red"));
        ui->statusbar->showMessage(QStringLiteral("请先连接控制器"));
    }
}


/**
 * @brief 一键自动初始化按钮：回零 → 建模 → 逆解模式
 *
 * 流程：
 * 1. 启动回零（异步，等待所有轴找原点 + 移动到homePosition）
 * 2. 回零完成后自动建模（写入DH参数、CONNFRAME等）
 * 3. 建模完成后自动切换到逆解模式（笛卡尔空间控制）
 *
 * 异步操作通过Qt信号链串联，状态更新显示在状态栏。
 */
void MainWindow::on_pushButton_4_clicked()
{
    if (!m_robotController) {
        ui->statusbar->showMessage(QStringLiteral("机器人控制器未初始化"));
        return;
    }

    if (!m_robotController->isConnected()) {
        setConnectionStatus(QStringLiteral("未连接控制器"), QStringLiteral("red"));
        ui->statusbar->showMessage(QStringLiteral("请先连接控制器"));
        return;
    }

    // 确认对话框
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        QStringLiteral("确认执行"),
        QStringLiteral("将执行以下操作：\n1. 回零（找原点 → 移动到初始位置）\n2. 初始化机器人模型\n3. 清零关节坐标\n4. 切换逆解模式\n\n是否继续？"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );
    if (reply != QMessageBox::Yes) {
        return;
    }

    // 禁用按钮，防止重复点击
    ui->btnScan->setEnabled(false);
    ui->btnConnect->setEnabled(false);
    ui->btnDisconnect->setEnabled(false);

    // 第一步：启动回零
    setConnectionStatus(QStringLiteral("回零中..."), QStringLiteral("orange"));
    ui->statusbar->showMessage(QStringLiteral("正在执行回零，请等待..."));

    bool success = m_robotController->executeHoming();
    if (!success) {
        setConnectionStatus(QStringLiteral("回零启动失败"), QStringLiteral("red"));
        ui->statusbar->showMessage(QStringLiteral("回零操作启动失败"));
        updateConnectionUI(false);
        return;
    }

    // 回零完成后的链式回调（回零是异步的，建模和逆解是同步的）
    auto conn1 = std::make_shared<QMetaObject::Connection>();
    *conn1 = connect(m_robotController, &RobotController::operationCompleted, this,
        [this, conn1](const QString& op) {
            if (op != QStringLiteral("回零")) return;
            disconnect(*conn1);

            // 第二步：建模
            setConnectionStatus(QStringLiteral("建模中..."), QStringLiteral("orange"));
            ui->statusbar->showMessage(QStringLiteral("回零完成，正在初始化机器人模型..."));

            bool modelOk = m_robotController->initRobotModel();
            if (!modelOk) {
                setConnectionStatus(QStringLiteral("建模失败"), QStringLiteral("red"));
                ui->statusbar->showMessage(QStringLiteral("机器人模型初始化失败"));
                updateConnectionUI(true);
                return;
            }

            // 第三步：把所有关节轴DPOS清零
            setConnectionStatus(QStringLiteral("清零关节坐标..."), QStringLiteral("orange"));
            ui->statusbar->showMessage(QStringLiteral("建模完成，正在清零关节坐标..."));

            ZmotionAPI* api = m_robotController->getMotionController()->getZmotionAPI();
            api->setAxisDpos({0, 1, 2, 3, 4, 5}, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
            qDebug() << "[MainWindow] 关节轴0-5 DPOS 已清零";

            // 第四步：切换正解模式
            setConnectionStatus(QStringLiteral("切换正解..."), QStringLiteral("orange"));
            ui->statusbar->showMessage(QStringLiteral("关节坐标已清零，正在切换正解模式..."));

            bool ok = m_robotController->switchToJointMode();

            if (ok) {
                // 切一次逆解，让 CONNFRAME LOAD 同步虚拟轴 DPOS，再回正解
                m_robotController->switchToCartesianMode();
                m_robotController->switchToJointMode();
                setConnectionStatus(QStringLiteral("已就绪(正解)"), QStringLiteral("green"));
                ui->statusbar->showMessage(QStringLiteral("回零+建模+清零+正解完成，机械臂已就绪"));
            } else {
                setConnectionStatus(QStringLiteral("正解切换失败"), QStringLiteral("red"));
                ui->statusbar->showMessage(QStringLiteral("正解模式切换失败"));
            }
            updateConnectionUI(true);
            ZmotionAPI::setVerbose(true);  // 链结束，恢复日志
        });

    // 回零失败的处理
    auto conn2 = std::make_shared<QMetaObject::Connection>();
    *conn2 = connect(m_robotController, &RobotController::operationFailed, this,
        [this, conn1, conn2](const QString& op, const QString& errorMsg) {
            if (op != QStringLiteral("回零")) return;
            disconnect(*conn1);
            disconnect(*conn2);

            setConnectionStatus(QStringLiteral("回零失败"), QStringLiteral("red"));
            ui->statusbar->showMessage(QStringLiteral("回零失败: %1").arg(errorMsg));
            updateConnectionUI(false);
            ZmotionAPI::setVerbose(true);  // 失败也恢复日志
        });
}


// 老化测试已彻底删除(2026-06-30)


/**
 * @brief 功能测试：切换视觉程序启停
 */
void MainWindow::on_pushButton_6_clicked()
{
    if (!m_vision) return;

    if (m_vision->isRunning()) {
        m_vision->stop();
    } else {
        m_vision->start();
    }
}

/**
 * @brief 停止按钮：暂停/停止任务
 *
 * 调用 UiController 的暂停逻辑，若任务已空闲则硬件急停。
 * 同时结束老化测试（如果正在进行）。
 */
void MainWindow::onBtnStop()
{
    // 统一暂停逻辑：正在执行→暂停，已暂停→停止
    m_uiController->pauseOrResumeTask();

    if (!m_robotController || !m_robotController->isConnected()) return;

    // 任务完全空闲时才硬件急停
    if (!m_uiController->isTaskRunning()) {
        m_robotController->stopMotion();
    }

    ui->statusbar->showMessage(QStringLiteral("已发送停止指令"));
}

/**
 * @brief 正解/逆解模式切换回调
 *
 * 根据 radioCart/radioJoint 状态切换控制模式并刷新位置显示。
 */
void MainWindow::onRadioModeChanged()
{
    if (!m_robotController) return;

    bool success;
    if (ui->radioCart->isChecked()) {
        success = m_robotController->switchToCartesianMode();
        if (success) {
            ui->statusbar->showMessage(QStringLiteral("已切换到笛卡尔空间控制模式"));
        }
    } else {
        success = m_robotController->switchToJointMode();
        if (success) {
            ui->statusbar->showMessage(QStringLiteral("已切换到关节空间控制模式"));
        }
    }
    updatePositionDisplay();
}

void MainWindow::initPositionDisplay()
{
    // btnApplyJoint / btnApplyCart 连接已移至 initManualProgUI 的 setupJogButtons 中

    // 运行页面：停止按钮
    connect(ui->btnStop, &QPushButton::clicked, this, &MainWindow::onBtnStop);

    // 运行页面：模式切换单选按钮
    connect(ui->radioJoint, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) onRadioModeChanged();
    });
    connect(ui->radioCart, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) onRadioModeChanged();
    });
}

/**
 * @brief 刷新位置显示面板
 *
 * 更新关节坐标和笛卡尔坐标标签/输入框数值，
 * 并根据当前模式高亮对应组（关节蓝色 / 笛卡尔蓝色）。
 */
void MainWindow::updatePositionDisplay()
{
    if (!m_robotController || !m_robotController->getState()) return;

    RobotState* state  = m_robotController->getState();
    QList<double> joints = state->getAllAxisPositions();
    QList<double> carts  = state->getAllCartesianPositions();

    QLabel* jLabels[]  = {ui->lblPosJ0, ui->lblPosJ1, ui->lblPosJ2, ui->lblPosJ3, ui->lblPosJ4, ui->lblPosJ5};
    QLabel* cLabels[]  = {ui->lblPosCartX, ui->lblPosCartY, ui->lblPosCartZ, ui->lblPosCartRx, ui->lblPosCartRy, ui->lblPosCartRz};
    QLineEdit* jSpins[] = {ui->spinManualJ0, ui->spinManualJ1, ui->spinManualJ2, ui->spinManualJ3, ui->spinManualJ4, ui->spinManualJ5};
    QLineEdit* cSpins[] = {ui->spinManualX, ui->spinManualY, ui->spinManualZ, ui->spinManualRx, ui->spinManualRy, ui->spinManualRz};

    bool isCart = m_robotController->isCartesianMode();

    QString jStyle = isCart
        ? QStringLiteral("font-family:Consolas,'Microsoft YaHei UI';font-size:14px;padding:3px 8px;color:#999;background:#f5f5f5;border:1px solid #e8e8e8;border-radius:4px;min-width:90px;qproperty-alignment:'AlignLeft | AlignVCenter';")
        : QStringLiteral("font-family:Consolas,'Microsoft YaHei UI';font-size:14px;padding:3px 8px;color:#333;background:#f5f5f5;border:1px solid #1565c0;border-radius:4px;min-width:90px;qproperty-alignment:'AlignLeft | AlignVCenter';");
    QString cStyle = isCart
        ? QStringLiteral("font-family:Consolas,'Microsoft YaHei UI';font-size:14px;padding:3px 8px;color:#0078d4;background:#f0f4ff;border:1px solid #1565c0;border-radius:4px;min-width:90px;qproperty-alignment:'AlignLeft | AlignVCenter';")
        : QStringLiteral("font-family:Consolas,'Microsoft YaHei UI';font-size:14px;padding:3px 8px;color:#999;background:#f5f5f5;border:1px solid #e8e8e8;border-radius:4px;min-width:90px;qproperty-alignment:'AlignLeft | AlignVCenter';");

    // 始终实时刷新，用户正在手动输入时暂停刷新该组
    for (int i = 0; i < 6 && i < joints.size(); ++i) {
        jLabels[i]->setText(QString::number(joints[i], 'f', 3));
        jLabels[i]->setStyleSheet(jStyle);

        if (!m_jogJointUserInput) {
            jSpins[i]->blockSignals(true);
            jSpins[i]->setText(QString::number(joints[i], 'f', 3));
            jSpins[i]->blockSignals(false);
        }

        if (i < carts.size()) {
            double display = carts[i];
            if (i >= 3) display = normalizeAngle180(display);
            cLabels[i]->setText(QString::number(display, 'f', 3));
            cLabels[i]->setStyleSheet(cStyle);

            if (!m_jogCartUserInput) {
                cSpins[i]->blockSignals(true);
                cSpins[i]->setText(QString::number(display, 'f', 3));
                cSpins[i]->blockSignals(false);
            }
        }
    }

    ui->labelModeIndicator->setText(isCart
        ? QStringLiteral("当前: 笛卡尔")
        : QStringLiteral("当前: 关节"));

    // 更新运动参数显示
    if (state && !state->getAllAxisVelocities().isEmpty()) {
        double speed = state->getAxisVelocity(0);
        double accel = state->getAxisAccel(0);
        ui->lblRunSpeed->setText(QString::number(speed, 'f', 1) + QStringLiteral(" mm/s"));
        ui->lblRunAccel->setText(QString::number(accel, 'f', 1) + QStringLiteral(" mm/s²"));
        ui->lblRunDecel->setText(QString::number(accel, 'f', 1) + QStringLiteral(" mm/s²"));
    }
}

/**
 * @brief 发送关节空间运动指令
 *
 * 读取手动调试面板的关节角度值，执行 MoveJ。
 */
void MainWindow::onSendJointMove()
{
    if (!m_robotController || !m_robotController->isConnected()) {
        ui->statusbar->showMessage(QStringLiteral("请先连接控制器"));
        return;
    }

    QList<double> joints = {
        ui->spinManualJ0->text().toDouble(), ui->spinManualJ1->text().toDouble(), ui->spinManualJ2->text().toDouble(),
        ui->spinManualJ3->text().toDouble(), ui->spinManualJ4->text().toDouble(), ui->spinManualJ5->text().toDouble()
    };

    bool ok = m_robotController->moveJ(joints, 25.0, 50.0, 60.0, 3000);
    if (ok) {
        ui->statusbar->showMessage(QStringLiteral("关节运动已发送"));
    } else {
        ui->statusbar->showMessage(QStringLiteral("关节运动失败:请检查连接、目标点可达性、是否处于报警状态"));
    }
}

/**
 * @brief 发送笛卡尔空间运动指令
 *
 * 读取手动调试面板的笛卡尔坐标值，执行 MoveL。
 */
void MainWindow::onSendCartMove()
{
    if (!m_robotController || !m_robotController->isConnected()) {
        ui->statusbar->showMessage(QStringLiteral("请先连接控制器"));
        return;
    }

    QList<double> cart = {
        ui->spinManualX->text().toDouble(), ui->spinManualY->text().toDouble(), ui->spinManualZ->text().toDouble(),
        ui->spinManualRx->text().toDouble(), ui->spinManualRy->text().toDouble(), ui->spinManualRz->text().toDouble()
    };

    bool ok = m_robotController->moveL(cart, 25.0, 50.0, 60.0, 3000);
    if (ok) {
        ui->statusbar->showMessage(QStringLiteral("笛卡尔运动已发送"));
    } else {
        ui->statusbar->showMessage(QStringLiteral("笛卡尔运动失败:请检查连接、目标点可达性、是否处于报警状态"));
    }
}

/**
 * @brief 获取当前关节位置并插入 MOVEJ 指令
 */
void MainWindow::onGetJoints()
{
    if (m_uiController)
        m_uiController->onProgInsertMoveJ();
    ui->statusbar->showMessage(QStringLiteral("已插入 MOVEJ 指令"));
}

/**
 * @brief 获取当前笛卡尔位置并插入 MOVEL 指令
 */
void MainWindow::onGetCart()
{
    if (m_uiController)
        m_uiController->onProgInsertMoveL();
    ui->statusbar->showMessage(QStringLiteral("已插入 MOVEL 指令"));
}

/**
 * @brief 导航按钮点击切换页面
 * @param index 页面索引（0-5）
 */
void MainWindow::onNavButtonClicked(int index)
{
    ui->contentStack->setCurrentIndex(index);
    const QString names[] = {"运行", "程序编程", "IO 监控", "报警日志", "设置", "通信"};
    if (index >= 0 && index < 6)
        ui->statusbar->showMessage(QStringLiteral("切换到: %1").arg(names[index]));
}

// ==================== RS485通信 ====================

/**
 * @brief 初始化 RS485
 *
 * 设置默认配置（9600/8/N/1），注册所有信号槽，加载当前可用端口。
 */
void MainWindow::initRs485UI()
{
    // 发送功能（通过 IP 连接）
    connect(ui->rs485BtnSend,       &QPushButton::clicked, this, &MainWindow::onRs485Send);
    connect(ui->rs485BtnClearSend,  &QPushButton::clicked, this, &MainWindow::onRs485ClearSend);

    // 接收功能
    connect(ui->rs485BtnClearRecv,  &QPushButton::clicked, this, &MainWindow::onRs485ClearRecv);

    // 定时发送功能
    connect(ui->rs485ChkAutoSend,   &QCheckBox::toggled,  this, &MainWindow::onRs485AutoSendToggled);

    // 串口控制器信号（通过ZMC发送，不支持接收回调）
    // connect(m_rs485, &Rs485Controller::dataReceived,   this, &MainWindow::onRs485DataReceived);
    // connect(m_rs485, &Rs485Controller::dataSent,       this, &MainWindow::onRs485DataSent);

    ui->rs485LblSent->setText(QStringLiteral("已发送: 0 字节"));
    ui->rs485LblReceived->setText(QStringLiteral("已接收: N/A"));
}

/**
 * @brief 通过 ZMC306X 控制器的串口发送 Modbus RTU 数据到 PWM 模块（使用 MODBUS 主端模式）
 *
 * 通信流程：
 *   1. 配置串口为 MODBUS 主端模式（mode=14，9600,8,N,1）
 *   2. 建立 MODBUS 连接（MODBUSM_DES）
 *   3. 将数据写入 MODBUS_REG 寄存器
 *   4. 发送 MODBUS 写命令（MODBUSM_REGSET）
 *   5. 读取 PWM 模块的响应
 *
 * 目标设备：QD-6PWM-V2.0 6路PWM输出模块（Modbus RTU，站号0x21）
 */
void MainWindow::onRs485Send()
{
    const QString text = ui->rs485EditSend->toPlainText();
    if (text.isEmpty()) return;

    if (!m_robotController || !m_robotController->isConnected()) {
        QMessageBox::warning(this, QStringLiteral("RS485"), QStringLiteral("请先通过以太网连接到控制器"));
        return;
    }

    ZmotionAPI* api = m_robotController->getMotionController()->getZmotionAPI();
    if (!api) {
        QMessageBox::warning(this, QStringLiteral("RS485"), QStringLiteral("无法获取 ZMotion API"));
        return;
    }

    // 步骤1: 解析 HEX 输入
    bool ok = false;
    QByteArray data = parseHexString(text, &ok);
    if (!ok) {
        QMessageBox::warning(this, QStringLiteral("RS485"),
                             QStringLiteral("HEX 格式错误，请输入形如 \"21 10 22 51 00 03 06 00 27 10 00 32 00\" 的字节序列"));
        return;
    }

    // 验证 Modbus RTU 帧格式
    if (data.size() < 6) {
        QMessageBox::warning(this, QStringLiteral("RS485"),
                             QStringLiteral("数据长度不足，至少需要包含站号+功能码+地址+数量"));
        return;
    }

    quint8 slaveAddr = static_cast<quint8>(data[0]);
    quint8 funcCode = static_cast<quint8>(data[1]);
    quint16 startAddr = (static_cast<quint8>(data[2]) << 8) | static_cast<quint8>(data[3]);
    quint16 regCount = (static_cast<quint8>(data[4]) << 8) | static_cast<quint8>(data[5]);

// 计算 CRC16 并自动补全到数据中
    quint16 crc = calcModbusCRC16(data);
    QByteArray dataWithCrc = data;
    dataWithCrc.append(static_cast<char>(crc & 0xFF));
    dataWithCrc.append(static_cast<char>((crc >> 8) & 0xFF));

const QString ts = QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss.zzz "));
    const QString prefix = ui->rs485ChkTimestamp->isChecked() ? ts : QString();

    // 显示发送数据
    qDebug() << "[Rs485] [发送]" << bytesToHex(data);
    qDebug() << "[Rs485] [完整帧]" << bytesToHex(dataWithCrc);
    appendRs485RecvText(QStringLiteral("%1[发送] %2\n").arg(prefix).arg(bytesToHex(data)));
    appendRs485RecvText(QStringLiteral("%1[帧+CRC] %2\n").arg(prefix).arg(bytesToHex(dataWithCrc)));

    // 提取寄存器数据部分（原始16位寄存器字节流，跳过Modbus PDU头）
    // data = [slaveAddr, funcCode, startAddrH, startAddrL, ...]
    //   0x10写多个: ...regCountH, regCountL, byteCount, payload...
    //   0x06写单个: ...valueH, valueL
    QByteArray modbusData;
    if (funcCode == 0x10 && data.size() > 7) {
        // 跳过 slave(1)+func(1)+addr(2)+regCount(2)+byteCount(1) = 7字节
        int pduByteCount = static_cast<quint8>(data[6]);
        modbusData = data.mid(7, pduByteCount);
    } else if (funcCode == 0x06 && data.size() >= 6) {
        // 写单个寄存器：data[4-5]是2字节的值
        modbusData = data.mid(4, 2);
    } else {
        modbusData = data.mid(4);
    }

    qDebug() << "[Rs485] 调用 Rs485Controller::sendModbusRequest()";
    bool success = m_robotController->getRs485Controller()->sendModbusRequest(
        slaveAddr, funcCode, startAddr, modbusData);

    if (!success) {
        appendRs485RecvText(QStringLiteral("%1[结果] ❌ 发送失败 (从站无响应或通信错误)\n").arg(prefix));
        return;
    }

    // 发送成功 — ZMC已确认将Modbus帧发送到RS485总线
    // 注意：sendModbusRequest 内部会尝试通过 MODBUSM_REGGET 读取响应
    // 详细结果请查看控制台日志
    appendRs485RecvText(QStringLiteral("%1[结果] ✓ 已发送 (见控制台日志)\n").arg(prefix));
}

/**
 * @brief 清空发送框
 */
void MainWindow::onRs485ClearSend()
{
    ui->rs485EditSend->clear();
}

/**
 * @brief 接收数据回调（通过ZMC发送时不支持接收）
 */
void MainWindow::onRs485DataReceived(const QByteArray& /*data*/)
{
    // ZMC Modbus转RS485模式不支持接收回调
}

/**
 * @brief 发送数据回调（更新发送字节计数显示）
 */
void MainWindow::onRs485DataSent(qint64 /*bytes*/)
{
    // 暂时不做显示更新
}

/**
 * @brief 清空接收框和计数
 */
void MainWindow::onRs485ClearRecv()
{
    ui->rs485EditRecv->clear();
    // resetCounters已移除，通过ZMC发送不支持计数
    ui->rs485LblSent->setText(QStringLiteral("已发送: 0 字节"));
    ui->rs485LblReceived->setText(QStringLiteral("已接收: N/A"));
}

/**
 * @brief 测试 PWM 占空比设置
 * 
 * 设置通道1的PWM占空比为75（对应舵机90度位置）
 */
void MainWindow::onTestPwmDuty()
{
    if (!m_robotController || !m_robotController->isConnected()) {
        QMessageBox::warning(this, QStringLiteral("PWM测试"), QStringLiteral("请先连接到控制器"));
        return;
    }

    qDebug() << "[PWM测试] 设置 PWM1 占空比为 75";
    
    bool success = m_robotController->getRs485Controller()->setPwmDuty(1, 75);
    
    if (success) {
        QMessageBox::information(this, QStringLiteral("PWM测试"), QStringLiteral("PWM1 占空比设置成功！\n通道: 1\n占空比: 75/1000"));
    } else {
        QMessageBox::warning(this, QStringLiteral("PWM测试"), QStringLiteral("PWM1 占空比设置失败！"));
    }
}

/**
 * @brief RS485 定时发送复选框切换
 *
 * 勾选时启动定时器，按设定的间隔自动调用 onRs485Send()。
 * 取消勾选时停止定时器。
 */
void MainWindow::onRs485AutoSendToggled(bool checked)
{
    if (checked) {
        if (!m_robotController || !m_robotController->isConnected()) {
            QMessageBox::warning(this, QStringLiteral("定时发送"), QStringLiteral("请先连接到控制器"));
            ui->rs485ChkAutoSend->blockSignals(true);
            ui->rs485ChkAutoSend->setChecked(false);
            ui->rs485ChkAutoSend->blockSignals(false);
            return;
        }

        if (!m_rs485AutoSendTimer) {
            m_rs485AutoSendTimer = new QTimer(this);
            connect(m_rs485AutoSendTimer, &QTimer::timeout, this, &MainWindow::onRs485AutoSendTick);
        }

        int interval = ui->rs485SpinInterval->text().toDouble();
        m_rs485AutoSendTimer->start(interval);
        ui->rs485SpinInterval->setEnabled(false);
        qDebug() << "[RS485] 定时发送已启动，间隔:" << interval << "ms";
    } else {
        if (m_rs485AutoSendTimer) {
            m_rs485AutoSendTimer->stop();
        }
        ui->rs485SpinInterval->setEnabled(true);
        qDebug() << "[RS485] 定时发送已停止";
    }
}

/**
 * @brief RS485 定时发送定时器回调
 *
 * 直接调用 onRs485Send() 发送当前输入框中的数据。
 */
void MainWindow::onRs485AutoSendTick()
{
    onRs485Send();
}

/**
 * @brief 追加文本到接收框（带自动滚动）
 */
void MainWindow::appendRs485RecvText(const QString& text)
{
    if (ui->rs485ChkAutoScroll->isChecked()) {
        ui->rs485EditRecv->moveCursor(QTextCursor::End);
    }
    ui->rs485EditRecv->insertPlainText(text);
    if (ui->rs485ChkAutoScroll->isChecked()) {
        QScrollBar* bar = ui->rs485EditRecv->verticalScrollBar();
        if (bar) bar->setValue(bar->maximum());
    }
}

// (以下函数的原有实现已被上方的空实现替代)
// setRs485Status、updateRs485Buttons 等 - 已移除串口配置相关控件

/**
 * @brief 解析 HEX 字符串为字节数组
 *
 * 支持空格、逗号、0x 前缀混合的输入，例如 "01 03, 0A 0B"。
 */
QByteArray MainWindow::parseHexString(const QString& hex, bool* ok) const
{
    QByteArray out;
    QString clean = hex;
    clean.remove(QChar(' '));
    clean.remove(QChar('\t'));
    clean.remove(QChar('\n'));
    clean.remove(QChar('\r'));
    clean.remove(QChar(','));
    clean.remove(QStringLiteral("0x"), Qt::CaseInsensitive);
    if (clean.length() % 2 != 0) {
        if (ok) *ok = false;
        return {};
    }
    for (int i = 0; i < clean.length(); i += 2) {
        bool lok = false;
        int v = clean.mid(i, 2).toInt(&lok, 16);
        if (!lok) {
            if (ok) *ok = false;
            return {};
        }
        out.append(static_cast<char>(v));
    }
    if (ok) *ok = true;
    return out;
}

/**
 * @brief 字节数组转 HEX 字符串
 */
QString MainWindow::bytesToHex(const QByteArray& data) const
{
    QString s;
    s.reserve(data.size() * 3);
    for (int i = 0; i < data.size(); ++i) {
        s += QString::number(static_cast<quint8>(data[i]), 16).toUpper().rightJustified(2, QChar('0'));
        if (i != data.size() - 1) s += QChar(' ');
    }
    return s;
}

QString MainWindow::decimalToHex(const QString& decimalStr) const
{
    QStringList parts = decimalStr.trimmed().split(' ');
    QString hexStr;
    for (const QString& part : parts) {
        QString numStr = part;
        if (numStr.contains('.')) {
            numStr = numStr.section('.', 0, 0);
        }
        bool ok;
        int value = numStr.toInt(&ok);
        if (ok && value >= 0 && value <= 255) {
            hexStr += QStringLiteral("%1 ").arg(value, 2, 16, QChar('0')).toUpper();
        } else {
            hexStr += QStringLiteral("?? ");
        }
    }
    return hexStr.trimmed();
}

/**
 * @brief 计算 Modbus CRC16 校验值
 *
 * 标准 Modbus CRC16 算法（多项式 0xA001，初始值 0xFFFF）。
 * 返回值低字节在前，高字节在后（与 Modbus 帧格式一致）。
 *
 * @param data 待计算的数据（不含 CRC）
 * @return quint16 CRC16 校验值
 */
quint16 MainWindow::calcModbusCRC16(const QByteArray& data)
{
    quint16 crc = 0xFFFF;  // 初始值
    for (int i = 0; i < data.size(); ++i) {
        crc ^= static_cast<quint8>(data[i]);
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;  // 多项式
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

// ============================================================
// 快捷指令面板
// ============================================================

void MainWindow::initManualProgUI()
{
    QVBoxLayout* layout = ui->pageManualLayout;
    layout->setSpacing(4);  // 压缩整体间距

    // ==================== 1. 抽取所有控件 ====================
    auto takeItem = [](QVBoxLayout* l, int index) -> QLayoutItem* {
        return l->takeAt(index);
    };

    QLayoutItem* consoleItem  = takeItem(layout, 9);  // progConsole
    QLayoutItem* editorItem   = takeItem(layout, 8);  // progEditor
    QLayoutItem* toolBarItem  = takeItem(layout, 7);  // progToolBar
    QLayoutItem* bar3Item     = takeItem(layout, 6);  // progInstrBar3
    QLayoutItem* bar2Item     = takeItem(layout, 5);  // progInstrBar2
    QLayoutItem* bar1Item     = takeItem(layout, 4);  // progInstrBar1
    QLayoutItem* sepItem      = takeItem(layout, 3);  // manualProgSep
    QLayoutItem* debugItem    = takeItem(layout, 2);  // jogDebugGroup
    QLayoutItem* jogHItem     = takeItem(layout, 1);  // jogHLayout
    QLayoutItem* breadItem    = takeItem(layout, 0);  // breadcrumbManual

    delete bar1Item; delete bar2Item; delete bar3Item;
    delete toolBarItem; delete editorItem; delete consoleItem;

    ui->progInstrBar1->hide();
    ui->progInstrBar2->hide();
    ui->progInstrBar3->hide();
    ui->manualProgSep->hide();

    // ==================== 2. 指令按钮 → QTabWidget ====================
    QTabWidget* instrTabs = new QTabWidget(ui->pageManual);
    instrTabs->setDocumentMode(true);
    instrTabs->setMinimumHeight(60);
    instrTabs->setMaximumHeight(72);
    instrTabs->setStyleSheet(
        QStringLiteral(
            "QTabWidget::pane{border:1px solid #d0d5dd;border-radius:0 6px 6px 6px;background:#fff;}"
            "QTabBar::tab{background:#f5f5f5;color:#666;padding:4px 16px;"
            "border:1px solid #d0d5dd;border-bottom:none;border-radius:4px 4px 0 0;"
            "margin-right:2px;font-size:14px;font-weight:bold;}"
            "QTabBar::tab:selected{background:#fff;color:#1565c0;}"
            "QTabBar::tab:hover{background:#e3f2fd;}"
        ));

    auto migrateBtn = [](QPushButton* btn, QHBoxLayout* dstLayout) {
        if (!btn) return;
        QWidget* oldParent = btn->parentWidget();
        if (oldParent) {
            QLayout* oldLayout = oldParent->layout();
            if (oldLayout) oldLayout->removeWidget(btn);
        }
        btn->setParent(dstLayout->parentWidget());
        dstLayout->addWidget(btn);
    };

    // Tab 0: 运动
    {
        QWidget* tab = new QWidget();
        QHBoxLayout* lay = new QHBoxLayout(tab);
        lay->setContentsMargins(10, 6, 10, 6); lay->setSpacing(8);
        migrateBtn(ui->btnProgMoveJ, lay);    ui->btnProgMoveJ->setText(QStringLiteral("关节"));
        migrateBtn(ui->btnProgMoveL, lay);    ui->btnProgMoveL->setText(QStringLiteral("直线"));
        migrateBtn(ui->btnProgMoveOff, lay);  ui->btnProgMoveOff->setText(QStringLiteral("关节偏移"));
        migrateBtn(ui->btnProgMoveLOff, lay); ui->btnProgMoveLOff->setText(QStringLiteral("直线偏移"));
        // 统一指令按钮样式
        for (QPushButton* b : {ui->btnProgMoveJ, ui->btnProgMoveL, ui->btnProgMoveOff, ui->btnProgMoveLOff}) {
            b->setMinimumSize(80, 36);
            b->setFont(QFont(b->font().family(), 11, QFont::Bold));
            b->setStyleSheet(QStringLiteral(
                "QPushButton{background:#e8f0fe;color:#1557b0;border:1px solid #a8c8fa;border-radius:4px;"
                "padding:2px 10px;font-weight:bold;}"
                "QPushButton:hover{background:#d0e2ff;}"));
        }
        lay->addStretch();
        instrTabs->addTab(tab, QStringLiteral("  运动  "));
    }

    // 隐藏已移除的按钮
    ui->btnProgSpeed->hide();
    ui->btnProgAccel->hide();
    ui->btnProgDecel->hide();
    ui->btnProgTimeout->hide();
    ui->btnProgWhileDI->hide();

    // Tab 1: 逻辑
    {
        QWidget* tab = new QWidget();
        QHBoxLayout* lay = new QHBoxLayout(tab);
        lay->setContentsMargins(10, 6, 10, 6); lay->setSpacing(8);
        migrateBtn(ui->btnProgWaitIdle, lay); ui->btnProgWaitIdle->setText(QStringLiteral("等待完成"));
        migrateBtn(ui->btnProgDelay, lay);    ui->btnProgDelay->setText(QStringLiteral("延时"));
        QFrame* s1 = new QFrame(); s1->setFrameShape(QFrame::VLine);
        s1->setStyleSheet(QStringLiteral("color:#e0e0e0;"));
        lay->addWidget(s1);
        migrateBtn(ui->btnProgSetDO, lay);  ui->btnProgSetDO->setText(QStringLiteral("设输出"));
        migrateBtn(ui->btnProgWaitDI, lay); ui->btnProgWaitDI->setText(QStringLiteral("等输入"));
        QFrame* s2 = new QFrame(); s2->setFrameShape(QFrame::VLine);
        s2->setStyleSheet(QStringLiteral("color:#e0e0e0;"));
        lay->addWidget(s2);
        migrateBtn(ui->btnProgIfDI, lay);   ui->btnProgIfDI->setText(QStringLiteral("如果"));
        migrateBtn(ui->btnProgElse, lay);   ui->btnProgElse->setText(QStringLiteral("否则"));
        QFrame* s3 = new QFrame(); s3->setFrameShape(QFrame::VLine);
        s3->setStyleSheet(QStringLiteral("color:#e0e0e0;"));
        lay->addWidget(s3);
        migrateBtn(ui->btnProgSet, lay);    ui->btnProgSet->setText(QStringLiteral("赋值"));
        // 统一逻辑按钮样式
        for (QPushButton* b : {ui->btnProgWaitIdle, ui->btnProgDelay, ui->btnProgSetDO,
                                ui->btnProgWaitDI, ui->btnProgIfDI, ui->btnProgElse, ui->btnProgSet}) {
            b->setMinimumSize(76, 36);
            b->setFont(QFont(b->font().family(), 11, QFont::Bold));
            b->setStyleSheet(QStringLiteral(
                "QPushButton{background:#f3e5f5;color:#7b1fa2;border:1px solid #ce93d8;border-radius:4px;"
                "padding:2px 10px;font-weight:bold;}"
                "QPushButton:hover{background:#e1bee7;}"));
        }
        lay->addStretch();
        instrTabs->addTab(tab, QStringLiteral("  逻辑  "));
    }

    // Tab 2: 工具
    {
        QWidget* tab = new QWidget();
        QHBoxLayout* lay = new QHBoxLayout(tab);
        lay->setContentsMargins(10, 6, 10, 6); lay->setSpacing(8);
        migrateBtn(ui->btnProgPrint, lay);   ui->btnProgPrint->setText(QStringLiteral("打印"));
        migrateBtn(ui->btnProgReadPos, lay); ui->btnProgReadPos->setText(QStringLiteral("读位置"));
        migrateBtn(ui->btnProgCall, lay);    ui->btnProgCall->setText(QStringLiteral("调用"));
        migrateBtn(ui->btnProgFor, lay);     ui->btnProgFor->setText(QStringLiteral("FOR循环"));
        migrateBtn(ui->btnProgComment, lay); ui->btnProgComment->setText(QStringLiteral("注释"));
        // 统一工具按钮样式
        for (QPushButton* b : {ui->btnProgPrint, ui->btnProgReadPos, ui->btnProgCall,
                                ui->btnProgFor, ui->btnProgComment}) {
            b->setMinimumSize(76, 36);
            b->setFont(QFont(b->font().family(), 11, QFont::Bold));
            b->setStyleSheet(QStringLiteral(
                "QPushButton{background:#e8f5e9;color:#2e7d32;border:1px solid #a5d6a7;border-radius:4px;"
                "padding:2px 10px;font-weight:bold;}"
                "QPushButton:hover{background:#c8e6c9;}"));
        }
        // SERVO 单舵机按钮
        QPushButton* btnServo = new QPushButton(QStringLiteral("SERVO"));
        btnServo->setMinimumSize(88, 36);
        btnServo->setStyleSheet(
            QStringLiteral("QPushButton{background:#e3f2fd;color:#1565c0;border:1px solid #90caf9;border-radius:4px;"
                "padding:2px 8px;font-weight:bold;font-size:15px;font-family:Consolas;}"
                "QPushButton:hover{background:#bbdefb;}"));
        connect(btnServo, &QPushButton::clicked, this, [this] {
            if (m_uiController) m_uiController->onProgInsertServo();
        });
        lay->addWidget(btnServo);
        // 注（2026-06-15）：原 SERVO_S 多舵机按钮已删除，SERVR_S 语法并入 SERVO，
        // 通过 SERVO(0=..., 1=..., 2=...) 即可同时控制多路。
        // WHILE TRUE 循环按钮
        QPushButton* btnWhile = new QPushButton(QStringLiteral("WHILE循环"));
        btnWhile->setMinimumSize(104, 36);
        btnWhile->setStyleSheet(
            QStringLiteral("QPushButton{background:#fce4ec;color:#c62828;border:1px solid #ef9a9a;border-radius:4px;"
                "padding:2px 8px;font-weight:bold;font-size:15px;font-family:Consolas;}"
                "QPushButton:hover{background:#ffcdd2;}"));
        connect(btnWhile, &QPushButton::clicked, this, [this] {
            if (m_uiController) {
                m_uiController->insertAtCursor(QStringLiteral("WHILE TRUE"));
                m_uiController->insertAtCursor(QStringLiteral("END_WHILE"));
            }
        });
        lay->addWidget(btnWhile);
        lay->addStretch();
        instrTabs->addTab(tab, QStringLiteral("  工具  "));
    }

    // ==================== 3. 手动调试 → 单行紧凑布局 ====================
    ui->debugSpeedRow->removeWidget(ui->label_15);
    ui->debugSpeedRow->removeWidget(ui->debugSpeed);
    ui->debugAccelRow->removeWidget(ui->label_16);
    ui->debugAccelRow->removeWidget(ui->debugAccel);
    ui->debugDecelRow->removeWidget(ui->label_17);
    ui->debugDecelRow->removeWidget(ui->debugDecel);
    ui->debugStepRow->removeWidget(ui->label_18);
    ui->debugStepRow->removeWidget(ui->debugStep);

    auto clearLayoutItems = [](QLayout* l) {
        while (l->count() > 0) { delete l->takeAt(0); }
    };
    clearLayoutItems(ui->debugSpeedRow);
    clearLayoutItems(ui->debugAccelRow);
    clearLayoutItems(ui->debugDecelRow);
    clearLayoutItems(ui->debugStepRow);

    QVBoxLayout* debugVLay = qobject_cast<QVBoxLayout*>(ui->jogDebugGroup->layout());
    if (debugVLay) {
        while (debugVLay->count() > 1) delete debugVLay->takeAt(1);

        // 2×2 网格: 步长/速度上排，加速度/减速度下排
        debugVLay->setContentsMargins(0, 10, 0, 10);
        debugVLay->setSpacing(8);

        ui->chkDebugMode->setText(QStringLiteral("调试模式"));
        ui->chkDebugMode->setStyleSheet(
            QStringLiteral("QCheckBox{color:#333;font-size:15px;spacing:3px;font-weight:bold;}"));

        // 清空旧项（保留 chkDebugMode）
        while (debugVLay->count() > 1) delete debugVLay->takeAt(1);

        // 标签和 SpinBox 紧凑化
        auto compactLabel = [](QLabel* lbl) {
            lbl->setMinimumSize(20, 0);
            lbl->setMaximumWidth(55);
            lbl->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            lbl->setStyleSheet(QStringLiteral("color:#888;font-size:13px;font-weight:bold;border:none;background:transparent;"));
        };
        auto compactLineEdit = [](QLineEdit* le) {
            le->setFixedHeight(24);
            le->setMinimumWidth(60);
            le->setMaximumWidth(85);
            le->setAlignment(Qt::AlignCenter);
            le->setStyleSheet(QStringLiteral(
                "QLineEdit{border:1px solid #d0d5dd;border-radius:3px;"
                "padding:1px 2px;font-size:14px;}"
                "QLineEdit:focus{border-color:#1565c0;}"
            ));
        };

        compactLabel(ui->label_15); compactLabel(ui->label_16);
        compactLabel(ui->label_17); compactLabel(ui->label_18);
        ui->label_15->setText(QStringLiteral("步长"));
        ui->label_16->setText(QStringLiteral("速度"));
        ui->label_17->setText(QStringLiteral("加速度"));
        ui->label_18->setText(QStringLiteral("减速度"));

        compactLineEdit(ui->debugStep);
        compactLineEdit(ui->debugSpeed);
        compactLineEdit(ui->debugAccel);
        compactLineEdit(ui->debugDecel);

        // 设置默认值，避免 toDouble() 对空字符串返回 0 导致运动无效
        if (ui->debugStep->text().isEmpty())  ui->debugStep->setText(QStringLiteral("10"));
        if (ui->debugSpeed->text().isEmpty()) ui->debugSpeed->setText(QStringLiteral("25"));
        if (ui->debugAccel->text().isEmpty()) ui->debugAccel->setText(QStringLiteral("50"));
        if (ui->debugDecel->text().isEmpty()) ui->debugDecel->setText(QStringLiteral("60"));

        // 2×2 QGridLayout
        QGridLayout* debugGrid = new QGridLayout();
        debugGrid->setSpacing(6);
        debugGrid->setContentsMargins(0, 0, 0, 0);
        debugGrid->setColumnStretch(0, 1);
        debugGrid->setColumnStretch(1, 1);

        struct { QLabel* label; QLineEdit* spin; } debugCells[] = {
            {ui->label_15, ui->debugStep},
            {ui->label_16, ui->debugSpeed},
            {ui->label_17, ui->debugAccel},
            {ui->label_18, ui->debugDecel},
        };
        for (int i = 0; i < 4; ++i) {
            QHBoxLayout* row = new QHBoxLayout();
            row->setSpacing(2);
            row->setContentsMargins(0, 0, 0, 0);
            row->addWidget(debugCells[i].label);
            row->addWidget(debugCells[i].spin);
            debugGrid->addLayout(row, i / 2, i % 2);
        }
        debugVLay->addLayout(debugGrid);
    }

    // 容器拉大（移除手动调试标题）
    ui->jogDebugGroup->setTitle("");
    ui->jogDebugGroup->setStyleSheet(
        QStringLiteral(
            "QGroupBox{background:#ffffff;border:1px solid #d0d5dd;border-radius:18px;"
            "margin-top:4px;padding:14px 10px 14px 10px;font-weight:bold;font-size:12px;color:#1a1a2e;}"
        ));

    // ==================== 合并点动组：关节 / 笛卡尔切换 ====================

    auto compactLabel = [](QLabel* lbl) {
        lbl->setMinimumSize(20, 0);
        lbl->setMaximumWidth(32);
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setStyleSheet(QStringLiteral("color:#999;font-size:12px;font-weight:bold;border:none;background:transparent;"));
    };
    auto compactLineEdit2 = [](QLineEdit* le) {
        le->setFixedHeight(26);
        le->setMinimumWidth(50);
        le->setMaximumWidth(68);
        le->setAlignment(Qt::AlignCenter);
        le->setStyleSheet(QStringLiteral(
            "QLineEdit{border:none;background:transparent;"
            "font-family:Consolas,'Microsoft YaHei UI';font-size:12px;"
            "color:#1565c0;font-weight:bold;padding:2px 4px;}"));
    };

    // 从旧 GroupBox 提取控件并构建 3×2 页面
    auto buildJogPage = [&](QGroupBox* srcGroup,
                            const QStringList& labelNames,
                            const QStringList& spinNames,
                            const QString& btnName,
                            const QStringList& prefixes) -> QWidget*
    {
        QMap<QString, QLabel*> labelMap;
        QMap<QString, QLineEdit*> spinMap;
        for (QLabel* w : srcGroup->findChildren<QLabel*>()) {
            if (labelNames.contains(w->objectName())) labelMap[w->objectName()] = w;
        }
        for (QLineEdit* w : srcGroup->findChildren<QLineEdit*>()) {
            if (spinNames.contains(w->objectName())) spinMap[w->objectName()] = w;
        }
        QPushButton* applyBtn = srcGroup->findChild<QPushButton*>(btnName);

       
        QLayout* oldLay = srcGroup->layout();
        if (oldLay) {
            std::function<void(QLayout*)> clearRec = [&](QLayout* l) {
                while (l->count() > 0) {
                    QLayoutItem* item = l->takeAt(0);
                    if (item->layout()) clearRec(item->layout());
                    delete item;
                }
            };
            clearRec(oldLay);
            delete oldLay;
        }
        srcGroup->hide();

        // 构建页面
        QWidget* page = new QWidget();
        QGridLayout* gridLay = new QGridLayout(page);
        gridLay->setSpacing(8);
        gridLay->setContentsMargins(0, 16, 0, 16);// 上下边距 16px，左右边距 0
        gridLay->setColumnStretch(0, 1);// 左右两列等宽，便于对齐
        gridLay->setColumnStretch(1, 1);// 左右两列等宽，便于对齐

        for (int i = 0; i < spinNames.size(); ++i) {
            QLineEdit* sp = spinMap.value(spinNames[i]);
            if (!sp) continue;

            // 隐藏旧 label（不再使用，名称通过 placeholder 内嵌到 QLineEdit 中）
            QLabel* lbl = labelMap.value(labelNames[i]);
            if (lbl) lbl->hide();

            sp->setParent(page);
            sp->setMinimumHeight(26);
            sp->setMinimumWidth(60);
            sp->setMaximumWidth(80);
            sp->setPlaceholderText(prefixes[i] + QStringLiteral(": "));
            sp->setStyleSheet(QStringLiteral(
                "QLineEdit{font-family:Consolas,'Microsoft YaHei UI';font-size:13px;"
                "padding:2px 6px;border:1px solid #d0d5dd;border-radius:4px;"
                "background:#fafbfc;color:#1565c0;font-weight:bold;min-width:60px;max-width:80px;}"));
            sp->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);      

            gridLay->addWidget(sp, i / 2, i % 2);
        }

        if (applyBtn) {// 底部"调节"按钮：跨两列居中
            applyBtn->setParent(page);
            applyBtn->setFixedHeight(48);//调节按钮
            applyBtn->setStyleSheet(
                QStringLiteral(
                    "QPushButton{background:#1565c0;color:#fff;border:1px solid #0d47a1;"
                    "border-radius:6px;font-weight:bold;font-size:18px;padding:8px 20px;}"
                    "QPushButton:hover{background:#0d47a1;}"
                    "QPushButton:pressed{background:#0a3d8a;}"));
            gridLay->addWidget(applyBtn, (labelNames.size() + 1) / 2, 0, 1, 2, Qt::AlignCenter);// 底部"调节"按钮：跨两列居中
        }

        return page;
    };

    // 左侧栏: 手动调试 + 点动控制(合并)
    QWidget* leftPanel = new QWidget();
    QVBoxLayout* leftLay = new QVBoxLayout(leftPanel);
    leftLay->setContentsMargins(0, 0, 4, 0);
    leftLay->setSpacing(6);

    // 从 jogHLayout 中取出两个 groupbox
    QHBoxLayout* jogHLay = qobject_cast<QHBoxLayout*>(jogHItem->layout());
    if (jogHLay) {
        jogHLay->removeWidget(ui->jogJointGroup);
        jogHLay->removeWidget(ui->jogCartGroup);
        while (jogHLay->count() > 0) delete jogHLay->takeAt(0);
    }
    delete jogHItem;

        // 取关节/笛卡尔 SpinBox(从旧 groupbox 拿,先把这些 groupbox 隐藏)
    ui->jogJointGroup->hide();
    ui->jogCartGroup->hide();

    QLineEdit* spinJ[6] = {
        ui->spinManualJ0, ui->spinManualJ1, ui->spinManualJ2,
        ui->spinManualJ3, ui->spinManualJ4, ui->spinManualJ5
    };
    QLineEdit* spinC[6] = {
        ui->spinManualX,  ui->spinManualY,  ui->spinManualZ,
        ui->spinManualRx, ui->spinManualRy, ui->spinManualRz
    };
    const QString jointLabels[6] = {"J0", "J1", "J2", "J3", "J4", "J5"};
    const QString cartLabels[6]  = {"X",  "Y",  "Z",  "A",  "B",  "C"};

    // 调节按钮:取出显示用
    QPushButton* btnApplyJoint = ui->jogJointGroup->findChild<QPushButton*>(QStringLiteral("btnApplyJoint"));
    QPushButton* btnApplyCart  = ui->jogCartGroup->findChild<QPushButton*>(QStringLiteral("btnApplyCart"));
    if (btnApplyJoint)
        connect(btnApplyJoint, &QPushButton::clicked, this, [this] { showPointAdjustDialog(true); });
    if (btnApplyCart)
        connect(btnApplyCart,  &QPushButton::clicked, this, [this] { showPointAdjustDialog(false); });

    // 合并容器
    QFrame* jogMergedGroup = new QFrame(leftPanel);
    jogMergedGroup->setStyleSheet(
        QStringLiteral(
            "QFrame{background:#ffffff;border:1px solid #d0d5dd;border-radius:18px;}"));
    jogMergedGroup->setFrameShape(QFrame::StyledPanel);

    QVBoxLayout* jogMainLay = new QVBoxLayout(jogMergedGroup);
    jogMainLay->setSpacing(8);
    jogMainLay->setContentsMargins(0, 0, 0, 0); // 18px 左右边距,26px 下边距

    // 标题("点动控制")
    QLabel* jogTitle = new QLabel(QStringLiteral("点动控制"), jogMergedGroup);
    jogTitle->setStyleSheet(
        QStringLiteral("color:#1a1a2e;font-weight:bold;font-size:15px;border:none;background:transparent;"));
    jogMainLay->addWidget(jogTitle);

    // ===== 两列同时显示:左列 J0~J5(关节),右列 X/Y/Z/A/B/C(笛卡尔) =====
    // 6 行 × 2 列的 SpinBox 网格:左边一列是 J0~J5,右边一列是 X/Y/Z/A/B/C
    QGridLayout* twoColGrid = new QGridLayout();
    twoColGrid->setHorizontalSpacing(8);
    twoColGrid->setVerticalSpacing(4);
    twoColGrid->setContentsMargins(0, 4, 0, 4);

    // 中间加一条 1px 分隔线(放在第 7 列)
    // 关节列:0,1,2,3,4,5 | 分隔:6 | 笛卡尔列:7,8,9,10,11,12
    // 让关节列拉伸 1、笛卡尔列拉伸 1,中间不拉伸
    for (int i = 0; i < 6; ++i) {
        QLineEdit* spJ = spinJ[i];
        QLineEdit* spC = spinC[i];

        // SpinBox 重新设置 prefix(因为 buildJogPage 没跑过,SpinBox 仍是默认值)
        spJ->setParent(jogMergedGroup);
        spC->setParent(jogMergedGroup);
        spJ->setPlaceholderText(jointLabels[i] + QStringLiteral(": "));
        spC->setPlaceholderText(cartLabels[i]  + QStringLiteral(": "));
        // 加大字体 + 加大 padding,值更醒目
        const QString spinStyle = QStringLiteral(
            "QLineEdit{font-family:Consolas,'Microsoft YaHei UI';font-size:17px;"
            "padding:5px 8px;border:1px solid #d0d5dd;border-radius:4px;"
            "background:#fafbfc;color:#1565c0;font-weight:bold;}");
        spJ->setStyleSheet(spinStyle);
        spC->setStyleSheet(spinStyle);
        spJ->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        spC->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        spJ->setMinimumHeight(36);
        spC->setMinimumHeight(36);
        // ★ 只读:点动控制只是显示位置,不让用户修改(否则会被"手动调试发送"读走当 moveJ 目标)
        spJ->setReadOnly(true);
        spC->setReadOnly(true);
        // 不限制最大宽度,让 SpinBox 在拉伸列里尽量占满
        spJ->setMinimumWidth(0);
        spC->setMinimumWidth(0);

        twoColGrid->addWidget(spJ, i, 0);
        twoColGrid->addWidget(spC, i, 2);
    }
    // 分隔线(第 1 列)
    QFrame* vline = new QFrame(jogMergedGroup);
    vline->setFrameShape(QFrame::VLine);
    vline->setFrameShadow(QFrame::Plain);
    vline->setStyleSheet(QStringLiteral("color:#e0e0e0;background:#e0e0e0;"));
    vline->setFixedWidth(1);
    twoColGrid->addWidget(vline, 0, 1, 6, 1);
    twoColGrid->setColumnStretch(0, 1);
    twoColGrid->setColumnStretch(1, 0);
    twoColGrid->setColumnStretch(2, 1);
    jogMainLay->addLayout(twoColGrid);

    // "调节"按钮:放在两列下方中央
    // 根据当前模式自动选关节/笛卡尔弹窗
    QPushButton* btnOpenPointAdjust = new QPushButton(QStringLiteral("调节"), jogMergedGroup);
    btnOpenPointAdjust->setFixedHeight(40);
    btnOpenPointAdjust->setCursor(Qt::PointingHandCursor);
    btnOpenPointAdjust->setStyleSheet(QStringLiteral(
        "QPushButton{background:#1565c0;color:#fff;border:1px solid #0d47a1;"
        "border-radius:6px;font-weight:bold;font-size:15px;padding:4px 16px;}"
        "QPushButton:hover{background:#0d47a1;}"
        "QPushButton:pressed{background:#0a3d8a;}"));
    connect(btnOpenPointAdjust, &QPushButton::clicked, this, [this] {
        if (!m_robotController) return;
        // 根据当前模式决定弹哪个对话框(正解=关节,逆解=笛卡尔)
        const bool isCart = m_robotController->isCartesianMode();
        showPointAdjustDialog(isCart);
    });
    jogMainLay->addWidget(btnOpenPointAdjust, 0, Qt::AlignHCenter);

    // 调试模式控制点动可用性
    auto setJogEnabled = [this, jogMergedGroup](bool enabled) {
        for (QLineEdit* sp : jogMergedGroup->findChildren<QLineEdit*>())
            sp->setReadOnly(!enabled);
        QString labelColor = enabled ? QStringLiteral("#000") : QStringLiteral("#666");
        QString labelSS = QStringLiteral("color:%1;font-size:16px;border:none;background:transparent;").arg(labelColor);
        for (QLabel* lb : {ui->label_15, ui->label_16, ui->label_17, ui->label_18})
            lb->setStyleSheet(labelSS);
    };
    setJogEnabled(ui->chkDebugMode->isChecked());
    connect(ui->chkDebugMode, &QCheckBox::toggled, this, setJogEnabled);

    leftLay->addWidget(ui->jogDebugGroup);      // 手动调试
    leftLay->addWidget(jogMergedGroup);          // 点动控制(关节/笛卡尔)

    // 舵机板块：3×2 网格布局
    // 控制全部在"舵机设置"弹窗内完成
    QFrame* servoGroup = new QFrame(leftPanel);
    servoGroup->setStyleSheet(
        QStringLiteral(
            "QFrame{background:#ffffff;border:1px solid #d0d5dd;border-radius:18px;}"));
    servoGroup->setFrameShape(QFrame::StyledPanel);

    QVBoxLayout* servoLay = new QVBoxLayout(servoGroup);
    servoLay->setSpacing(10);
    servoLay->setContentsMargins(18, 18, 18, 26);

    QLabel* servoTitle = new QLabel(QStringLiteral("舵机"), servoGroup);
    servoTitle->setStyleSheet(
        QStringLiteral("color:#1a1a2e;font-weight:bold;font-size:15px;border:none;background:transparent;"));
    servoLay->addWidget(servoTitle);

    // 3×2 QGridLayout
    QGridLayout* servoGrid = new QGridLayout();
    servoGrid->setSpacing(8);
    servoGrid->setContentsMargins(0, 0, 0, 0);
    servoGrid->setColumnStretch(0, 1);
    servoGrid->setColumnStretch(1, 1);

    m_servoAngleLabels.clear();
    for (int i = 0; i < 6; ++i) {
        QLabel* angleLbl = new QLabel(QStringLiteral("CH%1 : 90.0°").arg(i));
        angleLbl->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        angleLbl->setMinimumHeight(26);
        angleLbl->setMinimumWidth(100);
        angleLbl->setMaximumWidth(120);
        angleLbl->setStyleSheet(
            QStringLiteral(
                "QLabel{font-family:Consolas,'Microsoft YaHei UI';font-size:14px;"
                "color:#1565c0;font-weight:bold;background:#fafbfc;"
                "border:1px solid #d0d5dd;border-radius:5px;padding:3px 8px;}"
            ));
        m_servoAngleLabels.append(angleLbl);

        servoGrid->addWidget(angleLbl, i / 2, i % 2);
    }

    servoLay->addLayout(servoGrid);

    // 订阅 ServoController::angleChanged，实时刷新手动页舵机板块角度 Label
    {
        ServoController* servo = m_robotController ? m_robotController->getServoController() : nullptr;
        if (servo) {
            connect(servo, &ServoController::angleChanged, this,
                [this](int channel, float angle) {
                    if (channel < 0 || channel >= m_servoAngleLabels.size()) return;
                    m_servoAngleLabels[channel]->setText(
                        QString("CH%1 : %2°").arg(channel).arg(angle, 0, 'f', 1));
                });
        }
    }

    servoGroup->setLayout(servoLay);

    // 舵机设置按钮：样式与调节按钮一致，跨两列居中
    QPushButton* btnServoDebug = new QPushButton(QStringLiteral("舵机设置"), servoGroup);
    btnServoDebug->setFixedHeight(48);
    btnServoDebug->setCursor(Qt::PointingHandCursor);
    btnServoDebug->setStyleSheet(
        QStringLiteral(
            "QPushButton{background:#1565c0;color:#fff;border:1px solid #0d47a1;"
            "border-radius:6px;font-weight:bold;font-size:18px;padding:8px 20px;}"
            "QPushButton:hover{background:#0d47a1;}"
            "QPushButton:pressed{background:#0a3d8a;}"
        ));
    servoGrid->addWidget(btnServoDebug, 3, 0, 1, 2, Qt::AlignCenter);

    connect(btnServoDebug, &QPushButton::clicked, this, [this]() {
        // 防重复弹窗：已有 dialog 就置顶
        if (m_servoTestWidget && m_servoTestWidget->isVisible()) {
            m_servoTestWidget->raise();
            m_servoTestWidget->activateWindow();
            return;
        }

        ServoController* servo = m_robotController ? m_robotController->getServoController() : nullptr;
        if (!servo) {
            ui->statusbar->showMessage(QStringLiteral("舵机控制器未初始化"), 3000);
            return;
        }

        QDialog* dlg = new QDialog(this);
        dlg->setWindowTitle(QStringLiteral("舵机调试"));
        dlg->resize(680, 450);
        dlg->setAttribute(Qt::WA_DeleteOnClose);  // 关闭时自动销毁 dialog 及其子 widget

        QVBoxLayout* lay = new QVBoxLayout(dlg);
        lay->setContentsMargins(0, 0, 0, 0);

        // 复用已有的 ServoTestWidget（6通道勾选+滑块+角度+速度+应用/初始化/停止）
        // 注入 UiController 指针，使弹窗内的"插入 SERVO 到编辑器"按钮可用
        ServoTestWidget* w = new ServoTestWidget(servo, m_uiController, dlg);
        lay->addWidget(w);

        // 记录指针以便下次点击按钮时做防重复判断
        m_servoTestWidget = w;

        dlg->exec();
        // 模态返回后 dialog 已被标记为 delete，servotest widget 也会随之销毁
        m_servoTestWidget = nullptr;
    });
    leftLay->addWidget(servoGroup);

    leftLay->addStretch();

    // 右侧栏: 按参考图固定分区（指令集 + 编程控制台 + 输出控制台）
    QWidget* rightPanel = new QWidget();
    QVBoxLayout* rightLay = new QVBoxLayout(rightPanel);
    rightLay->setContentsMargins(0, 0, 0, 0);
    rightLay->setSpacing(2);

    // 1. 指令集区域（顶部）
    QGroupBox* instrGroup = new QGroupBox(QStringLiteral("指令集"));
    instrGroup->setMinimumHeight(120);
    instrGroup->setStyleSheet(
        QStringLiteral(
            "QGroupBox{background:#ffffff;border:1px solid #d0d5dd;border-radius:6px;"
            "margin-top:4px;padding:8px 6px 6px 6px;font-weight:bold;font-size:12px;color:#1a1a2e;}"
            "QGroupBox::title{subcontrol-origin:margin;left:12px;padding:0 4px;}"
        ));
    QVBoxLayout* instrLay = new QVBoxLayout(instrGroup);
    instrLay->setContentsMargins(4, 4, 4, 4);
    instrLay->setSpacing(4);
    instrLay->addWidget(instrTabs);
    rightLay->addWidget(instrGroup);

    // 2. 编程控制台区域（中部）
    // 内部布局:工具栏 | 编辑器 + 点位区域(上下分区)
    QGroupBox* progGroup = new QGroupBox(QStringLiteral("编程控制台"));
    progGroup->setStyleSheet(
        QStringLiteral(
            "QGroupBox{background:#ffffff;border:1px solid #d0d5dd;border-radius:6px;"
            "margin-top:4px;padding:8px 6px 6px 6px;font-weight:bold;font-size:12px;color:#1a1a2e;}"
            "QGroupBox::title{subcontrol-origin:margin;left:12px;padding:0 4px;}"
        ));
    QVBoxLayout* progLay = new QVBoxLayout(progGroup);
    progLay->setContentsMargins(0, 0, 0, 0);
    progLay->setSpacing(0);

    // 将 .ui 的 progEditor 替换为 CodeEditor（带行号），保留原内容
    CodeEditor* codeEditor = new CodeEditor();
    codeEditor->setObjectName(QStringLiteral("progEditor"));
    codeEditor->setStyleSheet(ui->progEditor->styleSheet());
    codeEditor->setTabStopDistance(ui->progEditor->tabStopDistance());
    codeEditor->setPlainText(ui->progEditor->toPlainText());
    codeEditor->setMinimumHeight(120);

    // 清理旧的 ui->progEditor（已从布局移除），让 ui->progEditor 指向新的 CodeEditor
    QPlainTextEdit* oldEditor = ui->progEditor;
    ui->progEditor = codeEditor;
    delete oldEditor;

    // 将工具栏添加到编程控制台区域顶部
    progLay->addWidget(ui->progToolBar);

    // 编辑器 + 点位区域:左右分区(左编辑流程,右点位数据)
    QSplitter* progSplit = new QSplitter(Qt::Horizontal, progGroup);
    progSplit->setChildrenCollapsible(false);
    progSplit->setHandleWidth(4);

    // 左:代码编辑器(编程流程)
    QWidget* editorWrap = new QWidget(progSplit);
    QVBoxLayout* editorWrapLay = new QVBoxLayout(editorWrap);
    editorWrapLay->setContentsMargins(0, 0, 0, 0);
    editorWrapLay->setSpacing(0);
    QLabel* editorLbl = new QLabel(QStringLiteral("编辑流程"), editorWrap);
    editorLbl->setStyleSheet(QStringLiteral("color:#666;font-size:11px;padding:2px 4px;"));
    editorWrapLay->addWidget(editorLbl);
    editorWrapLay->addWidget(codeEditor);

    // 右:点位数据显示区
    QWidget* targetWrap = new QWidget(progSplit);
    QVBoxLayout* targetWrapLay = new QVBoxLayout(targetWrap);
    targetWrapLay->setContentsMargins(0, 0, 0, 0);
    targetWrapLay->setSpacing(0);
    QLabel* targetLbl = new QLabel(QStringLiteral("点位数据"), targetWrap);
    targetLbl->setStyleSheet(QStringLiteral("color:#666;font-size:11px;padding:2px 4px;"));
    targetWrapLay->addWidget(targetLbl);

    QPlainTextEdit* progTargets = new QPlainTextEdit(targetWrap);
    progTargets->setObjectName(QStringLiteral("progTargets"));  // ★ 关键:UiController 通过这个 name 找到
    // 只读展示：所有点位修改统一通过"点位调节"按钮弹出的对话框完成
    progTargets->setReadOnly(true);
    progTargets->setToolTip(QStringLiteral(
        "点位定义区（只读）：显示当前 TARGET 定义；\n"
        "如需修改请点击上方「点位调节」按钮。"));
    progTargets->setStyleSheet(QStringLiteral(
        "QPlainTextEdit{background:#ffffff;color:#1565c0;"
        "border-top:1px solid #e0e0e0;border-left:1px solid #e0e0e0;border-right:none;border-bottom:none;"
        "font-family:Consolas,'Microsoft YaHei UI';font-size:11px;"
        "padding:4px 6px;selection-background-color:#264f78;}"
        "QPlainTextEdit:focus{border-top:1px solid #1565c0;background:#fcfdff;}"));
    targetWrapLay->addWidget(progTargets);

    progSplit->addWidget(editorWrap);
    progSplit->addWidget(targetWrap);
    // 初始比例:编辑器 70%, 点位 30%
    progSplit->setStretchFactor(0, 7);
    progSplit->setStretchFactor(1, 3);
    progSplit->setSizes({600, 250});

    progLay->addWidget(progSplit, 1);

    // 3. 输出控制台区域（底部）
    QGroupBox* consoleGroup = new QGroupBox(QStringLiteral("输出控制台"));
    consoleGroup->setMinimumHeight(80);
    consoleGroup->setStyleSheet(
        QStringLiteral(
            "QGroupBox{background:#ffffff;border:1px solid #d0d5dd;border-radius:6px;"
            "margin-top:4px;padding:8px 6px 6px 6px;font-weight:bold;font-size:12px;color:#1a1a2e;}"
            "QGroupBox::title{subcontrol-origin:margin;left:12px;padding:0 4px;}"
        ));
    QVBoxLayout* consoleLay = new QVBoxLayout(consoleGroup);
    consoleLay->setContentsMargins(0, 0, 0, 0);
    consoleLay->setSpacing(0);
    consoleLay->addWidget(ui->progConsole);

    // 编程控制台 + 输出控制台 上下分区(可拖动调整高度)
    QSplitter* progConsoleSplit = new QSplitter(Qt::Vertical, rightPanel);
    progConsoleSplit->setChildrenCollapsible(false);  // 不允许把任一块完全拖没
    progConsoleSplit->setHandleWidth(4);
    progConsoleSplit->addWidget(progGroup);
    progConsoleSplit->addWidget(consoleGroup);
    // 初始比例:编程控制台 4 : 输出控制台 1(让输出区更紧凑)
    progConsoleSplit->setStretchFactor(0, 4);
    progConsoleSplit->setStretchFactor(1, 1);
    progConsoleSplit->setSizes({520, 130});
    rightLay->addWidget(progConsoleSplit, 1);

    // 清除 UI 文件限制，允许自由拖动，但控制台不能完全消失
    ui->progConsole->setMinimumHeight(30);
    ui->progConsole->setMaximumHeight(16777215);
    codeEditor->setMinimumHeight(0);

    // 加"单步"按钮到工具栏（运行/暂停右侧）
    // 跟踪最后一个聚焦的代码编辑器（用于"单步"按钮）
    auto lastFocusedEditor = std::make_shared<QPlainTextEdit*>(qobject_cast<QPlainTextEdit*>(codeEditor));
    connect(qApp, &QApplication::focusChanged, this,
        [lastFocusedEditor](QWidget*, QWidget* now) {
            if (auto* ed = qobject_cast<QPlainTextEdit*>(now)) *lastFocusedEditor = ed;
        });

    QPushButton* btnRunStep = new QPushButton(QStringLiteral("单步"));
    btnRunStep->setMinimumSize(72, 36);
    btnRunStep->setStyleSheet(
        QStringLiteral("QPushButton{background:#ede7f6;color:#4527a0;border:1px solid #b39ddb;border-radius:4px;"
            "padding:2px 10px;font-weight:bold;font-size:13px;}QPushButton:hover{background:#d1c4e9;}"));
    // 单步会话状态：-1 表示尚未开始（或刚被运行/停止清掉），>=0 表示首次点击时的源行号
    auto stepState = std::make_shared<int>(-1);
    connect(btnRunStep, &QPushButton::clicked, this, [this, lastFocusedEditor, codeEditor, stepState] {
        if (!m_uiController) return;
        // TaskExecutor 正在运行或处于单步执行链中时，禁止并发点击
        if (m_uiController->isTaskRunning()) return;

        QPlainTextEdit* src = *lastFocusedEditor;
        if (!src) src = codeEditor;

        // 先同步点位表（无论是否首次点击都需要），确保 m_targets 包含用户编辑的最新点位
        m_uiController->syncTargets();
        
        QString text;
        auto* targetEd = findChild<QPlainTextEdit*>(QStringLiteral("progTargets"));
        if (targetEd) text += targetEd->toPlainText() + QStringLiteral("\n");
        
        // 关键修复：如果 lastFocusedEditor 指向了点位编辑器，则强制回退到程序编辑器
        // 否则组装文本时会丢失所有 MOVEJ/MOVEL 指令，点位值自然不生效
        if (src == targetEd) src = codeEditor;
        text += src->toPlainText();

        // 清掉旧高亮
        if (m_lastHighlightedEditor)
            m_lastHighlightedEditor->setExtraSelections({});
        m_lastHighlightedEditor = nullptr;

        if (*stepState < 0) {
            // 首次进入：从光标所在行开始
            QTextCursor cur = src->textCursor();
            int srcLine = cur.blockNumber();
            // 跳过注释/空行，找下一条可执行行
            QTextDocument* doc = src->document();
            int totalLines = doc->blockCount();
            int effectiveLine = -1;
            for (int i = srcLine; i < totalLines; ++i) {
                QString t = doc->findBlockByNumber(i).text().trimmed();
                if (t.isEmpty() || t.startsWith(QChar::fromLatin1('#')) || t.startsWith(QStringLiteral("--")))
                    continue;
                effectiveLine = i;
                break;
            }
            if (effectiveLine < 0) {
                if (auto* c = findChild<QPlainTextEdit*>(QStringLiteral("progConsoleOutput")))
                    c->appendPlainText(QStringLiteral("⚠ 未找到可执行的指令行"));
                return;
            }
            *stepState = effectiveLine;

            // 把光标移到该行首，便于用户追踪进度
            QTextCursor nc = src->textCursor();
            nc.movePosition(QTextCursor::Start);
            nc.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, effectiveLine);
            src->setTextCursor(nc);
            src->ensureCursorVisible();

            // 在该行加蓝色高亮
            QTextEdit::ExtraSelection sel;
            sel.format.setForeground(QColor(65, 105, 225));
            QTextCursor hlCur = src->textCursor();
            hlCur.movePosition(QTextCursor::StartOfBlock);
            hlCur.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            sel.cursor = hlCur;
            src->setExtraSelections({sel});
            m_lastHighlightedEditor = src;

            // 给这一行加蓝条块（轻微背景）
            QList<QTextEdit::ExtraSelection> extras = src->extraSelections();
            QTextEdit::ExtraSelection hl;
            hl.format.setBackground(QColor(65, 105, 225, 40));
            hl.format.setProperty(QTextFormat::FullWidthSelection, true);
            hl.cursor = hlCur;
            extras << hl;
            src->setExtraSelections(extras);

            // effectiveLine 是 codeEditor 内部行号，runStep 内 parseScript 使用完整文本
            // (progTargets + codeEditor) 的绝对行号，需要加上点位表的行数偏移
            int targetLineOffset = targetEd ? (targetEd->document()->blockCount() + 1) : 0;
            m_uiController->runStep(text, effectiveLine + targetLineOffset);
        } else {
            // 后续点击：从 TaskExecutor 当前 m_currentIdx 续走一条
            // 高亮由 currentLineChanged 信号负责刷新，这里只发请求
            m_uiController->runStep(text, -1);
        }
    });
    ui->progToolLayout->addWidget(btnRunStep);

    // "运行"按钮：清空单步状态 + 高亮，并通知 UiController 退出单步模式
    connect(ui->btnProgRun, &QPushButton::clicked, this, [this, stepState] {
        *stepState = -1;
        if (m_uiController) m_uiController->exitStepMode();
        if (m_lastHighlightedEditor) {
            m_lastHighlightedEditor->setExtraSelections({});
            m_lastHighlightedEditor = nullptr;
        }
    });
    // "停止"按钮：同样清空单步状态（按一次结束单步会话）
    connect(ui->btnProgStop, &QPushButton::clicked, this, [this, stepState] {
        *stepState = -1;
        if (m_uiController) m_uiController->exitStepMode();
        if (m_lastHighlightedEditor) {
            m_lastHighlightedEditor->setExtraSelections({});
            m_lastHighlightedEditor = nullptr;
        }
    });

    // "点位调节"按钮：弹出当前流程中所有引用点位的编辑对话框
    QPushButton* btnEditTargets = new QPushButton(QStringLiteral("点位调节"));
    btnEditTargets->setMinimumSize(72, 36);
    btnEditTargets->setStyleSheet(
        QStringLiteral("QPushButton{background:#fff3e0;color:#e65100;border:1px solid #ffcc80;border-radius:4px;"
            "padding:2px 10px;font-weight:bold;font-size:13px;}QPushButton:hover{background:#ffe0b2;}"));
    connect(btnEditTargets, &QPushButton::clicked, this, [this] {
        if (m_uiController) m_uiController->showTargetEditDialog();
    });
    ui->progToolLayout->addWidget(btnEditTargets);

    // 控制台标题栏加清除按钮
    QLabel* consoleTitle = ui->progConsole->findChild<QLabel*>(QStringLiteral("label_103"));
    QVBoxLayout* consoleLayout = ui->progConsole->findChild<QVBoxLayout*>(QStringLiteral("progConsoleLayout"));
    if (consoleTitle && consoleLayout) {
        consoleTitle->hide();
        auto* header = new QWidget(ui->progConsole);
        auto* headerLay = new QHBoxLayout(header);
        headerLay->setContentsMargins(6, 1, 4, 1);
        headerLay->setSpacing(0);
        auto* lbl = new QLabel(QStringLiteral("控制台"));
        lbl->setStyleSheet(QStringLiteral("color:#888;font-size:10px;font-weight:bold;border:none;background:transparent;"));
        headerLay->addWidget(lbl);
        headerLay->addStretch();
        auto* btnClear = new QPushButton(QStringLiteral("清空"));
        btnClear->setFixedSize(32, 16);
        btnClear->setStyleSheet(
            QStringLiteral("QPushButton{background:#444;color:#aaa;border:none;border-radius:2px;font-size:9px;}"
                "QPushButton:hover{background:#666;color:#fff;}"));
        connect(btnClear, &QPushButton::clicked, this, [this] {
            if (auto* c = ui->progConsole->findChild<QPlainTextEdit*>(QStringLiteral("progConsoleOutput")))
                c->clear();
        });
        headerLay->addWidget(btnClear);
        consoleLayout->insertWidget(0, header);
    }

    // 水平分割: 左侧点动 (固定宽度) | 右侧编程 (stretch)
    QWidget* bottomArea = new QWidget();
    QHBoxLayout* bottomLay = new QHBoxLayout(bottomArea);
    bottomLay->setContentsMargins(0, 0, 0, 0);
    bottomLay->setSpacing(0);
    bottomLay->addWidget(leftPanel);
    bottomLay->addWidget(rightPanel, 2);  // 右侧权重2，尽可能宽

    // 左侧面板固定宽度（收窄，给右侧更多空间）
    leftPanel->setFixedWidth(290);
    leftPanel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    // 注释掉组最大宽度限制，面板已加宽到260

    // ==================== 5. 组装最终布局 ====================
    // 面包屑 → 左右分栏(左侧:调试+点动 | 右侧:编程)
    layout->addItem(breadItem);
    delete debugItem;
    delete sepItem;
    layout->addWidget(bottomArea, 1);

    // 强制布局刷新，确保首次显示时计算正确
    ui->pageManual->updateGeometry();
}

/**
 * @brief 调节弹窗
 *   - 显示 6 轴 SpinBox + [-]/[+] 微调按钮
 *   - "获取当前点位"：从机器人读取当前位置填入各轴
 *   - "运行"：将弹窗中的目标值直接发送给机器人执行运动
 */
void MainWindow::showPointAdjustDialog(bool isJoint)
{
    // 仅调试模式下可打开调节
    if (!ui->chkDebugMode->isChecked()) return;

    QDialog dlg(this);
    double step = this->ui->debugStep->text().toDouble();
    Ui::PointAdjustDialog ui;
    ui.setupUi(&dlg);

    dlg.setWindowTitle(QStringLiteral("调节"));
    dlg.resize(380, 400);
    dlg.setMinimumSize(300, 320);

    // 隐藏 "目标点位" GroupBox 标题(下面 J0/X 标签已足够清晰)
    ui.groupBox->setTitle(QString());

    // ===== 顶部插入一行:模式切换按钮 =====
    // 单个按钮,点击后直接切换正解/逆解,文本显示当前模式
    QPushButton* btnToggleMode = new QPushButton(&dlg);
    btnToggleMode->setFixedHeight(42);  // 固定高度,适配紧凑布局
    btnToggleMode->setMaximumWidth(160);// 最大宽度,避免过长
    btnToggleMode->setCursor(Qt::PointingHandCursor);

    // ★ 提前声明 lblAxis(供 updateModeButton lambda 捕获,刷新标签文字)
    QLabel* lblAxis[6] = {ui.lblAxis0, ui.lblAxis1, ui.lblAxis2, ui.lblAxis3, ui.lblAxis4, ui.lblAxis5};

    // 刷新按钮文本/颜色 + 同步刷新轴标签
    auto updateModeButton = [this, btnToggleMode, lblAxis] {
        if (!m_robotController) {
            btnToggleMode->setText(QStringLiteral("未连接"));
            btnToggleMode->setStyleSheet(QStringLiteral(
                "QPushButton{background:#9e9e9e;color:#fff;border:none;"
                "border-radius:3px;font-size:11px;padding:2px 8px;}"));
            return;
        }
        const bool isCart = m_robotController->isCartesianMode();
        if (isCart) {
            // 当前逆解,显示一个简短的下拉箭头提示"点击切换"
            btnToggleMode->setText(QStringLiteral("逆解 (笛卡尔) ▾"));
            btnToggleMode->setStyleSheet(QStringLiteral(
                "QPushButton{background:#2e7d32;color:#fff;border:none;"
                "border-radius:3px;font-size:11px;padding:2px 8px;}"
                "QPushButton:hover{background:#1b5e20;}"));
            // 逆解模式:标签 X Y Z A B C
            const QString cartLabels[6] = {"X", "Y", "Z", "A", "B", "C"};
            for (int i = 0; i < 6; ++i) lblAxis[i]->setText(cartLabels[i]);
        } else {
            // 当前正解
            btnToggleMode->setText(QStringLiteral("正解 (关节) ▾"));
            btnToggleMode->setStyleSheet(QStringLiteral(
                "QPushButton{background:#1565c0;color:#fff;border:none;"
                "border-radius:3px;font-size:11px;padding:2px 8px;}"
                "QPushButton:hover{background:#0d47a1;}"));
            // 正解模式:标签 J0..J5
            const QString jointLabels[6] = {"J0", "J1", "J2", "J3", "J4", "J5"};
            for (int i = 0; i < 6; ++i) lblAxis[i]->setText(jointLabels[i]);
        }
    };
    updateModeButton();

    // 插入到 dialog 最顶部
    if (QVBoxLayout* mainLay = qobject_cast<QVBoxLayout*>(dlg.layout())) {
        QHBoxLayout* modeRow = new QHBoxLayout();
        modeRow->setContentsMargins(0, 0, 0, 0);
        modeRow->addWidget(btnToggleMode);
        mainLay->insertLayout(0, modeRow);
    }

    // 封装"切换到指定模式"的通用逻辑(无确认弹窗,直接切换)
    auto switchMode = [this, updateModeButton](bool wantCart) -> bool {
        if (!m_robotController || !m_robotController->isConnected()) {
            return false;
        }
        const bool isCart = m_robotController->isCartesianMode();
        if (isCart == wantCart) {
            return true;  // 已是目标模式
        }
        const bool ok = wantCart
            ? m_robotController->switchToCartesianMode()
            : m_robotController->switchToJointMode();
        if (ok) {
            updateModeButton();
            const QString modeName = wantCart
                ? QStringLiteral("逆解模式(笛卡尔空间控制)")
                : QStringLiteral("正解模式(关节空间控制)");
            if (QMainWindow* mw = qobject_cast<QMainWindow*>(this)) {
                mw->statusBar()->showMessage(QStringLiteral("已切换到 %1").arg(modeName));
            }
        }
        return ok;
    };

    // 连接按钮:点一下 → 切到与当前相反的模式
    // (switchMode 成功时会自动调 updateModeButton 刷新按钮文本/标签)
    connect(btnToggleMode, &QPushButton::clicked, &dlg, [this, switchMode] {
        if (!m_robotController) return;
        const bool wantCart = !m_robotController->isCartesianMode();
        switchMode(wantCart);
    });

    // lblAxis 已在前面提前声明(供 updateModeButton lambda 捕获)
    QLineEdit* spinAxis[6] = {ui.spinAxis0, ui.spinAxis1, ui.spinAxis2, ui.spinAxis3, ui.spinAxis4, ui.spinAxis5};
    QPushButton* btnMinus[6] = {ui.btnMinus0, ui.btnMinus1, ui.btnMinus2, ui.btnMinus3, ui.btnMinus4, ui.btnMinus5};
    QPushButton* btnPlus[6]  = {ui.btnPlus0,  ui.btnPlus1,  ui.btnPlus2,  ui.btnPlus3,  ui.btnPlus4,  ui.btnPlus5};

    const QStringList labels = m_robotController && m_robotController->isCartesianMode()
        ? QStringList{"X", "Y", "Z", "A", "B", "C"}
        : QStringList{"J0", "J1", "J2", "J3", "J4", "J5"};

    QList<QLineEdit*> spins;
    for (int i = 0; i < 6; ++i) {
        // 隐藏左侧标签(只要 6 行纯数据,不显示 J0/X 等名称)
        lblAxis[i]->hide();
        spinAxis[i]->setValidator(new QDoubleValidator(-9999.0, 9999.0, 3, spinAxis[i]));
        spinAxis[i]->setAlignment(Qt::AlignCenter);
        spinAxis[i]->setStyleSheet(QStringLiteral(
            "QLineEdit{font-family:Consolas,'Microsoft YaHei UI';font-size:20px;"
            "font-weight:bold;color:#1565c0;padding:0 10px;}"));
        spinAxis[i]->setText("0.000");
        // 给数据列留够宽度(防 6 位数被遮)
        spinAxis[i]->setMinimumWidth(140);
        spins.append(spinAxis[i]);
    }

    // 调整 [-]/[+] 按钮外观:蓝色填充圆角,与 SpinBox 视觉分离
    const QString btnStyle = QStringLiteral(
        "QPushButton{background:#1565c0;color:#fff;border:none;border-radius:6px;"
        "font-size:22px;font-weight:bold;}"
        "QPushButton:hover{background:#0d47a1;}"
        "QPushButton:pressed{background:#0a3d8a;}");
    for (int i = 0; i < 6; ++i) {
        btnMinus[i]->setStyleSheet(btnStyle);
        btnPlus[i]->setStyleSheet(btnStyle);
        // 松绑最大尺寸限制,让 layout 自由分配(允许它们略大或略小)
        btnMinus[i]->setMaximumSize(QSize(16777215, 16777215));  // QT 默认最大
        btnPlus[i]->setMaximumSize(QSize(16777215, 16777215));
        btnMinus[i]->setMinimumSize(QSize(44, 44));
        btnPlus[i]->setMinimumSize(QSize(44, 44));
    }

    // 编辑状态：每轴记录最后编辑时间，60 秒后自动恢复实时刷新
    QVector<qint64> lastEditMs(6, 0);   // QDateTime::currentMSecsSinceEpoch()
    const qint64 kEditTimeoutMs = 60000; // 60 秒

    // 逐轴判断是否处于编辑态
    auto isEditing = [&lastEditMs, spins](int i) -> bool {
        if (spins[i]->hasFocus()) return true;
        return lastEditMs[i] > 0 &&
               (QDateTime::currentMSecsSinceEpoch() - lastEditMs[i]) < kEditTimeoutMs;
    };

    // 刷新全部轴显示(跳过编辑中的轴)
    // 注意:刷新按"当前实际模式"决定显示关节坐标还是笛卡尔坐标,
    //      这样用户在弹窗内切模式后,SpinBox 会自动跟着切换显示内容。
    auto refreshPositions = [this, spins, &lastEditMs, &isEditing] {
        if (!m_robotController || !m_robotController->isConnected()) return;
        if (m_robotController->isCartesianMode()) {
            // 逆解模式:显示笛卡尔坐标
            QList<double> pos = m_robotController->getState()->getAllCartesianPositions();
            for (int i = 0; i < 6 && i < pos.size(); ++i) {
                if (isEditing(i)) continue;
                double display = (i >= 3) ? normalizeAngle180(pos[i]) : pos[i];
                spins[i]->blockSignals(true);
                spins[i]->setText(QString::number(display, 'f', 3));
                spins[i]->blockSignals(false);
            }
        } else {
            // 正解模式:显示关节角度
            QList<double> pos = m_robotController->getState()->getAllAxisPositions();
            for (int i = 0; i < 6 && i < pos.size(); ++i) {
                if (isEditing(i)) continue;
                spins[i]->blockSignals(true);
                spins[i]->setText(QString::number(pos[i], 'f', 3));
                spins[i]->blockSignals(false);
            }
        }
    };

    // 清除所有编辑状态（恢复全部实时刷新）
    auto resetEditState = [&lastEditMs] {
        for (int i = 0; i < 6; ++i) lastEditMs[i] = 0;
    };

    // 模式切换按钮连接(正解 btnModeJoint / 逆解 btnModeCart)已在前面 connect 完成

    // ===== 实时刷新定时器（200ms） =====
    QTimer timer(&dlg);
    timer.setInterval(200);
    QObject::connect(&timer, &QTimer::timeout, &dlg, refreshPositions);
    timer.start();

    // ===== 手动编辑检测 =====
    // ★ 使用内部 QLineEdit::textEdited 检测用户输入（通过 findChild 获取）
    for (int i = 0; i < 6; ++i) {
        QObject::connect(spins[i], &QLineEdit::textEdited, &dlg, [&lastEditMs, i] {
            lastEditMs[i] = QDateTime::currentMSecsSinceEpoch();
        });
    }

    // ===== [-]/[+] 按钮:驱动机器人 ± 步长(非阻塞 jog,立即返回) =====
    // 每次按下实时查询当前模式:在正解下点 -> jogJoint;在逆解下点 -> jogCart
    for (int i = 0; i < 6; ++i) {
        connect(btnMinus[i], &QPushButton::clicked, &dlg, [this, spins, step, i, &dlg, &lastEditMs] {
            if (!m_robotController || !m_robotController->isConnected()) {
                QMessageBox::warning(&dlg, QStringLiteral("提示"), QStringLiteral("请先连接控制器"));
                return;
            }
            if (m_robotController->isCartesianMode()) {
                // 逆解模式:取笛卡尔坐标,减步长
                QList<double> pos = m_robotController->getState()->getAllCartesianPositions();
                if (pos.size() <= i) return;
                pos[i] -= step;
                m_robotController->jogCart(pos,
                    this->ui->debugSpeed->text().toDouble(),
                    this->ui->debugAccel->text().toDouble(),
                    this->ui->debugDecel->text().toDouble());
            } else {
                // 正解模式:取关节角度,减步长
                QList<double> pos = m_robotController->getState()->getAllAxisPositions();
                if (pos.size() <= i) return;
                m_robotController->jogJoint(i, pos[i] - step,
                    this->ui->debugSpeed->text().toDouble(),
                    this->ui->debugAccel->text().toDouble(),
                    this->ui->debugDecel->text().toDouble());
            }
            lastEditMs[i] = 0;
        });
    }

    for (int i = 0; i < 6; ++i) {
        connect(btnPlus[i], &QPushButton::clicked, &dlg, [this, spins, step, i, &dlg, &lastEditMs] {
            if (!m_robotController || !m_robotController->isConnected()) {
                QMessageBox::warning(&dlg, QStringLiteral("提示"), QStringLiteral("请先连接控制器"));
                return;
            }
            if (m_robotController->isCartesianMode()) {
                // 逆解模式:取笛卡尔坐标,加步长
                QList<double> pos = m_robotController->getState()->getAllCartesianPositions();
                if (pos.size() <= i) return;
                pos[i] += step;
                m_robotController->jogCart(pos,
                    this->ui->debugSpeed->text().toDouble(),
                    this->ui->debugAccel->text().toDouble(),
                    this->ui->debugDecel->text().toDouble());
            } else {
                // 正解模式:取关节角度,加步长
                QList<double> pos = m_robotController->getState()->getAllAxisPositions();
                if (pos.size() <= i) return;
                m_robotController->jogJoint(i, pos[i] + step,
                    this->ui->debugSpeed->text().toDouble(),
                    this->ui->debugAccel->text().toDouble(),
                    this->ui->debugDecel->text().toDouble());
            }
            lastEditMs[i] = 0;
        });
    }

    // ===== "获取当前点位":插入 MOVEJ/MOVEL 指令到编程页 =====
    // 根据当前模式选 J 还是 L,同时强制刷新 SpinBox 显示
    connect(ui.btnGetPoint, &QPushButton::clicked, &dlg, [this, resetEditState, refreshPositions] {
        resetEditState();
        refreshPositions();

        // 按当前模式插入对应指令到编程页
        if (!m_uiController || !m_robotController || !m_robotController->isConnected()) {
            this->ui->statusbar->showMessage(QStringLiteral("已刷新,未连接控制器,无法插入指令"));
            return;
        }
        const bool isCart = m_robotController->isCartesianMode();
        if (isCart) {
            m_uiController->onProgInsertMoveL();
            this->ui->statusbar->showMessage(QStringLiteral("已插入 MOVEL 指令"));
        } else {
            m_uiController->onProgInsertMoveJ();
            this->ui->statusbar->showMessage(QStringLiteral("已插入 MOVEJ 指令"));
        }
    });

    // ===== "运行":按弹窗数值执行运动 → 恢复实时刷新 =====
    // 根据"当前实际模式"决定调 moveJ 还是 moveL
    connect(ui.btnRun, &QPushButton::clicked, &dlg, [this, spins, resetEditState, &dlg] {
        if (!m_robotController || !m_robotController->isConnected()) {
            QMessageBox::warning(&dlg, QStringLiteral("提示"), QStringLiteral("请先连接控制器"));
            return;
        }
        QList<double> vals;
        for (int i = 0; i < 6; ++i)
            vals.append(spins[i]->text().toDouble());

        const bool isCart = m_robotController->isCartesianMode();
        bool ok = isCart
            ? m_robotController->moveL(vals,
                this->ui->debugSpeed->text().toDouble(),
                this->ui->debugAccel->text().toDouble(),
                this->ui->debugDecel->text().toDouble(), 3000)
            : m_robotController->moveJ(vals,
                this->ui->debugSpeed->text().toDouble(),
                this->ui->debugAccel->text().toDouble(),
                this->ui->debugDecel->text().toDouble(), 3000);

        resetEditState();  // 运行后恢复实时刷新

        if (ok) {
            QMessageBox::information(&dlg, QStringLiteral("运行"),
                isCart ? QStringLiteral("笛卡尔运动完成") : QStringLiteral("关节运动完成"));
        } else {
            QMessageBox::warning(&dlg, QStringLiteral("运行失败"),
                QStringLiteral("请检查连接、目标点可达性、是否处于报警状态"));
        }
    });

    dlg.exec();
}

// ==================== 视觉系统控制 ====================

void MainWindow::onVisionStarted()
{
    qDebug() << "[MainWindow] 视觉程序已启动";
    ui->pushButton_6->setText(QStringLiteral("停止测试"));
}

void MainWindow::onVisionStopped(int exitCode)
{
    qDebug() << "[MainWindow] 视觉程序已退出，exitCode=" << exitCode;
    ui->pushButton_6->setText(QStringLiteral("功能测试"));
}

void MainWindow::onVisionError(const QString &error)
{
    qWarning() << "[MainWindow] 视觉程序错误:" << error;
    ui->pushButton_6->setText(QStringLiteral("功能测试"));
}

void MainWindow::onVisionMessage(const QString &channel, const QString &line)
{
    qDebug() << "[MainWindow] vision" << channel << ":" << line;
}

void MainWindow::onVisionMotorCommand(double angle)
{
    qDebug() << "[MainWindow] 视觉控制 J0 -> angle =" << angle;

    if (!m_robotController || !m_robotController->isConnected()) {
        qWarning() << "[MainWindow] 视觉 J0 指令未执行：控制器未连接";
        return;
    }

    // 连续相同值消抖：需相同值连续出现 N 次才有效
    if (qAbs(angle - m_lastVisionAngle) < 1e-6) {
        m_visionConsecutiveCount++;
    } else {
        m_lastVisionAngle = angle;
        m_visionConsecutiveCount = 1;
        return;
    }

    if (m_visionConsecutiveCount < kVisionConfirmCount) {
        return;
    }

    // 达到确认次数后，首次立即下发，后续加 500ms 间隔
    if (m_visionConsecutiveCount == kVisionConfirmCount) {
        m_visionThrottleTimer.start();
        m_robotController->jogJoint(0, angle, 25.0, 50.0, 60.0);
        return;
    }

    if (m_visionThrottleTimer.elapsed() < 500) {
        return;
    }

    m_visionThrottleTimer.restart();
    m_robotController->jogJoint(0, angle, 25.0, 50.0, 60.0);
}

