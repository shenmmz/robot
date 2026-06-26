#include "MainWindow.h"

#include "JsonProtocol.h"
#include "LocalCommBridge.h"
#include "PythonProcessController.h"

#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QDir>
#include <QFileDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QFileInfo>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QSplitter>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTime>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_bridge(new LocalCommBridge(this))
    , m_python(new PythonProcessController(this))
{
    setWindowTitle(QStringLiteral("VisualRoboticArm - Qt 主控骨架"));
    resize(1280, 820);

    buildUi();
    loadLaunchSettings();
    bindSignals();

    if (!m_bridge->startServer(m_hostEdit->text(), static_cast<quint16>(m_portSpin->value()))) {
        appendLog(QStringLiteral("ERR"), QStringLiteral("本地监听启动失败"));
    }

    appendLog(QStringLiteral("SYS"), QStringLiteral("Qt 主控已启动，等待 Python 子进程连接"));

    m_lastMessageTimer.start();
    m_watchdog.setInterval(500);
    connect(&m_watchdog, &QTimer::timeout, this, &MainWindow::updateWatchdog);
    m_watchdog.start();

    if (m_autoStartCheck->isChecked()) {
        QTimer::singleShot(300, this, &MainWindow::startPython);
    }
}

void MainWindow::buildUi()
{
    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(12, 12, 12, 12);
    rootLayout->setSpacing(10);

    auto *splitter = new QSplitter(Qt::Horizontal, central);
    rootLayout->addWidget(splitter, 1);

    auto *controlPanel = new QWidget(splitter);
    controlPanel->setMinimumWidth(420);
    auto *controlLayout = new QVBoxLayout(controlPanel);
    controlLayout->setContentsMargins(0, 0, 0, 0);
    controlLayout->setSpacing(10);

    auto *launchGroup = new QGroupBox(QStringLiteral("联合启动配置"), controlPanel);
    auto *launchForm = new QFormLayout(launchGroup);
    launchForm->setLabelAlignment(Qt::AlignRight);

    m_pythonExeEdit = new QLineEdit(defaultPythonExecutable(), launchGroup);
    m_projectRootEdit = new QLineEdit(defaultProjectRoot(), launchGroup);
    m_browsePythonButton = new QPushButton(QStringLiteral("浏览"), launchGroup);
    m_browseProjectButton = new QPushButton(QStringLiteral("浏览"), launchGroup);

    auto *pythonExeRow = new QWidget(launchGroup);
    auto *pythonExeLayout = new QHBoxLayout(pythonExeRow);
    pythonExeLayout->setContentsMargins(0, 0, 0, 0);
    pythonExeLayout->setSpacing(6);
    pythonExeLayout->addWidget(m_pythonExeEdit, 1);
    pythonExeLayout->addWidget(m_browsePythonButton);

    auto *projectRootRow = new QWidget(launchGroup);
    auto *projectRootLayout = new QHBoxLayout(projectRootRow);
    projectRootLayout->setContentsMargins(0, 0, 0, 0);
    projectRootLayout->setSpacing(6);
    projectRootLayout->addWidget(m_projectRootEdit, 1);
    projectRootLayout->addWidget(m_browseProjectButton);

    m_moduleEdit = new QLineEdit(QStringLiteral("main"), launchGroup);
    m_hostEdit = new QLineEdit(QStringLiteral("127.0.0.1"), launchGroup);
    m_portSpin = new QSpinBox(launchGroup);
    m_portSpin->setRange(1024, 65535);
    m_portSpin->setValue(5001);
    m_clientNameEdit = new QLineEdit(QStringLiteral("qt-main"), launchGroup);
    m_autoStartCheck = new QCheckBox(QStringLiteral("启动后自动拉起 Python"), launchGroup);
    m_autoStartCheck->setChecked(true);

    launchForm->addRow(QStringLiteral("Python 可执行文件"), pythonExeRow);
    launchForm->addRow(QStringLiteral("Python 工作目录"), projectRootRow);
    launchForm->addRow(QStringLiteral("模块入口"), m_moduleEdit);
    launchForm->addRow(QStringLiteral("监听地址"), m_hostEdit);
    launchForm->addRow(QStringLiteral("监听端口"), m_portSpin);
    launchForm->addRow(QStringLiteral("客户端名"), m_clientNameEdit);
    launchForm->addRow(QString(), m_autoStartCheck);

    auto *buttonGroup = new QGroupBox(QStringLiteral("控制"), controlPanel);
    auto *buttonRow = new QHBoxLayout(buttonGroup);
    m_startButton = new QPushButton(QStringLiteral("启动 Python"), buttonGroup);
    m_stopButton = new QPushButton(QStringLiteral("停止 Python"), buttonGroup);
    m_restartButton = new QPushButton(QStringLiteral("重启 Python"), buttonGroup);
    buttonRow->addWidget(m_startButton);
    buttonRow->addWidget(m_stopButton);
    buttonRow->addWidget(m_restartButton);

    auto *statusGroup = new QGroupBox(QStringLiteral("状态"), controlPanel);
    auto *statusGrid = new QGridLayout(statusGroup);
    m_serverStatusLabel = new QLabel(QStringLiteral("未启动"), statusGroup);
    m_pythonStatusLabel = new QLabel(QStringLiteral("未启动"), statusGroup);
    m_linkStatusLabel = new QLabel(QStringLiteral("未连接"), statusGroup);
    m_lastMessageLabel = new QLabel(QStringLiteral("-"), statusGroup);
    m_connectionLabel = new QLabel(QStringLiteral("-"), statusGroup);
    m_poseLabel = new QLabel(QStringLiteral("等待姿态数据"), statusGroup);
    statusGrid->addWidget(new QLabel(QStringLiteral("Qt 服务"), statusGroup), 0, 0);
    statusGrid->addWidget(m_serverStatusLabel, 0, 1);
    statusGrid->addWidget(new QLabel(QStringLiteral("Python"), statusGroup), 1, 0);
    statusGrid->addWidget(m_pythonStatusLabel, 1, 1);
    statusGrid->addWidget(new QLabel(QStringLiteral("链路"), statusGroup), 2, 0);
    statusGrid->addWidget(m_linkStatusLabel, 2, 1);
    statusGrid->addWidget(new QLabel(QStringLiteral("连接信息"), statusGroup), 3, 0);
    statusGrid->addWidget(m_connectionLabel, 3, 1);
    statusGrid->addWidget(new QLabel(QStringLiteral("最近消息"), statusGroup), 4, 0);
    statusGrid->addWidget(m_lastMessageLabel, 4, 1);
    statusGrid->addWidget(new QLabel(QStringLiteral("姿态状态"), statusGroup), 5, 0);
    statusGrid->addWidget(m_poseLabel, 5, 1);

    auto *poseGroup = new QGroupBox(QStringLiteral("姿态数据"), controlPanel);
    auto *poseLayout = new QVBoxLayout(poseGroup);
    m_poseView = new QTextEdit(poseGroup);
    m_poseView->setReadOnly(true);
    m_poseView->setMinimumHeight(160);
    m_poseView->setStyleSheet(QStringLiteral("background-color:#FFFFFF; color:#202124;"));
    m_poseView->setHtml(QStringLiteral("<div style='color:#888'>等待 Python 推送 pose</div>"));
    poseLayout->addWidget(m_poseView);

    auto *commandGroup = new QGroupBox(QStringLiteral("命令"), controlPanel);
    auto *commandLayout = new QHBoxLayout(commandGroup);
    m_commandEdit = new QLineEdit(commandGroup);
    m_commandEdit->setPlaceholderText(QStringLiteral("输入命令文本，例如 ping / shutdown"));
    m_commandEdit->setText(QStringLiteral("ping"));
    m_sendButton = new QPushButton(QStringLiteral("发送"), commandGroup);
    m_clearButton = new QPushButton(QStringLiteral("清空日志"), commandGroup);
    commandLayout->addWidget(m_commandEdit, 1);
    commandLayout->addWidget(m_sendButton);
    commandLayout->addWidget(m_clearButton);

    controlLayout->addWidget(launchGroup);
    controlLayout->addWidget(buttonGroup);
    controlLayout->addWidget(statusGroup);
    controlLayout->addWidget(poseGroup);
    controlLayout->addWidget(commandGroup);
    controlLayout->addStretch(1);

    auto *logGroup = new QGroupBox(QStringLiteral("运行日志"), splitter);
    auto *logLayout = new QVBoxLayout(logGroup);
    logLayout->setContentsMargins(8, 12, 8, 8);
    m_logView = new QPlainTextEdit(logGroup);
    m_logView->setReadOnly(true);
    m_logView->setMaximumBlockCount(3000);
    m_logView->setStyleSheet(QStringLiteral("background-color:#FFFFFF; color:#202124;"));
    logLayout->addWidget(m_logView, 1);

    splitter->addWidget(controlPanel);
    splitter->addWidget(logGroup);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    statusBar()->showMessage(QStringLiteral("就绪"));
}

void MainWindow::bindSignals()
{
    connect(m_startButton, &QPushButton::clicked, this, &MainWindow::startPython);
    connect(m_stopButton, &QPushButton::clicked, this, &MainWindow::stopPython);
    connect(m_restartButton, &QPushButton::clicked, this, &MainWindow::restartPython);
    connect(m_browsePythonButton, &QPushButton::clicked, this, &MainWindow::browsePythonExecutable);
    connect(m_browseProjectButton, &QPushButton::clicked, this, &MainWindow::browseProjectRoot);
    connect(m_sendButton, &QPushButton::clicked, this, &MainWindow::sendCommand);
    connect(m_clearButton, &QPushButton::clicked, this, &MainWindow::clearLog);

    connect(m_bridge, &LocalCommBridge::clientConnected, this, &MainWindow::onClientConnected);
    connect(m_bridge, &LocalCommBridge::clientDisconnected, this, &MainWindow::onClientDisconnected);
    connect(m_bridge, &LocalCommBridge::messageReceived, this, &MainWindow::onMessageReceived);
    connect(m_bridge, &LocalCommBridge::errorOccurred, this, &MainWindow::onBridgeError);
    connect(m_bridge, &LocalCommBridge::statusChanged, this, &MainWindow::onBridgeStatus);

    connect(m_python, &PythonProcessController::started, this, &MainWindow::onPythonStarted);
    connect(m_python, &PythonProcessController::finished, this, &MainWindow::onPythonFinished);
    connect(m_python, &PythonProcessController::crashed, this, &MainWindow::onPythonCrashed);
    connect(m_python, &PythonProcessController::stdoutLine, this, &MainWindow::onPythonStdout);
    connect(m_python, &PythonProcessController::stderrLine, this, &MainWindow::onPythonStderr);
    connect(m_python, &PythonProcessController::errorOccurred, this, &MainWindow::onPythonError);
}

void MainWindow::startPython()
{
    saveLaunchSettings();

    if (!ensureBridgeListening()) {
        appendLog(QStringLiteral("ERR"), QStringLiteral("Qt 监听未就绪，取消启动 Python"));
        return;
    }

    const PythonLaunchConfig config = currentLaunchConfig();
    if (m_python->isRunning()) {
        appendLog(QStringLiteral("SYS"), QStringLiteral("Python 已在运行"));
        return;
    }

    if (m_python->start(config)) {
        appendLog(QStringLiteral("SYS"), QStringLiteral("已发起 Python 启动请求"));
    } else {
        appendLog(QStringLiteral("ERR"), QStringLiteral("Python 启动失败"));
    }
}

void MainWindow::stopPython()
{
    if (!m_python->isRunning()) {
        appendLog(QStringLiteral("SYS"), QStringLiteral("Python 当前未运行"));
        return;
    }

    if (m_bridge->isConnected()) {
        m_bridge->sendMessage(QStringLiteral("shutdown"),
                              QJsonObject{{QStringLiteral("reason"), QStringLiteral("ui-stop")}},
                              QStringLiteral("shutdown"));
    }

    m_python->stop();
    appendLog(QStringLiteral("SYS"), QStringLiteral("已请求停止 Python"));
}

void MainWindow::restartPython()
{
    saveLaunchSettings();
    stopPython();
    startPython();
}

void MainWindow::loadLaunchSettings()
{
    const QSettings settings;

    const QString pythonExecutable = settings.value(QStringLiteral("launch/pythonExecutable")).toString();
    if (!pythonExecutable.isEmpty()) {
        m_pythonExeEdit->setText(pythonExecutable);
    }

    const QString projectRoot = settings.value(QStringLiteral("launch/projectRoot")).toString();
    if (!projectRoot.isEmpty()) {
        m_projectRootEdit->setText(projectRoot);
    }

    const QString module = settings.value(QStringLiteral("launch/module")).toString();
    if (!module.isEmpty()) {
        m_moduleEdit->setText(module);
    }

    const QString host = settings.value(QStringLiteral("launch/host")).toString();
    if (!host.isEmpty()) {
        m_hostEdit->setText(host);
    }

    const int port = settings.value(QStringLiteral("launch/port"), m_portSpin->value()).toInt();
    if (port >= m_portSpin->minimum() && port <= m_portSpin->maximum()) {
        m_portSpin->setValue(port);
    }

    const QString clientName = settings.value(QStringLiteral("launch/clientName")).toString();
    if (!clientName.isEmpty()) {
        m_clientNameEdit->setText(clientName);
    }

    const QVariant autoStart = settings.value(QStringLiteral("launch/autoStart"));
    if (autoStart.isValid()) {
        m_autoStartCheck->setChecked(autoStart.toBool());
    }
}

void MainWindow::saveLaunchSettings() const
{
    QSettings settings;
    settings.setValue(QStringLiteral("launch/pythonExecutable"), m_pythonExeEdit->text().trimmed());
    settings.setValue(QStringLiteral("launch/projectRoot"), m_projectRootEdit->text().trimmed());
    settings.setValue(QStringLiteral("launch/module"), m_moduleEdit->text().trimmed());
    settings.setValue(QStringLiteral("launch/host"), m_hostEdit->text().trimmed());
    settings.setValue(QStringLiteral("launch/port"), m_portSpin->value());
    settings.setValue(QStringLiteral("launch/clientName"), m_clientNameEdit->text().trimmed());
    settings.setValue(QStringLiteral("launch/autoStart"), m_autoStartCheck->isChecked());
    settings.sync();
}

void MainWindow::browsePythonExecutable()
{
    const QString current = m_pythonExeEdit->text().trimmed();
    const QString startDir = QFileInfo(current).exists()
                                 ? QFileInfo(current).absolutePath()
                                 : QDir::homePath();
    const QString file = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("选择 Python 可执行文件"),
        startDir,
        QStringLiteral("Python 可执行文件 (python.exe python);;可执行文件 (*.exe);;所有文件 (*.*)"));
    if (file.isEmpty()) {
        return;
    }

    m_pythonExeEdit->setText(QDir::toNativeSeparators(QDir::cleanPath(file)));
    saveLaunchSettings();
}

void MainWindow::browseProjectRoot()
{
    const QString current = m_projectRootEdit->text().trimmed();
    const QString startDir = QFileInfo(current).isDir()
                                 ? current
                                 : QDir::currentPath();
    const QString dir = QFileDialog::getExistingDirectory(
        this,
        QStringLiteral("选择 Python 工作目录"),
        startDir);
    if (dir.isEmpty()) {
        return;
    }

    m_projectRootEdit->setText(QDir::toNativeSeparators(QDir::cleanPath(dir)));
    saveLaunchSettings();
}

void MainWindow::sendCommand()
{
    const QString text = m_commandEdit->text().trimmed().isEmpty()
                             ? QStringLiteral("ping")
                             : m_commandEdit->text().trimmed();

    if (!m_bridge->isConnected()) {
        appendLog(QStringLiteral("ERR"), QStringLiteral("没有已连接的 Python 客户端"));
        return;
    }

    if (m_bridge->sendCommand(text)) {
        appendLog(QStringLiteral("TX"), text);
    }
}

void MainWindow::clearLog()
{
    m_logView->clear();
}

void MainWindow::onClientConnected(const QString &peer)
{
    m_lastMessageTimer.restart();
    setLinkStatus(QStringLiteral("已连接 %1").arg(peer));
    setLastMessage(QStringLiteral("等待首条消息"));
    m_connectionLabel->setText(peer);
    appendLog(QStringLiteral("QT"), QStringLiteral("Python 已连接: %1").arg(peer));
}

void MainWindow::onClientDisconnected()
{
    setLinkStatus(QStringLiteral("未连接"));
    m_connectionLabel->setText(QStringLiteral("-"));
    appendLog(QStringLiteral("QT"), QStringLiteral("Python 客户端已断开"));
}

void MainWindow::onMessageReceived(const QJsonObject &message)
{
    // LocalCommBridge 完成 TCP 分帧和 JSON 解析后，所有 Python 消息都会进入这里。
    // 当前测试程序只更新 UI 和日志；移植到正式主控后，可保留此处的消息解析，
    // 但应只读取 payload["motor_6"] / payload["motor_commands"]，
    // 不再依赖旧版完整 angles 文本去推导电机动作。
    const QString type = armcomm::messageType(message);
    const qint64 seq = armcomm::messageSeq(message);
    const QJsonObject payload = message.value(QStringLiteral("payload")).toObject();

    m_lastMessageTimer.restart();
    setLastMessage(QStringLiteral("%1 / seq %2").arg(type).arg(seq >= 0 ? QString::number(seq) : QStringLiteral("-")));

    if (type != QStringLiteral("heartbeat") && type != QStringLiteral("pose")) {
        appendLog(QStringLiteral("RX"), QString::fromUtf8(QJsonDocument(message).toJson(QJsonDocument::Compact)));
    }

    if (type == QStringLiteral("hello")) {
        m_bridge->sendMessage(QStringLiteral("ack"),
                              QJsonObject{{QStringLiteral("server"), QStringLiteral("qt-main")},
                                          {QStringLiteral("ok"), true}},
                              QStringLiteral("hello ok"));
        return;
    }

    if (type == QStringLiteral("heartbeat")) {
        return;
    }

    if (type == QStringLiteral("data")) {
        const QString summary = QStringLiteral("frame=%1 %2")
                                    .arg(payload.value(QStringLiteral("frame_id")).toInt(-1))
                                    .arg(summarizePayload(payload));
        appendLog(QStringLiteral("DATA"), summary);
        return;
    }

    if (type == QStringLiteral("pose")) {
        const QJsonObject motor6 = payload.value(QStringLiteral("motor_6")).toObject();
        if (!motor6.isEmpty()) {
            const QString payloadSummary = payload.value(QStringLiteral("summary")).toString();
            const QString displaySummary = payloadSummary.isEmpty() ? formatMotor6Summary(motor6) : payloadSummary;
            m_poseLabel->setText(displaySummary);
            appendLog(QStringLiteral("MOTOR6"), displaySummary);
            m_poseView->setHtml(formatMotor6Html(motor6, QJsonArray()));
            return;
        }

        const QString payloadSummary = payload.value(QStringLiteral("summary")).toString();
        if (!payloadSummary.isEmpty()) {
            m_poseLabel->setText(payloadSummary);
            appendLog(QStringLiteral("POSE"), payloadSummary);
        } else {
            m_poseLabel->setText(QStringLiteral("等待 6 号电机数据"));
            appendLog(QStringLiteral("POSE"), QStringLiteral("payload 缺少 motor_6"));
        }
        m_poseView->setHtml(QStringLiteral("<div style='padding:8px 10px; color:#666666; background:#FFFFFF;'>等待 motor_6 字段</div>"));
        return;
    }

    if (type == QStringLiteral("cmd")) {
        const QString cmd = payload.value(QStringLiteral("text")).toString(QStringLiteral("unknown"));
        appendLog(QStringLiteral("QT"), QStringLiteral("收到命令: %1").arg(cmd));
        m_bridge->sendMessage(QStringLiteral("ack"),
                              QJsonObject{{QStringLiteral("cmd"), cmd},
                                          {QStringLiteral("ok"), true}},
                              QStringLiteral("command ok"));
        return;
    }

    if (type == QStringLiteral("ack")) {
        appendLog(QStringLiteral("ACK"), summarizePayload(payload));
    }
}

void MainWindow::onBridgeError(const QString &error)
{
    appendLog(QStringLiteral("BRIDGE"), error);
    setServerStatus(error);
}

void MainWindow::onBridgeStatus(const QString &status)
{
    setServerStatus(status);
}

void MainWindow::onPythonStarted(const QString &status)
{
    setPythonStatus(status);
    appendLog(QStringLiteral("PY"), status);
}

void MainWindow::onPythonFinished(int exitCode)
{
    if (exitCode == 0) {
        setPythonStatus(QStringLiteral("Python 已退出 (code=0)"));
        appendLog(QStringLiteral("PY"), QStringLiteral("Python 正常退出"));
    } else {
        appendLog(QStringLiteral("PY"), QStringLiteral("Python 退出，code=%1").arg(exitCode));
    }
}

void MainWindow::onPythonCrashed(int exitCode)
{
    setPythonStatus(QStringLiteral("Python 崩溃 (code=%1)").arg(exitCode));
    appendLog(QStringLiteral("PY-ERR"), QStringLiteral("Python 崩溃，exitCode=%1").arg(exitCode));
}

void MainWindow::onPythonStdout(const QString &line)
{
    appendLog(QStringLiteral("PY"), line);
}

void MainWindow::onPythonStderr(const QString &line)
{
    appendLog(QStringLiteral("PY-ERR"), line);
}

void MainWindow::onPythonError(const QString &error)
{
    setPythonStatus(error);
    appendLog(QStringLiteral("PY-ERR"), error);
}

void MainWindow::updateWatchdog()
{
    if (!m_bridge->isListening()) {
        setServerStatus(QStringLiteral("未监听"));
        return;
    }

    if (!m_bridge->isConnected()) {
        setLinkStatus(QStringLiteral("等待 Python 连接"));
        return;
    }

    if (!m_lastMessageTimer.isValid()) {
        return;
    }

    const qint64 elapsed = m_lastMessageTimer.elapsed();
    if (elapsed > 3000) {
        setLinkStatus(QStringLiteral("已连接，消息超时 %1 ms").arg(elapsed));
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_watchdog.stop();

    if (m_bridge->isConnected()) {
        m_bridge->sendMessage(QStringLiteral("shutdown"),
                              QJsonObject{{QStringLiteral("reason"), QStringLiteral("ui-close")}},
                              QStringLiteral("shutdown"));
    }

    m_python->stop();
    m_bridge->stopServer();

    QMainWindow::closeEvent(event);
}

void MainWindow::appendLog(const QString &tag, const QString &text)
{
    const QString stamp = QTime::currentTime().toString(QStringLiteral("HH:mm:ss"));
    m_logView->appendPlainText(QStringLiteral("[%1] [%2] %3").arg(stamp, tag, text));
}

void MainWindow::setServerStatus(const QString &text)
{
    m_serverStatusLabel->setText(text);
    statusBar()->showMessage(text, 3000);
}

void MainWindow::setPythonStatus(const QString &text)
{
    m_pythonStatusLabel->setText(text);
    statusBar()->showMessage(text, 3000);
}

void MainWindow::setLinkStatus(const QString &text)
{
    m_linkStatusLabel->setText(text);
}

void MainWindow::setLastMessage(const QString &text)
{
    m_lastMessageLabel->setText(text);
}

bool MainWindow::ensureBridgeListening()
{
    const QString host = m_hostEdit->text().trimmed().isEmpty()
                             ? QStringLiteral("127.0.0.1")
                             : m_hostEdit->text().trimmed();
    const quint16 port = static_cast<quint16>(m_portSpin->value());
    return m_bridge->startServer(host, port);
}

PythonLaunchConfig MainWindow::currentLaunchConfig() const
{
    PythonLaunchConfig config;
    config.pythonExecutable = m_pythonExeEdit->text().trimmed();
    config.projectRoot = m_projectRootEdit->text().trimmed();
    config.module = m_moduleEdit->text().trimmed();
    config.host = m_hostEdit->text().trimmed();
    config.port = static_cast<quint16>(m_portSpin->value());
    config.clientName = m_clientNameEdit->text().trimmed();
    return config;
}

QString MainWindow::defaultProjectRoot() const
{
    QDir dir(QApplication::applicationDirPath());
    for (int i = 0; i < 3; ++i) {
        if (!dir.cdUp()) {
            break;
        }
    }
    const QString candidate = QDir::cleanPath(dir.absolutePath());
    if (QFileInfo::exists(QDir(candidate).filePath(QStringLiteral("stage1_comm"))) ||
        QFileInfo::exists(QDir(candidate).filePath(QStringLiteral("app")))) {
        return candidate;
    }
    return QDir::currentPath();
}

QString MainWindow::defaultPythonExecutable() const
{
    const QString envPython = qEnvironmentVariable("VISUAL_ARM_PYTHON_EXE");
    if (!envPython.isEmpty()) {
        return envPython;
    }

    const QString found = QStandardPaths::findExecutable(QStringLiteral("python.exe"));
    if (!found.isEmpty()) {
        return found;
    }

    const QString foundNoExt = QStandardPaths::findExecutable(QStringLiteral("python"));
    if (!foundNoExt.isEmpty()) {
        return foundNoExt;
    }

    return QStringLiteral("python");
}

QString MainWindow::summarizePayload(const QJsonObject &payload)
{
    if (payload.isEmpty()) {
        return QStringLiteral("{}");
    }

    const QString text = payload.value(QStringLiteral("text")).toString();
    const QString name = payload.value(QStringLiteral("name")).toString();
    const QString status = payload.value(QStringLiteral("status")).toString();
    const QString frameId = payload.contains(QStringLiteral("frame_id"))
                                ? QString::number(payload.value(QStringLiteral("frame_id")).toInt())
                                : QString();

    QStringList parts;
    if (!frameId.isEmpty()) {
        parts << QStringLiteral("frame_id=%1").arg(frameId);
    }
    if (!text.isEmpty()) {
        parts << QStringLiteral("text=%1").arg(text);
    }
    if (!name.isEmpty()) {
        parts << QStringLiteral("name=%1").arg(name);
    }
    if (!status.isEmpty()) {
        parts << QStringLiteral("status=%1").arg(status);
    }
    if (parts.isEmpty()) {
        return QString::fromUtf8(QJsonDocument(payload).toJson(QJsonDocument::Compact));
    }
    return parts.join(QStringLiteral(" | "));
}

QString MainWindow::summarizeRotation(const QJsonObject &rotation)
{
    if (rotation.isEmpty()) {
        return QStringLiteral("-");
    }
    return QStringLiteral("%1 %2 %3 %4")
        .arg(formatDirection(rotation, QStringLiteral("vertical")),
             formatDeg(rotation, QStringLiteral("vertical_deg")),
             formatDirection(rotation, QStringLiteral("horizontal")),
             formatDeg(rotation, QStringLiteral("horizontal_deg")));
}

QString MainWindow::formatPoseSummary(const QJsonArray &angles)
{
    const QJsonObject primaryArm = selectPrimaryArm(angles);
    if (primaryArm.isEmpty()) {
        return QStringLiteral("未找到手腕实验数据");
    }
    return formatWristExperimentSummary(primaryArm);
}

QString MainWindow::formatMotor6Summary(const QJsonObject &command)
{
    const bool valid = command.value(QStringLiteral("valid")).toBool(false);
    const bool motionAllowed = command.value(QStringLiteral("motion_allowed")).toBool(false);
    const QString reason = command.value(QStringLiteral("reason")).toString(QStringLiteral("-"));
    const QString directionText = command.value(QStringLiteral("direction_text")).toString();
    const QString angleDeg = command.contains(QStringLiteral("angle_deg")) && !command.value(QStringLiteral("angle_deg")).isNull()
                                 ? QString::number(qRound(command.value(QStringLiteral("angle_deg")).toDouble()))
                                 : formatDeg(command, QStringLiteral("roll_deg"));

    if (!valid) {
        return QStringLiteral("6号电机：保持 | 右手未有效识别 | reason=%1").arg(reason);
    }
    if (!motionAllowed) {
        return QStringLiteral("6号电机：保持 | %1 %2° | reason=%3")
            .arg(directionText.isEmpty() ? QStringLiteral("保持") : directionText,
                 angleDeg,
                 reason);
    }
    return QStringLiteral("6号电机：发送 | %1 %2°")
        .arg(directionText.isEmpty() ? QStringLiteral("保持") : directionText,
             angleDeg);
}

QString MainWindow::formatMotor6Html(const QJsonObject &command, const QJsonArray &angles)
{
    Q_UNUSED(angles);

    const bool valid = command.value(QStringLiteral("valid")).toBool(false);
    const bool motionAllowed = command.value(QStringLiteral("motion_allowed")).toBool(false);
    const QString state = valid && motionAllowed ? QStringLiteral("发送") : QStringLiteral("保持");
    const QString reason = command.value(QStringLiteral("reason")).toString(QStringLiteral("-"));
    const QString directionText = command.value(QStringLiteral("direction_text")).toString(QStringLiteral("保持"));
    const QString angleDeg = command.contains(QStringLiteral("angle_deg")) && !command.value(QStringLiteral("angle_deg")).isNull()
                                 ? QString::number(qRound(command.value(QStringLiteral("angle_deg")).toDouble()))
                                 : formatDeg(command, QStringLiteral("roll_deg"));
    const QString allowText = motionAllowed ? QStringLiteral("是") : QStringLiteral("否");
    const QString protocolVersion = QStringLiteral("vision_motor_v1");
    const QString controlStage = QStringLiteral("right_wrist_motor_6_only");

    return QStringLiteral(R"(
        <div style="padding:8px 10px; border:1px solid #D6D6D6; background:#FFFFFF; margin-bottom:10px;">
            <div style="font-weight:700; color:#202124; margin-bottom:6px;">6号电机验证</div>
            <div style="line-height:1.7;">协议：<span style="color:#3F6FB8;">%1</span></div>
            <div style="line-height:1.7;">控制阶段：<span style="color:#3F6FB8;">%2</span></div>
            <div style="line-height:1.7;">状态：<span style="color:#3F6FB8;">%3</span>，允许运动：<span style="color:#3F6FB8;">%4</span></div>
            <div style="line-height:1.7;">目标：<span style="color:#3F6FB8;">6号电机 / 右手腕左右旋转</span></div>
            <div style="line-height:1.7;">指令：<span style="color:#3F6FB8;">%5 %6°</span></div>
            <div style="line-height:1.7;">reason：<span style="color:#666666;">%7</span></div>
            <div style="line-height:1.7; color:#666666;">Qt 正式项目接收后，只把 pwm_index=6 与 angle_deg 交给 Rs485Controller::setServoAngle(...)。</div>
        </div>
    )")
        .arg(protocolVersion,
             controlStage,
             state,
             allowText,
             directionText.isEmpty() ? QStringLiteral("保持") : directionText,
             angleDeg,
             reason);
}

QString MainWindow::formatPoseHtml(const QJsonArray &angles)
{
    const QJsonObject primaryArm = selectPrimaryArm(angles);
    QJsonObject left;
    QJsonObject right;
    for (const QJsonValue &value : angles) {
        const QJsonObject arm = value.toObject();
        const QString side = arm.value(QStringLiteral("side")).toString();
        if (side == QStringLiteral("left")) {
            left = arm;
        } else if (side == QStringLiteral("right")) {
            right = arm;
        }
    }

    return QStringLiteral(R"(
        %1
        <table width="100%" cellspacing="0" cellpadding="0" style="color:#202124; font-size:14px;">
            <tr>
                %2
                <td style="width:12px;"></td>
                %3
            </tr>
        </table>
    )")
        .arg(formatWristExperimentHtml(primaryArm),
             formatArmColumn(QStringLiteral("左臂关节旋转角度"), left),
             formatArmColumn(QStringLiteral("右臂关节旋转角度"), right));
}

QString MainWindow::formatArmColumn(const QString &title, const QJsonObject &arm)
{
    return QStringLiteral(R"(
        <td width="50%" valign="top" style="padding:8px 10px; border:1px solid #D6D6D6; background:#FFFFFF;">
            <div style="font-weight:700; color:#202124; margin-bottom:8px;">%1</div>
            %2
            %3
            %4
        </td>
    )")
        .arg(title,
             formatJointRow(QStringLiteral("腕"), arm.value(QStringLiteral("wrist_rotation")).toObject()),
             formatJointRow(QStringLiteral("臂"), arm.value(QStringLiteral("arm_rotation")).toObject()),
             formatJointRow(QStringLiteral("肩"), arm.value(QStringLiteral("shoulder_rotation")).toObject()));
}

QString MainWindow::formatJointRow(const QString &label, const QJsonObject &rotation)
{
    return QStringLiteral(R"(
        <div style="white-space:nowrap; line-height:1.6;">
            <span style="display:inline-block; width:26px; color:#3F6FB8;">%1:</span>
            <span>%2</span>
        </div>
    )")
        .arg(label, summarizeRotation(rotation));
}

QJsonObject MainWindow::selectPrimaryArm(const QJsonArray &angles)
{
    QJsonObject leftArm;
    QJsonObject rightArm;

    for (const QJsonValue &value : angles) {
        const QJsonObject arm = value.toObject();
        const QString side = arm.value(QStringLiteral("side")).toString();
        if (side == QStringLiteral("left")) {
            leftArm = arm;
        } else if (side == QStringLiteral("right")) {
            rightArm = arm;
        }
    }

    if (!rightArm.isEmpty()) {
        return rightArm;
    }
    return leftArm;
}

QString MainWindow::formatWristExperimentSummary(const QJsonObject &arm)
{
    if (arm.isEmpty()) {
        return QStringLiteral("未找到手腕实验数据");
    }

    const QString side = arm.value(QStringLiteral("side")).toString();
    const QString sideLabel = side == QStringLiteral("left")
                                  ? QStringLiteral("左腕")
                                  : side == QStringLiteral("right")
                                        ? QStringLiteral("右腕")
                                        : QStringLiteral("手腕");
    const QJsonObject wrist = arm.value(QStringLiteral("wrist_rotation")).toObject();

    return QStringLiteral("%1实验 | 3号电机(上下): 已停用 | 6号电机(左右): %2 | WristFlex: %3°")
        .arg(sideLabel,
             formatAxisMotion(wrist, QStringLiteral("horizontal"), QStringLiteral("horizontal_deg")),
             formatDeg(arm, QStringLiteral("wrist_flex_deg")));
}

QString MainWindow::formatWristExperimentHtml(const QJsonObject &arm)
{
    if (arm.isEmpty()) {
        return QStringLiteral(R"(
            <div style="padding:8px 10px; border:1px solid #D6D6D6; background:#FFFFFF;">
                <div style="font-weight:700; color:#202124; margin-bottom:6px;">手腕电机映射（初始实验）</div>
                <div style="color:#666666;">等待手腕姿态</div>
            </div>
        )");
    }

    const QString side = arm.value(QStringLiteral("side")).toString();
    const QString sideLabel = side == QStringLiteral("left")
                                  ? QStringLiteral("左腕")
                                  : side == QStringLiteral("right")
                                        ? QStringLiteral("右腕")
                                        : QStringLiteral("手腕");
    const QJsonObject wrist = arm.value(QStringLiteral("wrist_rotation")).toObject();

    return QStringLiteral(R"(
        <div style="padding:8px 10px; border:1px solid #D6D6D6; background:#FFFFFF; margin-bottom:10px;">
            <div style="font-weight:700; color:#202124; margin-bottom:6px;">手腕电机映射（初始实验）</div>
            <div style="line-height:1.6;">目标手腕：<span style="color:#AFCBFF;">%1</span></div>
            <div style="line-height:1.6;">3号电机（上下）：<span style="color:#666666;">已停用</span></div>
            <div style="line-height:1.6;">6号电机（左右）：<span style="color:#AFCBFF;">%2</span></div>
            <div style="line-height:1.6;">Wrist Flex 3D：<span style="color:#AFCBFF;">%3°</span></div>
        </div>
    )")
        .arg(sideLabel,
             formatAxisMotion(wrist, QStringLiteral("horizontal"), QStringLiteral("horizontal_deg")),
             formatDeg(arm, QStringLiteral("wrist_flex_deg")));
}

QString MainWindow::formatAxisMotion(const QJsonObject &rotation, const QString &directionKey, const QString &degKey)
{
    if (rotation.isEmpty()) {
        return QStringLiteral("-");
    }

    const QString direction = formatDirection(rotation, directionKey);
    const QString deg = formatDeg(rotation, degKey);
    if (direction == QStringLiteral("-") && deg == QStringLiteral("-")) {
        return QStringLiteral("-");
    }
    if (deg == QStringLiteral("-")) {
        return direction;
    }
    return QStringLiteral("%1 %2°").arg(direction, deg);
}

QString MainWindow::formatDirection(const QJsonObject &rotation, const QString &key)
{
    const QString value = rotation.value(key).toString();
    if (value.isEmpty()) {
        return QStringLiteral("-");
    }
    if (value == QStringLiteral("up")) {
        return QStringLiteral("上");
    }
    if (value == QStringLiteral("down")) {
        return QStringLiteral("下");
    }
    if (value == QStringLiteral("left")) {
        return QStringLiteral("左");
    }
    if (value == QStringLiteral("right")) {
        return QStringLiteral("右");
    }
    return value;
}

QString MainWindow::formatDeg(const QJsonObject &rotation, const QString &key)
{
    if (!rotation.contains(key) || rotation.value(key).isNull()) {
        return QStringLiteral("-");
    }
    const double deg = rotation.value(key).toDouble();
    return QString::number(qRound(deg));
}
