/*
 * @file mainwindow.cpp
 * @brief 实现原始主窗口类 MainWindow 的构造和析构。
 *
 * 本文件负责：
 * - 初始化 UI 界面
 * - 控制器连接管理（扫描、连接、断开）
 * - 验证建模功能
 *
 * 模块划分（以 ==================== XX ==================== 分界注释标记）：
 * 运行界面 / 程序编程 / 操作界面 / 通信 / 视觉运动 / 画板
 */
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "ui_PointAdjustDialog.h"
#include "src/logic/RobotController.h"
#include "src/logic/UiController.h"
#include "src/vision/cs_vmcontrol/VisionRobotLink.h"
#include "src/vision/cs_vmcontrol/vmcontrol.h"
#include "src/vision/cs_vmcontrol/vmwindow/VmWindow.h"
#include "src/hardware/Rs485Controller.h"
#include "src/hardware/ServoController.h"
#include "src/vision/VisualRoboticArm/VisionController.h"
#include "src/models/RobotConfig.h"
#include "library/zmotion.h"
#include "src/utils/MathUtils.h"
#include "src/utils/CodeEditor.h"
#include "library/zauxdll2.h"
#include <QApplication>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QAxWidget>
#include <QThread>
#include <QTimer>
#include <QtConcurrent>
#include <QtMath>
#include <QLineEdit>
#include <cmath>
#include <functional>
#include "ZmotionAPI/ZmotionAPI.h"
#include "src/hardware/ServoController.h"
#include "src/ui/ServoTestWidget.h"
#include <QDebug>
#include <QCoreApplication>
#include <QMessageBox>
#include <QCheckBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QTextCursor>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFile>
#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonObject>
#include <QJsonDocument>
#include <QLabel>
#include <QPushButton>
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
#include <QCryptographicHash>
#include <QSettings>

QString MainWindow::findConfigDir()
{
    // 查找 exe 所在目录向上的 config/ 目录（与 developer_account.ini 等同级）
    QDir dir(QCoreApplication::applicationDirPath());
    while (!dir.isRoot()) {
        QString candidate = dir.absolutePath() + "/config";
        if (QFileInfo::exists(candidate)) {
//             qDebug() << "[MainWindow] 找到配置目录:" << candidate;
            return candidate;
        }
        if (!dir.cdUp()) break;
    }
    qWarning() << "[MainWindow] 未找到 config 目录，使用默认路径";
    return "config";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_robotController(nullptr)
{
    ui->setupUi(this);

    // pageRun 外层垂直布局：三栏占上半部分，下半留空
    ui->pageRunVLayout->setStretch(0, 1);  // 三栏容器（上半）
    ui->pageRunVLayout->setStretch(1, 1);  // 底部留白 spacer（下半）

    // pageRun 横排三栏均分：实时位置 / 控制 / 运动参数
    ui->pageRunLayout->setStretch(0, 0);  // breadcrumbRun 隐藏
    ui->pageRunLayout->setStretch(1, 5);  // positionGroupBox 左栏（参考图约50%）
    ui->pageRunLayout->setStretch(2, 3);  // controlGroupBox 中栏（参考图约30%）
    ui->pageRunLayout->setStretch(3, 2);  // motionParamGroup 右栏（参考图约20%）
    ui->pageRunLayout->setStretch(4, 1);  // runSpacer

    // 初始化机器人控制器（按机型规格 R01 构建配置，不再依赖外部 JSON 配置）
    m_robotController = new RobotController(this);
    m_robotController->initialize(RobotModelId::R01);

    // 初始化 UI 辅助控制器（IO监控/报警日志/编程/设置）
    m_uiController = new UiController(this, m_robotController, this);
    m_uiController->init();

    // 点动完成时立即刷新位置标签
    connect(m_uiController, &UiController::jogCompleted,
            this, &MainWindow::updatePositionDisplay);
    // 物理按钮 IN9 = 运行/恢复流程，IN10 = 暂停流程
    connect(m_uiController, &UiController::opRunTriggered,
            this, &MainWindow::onBtnOpRun);
    connect(m_uiController, &UiController::opPauseTriggered,
            this, &MainWindow::onBtnOpPause);

    // 初始化连接面板
    initConnectionUI();

    // 在顶部添加一个持久的机型显示标签（不会被 setConnectionStatus 覆盖）
    // 将父对象设为 topBar，保证它由 topBar 布局管理并显示在顶栏
    m_labelModel = new QLabel(ui->topBar);
    m_labelModel->setObjectName(QStringLiteral("labelModel"));
    m_labelModel->setStyleSheet(QStringLiteral(
        "color:#39d98a;font-size:13px;font-weight:600;"
        "padding:0;background:transparent;border:0;"));
    m_labelModel->setText(QStringLiteral("机型: -"));
    // 显式插入到 labelStatus 之后，避免占用 IP 区域
    if (ui->labelStatus && ui->labelStatus->parentWidget()) {
        QLayout* topLayout = ui->labelStatus->parentWidget()->layout();
        if (topLayout) {
            int count = topLayout->count();
            int insertPos = -1;
            for (int i = 0; i < count; ++i) {
                QLayoutItem* it = topLayout->itemAt(i);
                if (!it) continue;
                QWidget* w = it->widget();
                if (w == ui->labelStatus) { insertPos = i + 1; break; }
            }
            if (insertPos >= 0) {
                QBoxLayout* box = qobject_cast<QBoxLayout*>(topLayout);
                if (box) box->insertWidget(insertPos, m_labelModel);
                else topLayout->addWidget(m_labelModel);
            } else {
                topLayout->addWidget(m_labelModel);
            }
        } else if (ui->topBar && ui->topBar->layout()) {
            ui->topBar->layout()->addWidget(m_labelModel);
        }
    } else if (ui->topBar && ui->topBar->layout()) {
        ui->topBar->layout()->addWidget(m_labelModel);
    }

    // 初始化位置显示面板
    initPositionDisplay();

    // 初始化导航栏
    m_navGroup = new QButtonGroup(this);
    m_navGroup->setExclusive(true);
    m_navGroup->addButton(ui->btnNavRun, 0); // 运行页（第 0 个导航页）
    m_navGroup->addButton(ui->btnNavManual, 1);// 手动编程页（第 1 个导航页）
    m_navGroup->addButton(ui->btnNavIO, 2);// 输入输出页（第 2 个导航页）
    m_navGroup->addButton(ui->btnNavAlarm, 3);// 报警日志页（第 3 个导航页）
    m_navGroup->addButton(ui->btnNavSettings, 4);// 设置页（第 4 个导航页）
    m_navGroup->addButton(ui->btnNavRs485, 5);// RS485 测试页（第 5 个导航页）
    m_navGroup->addButton(ui->btnNavVision, 6);      // 视觉运动（第 7 个导航页）
    m_navGroup->addButton(ui->btnNavOperator, 7); // 操作者界面页（对应 contentStack 静态页 index 7）
    m_navGroup->addButton(ui->btnNavDraw, 8);  // 画板页（动态页，索引 8）

    // 默认显示"操作界面"页（打钉子/暂停/急停），放在运行界面前面
    ui->contentStack->setCurrentIndex(7);
    ui->btnNavOperator->setChecked(true);
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
    connect(m_vision, &VisionController::started,            this, &MainWindow::onVisionStarted);
    connect(m_vision, &VisionController::stopped,            this, &MainWindow::onVisionStopped);
    connect(m_vision, &VisionController::errorOccurred,      this, &MainWindow::onVisionError);
    connect(m_vision, &VisionController::outputMessage,      this, &MainWindow::onVisionMessage);
    // 视觉下发多轴指令：commands[i] = (axis 0-5, signed_angle_deg)
    connect(m_vision, &VisionController::motorCommands,      this, &MainWindow::onVisionMotorCommands);

    // --- 视觉→机械臂联动 ---
    m_visionLink = new VisionRobotLink(this);

    // 注册 Qt 元类型（跨线程信号槽安全）
    qRegisterMetaType<VisionStatusEvent>("VisionStatusEvent");
    qRegisterMetaType<VisionDataEvent>("VisionDataEvent");
    qRegisterMetaType<VisionMessageEvent>("VisionMessageEvent");

    // VmWindow 延迟创建：首次点击"视觉定位"时创建，内部管理 VMControl 和 ActiveX 渲染
    // 按钮点击在 onBtnVerifyVision() 中处理

    // 连接"视觉定位"按钮
    connect(ui->btnVerifyVision, &QPushButton::clicked, this, &MainWindow::onBtnVerifyVision);

    // 切换到手动编程页时强制刷新布局
    connect(ui->contentStack, &QStackedWidget::currentChanged, this, [this](int idx) {
        if (idx == 1) {  // 手动 & 编程页
            ui->pageManual->updateGeometry();
            ui->pageManualLayout->activate();
        }
    });

    // 初始化"视觉运动"页（6 轴勾选 + 实时下发日志）
    initVisionControlUI();

    // 初始化开发者账号管理
    initLoginUI();

    // 默认加载 test.rbt 流程脚本路径（供打钉子按钮直接使用）
    m_opScriptPath = locateOpScript();

    // 机型一次性烧录模式：解析 --setmodel <name> [--ip <ip>] 并触发连接烧录
    // （信号已在 initConnectionUI 中连好；非烧录模式此方法直接 return，不影响日常启动）
    provisionModelIfRequested();
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
    // 初始状态：断开
    ui->btnDisconnect->setEnabled(false);
    setConnectionStatus(QStringLiteral("未连接"), QStringLiteral("red"));

    // 程序启动时自动扫描一遍物理IP
    QTimer::singleShot(100, this, [this]() {
        onScanClicked();
    });

    // 按钮点击信号
    connect(ui->btnScan, &QPushButton::clicked,this, &MainWindow::onScanClicked);
    connect(ui->btnConnect, &QPushButton::clicked,this, &MainWindow::onConnectClicked);
    connect(ui->btnDisconnect, &QPushButton::clicked,this, &MainWindow::onDisconnectClicked);
    connect(ui->pushButton_4, &QPushButton::clicked, this, &MainWindow::onHomingClicked);

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
    // 状态栏只显示当前状态；机型显示由 m_labelModel 单独维护，避免重复拼接 "- R02"
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
 * 并始终追加本机回环 IP 127.0.0.1 作为虚拟机/模拟器内的控制器选项。
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
    if (ipList.isEmpty()) {
        setConnectionStatus(QStringLiteral("未找到物理控制器"), QStringLiteral("orange"));
    } else {
        // 添加扫描到的IP地址到下拉框
        ui->comboIpList->addItems(ipList);
        setConnectionStatus(
            QStringLiteral("找到 %1 个控制器").arg(ipList.size()), QStringLiteral("green"));
    }

    // 追加虚拟机/本机回环 IP：模拟器内的控制器通过 127.0.0.1 可达，始终作为可选项
    if (!ipList.contains(QStringLiteral("127.0.0.1")))
        ui->comboIpList->addItem(QStringLiteral("127.0.0.1"));

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
 * @brief 机型一次性烧录模式入口（--setmodel）
 *
 * 解析启动参数：--setmodel <R06|R12|…> 必填，--ip <ip> 可选（缺省用界面下拉框当前项）。
 * 解析到合法机型则打标记并连控制器，连上后由 onHardwareConnected 写 Flash 并退出。
 * 不带 --setmodel 时直接返回，日常启动完全不受影响。
 */
void MainWindow::provisionModelIfRequested()
{
    QStringList args = QApplication::arguments();
    int idx = args.indexOf(QStringLiteral("--setmodel"));
    if (idx < 0 || idx + 1 >= args.size()) {
        return;  // 正常启动，非烧录模式
    }

    QString modelName = args.at(idx + 1);
    RobotModelId id = robotModelFromName(modelName);
    if (id == RobotModelId::Unknown) {
        QMessageBox::warning(nullptr, QStringLiteral("机型烧录"),
            QStringLiteral("未知机型 \"%1\"，可用：R01 / R02").arg(modelName));
        QApplication::quit();
        return;
    }

    // 取控制器 IP：优先 --ip，否则用界面下拉框当前选中项
    QString ip;
    int ipIdx = args.indexOf(QStringLiteral("--ip"));
    if (ipIdx >= 0 && ipIdx + 1 < args.size()) {
        ip = args.at(ipIdx + 1).trimmed();
    } else {
        ip = ui->comboIpList->currentText().trimmed();
        ip = ip.split(QStringLiteral(" ")).first();  // 去除可能的后缀说明文字
    }
    if (ip.isEmpty()) {
        QMessageBox::warning(nullptr, QStringLiteral("机型烧录"),
            QStringLiteral("未指定控制器 IP，请用 --ip 192.168.0.x 或在界面先选好 IP"));
        QApplication::quit();
        return;
    }

    m_provisionModel = id;
    setConnectionStatus(QStringLiteral("烧录机型 %1 → %2")
                        .arg(robotModelName(id)).arg(ip),
                        QStringLiteral("orange"));
    qInfo() << "[机型烧录] 先初始化内存配置为目标机型" << robotModelName(id);
    // 先把内存配置切换到将要烧录的机型，确保在连接时 seed 使用正确的 homePosition
    m_robotController->initialize(id);
    qInfo() << "[机型烧录] 开始连接控制器" << ip << "以烧录机型" << robotModelName(id);
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
    ZmotionAPI* api = m_robotController->getMotionController()->getZmotionAPI();

    // —— 机型一次性烧录模式（--setmodel <name>）——
    // 出厂/换臂时连上控制器即把型号 ID 写入 Flash，随后退出程序；日常启动不带此参数则跳过。
    if (m_provisionModel != RobotModelId::Unknown) {
        if (m_robotController) {
            bool ok = m_robotController->provisionModel(m_provisionModel, /*force=*/false);
            QMessageBox::information(this, QStringLiteral("机型烧录"),
                ok
                    ? QStringLiteral("机型 %1 已写入控制器并校验通过，程序将退出。")
                         .arg(robotModelName(m_provisionModel))
                    : QStringLiteral("烧录校验失败，请检查设备与日志。"));
            qInfo() << "[机型烧录]" << robotModelName(m_provisionModel) << (ok ? "成功" : "失败");
        } else {
            QMessageBox::warning(this, QStringLiteral("机型烧录"),
                QStringLiteral("无法获取 RobotController，烧录中止"));
        }
        QApplication::quit();
        return;
    }

    setConnectionStatus(QStringLiteral("已连接"), QStringLiteral("green"));
    updateConnectionUI(true);

    // —— 启动自动识别机型（从 Flash 读型号 ID）——
    if (api) {
        RobotModelId readId = RobotModelId::Unknown;
        ZmotionResult mr = api->readModelFromFlash(readId);
        m_modelId = readId;
        if (mr.success) {
            qInfo() << "[机型识别] 已从控制器 Flash 识别到机型:" << robotModelName(m_modelId);
            // 也打印到标准输出，便于在控制台启动时直接查看
            fprintf(stdout, "Detected robot model: %s\n", qPrintable(robotModelName(m_modelId)));
            fflush(stdout);
            // 机型配置跟随 Flash：识别机型与当前运行配置不一致时轻量重载。
            // 回零/建模等动作在执行时按 RobotConfig 实时取机型参数(getAxisSpec/getAxisUnitsList)，
            // 故连接成功阶段切换 config 即可让后续流程使用正确机型数据。
            if (readId != RobotModelId::Unknown
                && m_robotController
                && m_robotController->getConfig()->modelId() != readId) {
                if (m_robotController->applyModel(readId)) {
                    qInfo() << "[机型识别] 已切换运行配置到机型:" << robotModelName(readId);
                    if (m_uiController) {
                        m_uiController->appendConsole(
                            QStringLiteral("已切换运行配置到机型: %1").arg(robotModelName(readId)));
                    }
                } else {
                    qWarning() << "[机型识别] 切换运行配置失败";
                }
            }
            // 同时推到界面控制台（若 UiController 存在）
            if (m_uiController) {
                m_uiController->appendConsole(QStringLiteral("Detected robot model: %1").arg(robotModelName(m_modelId)));
            }
            // 机型显示已经独立放在 m_labelModel 上，不再反复拼接到状态栏文本
            if (m_labelModel) {
                m_labelModel->setText(QStringLiteral("机型: %1").arg(robotModelName(m_modelId)));
            }
            if (ui->labelIp) {
                ui->labelIp->setText(QStringLiteral("IP:"));
            }
        } else {
            qInfo() << "[机型识别] 未读到有效机型（" << mr.errorMessage
                    << "），将使用默认参数；如需自动识别请在机型设置页写入";
            fprintf(stdout, "Detected robot model: Unknown (use default)\n");
            fflush(stdout);
            if (m_uiController) {
                m_uiController->appendConsole(QStringLiteral("Detected robot model: Unknown (use default)"));
            }
            if (m_labelModel) {
                m_labelModel->setText(QStringLiteral("机型: 未设置"));
            }
            if (ui->labelIp) {
                ui->labelIp->setText(QStringLiteral("IP:"));
            }
        }
    }

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
    // 机型烧录模式下连接失败：提示并退出，避免停留在无控制器的界面
    if (m_provisionModel != RobotModelId::Unknown) {
        QMessageBox::warning(nullptr, QStringLiteral("机型烧录"),
            QStringLiteral("连接控制器失败，无法烧录机型：%1").arg(errorMsg));
        QApplication::quit();
        return;
    }
    setConnectionStatus(QStringLiteral("连接失败: %1").arg(errorMsg), QStringLiteral("red"));
    updateConnectionUI(false);
}
/**
 * @brief 机器人建模按钮点击处理
 *
 * 将机器人模型写入控制器（DH参数、脉冲当量等）。
 * 注意：建模后需要手动点击正解/逆解按钮切换控制模式。
 */
void MainWindow::on_pushButton_clicked()
{
    if (!checkLoginGate()) return;

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

/* ==================== 运行界面 ==================== */
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
//         qDebug() << "[MainWindow] 已切换到笛卡尔模式";
    } else {
        ui->statusbar->showMessage(QStringLiteral("切换笛卡尔模式失败"));
//         qDebug() << "[MainWindow] 切换笛卡尔模式失败";
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
//         qDebug() << "[MainWindow] 已切换到关节模式";
    } else {
        ui->statusbar->showMessage(QStringLiteral("切换关节模式失败"));
//         qDebug() << "[MainWindow] 切换关节模式失败";
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
    if (!checkLoginGate()) return;

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
    if (!checkLoginGate()) return;

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
 * @brief 回零操作确认弹窗（自绘深色无边框，与急停/暂停提示弹窗视觉统一）
 *
 * 替代原生 QMessageBox::question：
 *   - 无边框深色圆角卡片（对齐 showOpOverlay 风格）
 *   - 底部「取消」为默认焦点，ESC 关闭亦视为取消
 *
 * @return true=用户点击「确认执行」；false=取消
 */
bool MainWindow::confirmHomingOperation()
{
    QDialog dlg(this);
    dlg.setModal(true);
    dlg.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint); // 无边框，样式与整体界面统一
    dlg.setObjectName(QStringLiteral("homeConfirmDlg"));
    dlg.setFixedSize(480, 250);
    dlg.setStyleSheet(QStringLiteral(
        "QDialog#homeConfirmDlg{background-color:#232a3a;border:2px solid #3d4658;border-radius:12px;}"
        "QLabel#hcTitle{color:#98a2b3;font-size:16px;font-weight:bold;background:transparent;border:none;}"
        "QLabel#hcText{color:#FFFFFF;font-size:26px;font-weight:bold;background:transparent;border:none;}"
        "QPushButton#hcCancel{background-color:#3d4658;color:#d7dce5;font-size:18px;font-weight:bold;"
        "border:none;border-radius:10px;min-width:150px;min-height:48px;}"
        "QPushButton#hcCancel:hover{background-color:#4a5468;}"
        "QPushButton#hcOk{background-color:#e67e22;color:#FFFFFF;font-size:18px;font-weight:bold;"
        "border:none;border-radius:10px;min-width:150px;min-height:48px;}"
        "QPushButton#hcOk:hover{background-color:#f39c12;}"));

    auto* root = new QVBoxLayout(&dlg);
    root->setContentsMargins(24, 16, 24, 20);
    root->setSpacing(0);

    // 顶部：标题
    auto* title = new QLabel(QStringLiteral("确认执行"), &dlg);
    title->setObjectName(QStringLiteral("hcTitle"));
    root->addWidget(title);

    // 中部：正文
    auto* text = new QLabel(QStringLiteral("确定执行回零操作？"), &dlg);
    text->setObjectName(QStringLiteral("hcText"));
    text->setAlignment(Qt::AlignCenter);
    root->addWidget(text, 1);

    // 底部：取消（默认焦点）+ 确认执行
    auto* btnRow = new QWidget(&dlg);
    btnRow->setStyleSheet(QStringLiteral("background:transparent;"));
    auto* btnLayout = new QHBoxLayout(btnRow);
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->setSpacing(16);
    auto* cancelBtn = new QPushButton(QStringLiteral("取消"), btnRow);
    cancelBtn->setObjectName(QStringLiteral("hcCancel"));
    auto* okBtn = new QPushButton(QStringLiteral("确认执行"), btnRow);
    okBtn->setObjectName(QStringLiteral("hcOk"));
    btnLayout->addStretch();
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(okBtn);
    root->addWidget(btnRow);

    bool confirmed = false;
    connect(okBtn, &QPushButton::clicked, &dlg, [&dlg, &confirmed]() {
        confirmed = true;
        dlg.accept();
    });
    connect(cancelBtn, &QPushButton::clicked, &dlg, &QDialog::reject);

    cancelBtn->setFocus(); // 安全默认：默认焦点在取消
    dlg.move(this->rect().center().x() - dlg.width() / 2,
             this->rect().center().y() - dlg.height() / 2);
    dlg.exec();
    return confirmed;
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
void MainWindow::onHomingClicked()
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

    // 关机状态下点击回零 → 解除关机，回到原点
    if (m_isShutdown) {
        setConnectionStatus(QStringLiteral("解除关机..."), QStringLiteral("orange"));
        ui->statusbar->showMessage(QStringLiteral("正在解除关机，回到原点..."));

        // 弹窗：正在解除关机
        QMessageBox* unShutdownDlg = new QMessageBox(this);
        unShutdownDlg->setWindowTitle(QStringLiteral("解除关机"));
        unShutdownDlg->setText(QStringLiteral("正在解除关机..."));
        unShutdownDlg->setStandardButtons(QMessageBox::NoButton);
        unShutdownDlg->setWindowFlags(unShutdownDlg->windowFlags() | Qt::WindowStaysOnTopHint);
        unShutdownDlg->show();
        QApplication::processEvents();

        m_robotController->moveJ({0, 0, 0, 0, 0, 0}, 10.0, 20.0, 30.0, 10000);
        m_isShutdown = false;

        // 归位完成
        unShutdownDlg->setText(QStringLiteral("已回到原点"));
        QApplication::processEvents();

        setConnectionStatus(QStringLiteral("已就绪"), QStringLiteral("green"));
        ui->statusbar->showMessage(QStringLiteral("关机已解除，已回到原点"));

        // 1.5 秒后自动关闭弹窗
        QTimer::singleShot(1500, this, [unShutdownDlg]() {
            unShutdownDlg->accept();
            unShutdownDlg->deleteLater();
        });
        return;
    }

    // 已回零过，跳过
    if (m_homingDone) {
        ui->statusbar->showMessage(QStringLiteral("已回零过，无需重复操作"));
        return;
    }

    // 确认对话框（自绘深色，与急停/暂停弹窗风格统一）
    if (!confirmHomingOperation()) {
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
//             qDebug() << "[MainWindow] 关节轴0-5 DPOS 已清零";

            // 第四步：切换正解模式
            setConnectionStatus(QStringLiteral("切换正解..."), QStringLiteral("orange"));
            ui->statusbar->showMessage(QStringLiteral("关节坐标已清零，正在切换正解模式..."));

            bool ok = m_robotController->switchToJointMode();

            if (ok) {
                // 切一次逆解，让 CONNFRAME LOAD 同步虚拟轴 DPOS，再回正解
                m_robotController->switchToCartesianMode();
                m_robotController->switchToJointMode();
                m_homingDone = true;
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
 * @brief 功能测试：切换视觉程序启停
 */
void MainWindow::on_pushButton_6_clicked()
{
    qDebug() << "[视觉跟随按钮] 被点击";
    qDebug() << "[视觉跟随按钮] m_vision =" << m_vision;

    if (!m_vision) {
        qDebug() << "[功能测试按钮] m_vision 为空";
        return;
    }

    qDebug() << "[功能测试按钮] m_vision->isRunning() =" << m_vision->isRunning();
    if (m_vision->isRunning()) {
        qDebug() << "[功能测试按钮] 停止视觉进程";
        m_visionControlActive = false;
        m_vision->stop();
    } else {
        qDebug() << "[功能测试按钮] 启动视觉进程";
        m_vision->start();
    }
}

/**
 * @brief 打钉子按钮：固定运行打钉子专用流程 task/test.rbt。
 *
 * 防重入规则：
 *   1. 点击一次 → 执行一次固定流程（UiController::runScriptFile 直接读磁盘
 *      task/test.rbt，与编辑器当前加载的任意流程完全解耦）；
 *   2. 按下 = 输出口 DO0 高电平，松开 = DO0 低电平（电平跟随按钮），
 *      脚本 WHILE 循环以 WAIT_DO(0,1) 阻塞捕获按下时刻，松开复位不中断已进入的流程；
 *   3. 监听循环未运行时，按下会先自动启动脚本。
 * 无传感器方案：流程内部用 SET_DO + DELAY 固定脉宽触发钉枪，不依赖输入口。
 */
void MainWindow::on_btnNail_pressed()
{
    if (!m_robotController || !m_robotController->isConnected()) {
        QMessageBox::warning(this, QStringLiteral("打钉子"),
            QStringLiteral("请先连接控制器"));
        return;
    }

    // 若监听循环（task/test.rbt 的 WHILE TRUE）尚未运行，先启动它；
    // 已在运行（任务执行中）时跳过启动，仅置位 DO0=1 作为触发信号（防重入）。
    const bool listening = (m_uiController != nullptr) && m_uiController->isTaskRunning();
    if (!listening) {
        // 使用启动时预加载的 m_opScriptPath，为空则重新查找
        QString nailScript = m_opScriptPath;
        if (nailScript.isEmpty()) {
            nailScript = locateOpScript();
        }
        if (nailScript.isEmpty() || !m_uiController) {
            QMessageBox::warning(this, QStringLiteral("打钉子"),
                QStringLiteral("未找到打钉子流程文件 task/test.rbt，请确认工程目录结构"));
            return;
        }
        if (!m_uiController->runScriptFile(nailScript)) {
            ui->statusbar->showMessage(QStringLiteral("打钉子：监听循环未能启动"));
            return;
        }
        m_opScriptPath = nailScript;
    }

    // 按下 = 置位上位机输出口 DO0=1，作为电平触发信号（松开时由 released 复位为 0）
    ZmotionAPI* api = m_robotController->getMotionController()->getZmotionAPI();
    if (!api) {
        ui->statusbar->showMessage(QStringLiteral("打钉子：控制器接口不可用"));
        return;
    }
    ZmotionResult r = api->setOp(0, 1);
    qDebug() << "[打钉子] 按下置位 DO0=1，成功=" << r.success;
    if (r.success) {
        ui->statusbar->showMessage(QStringLiteral("打钉子：按下，DO0=1"));
    } else {
        ui->statusbar->showMessage(QStringLiteral("打钉子：置位 DO0=1 失败 (%1)").arg(r.errorMessage));
    }
}

/**
 * @brief 打钉子按钮松开：输出口 DO0 复位为低电平（0）
 */
void MainWindow::on_btnNail_released()
{
    if (!m_robotController || !m_robotController->isConnected()) {
        return;
    }
    ZmotionAPI* api = m_robotController->getMotionController()->getZmotionAPI();
    if (!api) {
        return;
    }
    ZmotionResult r = api->setOp(0, 0);
    qDebug() << "[打钉子] 松开复位 DO0=0，成功=" << r.success;
}

/**
 * @brief 拿水按钮：移动至拍照位 → 视觉定位 → 抓取 → 回拍照位
 */
void MainWindow::on_btnGetWater_clicked()
{
    qDebug() << "[拿水按钮] 被点击";
    if (!m_robotController || !m_robotController->isConnected()) {
        QMessageBox::warning(this, QStringLiteral("视觉拿水"),
            QStringLiteral("请先连接控制器"));
        return;
    }

    if (!m_vmWindow) {
        QMessageBox::warning(this, QStringLiteral("视觉拿水"),
            QStringLiteral("请先点击「视觉定位」启动视觉方案"));
        return;
    }

    if (!m_visionLink) {
        QMessageBox::warning(this, QStringLiteral("视觉拿水"),
            QStringLiteral("视觉联动模块未初始化"));
        return;
    }

    const QString processName = m_vmWindow->selectedProcessName();
    if (processName.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("视觉拿水"),
            QStringLiteral("未选择视觉流程，请先在视觉窗口中选择流程"));
        return;
    }
    constexpr int SHOT_COUNT = 3;

    // 防重复点击
    ui->btnGetWater->setEnabled(false);
    ui->statusbar->showMessage(QStringLiteral("拿水流程：移动至拍照位置..."));

    // 1. 设置拍照舵机角度（每通道独立时间模式）
    struct ServoAction { int channel; float angle; int timeMs; };
    
    auto* servoCtrl = m_robotController->getServoController();
    const QList<ServoAction> photoServoActions = {
        {0, 45, 50}, {1, 125, 100}, {2, 33, 50}, {3, 138, 50}, {4, 33, 50}, {5, 125, 50}//张开手掌
    };
    QList<int> channels, times;
    QList<float> angles, speeds;
    for (const auto& act : photoServoActions) {
        channels.append(act.channel);
        angles.append(act.angle);
        times.append(act.timeMs);
        speeds.append(0.0);
    }
    servoCtrl->setAnglesForScriptMixed(channels, angles, speeds, times);

    // 2. 移动机械臂到拍照位置
    m_robotController->moveL({360.0, 0.0, 650.000, 0.0, 90.0, 0.000}, 300.0, 300.0, 300.0, 5000);

    // 共享采样状态，跨多次回调累积
    struct SampleState {
        int count = 0;
        double sumX = 0.0;
        double sumY = 0.0;
    };
    auto state = std::make_shared<SampleState>();
    auto conn = std::make_shared<QMetaObject::Connection>();

    // 整体超时保护：若 10 秒内未采够 3 次则超时退出
    auto timeoutTimer = std::make_shared<QTimer>(this);
    timeoutTimer->setSingleShot(true);
    connect(timeoutTimer.get(), &QTimer::timeout, this,
        [this, processName, conn, timeoutTimer]() {
            disconnect(*conn);
            ui->statusbar->showMessage(QStringLiteral("拿水超时：视觉定位无结果"));
            if (m_visionLogView)
                m_visionLogView->appendPlainText(QStringLiteral("--- 拿水超时：视觉定位未产出有效数据 ---"));
            ui->btnGetWater->setEnabled(true);
        });
    timeoutTimer->start(10000);

    // 帧间延迟定时器，避免连续触发导致相机未就绪
    auto shotDelay = std::make_shared<QTimer>(this);
    shotDelay->setSingleShot(true);

    *conn = connect(m_visionLink, &VisionRobotLink::targetReady, this,
        [this, processName, state, conn, timeoutTimer, shotDelay](double robotX, double robotY) {
            ++state->count;
            state->sumX += robotX;
            state->sumY += robotY;

            if (state->count < SHOT_COUNT) {
                // 延迟 300ms 后触发下一次单帧拍摄
                connect(shotDelay.get(), &QTimer::timeout, this,
                    [this, processName, shotDelay]() {
                        m_vmWindow->vmControl()->executeProcedureCommand(
                            ProcMotionType::Run, processName);
                        disconnect(shotDelay.get(), &QTimer::timeout, nullptr, nullptr);
                    });
                shotDelay->start(300);
                return;
            }

            // 采够 3 次，取消超时与后续触发
            timeoutTimer->stop();
            shotDelay->stop();
            disconnect(*conn);

            const double avgX = state->sumX / SHOT_COUNT;
            const double avgY = state->sumY / SHOT_COUNT;

            qDebug() << "[拿水按钮] 3次平均坐标: X=" << avgX << " Y=" << avgY;

            ui->statusbar->showMessage(
                QStringLiteral("拿水流程：%1次平均 X=%2 Y=%3")
                    .arg(SHOT_COUNT).arg(avgX, 0, 'f', 3).arg(avgY, 0, 'f', 3));
            if (m_visionLogView) {
                m_visionLogView->appendPlainText(
                    QStringLiteral("%1次视觉平均: X=%2  Y=%3")
                        .arg(SHOT_COUNT).arg(avgX, 0, 'f', 3).arg(avgY, 0, 'f', 3));
            }

            // 3. 移动到抓取安全位置
            m_robotController->moveL({avgX-120, avgY, 400.000, 0.0, 90.0, 0.000},300.0, 300.0, 300.0, 5000);
            m_robotController->moveL({avgX-120, avgY, 170.000, 0.0, 90.0, 0.000}, 300.0, 300.0, 300.0, 5000);

            // 抓取点位
            m_robotController->moveL({avgX+10, avgY, 170.000, 0.0, 90.0, 0.000}, 150.0, 100.0, 100.0, 5000);

            auto* servoCtrl = m_robotController->getServoController();
            // 6. 闭合手掌
            {
                const QList<ServoAction> closeActions = {
                    {0, 45, 50}, {1, 115, 10}, {2, 118, 10}, {3, 60, 10}, {4, 116, 10}, {5, 52, 10}
                };
                QList<int> channels, times;
                QList<float> angles, speeds;
                for (const auto& act : closeActions) {
                    channels.append(act.channel);
                    angles.append(act.angle);
                    times.append(act.timeMs);
                    speeds.append(0.0);
                }
                servoCtrl->setAnglesForScriptMixed(channels, angles, speeds, times);
            }

            // 7. 抬升回安全高度
            m_robotController->moveL({avgX, avgY, 400.000, 0.0, 90.0, 0.000}, 200.0, 300.0, 300.0, 5000);

            // 8. 放置点上方
            m_robotController->moveL({400.0, 80.0, 400.000, 0.0, 90.0, 0.000}, 300.0, 300.0, 300.0, 5000);

            // 9. 放置点
            m_robotController->moveL({400.0, 80.0, 200.000, 0.0, 90.0, 0.000}, 100.0, 300.0, 300.0, 5000);

            // 10. 张开手掌
            {
                const QList<ServoAction> openActions = {
                    {0, 45, 30}, {1, 125, 30}, {2, 33, 10}, {3, 140, 10}, {4, 33, 10}, {5, 125, 10}
                };
                QList<int> channels, times;
                QList<float> angles, speeds;
                for (const auto& act : openActions) {
                    channels.append(act.channel);
                    angles.append(act.angle);
                    times.append(act.timeMs);
                    speeds.append(0.0);
                }
                servoCtrl->setAnglesForScriptMixed(channels, angles, speeds, times);
            }

            // 11. 离开放置点
            m_robotController->moveL({300.0, 80.0, 250.000, 0.0, 90.0, 0.000}, 300.0, 300.0, 300.0, 5000);
            m_robotController->moveL({300.0, 80.0, 500.000, 0.0, 90.0, 0.000}, 300.0, 300.0, 300.0, 5000);

            // 12. 回到拍照位置
            m_robotController->moveL({360.0, 0.0, 650.000, 0.0, 90.0, 0.000}, 300.0, 300.0, 300.0, 5000);
            ui->statusbar->showMessage(QStringLiteral("拿水流程：已完成抓取并回到拍照位置"));
            if (m_visionLogView) {
                m_visionLogView->appendPlainText(QStringLiteral("机械臂已完成抓取并回到拍照位置"));
            }

            ui->btnGetWater->setEnabled(true);
        });

    // 重置帧计数与滤波缓冲区，避免上次运行的历史帧影响本次预热判断
    m_visionLink->resetFrameCount();

    // 逐帧模式下关闭预热与内部平均：每帧 Run 产出一个独立 targetReady，
    // 由本按钮回调自行收集并求算术平均，避免预热丢弃导致 shotDelay 永不触发
    m_visionLink->setWarmupFrames(0);
    m_visionLink->setFilterWindow(1);

    // 启动第一次单帧拍摄
    ui->statusbar->showMessage(QStringLiteral("拿水流程：启动视觉逐帧拍摄..."));
    m_vmWindow->vmControl()->executeProcedureCommand(ProcMotionType::Run, processName);
}

/**
 * @brief 视觉定位按钮：创建/显示 VmWindow（含 ActiveX 渲染和方案控制）
 */
void MainWindow::onBtnVerifyVision()
{
    qDebug() << "[视觉定位按钮] 被点击";

    // 延迟创建 VmWindow（首次点击时）
    if (!m_vmWindow) {
        m_vmWindow = new VmWindow();
        qDebug() << "[MainWindow] VmWindow created";

        // VMControl 结构化数据 → VisionRobotLink 滤波
        connect(m_vmWindow->vmControl(), &VMControl::dataReady,
                m_visionLink, &VisionRobotLink::onVisionDataEvent);

        // VMControl 日志 → 视觉日志面板
        connect(m_vmWindow->vmControl(), &VMControl::messageReady, this,
            [this](const VisionMessageEvent& event) {
                if (m_visionLogView)
                    m_visionLogView->appendPlainText(QStringLiteral("[VM] %1").arg(event.message));
            });

        // VMControl 状态 → 状态标签
        connect(m_vmWindow->vmControl(), &VMControl::statusChanged, this,
            [this](const VisionStatusEvent& event) {
                if (m_visionStatusLabel)
                    m_visionStatusLabel->setText(userRoleToString(event.Status));
            });
    }

    m_vmWindow->show();
    m_vmWindow->raise();
    m_vmWindow->activateWindow();
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
 * @brief 关机按钮：安全停止 + 禁用轴
 *
 * 先暂停任务，再执行安全停止（rapidStop + 设置空闲状态）。
 */
void MainWindow::onBtnShutdown()
{
    // 关机确认对话框
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        QStringLiteral("确认关机"),
        QStringLiteral("确定要执行关机操作吗？"),
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Cancel);
    if (reply != QMessageBox::Ok) {
        return;
    }

    // 防重复触发
    ui->btnShutdown->setEnabled(false);

    // 先停止任务
    m_uiController->pauseOrResumeTask();

    if (!m_robotController || !m_robotController->isConnected()) {
        ui->statusbar->showMessage(QStringLiteral("控制器未连接，无法执行关机"));
        ui->btnShutdown->setEnabled(true);
        return;
    }

    // 弹窗：正在关机
    QMessageBox* dlg = new QMessageBox(this);
    dlg->setWindowTitle(QStringLiteral("关机"));
    dlg->setText(QStringLiteral("正在关机..."));
    dlg->setStandardButtons(QMessageBox::NoButton);
    dlg->setWindowFlags(dlg->windowFlags() | Qt::WindowStaysOnTopHint);
    dlg->show();
    QApplication::processEvents();

    // 回到安全位置：按机型取关机位姿（R01 {0,-50,50,0,0,0} / R02 {0,-25,260,0,0,0}）
    QList<double> shutdownPos = m_robotController->getShutdownPosition();
    if (shutdownPos.size() != 6) {
        shutdownPos = {0, -50, 50, 0, 0, 0};  // spec 缺失时兜底（R01 值）
        qWarning() << "[关机] 机型未配置关机位姿，回退默认:" << shutdownPos;
    }
    qInfo() << "[关机] 移动到关机位姿:" << shutdownPos;
    m_robotController->moveJ(shutdownPos, 10.0, 20.0, 30.0, 10000);

    // 安全停止
    m_robotController->executeSafeStop();

    // 标记为关机状态
    m_isShutdown = true;

    // 关机完成
    dlg->setText(QStringLiteral("关机完成"));
    QApplication::processEvents();

    // 1.5 秒后自动关闭弹窗并恢复按钮
    QTimer::singleShot(1500, this, [this, dlg]() {
        dlg->accept();
        dlg->deleteLater();
        ui->btnShutdown->setEnabled(true);
    });
}

/* ==================== 操作界面 ==================== */
/**
 * @brief 操作界面 运行/恢复按钮：输入口 IN9
 *
 * 点击一次：
 *   - 若任务未运行 → 启动 test.rbt 流程
 *   - 若任务已暂停 → 从断点恢复继续执行
 */
void MainWindow::onBtnOpRun()
{
    if (!m_robotController || !m_robotController->isConnected()) {
        ui->statusbar->showMessage(QStringLiteral("运行：控制器未连接"));
        return;
    }

    if (m_opPause) {
        // 已暂停 → 恢复执行
        m_opPause = false;
        closeOpOverlay();
        if (m_uiController) m_uiController->resumeTask();
        qDebug() << "[操作界面] DI9 运行按钮：从暂停恢复执行";
        ui->statusbar->showMessage(QStringLiteral("已恢复，任务继续执行"));
        return;
    }

    if (m_uiController && m_uiController->isTaskRunning()) {
        ui->statusbar->showMessage(QStringLiteral("任务正在运行中"));
        return;
    }

    // 任务未运行 → 启动 test.rbt 流程
    QString nailScript = m_opScriptPath;
    if (nailScript.isEmpty()) {
        nailScript = locateOpScript();
    }
    if (nailScript.isEmpty() || !m_uiController) {
        QMessageBox::warning(this, QStringLiteral("运行流程"),
            QStringLiteral("未找到流程文件 task/test.rbt，请确认工程目录结构"));
        return;
    }
    if (!m_uiController->runScriptFile(nailScript)) {
        ui->statusbar->showMessage(QStringLiteral("流程启动失败"));
        return;
    }
    m_opScriptPath = nailScript;
    qDebug() << "[操作界面] DI9 运行按钮：启动流程" << nailScript;
    ui->statusbar->showMessage(QStringLiteral("流程已启动（DI9）"));
}

/**
 * @brief 操作界面 暂停按钮：输入口 IN10 电平切换
 *
 * 点击一次 → IN10 置位=1，当前运行的 rbt 流程暂停（机械臂停止运动），
 *           弹出大提示框「机械臂处于暂停状态」；
 * 再次点击 → IN10 复位=0，任务从断点继续执行尚未完成的代码，关闭提示框。
 */
void MainWindow::onBtnOpPause()
{
    if (!m_robotController || !m_robotController->isConnected()) {
        ui->statusbar->showMessage(QStringLiteral("暂停：控制器未连接"));
        return;
    }

    if (m_opPause) {
        // 已暂停 → 恢复（IN10 复位=0）
        m_opPause = false;
        closeOpOverlay();
        if (m_uiController) m_uiController->resumeTask();
        qDebug() << "[操作界面] 暂停解除：输入口 IN10 复位=0，任务继续执行";
        ui->statusbar->showMessage(QStringLiteral("已恢复，任务继续执行"));
    } else {
        // 未暂停 → 暂停（IN10 置位=1）
        m_opPause = true;
        showOpOverlay(QStringLiteral("机械臂处于暂停状态"), false);
        if (m_uiController) m_uiController->pauseTask();
        qDebug() << "[操作界面] 已暂停：输入口 IN10 置位=1";
        ui->statusbar->showMessage(QStringLiteral("已暂停（IN10=1）"));
    }
}

/**
 * @brief 操作界面 急停按钮：输入口 IN9 电平切换
 *
 * 点击一次 → IN9 置位=1，正在执行的 rbt 流程立即结束、机械臂停下，
 *           弹出大提示框「机械臂处于急停状态」；
 * 再次点击 → IN9 复位=0，解除急停并重新执行程序（重跑同一流程脚本）。
 */
void MainWindow::onBtnOpEStop()
{
    if (!m_robotController || !m_robotController->isConnected()) {
        ui->statusbar->showMessage(QStringLiteral("急停：控制器未连接"));
        return;
    }

    if (m_opEStop) {
        // 已急停 → 解除急停并重新执行（IN9 复位=0）
        m_opEStop = false;
        closeOpOverlay();
        qDebug() << "[操作界面] 急停解除：输入口 IN9 复位=0，程序重新执行";
        ui->statusbar->showMessage(QStringLiteral("急停解除，程序重新执行"));
        restartOpScript();
    } else {
        // 未急停 → 结束当前流程并停机（IN9 置位=1）
        m_opEStop = true;
        if (m_uiController) m_uiController->stopTask();
        if (m_robotController) m_robotController->stopMotion();
        showOpOverlay(QStringLiteral("机械臂处于急停状态"), true);
        qDebug() << "[操作界面] 已急停：输入口 IN9 置位=1，rbt 流程结束，机械臂停止";
        ui->statusbar->showMessage(QStringLiteral("急停中（IN9=1）"));
    }
}

/**
 * @brief 显示"急停/暂停 状态提示弹窗"
 *
 * 不再使用铺满主窗口的半透明遮罩，改为一个居中、固定尺寸、无边框的小弹窗
 * （460x240），不屏蔽底层界面、用户随时可操作；弹窗内包含：
 *   - 右上角关闭按钮（✕）：仅关闭提示弹窗，不改变急停/暂停状态；
 *   - 底部主操作按钮：执行与"再次点击对应按钮"完全一致的逻辑，
 *     保证不重复触发、不改变底层状态机语义：
 *       - 急停态：调用 onBtnOpEStop()（解除急停 IN9 复位 + 重新执行流程脚本）
 *       - 暂停态：调用 onBtnOpPause()（恢复运行 IN10 复位 + resumeTask）
 *
 * @param text     状态文案（如「机械臂处于急停状态」）
 * @param isEStop  true=急停态（按钮文案「解除急停」）；false=暂停态（按钮文案「恢复运行」）
 */
void MainWindow::showOpOverlay(const QString& text, bool isEStop)
{
    closeOpOverlay();
    auto* dlg = new QDialog(this);
    dlg->setModal(false);                                   // 非模态：不屏蔽底层界面操作
    dlg->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint); // 无边框，样式与整体界面统一
    dlg->setObjectName(QStringLiteral("opOverlayDlg"));
    dlg->setFixedSize(460, isEStop ? 240 : 280);
    dlg->setStyleSheet(QStringLiteral(
        "QDialog#opOverlayDlg{background-color:#232a3a;border:2px solid #3d4658;border-radius:12px;}"
        "QLabel#opTitle{color:#98a2b3;font-size:16px;font-weight:bold;background:transparent;border:none;}"
        "QLabel#opText{color:#FFFFFF;font-size:28px;font-weight:bold;background:transparent;border:none;}"
        "QLabel#opProgress{color:#98a2b3;font-size:20px;font-weight:bold;background:transparent;border:none;}"
        "QPushButton#opClose{background-color:transparent;color:#98a2b3;font-size:18px;font-weight:bold;"
        "border:none;min-width:30px;min-height:30px;}"
        "QPushButton#opClose:hover{color:#FFFFFF;background-color:#3d4658;border-radius:6px;}"
        "QPushButton#opAction{background-color:%1;color:#FFFFFF;font-size:22px;font-weight:bold;"
        "border:none;border-radius:10px;min-width:170px;min-height:56px;}"
        "QPushButton#opAction:hover{background-color:%2;}"
        "QPushButton#opEnd{background-color:#e74c3c;color:#FFFFFF;font-size:22px;font-weight:bold;"
        "border:none;border-radius:10px;min-width:170px;min-height:56px;}"
        "QPushButton#opEnd:hover{background-color:#c0392b;}")
        .arg(isEStop ? QStringLiteral("#e67e22") : QStringLiteral("#27ae60"),
             isEStop ? QStringLiteral("#f39c12") : QStringLiteral("#2ecc71")));

    auto* root = new QVBoxLayout(dlg);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // 顶部：状态标题 + 关闭按钮
    auto* topBar = new QWidget(dlg);
    topBar->setStyleSheet(QStringLiteral("background:transparent;"));
    auto* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(20, 10, 10, 0);
    auto* title = new QLabel(isEStop ? QStringLiteral("急停状态") : QStringLiteral("暂停状态"), topBar);
    title->setObjectName(QStringLiteral("opTitle"));
    auto* closeBtn = new QPushButton(QStringLiteral("×"), topBar);   // 仅关闭提示，不改变状态
    closeBtn->setObjectName(QStringLiteral("opClose"));
    closeBtn->setToolTip(QStringLiteral("关闭提示（不改变当前状态）"));
    topLayout->addWidget(title);
    topLayout->addStretch();
    topLayout->addWidget(closeBtn);
    root->addWidget(topBar);

    // 中部：状态文案 + 进度信息（暂停态）
    auto* body = new QWidget(dlg);
    body->setStyleSheet(QStringLiteral("background:transparent;"));
    auto* bodyLayout = new QVBoxLayout(body);
    bodyLayout->setContentsMargins(24, 12, 24, 8);
    auto* lbl = new QLabel(text, body);
    lbl->setObjectName(QStringLiteral("opText"));
    lbl->setAlignment(Qt::AlignCenter);
    bodyLayout->addWidget(lbl);
    if (!isEStop) {
        auto* progressLbl = new QLabel(QStringLiteral("等待进度..."), body);
        progressLbl->setObjectName(QStringLiteral("opProgress"));
        progressLbl->setAlignment(Qt::AlignCenter);
        bodyLayout->addWidget(progressLbl);
    }
    root->addWidget(body, 1);

    // 底部：操作按钮（暂停态含"结束任务"）
    auto* footer = new QWidget(dlg);
    footer->setStyleSheet(QStringLiteral("background:transparent;"));
    auto* footerLayout = new QHBoxLayout(footer);
    footerLayout->setContentsMargins(20, 8, 20, 20);
    footerLayout->setSpacing(20);
    auto* actionBtn = new QPushButton(isEStop ? QStringLiteral("解除急停")
                                              : QStringLiteral("恢复运行"), footer);
    actionBtn->setObjectName(QStringLiteral("opAction"));
    footerLayout->addWidget(actionBtn);
    if (!isEStop) {
        auto* endBtn = new QPushButton(QStringLiteral("结束任务"), footer);
        endBtn->setObjectName(QStringLiteral("opEnd"));
        footerLayout->addWidget(endBtn);
        connect(endBtn, &QPushButton::clicked, this, [this]() {
            QMessageBox confirmBox;
            confirmBox.setWindowTitle(QStringLiteral("确认停止"));
            confirmBox.setText(QStringLiteral("确定要结束任务吗？\n\n机械臂将保持在当前位置不动。"));
            confirmBox.setIcon(QMessageBox::Warning);
            confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            confirmBox.setDefaultButton(QMessageBox::No);
            if (confirmBox.exec() == QMessageBox::Yes) {
                if (m_uiController) m_uiController->stopTask();
                m_opPause = false;
                closeOpOverlay();
            }
        });
    } else {
        footerLayout->addStretch();
    }
    root->addWidget(footer);

    // 右上角 ✕ / ESC：仅关闭提示弹窗，不改变急停/暂停状态
    connect(closeBtn, &QPushButton::clicked, dlg, &QDialog::reject);
    // 主操作按钮 = 再次点击对应按钮：复用 onBtnOpEStop / onBtnOpPause 既有急停/恢复逻辑
    connect(actionBtn, &QPushButton::clicked, this, [this, isEStop]() {
        if (isEStop)
            onBtnOpEStop();   // 急停态：解除急停（IN9 复位=0）并重新执行流程脚本
        else
            onBtnOpPause();   // 暂停态：恢复运行（IN10 复位=0）并 resumeTask
    });
    // 任何关闭路径（reject / ESC）统一清理弹窗对象
    connect(dlg, &QDialog::finished, this, [this, dlg]() {
        if (m_opOverlay == dlg) {
            m_opOverlay->deleteLater();
            m_opOverlay = nullptr;
        }
    });

    // 居中显示于主窗口中央
    dlg->move(this->rect().center().x() - dlg->width() / 2,
              this->rect().center().y() - dlg->height() / 2);
    dlg->show();
    m_opOverlay = dlg;
}

/**
 * @brief 更新暂停覆盖层的进度信息（由 UiController::onTaskPaused 调用）
 */
void MainWindow::updateOpOverlayProgress(int current, int total)
{
    if (!m_opOverlay) return;
    auto* lbl = m_opOverlay->findChild<QLabel*>(QStringLiteral("opProgress"));
    if (lbl)
        lbl->setText(QStringLiteral("当前进度: %1 / %2").arg(current).arg(total));
}

/**
 * @brief 关闭状态提示覆盖层
 */
void MainWindow::closeOpOverlay()
{
    if (m_opOverlay) {
        m_opOverlay->deleteLater();
        m_opOverlay = nullptr;
    }
}

/**
 * @brief 定位操作界面流程脚本 task/test.rbt（exe 位于 build 子目录时上跳一级到工程根）
 */
QString MainWindow::locateOpScript() const
{
    const QString exeDir = QCoreApplication::applicationDirPath();
    if (QFileInfo::exists(exeDir + QStringLiteral("/task/test.rbt")))
        return exeDir + QStringLiteral("/task/test.rbt");
    if (QFileInfo::exists(exeDir + QStringLiteral("/../task/test.rbt")))
        return exeDir + QStringLiteral("/../task/test.rbt");
    if (QFileInfo::exists(exeDir + QStringLiteral("/../../task/test.rbt")))
        return exeDir + QStringLiteral("/../../task/test.rbt");
    return QString();
}

/**
 * @brief 急停解除后重新执行程序：优先重跑记录的操作界面流程脚本，
 *        否则按默认定位逻辑加载 task/test.rbt。
 */
void MainWindow::restartOpScript()
{
    QString script = m_opScriptPath;
    if (script.isEmpty()) script = locateOpScript();
    if (script.isEmpty() || !m_uiController) {
        ui->statusbar->showMessage(QStringLiteral("急停解除：未找到流程脚本，无法重新执行"));
        return;
    }
    if (!m_uiController->runScriptFile(script)) {
        ui->statusbar->showMessage(QStringLiteral("急停解除：程序未能重新启动"));
    }
}

/* ==================== 运行界面 ==================== */
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
    // 运行页面：关机按钮
    connect(ui->btnShutdown, &QPushButton::clicked, this, &MainWindow::onBtnShutdown);

    // 操作者界面：5 个大按钮显式映射到专用槽（不依赖 on_ 自动连接）
    connect(ui->btnOpHome, &QPushButton::clicked, this, &MainWindow::onHomingClicked);
    // 暂停按钮：输入口 IN10，点一次暂停、再点恢复
    connect(ui->btnOpStop, &QPushButton::clicked, this, &MainWindow::onBtnOpPause);
    // 急停按钮：输入口 IN9，点一次急停（结束流程+停机）、再点解除并重新执行
    connect(ui->btnOpStopAll, &QPushButton::clicked, this, &MainWindow::onBtnOpEStop);
    connect(ui->btnOpShutdown, &QPushButton::clicked, this, &MainWindow::onBtnShutdown);
    // 操作者页打钉子按钮：按下置位 DO0=1、松开复位 DO0=0（电平跟随按钮）
    connect(ui->btnOpNail, &QPushButton::pressed, this, &MainWindow::on_btnNail_pressed);
    connect(ui->btnOpNail, &QPushButton::released, this, &MainWindow::on_btnNail_released);

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
 * @brief 原点设置弹窗（统一入口）
 *
 * 打开一个模态弹窗，包含：
 *   - 只读显示当前 Flash 原点（6 轴）
 *   - 6 个 QDoubleSpinBox（只读，显示当前 Flash 原点，3 位小数）
 *   - [设置机械原点]  把"当前Flash原点 + 当前DPOS"写入 Flash（点击即保存），
 *                     成功后按钮置灰防重复点击；想再改需取消后重新打开弹窗
 *   - [取消]
 *
 * 语义：机械臂当前实际位姿 = Flash原点 + 当前DPOS（与主界面"机械原点"显示同一公式）。
 * 用户点动到目标位姿后点"设置机械原点"，即把当前实际位姿固化为新的 Flash 原点。
 */
void MainWindow::showOriginDialog()
{
    if (!m_robotController || !m_robotController->isConnected()) {
        QMessageBox::warning(this, QStringLiteral("原点设置"), QStringLiteral("请先连接控制器"));
        return;
    }

    QDialog dlg(this);
    dlg.setWindowTitle(QStringLiteral("原点设置"));
    dlg.setMinimumWidth(460);

    QVBoxLayout* mainLay = new QVBoxLayout(&dlg);

    // 只读显示当前 Flash 原点
    QLabel* lblCurrent = new QLabel(&dlg);
    lblCurrent->setWordWrap(true);
    lblCurrent->setStyleSheet(QStringLiteral(
        "QLabel{font-family:Consolas,'Microsoft YaHei UI';font-size:13px;"
        "color:#555;background:#f5f5f5;border:1px solid #e0e0e0;"
        "border-radius:4px;padding:6px 8px;}"));
    mainLay->addWidget(lblCurrent);

    // 6 个只读 SpinBox（显示当前 Flash 原点，不可手改——修改方式是点动后点"设置机械原点"）
    QDoubleSpinBox* spins[6] = {nullptr};
    QGridLayout* grid = new QGridLayout();
    grid->setHorizontalSpacing(6);
    for (int i = 0; i < 6; ++i) {
        QLabel* lbl = new QLabel(QStringLiteral("J%1").arg(i), &dlg);
        lbl->setAlignment(Qt::AlignCenter);
        spins[i] = new QDoubleSpinBox(&dlg);
        spins[i]->setRange(-9999.0, 9999.0);
        spins[i]->setDecimals(3);
        spins[i]->setSingleStep(0.001);
        spins[i]->setMinimumWidth(80);
        spins[i]->setReadOnly(true);
        spins[i]->setButtonSymbols(QAbstractSpinBox::NoButtons);
        grid->addWidget(lbl, 0, i);
        grid->addWidget(spins[i], 1, i);
    }
    mainLay->addLayout(grid);

    // 用当前 Flash 原点预填 SpinBox 并刷新只读显示
    auto fillCurrentOrigin = [this, spins, lblCurrent]() {
        QList<double> origin = m_robotController->getOriginPosition();
        QStringList parts;
        for (int i = 0; i < 6; ++i) {
            double v = (i < origin.size()) ? origin.at(i) : 0.0;
            spins[i]->setValue(v);
            parts << QStringLiteral("J%1=%2").arg(i).arg(v, 0, 'f', 3);
        }
        lblCurrent->setText(QStringLiteral("当前 Flash 原点：") + parts.join(QStringLiteral("  ")));
    };
    fillCurrentOrigin();

    // 按钮行
    QHBoxLayout* btnLay = new QHBoxLayout();
    QPushButton* btnSetOrigin = new QPushButton(QStringLiteral("设置机械原点"), &dlg);
    QPushButton* btnCancel    = new QPushButton(QStringLiteral("取消"), &dlg);
    btnSetOrigin->setStyleSheet(QStringLiteral(
        "QPushButton{background:#2e7d32;color:#fff;border:1px solid #1b5e20;"
        "border-radius:4px;font-weight:bold;padding:4px 14px;}"
        "QPushButton:hover{background:#1b5e20;}"
        "QPushButton:disabled{background:#9e9e9e;border-color:#757575;}"));
    btnLay->addWidget(btnSetOrigin);
    btnLay->addStretch();
    btnLay->addWidget(btnCancel);
    mainLay->addLayout(btnLay);

    // [设置机械原点]：新原点 = 当前Flash原点 + 当前DPOS，点击即写入 Flash；
    // 成功后按钮置灰，防止重复点击；想再改需取消后重新打开弹窗。
    connect(btnSetOrigin, &QPushButton::clicked, &dlg,
            [this, btnSetOrigin, fillCurrentOrigin, &dlg]() {
        QList<double> origin = m_robotController->getOriginPosition();
        QList<double> dpos   = m_robotController->getAllAxisPositions();
        QList<double> values;
        QStringList preview;
        for (int i = 0; i < 6; ++i) {
            double base = (i < origin.size()) ? origin.at(i) : 0.0;
            double off  = (i < dpos.size())   ? dpos.at(i)   : 0.0;
            double v = base + off;
            values.append(v);
            preview << QStringLiteral("J%1=%2").arg(i).arg(v, 0, 'f', 3);
        }

        QString err;
        if (m_robotController->setOriginFromValues(values, &err)) {
            // 设原点后复位 homing 标志，允许再次点击"回零"使新原点生效
            m_homingDone = false;
            btnSetOrigin->setEnabled(false);  // 置灰，防重复点击
            fillCurrentOrigin();              // 刷新显示为写入后的新值
            ui->statusbar->showMessage(QStringLiteral("机械原点已写入Flash"));
            QMessageBox::information(&dlg, QStringLiteral("原点设置"),
                QStringLiteral("机械原点已写入控制器 Flash（断电保持）：\n\n%1\n\n"
                               "当前 6 轴关节角度已清零（DPOS=0 ↔ 新原点）。")
                    .arg(preview.join(QStringLiteral("  "))));
        } else {
            QMessageBox::warning(&dlg, QStringLiteral("原点设置"),
                err.isEmpty() ? QStringLiteral("原点写入失败，请检查控制器连接状态。") : err);
        }
    });

    connect(btnCancel, &QPushButton::clicked, &dlg, &QDialog::reject);

    dlg.exec();
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
    // 画板为动态页：首次点击前 contentStack 暂无该索引，越界时跳过系统切换（画板分支后续用 setCurrentWidget 定位）
    if (index < ui->contentStack->count())
        ui->contentStack->setCurrentIndex(index);
    const QString names[] = {"运行", "程序编程", "IO 监控", "报警日志", "设置", "通信", "视觉运动", "操作界面", "画板"};
    if (index >= 0 && index < 9)
        ui->statusbar->showMessage(QStringLiteral("切换到: %1").arg(names[index]));

    if (index == 8) {
        if (!m_drawingWindow) {
            m_drawingWindow = new DrawingWindow(this);
            ui->contentStack->addWidget(m_drawingWindow);

            // 连接画板信号 → MainWindow 槽函数
            connect(m_drawingWindow, &DrawingWindow::sendDrawingPoints,
                    this, &MainWindow::onDrawingPointsReceived);
            connect(m_drawingWindow, &DrawingWindow::sendDrawingCircles,
                    this, &MainWindow::onDrawingCirclesReceived);
            connect(m_drawingWindow, &DrawingWindow::sendDrawingArcs,
                    this, &MainWindow::onDrawingArcsReceived);
            connect(m_drawingWindow, &DrawingWindow::requestInverseKinematics,
                    this, &MainWindow::onRequestInverseKinematics);
            connect(m_drawingWindow, &DrawingWindow::requestStopMotion,
                    this, &MainWindow::onRequestStopMotion);
            connect(m_drawingWindow, &DrawingWindow::requestDrawMotionParams,
                    this, &MainWindow::onMotionParamsReceived);
            connect(m_drawingWindow, &DrawingWindow::requestWorkpieceCoordSetup,
                    this, &MainWindow::onWorkpieceCoordAction);
        }
        // 传递工件坐标系给画板
        m_drawingWindow->setWorkPieceCoord(&m_workPieceCoord);
        m_drawingWindow->setDefaultPose(m_workPieceCoord.getRX(),
                                         m_workPieceCoord.getRY(),
                                         m_workPieceCoord.getRZ());
        ui->contentStack->setCurrentWidget(m_drawingWindow);
    }
}

/* ==================== 通信 ==================== */

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
//     qDebug() << "[Rs485] [发送]" << bytesToHex(data);
//     qDebug() << "[Rs485] [完整帧]" << bytesToHex(dataWithCrc);
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

//     qDebug() << "[Rs485] 调用 Rs485Controller::sendModbusRequest()";
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

//     qDebug() << "[PWM测试] 设置 PWM1 占空比为 75";
    
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
//         qDebug() << "[RS485] 定时发送已启动，间隔:" << interval << "ms";
    } else {
        if (m_rs485AutoSendTimer) {
            m_rs485AutoSendTimer->stop();
        }
        ui->rs485SpinInterval->setEnabled(true);
//         qDebug() << "[RS485] 定时发送已停止";
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

/* ==================== 程序编程 ==================== */
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
        migrateBtn(ui->btnProgWaitDO, lay); ui->btnProgWaitDO->setText(QStringLiteral("等DO"));
        QFrame* s2 = new QFrame(); s2->setFrameShape(QFrame::VLine);
        s2->setStyleSheet(QStringLiteral("color:#e0e0e0;"));
        lay->addWidget(s2);
        migrateBtn(ui->btnProgIfIO, lay);   ui->btnProgIfIO->setText(QStringLiteral("如果"));
        migrateBtn(ui->btnProgElse, lay);   ui->btnProgElse->setText(QStringLiteral("否则"));
        QFrame* s3 = new QFrame(); s3->setFrameShape(QFrame::VLine);
        s3->setStyleSheet(QStringLiteral("color:#e0e0e0;"));
        lay->addWidget(s3);
        migrateBtn(ui->btnProgSet, lay);    ui->btnProgSet->setText(QStringLiteral("赋值"));
        // 统一逻辑按钮样式
        for (QPushButton* b : {ui->btnProgWaitIdle, ui->btnProgDelay, ui->btnProgSetDO,
                                ui->btnProgWaitDI, ui->btnProgWaitDO, ui->btnProgIfIO, ui->btnProgElse, ui->btnProgSet}) {
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

    // 列布局: J标签 | J值 | 分隔线 | C标签 | C值
    const QString labelStyle = QStringLiteral(
        "color:#333;font-size:14px;font-weight:bold;border:none;background:transparent;");

    for (int i = 0; i < 6; ++i) {
        QLineEdit* spJ = spinJ[i];
        QLineEdit* spC = spinC[i];

        // SpinBox 重新设置 parent
        spJ->setParent(jogMergedGroup);
        spC->setParent(jogMergedGroup);

        // 关节标签 (J0~J5)
        QLabel* lbJ = new QLabel(jointLabels[i], jogMergedGroup);
        lbJ->setStyleSheet(labelStyle);
        lbJ->setFixedWidth(24);
        lbJ->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        // 笛卡尔标签 (X/Y/Z/A/B/C)
        QLabel* lbC = new QLabel(cartLabels[i], jogMergedGroup);
        lbC->setStyleSheet(labelStyle);
        lbC->setFixedWidth(24);
        lbC->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

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

        twoColGrid->addWidget(lbJ, i, 0);
        twoColGrid->addWidget(spJ, i, 1);
        twoColGrid->addWidget(lbC, i, 3);
        twoColGrid->addWidget(spC, i, 4);
    }
    // 分隔线(第 2 列)
    QFrame* vline = new QFrame(jogMergedGroup);
    vline->setFrameShape(QFrame::VLine);
    vline->setFrameShadow(QFrame::Plain);
    vline->setStyleSheet(QStringLiteral("color:#e0e0e0;background:#e0e0e0;"));
    vline->setFixedWidth(1);
    twoColGrid->addWidget(vline, 0, 2, 6, 1);
    twoColGrid->setColumnStretch(0, 0);
    twoColGrid->setColumnStretch(1, 1);
    twoColGrid->setColumnStretch(2, 0);
    twoColGrid->setColumnStretch(3, 0);
    twoColGrid->setColumnStretch(4, 1);
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

    // ★ 原点设置按钮已移至"调节"弹窗内的关节模式界面（showPointAdjustDialog 中），
    //   此处不再放置。请打开调节弹窗并切换到【正解（关节）】模式后即可看到该按钮。

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
        dlg->setWindowTitle(QStringLiteral("舵机设置"));
        dlg->resize(860, 560);
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
    // 只读展示：所有点位修改统一通过"点位管理"按钮弹出的对话框完成
    progTargets->setReadOnly(true);
    progTargets->setToolTip(QStringLiteral(
        "点位定义区（只读）：显示当前 TARGET 定义；\n"
        "如需修改请点击上方「点位管理」按钮。"));
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
    btnRunStep->setMinimumSize(52, 32);
    btnRunStep->setStyleSheet(
        QStringLiteral("QPushButton{background:#ede7f6;color:#4527a0;border:1px solid #b39ddb;border-radius:4px;"
            "padding:2px 5px;font-weight:bold;font-size:12px;}QPushButton:hover{background:#d1c4e9;}"));
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

    // "点位管理"按钮：弹出当前流程中所有引用点位的编辑对话框
    QPushButton* btnEditTargets = new QPushButton(QStringLiteral("点位管理"));
    btnEditTargets->setMinimumSize(56, 32);
    btnEditTargets->setStyleSheet(
        QStringLiteral("QPushButton{background:#fff3e0;color:#e65100;border:1px solid #ffcc80;border-radius:4px;"
            "padding:2px 5px;font-weight:bold;font-size:12px;}QPushButton:hover{background:#ffe0b2;}"));
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
    if (!ui->chkDebugMode->isChecked()) {
        QMessageBox::information(this, QStringLiteral("提示"),
            QStringLiteral("想使用该功能请在调试模式下使用"));
        return;
    }

    QDialog dlg(this);
    double step = this->ui->debugStep->text().toDouble();
    Ui::PointAdjustDialog ui;
    ui.setupUi(&dlg);

    dlg.setWindowTitle(QStringLiteral("调节"));
    dlg.resize(580, 560);
    dlg.setMinimumSize(380, 420);

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

    // ★ 原点设置按钮（仅在关节模式（正解）下显示，笛卡尔模式隐藏）
    QPushButton* btnSetOriginInDialog = new QPushButton(QStringLiteral("原点设置"), &dlg);
    btnSetOriginInDialog->setFixedHeight(40);
    btnSetOriginInDialog->setCursor(Qt::PointingHandCursor);
    btnSetOriginInDialog->setToolTip(QStringLiteral(
        "打开原点设置弹窗：查看当前 Flash 原点。\n"
        "点动到目标位姿后点\"设置机械原点\"，把\"当前Flash原点+当前DPOS\"写入 Flash（断电保持）。"));
    btnSetOriginInDialog->setStyleSheet(QStringLiteral(
        "QPushButton{background:#2e7d32;color:#fff;border:1px solid #1b5e20;"
        "border-radius:6px;font-weight:bold;font-size:14px;padding:4px 16px;}"
        "QPushButton:hover{background:#1b5e20;}"
        "QPushButton:pressed{background:#144e14;}"
    ));
    // ★ 只读显示：Flash 中当前保存的原点数据（6 个关节轴）
    QLabel* lblFlashOrigin = new QLabel(&dlg);
    lblFlashOrigin->setAlignment(Qt::AlignCenter);
    lblFlashOrigin->setWordWrap(true);
    lblFlashOrigin->setStyleSheet(QStringLiteral(
        "QLabel{font-family:Consolas,'Microsoft YaHei UI';font-size:13px;"
        "color:#555;background:#f5f5f5;border:1px solid #e0e0e0;"
        "border-radius:4px;padding:4px 8px;}"));
    lblFlashOrigin->setText(QStringLiteral("机械原点：—"));

    // 从控制器 Flash 读取原点并格式化为字符串（未连接/无数据给出占位）
    // ★ 换算显示（方案B）：机械原点 = Flash原点 + 当前DPOS
    //   Flash 存的是"原点偏移基准"；界面清零后 DPOS 是相对当前锚点的读数，
    //   显示层把两者相加换算成同一基准下的读数。
    //   写入侧（"设置机械原点"按钮）用同一公式：新Flash原点 = 旧Flash原点 + 当前DPOS。
    auto refreshFlashOrigin = [this]() -> QString {
        if (!m_robotController || !m_robotController->isConnected()) {
            return QStringLiteral("(未连接)");
        }
        QList<double> origin = m_robotController->getOriginPosition();
        if (origin.size() < 6) {
            return QStringLiteral("(无数据)");
        }
        QList<double> joints;
        if (RobotState* st = m_robotController->getState()) {
            joints = st->getAllAxisPositions();
        }
        QStringList parts;
        for (int i = 0; i < 6; ++i) {
            const double dpos = (i < joints.size()) ? joints.at(i) : 0.0;
            parts << QStringLiteral("J%1=%2").arg(i)
                         .arg(origin.at(i) + dpos, 0, 'f', 3);
        }
        return parts.join(QStringLiteral("  "));
    };

    connect(btnSetOriginInDialog, &QPushButton::clicked, &dlg, [this, &dlg, refreshFlashOrigin, lblFlashOrigin] {
        // 打开统一"原点设置"弹窗（只读显示当前 Flash 原点 + 设置机械原点 + 取消）
        showOriginDialog();
        // 弹窗关闭后刷新一次下方 Flash 原点显示（showOriginDialog 是模态阻塞的，返回即可能已写入）
        QTimer::singleShot(0, &dlg, [refreshFlashOrigin, lblFlashOrigin] {
            if (lblFlashOrigin) lblFlashOrigin->setText(QStringLiteral("机械原点：") + refreshFlashOrigin());
        });
    });

    // 刷新按钮文本/颜色 + 同步刷新轴标签 + 切换"原点设置"按钮/Flash原点显示可见性
    auto updateModeButton = [this, btnToggleMode, btnSetOriginInDialog, lblFlashOrigin, refreshFlashOrigin, lblAxis] {
        if (!m_robotController) {
            btnToggleMode->setText(QStringLiteral("未连接"));
            btnToggleMode->setStyleSheet(QStringLiteral(
                "QPushButton{background:#9e9e9e;color:#fff;border:none;"
                "border-radius:3px;font-size:11px;padding:2px 8px;}"));
            btnSetOriginInDialog->setVisible(false);  // 未连接，隐藏原点设置
            lblFlashOrigin->setVisible(false);        // 未连接，隐藏 Flash 原点
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
            // 笛卡尔模式:隐藏"原点设置"与 Flash 原点(只对关节坐标 J0~J5 有意义)
            btnSetOriginInDialog->setVisible(false);
            lblFlashOrigin->setVisible(false);
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
            // 关节模式:显示"原点设置"按钮 + Flash 原点数据（写在关节轴下的"关节这个界面中"）
            btnSetOriginInDialog->setVisible(true);
            lblFlashOrigin->setVisible(true);
            lblFlashOrigin->setText(QStringLiteral("机械原点：") + refreshFlashOrigin());
        }
    };
    updateModeButton();

    // ★ 原点设置按钮：移入底部按钮行（获取当前点位 / 运行）最左侧，
    //   即界面左下角红框位置（原 hSpacer 弹性空白区），与"获取当前点位"同行。
    //   仅在按钮行可用时插入，否则回退到 groupBox 下方独立摆放。
    if (QHBoxLayout* btnLay = ui.buttonLayout) {
        btnSetOriginInDialog->setFixedSize(120, 40);   // 与"获取当前点位"等宽，融入按钮行
        btnLay->insertWidget(0, btnSetOriginInDialog); // 插到行首（hSpacer 之前），占据左下角
    }
    // Flash 原点只读显示：紧跟在 groupBox 下方（占满整行），位于按钮行上方
    if (QVBoxLayout* mainLay = qobject_cast<QVBoxLayout*>(dlg.layout())) {
        int idx = mainLay->indexOf(ui.groupBox);
        if (idx >= 0) {
            mainLay->insertWidget(idx + 1, lblFlashOrigin);
        } else {
            if (!ui.buttonLayout) {
                mainLay->addWidget(btnSetOriginInDialog);
            }
            mainLay->addWidget(lblFlashOrigin);
        }
    }

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

/* ==================== 视觉运动 ==================== */

void MainWindow::onVisionStarted()
{
//     qDebug() << "[MainWindow] 视觉程序已启动";
    m_visionControlActive = true;
    ui->pushButton_6->setText(QStringLiteral("停止跟随"));
    if (m_visionStatusLabel) {
        m_visionStatusLabel->setText(QStringLiteral("视觉程序：运行中 (页签已切到【视觉运动】即可驱动)"));
    }
    if (m_visionLogView) {
        m_visionLogView->appendPlainText(QStringLiteral("--- 视觉程序已启动 ---"));
    }
    // 视觉跟随启动后，显示轴配置面板
    if (ui->visionGlobalSpeedBar)
        ui->visionGlobalSpeedBar->setVisible(true);
    if (ui->visionAxisGroup)
        ui->visionAxisGroup->setVisible(true);
}

void MainWindow::onVisionStopped(int exitCode)
{
//     qDebug() << "[MainWindow] 视觉程序已退出，exitCode=" << exitCode;
    m_visionControlActive = false;
    m_axisValueBuffer.clear();
    m_lastSentAnglePerAxis.clear();
    ui->pushButton_6->setText(QStringLiteral("视觉跟随"));
    if (m_visionStatusLabel) {
        m_visionStatusLabel->setText(QStringLiteral("视觉程序：已停止 (点击上方按钮启动)"));
    }
    if (m_visionLogView) {
        m_visionLogView->appendPlainText(
            QStringLiteral("--- 视觉程序已退出 (exitCode=%1) ---").arg(exitCode));
    }
}

void MainWindow::onVisionError(const QString &error)
{
    qWarning() << "[MainWindow] 视觉程序错误:" << error;
    m_visionControlActive = false;
    m_axisValueBuffer.clear();
    m_lastSentAnglePerAxis.clear();
    ui->pushButton_6->setText(QStringLiteral("视觉跟随"));
    if (ui->visionGlobalSpeedBar)
        ui->visionGlobalSpeedBar->setVisible(false);
    if (ui->visionAxisGroup)
        ui->visionAxisGroup->setVisible(false);
}

void MainWindow::onVisionMessage(const QString &channel, const QString &line)
{
//     qDebug() << "[MainWindow] vision" << channel << ":" << line;
    if (m_visionLogView) {
        m_visionLogView->appendPlainText(QStringLiteral("[%1] %2").arg(channel, line));
    }
}

/**
 * @brief 视觉多轴指令统一入口
 *
 * 每帧解析 motion_commands 数组后调用本函数 (axis 0-5, signed_angle_deg)。
 * 行为：
 * - 仅当对应 axis 的 QCheckBox 被勾选时才下发 jogJoint（用户在 pageVision 控制）；
 * - 仍沿用防抖 + 500ms 节流策略（按 axis 独立统计），避免 Python 高频 pose 把机器人抖死；
 * - 同一帧内 axis 顺序不影响，下发按 axis 升序遍历即可。
 *
 * @param commands 多轴指令列表，每条 (axis 0~5, signed_angle_deg 弧度？度)
 */
void MainWindow::onVisionMotorCommands(const QList<QPair<int, double>>& commands)
{
    if (commands.isEmpty()) return;
    if (!m_visionControlActive) {
        qWarning() << "[MainWindow] 视觉多轴指令被忽略：视觉控制未激活";
        return;
    }
    if (!m_robotController || !m_robotController->isConnected()) {
        qWarning() << "[MainWindow] 视觉多轴指令未执行：控制器未连接";
        return;
    }

    const qint64 nowMs = QDateTime::currentMSecsSinceEpoch();

    // 第一遍：逐轴滤波，收集通过所有条件的轴→目标角度
    QHash<int, double> filteredTargets;
    double maxSpeed = 0.0;

    for (const auto& cmd : commands) {
        const int axis = cmd.first;
        const double angle = cmd.second;
        if (axis < 0 || axis > 5) continue;
        if (!m_axisEnabled[axis]) continue;

        // 滑动窗口
        QList<double>& buf = m_axisValueBuffer[axis];
        buf.append(angle);
        if (buf.size() < kWindowSize) continue;
        if (buf.size() > kWindowSize) buf.removeFirst();

        // 均值 + 标准差
        double sum = 0.0;
        for (double v : buf) sum += v;
        const double mean = sum / kWindowSize;
        double sqSum = 0.0;
        for (double v : buf) {
            const double d = v - mean;
            sqSum += d * d;
        }
        const double stddev = std::sqrt(sqSum / kWindowSize);
        if (stddev > kStdDevThreshold) {
            buf.clear();
            continue;
        }

        // 200ms 节流
        const qint64 lastMs = m_lastVisionSendMsPerAxis.value(axis, 0);
        if ((nowMs - lastMs) < 200) continue;

        // 死区过滤
        const double lastSent = m_lastSentAnglePerAxis.value(axis, 0.0);
        if (qAbs(mean - lastSent) < kMinAngleDeadband) continue;

        filteredTargets[axis] = mean;
        if (m_axisSpeed[axis] > maxSpeed) maxSpeed = m_axisSpeed[axis];
    }

    // 对未通过节流/死区的轴，沿用上次下发目标，避免 jogJointBatch
    // 的 cancel+getAllAxisPositions 用真实中途位置覆盖未更新轴。
    // 仅在至少一个轴通过了完整过滤时才补齐（防止节流死循环）。
    if (filteredTargets.isEmpty()) return;

    for (int axis = 0; axis < 6; ++axis) {
        if (filteredTargets.contains(axis)) continue;
        if (!m_axisEnabled[axis]) continue;
        if (m_lastSentAnglePerAxis.contains(axis))
            filteredTargets[axis] = m_lastSentAnglePerAxis[axis];
    }

    // 第二遍：收集所有通过滤波的轴，一次批量下发，避免逐轴覆盖
    if (!m_robotController->jogJointBatch(filteredTargets, maxSpeed, 50.0, 60.0)) {
        qWarning() << "[MainWindow] jogJointBatch 失败";
        return;
    }

    // 下发成功后更新状态 + UI
    for (auto it = filteredTargets.begin(); it != filteredTargets.end(); ++it) {
        const int axis = it.key();
        const double outAngle = it.value();

        m_lastSentAnglePerAxis[axis] = outAngle;
        m_lastVisionSendMsPerAxis[axis] = nowMs;

        if (m_axisAngleLabels[axis]) {
            const QString sign = outAngle >= 0 ? QStringLiteral("+") : QString();
            m_axisAngleLabels[axis]->setText(
                QStringLiteral("%1%2°").arg(sign).arg(outAngle, 0, 'f', 3));
        }
    }

    // 日志：始终输出全部 6 轴最新值，避免逐轴零散打印
    if (m_visionLogView) {
        QStringList logItems;
        for (int axis = 0; axis < 6; ++axis) {
            if (!m_lastSentAnglePerAxis.contains(axis)) continue;
            const double outAngle = m_lastSentAnglePerAxis[axis];
            const QString sign = outAngle >= 0 ? QStringLiteral("+") : QString();
            logItems << QStringLiteral("J%1:%2%3").arg(axis).arg(sign).arg(outAngle, 0, 'f', 1);
        }
        if (logItems.size() == 6) {
            m_visionLogView->appendPlainText(
                QStringLiteral("[SEND] %1").arg(logItems.join(QStringLiteral(" "))));
        }
    }
}

/**
 * @brief pageVision 页 6 个轴勾选框回调
 * @param axis     0-5
 * @param enabled  当前勾选状态
 *
 * 仅切换 m_axisEnabled，硬件驱动由 onVisionMotorCommands 按此状态过滤；
 * 状态变化写入 visionLogView 便于观察。
 */
void MainWindow::onAxisEnableToggled(int axis, bool enabled)
{
    if (axis < 0 || axis > 5) return;
    m_axisEnabled[axis] = enabled;
    if (m_visionLogView) {
        m_visionLogView->appendPlainText(
            QStringLiteral("J%1 %2视觉控制").arg(axis).arg(enabled ? "已启用" : "已禁用"));
    }
}

/**
 * @brief 把顶部"整体速度"SpinBox 的值广播到 6 个轴的转速输入框
 *
 * 行为：
 * - 读 ui->spinVisionGlobalSpeed 的 value (1.0~500.0)
 * - 写回 m_axisSpeed[0..5] 与 6 个 QLineEdit (m_axisSpeedSpins[i])
 * - 在 visionLogView 写一行汇总，便于观察
 *
 * 单个轴的输入框仍可单独微调；总速度只在你按按钮时统一覆盖。
 */
void MainWindow::onApplyGlobalSpeedClicked()
{
    if (!ui || !ui->spinVisionGlobalSpeed) return;
    const double v = ui->spinVisionGlobalSpeed->value();
    for (int i = 0; i < 6; ++i) {
        m_axisSpeed[i] = v;
        if (m_axisSpeedSpins[i]) {
            m_axisSpeedSpins[i]->blockSignals(true);
            m_axisSpeedSpins[i]->setText(QString::number(v, 'f', 1));
            m_axisSpeedSpins[i]->blockSignals(false);
        }
    }
    if (m_visionLogView) {
        m_visionLogView->appendPlainText(
            QStringLiteral("已将所有 6 轴速度统一为 %1 °/s").arg(v, 0, 'f', 1));
    }
}

/**
 * @brief 初始化 pageVision（视觉运动）页面
 *
 * 在 ui->pageVision 已 setupUi 好的 QGroupBox visionAxisGrid 上动态添加 6 行
 * (勾选框 / 轴标签 / 实时角度 / 转速)。每个轴一行，命名与 ServoTestWidget 风格保持一致。
 * 已存在 visualAxisGrid / lblVisionStatus / visionLogView（mainwindow.ui 已建好），
 * 本函数仅做"行内控件"的 row-by-row add。
 */
void MainWindow::initVisionControlUI()
{
    if (!ui || !ui->pageVision) return;
    QGridLayout* grid = ui->visionAxisGrid;
    if (!grid) return;

    // 6 行数据行（index 0~5 → row 1~6, 与 header row 0 错开）
    static const char* kAxisNames[6] = {"J0", "J1", "J2", "J3", "J4", "J5"};
    for (int i = 0; i < 6; ++i) {
        const int row = i + 1;  // header 在 row 0

        // 列 0：勾选框
        m_axisEnableCks[i] = new QCheckBox(ui->visionAxisGroup);
        m_axisEnableCks[i]->setObjectName(QStringLiteral("chkVisionAxisJ%1").arg(i));
        m_axisEnableCks[i]->setStyleSheet(
            QStringLiteral("QCheckBox{spacing:0px;}QCheckBox::indicator{width:22px;height:22px;}"
                           "QCheckBox::indicator:unchecked{border:1px solid #90a4ae;border-radius:4px;background:#fff;}"
                           "QCheckBox::indicator:checked{border:1px solid #1565c0;border-radius:4px;background:#1565c0;image:url(:/icon/check.png);}"
                           "QCheckBox::indicator:unchecked:hover{border-color:#1565c0;}"));
        connect(m_axisEnableCks[i], &QCheckBox::toggled, this, [this, i](bool en){ onAxisEnableToggled(i, en); });
        grid->addWidget(m_axisEnableCks[i], row, 0, Qt::AlignCenter);

        // 列 1：轴标签
        m_axisNameLabels[i] = new QLabel(QString::fromUtf8(kAxisNames[i]), ui->visionAxisGroup);
        m_axisNameLabels[i]->setStyleSheet(
            QStringLiteral("color:#1565c0;font-size:16px;font-weight:bold;font-family:Consolas,'Microsoft YaHei UI';border:none;background:transparent;"));
        grid->addWidget(m_axisNameLabels[i], row, 1);

        // 列 2：实时下发角度
        m_axisAngleLabels[i] = new QLabel(QStringLiteral("-"), ui->visionAxisGroup);
        m_axisAngleLabels[i]->setStyleSheet(
            QStringLiteral("font-family:Consolas,'Microsoft YaHei UI';font-size:14px;color:#333;"
                           "background:#f5f5f5;border:1px solid #e8e8e8;border-radius:4px;padding:3px 8px;"));
        m_axisAngleLabels[i]->setMinimumWidth(110);
        m_axisAngleLabels[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        grid->addWidget(m_axisAngleLabels[i], row, 2);

        // 列 3：转速输入
        m_axisSpeedSpins[i] = new QLineEdit(QStringLiteral("25.0"), ui->visionAxisGroup);
        m_axisSpeedSpins[i]->setObjectName(QStringLiteral("lineVisionSpeedJ%1").arg(i));
        m_axisSpeedSpins[i]->setMaximumWidth(72);
        m_axisSpeedSpins[i]->setStyleSheet(
            QStringLiteral("QLineEdit{font-family:Consolas,'Microsoft YaHei UI';font-size:13px;"
                           "background:#fff;border:1px solid #d0d5dd;border-radius:4px;padding:2px 6px;}"
                           "QLineEdit:focus{border-color:#1565c0;}"));
        connect(m_axisSpeedSpins[i], &QLineEdit::editingFinished, this, [this, i]() {
            bool ok = false;
            const double v = m_axisSpeedSpins[i]->text().toDouble(&ok);
            if (ok && v > 0.0 && v <= 500.0) {
                m_axisSpeed[i] = v;
            } else {
                m_axisSpeedSpins[i]->setText(QString::number(m_axisSpeed[i], 'f', 1));
            }
        });
        grid->addWidget(m_axisSpeedSpins[i], row, 3, Qt::AlignCenter);
    }

    // 列宽分配：列 2（角度）占大头
    grid->setColumnStretch(0, 0);
    grid->setColumnStretch(1, 0);
    grid->setColumnStretch(2, 2);  // 角度
    grid->setColumnStretch(3, 0);

    // 状态标签与日志视图（.ui 已创建好，wire 到成员指针）
    m_visionStatusLabel = ui->lblVisionStatus;
    m_visionLogView = ui->visionLogView;
    if (ui->btnClearVisionLog) {
        connect(ui->btnClearVisionLog, &QPushButton::clicked, this, [this]() {
            if (m_visionLogView) m_visionLogView->clear();
        });
    }
    if (m_visionStatusLabel) {
        m_visionStatusLabel->setText(QStringLiteral("视觉程序：已停止 (点击上方按钮启动)"));
    }

    // 顶部总速度 SpinBox + "应用到全部轴" 按钮
    if (ui->spinVisionGlobalSpeed) {
        ui->spinVisionGlobalSpeed->setValue(25.0);  // 与每轴默认一致
        connect(ui->btnVisionApplySpeed, &QPushButton::clicked,
                this, &MainWindow::onApplyGlobalSpeedClicked);
    }

    // 初始隐藏关节控制面板，仅在视觉启动后显示
    if (ui->visionGlobalSpeedBar)
        ui->visionGlobalSpeedBar->setVisible(false);
    if (ui->visionAxisGroup)
        ui->visionAxisGroup->setVisible(false);
}

// ==================== 开发者账号管理 ====================

QString MainWindow::getAccountConfigPath() const
{
    // 查找 config/ 目录（与 developer_account.ini 等同级）
    QString configDir = findConfigDir();
    return configDir + "/developer_account.ini";
}
/**
 * @brief 初始化登录界面
 */
void MainWindow::initLoginUI()
{
    connect(ui->btnLogin, &QPushButton::clicked, this, &MainWindow::onBtnLogin);

    QSettings settings(getAccountConfigPath(), QSettings::IniFormat);
    settings.beginGroup("Account");
    QString savedUser = settings.value("username", "").toString();
    QString savedHash = settings.value("password_hash", "").toString();
    settings.endGroup();

    if (savedUser.isEmpty() || savedHash.isEmpty()) {
        QDir().mkpath(QFileInfo(getAccountConfigPath()).absolutePath());
        QSettings cfg(getAccountConfigPath(), QSettings::IniFormat);
        cfg.beginGroup("Account");
        cfg.setValue("username", "admin");
        QByteArray hash = QCryptographicHash::hash(
            QByteArray("hg123"), QCryptographicHash::Sha256);
        cfg.setValue("password_hash", QString::fromLatin1(hash.toHex()));
        cfg.endGroup();
        cfg.sync();
    }

    m_loggedIn = false;
    updateLoginUI();
}
/**
 * @brief 更新登录按钮状态
 */
void MainWindow::updateLoginUI()
{
    if (m_loggedIn) {
        ui->btnLogin->setText(QStringLiteral("退出登录"));
        ui->btnLogin->setStyleSheet(
            "QPushButton{font-size:12px;font-weight:bold;min-height:28px;border-radius:4px;"
            "background:#fff6f0;color:#d15f0a;"
            "border-top:2px solid #fff;border-left:2px solid #fff;"
            "border-bottom:3px solid #e67020;border-right:3px solid #e67020;}"
            "QPushButton:hover{background:#ffe9d9;}"
            "QPushButton:pressed{background:#ffe9d9;color:#d15f0a;"
            "border-top:3px solid #e67020;border-left:3px solid #e67020;"
            "border-bottom:2px solid #fff;border-right:2px solid #fff;}");
    } else {
        ui->btnLogin->setText(QStringLiteral("登录"));
        ui->btnLogin->setStyleSheet(
            "QPushButton{font-size:12px;font-weight:bold;min-height:28px;border-radius:4px;"
            "background:#f0f6fc;color:#2c62b3;"
            "border-top:2px solid #fff;border-left:2px solid #fff;"
            "border-bottom:3px solid #4078c0;border-right:3px solid #4078c0;}"
            "QPushButton:hover{background:#e1edfb;}"
            "QPushButton:pressed{background:#e1edfb;color:#2c62b3;"
            "border-top:3px solid #4078c0;border-left:3px solid #4078c0;"
            "border-bottom:2px solid #fff;border-right:2px solid #fff;}");
    }
}
/**
 * @brief 处理登录按钮点击事件
 */
void MainWindow::onBtnLogin()
{
    if (m_loggedIn) {
        // 已登录 → 登出
        m_loggedIn = false;
        updateLoginUI();
        ui->statusbar->showMessage(QStringLiteral("已退出开发者账号"));
        return;
    }

    QDialog dlg(this);
    dlg.setWindowTitle(QStringLiteral("开发者登录"));
    dlg.setFixedSize(260, 160);
    dlg.setStyleSheet("QDialog{background:#f5f6f8;}");

    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    layout->setSpacing(10);
    layout->setContentsMargins(20, 20, 20, 20);

    QLabel *titleLabel = new QLabel(QStringLiteral("请输入开发者账号"), &dlg);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size:13px;font-weight:bold;color:#333;border:none;background:transparent;");

    QLineEdit *editUser = new QLineEdit(&dlg);
    editUser->setPlaceholderText(QStringLiteral("账号"));
    editUser->setText(QStringLiteral("admin"));
    editUser->setStyleSheet("border:1px solid #ccc;border-radius:4px;padding:4px 8px;font-size:12px;min-height:24px;");

    QLineEdit *editPwd = new QLineEdit(&dlg);
    editPwd->setPlaceholderText(QStringLiteral("密码"));
    editPwd->setEchoMode(QLineEdit::Password);
    editPwd->setStyleSheet("border:1px solid #ccc;border-radius:4px;padding:4px 8px;font-size:12px;min-height:24px;");

    QPushButton *confirmBtn = new QPushButton(QStringLiteral("登录"), &dlg);
    confirmBtn->setStyleSheet(
        "QPushButton{font-size:12px;font-weight:bold;min-height:28px;border-radius:4px;"
        "background:#f0f6fc;color:#2c62b3;border-top:2px solid #fff;border-left:2px solid #fff;"
        "border-bottom:3px solid #4078c0;border-right:3px solid #4078c0;}"
        "QPushButton:hover{background:#e1edfb;}"
        "QPushButton:pressed{background:#e1edfb;border-top:3px solid #4078c0;"
        "border-left:3px solid #4078c0;border-bottom:2px solid #fff;border-right:2px solid #fff;}");

    layout->addWidget(titleLabel);
    layout->addWidget(editUser);
    layout->addWidget(editPwd);
    layout->addWidget(confirmBtn);

    QObject::connect(confirmBtn, &QPushButton::clicked, &dlg, [&dlg, editUser, editPwd, this]() {
        QString username = editUser->text().trimmed();
        QString password = editPwd->text();

        if (username.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(&dlg, QStringLiteral("登录失败"), QStringLiteral("账号和密码不能为空"));
            return;
        }

        QSettings settings(getAccountConfigPath(), QSettings::IniFormat);
        settings.beginGroup("Account");
        QString savedUser = settings.value("username", "").toString();
        QString savedHash = settings.value("password_hash", "").toString();
        settings.endGroup();

        QByteArray inputHash = QCryptographicHash::hash(
            password.toUtf8(), QCryptographicHash::Sha256);
        QString inputHashStr = QString::fromLatin1(inputHash.toHex());

        if (username == savedUser && inputHashStr == savedHash) {
            m_loggedIn = true;
            updateLoginUI();
            ui->statusbar->showMessage(QStringLiteral("开发者登录成功"));
            dlg.accept();
        } else {
            QMessageBox::warning(&dlg, QStringLiteral("登录失败"), QStringLiteral("账号或密码错误"));
            editPwd->clear();
            editPwd->setFocus();
        }
    });

    // 回车快速登录
    QObject::connect(editPwd, &QLineEdit::returnPressed, confirmBtn, &QPushButton::click);

    // 弹出登录框前把面板上的按钮焦点去掉
    ui->btnLogin->setFocusPolicy(Qt::NoFocus);
    dlg.exec();
    ui->btnLogin->setFocusPolicy(Qt::StrongFocus);

    // 取消/关闭弹窗时焦点回到登录按钮
    ui->btnLogin->setFocus();
}

/**
 * @brief 检查登录状态
 * @return true 如果已登录，否则返回 false
 */
bool MainWindow::checkLoginGate()
{
    if (m_loggedIn) return true;
    QMessageBox::warning(this, QStringLiteral("需要登录"),
                         QStringLiteral("请先在「开发者」面板中登录账号后使用此功能"));
    return false;
}

/* ==================== 视觉运动 ==================== */
void MainWindow::initVisionAutoLoad()
{
    if (!m_vmWindow) {
        m_vmWindow = new VmWindow();
        connect(m_vmWindow->vmControl(), &VMControl::dataReady,
                m_visionLink, &VisionRobotLink::onVisionDataEvent);
    }
    // VmWindow 内部提供完整方案加载和执行界面，用户手动操作
    m_vmWindow->show();
}

/* ==================== 画板 ==================== */

void MainWindow::onRequestInverseKinematics()
{
    if (!m_robotController || !m_robotController->isConnected()) {
        qDebug() << "[画板] 控制器未连接，无法切换逆解模式";
        return;
    }
    qDebug() << "[画板] 切换到逆解模式...";
    if (m_robotController->switchToCartesianMode()) {
        qDebug() << "[画板] 已切换到逆解模式";
    } else {
        qDebug() << "[画板] 切换到逆解模式失败";
    }
}

void MainWindow::onRequestStopMotion()
{
    if (!m_robotController || !m_robotController->isConnected()) {
        qDebug() << "[画板] 控制器未连接，无法停止";
        return;
    }
    qDebug() << "[画板] 急停...";
    m_robotController->executeSafeStop();
}

void MainWindow::onMotionParamsReceived(const DrawMotionParams &params)
{
    m_motionParams = params;
    qDebug() << "[画板] 运动参数已更新: speed=" << params.speed
             << "accel=" << params.acceleration
             << "blend=" << params.blendMode;
}

/**
 * @brief 处理画板轨迹点位，执行机器人绘制运动
 *
 * 从画板组件接收点位数据，控制机械臂沿指定轨迹运动。函数执行以下流程：
 * - 检查控制器连接状态，未连接则直接返回
 * - 统计并打印轨迹组数和总点数
 * - 切换到笛卡尔（逆解）模式
 * - 设置虚拟轴和关节轴的运动参数
 * - 逐组执行轨迹，支持两种运动模式：
 *   - Merge 连续模式：启用速度融合，适合平滑轨迹绘制
 *   - 逐点模式：每个点执行一次定位，轨迹精确但速度有停顿
 * - 组间自动抬笔避障（抬升10mm）
 * - 执行完成后归位到最后一个轨迹点上方
 *
 * @param pointGroups 三维点位组，外层列表表示轨迹分组，内层列表为组内连续点位
 * @param rx 末端执行器绕X轴旋转角度（弧度）
 * @param ry 末端执行器绕Y轴旋转角度（弧度）
 * @param rz 末端执行器绕Z轴旋转角度（弧度）
 * @note 运动速度、加速度等参数从 m_motionParams 成员获取
 * @note 关节轴速度使用固定值 200.0，不受画板参数影响
 */
void MainWindow::onDrawingPointsReceived(const QVector<QVector<QVector3D>> &pointGroups,
                                          double rx, double ry, double rz)
{
    if (pointGroups.isEmpty()) return;
    if (!m_robotController || !m_robotController->isConnected()) {
        qDebug() << "[画板] 控制器未连接，跳过轨迹执行";
        return;
    }

    int totalPoints = 0;
    for (const auto &g : pointGroups) totalPoints += g.size();
    qDebug() << "[画板] 执行" << pointGroups.size() << "组轨迹, 共" << totalPoints << "个点位";

    // 切换到逆解模式
    m_robotController->switchToCartesianMode();
    QThread::msleep(100);

    ZmotionAPI *api = m_robotController->getMotionController()->getZmotionAPI();
    const QList<int> vAxes = {6, 7, 8, 9, 10, 11};
    const QList<int> jAxes = {0, 1, 2, 3, 4, 5};
    float speed  = static_cast<float>(m_motionParams.speed);
    float accel  = static_cast<float>(m_motionParams.acceleration);
    float decel  = static_cast<float>(m_motionParams.deceleration);
    float sramp  = static_cast<float>(m_motionParams.sramp);
    int blendMode = m_motionParams.blendMode;

    // 设置虚拟轴运动参数
    QList<double> s(vAxes.size(), speed);
    QList<double> a(vAxes.size(), accel);
    QList<double> d(vAxes.size(), decel);
    QList<double> sr(vAxes.size(), sramp);
    api->setAxisSpeed(vAxes, s);
    api->setAxisAccel(vAxes, a);
    api->setAxisDecel(vAxes, d);
    api->setAxisSramp(vAxes, sr);

    // 设置关节轴速度（用固定值，不随画板）
    QList<double> js(jAxes.size(), 200.0);
    api->setAxisSpeed(jAxes, js);
    api->setAxisAccel(jAxes, js);
    api->setAxisDecel(jAxes, js);

    // Force_Speed
    api->setAxisParamValue(vAxes[0], AxisParamsCommand::Force_Speed, speed);

    QList<int> allAxes = {6, 7, 8, 9, 10, 11};

    // 逐组执行
    for (int g = 0; g < pointGroups.size(); ++g) {
        const auto &group = pointGroups[g];
        if (group.isEmpty()) continue;

        if (g > 0) {
            // 组间断笔抬笔 10mm
            auto posZ = api->getAxisPosition(vAxes[2]);
            auto posX = api->getAxisPosition(vAxes[0]);
            auto posY = api->getAxisPosition(vAxes[1]);
            float liftZ = posZ.axisPosition + 10.0f;
            QList<float> liftPos = { posX.axisPosition, posY.axisPosition,
                                      liftZ, (float)rx, (float)ry, (float)rz };
            api->directMoveAbsolute(vAxes, liftPos, speed, accel, decel);
            api->waitForAxesIdle(vAxes, 30000);
        }

        float sx = (float)group[0].x(), sy = (float)group[0].y(), sz = (float)group[0].z();

        // 先走到起点上方 10mm
        QList<float> abovePos = { sx, sy, sz + 10.0f, (float)rx, (float)ry, (float)rz };
        api->directMoveAbsolute(allAxes, abovePos, speed, accel, decel);
        api->waitForAxesIdle(allAxes, 30000);

        if (blendMode > 0) {
            // Merge 连续模式
            for (int ax : allAxes) api->directSetMerge(ax, true);
            api->directSetCornerMode(allAxes[0], 2);

            QList<float> firstPos = { sx, sy, sz, (float)rx, (float)ry, (float)rz };
            api->directMoveAbsolute(allAxes, firstPos, speed, accel, decel);
            for (int idx = 1; idx < group.size(); idx++) {
                QList<float> pos = { (float)group[idx].x(), (float)group[idx].y(),
                                     (float)group[idx].z(), (float)rx, (float)ry, (float)rz };
                api->directMoveAbsolute(allAxes, pos, speed, accel, decel);
                QCoreApplication::processEvents();
            }
            api->waitForAxesIdle(allAxes, 30000);
            for (int ax : allAxes) api->directSetMerge(ax, false);
        } else {
            // 逐点模式
            QList<float> firstPos = { sx, sy, sz, (float)rx, (float)ry, (float)rz };
            api->directMoveAbsolute(allAxes, firstPos, speed, accel, decel);
            api->waitForAxesIdle(allAxes, 30000);
            for (int idx = 1; idx < group.size(); idx++) {
                QList<float> pos = { (float)group[idx].x(), (float)group[idx].y(),
                                     (float)group[idx].z(), (float)rx, (float)ry, (float)rz };
                api->directMoveAbsolute(allAxes, pos, speed, accel, decel);
                api->waitForAxesIdle(allAxes, 30000);
            }
        }
    }

    // 归位：移动到最后一个轨迹点的正上方 z=420
    api->singleAxisCancel(allAxes, 0);
    QThread::msleep(50);
    const auto &lastGroup = pointGroups.last();
    float lastX = (float)lastGroup.last().x();
    float lastY = (float)lastGroup.last().y();
    QList<float> homePos = { lastX, lastY, (float)lastGroup.last().z() + 20.0f, (float)rx, (float)ry, (float)rz };
    api->directMoveAbsolute(allAxes, homePos, speed, accel, decel);
    qDebug() << "[画板] 轨迹执行完成，已归位到最后一个轨迹点上方";
}

/**
 * @brief 处理接收到的圆形绘制请求
 *
 * 将圆形离散化为采样点集，并使用工件坐标系姿态执行绘制。
 *
 * @param centers 圆心坐标数组，包含所有圆形的 3D 坐标
 * @param radii 半径数组，与 centers 对应每个圆形的半径
 *
 * @note 每个圆形将被离散化为 72 个采样点（N=72）
 * @note 使用 m_workPieceCoord 获取工件坐标系姿态参数（rx, ry, rz）
 */
void MainWindow::onDrawingCirclesReceived(const QVector<QVector3D> &centers,
                                           const QVector<double> &radii)
{
    if (centers.isEmpty()) return;
    qDebug() << "[画板] 收到" << centers.size() << "个圆形，离散为点集执行";

    const int N = 72; // 每圆采样点数
    QVector<QVector<QVector3D>> pointGroups;

    for (int i = 0; i < centers.size(); ++i) {
        float cx = (float)centers[i].x(), cy = (float)centers[i].y(), cz = (float)centers[i].z();
        float r  = (float)radii[i];
        QVector<QVector3D> pts;
        for (int j = 0; j <= N; ++j) {
            double angle = 2.0 * M_PI * j / N;
            pts.append(QVector3D(cx + r * (float)qCos(angle),
                                  cy + r * (float)qSin(angle), cz));
        }
        pointGroups.append(pts);
    }

    // 使用工件坐标系姿态
    double rx = m_workPieceCoord.getRX();
    double ry = m_workPieceCoord.getRY();
    double rz = m_workPieceCoord.getRZ();
    onDrawingPointsReceived(pointGroups, rx, ry, rz);
}

/**
 * @brief 处理画板弧线数据，转换为点集后执行运动
 *
 * 使用 de Casteljau 二次贝塞尔算法逼近圆弧，将每条弧线离散为 72 个采样点，
 * 坐标经过工件坐标系转换后调用 onDrawingPointsReceived 执行轨迹运动。
 *
 * @param starts 弧线起点集合
 * @param mids   弧线中点集合
 * @param ends   弧线终点集合
 */
void MainWindow::onDrawingArcsReceived(const QVector<QVector3D> &starts,
                                        const QVector<QVector3D> &mids,
                                        const QVector<QVector3D> &ends)
{
    if (starts.isEmpty()) return;
    qDebug() << "[画板] 收到" << starts.size() << "条弧线，离散为点集执行";

    const int N = 72; // 每弧线采样点数
    QVector<QVector<QVector3D>> pointGroups;

    for (int i = 0; i < starts.size(); ++i) {
        float sx = starts[i].x(), sy = starts[i].y(), sz = starts[i].z();
        float mx = mids[i].x(),  my = mids[i].y();
        float ex = ends[i].x(),   ey = ends[i].y(), ez = ends[i].z();

        // 用 de Casteljau 二次贝塞尔逼近圆弧
        QVector<QVector3D> pts;
        for (int j = 0; j <= N; ++j) {
            float t = (float)j / N;
            float it = 1.0f - t;
            float x = it*it * sx + 2.0f*it*t * mx + t*t * ex;
            float y = it*it * sy + 2.0f*it*t * my + t*t * ey;
            float z = it * sz + t * ez;
            pts.append(QVector3D(x, y, z));
        }
        pointGroups.append(pts);
    }

    double rx = m_workPieceCoord.getRX();
    double ry = m_workPieceCoord.getRY();
    double rz = m_workPieceCoord.getRZ();
    onDrawingPointsReceived(pointGroups, rx, ry, rz);
}

/**
 * 工件坐标系设置对话框
 *
 * 打开工件坐标系设置窗口，允许用户通过三点法（原点、X轴方向点、XY平面点）
 * 定义工件坐标系。功能包括：
 * - 三个坐标点的6自由度（X, Y, Z, RX, RY, RZ）参数设置
 * - 获取机器人当前位置到指定点位
 * - 点位运行：移动机器人到指定位置
 * - 保存坐标系配置到JSON文件
 * - 从JSON文件加载已有坐标系
 * - 确认后激活工件坐标系并在绘图窗口显示
 *
 * @note 函数内部会验证机器人控制器连接状态，未连接时提示用户
 * @note 坐标系文件保存在程序目录下的"../坐标系管理文件"文件夹中
 */
void MainWindow::onWorkpieceCoordAction()
{
    if (!m_robotController || !m_robotController->isConnected()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请先连接机器人控制器"));
        return;
    }

    ZmotionAPI *api = m_robotController->getMotionController()->getZmotionAPI();
    QList<int> vAxes = {6, 7, 8, 9, 10, 11};

    auto getCurrentPos = [=](QDoubleSpinBox *x, QDoubleSpinBox *y, QDoubleSpinBox *z,
                              QDoubleSpinBox *rx, QDoubleSpinBox *ry, QDoubleSpinBox *rz) {
        for (int i = 0; i < 6; ++i) {
            auto r = api->getAxisPosition(vAxes[i]);
            float val = r.success ? r.axisPosition : 0.0f;
            (i == 0 ? x : i == 1 ? y : i == 2 ? z : i == 3 ? rx : i == 4 ? ry : rz)->setValue(val);
        }
    };

    auto moveToPoint = [=](QDoubleSpinBox *x, QDoubleSpinBox *y, QDoubleSpinBox *z,
                            QDoubleSpinBox *rx, QDoubleSpinBox *ry, QDoubleSpinBox *rz) {
        m_robotController->switchToCartesianMode();
        QThread::msleep(100);
        QList<float> pos = { (float)x->value(), (float)y->value(), (float)z->value(),
                              (float)rx->value(), (float)ry->value(), (float)rz->value() };
        api->directMoveAbsolute(vAxes, pos, 50.0f, 200.0f, 200.0f);
        api->waitForAxesIdle(vAxes, 30000);
    };

    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle(QStringLiteral("工件坐标系设置"));
    dlg->resize(550, 600);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *mainLayout = new QVBoxLayout(dlg);

    // 名称
    QGroupBox *nameGrp = new QGroupBox(QStringLiteral("坐标系名称"));
    QHBoxLayout *nameLay = new QHBoxLayout(nameGrp);
    QLineEdit *editName = new QLineEdit(QStringLiteral("工件坐标系1"));
    nameLay->addWidget(new QLabel(QStringLiteral("名称:")));
    nameLay->addWidget(editName);
    mainLayout->addWidget(nameGrp);

    // 创建一行输入框 + 获取/运行按钮
    auto makePointGroup = [=](const QString &title, double defX, double defY, double defZ) {
        QGroupBox *grp = new QGroupBox(title);
        QVBoxLayout *vlay = new QVBoxLayout(grp);
        QGridLayout *grid = new QGridLayout();
        QStringList labels = {"X", "Y", "Z", "RX", "RY", "RZ"};
        QDoubleSpinBox *spins[6];
        for (int i = 0; i < 6; ++i) {
            grid->addWidget(new QLabel(labels[i]), 0, i * 2);
            auto *sb = new QDoubleSpinBox();
            sb->setDecimals(3);
            sb->setRange(-9999, 9999);
            spins[i] = sb;
            grid->addWidget(sb, 1, i * 2);
            if (i < 5) grid->addItem(new QSpacerItem(10, 0), 0, i * 2 + 1);
        }
        spins[0]->setValue(defX);
        spins[1]->setValue(defY);
        spins[2]->setValue(defZ);
        vlay->addLayout(grid);

        QHBoxLayout *btnLay = new QHBoxLayout();
        QPushButton *btnGet = new QPushButton(QStringLiteral("获取当前位置"));
        QPushButton *btnMove = new QPushButton(QStringLiteral("点位运行"));
        btnGet->setStyleSheet("background-color: #2196F3; color: white; padding: 5px 12px;");
        btnMove->setStyleSheet("background-color: #FF9800; color: white; padding: 5px 12px;");
        btnLay->addWidget(btnGet);
        btnLay->addWidget(btnMove);
        btnLay->addStretch();
        vlay->addLayout(btnLay);

        QObject::connect(btnGet, &QPushButton::clicked, [=]() {
            getCurrentPos(spins[0], spins[1], spins[2], spins[3], spins[4], spins[5]);
        });
        QObject::connect(btnMove, &QPushButton::clicked, [=]() {
            moveToPoint(spins[0], spins[1], spins[2], spins[3], spins[4], spins[5]);
        });

        return std::make_tuple(grp, spins[0], spins[1], spins[2], spins[3], spins[4], spins[5]);
    };

    auto [p1Grp, p1x, p1y, p1z, p1rx, p1ry, p1rz] =
        makePointGroup(QStringLiteral("点1 (原点)"), 0, 0, 0);
    auto [p2Grp, p2x, p2y, p2z, p2rx, p2ry, p2rz] =
        makePointGroup(QStringLiteral("点2 (X轴方向)"), 100, 0, 0);
    auto [p3Grp, p3x, p3y, p3z, p3rx, p3ry, p3rz] =
        makePointGroup(QStringLiteral("点3 (XY平面)"), 0, 100, 0);

    // 填入当前坐标系已有值
    if (m_workPieceCoord.isValid()) {
        auto p1 = m_workPieceCoord.getPoint1(), p2 = m_workPieceCoord.getPoint2(),
             p3 = m_workPieceCoord.getPoint3();
        p1x->setValue(p1.x()); p1y->setValue(p1.y()); p1z->setValue(p1.z());
        p2x->setValue(p2.x()); p2y->setValue(p2.y()); p2z->setValue(p2.z());
        p3x->setValue(p3.x()); p3y->setValue(p3.y()); p3z->setValue(p3.z());
        p1rx->setValue(m_workPieceCoord.getRXOriginal());
        p1ry->setValue(m_workPieceCoord.getRYOriginal());
        p1rz->setValue(m_workPieceCoord.getRZOriginal());
        editName->setText(m_workPieceCoord.getName());
    }

    mainLayout->addWidget(p1Grp);
    mainLayout->addWidget(p2Grp);
    mainLayout->addWidget(p3Grp);

    // 按钮栏
    QHBoxLayout *actLay = new QHBoxLayout();
    QPushButton *btnLoad = new QPushButton(QStringLiteral("加载坐标系"));
    QPushButton *btnSave = new QPushButton(QStringLiteral("保存坐标系"));
    QPushButton *btnConfirm = new QPushButton(QStringLiteral("确认设置"));
    QPushButton *btnCancel = new QPushButton(QStringLiteral("取消"));
    btnLoad->setStyleSheet("background-color: #1976D2; color: white; padding: 8px 16px;");
    btnSave->setStyleSheet("background-color: #FF9800; color: white; padding: 8px 16px;");
    btnConfirm->setStyleSheet("background-color: #4CAF50; color: white; padding: 8px 16px;");
    btnCancel->setStyleSheet("background-color: #9E9E9E; color: white; padding: 8px 16px;");
    actLay->addWidget(btnLoad);
    actLay->addWidget(btnSave);
    actLay->addStretch();
    actLay->addWidget(btnConfirm);
    actLay->addWidget(btnCancel);
    mainLayout->addLayout(actLay);

    // 保存/加载目录
    QString coordDir = QCoreApplication::applicationDirPath() + "/../坐标系管理文件";
    QDir().mkpath(coordDir);

    // 保存
    QObject::connect(btnSave, &QPushButton::clicked, dlg, [=]() {
        QJsonObject json;
        json["name"] = editName->text();
        QJsonObject jp1, jp2, jp3;
        auto savePoint = [](QJsonObject &jp, QDoubleSpinBox *x, QDoubleSpinBox *y,
                             QDoubleSpinBox *z, QDoubleSpinBox *rx, QDoubleSpinBox *ry,
                             QDoubleSpinBox *rz) {
            jp["x"] = x->value(); jp["y"] = y->value(); jp["z"] = z->value();
            jp["rx"] = rx->value(); jp["ry"] = ry->value(); jp["rz"] = rz->value();
        };
        savePoint(jp1, p1x, p1y, p1z, p1rx, p1ry, p1rz);
        savePoint(jp2, p2x, p2y, p2z, p2rx, p2ry, p2rz);
        savePoint(jp3, p3x, p3y, p3z, p3rx, p3ry, p3rz);
        json["point1"] = jp1; json["point2"] = jp2; json["point3"] = jp3;
        QString path = coordDir + "/" + editName->text() + ".json";
        QFile f(path);
        if (f.open(QIODevice::WriteOnly)) {
            f.write(QJsonDocument(json).toJson(QJsonDocument::Indented));
            f.close();
            QMessageBox::information(dlg, QStringLiteral("已保存"),
                                     QStringLiteral("已保存到:\n%1").arg(path));
        }
    });

    // 加载
    QObject::connect(btnLoad, &QPushButton::clicked, dlg, [=]() {
        QString path = QFileDialog::getOpenFileName(dlg, QStringLiteral("加载坐标系"),
                                                     coordDir, QStringLiteral("JSON (*.json)"));
        if (path.isEmpty()) return;
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly)) return;
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        f.close();
        QJsonObject json = doc.object();
        editName->setText(json["name"].toString());
        auto fill = [](QJsonObject jp, QDoubleSpinBox *x, QDoubleSpinBox *y,
                        QDoubleSpinBox *z, QDoubleSpinBox *rx, QDoubleSpinBox *ry,
                        QDoubleSpinBox *rz) {
            x->setValue(jp["x"].toDouble()); y->setValue(jp["y"].toDouble());
            z->setValue(jp["z"].toDouble());
            rx->setValue(jp["rx"].toDouble()); ry->setValue(jp["ry"].toDouble());
            rz->setValue(jp["rz"].toDouble());
        };
        fill(json["point1"].toObject(), p1x, p1y, p1z, p1rx, p1ry, p1rz);
        fill(json["point2"].toObject(), p2x, p2y, p2z, p2rx, p2ry, p2rz);
        fill(json["point3"].toObject(), p3x, p3y, p3z, p3rx, p3ry, p3rz);

        // 自动激活
        m_workPieceCoord.setOriginalPoints(
            QVector3D(p1x->value(), p1y->value(), p1z->value()),
            QVector3D(p2x->value(), p2y->value(), p2z->value()),
            QVector3D(p3x->value(), p3y->value(), p3z->value()),
            p1rx->value(), p1ry->value(), p1rz->value());
        m_workPieceCoord.setName(editName->text());
        m_workPieceCoord.calculateFromThreePoints(
            QVector3D(p1x->value(), p1y->value(), p1z->value()),
            QVector3D(p2x->value(), p2y->value(), p2z->value()),
            QVector3D(p3x->value(), p3y->value(), p3z->value()));
        if (m_drawingWindow) {
            m_drawingWindow->setDefaultPose(m_workPieceCoord.getRX(),
                                             m_workPieceCoord.getRY(),
                                             m_workPieceCoord.getRZ());
        }
        QMessageBox::information(dlg, QStringLiteral("已加载"),
            QStringLiteral("工件坐标系 \"%1\" 已激活\n原点: (%2, %3, %4)")
                .arg(editName->text())
                .arg(m_workPieceCoord.getOrigin().x(), 0, 'f', 2)
                .arg(m_workPieceCoord.getOrigin().y(), 0, 'f', 2)
                .arg(m_workPieceCoord.getOrigin().z(), 0, 'f', 2));
    });

    // 确认
    QObject::connect(btnConfirm, &QPushButton::clicked, dlg, [=]() {
        m_workPieceCoord.setOriginalPoints(
            QVector3D(p1x->value(), p1y->value(), p1z->value()),
            QVector3D(p2x->value(), p2y->value(), p2z->value()),
            QVector3D(p3x->value(), p3y->value(), p3z->value()),
            p1rx->value(), p1ry->value(), p1rz->value());
        m_workPieceCoord.setName(editName->text());
        m_workPieceCoord.calculateFromThreePoints(
            QVector3D(p1x->value(), p1y->value(), p1z->value()),
            QVector3D(p2x->value(), p2y->value(), p2z->value()),
            QVector3D(p3x->value(), p3y->value(), p3z->value()));
        if (!m_workPieceCoord.isValid()) {
            QMessageBox::warning(dlg, QStringLiteral("错误"),
                                 QStringLiteral("三点计算失败，请检查输入点是否共线或重合"));
            return;
        }
        if (m_drawingWindow) {
            m_drawingWindow->setDefaultPose(m_workPieceCoord.getRX(),
                                             m_workPieceCoord.getRY(),
                                             m_workPieceCoord.getRZ());
            // 在画板上绘制坐标系区域
            QVector3D p1 = m_workPieceCoord.getPoint1();
            QVector3D p2 = m_workPieceCoord.getPoint2();
            QVector3D p3 = m_workPieceCoord.getPoint3();
            QVector3D xAxis = m_workPieceCoord.getXAxis();
            QVector3D yAxis = m_workPieceCoord.getYAxis();
            auto toCanvas = [&](const QVector3D &pt) -> QPointF {
                QVector3D d = pt - p1;
                return QPointF(QVector3D::dotProduct(d, xAxis),
                               QVector3D::dotProduct(d, yAxis));
            };
            QPointF c1 = toCanvas(p1);
            QPointF c2 = toCanvas(p2);
            QPointF c3 = toCanvas(p3);
            m_drawingWindow->drawWorkpieceCoordRegion(c1, c2, c3);
        }
        QMessageBox::information(dlg, QStringLiteral("已设置"),
            QStringLiteral("工件坐标系 \"%1\" 已激活\n"
                           "原点: (%2, %3, %4)\n"
                           "X轴方向: (%5, %6, %7)\nZ轴方向: (%8, %9, %10)")
                .arg(editName->text())
                .arg(m_workPieceCoord.getOrigin().x(), 0, 'f', 2)
                .arg(m_workPieceCoord.getOrigin().y(), 0, 'f', 2)
                .arg(m_workPieceCoord.getOrigin().z(), 0, 'f', 2)
                .arg(m_workPieceCoord.getXAxis().x(), 0, 'f', 4)
                .arg(m_workPieceCoord.getXAxis().y(), 0, 'f', 4)
                .arg(m_workPieceCoord.getXAxis().z(), 0, 'f', 4)
                .arg(m_workPieceCoord.getZAxis().x(), 0, 'f', 4)
                .arg(m_workPieceCoord.getZAxis().y(), 0, 'f', 4)
                .arg(m_workPieceCoord.getZAxis().z(), 0, 'f', 4));
        dlg->close();
    });

    QObject::connect(btnCancel, &QPushButton::clicked, dlg, &QDialog::close);

    dlg->show();
}
