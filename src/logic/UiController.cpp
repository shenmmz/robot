/*
 * @file UiController.cpp
 * @brief 实现 UiController 类，负责 IO 监控、报警日志、编程运行和设置页面逻辑。
 *
 * 所有子控件通过 mainWindow->findChild 按名称查找，不依赖 ui_mainwindow.h 生成头文件。
 */
#include "UiController.h"
#include "RobotController.h"
#include "TaskExecutor.h"
#include "../hardware/MotionController.h"
#include "../models/RobotConfig.h"
#include "../models/RobotState.h"
#include "../../ZmotionAPI/ZmotionAPI.h"
#include "../../library/zauxdll2.h"
#include "../utils/MathUtils.h"

#include <QWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleValidator>
#include <QCheckBox>
#include <QRegularExpression>
#include <QFrame>
#include <QLabel>
#include <QShortcut>
#include <QKeySequence>
#include <QTableWidget>
#include <QPlainTextEdit>
#include <QTextEdit>           // QTextEdit::ExtraSelection
#include <QTextCharFormat>
#include <QTextFormat>
#include <QTextCursor>
#include <QDoubleSpinBox>
#include <QStatusBar>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QScrollArea>
#include <QGroupBox>
#include <QSplitter>
#include <QScrollBar>
#include <QWheelEvent>
#include <QGridLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QRegularExpression>
#include <QSet>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <utility>
#include <functional>

// ==================== 构造 / 析构 / 初始化 ====================

/**
 * @brief UiController 构造函数
 *
 * 保存主窗口指针和机械臂控制器指针，其他成员在 init() 中初始化。
 *
 * @param mainWindow  Qt 主窗口指针，用于查找 UI 控件
 * @param robotCtrl   机械臂核心业务控制器，用于下发运动指令和读取状态
 * @param parent      Qt 父对象
 */
UiController::UiController(QWidget* mainWindow, RobotController* robotCtrl, QObject* parent)
    : QObject(parent)
    , m_mainWindow(mainWindow)
    , m_robotController(robotCtrl)
{
}

/**
 * @brief UiController 析构函数
 *
 * 若 IO 定时刷新器仍在运行，则停止它，避免无效回调。
 */
UiController::~UiController()
{
    if (m_ioTimer) m_ioTimer->stop();
}

/**
 * @brief 初始化 UI 控制器
 *
 * 完成以下工作：
 *   1. 查找 Qt 主窗口的 centralwidget（用于查找子控件）
 *   2. 启动 200ms 的 IO 刷新定时器，实时显示 DI/DO 状态
 *   3. 创建 TaskExecutor（任务解释执行器）并连接其所有信号
 *   4. 连接报警相关信号与按钮（机械臂错误、硬件错误、清除报警）
 *   5. 连接编程页面的"运行/停止/指令插入/文件操作"按钮
 *   6. 连接关节和笛卡尔点动按钮
 *   7. 调试模式开关与加载默认设置
 *   8. 程序区文字变化 → 500ms 去抖动 → 同步点位（TARGET）顺序
 */
void UiController::init()
{
    // 保存 centralwidget，后续通过它以 objectName 查找所有子控件
    m_centralWidget = m_mainWindow->findChild<QWidget*>(QStringLiteral("centralwidget"));

    // === IO 监控定时器：每 50ms 刷新一次 DI/DO 指示灯 ===
    // 注：原 200ms 间隔在"点动"等短脉冲场景下容易错过电平变化，
    //     50ms 既能稳定捕获点动，也对控制器负载影响很小。
    m_ioTimer = new QTimer(this);
    connect(m_ioTimer, &QTimer::timeout, this, &UiController::refreshIO);
    m_ioTimer->start(50);

    // === 任务执行器：负责逐条解析并执行脚本指令 ===
    m_taskExecutor = new TaskExecutor(m_robotController, this);
    connect(m_taskExecutor, &TaskExecutor::taskProgress, this, &UiController::onTaskProgress);
    connect(m_taskExecutor, &TaskExecutor::taskPaused,  this, &UiController::onTaskPaused);
    connect(m_taskExecutor, &TaskExecutor::taskResumed, this, &UiController::onTaskResumed);
    connect(m_taskExecutor, &TaskExecutor::taskFinished,this, &UiController::onTaskFinished);
    connect(m_taskExecutor, &TaskExecutor::taskError,   this, &UiController::onTaskError);
    connect(m_taskExecutor, &TaskExecutor::consoleOutput, this, &UiController::onConsoleOutput);
    connect(m_taskExecutor, &TaskExecutor::currentLineChanged, this, &UiController::onCurrentLineChanged);

    // === 报警日志：将 RobotController 与 MotionController 的错误信号接入报警表 ===
    connect(m_robotController, &RobotController::errorOccurred,
            this, &UiController::onRobotError);
    if (m_robotController->getMotionController()) {
        connect(m_robotController->getMotionController(), &MotionController::hardwareError,
                this, &UiController::onHardwareError);
    }
    if (auto* btn = m_mainWindow->findChild<QPushButton*>(QStringLiteral("btnClearAlarms")))
        connect(btn, &QPushButton::clicked, this, &UiController::onClearAlarms);

    // === 编程页面：运行 / 停止 ===
    if (auto* btn = m_mainWindow->findChild<QPushButton*>(QStringLiteral("btnProgRun")))
        connect(btn, &QPushButton::clicked, this, &UiController::onProgRun);
    if (auto* btn = m_mainWindow->findChild<QPushButton*>(QStringLiteral("btnProgStop")))
        connect(btn, &QPushButton::clicked, this, &UiController::onProgStop);

    // === 编程页面：指令插入按钮（通过 lambda 封装同名的 btnProgXxx 到对应的槽） ===
    auto connProgBtn = [this](const QString& name, void (UiController::*slot)()) {
        if (auto* btn = m_mainWindow->findChild<QPushButton*>(name))
            connect(btn, &QPushButton::clicked, this, slot);
    };
    // 运动类
    connProgBtn(QStringLiteral("btnProgMoveJ"),    &UiController::onProgInsertMoveJ);
    connProgBtn(QStringLiteral("btnProgMoveL"),    &UiController::onProgInsertMoveL);
    // 流程/延时类
    connProgBtn(QStringLiteral("btnProgWaitIdle"), &UiController::onProgInsertWaitIdle);
    connProgBtn(QStringLiteral("btnProgDelay"),    &UiController::onProgInsertDelay);
    // IO 类
    connProgBtn(QStringLiteral("btnProgSetDO"),    &UiController::onProgInsertSetDO);
    connProgBtn(QStringLiteral("btnProgWaitDI"),   &UiController::onProgInsertWaitDI);
    // 输出类
    connProgBtn(QStringLiteral("btnProgPrint"),    &UiController::onProgInsertPrint);
    connProgBtn(QStringLiteral("btnProgComment"),  &UiController::onProgInsertComment);
    connProgBtn(QStringLiteral("btnProgCall"),     &UiController::onProgInsertCall);
    // 变量/读数
    connProgBtn(QStringLiteral("btnProgSet"),      &UiController::onProgInsertSet);
    connProgBtn(QStringLiteral("btnProgReadPos"),  &UiController::onProgReadPosition);
    // 循环
    connProgBtn(QStringLiteral("btnProgFor"),      &UiController::onProgInsertFor);
    // 偏移运动
    connProgBtn(QStringLiteral("btnProgMoveOff"),  &UiController::onProgInsertMoveOff);
    connProgBtn(QStringLiteral("btnProgMoveLOff"), &UiController::onProgInsertMoveLOff);
    // 参数设定
    connProgBtn(QStringLiteral("btnProgSpeed"),    &UiController::onProgInsertSpeed);
    connProgBtn(QStringLiteral("btnProgAccel"),    &UiController::onProgInsertAccel);
    connProgBtn(QStringLiteral("btnProgDecel"),    &UiController::onProgInsertDecel);
    connProgBtn(QStringLiteral("btnProgTimeout"),  &UiController::onProgInsertTimeout);
    // 条件
    connProgBtn(QStringLiteral("btnProgIfDI"),     &UiController::onProgInsertIfDI);
    connProgBtn(QStringLiteral("btnProgElse"),     &UiController::onProgInsertElse);
    connProgBtn(QStringLiteral("btnProgEndIf"),    &UiController::onProgInsertEndIf);
    // SERVO 按钮在 mainwindow.cpp rebuildInstrTabs() 中动态创建并连接
    // （2026-06-15：原 SERVO_S 按钮已删除，多路语法并入 SERVO）

    // === 编程页面：文件操作按钮（保存/加载/新建/删行） ===
    if (auto* btn = m_mainWindow->findChild<QPushButton*>(QStringLiteral("btnProgSave")))
        connect(btn, &QPushButton::clicked, this, &UiController::onProgSave);
    if (auto* btn = m_mainWindow->findChild<QPushButton*>(QStringLiteral("btnProgLoad")))
        connect(btn, &QPushButton::clicked, this, &UiController::onProgLoad);
    if (auto* btn = m_mainWindow->findChild<QPushButton*>(QStringLiteral("btnProgNew")))
        connect(btn, &QPushButton::clicked, this, &UiController::onProgNew);
    if (auto* btn = m_mainWindow->findChild<QPushButton*>(QStringLiteral("btnProgDeleteLine")))
        connect(btn, &QPushButton::clicked, this, &UiController::onProgDeleteLine);

    // === 设置页面：控制器 IP 编辑完成 → 保存到配置 ===
    if (auto* edit = m_mainWindow->findChild<QLineEdit*>(QStringLiteral("editControllerIP")))
        connect(edit, &QLineEdit::editingFinished, this, &UiController::onSaveSettings);

    // === 设置页面：保存按钮点击 → 保存到配置 ===
    if (auto* btn = m_mainWindow->findChild<QPushButton*>(QStringLiteral("btnSaveHome")))
        connect(btn, &QPushButton::clicked, this, &UiController::onSaveSettings);

    // === 手动点动：关节 J0-J5 的 ± 按钮（点一下移一步，松开不阻止运动） ===
    for (int j = 0; j < 6; ++j) {
        if (auto* btn = m_mainWindow->findChild<QPushButton*>(QStringLiteral("btnJogJ%1M").arg(j)))
            bindHoldButton(btn, 0, [this, j]{ onJogJoint(j, false); },
                               [this]   { /* 单击模式:松开不做任何事,让机械臂自然完成移动 */ });
        if (auto* btn = m_mainWindow->findChild<QPushButton*>(QStringLiteral("btnJogJ%1P").arg(j)))
            bindHoldButton(btn, 0, [this, j]{ onJogJoint(j, true);  },
                               [this]   { /* 单击模式 */ });
    }

    // === 手动点动：笛卡尔 X/Y/Z/Rx/Ry/Rz 的 ± 按钮（同上） ===
    static const char* cartNames[] = {"X","Y","Z","Rx","Ry","Rz"};
    for (int c = 0; c < 6; ++c) {
        QString mn = QStringLiteral("btnJog%1M").arg(QLatin1String(cartNames[c]));
        QString pn = QStringLiteral("btnJog%1P").arg(QLatin1String(cartNames[c]));
        if (auto* btn = m_mainWindow->findChild<QPushButton*>(mn))
            bindHoldButton(btn, 0, [this, c]{ onJogCart(c, false); },
                               [this]   { /* 单击模式 */ });
        if (auto* btn = m_mainWindow->findChild<QPushButton*>(pn))
            bindHoldButton(btn, 0, [this, c]{ onJogCart(c, true);  },
                               [this]   { /* 单击模式 */ });
    }

    // === 调试模式开关：勾选后才允许手动点动，避免误操作 ===
    if (auto* chkDebug = m_mainWindow->findChild<QCheckBox*>(QStringLiteral("chkDebugMode"))) {
        connect(chkDebug, &QCheckBox::toggled, this, &UiController::onDebugModeToggled);
        // 初始状态：未勾选 → 禁用所有点动控件，防止误碰
        onDebugModeToggled(false);
    }

    // === 将配置文件中的默认值写入界面 ===
    onLoadSettings();

    // === 程序编辑器文字变化 → 500ms 去抖后同步点位顺序 ===
    // 作用：按程序区 MOVEJ/MOVEL 引用 TARGET 的先后顺序重排点位编辑器
    auto* syncTimer = new QTimer(this);
    syncTimer->setSingleShot(true);
    syncTimer->setInterval(500);
    connect(syncTimer, &QTimer::timeout, this, &UiController::syncTargets);
    if (auto* progEd = getEditor())
        connect(progEd, &QPlainTextEdit::textChanged, this, [syncTimer] { syncTimer->start(); });

    // === 点位编辑器文字变化 → 100ms 去抖后同步到 m_targets 缓存 ===
    // 作用：用户手动改 TARGET P1 = {J0: 90, ...} 中的数值时，让"运行/单步"能立即看到新值
    // 实现：只解析 TARGET 行，**增量覆盖**（已存在则更新值），不删除/重排
    auto* targetSyncTimer = new QTimer(this);
    targetSyncTimer->setSingleShot(true);
    targetSyncTimer->setInterval(100);
    if (auto* targetEd = getTargetEditor())
        connect(targetEd, &QPlainTextEdit::textChanged, this, [this, targetSyncTimer] {
            // 正在 syncTargets() 写回时跳过（防止与重排逻辑抢着改 m_targets）
            if (!m_syncTargetsBusy) targetSyncTimer->start();
        });
    connect(targetSyncTimer, &QTimer::timeout, this, &UiController::onTargetEditorChanged);
}

// ==================== IO 监控 ====================

/**
 * @brief 定时刷新 DI/DO 状态显示（每 50ms 调用一次）
 *
 * 若硬件已连接，则通过 ZmotionAPI 读取控制器 DI/DO 并刷新界面指示灯；
 * 若未连接，则将所有指示灯置为灰色（未触发）。
 *
 * 注意：DI 采用低电平触发（val==0 时视为 active），与常见的 NPN 传感器接法一致。
 *      DI 总数 24 个（0~23），DO 保持 20 个（0~19）。
 */
void UiController::refreshIO()
{
    // 尝试获取已连接的 ZmotionAPI 实例
    ZmotionAPI* api = nullptr;
    if (m_robotController->getMotionController() &&
        m_robotController->getMotionController()->isConnected()) {
        api = m_robotController->getMotionController()->getZmotionAPI();
    }

    // 未连接 → 清掉所有指示灯
    if (!api) {
        for (int i = 0; i < 24; ++i) {
            setIOIndicator(QStringLiteral("di%1").arg(i), false);
        }
        for (int i = 0; i < 20; ++i) {
            setIOIndicator(QStringLiteral("do%1").arg(i), false);
        }
        return;
    }

    // 读取 DI 状态（低电平=触发=active），共 24 个（DI0~DI23）
    for (int i = 0; i < 24; ++i) {
        int val = api->getDI(i);
        setIOIndicator(QStringLiteral("di%1").arg(i), (val == 0));
    }

    // 读取 DO 状态（输出=1 视为 active），共 20 个（DO0~DO19）
    for (int i = 0; i < 20; ++i) {
        uint32 outVal = 0;
        ZAux_Direct_GetOp(api->getHandle(), i, &outVal);
        setIOIndicator(QStringLiteral("do%1").arg(i), (outVal != 0));
    }
}

/**
 * @brief 设置 IO 指示灯的视觉状态
 *
 * 在 UI 上每个 IO 用一个 QFrame（objectName = diN/doN）承载：
 *   - labels[0] 为 IO 编号标签
 *   - labels[1] 为状态指示灯（显示 ● 绿色 / ○ 灰色）
 *
 * @param frameName  对应 QFrame 的 objectName（如 "di0"、"do5"）
 * @param active     true=有信号（绿色实心圆），false=无信号（灰色空心圆）
 */
void UiController::setIOIndicator(const QString& frameName, bool active)
{
    if (!m_centralWidget) return;
    QFrame* frame = m_centralWidget->findChild<QFrame*>(frameName);
    if (!frame) return;

    // 取 frame 内所有 QLabel 子控件，第 2 个（labels[1]）作为指示灯
    QList<QLabel*> labels = frame->findChildren<QLabel*>();
    if (labels.size() < 2) return;

    QLabel* indicator = labels[1];
    if (active) {
        // 绿色实心圆 + 绿色字体
        indicator->setText(QStringLiteral("●"));
        indicator->setStyleSheet(QStringLiteral("color:#2e7d32;font-size:24px;border:none;background:transparent;"));
    } else {
        // 灰色空心圆
        indicator->setText(QStringLiteral("○"));
        indicator->setStyleSheet(QStringLiteral("color:#bdbdbd;font-size:24px;border:none;background:transparent;"));
    }
}

// ==================== 报警日志 ====================

/**
 * @brief 机械臂业务层错误回调
 *
 * RobotController 发出 errorOccurred 信号时被调用，
 * 将错误作为"操作错误"类型写入报警表格。
 *
 * @param errorCode  错误代码（由业务层定义）
 * @param errorMsg   错误描述
 */
void UiController::onRobotError(int errorCode, const QString& errorMsg)
{
    addAlarmRow(QStringLiteral("操作错误"), errorCode, errorMsg);
}

/**
 * @brief 硬件控制器错误回调
 *
 * MotionController 发出 hardwareError 信号时被调用，
 * 将错误作为"硬件错误"类型写入报警表格。
 */
void UiController::onHardwareError(int errorCode, const QString& errorMsg)
{
    addAlarmRow(QStringLiteral("硬件错误"), errorCode, errorMsg);
}

/**
 * @brief 在报警表格中新增一行报警记录
 *
 * 新行包含：[时间 HH:mm:ss][错误类型][错误码][错误描述]，
 * 背景填充浅红色以突出显示，表格自动滚到底部；
 * 同时将顶部 currentAlarmBar 置为红色警示色。
 *
 * @param type  错误类型（"操作错误"/"硬件错误" 等）
 * @param code  错误码
 * @param desc  错误描述
 */
void UiController::addAlarmRow(const QString& type, int code, const QString& desc)
{
    auto* table = m_mainWindow->findChild<QTableWidget*>(QStringLiteral("alarmTable"));
    if (!table) return;

    int row = table->rowCount();
    table->insertRow(row);

    // 组装 4 列：时间 / 类型 / 码 / 描述
    auto* timeItem = new QTableWidgetItem(
        QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss")));
    auto* typeItem = new QTableWidgetItem(type);
    auto* codeItem = new QTableWidgetItem(QString::number(code));
    auto* descItem = new QTableWidgetItem(desc);

    // 浅红背景突出显示
    QColor errBg(255, 245, 245);
    timeItem->setBackground(errBg);
    typeItem->setBackground(errBg);
    codeItem->setBackground(errBg);
    descItem->setBackground(errBg);

    table->setItem(row, 0, timeItem);
    table->setItem(row, 1, typeItem);
    table->setItem(row, 2, codeItem);
    table->setItem(row, 3, descItem);
    table->scrollToBottom();

    // 顶部报警指示条变红色
    if (auto* bar = m_mainWindow->findChild<QFrame*>(QStringLiteral("currentAlarmBar"))) {
        bar->setStyleSheet(
            QStringLiteral("QFrame#currentAlarmBar{background:#ffebee;border:1px solid #ef9a9a;border-radius:6px;}"));
    }
}

/**
 * @brief 清除所有报警记录
 *
 * 清空 alarmTable 的所有行；将顶部 currentAlarmBar 恢复为绿色，
 * 并将文字改为"✅ 当前无报警"。
 */
void UiController::onClearAlarms()
{
    // 清空表格
    auto* table = m_mainWindow->findChild<QTableWidget*>(QStringLiteral("alarmTable"));
    if (table) table->setRowCount(0);

    // 顶部报警条改为绿色 + 提示"当前无报警"
    if (auto* bar = m_mainWindow->findChild<QFrame*>(QStringLiteral("currentAlarmBar"))) {
        bar->setStyleSheet(
            QStringLiteral("QFrame#currentAlarmBar{background:#e8f5e9;border:1px solid #a5d6a7;border-radius:6px;}"));
        if (auto* lbl = bar->findChild<QLabel*>()) {
            lbl->setStyleSheet(
                QStringLiteral("color:#2e7d32;font-size:12px;font-weight:bold;border:none;background:transparent;"));
            lbl->setText(QStringLiteral("✅ 当前无报警"));
        }
    }
}

// ==================== 编程运行 ====================

/**
 * @brief 获取程序编辑器控件（MOVEJ/MOVEL/WAIT_IDLE 等指令写在这里）
 * @return objectName = "progEditor" 的 QPlainTextEdit 指针
 */
QPlainTextEdit* UiController::getEditor() const
{
    return m_mainWindow->findChild<QPlainTextEdit*>(QStringLiteral("progEditor"));
}

/**
 * @brief 在程序编辑器的光标位置插入一行文本（默认编辑器重载）
 */
void UiController::insertAtCursor(const QString& text)
{
    insertAtCursor(getEditor(), text);
}

/**
 * @brief 在指定编辑器的光标位置插入一行文本
 *
 * 逻辑：若光标不在行首，则先移到行尾并换行，再插入 text + 换行，
 *       这样能保证每条指令独占一行。
 *
 * @param editor 目标编辑器
 * @param text   要插入的一行文本（如 "MOVEJ(Target1, speed=25)"）
 */
void UiController::insertAtCursor(QPlainTextEdit* editor, const QString& text)
{
    if (!editor) return;

    QTextCursor cursor = editor->textCursor();
    // 若光标不在行首，则先移到行尾再换行
    if (cursor.positionInBlock() > 0) {
        cursor.movePosition(QTextCursor::EndOfBlock);
        editor->setTextCursor(cursor);
        editor->insertPlainText(QStringLiteral("\n"));
    }
    editor->insertPlainText(text + QStringLiteral("\n"));
}

/**
 * @brief 获取点位（TARGET）编辑器控件
 * @return objectName = "progTargets" 的 QPlainTextEdit 指针
 */
QPlainTextEdit* UiController::getTargetEditor() const
{
    return m_mainWindow->findChild<QPlainTextEdit*>(QStringLiteral("progTargets"));
}

/**
 * @brief 在控制台输出一条带时间戳的日志（用于脚本调试与运行结果展示）
 *
 * 每行的格式：[HH:mm:ss] <text>
 */
void UiController::appendConsole(const QString& text)
{
    auto* console = m_mainWindow->findChild<QPlainTextEdit*>(QStringLiteral("progConsoleOutput"));
    if (!console) return;
    QString ts = QDateTime::currentDateTime().toString(QStringLiteral("[HH:mm:ss] "));
    console->appendPlainText(ts + text);
}

/**
 * @brief 从 UI 读取默认运动参数（速度/加速度/减速度/超时）
 *
 * 优先级：
 *   - 若存在调试面板（debugSpeed / debugAccel / debugDecel）则覆盖默认值
 *   - 否则使用 spinDefaultSpeed / spinDefaultAccel 中的值
 *   - 最终仍未找到则使用固定默认 (speed=25, accel=50, decel=60, timeout=5000)
 *
 * @param[out] speed      速度
 * @param[out] accel      加速度
 * @param[out] decel      减速度
 * @param[out] timeoutMs  超时时间（毫秒）
 */
void UiController::readDefaultMotionParams(double& speed, double& accel, double& decel, int& timeoutMs)
{
    // 默认值兜底
    speed  = 25.0; accel = 50.0; decel = 60.0; timeoutMs = 5000;
    // 编程页默认速度/加速度
    if (auto* sp = m_mainWindow->findChild<QLineEdit*>(QStringLiteral("spinDefaultSpeed")))
        speed = sp->text().toDouble();
    if (auto* sp = m_mainWindow->findChild<QLineEdit*>(QStringLiteral("spinDefaultAccel")))
        accel = sp->text().toDouble();
    // 调试面板的参数优先级更高（覆盖上面的默认值）
    if (auto* sp = m_mainWindow->findChild<QLineEdit*>(QStringLiteral("debugSpeed")))
        speed = sp->text().toDouble();
    if (auto* sp = m_mainWindow->findChild<QLineEdit*>(QStringLiteral("debugAccel")))
        accel = sp->text().toDouble();
    if (auto* sp = m_mainWindow->findChild<QLineEdit*>(QStringLiteral("debugDecel")))
        decel = sp->text().toDouble();
}

/**
 * @brief 解析 TARGET 行 `}` 之后的 motion 后缀段
 *
 * 期望格式(可含或不含 `//`):
 *   `}` ... `#speed=N acc=N dec=N tmo=N` [空格] [`//` 注释]
 *
 * 规则:
 *   1. 先按 `//` 拆分,只在 `//` 之前(若有)查找 `#` 段
 *   2. 第一个非空白字符必须为 `#`,否则视为无 motion 段(注释里含 `#speed=...` 不会被误识别)
 *   3. 段内按空白拆键值对,键支持 speed/acc/accel/acceleration、dec/decel/deceleration、tmo/timeout
 *   4. 任何字段缺失或解析失败视作 0(对应"未设")
 *
 * @param tail `}` 之后到行尾的全部字符串
 * @return 解析结果
 */
MotionParams UiController::parseMotionSuffix(const QString& tail)
{
    MotionParams mp;
    // 只看 // 注释之前的内容(若没有 // 则整段都是 motion 段)
    QString prefix = tail;
    int cIdx = prefix.indexOf(QStringLiteral("//"));
    if (cIdx >= 0) prefix = prefix.left(cIdx);
    prefix = prefix.trimmed();
    if (prefix.isEmpty() || !prefix.startsWith(QChar::fromLatin1('#'))) return mp;
    // 去掉起始的 # 后按空白拆
    QString body = prefix.mid(1).trimmed();
    if (body.isEmpty()) return mp;
    const QStringList tokens = body.split(QRegularExpression(QStringLiteral("\\s+")),
                                          Qt::SkipEmptyParts);
    for (const QString& tok : tokens) {
        int eq = tok.indexOf(QChar::fromLatin1('='));
        if (eq <= 0) continue;
        const QString key = tok.left(eq).trimmed().toLower();
        bool ok = false;
        double val = tok.mid(eq + 1).trimmed().toDouble(&ok);
        if (!ok) continue;
        if (key == QStringLiteral("speed") ||
            key == QStringLiteral("spd") ||
            key == QStringLiteral("v")) {
            mp.speed = val;
        } else if (key == QStringLiteral("acc") ||
                   key == QStringLiteral("accel") ||
                   key == QStringLiteral("acceleration")) {
            mp.accel = val;
        } else if (key == QStringLiteral("dec") ||
                   key == QStringLiteral("decel") ||
                   key == QStringLiteral("deceleration")) {
            mp.decel = val;
        } else if (key == QStringLiteral("tmo") ||
                   key == QStringLiteral("timeout")) {
            mp.timeoutMs = static_cast<int>(val);
        }
        // 未知键忽略,保持向前兼容
    }
    return mp;
}

/**
 * @brief 把 MotionParams 格式化为 `#speed=N acc=N dec=N tmo=N` 段
 *
 * @param mp 运动参数(任一字段 > 0 才输出)
 * @return `#speed=... acc=... dec=... tmo=...`(前面带 `#`,字段间空格分隔);
 *         若 hasAny() 为 false 返回空串
 */
QString UiController::formatMotionSuffix(const MotionParams& mp)
{
    if (!mp.hasAny()) return QString();
    QStringList segs;
    // speed/accel/decel 是 double,timeoutMs 是 int — 分别用浮点/整数 arg 模板
    if (mp.speed    > 0) segs << QStringLiteral("speed=%1").arg(mp.speed,    0, 'f', 0);
    if (mp.accel    > 0) segs << QStringLiteral("acc=%1").arg(mp.accel,      0, 'f', 0);
    if (mp.decel    > 0) segs << QStringLiteral("dec=%1").arg(mp.decel,      0, 'f', 0);
    if (mp.timeoutMs > 0) segs << QStringLiteral("tmo=%1").arg(mp.timeoutMs);
    if (segs.isEmpty()) return QString();
    return QStringLiteral("#%1").arg(segs.join(QChar::fromLatin1(' ')));
}

/**
 * @brief 同步点位编辑器中 TARGET 的顺序（去抖后调用）
 *
 * 作用：当用户在程序区修改/删除 MOVEJ/MOVEL 指令后，
 *       自动按引用顺序重排点位区的 TARGET 行，
 *       同时将未被引用的 TARGET 保留在末尾。
 *
 * 流程：
 *   1. 从程序区正则匹配所有 MOVEJ/MOVEL/MOVEJOFF/MOVELOFF 引用的点位名，
 *      去重后按首次出现顺序保存到 refOrder
 *   2. 读取点位区，按 "TARGET <name> = ..." 的形式建立 name→line 的映射
 *   3. 先按 refOrder 顺序取已被引用的 TARGET，再把剩下未被引用的追加到末尾
 *   4. 整体写回点位编辑器（期间屏蔽 textChanged 信号避免递归触发）
 */
void UiController::syncTargets()
{
    // 正在重入（写回时又触发 textChanged）→ 忽略
    if (m_syncTargetsBusy) return;
    m_syncTargetsBusy = true;
    auto* targetEd = getTargetEditor();
    auto* progEd = getEditor();
    if (!targetEd || !progEd) { m_syncTargetsBusy = false; return; }

    // 1. 从程序区提取所有引用的目标名（保持首次出现顺序，去重）
    QString progText = progEd->toPlainText();
    // 正则：匹配 MOVEJ( / MOVEL( 等括号后紧跟的第一个标识符
    QRegularExpression refRe(QStringLiteral("\\b(?:MOVEJ|MOVEL|MOVEJOFF|MOVELOFF)\\(\\s*(\\w+)"));
    QSet<QString> seen;
    QStringList refOrder;
    auto it = refRe.globalMatch(progText);
    while (it.hasNext()) {
        auto m = it.next();
        QString name = m.captured(1);
        if (!seen.contains(name)) {
            seen.insert(name);
            refOrder.append(name);
        }
    }

    // 2. 读取点位编辑器所有 TARGET 行，建立 name→line+comment（行尾格式）
    QStringList targetLines = targetEd->toPlainText().split(QChar::fromLatin1('\n'), Qt::SkipEmptyParts);
    QHash<QString, QString> targetMap;   // name → TARGET line（不含注释）
    QHash<QString, QString> commentMap;  // name → comment
    for (const QString& line : targetLines) {
        QString trimmed = line.trimmed();
        if (!trimmed.startsWith(QStringLiteral("TARGET "), Qt::CaseInsensitive)) continue;
        int eq = trimmed.indexOf(QChar::fromLatin1('='));
        if (eq < 0) continue;
        QString name = trimmed.mid(7, eq - 7).trimmed();
        // 提取 } 之后 // 注释
        int br = trimmed.indexOf(QChar::fromLatin1('{'), eq);
        int e = (br >= 0) ? trimmed.indexOf(QChar::fromLatin1('}'), br) : -1;
        QString tail = (e >= 0) ? trimmed.mid(e + 1) : QString();
        int commentIdx = tail.indexOf(QStringLiteral("//"));
        QString comment = (commentIdx >= 0) ? tail.mid(commentIdx + 2).trimmed() : QString();
        // 去掉注释后存 targetMap
        QString lineWithoutComment = (commentIdx >= 0)
            ? trimmed.left(trimmed.size() - (tail.size() - commentIdx)).trimmed()
            : trimmed;
        targetMap.insert(name, lineWithoutComment);
        if (!comment.isEmpty())
            commentMap.insert(name, comment);
    }

    // 3. 按引用顺序重排（被引用的放前面，未引用的放末尾保留），附注释在行尾
    QStringList result;
    for (const QString& name : refOrder) {
        if (targetMap.contains(name)) {
            QString line = targetMap.take(name);
            if (commentMap.contains(name))
                line += QStringLiteral("// %1").arg(commentMap[name]);
            result.append(line);
        }
    }
    // 保留未被引用但已存在的（放在末尾）
    for (auto it2 = targetMap.begin(); it2 != targetMap.end(); ++it2) {
        QString name = it2.key();
        QString line = it2.value();
        if (commentMap.contains(name))
            line += QStringLiteral("// %1").arg(commentMap[name]);
        result.append(line);
    }

    // 4. 写回（阻止 textChanged 再次触发，防止递归）
    targetEd->blockSignals(true);
    targetEd->setPlainText(result.join(QChar::fromLatin1('\n')));
    targetEd->blockSignals(false);
    m_syncTargetsBusy = false;
}

/**
 * @brief 点位编辑器文本变化回调（用户手动编辑 TARGET 行）
 *
 * 行为：增量解析点位编辑器所有 TARGET 行，覆盖到 m_targets[name] = vals。
 *   - 已存在的 name → 覆盖值
 *   - 不存在的 name → 新增
 *   - 不删除任何 name（避免用户编辑过程中误删）
 *   - 不重排序（syncTargets 才负责重排）
 *
 * 调用链：progTargets->textChanged → 100ms 去抖 → 本函数 → m_targets[name]=vals
 *         → 下次 runText/parseScript 时会读到最新值
 */
void UiController::onTargetEditorChanged()
{
    auto* targetEd = getTargetEditor();
    if (!targetEd) return;

    QStringList lines = targetEd->toPlainText().split(QChar::fromLatin1('\n'), Qt::SkipEmptyParts);
    // 记录本次出现的 name,用于清理已删除点位的 m_targetMotion 残留
    QSet<QString> seenNames;
    for (const QString& rawLine : lines) {
        QString line = rawLine.trimmed();
        if (!line.startsWith(QStringLiteral("TARGET "), Qt::CaseInsensitive)) continue;
        int eq = line.indexOf(QChar::fromLatin1('='));
        int br = line.indexOf(QChar::fromLatin1('{'));
        if (eq < 0 || br < 0) continue;
        QString name = line.mid(7, eq - 7).trimmed();
        if (name.isEmpty()) continue;
        int e = line.indexOf(QChar::fromLatin1('}'), br);
        if (e < 0) continue;
        QString raw = line.mid(br + 1, e - br - 1);
        QList<double> vals;
        bool ok = true;
        for (const auto& p : raw.split(QChar::fromLatin1(','), Qt::SkipEmptyParts)) {
            QString t = p.trimmed();
            int colon = t.indexOf(QChar::fromLatin1(':'));
            QString numStr = (colon >= 0) ? t.mid(colon + 1).trimmed() : t;
            bool convOk = false;
            double v = numStr.toDouble(&convOk);
            if (!convOk) { ok = false; break; }
            vals.append(v);
        }
        // 至少 6 个值（关节 J0-J5 或笛卡尔 X/Y/Z/A/B/C）才算合法点位
        if (!ok || vals.size() < 6) continue;

        // 覆盖式更新（用户编辑就是合法源）
        m_targets[name] = vals;
        // 顺手更新 m_targetCounter，确保下次自动生成的点位名不冲突
        QRegularExpression numRe(QStringLiteral("(\\d+)"));
        auto m = numRe.match(name);
        if (m.hasMatch()) {
            int n = m.captured(1).toInt();
            if (n > m_targetCounter) m_targetCounter = n;
        }

        // 解析 } 之后的 motion 后缀段(#speed=N acc=N dec=N tmo=N)
        // 即便 motion 全 0 也写入 key,使得外部可区分"未出现"vs"出现但全 0"
        const QString tail = line.mid(e + 1);
        m_targetMotion[name] = parseMotionSuffix(tail);
        seenNames.insert(name);
    }

    // 清理被删点位的 m_targetMotion 残留(以及 m_targets 残留)
    if (!seenNames.isEmpty()) {
        for (auto it = m_targetMotion.begin(); it != m_targetMotion.end(); ) {
            if (!seenNames.contains(it.key())) it = m_targetMotion.erase(it);
            else ++it;
        }
        for (auto it = m_targets.begin(); it != m_targets.end(); ) {
            if (!seenNames.contains(it.key())) it = m_targets.erase(it);
            else ++it;
        }
    }

    // 点位变更后自动同步注释到程序编辑器
    syncProgramComments();
}

/** @brief 弹出对话框编辑当前流程中引用的所有点位 */
void UiController::showTargetEditDialog()
{
    // 屏蔽点位管理中所有 SpinBox 的鼠标滚轮修改数值:
    // 在弹窗打开期间拦截所有 wheel 事件(QSpinBox 默认 wheelFocus,
    // 鼠标 hover 就会触发增减),避免用户误修改
    class WheelBlocker : public QObject {
    public:
        using QObject::QObject;
        bool eventFilter(QObject* obj, QEvent* ev) override {
            if (ev->type() == QEvent::Wheel) return true;  // 吞掉
            return QObject::eventFilter(obj, ev);
        }
    };
    auto* progEd = getEditor();
    auto* targetEd = getTargetEditor();
    if (!progEd || !targetEd) return;

    // 1. 先从点位编辑器解析所有 TARGET 行
    struct TargetInfo {
        QString name;
        QMap<QString, double> values;
        QStringList labelOrder;
        QString rawBody;  // 大括号内的原始字符串
        QString comment;  // 行尾注释（} 之后 // 的内容）
        MotionParams motion;  // 点位级 speed/accel/decel/timeoutMs(0 表示未设)
    };
    QHash<QString, TargetInfo> infoMap;
    QStringList targetLines = targetEd->toPlainText().split(
        QChar::fromLatin1('\n'), Qt::SkipEmptyParts);
    QStringList allTargetNames;
    for (const QString& line : targetLines) {
        QString trimmed = line.trimmed();
        if (!trimmed.startsWith(QStringLiteral("TARGET "), Qt::CaseInsensitive)) continue;
        int eq = trimmed.indexOf(QChar::fromLatin1('='));
        int br = trimmed.indexOf(QChar::fromLatin1('{'));
        if (eq < 0 || br < 0) continue;
        QString name = trimmed.mid(7, eq - 7).trimmed();
        int e = trimmed.indexOf(QChar::fromLatin1('}'), br);
        if (e < 0) continue;
        QString body = trimmed.mid(br + 1, e - br - 1);
        // 提取 } 之后 // 注释
        QString tail = trimmed.mid(e + 1);
        int commentIdx = tail.indexOf(QStringLiteral("//"));
        QString comment = (commentIdx >= 0)
            ? tail.mid(commentIdx + 2).trimmed() : QString();
        TargetInfo info;
        info.name = name;
        info.rawBody = body.trimmed();
        info.comment = comment;
        // 解析 } 之后的 motion 后缀段 (#speed=N acc=N dec=N tmo=N)
        info.motion = parseMotionSuffix(tail);
        for (const auto& part : body.split(QChar::fromLatin1(','), Qt::SkipEmptyParts)) {
            QString p = part.trimmed();
            int colon = p.indexOf(QChar::fromLatin1(':'));
            if (colon < 0) continue;
            QString label = p.left(colon).trimmed();
            double val = p.mid(colon + 1).trimmed().toDouble();
            info.values[label] = val;
            info.labelOrder.append(label);
        }
        if (info.values.size() >= 6) {
            infoMap[name] = info;
            allTargetNames.append(name);
        }
    }

    // 2. 从程序区提取被引用的点位名 + 行级默认 motion(用于 TARGET 行没有 #speed 段时填充)
    QString progText = progEd->toPlainText();
    QRegularExpression refRe(QStringLiteral("\\b(?:MOVEJ|MOVEL|MOVEJOFF|MOVELOFF)\\s*\\(\\s*(\\w+)"));
    // 行级 motion 映射:name → MotionParams(只填 speed/acc/dec/timeout;未填字段保持 0)
    QHash<QString, MotionParams> lineMotionMap;
    {
        QRegularExpression lineRe(QStringLiteral(
            "\\b(?:MOVEJ|MOVEL)\\s*\\(\\s*(\\w+)\\s*,([^)]*)\\)"));
        auto lit = lineRe.globalMatch(progText);
        while (lit.hasNext()) {
            auto lm = lit.next();
            QString name = lm.captured(1).trimmed();
            QString args = lm.captured(2);
            MotionParams mp;
            // 解析 args 中的 speed=/acc=/dec=/timeout=
            QRegularExpression kwRe(QStringLiteral("(\\w+)\\s*=\\s*([\\d.]+)"));
            auto kit = kwRe.globalMatch(args);
            while (kit.hasNext()) {
                auto km = kit.next();
                QString k = km.captured(1).toLower();
                double v = km.captured(2).toDouble();
                if (k == QStringLiteral("speed"))        mp.speed = v;
                else if (k == QStringLiteral("acc"))    mp.accel = v;
                else if (k == QStringLiteral("dec"))    mp.decel = v;
                else if (k == QStringLiteral("timeout")) mp.timeoutMs = static_cast<int>(v);
            }
            // 用最近一次出现覆盖(用户最后一行写的就是默认)
            lineMotionMap[name] = mp;
        }
    }

    // 2.5 从程序编辑器中提取每个点位前的 SERVO 舵机数据，供舵机面板回显
    QHash<QString, ServoSlotArray> servosForTarget;
    {
        QRegularExpression moveExtractRe(QStringLiteral(
            "^(?:\\s*(?:#\\s*)?)?(?:MOVEJ|MOVEL|MOVEJOFF|MOVELOFF)\\s*\\(\\s*(\\w+)"),
            QRegularExpression::CaseInsensitiveOption);
        QRegularExpression servoExtractRe(QStringLiteral(
            "^\\s*(?:#\\s*)?SERVO\\s*\\("),
            QRegularExpression::CaseInsensitiveOption);
        QStringList progLines = progText.split(QChar::fromLatin1('\n'));
        // 从底向上扫描，为每个 MOVE 行向前找 SERVO 行（跳过空行和行注释，遇其他指令止步）
        for (int i = 0; i < progLines.size(); ++i) {
            auto mm = moveExtractRe.match(progLines[i]);
            if (!mm.hasMatch()) continue;
            QString tgtName = mm.captured(1).trimmed();
            if (servosForTarget.contains(tgtName)) continue; // 只取第一次（最靠前的 SERVO）

            for (int j = i - 1; j >= 0; --j) {
                QString prev = progLines[j].trimmed();
                if (prev.isEmpty() || prev.startsWith(QStringLiteral("//"))) continue;
                if (servoExtractRe.match(prev).hasMatch()) {
                    // 解析 SERVO(ch=ang:spd, ...) 或 SERVO(ch=ang:Tms, ...)
                    ServoSlotArray srv;
                    int pe = prev.indexOf(QChar::fromLatin1(')'));
                    QString inner = prev.mid(prev.indexOf(QChar::fromLatin1('(')) + 1, pe - prev.indexOf(QChar::fromLatin1('(')) - 1).trimmed();
                    if (!inner.isEmpty() && inner.contains(QChar::fromLatin1('='))) {
                        for (const QString& g : inner.split(QChar::fromLatin1(','))) {
                            auto chanPart = g.split(QChar::fromLatin1('='));
                            if (chanPart.size() >= 2) {
                                int ch = chanPart[0].trimmed().toInt();
                                auto angSpd = chanPart[1].split(QChar::fromLatin1(':'));
                                if (ch >= 0 && ch < 6) {
                                    srv[ch].channel = ch;
                                    srv[ch].angle   = angSpd.value(0, QString()).trimmed().toDouble();
                                    QString paramStr = (angSpd.size() >= 2) ? angSpd[1].trimmed() : QString();
                                    if (paramStr.startsWith(QLatin1Char('T'), Qt::CaseInsensitive)
                                        || paramStr.startsWith(QLatin1Char('t'), Qt::CaseInsensitive)) {
                                        srv[ch].timeMs = paramStr.mid(1).trimmed().toInt();
                                        srv[ch].speed  = 10.0;
                                    } else {
                                        srv[ch].speed  = paramStr.isEmpty() ? 10.0 : paramStr.toDouble();
                                    }
                                }
                            }
                        }
                        servosForTarget.insert(tgtName, srv);
                    }
                    break;
                }
                // 碰到非 SERVO 的其他指令行则停止
                break;
            }
        }
    }

    QSet<QString> seen;
    QStringList refOrder;
    auto it = refRe.globalMatch(progText);
    while (it.hasNext()) {
        auto m = it.next();
        QString name = m.captured(1);
        if (!seen.contains(name) && infoMap.contains(name)) {
            seen.insert(name);
            refOrder.append(name);
        }
    }

    // 3. 程序区无引用时,展示全部点位;有引用时,把未被引用的 TARGET 也追加到末尾
    // (否则"增加点位"新增的点位不在任何 MOVEJ 行里,下次开弹窗就看不到了)
    if (refOrder.isEmpty()) {
        if (allTargetNames.isEmpty()) {
            QMessageBox::information(m_mainWindow,
                QStringLiteral("无点位"),
                QStringLiteral("点位编辑器中无有效 TARGET 定义。"));
            return;
        }
        refOrder = allTargetNames;
    } else {
        // refOrder 非空,把 allTargetNames 中未被 refOrder 引用的追加到末尾
        for (const QString& name : allTargetNames) {
            if (!refOrder.contains(name))
                refOrder.append(name);
        }
    }

    // 4. 对话框：左右双区域布局 —— 左侧点位网格，右侧舵机网格
    QDialog dlg(m_mainWindow);
    dlg.setWindowTitle(QStringLiteral("点位管理"));
    dlg.setMinimumWidth(900);

    auto* mainLayout = new QVBoxLayout(&dlg);

    // "增加"按钮放在顶部
    auto* topBar = new QHBoxLayout;
    auto* addBtn = new QPushButton(QStringLiteral("＋ 增加点位"));
    addBtn->setStyleSheet(QStringLiteral(
        "QPushButton{background:#e8f5e9;color:#2e7d32;border:1px solid #a5d6a7;border-radius:4px;"
        "padding:4px 14px;font-size:13px;font-weight:bold;}"
        "QPushButton:hover{background:#c8e6c9;}"));
    topBar->addWidget(addBtn);
    topBar->addStretch();
    // 全屏切换按钮
    auto* fullscreenBtn = new QPushButton(QStringLiteral("\u26f6 全屏"), &dlg);
    fullscreenBtn->setCheckable(true);
    fullscreenBtn->setStyleSheet(QStringLiteral(
        "QPushButton{background:#fff;color:#555;border:1px solid #d0d5dd;border-radius:4px;"
        "padding:4px 12px;font-size:13px;}"
        "QPushButton:hover{background:#f5f5f5;}"
        "QPushButton:checked{background:#1565c0;color:#fff;border-color:#0d47a1;}"));
    fullscreenBtn->setToolTip(QStringLiteral("进入/退出全屏 (F11)"));
    topBar->addWidget(fullscreenBtn);
    QObject::connect(fullscreenBtn, &QPushButton::toggled, &dlg,
                     [fullscreenBtn, &dlg](bool checked) {
        if (checked) {
            dlg.setWindowState(dlg.windowState() | Qt::WindowFullScreen);
            fullscreenBtn->setText(QStringLiteral("\u26f6 退出全屏"));
        } else {
            dlg.setWindowState(dlg.windowState() & ~Qt::WindowFullScreen);
            fullscreenBtn->setText(QStringLiteral("\u26f6 全屏"));
        }
    });
    auto* fsShortcut = new QShortcut(QKeySequence(Qt::Key_F11), &dlg);
    fsShortcut->setContext(Qt::ApplicationShortcut);
    QObject::connect(fsShortcut, &QShortcut::activated, fullscreenBtn, [fullscreenBtn]() {
        fullscreenBtn->toggle();
    });
    mainLayout->addLayout(topBar);

    // ===== 检测程序中是否有 SERVO 指令 =====
    bool hasServo = !servosForTarget.isEmpty();

    // ===== 顶部切换按钮：点位 / 舵机 =====
    auto* toggleBar = new QHBoxLayout;
    toggleBar->setSpacing(0);
    toggleBar->setContentsMargins(4, 4, 4, 2);
    auto* btnPoint = new QPushButton(QStringLiteral("点位"));
    auto* btnServo = new QPushButton(QStringLiteral("舵机"));
    QString toggleStyle = QStringLiteral(
        "QPushButton{background:#f0f0f0;color:#555;border:1px solid #d0d5dd;"
        "padding:6px 20px;font-size:13px;font-weight:bold;}"
        "QPushButton:checked{background:#1565c0;color:#fff;border-color:#0d47a1;}"
        "QPushButton:disabled{background:#e8e8e8;color:#bbb;border-color:#ddd;}");
    btnPoint->setCheckable(true);
    btnPoint->setChecked(true);
    btnPoint->setStyleSheet(toggleStyle);
    btnServo->setCheckable(true);
    btnServo->setEnabled(hasServo);
    btnServo->setStyleSheet(toggleStyle);
    if (!hasServo)
        btnServo->setToolTip(QStringLiteral("程序中无 SERVO 指令"));
    toggleBar->addWidget(btnPoint);
    toggleBar->addWidget(btnServo);
    toggleBar->addStretch();
    mainLayout->addLayout(toggleBar);

    auto* stackedWidget = new QStackedWidget(&dlg);

    QObject::connect(btnPoint, &QPushButton::clicked, stackedWidget, [=]() {
        btnPoint->setChecked(true);
        btnServo->setChecked(false);
        stackedWidget->setCurrentIndex(0);
    });
    QObject::connect(btnServo, &QPushButton::clicked, stackedWidget, [=]() {
        if (!hasServo) return;
        btnPoint->setChecked(false);
        btnServo->setChecked(true);
        stackedWidget->setCurrentIndex(1);
    });

    // ===== 每组的数据结构 =====
    struct TargetGroup {
        QString name;
        QStringList labelOrder;  // 标签显示顺序，写回时按此遍历
        QString typeLabel;       // 点位类型标签："关节" / "笛卡尔"
        // 点位面板行的容器控件
        QWidget* pointRow = nullptr;
        // 舵机面板行的容器控件
        QWidget* servoRow = nullptr;
        // 点位坐标编辑框 (J0~J5 或 X/Y/Z/A/B/C)
        QHash<QString, QLineEdit*> edits;
        // 点位注释
        QLineEdit* commentEdit = nullptr;
        // 运动参数（SpinBox 无滚轮按钮，点击弹出软键盘）
        QSpinBox* spinSpeed = nullptr;
        QSpinBox* spinAccel = nullptr;
        QSpinBox* spinDecel = nullptr;
        QSpinBox* spinTimeout = nullptr;
        // 舵机面板控件
        QLineEdit* servoAngle[6] = {nullptr};
        QLineEdit* servoSpeed[6] = {nullptr};
        QLineEdit* servoCommentEdit = nullptr;   // 舵机注释（第二行）
    };
    QVector<TargetGroup> groups;

    // 生成新点位名
    int nextId = m_targetCounter;
    auto nextName = [&nextId]() -> QString {
        return QStringLiteral("Target%1").arg(++nextId);
    };

    // ===== 点位区域：每行一个紧凑水平布局（Target名 + 坐标 + 运动参数 + 注释 + 操作按钮） =====
    auto* pointScroll = new QScrollArea(&dlg);
    pointScroll->setWidgetResizable(true);
    auto* pointScrollContent = new QWidget;
    auto* groupsLayout = new QVBoxLayout(pointScrollContent);
    groupsLayout->setSpacing(1);

    // ===== 默认标签（预先计算，供表头和后续行使用） =====
    QStringList defaultLabels{QStringLiteral("J0"), QStringLiteral("J1"), QStringLiteral("J2"),
                              QStringLiteral("J3"), QStringLiteral("J4"), QStringLiteral("J5")};
    QStringList activeLabels = defaultLabels;
    if (!infoMap.isEmpty())
        activeLabels = infoMap.begin().value().labelOrder;

    // 表头 — 每个点位自带坐标标签，全局仅保留点位名列
    auto* pointHeader = new QWidget(pointScrollContent);
    auto* pointHeaderLayout = new QHBoxLayout(pointHeader);
    pointHeaderLayout->setSpacing(3);
    pointHeaderLayout->setContentsMargins(2, 0, 2, 0);
    auto* phLabelName = new QLabel(QStringLiteral("点位"), pointScrollContent);
    phLabelName->setFixedWidth(80);
    phLabelName->setFixedHeight(18);
    phLabelName->setAlignment(Qt::AlignCenter);
    phLabelName->setStyleSheet(QStringLiteral("color:#555;font-weight:bold;font-size:12px;font-family:Consolas,'Microsoft YaHei UI';"));
    pointHeaderLayout->addWidget(phLabelName);
    pointHeaderLayout->addStretch();
    groupsLayout->addWidget(pointHeader);

    // 辅助：紧凑型表头标签（仅舵机区使用）
    auto headerLabel = [&](const QString& text, int w = 60) -> QLabel* {
        auto* lbl = new QLabel(text);
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setFixedWidth(w);
        lbl->setFixedHeight(18);
        lbl->setStyleSheet(QStringLiteral("color:#555;font-weight:bold;font-size:12px;font-family:Consolas,'Microsoft YaHei UI';"));
        return lbl;
    };

    // ------- 区域二：舵机 (下方) -------
    auto* servoGroupBox = new QGroupBox(QStringLiteral("舵机"), &dlg);
    auto* servoOuterLayout = new QVBoxLayout(servoGroupBox);
    servoOuterLayout->setSpacing(0);
    servoOuterLayout->setContentsMargins(2, 4, 2, 4);

    // 舵机滚动区
    auto* servoScroll = new QScrollArea(servoGroupBox);
    servoScroll->setWidgetResizable(true);
    servoScroll->setFrameShape(QFrame::NoFrame);
    auto* servoScrollContent = new QWidget;
    auto* servoRowsLayout = new QVBoxLayout(servoScrollContent);
    servoRowsLayout->setSpacing(1);
    servoRowsLayout->setContentsMargins(0, 0, 0, 0);
    servoScroll->setWidget(servoScrollContent);
    servoOuterLayout->addWidget(servoScroll);

    // 将区域加入切换面板
    stackedWidget->addWidget(pointScroll);     // 第0页：点位
    stackedWidget->addWidget(servoGroupBox);    // 第1页：舵机
    mainLayout->addWidget(stackedWidget);

    // ===== 辅助函数：创建一行点位控件 + 对应舵机行 =====
    std::function<TargetGroup(const QString&, const QHash<QString, double>&,
                              const QStringList&, const QString&,
                              const MotionParams&,
                              const ServoSlotArray*)> createPointAndServoRow;
    createPointAndServoRow = [&](const QString& name,
                                  const QHash<QString, double>& vals,
                                  const QStringList& labels,
                                  const QString& comment,
                                  const MotionParams& motion,
                                  const ServoSlotArray* srvData) -> TargetGroup {
        TargetGroup g;
        g.name = name;
        g.labelOrder = labels;

        // 根据首标签判断点位类型
        if (!labels.isEmpty()) {
            QString firstLabel = labels.first();
            if (firstLabel.startsWith(QChar::fromLatin1('J')))
                g.typeLabel = QStringLiteral("关节");
            else
                g.typeLabel = QStringLiteral("笛卡尔");
        }

        // ---------- 点位行：多行布局（标题行 + 轴标签行 + 数据行 + 注释行）----------
        auto* pRow = new QWidget(pointScrollContent);
        auto* pRowOuter = new QVBoxLayout(pRow);
        pRowOuter->setSpacing(1);
        pRowOuter->setContentsMargins(2, 1, 2, 1);

        // 行1：点位名 + 类型 + stretch + 操作按钮
        auto* row1Layout = new QHBoxLayout;
        row1Layout->setSpacing(3);
        row1Layout->setContentsMargins(0, 0, 0, 0);
        pRowOuter->addLayout(row1Layout);

        // 点位名
        auto* nameLabel = new QLabel(name, pointScrollContent);
        nameLabel->setFixedWidth(80);
        nameLabel->setFixedHeight(24);
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->setStyleSheet(QStringLiteral("color:#333;font-weight:bold;font-size:12px;font-family:Consolas,'Microsoft YaHei UI';"));
        row1Layout->addWidget(nameLabel);

        // 注释紧接在点位名后
        g.commentEdit = new QLineEdit(pointScrollContent);
        g.commentEdit->setText(comment);
        g.commentEdit->setPlaceholderText(QStringLiteral("注释..."));
        g.commentEdit->setFixedHeight(24);
        g.commentEdit->setMinimumWidth(100);
        g.commentEdit->setStyleSheet(QStringLiteral(
            "QLineEdit{background:#fafafa; border:1px dashed #ccc; padding:2px 4px; font-size:12px;}"
            "QLineEdit:focus{background:#fff; border:1px solid #90caf9;}"));
        row1Layout->addWidget(g.commentEdit);

        row1Layout->addStretch(1);

        // 运动参数 SpinBox（稍后在行2中使用）
        QSpinBox* spinSpeed = nullptr;
        QSpinBox* spinAccel = nullptr;
        QSpinBox* spinDecel = nullptr;
        QSpinBox* spinTimeout_local = nullptr;
        QPushButton* copyBtn = nullptr;
        QPushButton* delBtn = nullptr;

        auto compactParamSpin = [](QSpinBox* sp) {
            sp->setFixedHeight(24);
            sp->setMinimumWidth(54);
            sp->setMaximumWidth(66);
            sp->setButtonSymbols(QAbstractSpinBox::NoButtons);
            sp->setAttribute(Qt::WA_InputMethodEnabled, true);
            sp->setAlignment(Qt::AlignCenter);
            sp->setStyleSheet(QStringLiteral(
                "QSpinBox{border:1px solid #d0d5dd;border-radius:3px;"
                "padding:1px 2px;font-size:13px;background:#ffffff;}"
                "QSpinBox:focus{border:1px solid #1565c0;}"));
        };

        // 行2：坐标数据列（标签在上，编辑框在下，竖向排列）
        auto* row2Layout = new QHBoxLayout;
        row2Layout->setSpacing(3);
        row2Layout->setContentsMargins(0, 0, 0, 0);
        pRowOuter->addLayout(row2Layout);
        // 留出点位名+类型的空间
        auto* colSpacer = new QWidget(pointScrollContent);
        colSpacer->setFixedWidth(80);
        row2Layout->addWidget(colSpacer);

        for (const QString& label : labels) {
            auto* colWidget = new QWidget(pointScrollContent);
            auto* colLayout = new QVBoxLayout(colWidget);
            colLayout->setSpacing(1);
            colLayout->setContentsMargins(0, 0, 0, 0);

            auto* lbl = new QLabel(label, colWidget);
            lbl->setAlignment(Qt::AlignCenter);
            lbl->setFixedHeight(16);
            lbl->setMinimumWidth(62);
            lbl->setMaximumWidth(78);
            lbl->setStyleSheet(QStringLiteral("color:#555;font-weight:bold;font-size:11px;font-family:Consolas,'Microsoft YaHei UI';"));
            colLayout->addWidget(lbl);

            double val = vals.value(label, 0.0);
            auto* edit = new QLineEdit(colWidget);
            edit->setText(QString::number(val, 'f', 3));
            edit->setFixedHeight(24);
            edit->setMinimumWidth(62);
            edit->setMaximumWidth(78);
            edit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            edit->setAlignment(Qt::AlignCenter);
            edit->setStyleSheet(QStringLiteral(
                "QLineEdit{border:1px solid #d0d5dd;border-radius:3px;"
                "padding:1px 2px;font-size:13px;background:#ffffff;"
                "selection-background-color:#264f78;selection-color:#ffffff;}"
                "QLineEdit:focus{border:1px solid #1565c0;}"));
            auto* v = new QDoubleValidator(-9999.999, 9999.999, 3, edit);
            v->setNotation(QDoubleValidator::StandardNotation);
            edit->setValidator(v);
            colLayout->addWidget(edit);

            g.edits.insert(label, edit);
            row2Layout->addWidget(colWidget);
        }
        // 速度/加速度/减速度 竖排列
        {
            auto* colWidget = new QWidget(pointScrollContent);
            auto* colLayout = new QVBoxLayout(colWidget);
            colLayout->setSpacing(1);
            colLayout->setContentsMargins(0, 0, 0, 0);

            auto* lbl = new QLabel(QStringLiteral("速度"), colWidget);
            lbl->setAlignment(Qt::AlignCenter);
            lbl->setFixedHeight(16);
            lbl->setMinimumWidth(54);
            lbl->setMaximumWidth(66);
            lbl->setStyleSheet(QStringLiteral("color:#888;font-size:11px;font-family:'Microsoft YaHei UI';"));
            colLayout->addWidget(lbl);

            spinSpeed = new QSpinBox(colWidget);
            spinSpeed->setMinimum(0); spinSpeed->setMaximum(10000);
            compactParamSpin(spinSpeed);
            colLayout->addWidget(spinSpeed);

            row2Layout->addWidget(colWidget);
        }
        {
            auto* colWidget = new QWidget(pointScrollContent);
            auto* colLayout = new QVBoxLayout(colWidget);
            colLayout->setSpacing(1);
            colLayout->setContentsMargins(0, 0, 0, 0);

            auto* lbl = new QLabel(QStringLiteral("加速度"), colWidget);
            lbl->setAlignment(Qt::AlignCenter);
            lbl->setFixedHeight(16);
            lbl->setMinimumWidth(54);
            lbl->setMaximumWidth(66);
            lbl->setStyleSheet(QStringLiteral("color:#888;font-size:11px;font-family:'Microsoft YaHei UI';"));
            colLayout->addWidget(lbl);

            spinAccel = new QSpinBox(colWidget);
            spinAccel->setMinimum(0); spinAccel->setMaximum(10000);
            compactParamSpin(spinAccel);
            colLayout->addWidget(spinAccel);

            row2Layout->addWidget(colWidget);
        }
        {
            auto* colWidget = new QWidget(pointScrollContent);
            auto* colLayout = new QVBoxLayout(colWidget);
            colLayout->setSpacing(1);
            colLayout->setContentsMargins(0, 0, 0, 0);

            auto* lbl = new QLabel(QStringLiteral("减速度"), colWidget);
            lbl->setAlignment(Qt::AlignCenter);
            lbl->setFixedHeight(16);
            lbl->setMinimumWidth(54);
            lbl->setMaximumWidth(66);
            lbl->setStyleSheet(QStringLiteral("color:#888;font-size:11px;font-family:'Microsoft YaHei UI';"));
            colLayout->addWidget(lbl);

            spinDecel = new QSpinBox(colWidget);
            spinDecel->setMinimum(0); spinDecel->setMaximum(10000);
            compactParamSpin(spinDecel);
            colLayout->addWidget(spinDecel);

            row2Layout->addWidget(colWidget);
        }
        // 操作按钮竖排列
        {
            auto* btnCol = new QWidget(pointScrollContent);
            auto* btnColLayout = new QVBoxLayout(btnCol);
            btnColLayout->setSpacing(1);
            btnColLayout->setContentsMargins(0, 0, 0, 0);

            auto* btnSpacer = new QWidget(btnCol);
            btnSpacer->setFixedHeight(16);
            btnColLayout->addWidget(btnSpacer);

            auto* copyBtnLocal = new QPushButton(QStringLiteral("复制"), btnCol);
            auto* delBtnLocal  = new QPushButton(QStringLiteral("删除"), btnCol);
            copyBtn = copyBtnLocal;
            delBtn = delBtnLocal;
            QString btnStyle = QStringLiteral(
                "QPushButton{background:#fff; border:1px solid #ccc; border-radius:3px;"
                "padding:2px 6px; font-size:11px;}"
                "QPushButton:hover{background:#f5f5f5;}");
            copyBtnLocal->setStyleSheet(btnStyle);
            delBtnLocal->setStyleSheet(btnStyle);
            copyBtnLocal->setFixedSize(36, 24);
            delBtnLocal->setFixedSize(36, 24);
            btnColLayout->addWidget(copyBtnLocal);
            btnColLayout->addWidget(delBtnLocal);

            row2Layout->addWidget(btnCol);
        }
        row2Layout->addStretch(1);

        // 填入 motion 值
        spinSpeed->setValue(static_cast<int>(motion.speed));
        spinAccel->setValue(static_cast<int>(motion.accel));
        spinDecel->setValue(static_cast<int>(motion.decel));

        g.pointRow = pRow;
        g.spinSpeed = spinSpeed;
        g.spinAccel = spinAccel;
        g.spinDecel = spinDecel;
        g.spinTimeout = spinTimeout_local;
        groupsLayout->addWidget(pRow);

        // ---------- 舵机行 ----------
        auto* sRow = new QWidget(servoScrollContent);
        auto* sRowOuter = new QVBoxLayout(sRow);
        sRowOuter->setSpacing(0);
        sRowOuter->setContentsMargins(0, 0, 0, 0);

        // 注释行（第一行，独占整行）
        {
            auto* srvCommentRow = new QHBoxLayout;
            srvCommentRow->setSpacing(4);
            srvCommentRow->setContentsMargins(0, 0, 0, 1);
            auto* srvCommentLabel = new QLabel(QStringLiteral("注释:"), servoScrollContent);
            srvCommentLabel->setFixedHeight(22);
            srvCommentLabel->setStyleSheet(QStringLiteral("font-size:12px;color:#888;"));
            srvCommentRow->addWidget(srvCommentLabel);
            auto* srvCommentEdit = new QLineEdit(servoScrollContent);
            srvCommentEdit->setFixedHeight(22);
            if (srvData && !srvData->comment.isEmpty()) {
                srvCommentEdit->setText(srvData->comment);
            }
            srvCommentEdit->setStyleSheet(QStringLiteral(
                "QLineEdit{background:#fafafa;border:1px dashed #ccc;border-radius:3px;"
                "font-size:12px;padding:1px 4px;}"
                "QLineEdit:focus{border:1px solid #1565c0;background:#fff;}"));
            srvCommentRow->addWidget(srvCommentEdit, 1);
            sRowOuter->addLayout(srvCommentRow);
            g.servoCommentEdit = srvCommentEdit;
        }

        // 舵机编辑控件（第二行）
        auto* sRowLayout = new QHBoxLayout;
        sRowLayout->setSpacing(1);
        sRowLayout->setContentsMargins(0, 0, 0, 0);
        sRowOuter->addLayout(sRowLayout);

        for (int s = 0; s < 6; ++s) {
            // A标签
            auto* aLabel = new QLabel(QStringLiteral("A%1:").arg(s), servoScrollContent);
            aLabel->setFixedHeight(24);
            aLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            aLabel->setStyleSheet(QStringLiteral("font-size:10px;font-weight:bold;color:#555;padding-right:1px;"));
            sRowLayout->addWidget(aLabel);

            // 角度 (0 ~ 180) — 优先取 .rbt 中 SERVO 数据
            auto* aEdit = new QLineEdit(servoScrollContent);
            double defAngle = (srvData && (*srvData)[s].enabled()) ? (*srvData)[s].angle : 0.0;
            aEdit->setText(QString::number(defAngle, 'f', 0));
            aEdit->setFixedHeight(24);
            aEdit->setMinimumWidth(38);
            aEdit->setMaximumWidth(42);
            aEdit->setAlignment(Qt::AlignCenter);
            aEdit->setStyleSheet(QStringLiteral(
                "QLineEdit{border:1px solid #d0d5dd;border-radius:3px;"
                "padding:1px 1px;font-size:13px;background:#ffffff;"
                "selection-background-color:#264f78;selection-color:#ffffff;}"
                "QLineEdit:focus{border:1px solid #1565c0;}"));
            auto* aVal = new QDoubleValidator(0.0, 180.0, 0, aEdit);
            aVal->setNotation(QDoubleValidator::StandardNotation);
            aEdit->setValidator(aVal);
            sRowLayout->addWidget(aEdit);
            g.servoAngle[s] = aEdit;

            // S标签
            auto* sLabel = new QLabel(QStringLiteral("S%1:").arg(s), servoScrollContent);
            sLabel->setFixedHeight(24);
            sLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            sLabel->setStyleSheet(QStringLiteral("font-size:10px;font-weight:bold;color:#555;padding-right:1px;"));
            sRowLayout->addWidget(sLabel);

            // 速度 (1 ~ 360) — 优先取 .rbt 中 SERVO 数据
            auto* spdEdit = new QLineEdit(servoScrollContent);
            double defSpeed = (srvData && (*srvData)[s].enabled()) ? (*srvData)[s].speed : 10.0;
            if (defSpeed <= 0.0) defSpeed = 10.0;
            spdEdit->setText(QString::number(defSpeed, 'f', 0));
            spdEdit->setFixedHeight(24);
            spdEdit->setMinimumWidth(36);
            spdEdit->setMaximumWidth(40);
            spdEdit->setAlignment(Qt::AlignCenter);
            spdEdit->setStyleSheet(QStringLiteral(
                "QLineEdit{border:1px solid #d0d5dd;border-radius:3px;"
                "padding:1px 1px;font-size:13px;background:#ffffff;"
                "selection-background-color:#264f78;selection-color:#ffffff;}"
                "QLineEdit:focus{border:1px solid #1565c0;}"));
            auto* sVal = new QDoubleValidator(1.0, 360.0, 0, spdEdit);
            sVal->setNotation(QDoubleValidator::StandardNotation);
            spdEdit->setValidator(sVal);
            sRowLayout->addWidget(spdEdit);
            g.servoSpeed[s] = spdEdit;
        }
        sRowLayout->addStretch();

        g.servoRow = sRow;
        servoRowsLayout->addWidget(sRow);

        // ---------- 复制按钮 ----------
        QObject::connect(copyBtn, &QPushButton::clicked, &dlg, [&dlg, &groups, &nextName,
                &createPointAndServoRow, &groupsLayout, &servoRowsLayout, name] {
            TargetGroup* srcGroup = nullptr;
            for (auto& grp : groups) {
                if (grp.name == name) { srcGroup = &grp; break; }
            }
            if (!srcGroup) return;

            QHash<QString, double> curVals;
            for (const QString& label : srcGroup->labelOrder)
                curVals[label] = srcGroup->edits.value(label)->text().toDouble();
            QString curComment = srcGroup->commentEdit->text();
            MotionParams curMotion;
            curMotion.speed     = srcGroup->spinSpeed->value();
            curMotion.accel     = srcGroup->spinAccel->value();
            curMotion.decel     = srcGroup->spinDecel->value();
            curMotion.timeoutMs = 0;

            QString newName = nextName();
            // 从源行复制舵机数据
            ServoSlotArray copySrvData;
            copySrvData.comment = srcGroup->servoCommentEdit ? srcGroup->servoCommentEdit->text() : QString();
            for (int s = 0; s < 6; ++s) {
                copySrvData[s].channel = s;
                if (srcGroup->servoAngle[s]) {
                    copySrvData[s].angle = srcGroup->servoAngle[s]->text().toDouble();
                    copySrvData[s].speed = srcGroup->servoSpeed[s]->text().toDouble();
                } else {
                    copySrvData[s].channel = -1;
                }
            }
            auto newGroup = createPointAndServoRow(newName, curVals, srcGroup->labelOrder,
                                                   curComment, curMotion, &copySrvData);

            int idx = groupsLayout->indexOf(srcGroup->pointRow);
            if (idx >= 0 && idx + 1 < groupsLayout->count()) {
                groupsLayout->insertWidget(idx + 1, newGroup.pointRow);
                servoRowsLayout->insertWidget(idx + 1, newGroup.servoRow);
            } else {
                groupsLayout->addWidget(newGroup.pointRow);
                servoRowsLayout->addWidget(newGroup.servoRow);
            }

            // Sync groups vector with layout order
            QVector<TargetGroup> ordered;
            for (int i = 0; i < groupsLayout->count(); ++i) {
                QWidget* row = groupsLayout->itemAt(i)->widget();
                for (auto& grp : groups) {
                    if (grp.pointRow == row) { ordered.append(grp); break; }
                }
            }
            ordered.append(newGroup);
            groups = ordered;
        });

        // ---------- 删除按钮 ----------
        QObject::connect(delBtn, &QPushButton::clicked, &dlg,
                [&groups, &groupsLayout, &servoRowsLayout,
                 pointRow = g.pointRow, servoRow = g.servoRow, this] {
            if (groups.size() <= 1) return;
            QString delName;
            for (const auto& grp : groups) {
                if (grp.pointRow == pointRow) { delName = grp.name; break; }
            }
            groupsLayout->removeWidget(pointRow);
            pointRow->deleteLater();
            servoRowsLayout->removeWidget(servoRow);
            servoRow->deleteLater();
            groups.erase(std::remove_if(groups.begin(), groups.end(),
                [&](const TargetGroup& t) { return t.pointRow == pointRow; }), groups.end());
            if (!delName.isEmpty()) {
                m_targetMotion.remove(delName);
                m_targets.remove(delName);
                m_targetServos.remove(delName);
                if (m_taskExecutor) {
                    m_taskExecutor->setTargets(m_targets);
                    m_taskExecutor->setTargetMotion(m_targetMotion);
                }
                qDebug() << "[点位管理] 删除点位:" << delName;
            }
        });

        return g;
    };

    // ===== 依据 refOrder 创建所有点位行 =====
    for (const QString& name : refOrder) {
        QHash<QString, double> vals;
        QString comment;
        MotionParams motion;
        QStringList labels = activeLabels;
        if (infoMap.contains(name)) {
            const auto& info = infoMap[name];
            labels = info.labelOrder;
            for (auto it = info.values.begin(); it != info.values.end(); ++it)
                vals[it.key()] = it.value();
            comment = info.comment;
            motion = info.motion;
        }
        if (!motion.hasAny() && lineMotionMap.contains(name)) {
            motion = lineMotionMap[name];
        }
        groups.append(createPointAndServoRow(name, vals, labels, comment, motion,
                                            servosForTarget.contains(name) ? &servosForTarget[name] : nullptr));
    }

    // ===== "增加"按钮 =====
    QObject::connect(addBtn, &QPushButton::clicked, &dlg, [&] {
        QMessageBox typeDlg(&dlg);
        typeDlg.setWindowTitle(QStringLiteral("选择点位类型"));
        typeDlg.setText(QStringLiteral("请选择要添加的点位类型:"));
        QPushButton* jointBtn = typeDlg.addButton(
            QStringLiteral("关节点位 (J0-J5)"), QMessageBox::AcceptRole);
        QPushButton* cartBtn = typeDlg.addButton(
            QStringLiteral("笛卡尔点位 (X/Y/Z/A/B/C)"), QMessageBox::AcceptRole);
        QPushButton* cancelBtn = typeDlg.addButton(QMessageBox::Cancel);
        typeDlg.setDefaultButton(jointBtn);
        typeDlg.exec();
        if (typeDlg.clickedButton() == cancelBtn) return;

        QStringList newLabels;
        if (typeDlg.clickedButton() == jointBtn) {
            newLabels = {QStringLiteral("J0"), QStringLiteral("J1"), QStringLiteral("J2"),
                         QStringLiteral("J3"), QStringLiteral("J4"), QStringLiteral("J5")};
        } else {
            newLabels = {QStringLiteral("X"), QStringLiteral("Y"), QStringLiteral("Z"),
                         QStringLiteral("A"), QStringLiteral("B"), QStringLiteral("C")};
        }
        QHash<QString, double> zeros;
        for (const QString& l : newLabels) zeros[l] = 0.0;
        groups.append(createPointAndServoRow(nextName(), zeros, newLabels,
                                              QString(), MotionParams(), nullptr));
    });

    // ===== 滚轮事件隔离：每个面板独立滚动，防止跨区传播 =====
    class WheelIsolator : public QObject {
    public:
        using QObject::QObject;
        bool eventFilter(QObject* obj, QEvent* ev) override {
            if (ev->type() == QEvent::Wheel) {
                auto* we = static_cast<QWheelEvent*>(ev);
                auto* sa = qobject_cast<QScrollArea*>(obj->parent());
                if (sa && sa->verticalScrollBar()) {
                    sa->verticalScrollBar()->setValue(
                        sa->verticalScrollBar()->value() - we->angleDelta().y());
                }
                return true; // 吞掉事件，阻止跨区传播
            }
            return QObject::eventFilter(obj, ev);
        }
    };
    pointScroll->viewport()->installEventFilter(new WheelIsolator(&dlg));
    servoScroll->viewport()->installEventFilter(new WheelIsolator(&dlg));

    groupsLayout->addStretch();
    pointScroll->setWidget(pointScrollContent);

    // ===== 底部按钮 =====
    auto* btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    auto* okBtn = new QPushButton(QStringLiteral("确定"));
    auto* cancelBtn = new QPushButton(QStringLiteral("取消"));
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);
    mainLayout->addLayout(btnLayout);

    QObject::connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
    QObject::connect(cancelBtn, &QPushButton::clicked, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    // 5. 写回点位编辑器 — 先收集分组数据，再与原有非TARGET行合并
    QHash<QString, QString> groupOutput;  // name -> "TARGET name = { ... }// comment"
    for (const auto& g : groups) {
        if (!g.pointRow) continue;
        QStringList pairs;
        for (const QString& label : g.labelOrder) {
            auto* edit = g.edits.value(label);
            double val = edit ? edit->text().toDouble() : 0.0;
            pairs.append(QStringLiteral("%1:%2").arg(label).arg(val, 0, 'f', 3));
        }
        QString comment = g.commentEdit->text().trimmed();
        MotionParams mp;
        mp.speed     = g.spinSpeed     ? g.spinSpeed->value()              : 0;
        mp.accel     = g.spinAccel     ? g.spinAccel->value()              : 0;
        mp.decel     = g.spinDecel     ? g.spinDecel->value()              : 0;
        mp.timeoutMs = g.spinTimeout   ? g.spinTimeout->value()            : 0;
        m_targetMotion[g.name] = mp;
        QString commentSeg = comment.isEmpty()
            ? QString()
            : QStringLiteral("// %1").arg(comment);
        groupOutput[g.name] = QStringLiteral("TARGET %1 = { %2 }%3")
            .arg(g.name)
            .arg(pairs.join(QStringLiteral(", ")))
            .arg(commentSeg);

        // 收集舵机角度到 m_targetServos
        ServoSlotArray srv;
        for (int s = 0; s < 6; ++s) {
            srv[s] = ServoSlot();
            srv[s].channel = s;
        }
        for (int s = 0; s < 6; ++s) {
            if (g.servoAngle[s]) srv[s].angle = g.servoAngle[s]->text().toDouble();
            if (g.servoSpeed[s]) srv[s].speed = g.servoSpeed[s]->text().toDouble();
        }
        srv.comment = g.servoCommentEdit ? g.servoCommentEdit->text().trimmed() : QString();
        m_targetServos[g.name] = srv;
    }

    // 构建原有的 TARGET 名集合
    QSet<QString> oldTargetNames;
    for (const QString& t : refOrder) oldTargetNames.insert(t);

    // 合并：原有行中的 TARGET 替换为 groupOutput，被删除的 TARGET 不追加
    QStringList newTargetLines;
    for (const QString& oldLine : targetLines) {
        QString trimmed = oldLine.trimmed();
        if (trimmed.startsWith(QStringLiteral("TARGET "), Qt::CaseInsensitive)) {
            int eq = trimmed.indexOf(QChar::fromLatin1('='));
            if (eq > 7) {
                QString name = trimmed.mid(7, eq - 7).trimmed();
                if (groupOutput.contains(name)) {
                    newTargetLines.append(groupOutput.take(name));
                    continue;
                }
                qDebug() << "[点位管理] 跳过被删除点位:" << name;
                continue;
            }
        }
        newTargetLines.append(oldLine);
    }
    // 追加新增点位
    for (const auto& g : groups) {
        if (!g.pointRow) continue;
        if (groupOutput.contains(g.name))
            newTargetLines.append(groupOutput[g.name]);
    }

    targetEd->setPlainText(newTargetLines.join(QChar::fromLatin1('\n')));
    qDebug() << "[点位管理] 写回后 targetEd 文本:" << targetEd->toPlainText();
    if (m_taskExecutor) {
        m_taskExecutor->setTargets(m_targets);
    }
    // 同步程序编辑器中的 MOVEJ/MOVEL 行的 speed/acc/dec/timeout 关键字参数
    // 先于 onTargetEditorChanged 执行，因此时 m_targetMotion 仍持有弹窗写入的正确值
    syncMotionInMoveLines();

    // 同步程序编辑器中的 SERVO 舵机行：把 m_targetServos 的角度/速度写回
    syncServoInMoveLines();

    // 把更新后的 m_targetMotion 推给 TaskExecutor
    if (m_taskExecutor)
        m_taskExecutor->setTargetMotion(m_targetMotion);

    // 最后触发 TARGET 行解析（会从无 motion 段的 TARGET 行清零 m_targetMotion，
    // 但同步与下发已先行完成）
    onTargetEditorChanged();

    // 自动保存:用户点确定后,把修改直接持久化到当前 .rbt 文件,
    // 避免用户每次都要再手动点"保存"按钮
    // 如果之前没打开过文件(m_currentScriptPath 为空),不自动保存,等用户首次手动保存
    if (!m_currentScriptPath.isEmpty()) {
        saveScriptToFile(m_currentScriptPath);
    }
}

/**
 * @brief 同步程序编辑器中的 MOVEJ/MOVEL 行的 speed/acc/dec/timeout 关键字参数
 *
 * 把点位 motion (m_targetMotion) 中的值同步到程序编辑器对应行:
 *   - 扫描每行 `MOVEJ(P1, speed=X, acc=Y, dec=Z, timeout=T)` / `MOVEL(...)`
 *   - 提取点位名,查 m_targetMotion[name] 取最新的 speed/acc/dec/timeout
 *   - 如果该点位 motion hasAny(),则用新值替换行级 speed=/acc=/dec=/timeout=
 *   - 如果 motion 全 0 (未设),不动该行,保留用户原本的行级值
 *
 * 这样用户改了点位的 motion,程序编辑器的行级参数会同步更新,双向一致。
 */
void UiController::syncMotionInMoveLines()
{
    auto* progEd = getEditor();
    if (!progEd) return;

    QString progText = progEd->toPlainText();
    QStringList lines = progText.split(QChar::fromLatin1('\n'));
    bool modified = false;

    QRegularExpression moveRe(QStringLiteral(
        "^(\\s*(?:#\\s*)?(?:MOVEJ|MOVEL)\\s*\\(\\s*)(\\w+)(\\s*,\\s*)([^)]*)(\\))(.*)$"),
        QRegularExpression::CaseInsensitiveOption);

    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i];
        auto m = moveRe.match(line);
        if (!m.hasMatch()) continue;

        QString prefix = m.captured(1);   // "MOVEJ(" (不含点位名)
        QString name   = m.captured(2).trimmed();
        QString midSep = m.captured(3);    // ", "
        QString args   = m.captured(4).trimmed();   // "speed=25, acc=50, dec=60"
        QString suffix = m.captured(5);              // ")"
        QString tail   = m.captured(6);              // " // 注释"

        if (!m_targetMotion.contains(name)) continue;
        const MotionParams& mp = m_targetMotion[name];
        if (!mp.hasAny()) continue;   // 点位 motion 全 0 → 不动行级参数

        // 解析当前 args,保留其它关键字(只改 speed/acc/dec/timeout)
        QStringList keepOrder;
        QHash<QString, QString> otherKeys;
        for (const QString& seg : args.split(QChar::fromLatin1(','), Qt::SkipEmptyParts)) {
            QString s = seg.trimmed();
            int eq = s.indexOf(QChar::fromLatin1('='));
            if (eq <= 0) continue;
            QString k = s.left(eq).trimmed().toLower();
            if (k == QStringLiteral("speed") || k == QStringLiteral("acc") ||
                k == QStringLiteral("dec")  || k == QStringLiteral("timeout")) {
                continue;  // 跳过老的 speed/acc/dec/timeout,稍后用点位的值替换
            }
            otherKeys[k] = s;
            keepOrder.append(k);
        }
        // 按 [name, ...others, speed, acc, dec, timeout] 顺序生成
        QStringList allSegs;
        allSegs.append(name);
        for (const QString& k : keepOrder) {
            allSegs.append(otherKeys[k]);
        }
        if (mp.speed     > 0) allSegs.append(QStringLiteral("speed=%1").arg(mp.speed,     0, 'f', 0));
        if (mp.accel     > 0) allSegs.append(QStringLiteral("acc=%1").arg(mp.accel,       0, 'f', 0));
        if (mp.decel     > 0) allSegs.append(QStringLiteral("dec=%1").arg(mp.decel,       0, 'f', 0));
        if (mp.timeoutMs > 0) allSegs.append(QStringLiteral("timeout=%1").arg(mp.timeoutMs));

        QString newLine = prefix + allSegs.join(QStringLiteral(", ")) + suffix + tail;
        if (newLine != line) {
            lines[i] = newLine;
            modified = true;
        }
    }

    if (modified) {
        progEd->setPlainText(lines.join(QChar::fromLatin1('\n')));
    }
}

/**
 * @brief 同步程序编辑器中的 SERVO 行(舵机角度/速度):把点位 m_targetServos
 *        的舵机值写回紧随 MOVEJ/MOVEL/MOVEJOFF/MOVELOFF 之前的 SERVO 指令;
 *        若该点位无 SERVO 行则自动插入一条。
 *
 * 逻辑:
 *   - 扫描程序编辑器每一行,匹配 MOVEJ/MOVEL/MOVEJOFF/MOVELOFF
 *   - 提取点位名,查 m_targetServos[name] 获取最新的6路舵机角度/速度
 *   - 向前扫描查找该 MOVE 行之前的 SERVO 行:
 *       * 找到 → 用新的舵机值重写该 SERVO 行
 *       * 没找到(遇到其他指令或文件开头) → 在该 MOVE 行上方插入一条 SERVO
 *   - 跳过空行和行注释,不跨越其他指令行
 */
void UiController::syncServoInMoveLines()
{
    auto* progEd = getEditor();
    if (!progEd) return;

    QString progText = progEd->toPlainText();
    QStringList lines = progText.split(QChar::fromLatin1('\n'));

    QRegularExpression moveRe(QStringLiteral(
        "^(\\s*(?:#\\s*)?(?:MOVEJ|MOVEL|MOVEJOFF|MOVELOFF)\\s*\\(\\s*)(\\w+)"),
        QRegularExpression::CaseInsensitiveOption);
    QRegularExpression servoRe(QStringLiteral(
        "^\\s*(?:#\\s*)?SERVO\\s*\\("),
        QRegularExpression::CaseInsensitiveOption);

    bool modified = false;

    for (int i = 0; i < lines.size(); ++i) {
        auto m = moveRe.match(lines[i]);
        if (!m.hasMatch()) continue;

        QString name = m.captured(2).trimmed();
        if (!m_targetServos.contains(name)) continue;

        const ServoSlotArray& srv = m_targetServos[name];

        // 构建 SERVO(ch=ang:spd, ...) 字符串
        QStringList segments;
        for (int ch = 0; ch < 6; ++ch) {
            if (!srv[ch].enabled()) continue;
            double spd = srv[ch].speed;
            if (spd <= 0.0) spd = 10.0;  // 默认速度
            segments.append(QStringLiteral("%1=%2:%3")
                .arg(srv[ch].channel)
                .arg(srv[ch].angle, 0, 'f', 0)
                .arg(spd, 0, 'f', 0));
        }
        if (segments.isEmpty()) continue;

        QString newServoLine = QStringLiteral("SERVO(%1)")
            .arg(segments.join(QStringLiteral(", ")));

        // 向前找该 MOVE 行紧邻的 SERVO 行
        bool foundServo = false;
        for (int j = i - 1; j >= 0; --j) {
            QString trimmed = lines[j].trimmed();
            if (trimmed.isEmpty()) continue;
            if (trimmed.startsWith(QStringLiteral("//"))) continue;

            if (servoRe.match(trimmed).hasMatch()) {
                // 找到现有 SERVO → 替换
                if (lines[j] != newServoLine) {
                    lines[j] = newServoLine;
                    modified = true;
                }
                foundServo = true;
                break;
            }
            // 遇到其他非空非注释行 → 不再往上找
            break;
        }

        if (!foundServo) {
            // 在 MOVE 行上方插入一条 SERVO
            lines.insert(i, newServoLine);
            ++i;  // 跳过刚插入的行
            modified = true;
        }
    }

    if (modified) {
        progEd->setPlainText(lines.join(QChar::fromLatin1('\n')));
    }
}

/**
 * @brief 同步程序注释：将点位编辑器中的 TARGET 行尾注释同步到程序编辑器
 *
 * 扫描点位编辑器的 TARGET 行，提取 `// 注释` 部分，建立 name→comment 映射。
 * 然后遍历程序编辑器，对每条 MOVEJ/MOVEL/MOVEJOFF/MOVELOFF 指令，
 * 若引用的目标点位有注释且该行尚未包含注释，则追加行尾注释。
 */
void UiController::syncProgramComments()
{
    if (m_syncingComments) return;
    auto* targetEd = getTargetEditor();
    auto* progEd = getEditor();
    if (!targetEd || !progEd) return;

    m_syncingComments = true;

    // 1. 从点位编辑器提取 name → comment 映射
    QHash<QString, QString> targetComments;
    const QStringList targetLines = targetEd->toPlainText().split(QChar::fromLatin1('\n'), Qt::SkipEmptyParts);
    for (const QString& line : targetLines) {
        QString trimmed = line.trimmed();
        if (!trimmed.startsWith(QStringLiteral("TARGET "), Qt::CaseInsensitive)) continue;
        int eq = trimmed.indexOf(QChar::fromLatin1('='));
        if (eq < 0) continue;
        QString name = trimmed.mid(7, eq - 7).trimmed();
        int br = trimmed.indexOf(QChar::fromLatin1('{'), eq);
        int e = (br >= 0) ? trimmed.indexOf(QChar::fromLatin1('}'), br) : -1;
        QString tail = (e >= 0) ? trimmed.mid(e + 1) : trimmed.mid(eq + 1);
        int commentIdx = tail.indexOf(QStringLiteral("//"));
        if (commentIdx >= 0)
            targetComments[name] = tail.mid(commentIdx + 2).trimmed();
    }

    if (targetComments.isEmpty()) return;

    // 2. 扫描程序编辑器，追加注释到引用有点位注释的 MOVE 行
    const QStringList progLines = progEd->toPlainText().split(QChar::fromLatin1('\n'));
    QStringList result;
    QRegularExpression moveRe(
        QStringLiteral("\\b(MOVEJ|MOVEL|MOVEJOFF|MOVELOFF)\\s*\\(\\s*(\\w+)"),
        QRegularExpression::CaseInsensitiveOption);

    for (int i = 0; i < progLines.size(); ++i) {
        const QString& line = progLines[i];
        QString trimmed = line.trimmed();
        auto m = moveRe.match(trimmed);
        if (m.hasMatch()) {
            QString targetName = m.captured(2);
            if (targetComments.contains(targetName) && !trimmed.contains(QStringLiteral("//")))
                result.append(line + QStringLiteral("// %1").arg(targetComments[targetName]));
            else
                result.append(line);
        } else {
            result.append(line);
        }
    }

    // 3. 写回程序编辑器（仅内容有变化时，避免触发不必要的 textChanged）
    QString newText = result.join(QChar::fromLatin1('\n'));
    if (newText == progEd->toPlainText()) {
        m_syncingComments = false;
        return;
    }
    QTextCursor cursor = progEd->textCursor();
    int oldPos = cursor.position();
    progEd->setPlainText(newText);
    int newLen = progEd->toPlainText().length();
    cursor.setPosition(qMin(oldPos, newLen));
    progEd->setTextCursor(cursor);
    m_syncingComments = false;
}


/**
 * @brief "运行"按钮回调：解析脚本并交给 TaskExecutor 逐条执行
 *
 * 流程：
 *   1. 检查是否已连接硬件，未连接则弹窗提示
 *   2. 先调用 syncTargets() 同步点位顺序
 *   3. 将点位区 (TARGET) + 程序区 (MOVEJ/MOVEL/...) 合并为完整文本
 *   4. 调用 runText() 解析并执行
 */
void UiController::onProgRun()
{
    if (!m_robotController || !m_robotController->isConnected()) {
        QMessageBox::warning(nullptr, QStringLiteral("未连接"),
                             QStringLiteral("请先连接控制器"));
        return;
    }

    auto* editor = getEditor();
    if (!editor) return;

    // 同步点位顺序（保证 TARGET 行按被引用的顺序排列）
    syncTargets();

    // 组装完整脚本：点位区 + 程序区
    QString text; auto* targetEd = getTargetEditor();
    if (targetEd) text += targetEd->toPlainText() + QStringLiteral("\n");
    text += editor->toPlainText();
    if (text.isEmpty()) {
        QMessageBox::information(nullptr, QStringLiteral("空脚本"),
                                 QStringLiteral("编程编辑器中无指令"));
        return;
    }
    runText(text, QString());
}

/**
 * @brief 解析并执行一段脚本文本（用于整体运行或单步调试）
 *
 * @param text       完整脚本文本（含 TARGET 定义与指令）
 * @param sourceLine 当单步执行时传入选中的源行，仅用于日志展示；空串表示整脚本
 */
void UiController::runText(const QString& text, const QString& sourceLine)
{
    if (!m_robotController || !m_robotController->isConnected()) {
        QMessageBox::warning(nullptr, QStringLiteral("未连接"), QStringLiteral("请先连接控制器"));
        return;
    }

    // 清空控制台
    auto* console = m_mainWindow->findChild<QPlainTextEdit*>(QStringLiteral("progConsoleOutput"));
    if (console) console->clear();

    // 清除旧点位缓存，由 parseScript 重新解析（m_targetCounter 也会重新计算）
    // m_targets.clear();  // 注释：单步模式保留 onTargetEditorChanged 已同步的实时点位
    QList<ScriptCommand> commands = parseScript(text);
    if (commands.isEmpty()) {
        appendConsole(QStringLiteral("✘ 无法解析: %1").arg(sourceLine.isEmpty() ? text.trimmed() : sourceLine));
        return;
    }

    // 关闭 ZmotionAPI 的详细日志，避免刷屏
    ZmotionAPI::setVerbose(false);

    // 决定日志中的文件名（若有保存路径则使用其文件名，否则标记为未命名）
    QString fileName = m_currentScriptPath.isEmpty()
        ? QStringLiteral("未命名.rbt") : QFileInfo(m_currentScriptPath).fileName();

    appendConsole(QStringLiteral("══════════════════════════════"));
    if (!sourceLine.isEmpty()) {
        appendConsole(QStringLiteral("▶ 单步执行: %1").arg(sourceLine));
    } else {
        appendConsole(QStringLiteral("▶ 开始执行: %1").arg(fileName));
    }
    // 把点位表与命令序列交给 TaskExecutor，然后启动
    m_taskExecutor->setTargets(m_targets);
    m_taskExecutor->setScriptName(fileName);
    m_taskExecutor->setScriptPath(m_currentScriptPath); // CALL 子脚本时按主脚本同目录解析
    m_taskExecutor->setCommands(commands);
    m_taskExecutor->start();
}

/**
 * @brief 单步执行：解析整脚本，设置指令队列，然后调用 TaskExecutor::stepOnce()。
 *
 * 行为：
 *   - 首次调用（startLine >= 0）清空控制台，解析脚本，把 m_currentIdx 跳到包含
 *     startLine 的那条指令；连续点击会从下一条继续（startLine = -1）。
 *   - 当前已有任务在跑时直接返回，避免抢占。
 *   - 如果给定点位后没有可执行指令，错误提示且不启动。
 *
 * @param fullText   完整脚本（含 TARGET 定义与指令）
 * @param startLine  起始源代码行号，-1 = 继续上次的进度
 */
void UiController::runStep(const QString& fullText, int startLine)
{
    if (!m_robotController || !m_robotController->isConnected()) {
        QMessageBox::warning(nullptr, QStringLiteral("未连接"), QStringLiteral("请先连接控制器"));
        return;
    }
    if (m_taskExecutor->isRunning() || m_taskExecutor->isPaused()) {
        // 已有任务在跑，单步模式不能并行
        return;
    }

    QString fileName = m_currentScriptPath.isEmpty()
        ? QStringLiteral("未命名.rbt") : QFileInfo(m_currentScriptPath).fileName();

    // 首次进入：清控制台、解析整脚本
    if (startLine >= 0) {
        auto* console = m_mainWindow->findChild<QPlainTextEdit*>(QStringLiteral("progConsoleOutput"));
        if (console) console->clear();
        ZmotionAPI::setVerbose(false);

        m_targets.clear();
        m_targetMotion.clear();
        QList<ScriptCommand> commands = parseScript(fullText);
        if (commands.isEmpty()) {
            appendConsole(QStringLiteral("✘ 无法解析脚本"));
            return;
        }
        m_taskExecutor->setTargets(m_targets);
        m_taskExecutor->setTargetMotion(m_targetMotion);
        m_taskExecutor->setScriptName(fileName);
        m_taskExecutor->setScriptPath(m_currentScriptPath);
        m_taskExecutor->setCommands(commands);

        appendConsole(QStringLiteral("══════════════════════════════"));
        appendConsole(QStringLiteral("▶ 单步模式: %1").arg(fileName));

        // 把 m_currentIdx 定位到 startLine 对应的指令
        int targetIdx = -1;
        for (int i = 0; i < commands.size(); ++i) {
            if (commands[i].sourceLineNo >= startLine) { targetIdx = i; break; }
        }
        if (targetIdx < 0) {
            appendConsole(QStringLiteral("⚠ 指定行号不在指令列表内，从头开始"));
            targetIdx = 0;
        }
        m_taskExecutor->setStartIndex(targetIdx);
    }

    // 关闭 ZmotionAPI 详细日志在首次打开；后续由 runStep 进入不会重复设置
    ZmotionAPI::setVerbose(false);
    m_stepMode = true;
    m_taskExecutor->stepOnce();
}

/**
 * @brief 将脚本文本解析为 ScriptCommand 列表（脚本指令的词法/语法解析核心函数）
 *
 * 支持的指令类型：
 *   - TARGET <name> = {J0:, J1:, ...}         → 点位定义（存入 m_targets，不产生执行动作）
 *   - TARGET <name> = {X:, Y:, Z:, A:, B:, C:} → 笛卡尔点位
 *   - MOVEJ(name, speed=, acc=, dec=, timeout=)→ 关节空间运动（支持新/旧两种写法）
 *   - MOVEL(...)                                → 笛卡尔空间直线运动
 *   - MOVEJOFF(offset...) / MOVELOFF(offset...) → 相对偏移运动
 *   - WAIT_IDLE() / READ_POS()                 → 等待轴停止 / 读当前位置
 *   - FOR(var, start, end) ... END_FOR         → 循环
 *   - WHILE TRUE / WHILE_DI(io,val) ... END_WHILE → 循环
 *   - SPEED(x) / ACCEL(x) / DECEL(x) / TIMEOUT(x) → 运行中参数设置
 *   - IF_DI(io, val) ... END_IF                → 条件分支
 *   - <var> = <number> 或 <var> = <expr>       → 变量赋值
 *   - DELAY(ms)                                 → 延时（毫秒）
 *   - SET_DO(io, val)                           → 写数字输出
 *   - WAIT_DI(io, val, timeout)                 → 等待 DI 状态
 *   - PRINT("text")                             → 打印文本到控制台
 *   - CALL("file.rbt")                          → 调用子脚本（子脚本由外部解析）
 *   - # 注释                                    → 整行注释，忽略
 *
 * @param text  完整脚本文本
 * @return      解析出的指令列表（可直接交给 TaskExecutor）
 */
QList<ScriptCommand> UiController::parseScript(const QString& text)
{
    QList<ScriptCommand> result;
    double defSpd, defAcc, defDecel; int defTimeout;
    readDefaultMotionParams(defSpd, defAcc, defDecel, defTimeout);

    QStringList lines = text.split(QChar::fromLatin1('\n'), Qt::KeepEmptyParts);
    for (int srcLineNo = 0; srcLineNo < lines.size(); ++srcLineNo) {
        const QString& rawLine = lines[srcLineNo];
        QString line = rawLine.trimmed();
        if (line.isEmpty() || line.startsWith(QChar::fromLatin1('#')))
            continue;

        ScriptCommand cmd;
        cmd.speed = defSpd; cmd.accel = defAcc; cmd.decel = defDecel; cmd.timeoutMs = defTimeout;
        cmd.sourceLine = line;
        cmd.sourceLineNo = srcLineNo;

        if (line.startsWith(QStringLiteral("TARGET "), Qt::CaseInsensitive)) {
            // TARGET P1 = {J0: 0, J1: 0, ...} 或 {X: 0, Y: 0, ...}
            int eq = line.indexOf(QChar::fromLatin1('='));
            int br = line.indexOf(QChar::fromLatin1('{'));
            if (eq < 0 || br < 0) continue;
            QString name = line.mid(7, eq - 7).trimmed();  // "P1"
            int e = line.indexOf(QChar::fromLatin1('}'), br);
            if (e < 0) continue;
            QString raw = line.mid(br + 1, e - br - 1);
            QList<double> vals;
            for (const auto& p : raw.split(QChar::fromLatin1(','), Qt::SkipEmptyParts)) {
                QString t = p.trimmed();
                int colon = t.indexOf(QChar::fromLatin1(':'));
                vals.append(t.mid(colon + 1).trimmed().toDouble());
            }
            if (vals.size() >= 6) {
                // 已存在的 name 也覆盖（用户手动编辑点位 → onTargetEditorChanged 已写入，
                // 此处再次"覆盖"即最新值；首次发现则插入）。覆盖逻辑天然兼容两种情况。
                if (!m_targets.contains(name)) {
                    QRegularExpression numRe(QStringLiteral("(\\d+)"));
                    auto m = numRe.match(name);
                    if (m.hasMatch()) {
                        int n = m.captured(1).toInt();
                        if (n > m_targetCounter) m_targetCounter = n;
                    }
                }
                m_targets[name] = vals;   // 直接覆盖/插入，覆盖路径兼容"用户编辑"
                // 同步 motion 段(#speed=N acc=N dec=N tmo=N) 到 m_targetMotion
                m_targetMotion[name] = parseMotionSuffix(line.mid(e + 1));
            } else {
                // 6 坐标不齐但仍尝试读 motion,允许部分点位只定义 motion
                m_targetMotion[name] = parseMotionSuffix(line.mid(e + 1));
            }
        } else if (line.startsWith(QStringLiteral("MOVEJ("), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::MoveJ;
            int s = line.indexOf(QChar::fromLatin1('{'));
            if (s >= 0) {
                // 旧格式: MOVEJ({J0: 0, ...}, speed=25)
                int e = line.indexOf(QChar::fromLatin1('}'));
                if (e < 0) continue;
                QString raw = line.mid(s+1, e-s-1);
                for (const auto& p : raw.split(QChar::fromLatin1(','), Qt::SkipEmptyParts)) {
                    QString t = p.trimmed();
                    int colon = t.indexOf(QChar::fromLatin1(':'));
                    QString val = t.mid(colon + 1).trimmed();
                    cmd.targetRaw.append(val);
                    cmd.targets.append(val.toDouble());
                }
                if (cmd.targets.size() < 6) continue;
            } else {
                // 新格式: MOVEJ(P1, speed=25, acc=50, dec=60)
                int p0 = line.indexOf(QChar::fromLatin1('('));
                int p1 = line.indexOf(QChar::fromLatin1(','));
                if (p0 < 0 || p1 < 0) continue;
                cmd.targetName = line.mid(p0 + 1, p1 - p0 - 1).trimmed();
                if (m_targets.contains(cmd.targetName)) {
                    cmd.targets = m_targets[cmd.targetName];
                    for (double v : cmd.targets)
                        cmd.targetRaw.append(QString::number(v, 'f', 3));
                }
                if (cmd.targets.size() < 6) continue;
            }
            QRegularExpression re("(\\w+)=([\\d.]+)");
            auto it = re.globalMatch(line);
            while (it.hasNext()) {
                auto m = it.next();
                if (m.captured(1) == QStringLiteral("speed")) cmd.speed = m.captured(2).toDouble();
                else if (m.captured(1) == QStringLiteral("acc")) cmd.accel = m.captured(2).toDouble();
                else if (m.captured(1) == QStringLiteral("dec")) cmd.decel = m.captured(2).toDouble();
                else if (m.captured(1) == QStringLiteral("timeout")) cmd.timeoutMs = m.captured(2).toInt();
            }
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("MOVEL("), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::MoveL;
            int s = line.indexOf(QChar::fromLatin1('{'));
            if (s >= 0) {
                int e = line.indexOf(QChar::fromLatin1('}'));
                if (e < 0) continue;
                QString raw = line.mid(s+1, e-s-1);
                for (const auto& p : raw.split(QChar::fromLatin1(','), Qt::SkipEmptyParts)) {
                    QString t = p.trimmed();
                    int colon = t.indexOf(QChar::fromLatin1(':'));
                    QString val = t.mid(colon + 1).trimmed();
                    cmd.targetRaw.append(val);
                    cmd.targets.append(val.toDouble());
                }
                if (cmd.targets.size() < 6) continue;
            } else {
                int p0 = line.indexOf(QChar::fromLatin1('('));
                int p1 = line.indexOf(QChar::fromLatin1(','));
                if (p0 < 0 || p1 < 0) continue;
                cmd.targetName = line.mid(p0 + 1, p1 - p0 - 1).trimmed();
                if (m_targets.contains(cmd.targetName)) {
                    cmd.targets = m_targets[cmd.targetName];
                    for (double v : cmd.targets)
                        cmd.targetRaw.append(QString::number(v, 'f', 3));
                }
                if (cmd.targets.size() < 6) continue;
            }
            QRegularExpression re("(\\w+)=([\\d.]+)");
            auto it = re.globalMatch(line);
            while (it.hasNext()) {
                auto m = it.next();
                if (m.captured(1) == QStringLiteral("speed")) cmd.speed = m.captured(2).toDouble();
                else if (m.captured(1) == QStringLiteral("acc")) cmd.accel = m.captured(2).toDouble();
                else if (m.captured(1) == QStringLiteral("dec")) cmd.decel = m.captured(2).toDouble();
                else if (m.captured(1) == QStringLiteral("timeout")) cmd.timeoutMs = m.captured(2).toInt();
            }
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("WAIT_IDLE"), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::WaitIdle;
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("READ_POS"), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::ReadPos;
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("FOR("), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::ForLoop;
            int pe = line.indexOf(QChar::fromLatin1(')'));
            QString inner = line.mid(4, pe - 4).trimmed();
            QStringList parts = inner.split(QChar::fromLatin1(','));
            if (parts.size() >= 3) {
                // FOR(var, start, end)  默认步长=1
                cmd.varName   = parts[0].trimmed();
                cmd.loopStart = parts[1].trimmed().toDouble();
                cmd.loopEnd   = parts[2].trimmed().toDouble();
                cmd.loopStep  = 1.0;
                cmd.loopCount = -1;
            } else {
                // FOR(n) 旧语法
                cmd.loopCount = inner.toInt();
            }
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("END_FOR"), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::EndFor;
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("WHILE TRUE"), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::WhileTrue;
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("WHILE_DI("), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::WhileDI;
            int pe = line.indexOf(QChar::fromLatin1(')'));
            auto parts = line.mid(9, pe - 9).split(QChar::fromLatin1(','));
            if (parts.size() >= 2) { cmd.ioIndex = parts[0].trimmed().toInt(); cmd.ioValue = parts[1].trimmed().toInt(); }
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("END_WHILE"), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::EndWhile;
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("SPEED("), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::SpeedSet;
            int pe = line.indexOf(QChar::fromLatin1(')'));
            cmd.targets.append(line.mid(6, pe - 6).trimmed().toDouble());
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("ACCEL("), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::AccelSet;
            int pe = line.indexOf(QChar::fromLatin1(')'));
            cmd.targets.append(line.mid(6, pe - 6).trimmed().toDouble());
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("DECEL("), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::DecelSet;
            int pe = line.indexOf(QChar::fromLatin1(')'));
            cmd.targets.append(line.mid(6, pe - 6).trimmed().toDouble());
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("TIMEOUT("), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::TimeoutSet;
            int pe = line.indexOf(QChar::fromLatin1(')'));
            cmd.targets.append(line.mid(8, pe - 8).trimmed().toDouble());
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("MOVEJOFF("), Qt::CaseInsensitive)
                   || line.startsWith(QStringLiteral("MOVEOFF("), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::MoveOff;
            int s = line.indexOf(QChar::fromLatin1('('));
            int pe = line.indexOf(QChar::fromLatin1(')'));
            for (const auto& p : line.mid(s + 1, pe - s - 1).split(QChar::fromLatin1(','), Qt::SkipEmptyParts))
                cmd.targets.append(p.trimmed().toDouble());
            if (cmd.targets.size() >= 6) result.append(cmd);
        } else if (line.startsWith(QStringLiteral("MOVELOFF("), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::MoveLOff;
            int pe = line.indexOf(QChar::fromLatin1(')'));
            for (const auto& p : line.mid(9, pe - 9).split(QChar::fromLatin1(','), Qt::SkipEmptyParts))
                cmd.targets.append(p.trimmed().toDouble());
            if (cmd.targets.size() >= 6) result.append(cmd);
        } else if (line.startsWith(QStringLiteral("IF_DI("), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::IfDI;
            int pe = line.indexOf(QChar::fromLatin1(')'));
            auto parts = line.mid(6, pe - 6).split(QChar::fromLatin1(','));
            if (parts.size() >= 2) { cmd.ioIndex = parts[0].trimmed().toInt(); cmd.ioValue = parts[1].trimmed().toInt(); }
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("ELSE"), Qt::CaseInsensitive) && (line.length() == 4 || !line[4].isLetter())) {
            cmd.type = ScriptCmdType::Else;
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("END_IF"), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::EndIf;
            result.append(cmd);
        } else if (line.contains(QChar::fromLatin1('=')) && !line.startsWith(QChar::fromLatin1('#'))
                   && !line.startsWith(QStringLiteral("SERVO("), Qt::CaseInsensitive)
                   && !line.startsWith(QStringLiteral("MOVEJ("), Qt::CaseInsensitive)
                   && !line.startsWith(QStringLiteral("MOVEL("), Qt::CaseInsensitive)) {
            // 通用 SetVar 分支（变量赋值）。排除 SERVO/MOVEJ/MOVEL，因为它们的参数
            // 也含 '=' 号（多路 SERVO(0=10:30) 或关键字参数 speed=25, acc=50 等），
            // 必须先由对应专用分支解析。
            int eq = line.indexOf(QChar::fromLatin1('='));
            cmd.type = ScriptCmdType::SetVar;
            cmd.varName = line.left(eq).trimmed();
            QString rhs = line.mid(eq+1).trimmed();
            bool isNum;
            cmd.varValue = rhs.toDouble(&isNum);
            if (!isNum) cmd.varExpr = rhs;  // 表达式，如 "x + 10"
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("DELAY("), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::Delay;
            int pe = line.indexOf(QChar::fromLatin1(')'));
            cmd.delayMs = line.mid(6, pe - 6).trimmed().toInt();
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("SERVO("), Qt::CaseInsensitive)) {
            // 统一格式（2026-06-15 语法合并；2026-06-16 扩展时间模式）：
            //   推荐格式（时间模式）：SERVO(0=10:T2000)              —— ch=角度:T<ms>
            //   已废弃（速度模式）：SERVO(0=10:30)                    —— 仍可解析但报警告
            //   多路混合：SERVO(0=10:T2000, 1=20:T1500)              —— 每路独立时间
            // 不再支持 SERVO_S 前缀，也不支持 SERVO(ch, ang, spd) 旧逗号语法。
            int pe = line.indexOf(QChar::fromLatin1(')'));
            QString inner = line.mid(6, pe - 6).trimmed();
            if (inner.isEmpty()) continue;
            if (!inner.contains(QChar::fromLatin1('='))) {
                appendConsole(QStringLiteral("[警告] SERVO 语法已变更: 应为 SERVO(ch=ang:spd) 或 SERVO(ch=ang:T<ms>)，不再支持旧逗号语法。原文: %1").arg(line));
                continue;
            }
            cmd.type = ScriptCmdType::ServoMove;
            auto groups = inner.split(QChar::fromLatin1(','));
            for (const QString& g : groups) {
                auto chanPart = g.split(QChar::fromLatin1('='));
                if (chanPart.size() >= 2) {
                    int ch = chanPart[0].trimmed().toInt();
                    auto angSpd = chanPart[1].split(QChar::fromLatin1(':'));
                    float ang = angSpd.value(0, QString()).trimmed().toFloat();
                    QString paramStr = (angSpd.size() >= 2) ? angSpd[1].trimmed() : QString();
                    // 识别时间模式：T 前缀（大小写不敏感）
                    if (paramStr.startsWith(QLatin1Char('T'), Qt::CaseInsensitive)
                        || paramStr.startsWith(QLatin1Char('t'), Qt::CaseInsensitive)) {
                        QString msStr = paramStr.mid(1).trimmed();
                        bool msOk = false;
                        int ms = msStr.toInt(&msOk);
                        if (!msOk || ms < 0) {
                            appendConsole(QStringLiteral("[警告] SERVO 时间参数非法: '%1' (应为 T<ms>，例如 T2000)。原文: %2").arg(paramStr, line));
                            continue;
                        }
                        cmd.servoChannels.append(ch);
                        cmd.servoAngles.append(ang);
                        cmd.servoSpeeds.append(10.0f);   // 时间模式下速度字段无意义，给默认值
                        cmd.servoTimes.append(ms);       // >0 表示走时间分支
                    } else {
                        // 速度模式（已废弃，保留解析但报警告）
                        appendConsole(QStringLiteral("[警告] SERVO 速度模式已废弃: '%1' 应改写为 SERVO(ch=ang:T<ms>) 格式，例如 T2000。原文: %2")
                            .arg(paramStr, line));
                        float spd = paramStr.isEmpty() ? 10.0f : paramStr.toFloat();
                        cmd.servoChannels.append(ch);
                        cmd.servoAngles.append(ang);
                        cmd.servoSpeeds.append(spd);
                        cmd.servoTimes.append(0);         // 0 表示走速度分支
                    }
                }
            }
            // 兼容：把首通道填入旧单字段，避免其它地方读取 servoChannel/Angle/Speed 时为空
            if (!cmd.servoChannels.isEmpty()) {
                cmd.servoChannel = cmd.servoChannels.first();
                cmd.servoAngle   = cmd.servoAngles.first();
                cmd.servoSpeed   = cmd.servoSpeeds.first();
            }
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("SET_DO("), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::SetDO;
            int pe = line.indexOf(QChar::fromLatin1(')'));
            auto parts = line.mid(7, pe - 7).split(QChar::fromLatin1(','));
            if (parts.size() >= 2) { cmd.ioIndex = parts[0].trimmed().toInt(); cmd.ioValue = parts[1].trimmed().toInt(); }
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("WAIT_DI("), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::WaitDI;
            int pe = line.indexOf(QChar::fromLatin1(')'));
            auto parts = line.mid(8, pe - 8).split(QChar::fromLatin1(','));
            if (parts.size() >= 2) { cmd.ioIndex = parts[0].trimmed().toInt(); cmd.ioValue = parts[1].trimmed().toInt(); }
            if (parts.size() >= 3) cmd.ioTimeout = parts[2].trimmed().toInt();
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("PRINT(\""), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::Print;
            int q1 = line.indexOf(QChar::fromLatin1('"'));
            int q2 = line.lastIndexOf(QChar::fromLatin1('"'));
            if (q1 >= 0 && q2 > q1) cmd.printText = line.mid(q1+1, q2-q1-1);
            result.append(cmd);
        } else if (line.startsWith(QStringLiteral("CALL(\""), Qt::CaseInsensitive)) {
            cmd.type = ScriptCmdType::Call;
            int q1 = line.indexOf(QChar::fromLatin1('"'));
            int q2 = line.lastIndexOf(QChar::fromLatin1('"'));
            if (q1 >= 0 && q2 > q1) cmd.callFile = line.mid(q1+1, q2-q1-1);
            result.append(cmd);
        }
    }
    return result;
}

/**
 * @brief "停止/暂停"按钮回调：三态切换
 *
 *   - 正在执行 (isRunning) → 调用 pause() 暂停（当前动作完成后停止）
 *   - 已暂停 (isPaused) → 调用 stop() 彻底停止，按钮文字复位，状态栏提示
 *   - 空闲 → 不动作
 */
void UiController::onProgStop()
{
    if (!m_taskExecutor) return;

    if (m_taskExecutor->isRunning()) {
        // 正在执行 → 暂停
        m_taskExecutor->pause();
    } else if (m_taskExecutor->isPaused()) {
        // 已暂停 → 彻底停止（清空队列，机械臂保持当前位置不动）
        m_taskExecutor->stop();
        if (auto* btn = m_mainWindow->findChild<QPushButton*>(QStringLiteral("btnProgStop")))
            btn->setText(QStringLiteral("⏸ 暂停"));
        if (auto* sb = m_mainWindow->findChild<QStatusBar*>(QStringLiteral("statusbar")))
            sb->showMessage(QStringLiteral("任务已取消"));
        // 清空编辑器行高亮
        m_currentExecLine = -1;
        highlightCurrentLine(-1);
    }
    // 空闲：什么都不做
}

/**
 * @brief "保存脚本"按钮回调
 *
 * 弹出文件保存对话框，将：[点位区文本] + "\n---\n" + [程序区文本]
 * 写入所选文件，同时更新 m_currentScriptPath 与 UI 文件名标签。
 */
void UiController::onProgSave()
{
    auto* editor = getEditor();
    if (!editor) return;

    // 若已有保存路径则作为默认路径，否则使用 tasks/script.rbt
    QString defaultPath = m_currentScriptPath.isEmpty()
        ? QStringLiteral("tasks/script.rbt") : m_currentScriptPath;

    QString filePath = QFileDialog::getSaveFileName(
        nullptr,
        QStringLiteral("保存脚本"),
        defaultPath,
        QStringLiteral("R06 脚本 (*.rbt);;文本文件 (*.txt);;所有文件 (*)")
    );
    if (filePath.isEmpty()) return;
    saveScriptToFile(filePath);
}

/**
 * @brief 内部直接保存脚本到指定路径(不弹文件对话框)
 * @param filePath 完整文件路径
 * @return true 成功,false 失败
 *
 * 用于点位管理弹窗 OK 后自动保存场景:直接用当前 m_currentScriptPath 写入,
 * 避免用户每次都点保存按钮。如果 m_currentScriptPath 为空,使用 tasks/script.rbt。
 */
bool UiController::saveScriptToFile(const QString& filePath)
{
    auto* editor = getEditor();
    auto* targetEd = getTargetEditor();
    if (!editor) return false;

    QString realPath = filePath;
    if (realPath.isEmpty()) {
        realPath = m_currentScriptPath.isEmpty()
            ? QStringLiteral("tasks/script.rbt") : m_currentScriptPath;
    }

    QFile file(realPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream stream(&file);
    // 写入：点位区 + 分隔 + 程序区
    if (targetEd)
        stream << targetEd->toPlainText() << "\n---\n";
    stream << editor->toPlainText();
    file.close();
    m_currentScriptPath = realPath;

    if (auto* lbl = m_mainWindow->findChild<QLabel*>(QStringLiteral("progFileName")))
        lbl->setText(QStringLiteral("📄 %1").arg(QFileInfo(realPath).fileName()));
    if (auto* sb = m_mainWindow->findChild<QStatusBar*>(QStringLiteral("statusbar")))
        sb->showMessage(QStringLiteral("已自动保存: %1").arg(realPath));
    return true;
}

/**
 * @brief "加载脚本"按钮回调
 *
 * 1. 弹出文件对话框选择 .rbt 文件
 * 2. 读入内容，按 "\n---\n" 分隔为"点位区"和"程序区"，并分别写入两个编辑器
 * 3. 若不存在分隔符，则整段内容写入程序区，点位区清空
 * 4. 重新解析文件以填充 m_targets（点位缓存）
 * 5. 更新 m_currentScriptPath 与 UI 文件名标签
 */
void UiController::onProgLoad()
{
    auto* editor = getEditor();
    if (!editor) return;

    QString filePath = QFileDialog::getOpenFileName(
        nullptr,
        QStringLiteral("加载脚本"),
        QStringLiteral("tasks"),
        QStringLiteral("R06 脚本 (*.rbt);;文本文件 (*.txt);;所有文件 (*)")
    );
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString content = stream.readAll();
        file.close();
        m_currentScriptPath = filePath;

        // 按 "\n---\n" 分隔点位和程序
        auto* targetEd = getTargetEditor();
        int sep = content.indexOf(QStringLiteral("\n---\n"));
        if (sep >= 0 && targetEd) {
            targetEd->setPlainText(content.left(sep).trimmed());
            editor->setPlainText(content.mid(sep + 5).trimmed());
        } else {
            if (targetEd) targetEd->clear();
            editor->setPlainText(content.trimmed());
        }

        // 解析已有点位，恢复 m_targets 与 m_targetCounter
        m_targets.clear();
        m_targetMotion.clear();
        m_targetCounter = 0;
        parseScript(content);

        // 把点位运动参数同步给 TaskExecutor,供运行时优先级链使用
        if (m_taskExecutor)
            m_taskExecutor->setTargetMotion(m_targetMotion);

        if (auto* lbl = m_mainWindow->findChild<QLabel*>(QStringLiteral("progFileName")))
            lbl->setText(QStringLiteral("📄 %1").arg(QFileInfo(filePath).fileName()));
        if (auto* sb = m_mainWindow->findChild<QStatusBar*>(QStringLiteral("statusbar")))
            sb->showMessage(QStringLiteral("已加载: %1").arg(filePath));
    }
}

/**
 * @brief "新建脚本"按钮回调
 *
 * 若当前程序区非空，先弹窗询问是否丢弃；确认后：
 *   - 清空两个编辑器
 *   - 清空 m_currentScriptPath / m_targets / m_targetCounter
 *   - 文件名标签显示 "未命名.rbt"
 */
void UiController::onProgNew()
{
    auto* editor = getEditor();
    if (!editor) return;
    if (!editor->toPlainText().trimmed().isEmpty()) {
        auto btn = QMessageBox::question(nullptr, QStringLiteral("新建脚本"),
            QStringLiteral("当前内容未保存，确定清空？"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (btn != QMessageBox::Yes) return;
    }
    editor->clear();
    if (auto* targetEd = getTargetEditor()) targetEd->clear();
    m_currentScriptPath.clear();
    m_targets.clear();
    m_targetMotion.clear();
    m_targetCounter = 0;
    if (m_taskExecutor)
        m_taskExecutor->setTargetMotion(m_targetMotion);
    if (auto* lbl = m_mainWindow->findChild<QLabel*>(QStringLiteral("progFileName")))
        lbl->setText(QStringLiteral("📄 未命名.rbt"));
}

/**
 * @brief "删除当前行"按钮回调
 *
 * 删除编辑器光标所在行的全部文本，并清除残留换行。
 */
void UiController::onProgDeleteLine()
{
    auto* editor = getEditor();
    if (!editor) return;
    QTextCursor cursor = editor->textCursor();
    cursor.select(QTextCursor::LineUnderCursor);
    cursor.removeSelectedText();
    cursor.deleteChar(); // 删除残留换行
    editor->setTextCursor(cursor);
}

// ==================== 编程-指令插入按钮 ====================

/**
 * @brief 根据两点间距离与速度估算超时时长（帮助函数）
 *
 * 计算方法：在各轴差值中取最大那一个，按 maxDist / speed * 2000 估算；
 * 若输入无效或结果小于 3000ms，则使用 3000ms 作为最低值。
 *
 * @param from  起点坐标（关节或笛卡尔皆可）
 * @param to    终点坐标
 * @param speed 运动速度
 * @return 超时时长（毫秒）
 */
static int calcTimeout(const QList<double>& from, const QList<double>& to, double speed)
{
    if (from.size() < 3 || to.size() < 3 || speed <= 0.0) return 3000;
    double maxDist = 0.0;
    for (int i = 0; i < qMin(from.size(), to.size()); ++i)
        maxDist = qMax(maxDist, qAbs(to[i] - from[i]));
    // 距离远 / 速度低 → 加长超时；否则最少 3s
    return qMax(3000, static_cast<int>(maxDist / speed * 2000.0));
}

/**
 * @brief 生成下一个未被使用的点位名（帮助函数）
 *
 * 自增传入的 counter，直到找到一个在 targets 哈希中未被使用的
 * "TargetN" 名字后返回（保证不覆盖已有 TARGET）。
 */
static QString nextTargetName(int& counter, QHash<QString, QList<double>>& targets)
{
    QString name;
    do {
        counter++;
        name = QStringLiteral("Target%1").arg(counter);
    } while (targets.contains(name));
    return name;
}

/**
 * @brief "插入 MOVEJ"按钮回调：记录当前关节坐标为一个 TARGET，并插入 MOVEJ 指令
 *
 * 步骤：
 *   1. 从 RobotState 读取当前 6 轴关节角度
 *   2. 生成唯一 TARGET 名（如 Target1），写入点位编辑器
 *   3. 向程序编辑器插入 "MOVEJ(Target1, speed=, acc=, dec=)"
 *   4. 更新 m_targets 缓存并触发点位顺序同步
 */
void UiController::onProgInsertMoveJ()
{
    if (!m_robotController || !m_robotController->getState()) return;
    QList<double> joints = m_robotController->getState()->getAllAxisPositions();
    double spd, acc, dec; int timeout;
    readDefaultMotionParams(spd, acc, dec, timeout);

    QString name = nextTargetName(m_targetCounter, m_targets);

    auto* targetEd = getTargetEditor();
    QString target = QStringLiteral("TARGET %1 = {J0: %2, J1: %3, J2: %4, J3: %5, J4: %6, J5: %7}")
        .arg(name)
        .arg(joints.value(0,0), 0, 'f', 3).arg(joints.value(1,0), 0, 'f', 3).arg(joints.value(2,0), 0, 'f', 3)
        .arg(joints.value(3,0), 0, 'f', 3).arg(joints.value(4,0), 0, 'f', 3).arg(joints.value(5,0), 0, 'f', 3);
    insertAtCursor(targetEd, target);

    // MOVEJ 写入程序编辑器（timeout 内部计算，不显示在脚本中）
    QString cmd = QStringLiteral("MOVEJ(%1, speed=%2, acc=%3, dec=%4)")
        .arg(name).arg(spd, 0, 'f', 0).arg(acc, 0, 'f', 0).arg(dec, 0, 'f', 0);
    insertAtCursor(cmd);

    QList<double> vals;
    for (double v : joints) vals.append(v);
    m_targets.insert(name, vals);

    syncTargets();
}

/**
 * @brief "插入 MOVEL"按钮回调：记录当前笛卡尔坐标为一个 TARGET，并插入 MOVEL 指令
 *
 * 与 onProgInsertMoveJ 类似，区别是：
 *   - 从 RobotState 读取的是 getAllCartesianPositions()（笛卡尔 X/Y/Z/A/B/C）
 *   - 点位写入格式为 "TARGET name = {X:, Y:, Z:, A:, B:, C:}"
 *   - 指令为 MOVEL(...)（直线插补）
 */
void UiController::onProgInsertMoveL()
{
    if (!m_robotController || !m_robotController->getState()) return;
    QList<double> cart = m_robotController->getState()->getAllCartesianPositions();
    double spd, acc, dec; int timeout;
    readDefaultMotionParams(spd, acc, dec, timeout);

    QString name = nextTargetName(m_targetCounter, m_targets);

    QString target = QStringLiteral("TARGET %1 = {X: %2, Y: %3, Z: %4, A: %5, B: %6, C: %7}")
        .arg(name)
        .arg(cart.value(0,0), 0, 'f', 3).arg(cart.value(1,0), 0, 'f', 3).arg(cart.value(2,0), 0, 'f', 3)
        .arg(cart.value(3,0), 0, 'f', 3).arg(cart.value(4,0), 0, 'f', 3).arg(cart.value(5,0), 0, 'f', 3);
    insertAtCursor(getTargetEditor(), target);

    QString cmd = QStringLiteral("MOVEL(%1, speed=%2, acc=%3, dec=%4)")
        .arg(name).arg(spd, 0, 'f', 0).arg(acc, 0, 'f', 0).arg(dec, 0, 'f', 0);
    insertAtCursor(cmd);

    QList<double> vals;
    for (double v : cart) vals.append(v);
    m_targets.insert(name, vals);

    syncTargets();
}

/**
 * @brief "插入 WAIT_IDLE"按钮回调：等待所有轴停止运动
 */
void UiController::onProgInsertWaitIdle()
{
    insertAtCursor(QStringLiteral("WAIT_IDLE()"));
}

/**
 * @brief "插入 DELAY"按钮回调：弹窗输入毫秒数，插入 DELAY(ms)
 */
void UiController::onProgInsertDelay()
{
    bool ok = false;
    int ms = QInputDialog::getInt(nullptr, QStringLiteral("延时"),
        QStringLiteral("等待时间 (毫秒):"), 500, 0, 60000, 100, &ok);
    if (ok)
        insertAtCursor(QStringLiteral("DELAY(%1)").arg(ms));
}

/**
 * @brief "插入 SET_DO"按钮回调：弹窗输入 DO 编号与期望电平，插入 SET_DO(idx, val)
 */
void UiController::onProgInsertSetDO()
{
    bool ok = false;
    int idx = QInputDialog::getInt(nullptr, QStringLiteral("SET_DO"),
        QStringLiteral("DO 编号 (0-19):"), 0, 0, 19, 1, &ok);
    if (!ok) return;
    int val = QInputDialog::getInt(nullptr, QStringLiteral("SET_DO"),
        QStringLiteral("值 (0/1):"), 1, 0, 1, 1, &ok);
    if (ok)
        insertAtCursor(QStringLiteral("SET_DO(%1, %2)").arg(idx).arg(val));
}

/**
 * @brief "插入 WAIT_DI"按钮回调：等待指定 DI 变为期望电平（带超时）
 *
 * 三次弹窗依次输入：DI 编号 / 期望值 / 超时毫秒数。
 */
void UiController::onProgInsertWaitDI()
{
    bool ok = false;
    int idx = QInputDialog::getInt(nullptr, QStringLiteral("WAIT_DI"),
        QStringLiteral("DI 编号 (0-23):"), 0, 0, 23, 1, &ok);
    if (!ok) return;
    int exp = QInputDialog::getInt(nullptr, QStringLiteral("WAIT_DI"),
        QStringLiteral("期望值 (0=低电平触发, 1=高电平):"), 0, 0, 1, 1, &ok);
    if (!ok) return;
    int timeout = QInputDialog::getInt(nullptr, QStringLiteral("WAIT_DI"),
        QStringLiteral("超时 (毫秒):"), 5000, 0, 60000, 100, &ok);
    if (ok)
        insertAtCursor(QStringLiteral("WAIT_DI(%1, %2, %3)").arg(idx).arg(exp).arg(timeout));
}

/**
 * @brief "插入 PRINT"按钮回调：弹窗输入文本，插入 PRINT("text")
 */
void UiController::onProgInsertPrint()
{
    bool ok = false;
    QString text = QInputDialog::getText(nullptr, QStringLiteral("PRINT"),
        QStringLiteral("打印文本:"), QLineEdit::Normal, QString(), &ok);
    if (ok && !text.isEmpty())
        insertAtCursor(QStringLiteral("PRINT(\"%1\")").arg(text));
}

/**
 * @brief "插入注释"按钮回调：插入 "# "前缀，光标跳至行尾等待用户书写
 */
void UiController::onProgInsertComment()
{
    insertAtCursor(QStringLiteral("# "));
    auto* editor = getEditor();
    if (editor) editor->moveCursor(QTextCursor::EndOfLine);
}

/**
 * @brief "插入变量赋值"按钮回调：弹窗输入变量名与值，插入 "name = val"
 */
void UiController::onProgInsertSet()
{
    bool ok = false;
    QString name = QInputDialog::getText(nullptr, QStringLiteral("变量赋值"),
        QStringLiteral("变量名:"), QLineEdit::Normal, QString(), &ok);
    if (!ok || name.isEmpty()) return;
    double val = QInputDialog::getDouble(nullptr, QStringLiteral("变量赋值"),
        QStringLiteral("值:"), 0, -99999, 99999, 3, &ok);
    if (ok)
        insertAtCursor(QStringLiteral("%1 = %2").arg(name.trimmed()).arg(val, 0, 'f', 3));
}

/**
 * @brief "插入 CALL"按钮回调：弹窗选择子脚本文件，插入 CALL("file.rbt")
 */
void UiController::onProgInsertCall()
{
    QString filePath = QFileDialog::getOpenFileName(
        nullptr,
        QStringLiteral("选择子脚本"),
        QString(),
        QStringLiteral("R06 脚本 (*.rbt);;所有文件 (*)")
    );
    if (!filePath.isEmpty())
        insertAtCursor(QStringLiteral("CALL(\"%1\")").arg(QFileInfo(filePath).fileName()));
}

/**
 * @brief "读取位置"按钮回调：插入 READ_POS() 指令，同时立即打印当前关节/笛卡尔坐标到控制台
 */
void UiController::onProgReadPosition()
{
    insertAtCursor(QStringLiteral("READ_POS()"));

    if (!m_robotController || !m_robotController->getState()) {
        appendConsole(QStringLiteral("[WARN] 控制器未连接"));
        return;
    }

    RobotState* state = m_robotController->getState();
    QList<double> joints = state->getAllAxisPositions();
    QList<double> carts  = state->getAllCartesianPositions();

    appendConsole(QStringLiteral("--- 关节 (°) ---"));
    appendConsole(QStringLiteral("J0=%1 J1=%2 J2=%3 J3=%4 J4=%5 J5=%6")
        .arg(joints.value(0,0), 0, 'f', 3).arg(joints.value(1,0), 0, 'f', 3).arg(joints.value(2,0), 0, 'f', 3)
        .arg(joints.value(3,0), 0, 'f', 3).arg(joints.value(4,0), 0, 'f', 3).arg(joints.value(5,0), 0, 'f', 3));
    appendConsole(QStringLiteral("--- 笛卡尔 (mm/°) ---"));
    appendConsole(QStringLiteral("X=%1 Y=%2 Z=%3 RX=%4 RY=%5 RZ=%6")
        .arg(carts.value(0,0), 0, 'f', 3).arg(carts.value(1,0), 0, 'f', 3).arg(carts.value(2,0), 0, 'f', 3)
        .arg(carts.value(3,0), 0, 'f', 3).arg(carts.value(4,0), 0, 'f', 3).arg(carts.value(5,0), 0, 'f', 3));
}

/**
 * @brief "插入 FOR 循环"按钮回调：弹窗输入变量名 / 起始 / 结束，插入 FOR(...) 与 END_FOR
 */
void UiController::onProgInsertFor()
{
    bool ok = false;
    QString name = QInputDialog::getText(nullptr, QStringLiteral("FOR 循环"),
        QStringLiteral("循环变量名 (如 i、row、x):"), QLineEdit::Normal, QStringLiteral("i"), &ok);
    if (!ok || name.isEmpty()) return;

    double start = QInputDialog::getDouble(nullptr, QStringLiteral("FOR 循环"),
        QStringLiteral("起始值:"), 0, -99999, 99999, 2, &ok);
    if (!ok) return;

    double end = QInputDialog::getDouble(nullptr, QStringLiteral("FOR 循环"),
        QStringLiteral("结束值:"), 10, -99999, 99999, 2, &ok);
    if (!ok) return;

    insertAtCursor(QStringLiteral("FOR(%1, %2, %3)")
        .arg(name.trimmed()).arg(start).arg(end));
    insertAtCursor(QStringLiteral("END_FOR"));
}

/**
 * @brief "插入关节增量运动"按钮回调：弹窗输入 6 轴偏移量（逗号分隔），插入 MOVEJOFF(dJ0,dJ1,...)
 */
void UiController::onProgInsertMoveOff()
{
    QList<double> joints = m_robotController && m_robotController->getState()
        ? m_robotController->getState()->getAllAxisPositions() : QList<double>();
    bool ok = false;
    QString input = QInputDialog::getText(nullptr, QStringLiteral("MOVE_OFF"),
        QStringLiteral("6轴偏移量 (逗号分隔, 如 10,0,0,0,0,0):"),
        QLineEdit::Normal, QStringLiteral("10,0,0,0,0,0"), &ok);
    if (!ok || input.isEmpty()) return;
    insertAtCursor(QStringLiteral("MOVEJOFF(%1)").arg(input.trimmed()));
}

/**
 * @brief "插入笛卡尔增量运动"按钮回调：弹窗输入 6 轴偏移量（逗号分隔），插入 MOVELOFF(dX,dY,...)
 */
void UiController::onProgInsertMoveLOff()
{
    QList<double> cart = m_robotController && m_robotController->getState()
        ? m_robotController->getState()->getAllCartesianPositions() : QList<double>();
    bool ok = false;
    QString input = QInputDialog::getText(nullptr, QStringLiteral("MOVEL_OFF"),
        QStringLiteral("6轴偏移量 (逗号分隔, 如 0,50,0,0,0,0):"),
        QLineEdit::Normal, QStringLiteral("0,50,0,0,0,0"), &ok);
    if (!ok || input.isEmpty()) return;
    insertAtCursor(QStringLiteral("MOVELOFF(%1)").arg(input.trimmed()));
}

/**
 * @brief "插入 SPEED"按钮回调：弹窗输入全局速度（mm/s），插入 SPEED(value)
 */
void UiController::onProgInsertSpeed()
{
    bool ok = false;
    double v = QInputDialog::getDouble(nullptr, QStringLiteral("SPEED"),
        QStringLiteral("全局默认速度 (mm/s):"), 250, 1, 999, 0, &ok);
    if (ok) insertAtCursor(QStringLiteral("SPEED(%1)").arg(v, 0, 'f', 0));
}

/**
 * @brief "插入 ACCEL"按钮回调：弹窗输入全局加速度（mm/s²），插入 ACCEL(value)
 */
void UiController::onProgInsertAccel()
{
    bool ok = false;
    double v = QInputDialog::getDouble(nullptr, QStringLiteral("ACCEL"),
        QStringLiteral("全局默认加速度 (mm/s²):"), 500, 1, 9999, 0, &ok);
    if (ok) insertAtCursor(QStringLiteral("ACCEL(%1)").arg(v, 0, 'f', 0));
}

/**
 * @brief "插入 DECEL"按钮回调：弹窗输入全局减速度（mm/s²），插入 DECEL(value)
 */
void UiController::onProgInsertDecel()
{
    bool ok = false;
    double v = QInputDialog::getDouble(nullptr, QStringLiteral("DECEL"),
        QStringLiteral("全局默认减速度 (mm/s²):"), 600, 1, 9999, 0, &ok);
    if (ok) insertAtCursor(QStringLiteral("DECEL(%1)").arg(v, 0, 'f', 0));
}

/**
 * @brief "插入 TIMEOUT"按钮回调：弹窗输入全局超时（毫秒），插入 TIMEOUT(value)
 */
void UiController::onProgInsertTimeout()
{
    bool ok = false;
    int v = QInputDialog::getInt(nullptr, QStringLiteral("TIMEOUT"),
        QStringLiteral("全局默认超时 (ms):"), 10000, 100, 60000, 100, &ok);
    if (ok) insertAtCursor(QStringLiteral("TIMEOUT(%1)").arg(v));
}

/**
 * @brief "插入 IF_DI 条件"按钮回调：弹窗输入 DI 编号与期望值，插入 IF_DI(idx, val) 与 END_IF 块
 */
void UiController::onProgInsertIfDI()
{
    bool ok = false;
    int idx = QInputDialog::getInt(nullptr, QStringLiteral("IF_DI"),
        QStringLiteral("DI 编号 (0-23):"), 0, 0, 23, 1, &ok);
    if (!ok) return;
    int val = QInputDialog::getInt(nullptr, QStringLiteral("IF_DI"),
        QStringLiteral("期望值 (0/1):"), 1, 0, 1, 1, &ok);
    if (ok) {
        insertAtCursor(QStringLiteral("IF_DI(%1, %2)").arg(idx).arg(val));
        insertAtCursor(QStringLiteral("END_IF"));
    }
}

/**
 * @brief "插入 ELSE"按钮回调：插入 ELSE 关键字（配合 IF_DI / END_IF 使用）
 */
void UiController::onProgInsertElse()
{
    insertAtCursor(QStringLiteral("ELSE"));
}

/**
 * @brief "插入 END_IF"按钮回调：插入 END_IF 关键字（配合 IF_DI 使用）
 */
void UiController::onProgInsertEndIf()
{
    insertAtCursor(QStringLiteral("END_IF"));
}

/**
 * @brief "插入 SERVO"按钮回调：弹窗输入通道、角度、控制模式（速度/时间），插入 SERVO(ch=ang:spd|Tms)
 *
 * 2026-06-15 语法统一：原 SERVO(ch, ang, spd) 逗号写法已废弃，改为统一格式 SERVO(ch=ang:spd)。
 * 2026-06-16 扩展：增加时间模式 SERVO(ch=ang:T<ms>)，与 KPZ32 出厂协议 `T<ms>` 字段语义一致。
 * 2026-06-16 推广：默认使用"时间模式"作为推荐格式（速度模式已废弃，保留为备选）。
 * 多路场景可写 SERVO(0=..., 1=..., 2=...)，每路可独立选择速度或时间。
 */
void UiController::onProgInsertServo()
{
    bool ok = false;
    int ch = QInputDialog::getInt(nullptr, QStringLiteral("SERVO - 舵机控制"),
        QStringLiteral("通道号 (0-5):"), 0, 0, 5, 1, &ok);
    if (!ok) return;

    int angle = QInputDialog::getInt(nullptr, QStringLiteral("SERVO - 舵机控制"),
        QStringLiteral("目标角度 (0-180°):"), 90, 0, 180, 1, &ok);
    if (!ok) return;

    // 控制模式选择：默认时间模式（推荐格式），速度模式作为备选
    QStringList modeItems = {
        QStringLiteral("时间模式 (ms，推荐，仿 KPZ32)"),
        QStringLiteral("速度模式 (°/s，已废弃)")
    };
    QString mode = QInputDialog::getItem(nullptr, QStringLiteral("SERVO - 舵机控制"),
        QStringLiteral("控制模式:"), modeItems, 0, false, &ok);
    if (!ok) return;

    if (mode == modeItems.value(1)) {
        // 速度模式（已废弃，保留解析但报警告）
        int speed = QInputDialog::getInt(nullptr, QStringLiteral("SERVO - 舵机控制"),
            QStringLiteral("转动速度 (°/s, 1-180):"), 10, 1, 180, 1, &ok);
        if (!ok) return;
        insertAtCursor(QStringLiteral("SERVO(%1=%2:%3)").arg(ch).arg(angle).arg(speed));
    } else {
        // 时间模式（推荐格式，仿 KPZ32 出厂协议 #xxxPxxxT<ms>!）
        int timeMs = QInputDialog::getInt(nullptr, QStringLiteral("SERVO - 舵机控制"),
            QStringLiteral("转动时间 (ms, 0-30000；0 或 <20 视为瞬时):"), 2000, 0, 30000, 100, &ok);
        if (!ok) return;
        insertAtCursor(QStringLiteral("SERVO(%1=%2:T%3)").arg(ch).arg(angle).arg(timeMs));
    }
}

// ==================== 控制台输出 ====================

/**
 * @brief 控制台输出转发：将 text 写入程序编辑器下方的控制台窗口
 */
void UiController::onConsoleOutput(const QString& text)
{
    appendConsole(text);
}

// ==================== 设置 ====================

/**
 * @brief "加载设置"按钮回调：从 RobotConfig 读取参数（速度/加速度/超时/IP）并写回界面控件
 */
void UiController::onLoadSettings()
{
    RobotConfig* config = m_robotController->getConfig();
    if (!config) return;

    // 回零原点位置
    QList<double> homePos = config->getHomePosition();
    for (int j = 0; j < homePos.size() && j < 6; ++j) {
        if (auto* edit = m_mainWindow->findChild<QLineEdit*>(QStringLiteral("editHomeJ%1").arg(j)))
            edit->setText(QString::number(homePos[j], 'f', 3));
    }
}

/**
 * @brief "保存设置"按钮回调：采集回零原点位置，写回 JSON 配置文件
 */
void UiController::onSaveSettings()
{
    RobotConfig* config = m_robotController->getConfig();
    if (!config) return;

    // 回零原点位置
    QList<double> homePos;
    for (int j = 0; j < 6; ++j) {
        auto* edit = m_mainWindow->findChild<QLineEdit*>(QStringLiteral("editHomeJ%1").arg(j));
        homePos.append(edit ? edit->text().toDouble() : 0.0);
    }
    config->setHomePosition(homePos);

    QString configPath = QCoreApplication::applicationDirPath() + "/config/robot_config.json";
    auto* statusbar = m_mainWindow->findChild<QStatusBar*>(QStringLiteral("statusbar"));
    if (config->saveToJson(configPath)) {
        if (statusbar) statusbar->showMessage(QStringLiteral("配置已保存"));
    } else {
        if (statusbar) statusbar->showMessage(QStringLiteral("配置保存失败"));
    }
}

// ==================== 手动点动 ====================

/**
 * @brief 关节点动回调：按指定轴的正负方向移动一个步长（调试模式下才生效）
 *
 * 流程：读取调试面板的步长/速度/加速度 → 计算目标角度 → 调用 jogJoint() 移动 → 刷新界面和状态
 *
 * @param axis     关节轴编号（0=J0, 1=J1, ...）
 * @param positive true=正向（+），false=负向（-）
 */
void UiController::onJogJoint(int axis, bool positive)
{
    if (!m_robotController || !m_robotController->isConnected()) return;

    // 检查调试模式是否已开启
    auto* chkDebug = m_mainWindow->findChild<QCheckBox*>(QStringLiteral("chkDebugMode"));
    if (!chkDebug || !chkDebug->isChecked()) return;

    RobotState* state = m_robotController->getState();
    if (!state) return;

    // 从调试面板读取步长和运动参数
    double step   = 5.0;
    double speed  = 25.0, accel = 50.0, decel = 60.0;
    if (auto* sp = m_mainWindow->findChild<QLineEdit*>(QStringLiteral("debugStep")))  step  = sp->text().toDouble();
    if (auto* sp = m_mainWindow->findChild<QLineEdit*>(QStringLiteral("debugSpeed"))) speed = sp->text().toDouble();
    if (auto* sp = m_mainWindow->findChild<QLineEdit*>(QStringLiteral("debugAccel"))) accel = sp->text().toDouble();
    if (auto* sp = m_mainWindow->findChild<QLineEdit*>(QStringLiteral("debugDecel"))) decel = sp->text().toDouble();

    QList<double> joints = state->getAllAxisPositions();
    if (axis >= joints.size()) return;

    joints[axis] += (positive ? step : -step);

    // 更新界面上的关节输入框
    QString spinName = QStringLiteral("spinManualJ%1").arg(axis);
    if (auto* spin = m_mainWindow->findChild<QLineEdit*>(spinName))
        spin->setText(QString::number(joints[axis], 'f', 3));

    // Cancel → Move，不等待
    m_robotController->jogJoint(axis, joints[axis], speed, accel, decel);
    m_robotController->getMotionController()->updateState();
    emit jogCompleted();

    // 更新状态栏
    if (auto* sb = m_mainWindow->findChild<QStatusBar*>(QStringLiteral("statusbar")))
        sb->showMessage(QStringLiteral("关节 J%1 %2%3°").arg(axis)
            .arg(positive ? QStringLiteral("+") : QStringLiteral("-")).arg(step));
}

/**
 * @brief 笛卡尔点动回调：按指定轴的正负方向移动一个步长（调试模式下才生效）
 *
 * 与 onJogJoint 类似，区别：
 *   - 作用于笛卡尔坐标（index=0→X, 1→Y, 2→Z, 3→Rx, 4→Ry, 5→Rz）
 *   - 角度轴（index>=3）会调用 normalizeAngle180() 归一化到 [-180, 180]
 *
 * @param index    笛卡尔轴索引（0=X, 1=Y, 2=Z, 3=Rx, 4=Ry, 5=Rz）
 * @param positive true=正向（+），false=负向（-）
 */
void UiController::onJogCart(int index, bool positive)
{
    if (!m_robotController || !m_robotController->isConnected()) return;

    // 检查调试模式是否已开启
    auto* chkDebug = m_mainWindow->findChild<QCheckBox*>(QStringLiteral("chkDebugMode"));
    if (!chkDebug || !chkDebug->isChecked()) return;

    RobotState* state = m_robotController->getState();
    if (!state) return;

    // 从调试面板读取步长和运动参数
    double step   = 5.0;
    double speed  = 25.0, accel = 50.0, decel = 60.0;
    if (auto* sp = m_mainWindow->findChild<QLineEdit*>(QStringLiteral("debugStep")))  step  = sp->text().toDouble();
    if (auto* sp = m_mainWindow->findChild<QLineEdit*>(QStringLiteral("debugSpeed"))) speed = sp->text().toDouble();
    if (auto* sp = m_mainWindow->findChild<QLineEdit*>(QStringLiteral("debugAccel"))) accel = sp->text().toDouble();
    if (auto* sp = m_mainWindow->findChild<QLineEdit*>(QStringLiteral("debugDecel"))) decel = sp->text().toDouble();

    QList<double> cart = state->getAllCartesianPositions();
    if (index >= cart.size()) return;

    cart[index] += (positive ? step : -step);

    // 更新界面上的笛卡尔输入框
    static const char* cartNames[] = {"X","Y","Z","Rx","Ry","Rz"};
    QString spinName = QStringLiteral("spinManual%1").arg(QLatin1String(cartNames[index]));
    if (auto* spin = m_mainWindow->findChild<QLineEdit*>(spinName)) {
        double display = (index >= 3) ? normalizeAngle180(cart[index]) : cart[index];
        spin->setText(QString::number(display, 'f', 3));
    }

    // 传入归一化后的目标值
    QList<double> target;
    for (int i = 0; i < cart.size(); ++i) {
        if (i >= 3)
            target.append(normalizeAngle180(cart[i]));
        else
            target.append(cart[i]);
    }

    // Cancel → Move，不等待
    m_robotController->jogCart(target, speed, accel, decel);
    m_robotController->getMotionController()->updateState();
    emit jogCompleted();

    // 更新状态栏
    QString unit = (index < 3) ? QStringLiteral("mm") : QStringLiteral("°");
    if (auto* sb = m_mainWindow->findChild<QStatusBar*>(QStringLiteral("statusbar")))
        sb->showMessage(QStringLiteral("笛卡尔 %1 %2%3%4").arg(QLatin1String(cartNames[index]))
            .arg(positive ? QStringLiteral("+") : QStringLiteral("-")).arg(step).arg(unit));
}

/**
 * @brief 调试模式开关：勾选后启用手动点动控件和调试参数输入框；未勾选时全部禁用
 *
 * 通过 findChild / findChildren + 名称前缀匹配来批量开关两类控件：
 *   - debugSpeed / debugAccel / debugDecel / debugStep（调试参数）
 *   - spinManualJ* / spinManual* / btnJogJ* / btnJog* / btnApplyJoint / btnApplyCart（点动控件）
 */
void UiController::onDebugModeToggled(bool checked)
{
    // 调试参数输入框
    static const char* debugWidgets[] = {"debugSpeed", "debugAccel", "debugDecel", "debugStep", nullptr};
    for (int i = 0; debugWidgets[i]; ++i) {
        if (auto* w = m_mainWindow->findChild<QWidget*>(QLatin1String(debugWidgets[i])))
            w->setEnabled(checked);
    }

    // 关节/笛卡尔输入框和±按钮
    static const char* jogPrefixes[] = {"spinManualJ", "spinManual", "btnJogJ", "btnJog", "btnApplyJoint", "btnApplyCart", nullptr};
    for (int p = 0; jogPrefixes[p]; ++p) {
        QList<QWidget*> widgets = m_mainWindow->findChildren<QWidget*>(QRegularExpression(
            QStringLiteral("^%1.*").arg(QLatin1String(jogPrefixes[p]))));
        for (auto* w : std::as_const(widgets))
            w->setEnabled(checked);
    }
}

/**
 * @brief 通用按钮绑定(支持单击模式和按住连续模式)
 *
 * 两种工作模式由 intervalMs 决定：
 *   - **intervalMs > 0** = 按住连续模式：
 *       - 鼠标按下 (pressed) 立即触发一次 onTick，再启动 timer 周期重复 onTick
 *       - 鼠标松开 (released) 停止 timer，触发 onStop(默认空)
 *   - **intervalMs <= 0** = 单击模式：
 *       - 鼠标按下 (pressed) 只触发一次 onTick，不创建 timer
 *       - 鼠标松开 (released) 触发 onStop(若为空则不做任何事，让机械臂自然完成移动)
 *
 * 注：Qt 按钮 `setAutoRepeat(false)` 已设置，避免 Qt 自带连发机制干扰
 *
 * 典型用途：
 *   - 单击模式(intervalMs=0)：手动点动按钮（J0-/J0+/X-/X+），点一下走一步
 *   - 按住连续模式(intervalMs=50)：其他需要按住持续触发的场景
 *
 * @param btn        目标按钮
 * @param intervalMs 重复间隔（毫秒，<=0 表示单击模式）
 * @param onTick     每次触发的逻辑
 * @param onStop     松开时的逻辑（可选；单击模式下通常传空 lambda）
 */
void UiController::bindHoldButton(QPushButton* btn, int intervalMs,
                                  std::function<void()> onTick,
                                  std::function<void()> onStop)
{
    if (!btn) return;
    btn->setAutoRepeat(false);  // 关闭 Qt 自带 autoRepeat,避免快速连击

    if (intervalMs <= 0) {
        // === 单击模式:按下触发一次,松开按 onStop 处理(通常为空)===
        connect(btn, &QPushButton::pressed, this, [onTick]{
            onTick();  // 触发一次 jogJoint/jogCart,机械臂自然完成 MoveAbs
        });
        if (onStop) {
            connect(btn, &QPushButton::released, this, [onStop]{
                onStop();  // 通常是空 lambda,让机械臂自然停
            });
        }
        return;
    }

    // === 按住连续模式(原行为)===
    auto* timer = new QTimer(this);
    timer->setInterval(intervalMs);

    // 防止定时器在按钮析构后还在跑
    connect(btn, &QObject::destroyed, timer, [timer]{ timer->stop(); timer->deleteLater(); });

    connect(btn, &QPushButton::pressed, this, [onTick, timer]{
        onTick();
        timer->start();
    });
    connect(btn, &QPushButton::released, this, [timer, onStop]{
        timer->stop();
        if (onStop) onStop();
    });
    // 注:Qt 在按钮上按下后即使鼠标拖出再松开也会触发 released(只要松开点在
    // 按钮所在窗口内),所以这里不再额外装事件过滤器避免泄漏。
}

void UiController::runTestPoints()
{
    if (auto* sb = m_mainWindow->findChild<QStatusBar*>(QStringLiteral("statusbar"))) {
        sb->showMessage(QStringLiteral("功能测试已移除"));
    }
}

/**
 * @brief 暂停或恢复当前任务：运行中 → 暂停；暂停中 → 停止
 */
void UiController::pauseOrResumeTask()
{
    if (!m_taskExecutor) return;
    if (m_taskExecutor->isRunning()) {
        m_taskExecutor->pause();
    } else if (m_taskExecutor->isPaused()) {
        m_taskExecutor->stop();
    }
}

/**
 * @brief 判断任务是否正在运行或暂停中（供 UI 判断按钮状态）
 */
bool UiController::isTaskRunning() const
{
    return m_taskExecutor && (m_taskExecutor->isRunning() || m_taskExecutor->isPaused());
}

// ==================== 任务执行器回调 ====================

/**
 * @brief 任务进度更新回调：刷新状态栏进度、同步机械臂状态、触发界面刷新信号
 */
void UiController::onTaskProgress(int current, int total)
{
    if (auto* sb = m_mainWindow->findChild<QStatusBar*>(QStringLiteral("statusbar")))
        sb->showMessage(QStringLiteral("任务进度: %1/%2").arg(current).arg(total));

    m_robotController->getMotionController()->updateState();
    emit jogCompleted();
}

/**
 * @brief 任务暂停回调：弹出对话框，让用户选择"继续执行"或"结束任务"
 *
 * 选择"结束任务"时会二次确认，避免误操作。
 */
void UiController::onTaskPaused(int current, int total)
{
    // 更新停止按钮文本
    if (auto* btn = m_mainWindow->findChild<QPushButton*>(QStringLiteral("btnProgStop")))
        btn->setText(QStringLiteral("▶ 恢复"));

    QMessageBox msgBox;
    msgBox.setWindowTitle(QStringLiteral("设备暂停中"));
    msgBox.setText(QStringLiteral("设备已暂停\n\n当前进度: %1 / %2").arg(current).arg(total));
    msgBox.setIcon(QMessageBox::Information);
    QPushButton* btnResume = msgBox.addButton(QStringLiteral("继续执行"), QMessageBox::AcceptRole);
    msgBox.addButton(QStringLiteral("结束任务"), QMessageBox::DestructiveRole);
    msgBox.setDefaultButton(btnResume);
    msgBox.exec();

    if (msgBox.clickedButton() == btnResume) {
        m_taskExecutor->resume();
    } else {
        // 二次确认
        QMessageBox confirmBox;
        confirmBox.setWindowTitle(QStringLiteral("确认停止"));
        confirmBox.setText(QStringLiteral("确定要结束任务吗？\n\n机械臂将保持在当前位置不动。"));
        confirmBox.setIcon(QMessageBox::Warning);
        confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        confirmBox.setDefaultButton(QMessageBox::No);
        if (confirmBox.exec() == QMessageBox::Yes) {
            m_taskExecutor->stop();
            if (auto* btn = m_mainWindow->findChild<QPushButton*>(QStringLiteral("btnProgStop")))
                btn->setText(QStringLiteral("⏸ 暂停"));
            if (auto* sb = m_mainWindow->findChild<QStatusBar*>(QStringLiteral("statusbar")))
                sb->showMessage(QStringLiteral("是否取消当前任务"));
        }
    }
}

/**
 * @brief 任务恢复执行回调：在状态栏显示"任务继续执行"提示
 */
void UiController::onTaskResumed(int current, int total)
{
    if (auto* sb = m_mainWindow->findChild<QStatusBar*>(QStringLiteral("statusbar")))
        sb->showMessage(QStringLiteral("任务继续执行 (%1/%2)").arg(current).arg(total));
}

/**
 * @brief 任务完成回调：恢复按钮文本、恢复日志详细输出、打印完成提示、刷新状态
 *
 * 单步模式（m_stepMode=true）下：自动 reset TaskExecutor 到第 0 条，提示"已重置，可继续单步"，
 * 不打"任务执行完毕"——避免与"已重置"提示混在一起。下次再点单步按钮即从头重跑。
 */
void UiController::onTaskFinished()
{
    if (auto* btn = m_mainWindow->findChild<QPushButton*>(QStringLiteral("btnProgStop")))
        btn->setText(QStringLiteral("⏹ 停止"));

    ZmotionAPI::setVerbose(true);
    m_robotController->getMotionController()->updateState();
    emit jogCompleted();

    // 清空编辑器行高亮
    m_currentExecLine = -1;
    highlightCurrentLine(-1);

    if (m_stepMode) {
        // 单步模式走完末尾：清变量/循环栈，把 m_currentIdx 复位为 0，m_stepMode 保留以便继续
        m_taskExecutor->reset();
        appendConsole(QStringLiteral("✔ 单步执行完毕，已重置；再按\"单步\"从头开始"));
        appendConsole(QStringLiteral("══════════════════════════════"));
        if (auto* sb = m_mainWindow->findChild<QStatusBar*>(QStringLiteral("statusbar")))
            sb->showMessage(QStringLiteral("单步已重置"));
        return;
    }

    appendConsole(QStringLiteral("✔ 任务执行完毕"));
    appendConsole(QStringLiteral("══════════════════════════════"));
    if (auto* sb = m_mainWindow->findChild<QStatusBar*>(QStringLiteral("statusbar")))
        sb->showMessage(QStringLiteral("所有任务执行完毕"));
}

/**
 * @brief 任务错误回调：恢复按钮文本、恢复日志详细输出、把错误信息打印到控制台和状态栏
 */
void UiController::onTaskError(const QString& msg)
{
    if (auto* btn = m_mainWindow->findChild<QPushButton*>(QStringLiteral("btnProgStop")))
        btn->setText(QStringLiteral("⏹ 停止"));

    ZmotionAPI::setVerbose(true);
    appendConsole(QStringLiteral("✘ 错误: %1").arg(msg));
    if (auto* sb = m_mainWindow->findChild<QStatusBar*>(QStringLiteral("statusbar")))
        sb->showMessage(QStringLiteral("任务错误: %1").arg(msg));
    // 清空高亮
    m_currentExecLine = -1;
    highlightCurrentLine(-1);
}

/**
 * @brief 当前执行源代码行变化：在编辑器上把该行加绿色高亮 + 行首加绿色圆点
 *
 * 通过 QPlainTextEdit::extraSelections() 给指定行加浅绿背景。
 * 同时在行首插入一个绿色圆点字符（用 FullWidthSelection 之外的 charFormat 模拟）。
 *
 * @param line 源代码行号（0-based），-1 表示清除高亮
 */
void UiController::onCurrentLineChanged(int line)
{
    m_currentExecLine = line;
    highlightCurrentLine(line);

    // 单步调试/正常执行通用：把编辑器光标移到该行，保证可见
    if (line >= 0) {
        if (auto* editor = getEditor()) {
            QTextCursor c = editor->textCursor();
            c.movePosition(QTextCursor::Start);
            c.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line);
            c.movePosition(QTextCursor::EndOfBlock);
            c.clearSelection();
            editor->setTextCursor(c);
            editor->ensureCursorVisible();
        }
    }
}

/**
 * @brief 在 progEditor 上给指定行加绿色高亮 + 行首绿色圆点
 *
 * - 高亮：浅绿背景（QTextCharFormat::BackgroundBrush），全行宽（FullWidthSelection）
 * - 圆点：在行首添加 "🟢 "（emoji 圆点字符），其他行显示 "  "
 *
 * @param line 行号（0-based），< 0 时清除所有高亮并恢复空标记
 */
void UiController::highlightCurrentLine(int line)
{
    auto* editor = getEditor();
    if (!editor) return;

    QList<QTextEdit::ExtraSelection> extra;

    if (line >= 0) {
        // === 1. 当前执行行：浅绿高亮 ===
        QTextEdit::ExtraSelection hl;
        hl.format.setBackground(QColor(0, 200, 0, 60));
        hl.format.setProperty(QTextFormat::FullWidthSelection, true);
        QTextCursor c = editor->textCursor();
        c.movePosition(QTextCursor::Start);
        c.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line);
        c.select(QTextCursor::LineUnderCursor);
        hl.cursor = c;
        extra << hl;
    }

    editor->setExtraSelections(extra);

    // === 2. 行首圆点：保存原始文本 + 注入"🟢 " / "  " 前缀 ===
    // 为了避免每次高亮都全量重写（影响性能），采用增量模式：
    // - 记录上次注入的 prefix，撤掉之后再注入新的
    // - 不修改用户内容，只通过 overlay 模拟
    // QPlainTextEdit 不支持 overlay 文本，这里用 setExtraSelections 中的一个
    // 0-width selection + 工具提示的方式过于 hack。
    // 简化方案：每次 emit currentLineChanged 时在控制台追加一个圆点标记；
    // 编辑器行首圆点由后续扩展（CodeEditor 子类）实现。
    Q_UNUSED(line);
}
