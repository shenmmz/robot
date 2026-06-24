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
#include "src/logic/RobotController.h"
#include "src/logic/UiController.h"
#include "src/hardware/Rs485Controller.h"
#include "src/models/RobotConfig.h"
#include "src/models/RobotKinematics.h"
#include "library/zmotion.h"
#include "src/utils/MathUtils.h"
#include "src/utils/CodeEditor.h"
#include "library/zauxdll2.h"
#include <QThread>
#include <QTimer>
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
    if (m_agingTimer) m_agingTimer->stop();
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
}

/**
 * @brief 断开连接回调
 *
 * 更新状态为"未连接"，恢复所有操作按钮的可用性。
 */
void MainWindow::onHardwareDisconnected()
{
    // TODO: 暂时注释掉舵机控制器
    // // 停止舵机 PWM 输出
    // if (m_servo) {
    //     m_servo->stopAll();
    // }

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


/**
 * @brief 启动老化测试
 *
 * 在 3 个预设关节目标之间循环执行 MoveJ/MoveL，
 * 支持真实硬件和模拟器自动检测切换。
 */
void MainWindow::on_pushButton_5_clicked()
{
    if (!m_robotController || !m_robotController->isConnected()) {
        ui->statusbar->showMessage(QStringLiteral("请先连接控制器"));
        return;
    }

    ui->pushButton_5->setEnabled(false);
    setConnectionStatus(QStringLiteral("老化测试中..."), QStringLiteral("orange"));

    m_agingJointTargets = {
        {10.0,   0.0,   0.0,   0.0, 90.0,   0.0},
        {30.0,  20.0, -10.0,  10.0, 90.0,   0.0},
        {90.0, -20.0, -50.0,  30.0, 90.0,   0.0},
    };
    m_agingCycle = 1;
    m_agingTargetIdx = 0;
    m_agingStep = AgingStep::MoveJ;
    m_agingSimMode = false;

    if (!m_agingTimer) {
        m_agingTimer = new QTimer(this);
        connect(m_agingTimer, &QTimer::timeout, this, &MainWindow::onAgingTick);
    }
    m_agingTimer->start(50);
}

/**
 * @brief 老化测试定时器回调（每 50ms）
 *
 * 循环执行 MoveJ → MoveL 验证正逆解往返：
 * - 检测到轴不动时自动切换模拟器模式
 * - 500 轮后自动结束
 */
void MainWindow::onAgingTick()
{
    constexpr double MOVE_SPEED  = 80.0;
    constexpr double MOVE_ACCEL  = 160.0;
    constexpr double MOVE_DECEL  = 180.0;
    constexpr int    MAX_CYCLES  = 500;
    // 真实硬件等待3秒, 模拟器只等200ms
    constexpr int TIMEOUT_REAL  = 3000;
    constexpr int TIMEOUT_SIM   = 200;

    int timeout = m_agingSimMode ? TIMEOUT_SIM : TIMEOUT_REAL;

    if (!m_robotController || !m_robotController->isConnected()) {
        finishAgingTest(false, QStringLiteral("控制器已断开"));
        return;
    }

    if (m_agingCycle > MAX_CYCLES) {
        finishAgingTest(true, QStringLiteral("老化测试完成"));
        return;
    }

    if (m_agingSimMode) {
        // 模拟器模式：快速轮转，不实际等待moveJ/moveL
        if (m_agingStep == AgingStep::MoveL) {
            m_agingTargetIdx++;
            if (m_agingTargetIdx >= m_agingJointTargets.size()) {
                m_agingTargetIdx = 0;
                m_agingCycle++;
            }
        }
        m_agingStep = (m_agingStep == AgingStep::MoveJ) ? AgingStep::MoveL : AgingStep::MoveJ;
        ui->statusbar->showMessage(
            QStringLiteral("老化测试: 第%1/%2轮 (模拟器)").arg(m_agingCycle).arg(MAX_CYCLES));
        return;
    }

    const auto& jt = m_agingJointTargets[m_agingTargetIdx];

    if (m_agingStep == AgingStep::MoveJ) {
        QList<double> before = m_robotController->getState()->getAllAxisPositions();
        m_robotController->switchToJointMode();
        if (!m_robotController->moveJ(jt, MOVE_SPEED, MOVE_ACCEL, MOVE_DECEL, timeout)) {
            QList<double> after = m_robotController->getState()->getAllAxisPositions();
            if (before == after) {
                m_agingSimMode = true;
                qWarning() << "[Aging] 检测到模拟器模式";

                // 立即进一次模拟器轮转
                m_agingStep = AgingStep::MoveL;
            } else {
                finishAgingTest(false, QStringLiteral("moveJ 失败 (第%1轮, 目标%2)")
                    .arg(m_agingCycle).arg(m_agingTargetIdx + 1));
                return;
            }
        } else {
            m_agingStep = AgingStep::MoveL;
        }
    } else {
        // 真实硬件: 读完整笛卡尔坐标 → moveL验证FK/IK往返
        QList<double> cart = m_robotController->getState()->getAllCartesianPositions();
        m_robotController->switchToCartesianMode();
        if (!m_robotController->moveL(cart, MOVE_SPEED, MOVE_ACCEL, MOVE_DECEL, timeout)) {
            finishAgingTest(false, QStringLiteral("moveL 失败 (第%1轮, 目标%2)")
                .arg(m_agingCycle).arg(m_agingTargetIdx + 1));
            return;
        }

        m_agingTargetIdx++;
        if (m_agingTargetIdx >= m_agingJointTargets.size()) {
            m_agingTargetIdx = 0;
            m_agingCycle++;
        }
        m_agingStep = AgingStep::MoveJ;
    }

    ui->statusbar->showMessage(
        QStringLiteral("老化测试: 第%1/%2轮").arg(m_agingCycle).arg(MAX_CYCLES));
}

/**
 * @brief 结束老化测试
 *
 * @param success true=正常完成，false=异常终止
 * @param message 结束原因描述
 */
void MainWindow::finishAgingTest(bool success, const QString& message)
{
    m_agingTimer->stop();
    setConnectionStatus(success ? QStringLiteral("老化测试完成") : QStringLiteral("老化测试失败"),
                      success ? QStringLiteral("green") : QStringLiteral("red"));
    ui->statusbar->showMessage(message);
    ui->pushButton_5->setEnabled(true);
}


/**
 * @brief 运行预设测试点位
 */
void MainWindow::on_pushButton_6_clicked()
{
    m_uiController->runTestPoints();
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

    if (m_agingTimer && m_agingTimer->isActive()) {
        finishAgingTest(false, QStringLiteral("用户已停止老化测试"));
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
    connect(ui->btnApplyJoint, &QPushButton::clicked, this, &MainWindow::onSendJointMove);
    connect(ui->btnApplyCart, &QPushButton::clicked, this, &MainWindow::onSendCartMove);

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
    QDoubleSpinBox* jSpins[] = {ui->spinManualJ0, ui->spinManualJ1, ui->spinManualJ2, ui->spinManualJ3, ui->spinManualJ4, ui->spinManualJ5};
    QDoubleSpinBox* cSpins[] = {ui->spinManualX, ui->spinManualY, ui->spinManualZ, ui->spinManualRx, ui->spinManualRy, ui->spinManualRz};

    bool isCart = m_robotController->isCartesianMode();

    QString jStyle = isCart
        ? QStringLiteral("font-family:Consolas,'Microsoft YaHei UI';font-size:13px;padding:2px 6px;color:#999;background:#f5f5f5;border:1px solid #e8e8e8;border-radius:4px;")
        : QStringLiteral("font-family:Consolas,'Microsoft YaHei UI';font-size:13px;padding:2px 6px;color:#333;background:#f5f5f5;border:1px solid #1565c0;border-radius:4px;");
    QString cStyle = isCart
        ? QStringLiteral("font-family:Consolas,'Microsoft YaHei UI';font-size:13px;padding:2px 6px;color:#0078d4;background:#f0f4ff;border:1px solid #1565c0;border-radius:4px;")
        : QStringLiteral("font-family:Consolas,'Microsoft YaHei UI';font-size:13px;padding:2px 6px;color:#999;background:#f5f5f5;border:1px solid #e8e8e8;border-radius:4px;");

    // 始终实时刷新，用户正在手动输入时暂停刷新该组
    for (int i = 0; i < 6 && i < joints.size(); ++i) {
        jLabels[i]->setText(QString::number(joints[i], 'f', 3));
        jLabels[i]->setStyleSheet(jStyle);

        if (!m_jogJointUserInput) {
            jSpins[i]->blockSignals(true);
            jSpins[i]->setValue(joints[i]);
            jSpins[i]->blockSignals(false);
        }

        if (i < carts.size()) {
            double display = carts[i];
            if (i >= 3) display = normalizeAngle180(display);
            cLabels[i]->setText(QString::number(display, 'f', 3));
            cLabels[i]->setStyleSheet(cStyle);

            if (!m_jogCartUserInput) {
                cSpins[i]->blockSignals(true);
                cSpins[i]->setValue(display);
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
        ui->spinManualJ0->value(), ui->spinManualJ1->value(), ui->spinManualJ2->value(),
        ui->spinManualJ3->value(), ui->spinManualJ4->value(), ui->spinManualJ5->value()
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
        ui->spinManualX->value(), ui->spinManualY->value(), ui->spinManualZ->value(),
        ui->spinManualRx->value(), ui->spinManualRy->value(), ui->spinManualRz->value()
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

        int interval = ui->rs485SpinInterval->value();
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
    instrTabs->setMinimumHeight(44);
    instrTabs->setMaximumHeight(50);
    instrTabs->setStyleSheet(
        QStringLiteral(
            "QTabWidget::pane{border:1px solid #d0d5dd;border-radius:0 6px 6px 6px;background:#fff;}"
            "QTabBar::tab{background:#f5f5f5;color:#666;padding:2px 12px;"
            "border:1px solid #d0d5dd;border-bottom:none;border-radius:4px 4px 0 0;"
            "margin-right:2px;font-size:11px;font-weight:bold;}"
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
        lay->setContentsMargins(6, 2, 6, 2); lay->setSpacing(4);
        migrateBtn(ui->btnProgMoveJ, lay);    ui->btnProgMoveJ->setText(QStringLiteral("关节"));
        migrateBtn(ui->btnProgMoveL, lay);    ui->btnProgMoveL->setText(QStringLiteral("直线"));
        migrateBtn(ui->btnProgMoveOff, lay);  ui->btnProgMoveOff->setText(QStringLiteral("关节偏移"));
        migrateBtn(ui->btnProgMoveLOff, lay); ui->btnProgMoveLOff->setText(QStringLiteral("直线偏移"));
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
        lay->setContentsMargins(6, 2, 6, 2); lay->setSpacing(4);
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
        lay->addStretch();
        instrTabs->addTab(tab, QStringLiteral("  逻辑  "));
    }

    // Tab 2: 工具
    {
        QWidget* tab = new QWidget();
        QHBoxLayout* lay = new QHBoxLayout(tab);
        lay->setContentsMargins(6, 2, 6, 2); lay->setSpacing(4);
        migrateBtn(ui->btnProgPrint, lay);   ui->btnProgPrint->setText(QStringLiteral("打印"));
        migrateBtn(ui->btnProgReadPos, lay); ui->btnProgReadPos->setText(QStringLiteral("读位置"));
        migrateBtn(ui->btnProgCall, lay);    ui->btnProgCall->setText(QStringLiteral("调用"));
        migrateBtn(ui->btnProgFor, lay);     ui->btnProgFor->setText(QStringLiteral("FOR循环"));
        migrateBtn(ui->btnProgComment, lay); ui->btnProgComment->setText(QStringLiteral("注释"));
        // SERVO 单舵机按钮
        QPushButton* btnServo = new QPushButton(QStringLiteral("SERVO"));
        btnServo->setMinimumSize(42, 24);
        btnServo->setStyleSheet(
            QStringLiteral("QPushButton{background:#e3f2fd;color:#1565c0;border:1px solid #90caf9;border-radius:3px;"
                "padding:2px 4px;font-weight:bold;font-size:10px;font-family:Consolas;}"
                "QPushButton:hover{background:#bbdefb;}"));
        connect(btnServo, &QPushButton::clicked, this, [this] {
            if (m_uiController) m_uiController->onProgInsertServo();
        });
        lay->addWidget(btnServo);
        // 注（2026-06-15）：原 SERVO_S 多舵机按钮已删除，SERVR_S 语法并入 SERVO，
        // 通过 SERVO(0=..., 1=..., 2=...) 即可同时控制多路。
        // WHILE TRUE 循环按钮
        QPushButton* btnWhile = new QPushButton(QStringLiteral("WHILE循环"));
        btnWhile->setMinimumSize(52, 24);
        btnWhile->setStyleSheet(
            QStringLiteral("QPushButton{background:#fce4ec;color:#c62828;border:1px solid #ef9a9a;border-radius:3px;"
                "padding:2px 4px;font-weight:bold;font-size:10px;font-family:Consolas;}"
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

        // 紧凑: [调试模式] 一行，下方4个参数纵向排列，每个带±按钮
        ui->chkDebugMode->setText(QStringLiteral("调试模式"));
        ui->chkDebugMode->setStyleSheet(
            QStringLiteral("QCheckBox{color:#333;font-size:11px;spacing:3px;font-weight:bold;}"));

        QGridLayout* debugGrid = new QGridLayout();
        debugGrid->setSpacing(2);
        debugGrid->setContentsMargins(0, 0, 0, 0);

        auto compactLabel = [](QLabel* lbl) {
            lbl->setMinimumWidth(0); lbl->setMaximumWidth(40);
            lbl->setStyleSheet(QStringLiteral("color:#666;font-size:11px;border:none;background:transparent;"));
        };
        auto compactSpin = [](QDoubleSpinBox* sp) {
            sp->setFixedHeight(22);
            sp->setMinimumWidth(55);
            sp->setMaximumWidth(70);
            sp->setButtonSymbols(QAbstractSpinBox::NoButtons);
            sp->setStyleSheet(QStringLiteral("QDoubleSpinBox{border:1px solid #d0d5dd;border-radius:3px;padding:1px 2px;font-size:11px;}"));
        };
        auto makeDebugBtn = [](const QString& text, const QString& color) -> QPushButton* {
            auto* btn = new QPushButton(text);
            btn->setFixedSize(18, 18);
            btn->setStyleSheet(
                QStringLiteral("QPushButton{background:#f5f5f5;color:%1;border:1px solid #e0e0e0;"
                    "border-radius:3px;font-weight:bold;font-size:12px;}"
                    "QPushButton:hover{background:%2;border-color:%3;}")
                    .arg(color, color == QStringLiteral("#c62828")
                        ? QStringLiteral("#ffebee") : QStringLiteral("#e3f2fd"),
                        color == QStringLiteral("#c62828")
                        ? QStringLiteral("#ef9a9a") : QStringLiteral("#90caf9")));
            return btn;
        };

        compactLabel(ui->label_15); compactLabel(ui->label_16);
        compactLabel(ui->label_17); compactLabel(ui->label_18);
        ui->label_15->setText(QStringLiteral("步长"));
        ui->label_16->setText(QStringLiteral("速度"));
        ui->label_17->setText(QStringLiteral("加速度"));
        ui->label_18->setText(QStringLiteral("减速度"));

        struct DebugRow {
            QLabel* label;
            QDoubleSpinBox* spin;
            QPushButton *btnM, *btnP;
        };
        QList<DebugRow> debugRows = {
            {ui->label_15, ui->debugStep,  nullptr, nullptr},
            {ui->label_16, ui->debugSpeed, nullptr, nullptr},
            {ui->label_17, ui->debugAccel, nullptr, nullptr},
            {ui->label_18, ui->debugDecel, nullptr, nullptr},
        };
        // 去掉 UI 文件中预设的 suffix
        ui->debugStep->setSuffix(QString());
        ui->debugSpeed->setSuffix(QString());
        ui->debugAccel->setSuffix(QString());
        ui->debugDecel->setSuffix(QString());

        // 单位标签（放在 spinbox 后面）
        const QString debUnits[] = {QStringLiteral("mm或°"), QStringLiteral("mm/s"), QStringLiteral("mm/s²"), QStringLiteral("mm/s²")};
        QLabel* unitLabels[4];
        for (int i = 0; i < 4; ++i) {
            unitLabels[i] = new QLabel(debUnits[i]);
            unitLabels[i]->setStyleSheet(QStringLiteral("color:#999;font-size:10px;border:none;background:transparent;"));
        }
        for (auto& r : debugRows) {
            compactSpin(r.spin);
            r.btnM = makeDebugBtn(QStringLiteral("－"), QStringLiteral("#c62828"));
            r.btnP = makeDebugBtn(QStringLiteral("＋"), QStringLiteral("#1565c0"));
            double step = (r.spin == ui->debugStep) ? 0.1 : 1.0;
            connect(r.btnM, &QPushButton::clicked, r.spin,
                [r, step] { r.spin->setValue(r.spin->value() - step); });
            connect(r.btnP, &QPushButton::clicked, r.spin,
                [r, step] { r.spin->setValue(r.spin->value() + step); });
        }

        // 每行: 标签 | － | spinbox | 单位 | ＋
        for (int i = 0; i < debugRows.size(); ++i) {
            debugGrid->addWidget(debugRows[i].label, i, 0);
            debugGrid->addWidget(debugRows[i].btnM,  i, 1);
            debugGrid->addWidget(debugRows[i].spin,  i, 2);
            debugGrid->addWidget(unitLabels[i],     i, 3);
            debugGrid->addWidget(debugRows[i].btnP,  i, 4);
        }

        debugVLay->addLayout(debugGrid);
    }

    // 收紧调试组，和点动组 style 统一
    ui->jogDebugGroup->setStyleSheet(
        QStringLiteral(
            "QGroupBox{background:#ffffff;border:1px solid #d0d5dd;border-radius:6px;"
            "margin-top:10px;padding:4px 6px 4px 6px;font-weight:bold;font-size:12px;color:#1a1a2e;}"
            "QGroupBox::title{subcontrol-origin:margin;left:12px;padding:0 4px;}"
        ));

    // ==================== 4. 关节点动 & 笛卡尔点动 → 紧凑 ====================
    auto compactJogGroup = [](QGroupBox* group) {
        group->setStyleSheet(
            QStringLiteral(
                "QGroupBox{background:#ffffff;border:1px solid #d0d5dd;border-radius:6px;"
                "margin-top:10px;padding:4px 6px 4px 6px;font-weight:bold;font-size:12px;color:#1a1a2e;}"
                "QGroupBox::title{subcontrol-origin:margin;left:12px;padding:0 4px;}"
            ));
        QVBoxLayout* vlay = qobject_cast<QVBoxLayout*>(group->layout());
        if (vlay) vlay->setSpacing(1);

        const QString btnMinusStyle = QStringLiteral(
            "QPushButton{background:#f5f5f5;color:#c62828;border:1px solid #e0e0e0;border-radius:3px;"
            "font-weight:bold;font-size:12px;}QPushButton:hover{background:#ffebee;border-color:#ef9a9a;}");
        const QString btnPlusStyle = QStringLiteral(
            "QPushButton{background:#f5f5f5;color:#1565c0;border:1px solid #e0e0e0;border-radius:3px;"
            "font-weight:bold;font-size:12px;}QPushButton:hover{background:#e3f2fd;border-color:#90caf9;}");

        for (QPushButton* btn : group->findChildren<QPushButton*>()) {
            QString t = btn->text();
            if (t == QString::fromWCharArray(L"\xff0d")) {
                btn->setStyleSheet(btnMinusStyle);
                btn->setFixedSize(20, 20);
            } else if (t == QString::fromWCharArray(L"\xff0b")) {
                btn->setStyleSheet(btnPlusStyle);
                btn->setFixedSize(20, 20);
            } else if (t.contains(QStringLiteral("运行"))) {
                btn->setMinimumSize(0, 20);
                btn->setMaximumSize(16777215, 20);
                btn->setStyleSheet(
                    QStringLiteral("QPushButton{background:#2e7d32;color:#fff;border:none;border-radius:3px;"
                        "font-size:10px;font-weight:bold;}QPushButton:hover{background:#388e3c;}"));
            }
        }
        for (QDoubleSpinBox* sp : group->findChildren<QDoubleSpinBox*>()) {
            sp->setFixedHeight(22);
            sp->setMinimumWidth(65);
            sp->setMaximumWidth(85);
            sp->setButtonSymbols(QAbstractSpinBox::NoButtons);
            sp->setStyleSheet(
                QStringLiteral("QDoubleSpinBox{font-family:Consolas,'Microsoft YaHei UI';font-size:11px;"
                    "padding:1px 2px;border:1px solid #d0d5dd;border-radius:3px;background:#fafbfc;}"
                    "QDoubleSpinBox:focus{border-color:#1565c0;background:#fff;}"));
        }

        // 轴行: 收紧标签样式和间距
        for (QHBoxLayout* row : group->findChildren<QHBoxLayout*>()) {
            row->setSpacing(2);
            if (row->count() < 3) continue;
            QWidget* first = row->itemAt(0) ? row->itemAt(0)->widget() : nullptr;
            auto* lbl = qobject_cast<QLabel*>(first);
            if (!lbl) continue;
            lbl->setMinimumWidth(18);
            lbl->setMaximumWidth(22);
            lbl->setStyleSheet(
                QStringLiteral("color:#888;font-size:11px;font-weight:bold;border:none;background:transparent;"));
        }
    };

    compactJogGroup(ui->jogJointGroup);
    compactJogGroup(ui->jogCartGroup);

    // 调试模式控制点动可用性：不勾选→全部灰色不可调
    auto setJogEnabled = [this](bool enabled) {
        for (QGroupBox* g : {ui->jogJointGroup, ui->jogCartGroup}) {
            for (QPushButton* btn : g->findChildren<QPushButton*>()) {
                QString t = btn->text();
                if (t == QString::fromWCharArray(L"\xff0d") || t == QString::fromWCharArray(L"\xff0b"))
                    btn->setEnabled(enabled);
            }
            for (QDoubleSpinBox* sp : g->findChildren<QDoubleSpinBox*>())
                sp->setReadOnly(!enabled);
        }
    };
    setJogEnabled(ui->chkDebugMode->isChecked());
    connect(ui->chkDebugMode, &QCheckBox::toggled, this, setJogEnabled);

    // 重新绑定按钮逻辑
    auto rebindJogButtons = [this](QGroupBox* group, bool isJoint) {
        auto* applyBtn = group->findChild<QPushButton*>("btnApplyJoint");
        if (!applyBtn) applyBtn = group->findChild<QPushButton*>("btnApplyCart");
        QVBoxLayout* vlay = qobject_cast<QVBoxLayout*>(group->layout());
        if (!vlay || !applyBtn) return;

        bool* userInputFlag = isJoint ? &m_jogJointUserInput : &m_jogCartUserInput;

        QTimer* resetTimer = new QTimer(this);
        resetTimer->setSingleShot(true);
        resetTimer->setInterval(5000);
        if (isJoint) m_jogResetTimerJoint = resetTimer;
        else         m_jogResetTimerCart  = resetTimer;

        // 恢复刷新：仅清 userInputFlag，让 100ms 周期的 updatePositionDisplay 自然接管刷新
        // 不再覆写 spin 值——避免用户改了 500 却在 5 秒后被偷偷改回当前值的困惑
        auto resumeRefresh = [this, isJoint, resetTimer, userInputFlag] {
            resetTimer->stop();
            *userInputFlag = false;
        };
        connect(resetTimer, &QTimer::timeout, this, resumeRefresh);

        // 用户手动改值 → 暂停刷新 + 启动5s倒计时
        for (auto* sp : group->findChildren<QDoubleSpinBox*>()) {
            connect(sp, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
                [resetTimer, userInputFlag] { *userInputFlag = true; resetTimer->start(); });
        }

        // 设置按钮文本和统一样式
        applyBtn->setText(isJoint ? QStringLiteral("获取MOVEJ") : QStringLiteral("获取MOVEL"));
        applyBtn->setMinimumSize(0, 20);
        applyBtn->setMaximumSize(16777215, 20);
        applyBtn->setStyleSheet(
            QStringLiteral("QPushButton{background:#1565c0;color:#fff;border:none;border-radius:3px;"
                "font-size:10px;font-weight:bold;}QPushButton:hover{background:#1976d2;}"));

        vlay->removeWidget(applyBtn);
        applyBtn->disconnect();
        connect(applyBtn, &QPushButton::clicked, this, [this, isJoint, resetTimer, userInputFlag] {
            // "获取MOVEJ/MOVEL"按钮:复位 timer + 标志位 + 显式覆写 spin 为当前位姿(用 blockSignals 防止触发新一轮 5 秒倒计时)
            resetTimer->stop();
            *userInputFlag = false;
            QDoubleSpinBox* jSpins[] = {ui->spinManualJ0, ui->spinManualJ1, ui->spinManualJ2, ui->spinManualJ3, ui->spinManualJ4, ui->spinManualJ5};
            QDoubleSpinBox* cSpins[] = {ui->spinManualX, ui->spinManualY, ui->spinManualZ, ui->spinManualRx, ui->spinManualRy, ui->spinManualRz};
            if (m_robotController && m_robotController->getState()) {
                if (isJoint) {
                    QList<double> pos = m_robotController->getState()->getAllAxisPositions();
                    for (int i = 0; i < 6 && i < pos.size(); ++i) {
                        jSpins[i]->blockSignals(true);
                        jSpins[i]->setValue(pos[i]);
                        jSpins[i]->blockSignals(false);
                    }
                } else {
                    QList<double> pos = m_robotController->getState()->getAllCartesianPositions();
                    for (int i = 0; i < 6 && i < pos.size(); ++i) {
                        double display = (i >= 3) ? normalizeAngle180(pos[i]) : pos[i];
                        cSpins[i]->blockSignals(true);
                        cSpins[i]->setValue(display);
                        cSpins[i]->blockSignals(false);
                    }
                }
            }
            // 同时插入指令到编程编辑器
            if (isJoint)
                m_uiController->onProgInsertMoveJ();
            else
                m_uiController->onProgInsertMoveL();
        });

        // "运行" → 恢复刷新 + 执行运动
        QPushButton* runBtn = new QPushButton(QStringLiteral("运行"));
        runBtn->setMinimumSize(0, 20);
        runBtn->setMaximumSize(16777215, 20);
        runBtn->setStyleSheet(
            QStringLiteral("QPushButton{background:#2e7d32;color:#fff;border:none;border-radius:3px;"
                "font-size:10px;font-weight:bold;}QPushButton:hover{background:#388e3c;}"));
        connect(runBtn, &QPushButton::clicked, this, [this, isJoint, userInputFlag, resetTimer] {
            resetTimer->stop();
            *userInputFlag = false;
            if (isJoint) onSendJointMove();
            else         onSendCartMove();
        });

        QHBoxLayout* btnRow = new QHBoxLayout();
        btnRow->setSpacing(4);
        btnRow->addWidget(applyBtn);
        btnRow->addWidget(runBtn);
        vlay->addLayout(btnRow);
    };

    rebindJogButtons(ui->jogJointGroup, true);
    rebindJogButtons(ui->jogCartGroup, false);

    // 左侧栏: 手动调试 + 关节点动 + 笛卡尔点动 上下排列
    QWidget* leftPanel = new QWidget();
    QVBoxLayout* leftLay = new QVBoxLayout(leftPanel);
    leftLay->setContentsMargins(0, 0, 8, 0);
    leftLay->setSpacing(4);

    // 从 jogHLayout 中取出两个 groupbox
    QHBoxLayout* jogHLay = qobject_cast<QHBoxLayout*>(jogHItem->layout());
    if (jogHLay) {
        jogHLay->removeWidget(ui->jogJointGroup);
        jogHLay->removeWidget(ui->jogCartGroup);
        while (jogHLay->count() > 0) delete jogHLay->takeAt(0);
    }
    delete jogHItem;

    leftLay->addWidget(ui->jogDebugGroup);   // 手动调试
    leftLay->addWidget(ui->jogJointGroup);   // 关节点动
    leftLay->addWidget(ui->jogCartGroup);    // 笛卡尔点动

    // 舵机板块：2 列 3 行紧凑布局，每格只显示 "CHx:  90.0°"（被动显示）
    // 控制全部在"舵机设置"弹窗内完成
    QGroupBox* servoGroup = new QGroupBox(QStringLiteral("舵机"), leftPanel);
    servoGroup->setStyleSheet(
        QStringLiteral(
            "QGroupBox{background:#ffffff;border:1px solid #d0d5dd;border-radius:6px;"
            "margin-top:10px;padding:4px 6px 4px 6px;font-weight:bold;font-size:12px;color:#1a1a2e;}"
            "QGroupBox::title{subcontrol-origin:margin;left:12px;padding:0 4px;}"
        ));
    QVBoxLayout* servoLay = new QVBoxLayout();
    servoLay->setSpacing(4);
    servoLay->setContentsMargins(2, 2, 2, 2);

    m_servoAngleLabels.clear();
    QGridLayout* grid = new QGridLayout();
    grid->setSpacing(6);
    grid->setContentsMargins(4, 4, 4, 4);
    for (int i = 0; i < 6; ++i) {
        const int row = i / 2;
        const int col = i % 2;

        QHBoxLayout* cell = new QHBoxLayout();
        cell->setSpacing(4);

        QLabel* lbl = new QLabel(QStringLiteral("CH%1:").arg(i));
        lbl->setFixedWidth(34);
        lbl->setStyleSheet(
            QStringLiteral("color:#888;font-size:11px;font-weight:bold;border:none;background:transparent;"));

        QLabel* angleLbl = new QLabel(QStringLiteral("90.0°"));
        angleLbl->setAlignment(Qt::AlignCenter);
        angleLbl->setMinimumWidth(70);
        angleLbl->setStyleSheet(
            QStringLiteral(
                "QLabel{font-family:Consolas,'Microsoft YaHei UI';font-size:12px;"
                "color:#1565c0;font-weight:bold;background:#fafbfc;"
                "border:1px solid #d0d5dd;border-radius:3px;padding:1px 6px;}"
            ));
        m_servoAngleLabels.append(angleLbl);

        cell->addWidget(lbl);
        cell->addWidget(angleLbl);
        cell->addStretch();
        grid->addLayout(cell, row, col);
    }
    servoLay->addLayout(grid);

    // 订阅 ServoController::angleChanged，实时刷新手动页舵机板块角度 Label
    {
        ServoController* servo = m_robotController ? m_robotController->getServoController() : nullptr;
        if (servo) {
            connect(servo, &ServoController::angleChanged, this,
                [this](int channel, float angle) {
                    if (channel < 0 || channel >= m_servoAngleLabels.size()) return;
                    m_servoAngleLabels[channel]->setText(
                        QString("%1°").arg(angle, 0, 'f', 1));
                });
        }
    }

    servoGroup->setLayout(servoLay);

    // 舵机设置按钮：点击后弹出模态对话框，复用 ServoTestWidget
    QPushButton* btnServoDebug = new QPushButton(QStringLiteral("舵机设置"), servoGroup);
    btnServoDebug->setFixedHeight(26);
    btnServoDebug->setCursor(Qt::PointingHandCursor);
    btnServoDebug->setStyleSheet(
        QStringLiteral(
            "QPushButton{background:#1565c0;color:#ffffff;border:1px solid #0d47a1;"
            "border-radius:3px;font-weight:bold;font-size:12px;}"
            "QPushButton:hover{background:#0d47a1;}"
            "QPushButton:pressed{background:#0a3d8a;}"
        ));
    servoLay->addWidget(btnServoDebug);

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
        dlg->resize(720, 520);
        dlg->setAttribute(Qt::WA_DeleteOnClose);  // 关闭时自动销毁 dialog 及其子 widget

        QVBoxLayout* lay = new QVBoxLayout(dlg);
        lay->setContentsMargins(0, 0, 0, 0);

        // 复用已有的 ServoTestWidget（6通道勾选+滑块+角度+速度+应用/初始化/停止）
        ServoTestWidget* w = new ServoTestWidget(servo, dlg);
        lay->addWidget(w);

        // 记录指针以便下次点击按钮时做防重复判断
        m_servoTestWidget = w;

        dlg->exec();
        // 模态返回后 dialog 已被标记为 delete，servotest widget 也会随之销毁
        m_servoTestWidget = nullptr;
    });
    leftLay->addWidget(servoGroup);

    leftLay->addStretch();

    // 右侧栏: 编程（指令Tab + 工具栏 + 程序/点位分栏 + 控制台）
    QWidget* rightPanel = new QWidget();
    QVBoxLayout* rightLay = new QVBoxLayout(rightPanel);
    rightLay->setContentsMargins(0, 0, 0, 0);
    rightLay->setSpacing(4);
    rightLay->addWidget(instrTabs);
    rightLay->addWidget(ui->progToolBar);

    // 程序/点位水平分栏
    QWidget* progLabelRow = new QWidget();
    QHBoxLayout* progLabelLay = new QHBoxLayout(progLabelRow);
    progLabelLay->setContentsMargins(2, 0, 2, 0);
    progLabelLay->setSpacing(0);
    QLabel* lblProg = new QLabel(QStringLiteral("程序"));
    lblProg->setStyleSheet(QStringLiteral("color:#1565c0;font-size:11px;font-weight:bold;border:none;background:transparent;padding:0 4px;"));
    QLabel* lblTarget = new QLabel(QStringLiteral("点位"));
    lblTarget->setStyleSheet(QStringLiteral("color:#2e7d32;font-size:11px;font-weight:bold;border:none;background:transparent;padding:0 4px;"));
    progLabelLay->addWidget(lblProg);
    progLabelLay->addStretch();
    progLabelLay->addWidget(lblTarget);
    rightLay->addWidget(progLabelRow);

    // 新建点位编辑器（带行号）
    CodeEditor* targetEditor = new CodeEditor();
    targetEditor->setObjectName(QStringLiteral("progTargets"));
    targetEditor->setStyleSheet(
        QStringLiteral("QPlainTextEdit{background:#1e1e2e;color:#d4d4d4;border:1px solid #d0d5dd;"
            "border-radius:6px;font-family:Consolas,'Microsoft YaHei UI';font-size:13px;"
            "padding:8px;selection-background:#264f78;}"));
    targetEditor->setTabStopDistance(40.0);

    // 将 .ui 的 progEditor 替换为 CodeEditor（带行号），保留原内容
    CodeEditor* codeEditor = new CodeEditor();
    codeEditor->setObjectName(QStringLiteral("progEditor"));
    codeEditor->setStyleSheet(ui->progEditor->styleSheet());
    codeEditor->setTabStopDistance(ui->progEditor->tabStopDistance());
    codeEditor->setPlainText(ui->progEditor->toPlainText());

    // 清理旧的 ui->progEditor（已从布局移除），让 ui->progEditor 指向新的 CodeEditor
    QPlainTextEdit* oldEditor = ui->progEditor;
    ui->progEditor = codeEditor;
    delete oldEditor;

    QSplitter* hSplitter = new QSplitter(Qt::Horizontal);
    hSplitter->addWidget(codeEditor);
    hSplitter->addWidget(targetEditor);
    hSplitter->setStretchFactor(0, 3);
    hSplitter->setStretchFactor(1, 1);
    hSplitter->setChildrenCollapsible(false);

    // 编辑区和控制台放到垂直 QSplitter
    QSplitter* splitter = new QSplitter(Qt::Vertical);
    splitter->addWidget(hSplitter);
    splitter->addWidget(ui->progConsole);
    splitter->setStretchFactor(0, 3);  // 编辑器占 3
    splitter->setStretchFactor(1, 2);  // 控制台占 2
    splitter->setCollapsible(0, false);  // 编辑器不可折叠
    splitter->setCollapsible(1, false);  // 控制台不可折叠
    rightLay->addWidget(splitter, 1);

    // 清除 UI 文件限制，允许自由拖动，但控制台不能完全消失
    ui->progConsole->setMinimumHeight(30);
    ui->progConsole->setMaximumHeight(16777215);
    codeEditor->setMinimumHeight(0);

    // 加"单步"按钮到工具栏（运行/暂停右侧）
    // 跟踪最后一个聚焦的代码编辑器（用于"单步"按钮）
    auto lastFocusedEditor = std::make_shared<QPlainTextEdit*>(qobject_cast<QPlainTextEdit*>(codeEditor));
    connect(qApp, &QApplication::focusChanged, this,
        [lastFocusedEditor, targetEditor](QWidget*, QWidget* now) {
            if (auto* ed = qobject_cast<QPlainTextEdit*>(now)) *lastFocusedEditor = ed;
        });

    QPushButton* btnRunStep = new QPushButton(QStringLiteral("单步"));
    btnRunStep->setMinimumSize(50, 26);
    btnRunStep->setStyleSheet(
        QStringLiteral("QPushButton{background:#ede7f6;color:#4527a0;border:1px solid #b39ddb;border-radius:4px;"
            "padding:2px 8px;font-weight:bold;font-size:11px;}QPushButton:hover{background:#d1c4e9;}"));
    connect(btnRunStep, &QPushButton::clicked, this, [this, lastFocusedEditor, codeEditor] {
        if (!m_uiController || m_uiController->isTaskRunning()) return;
        QPlainTextEdit* src = *lastFocusedEditor;
        if (!src) src = codeEditor;
        QTextCursor cursor = src->textCursor();
        cursor.select(QTextCursor::LineUnderCursor);
        QString line = cursor.selectedText().trimmed();
        if (line.isEmpty() || line.startsWith(QChar::fromLatin1('#')) || line.startsWith(QStringLiteral("--")))
            return;
        // 清除上一个高亮
        if (m_lastHighlightedEditor)
            m_lastHighlightedEditor->setExtraSelections({});
        // setTextCursor 会触发 cursorPositionChanged→误清高亮，先置空
        m_lastHighlightedEditor = nullptr;
        src->setTextCursor(cursor);
        src->ensureCursorVisible();
        QTextEdit::ExtraSelection sel;
        sel.format.setForeground(QColor(65, 105, 225));
        sel.cursor = cursor;
        src->setExtraSelections({sel});
        m_lastHighlightedEditor = src;
        m_uiController->syncTargets();
        QString text;
        auto* targetEd = findChild<QPlainTextEdit*>(QStringLiteral("progTargets"));
        if (targetEd) text += targetEd->toPlainText() + QStringLiteral("\n");
        text += line;
        m_uiController->runText(text, line);
    });
    ui->progToolLayout->addWidget(btnRunStep);

    // "运行"时清除单步高亮
    connect(ui->btnProgRun, &QPushButton::clicked, this, [this] {
        if (m_lastHighlightedEditor) {
            m_lastHighlightedEditor->setExtraSelections({});
            m_lastHighlightedEditor = nullptr;
        }
    });

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

    // 左侧面板固定宽度
    leftPanel->setFixedWidth(260);
    leftPanel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    // 点动GroupBox也限制最大宽度
    ui->jogJointGroup->setMaximumWidth(260);
    ui->jogCartGroup->setMaximumWidth(260);
    ui->jogDebugGroup->setMaximumWidth(260);

    // ==================== 5. 组装最终布局 ====================
    // 面包屑 → 左右分栏(左侧:调试+点动 | 右侧:编程)
    layout->addItem(breadItem);
    delete debugItem;
    delete sepItem;
    layout->addWidget(bottomArea, 1);

    // 强制布局刷新，确保首次显示时计算正确
    ui->pageManual->updateGeometry();
}

