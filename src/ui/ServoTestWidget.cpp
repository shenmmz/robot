/*
 * @file ServoTestWidget.cpp
 * @brief 舵机测试界面组件实现
 */
#include "ServoTestWidget.h"
#include "../hardware/ServoController.h"
#include "../logic/UiController.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QFrame>
#include <QMessageBox>
#include <QStatusBar>
#include <QMainWindow>
#include <QShowEvent>
#include <QEvent>
#include <QTimer>
#include <QtGlobal>
#include <QDoubleValidator>
#include <QDebug>

ServoTestWidget::ServoTestWidget(ServoController* servo, UiController* ui, QWidget *parent)
    : QWidget(parent)
    , m_servo(servo)
    , m_ui(ui)
{
    setupUI();

    // 屏幕键盘(TabTip/osk.exe)由全局 OsKEventFilter 统一接管,不再需要本地 Keypad

    // 操作反馈 toast 浮层(顶部居中)
    m_toastLabel = new QLabel(this);
    m_toastLabel->setAlignment(Qt::AlignCenter);
    m_toastLabel->setWordWrap(true);
    m_toastLabel->setStyleSheet(QStringLiteral(
        "QLabel{background:rgba(21,101,192,0.92);color:#ffffff;font-size:15px;"
        "font-weight:bold;padding:8px 18px;border-radius:6px;"
        "font-family:'Microsoft YaHei UI','Microsoft YaHei',sans-serif;}"));
    m_toastLabel->hide();
    m_toastLabel->setMinimumHeight(36);

    m_toastTimer = new QTimer(this);
    m_toastTimer->setSingleShot(true);
    m_toastTimer->setInterval(2000);
    connect(m_toastTimer, &QTimer::timeout, this, &ServoTestWidget::onToastTimeout);

    // 初始化勾选状态
    for (int i = 0; i < 6; ++i) {
        m_checkedChannels[i] = false;
    }
}

ServoTestWidget::~ServoTestWidget()
{
}

/**
 * @brief 在窗口顶部居中显示临时提示,2 秒后自动消失
 */
void ServoTestWidget::showToast(const QString& text)
{
    if (!m_toastLabel) return;
    if (text.isEmpty()) {
        m_toastLabel->hide();
        return;
    }
    m_toastLabel->setText(text);
    m_toastLabel->adjustSize();
    // 顶部居中,留 12px 边距
    const int w = qMin(m_toastLabel->sizeHint().width() + 24, width() - 24);
    const int x = (width() - w) / 2;
    m_toastLabel->setGeometry(x, 8, w, m_toastLabel->sizeHint().height());
    m_toastLabel->raise();
    m_toastLabel->show();
    if (m_toastTimer) {
        m_toastTimer->start();
    }
}

void ServoTestWidget::onToastTimeout()
{
    if (m_toastLabel) {
        m_toastLabel->hide();
    }
}

/**
 * @brief 弹窗每次显示时同步一次 ServoController 当前角度到 UI（标签+滑块+spinbox）
 *
 * 行为：
 *   - 读 m_servo->getCurrentAngle(0..5) 写回 6 路"当前角度"标签
 *   - 未勾选通道：滑块/spinbox 也回写（让"角度"列显示实际位置）
 *   - 已勾选通道：保持用户编辑的值不动，避免打断
 *
 * 注意：ServoController::currentAngle 是软件推算值（速度×dt 累加），初始为 90°。
 *       如需硬件真实位置，需要扩展 Rs485Controller 读寄存器功能——本次仅同步已有缓存。
 */
void ServoTestWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    if (!m_servo) return;
    for (int i = 0; i < 6; ++i) {
        float cur = m_servo->getCurrentAngle(i);
        // 未勾选通道：把滑块/spinbox 同步到 currentAngle（让"角度"列与实际位置一致）
        if (!m_checkedChannels[i]) {
            m_angleSliders[i]->blockSignals(true);
            m_angleSpins[i]->blockSignals(true);
            m_angleSliders[i]->setValue(qBound(30, static_cast<int>(cur), 150));
            m_angleSpins[i]->setText(QString::number(cur, 'f', 1));
            m_angleSliders[i]->blockSignals(false);
            m_angleSpins[i]->blockSignals(false);
        }
    }

    // 让 toast 跟着显示出来(若上次超时还没到就被关闭)
    if (m_toastLabel && !m_toastLabel->text().isEmpty() && m_toastTimer && m_toastTimer->isActive()) {
        // 重新计算位置(因为窗口尺寸可能变化了)
        const int w = qMin(m_toastLabel->sizeHint().width() + 24, width() - 24);
        const int x = (width() - w) / 2;
        m_toastLabel->setGeometry(x, 8, w, m_toastLabel->sizeHint().height());
        m_toastLabel->raise();
        m_toastLabel->show();
    }
}

/**
 * @brief 初始化UI界面
 */
void ServoTestWidget::setupUI()
{
    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(16, 16, 16, 16);

    // 标题
    QLabel* titleLabel = new QLabel("舵机设置界面", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);

    // 表头
    QGridLayout* headerLayout = new QGridLayout();
    headerLayout->setSpacing(16);
    QLabel* h1 = new QLabel("选择", this);
    QLabel* h2 = new QLabel("通道", this);
    QLabel* h3 = new QLabel("角度 (°)", this);
    QLabel* h4 = new QLabel("应用", this);
    QLabel* h5 = new QLabel("速度 (°/s)", this);
    QString headerStyle = "font-size:16px;font-weight:bold;color:#555;";
    h1->setStyleSheet(headerStyle);
    h2->setStyleSheet(headerStyle);
    h3->setStyleSheet(headerStyle);
    h4->setStyleSheet(headerStyle);
    h5->setStyleSheet(headerStyle);
    headerLayout->addWidget(h1, 0, 0);
    headerLayout->addWidget(h2, 0, 1);
    headerLayout->addWidget(h3, 0, 2);
    headerLayout->addWidget(h4, 0, 3);
    headerLayout->addWidget(h5, 0, 4);
    mainLayout->addLayout(headerLayout);

    // 分隔线
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: #cccccc; min-height: 2px;");
    mainLayout->addWidget(line);

    // 6个舵机通道
    for (int i = 0; i < 6; ++i) {
        QHBoxLayout* rowLayout = new QHBoxLayout();
        rowLayout->setSpacing(16);

        // 勾选框
        m_checkBoxes[i] = new QCheckBox(this);
        m_checkBoxes[i]->setObjectName(QString("checkBox_%1").arg(i));
        m_checkBoxes[i]->setStyleSheet("QCheckBox::indicator{width:24px;height:24px;}");
        rowLayout->addWidget(m_checkBoxes[i], 0, Qt::AlignCenter);

        // 通道标签
        QLabel* chLabel = new QLabel(QString("P%1").arg(i + 1), this);
        chLabel->setStyleSheet("font-weight: bold; color: #2196F3; font-size: 18px;");
        chLabel->setMinimumWidth(56);
        rowLayout->addWidget(chLabel);

        // 角度滑块
        m_angleSliders[i] = new QSlider(Qt::Horizontal, this);
        m_angleSliders[i]->setMinimum(30);
        m_angleSliders[i]->setMaximum(150);
        m_angleSliders[i]->setValue(90);
        m_angleSliders[i]->setMinimumWidth(280);
        m_angleSliders[i]->setMaximumWidth(400);
        m_angleSliders[i]->setFixedHeight(40);
        m_angleSliders[i]->setStyleSheet(
            "QSlider::groove:horizontal{height:10px;background:#d0d5dd;border-radius:5px;}"
            "QSlider::handle:horizontal{width:28px;height:28px;margin:-9px 0;"
            "background:#1565c0;border-radius:4px;}"
            "QSlider::handle:horizontal:hover{background:#1976d2;}"
            "QSlider::sub-page:horizontal{background:#1565c0;border-radius:5px;}");
        rowLayout->addWidget(m_angleSliders[i]);

        // 角度输入框
        m_angleSpins[i] = new QLineEdit(this);
        m_angleSpins[i]->setValidator(new QDoubleValidator(30.0, 140.0, 1, m_angleSpins[i]));
        m_angleSpins[i]->setText("90.0");
        m_angleSpins[i]->setAlignment(Qt::AlignCenter);
        m_angleSpins[i]->setMinimumWidth(100);
        m_angleSpins[i]->setStyleSheet("font-size:16px;font-weight:bold;");
        rowLayout->addWidget(m_angleSpins[i]);

        // 应用按钮
        m_applyBtns[i] = new QPushButton("应用", this);
        m_applyBtns[i]->setStyleSheet(
            "QPushButton {"
            "background-color: #E3F2FD; "
            "border: 1px solid #2196F3; "
            "border-radius: 3px; "
            "padding: 2px 8px; "
            "min-width: 70px;"
            "font-size: 16px;"
            "}"
            "QPushButton:hover {"
            "background-color: #BBDEFB;"
            "}"
            "QPushButton:pressed {"
            "background-color: #90CAF9;"
            "}"
        );
        m_applyBtns[i]->setCursor(Qt::PointingHandCursor);
        rowLayout->addWidget(m_applyBtns[i]);

        // 速度输入框（固定1.0，不可编辑）
        m_speedSpins[i] = new QLineEdit(this);
        m_speedSpins[i]->setText("1.0");
        m_speedSpins[i]->setReadOnly(true);
        m_speedSpins[i]->setAlignment(Qt::AlignCenter);
        m_speedSpins[i]->setMinimumWidth(100);
        m_speedSpins[i]->setStyleSheet("font-size:16px;font-weight:bold;color:#999;background:#f0f0f0;");
        rowLayout->addWidget(m_speedSpins[i]);

        mainLayout->addLayout(rowLayout);

        // 连接信号
        connect(m_checkBoxes[i], &QCheckBox::toggled, this, [this, i](bool checked) {
            m_checkedChannels[i] = checked;
            // 勾选/取消时刷新该行的锁定状态
            updateChannelLockState(i);
        });

        connect(m_angleSliders[i], &QSlider::sliderMoved, this, [this, i](int value) {
            if (!m_servo) return;
            if (!m_checkedChannels[i]) return;   // 未勾选：拖动无效
            // 同步角度输入框显示（用 blockSignals 避免触发 spin 的 valueChanged 死循环）
            m_angleSpins[i]->blockSignals(true);
            m_angleSpins[i]->setText(QString::number(value, 'f', 1));
            m_angleSpins[i]->blockSignals(false);
            // 关键：setAngleDrag 走 50ms timer 批量下发 6 路，不卡
            m_servo->setAngleDrag(i, static_cast<float>(value));
        });

        // 拖动结束：用 setAngleWithSpeed 兜底
        connect(m_angleSliders[i], &QSlider::sliderReleased, this, [this, i]() {
            if (!m_servo) return;
            if (!m_checkedChannels[i]) return;
            float angle = static_cast<float>(m_angleSpins[i]->text().toDouble());
            m_servo->setAngleWithSpeed(i, angle, 1.0f);
        });

        // 角度输入框编辑结束后同步滑块
        connect(m_angleSpins[i], &QLineEdit::editingFinished,
                this, [this, i]() {
            if (!m_checkedChannels[i]) return;   // 未勾选：改值也不动舵机
            double value = m_angleSpins[i]->text().toDouble();
            m_angleSliders[i]->setValue(static_cast<int>(value));
        });

        // 速度输入框编辑结束后同步给舵机控制器作为默认速度
        connect(m_speedSpins[i], &QLineEdit::editingFinished, this, [this, i]() {
            if (!m_servo) return;
            if (!m_checkedChannels[i]) return;   // 未勾选：不同步默认速度
            m_servo->setDefaultSpeed(i, static_cast<float>(m_speedSpins[i]->text().toDouble()));
        });

        // 应用按钮：仅勾选通道生效，读取角度驱动舵机（速度固定1°/s）
        connect(m_applyBtns[i], &QPushButton::clicked, this, [this, i]() {
            if (!m_servo) return;
            if (!m_checkedChannels[i]) return;
            float angle = static_cast<float>(m_angleSpins[i]->text().toDouble());
            m_servo->setAngleWithSpeed(i, angle, 1.0f);
        });

        // 初始化该行的锁定状态（默认未勾选 → 角度控件禁用）
        updateChannelLockState(i);
    }

    // 连接舵机角度变化信号
    if (m_servo) {
        connect(m_servo, &ServoController::angleChanged,
                this, &ServoTestWidget::onAngleChanged);
    }

    // ===== 底部操作条：插入 SERVO 到编辑器 =====
    QFrame* line2 = new QFrame(this);
    line2->setFrameShape(QFrame::HLine);
    line2->setStyleSheet("background-color: #cccccc; min-height: 2px; margin-top: 8px;");
    mainLayout->addWidget(line2);

    QHBoxLayout* actionRow = new QHBoxLayout();
    actionRow->setSpacing(12);

    QLabel* actionHint = new QLabel(QStringLiteral("勾选要插入的通道 → 设置时间 → 点击插入"), this);
    actionHint->setStyleSheet("color:#666;font-size:12px;");
    actionRow->addWidget(actionHint);
    actionRow->addStretch();

    QLabel* timeLabel = new QLabel(QStringLiteral("T (ms):"), this);
    timeLabel->setStyleSheet("font-size:13px;font-weight:bold;color:#444;");
    actionRow->addWidget(timeLabel);

    m_insertTimeSpin = new QLineEdit(this);
    m_insertTimeSpin->setValidator(new QDoubleValidator(1.0, 30000.0, 0, m_insertTimeSpin));
    m_insertTimeSpin->setText("500");
    m_insertTimeSpin->setAlignment(Qt::AlignCenter);
    m_insertTimeSpin->setMinimumWidth(110);
    m_insertTimeSpin->setToolTip(QStringLiteral("生成 SERVO 时间模式的时间参数（ms），≥10 走软插值"));
    actionRow->addWidget(m_insertTimeSpin);

    // 失焦时若值 <10 自动归正为 10，与生成逻辑对齐
    connect(m_insertTimeSpin, &QLineEdit::editingFinished, this, [this]() {
        int v = static_cast<int>(m_insertTimeSpin->text().toDouble());
        if (v < 1) m_insertTimeSpin->setText("1");
    });

    // 插入到编辑器按钮：按勾选通道生成 SERVO(ch=ang:Tms,...) 多路表达式
    QPushButton* btnInsert = new QPushButton(QStringLiteral("📋 插入 SERVO 到编辑器"), this);
    btnInsert->setMinimumSize(180, 36);
    btnInsert->setStyleSheet(
        QStringLiteral(
            "QPushButton{background:#1565c0;color:#fff;border:1px solid #0d47a1;"
            "border-radius:4px;font-weight:bold;font-size:13px;padding:4px 12px;}"
            "QPushButton:hover{background:#0d47a1;}"
            "QPushButton:pressed{background:#0a3d8a;}"));
    btnInsert->setToolTip(QStringLiteral("按勾选通道生成 SERVO 指令并插入到主编辑器（多路用逗号分隔）"));
    connect(btnInsert, &QPushButton::clicked, this, &ServoTestWidget::onInsertToEditor);
    actionRow->addWidget(btnInsert);

    mainLayout->addLayout(actionRow);

    // 去掉底部弹性空间，界面紧凑贴合内容
}

/**
 * @brief 角度更新回调
 */
void ServoTestWidget::onAngleChanged(int channel, float angle)
{
    if (channel < 0 || channel >= 6) return;
    if (!m_servo) return;

    // 拖动中（用户正握着滑块）不更新滑块/spinbox，避免和用户手抖对抗
    if (m_servo->isMoving(channel)) {
        return;
    }

    // 非移动：把实际角度回写到滑块和输入框（勾选通道不打断用户的输入）
    if (!m_checkedChannels[channel]) {
        m_angleSliders[channel]->setValue(static_cast<int>(angle));
        m_angleSpins[channel]->setText(QString::number(angle, 'f', 1));
    }
}

/**
 * @brief 刷新某通道的勾选/锁定状态
 *
 * - 勾选：滑块 + 角度输入框启用，灰底变白底，可拖可改
 * - 未勾选：滑块 + 角度输入框禁用，灰底变浅灰，拖动/改值都无效（实现"必须勾选才能控制"）
 *   速度输入框和"应用"按钮始终启用（速度即使未勾选也可改默认速度，但只对勾选通道生效）
 */
void ServoTestWidget::updateChannelLockState(int channel)
{
    if (channel < 0 || channel >= 6) return;

    const bool enabled = m_checkedChannels[channel];
    m_angleSliders[channel]->setEnabled(enabled);
    m_angleSpins[channel]->setEnabled(enabled);

    // 角度输入框样式：禁用时灰底 + 浅灰文字，启用时正常白底
    const QString angleStyle =
        enabled
        ? QStringLiteral(
              "QLineEdit{font-family:Consolas,'Microsoft YaHei UI';font-size:16px;"
              "padding:1px 2px;border:1px solid #d0d5dd;border-radius:3px;background:#fafbfc;}"
              "QLineEdit:focus{border-color:#1565c0;background:#fff;}")
        : QStringLiteral(
              "QLineEdit{font-family:Consolas,'Microsoft YaHei UI';font-size:16px;"
              "padding:1px 2px;border:1px solid #e0e0e0;border-radius:3px;background:#f0f1f3;color:#aaa;}");
    m_angleSpins[channel]->setStyleSheet(angleStyle);
}

/**
 * @brief "📋 插入 SERVO 到编辑器"槽：按勾选顺序生成 SERVO(ch=ang:Tms, ...) 并插入主编辑器
 *
 * 行为：
 *   - 仅处理勾选的通道（按通道号 0..5 升序）
 *   - 角度使用每行 angleSpin 的当前值（即用户拖动滑块后、或弹窗打开时由 showEvent 同步的当前角度）
 *   - 时间使用底部 m_insertTimeSpin，单位 ms
 *   - 生成 SERVO(0=130:T500, 1=30:T500) 这种多路表达式，插入到主编辑器光标处
 *   - m_ui 空时只把文本打到控制台，不报错弹窗（便于脱机测试）
 */
void ServoTestWidget::onInsertToEditor()
{
    QStringList parts;
    int timeMs = m_insertTimeSpin ? static_cast<int>(m_insertTimeSpin->text().toDouble()) : 500;
    if (timeMs < 10) timeMs = 10;   // 与脚本 SERVO 时间模式规则对齐

    for (int i = 0; i < 6; ++i) {
        if (!m_checkedChannels[i]) continue;
        float ang = static_cast<float>(m_angleSpins[i]->text().toDouble());
        // 用整型角度，更贴近实际硬件刻度；非整数也保留 1 位小数便于精调
        QString angStr = (qFuzzyCompare(ang, qRound(ang) * 1.0f))
                         ? QString::number(qRound(ang))
                         : QString::number(ang, 'f', 1);
        parts << QStringLiteral("%1=%2:T%3").arg(i).arg(angStr).arg(timeMs);
    }

    if (parts.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("未勾选通道"),
                                 QStringLiteral("请先勾选要插入 SERVO 的通道"));
        return;
    }

    QString text = QStringLiteral("SERVO(%1)").arg(parts.join(QStringLiteral(", ")));

    if (!m_ui) {
        // 没有 UiController:用 toast 提示,避免静默失败
        showToast(QStringLiteral("插入失败:UiController 未初始化"));
//         qDebug() << "[ServoTestWidget] generated SERVO line (no UiController):" << text;
        return;
    }

    m_ui->insertAtCursor(text);

    // 主路径:toast 提示(始终可见,与 statusBar 无关)
    showToast(QStringLiteral("已插入: %1").arg(text));

    // 兼容兜底:尝试同步到主窗口 statusBar(失败也不影响 toast)
    QWidget* p = parentWidget();
    while (p && !qobject_cast<QMainWindow*>(p)) p = p->parentWidget();
    if (auto* mw = qobject_cast<QMainWindow*>(p)) {
        mw->statusBar()->showMessage(QStringLiteral("已插入: %1").arg(text), 3000);
    }
}
