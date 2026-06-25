/*
 * @file ServoTestWidget.cpp
 * @brief 舵机测试界面组件实现
 */
#include "ServoTestWidget.h"
#include "../hardware/ServoController.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QFrame>
#include <QDebug>

ServoTestWidget::ServoTestWidget(ServoController* servo, QWidget *parent)
    : QWidget(parent)
    , m_servo(servo)
{
    setupUI();

    // 初始化勾选状态
    for (int i = 0; i < 6; ++i) {
        m_checkedChannels[i] = false;
    }
}

ServoTestWidget::~ServoTestWidget()
{
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
    QLabel* titleLabel = new QLabel("舵机测试界面", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);

    // 表头
    QGridLayout* headerLayout = new QGridLayout();
    headerLayout->setSpacing(16);
    QLabel* h1 = new QLabel("选择", this);
    QLabel* h2 = new QLabel("通道", this);
    QLabel* h3 = new QLabel("角度 (°)", this);
    QLabel* h4 = new QLabel("当前", this);
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
        m_angleSliders[i]->setMinimum(0);
        m_angleSliders[i]->setMaximum(180);
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
        m_angleSpins[i] = new QDoubleSpinBox(this);
        m_angleSpins[i]->setMinimum(0);
        m_angleSpins[i]->setMaximum(180);
        m_angleSpins[i]->setValue(90);
        m_angleSpins[i]->setSuffix(" °");
        m_angleSpins[i]->setSingleStep(1);
        m_angleSpins[i]->setMinimumWidth(100);
        m_angleSpins[i]->setStyleSheet("font-size:16px;font-weight:bold;");
        rowLayout->addWidget(m_angleSpins[i]);

        // 当前角度显示
        m_currentLabels[i] = new QLabel("90°", this);
        m_currentLabels[i]->setStyleSheet(
            "background-color: #E3F2FD; "
            "border: 1px solid #2196F3; "
            "border-radius: 3px; "
            "padding: 2px 8px; "
            "min-width: 70px;"
            "font-size: 16px;"
        );
        m_currentLabels[i]->setAlignment(Qt::AlignCenter);
        rowLayout->addWidget(m_currentLabels[i]);

        // 速度输入框
        m_speedSpins[i] = new QDoubleSpinBox(this);
        m_speedSpins[i]->setMinimum(1);
        m_speedSpins[i]->setMaximum(180);
        m_speedSpins[i]->setValue(10);   // 默认速度 10°/s
        m_speedSpins[i]->setSuffix(" °/s");
        m_speedSpins[i]->setSingleStep(5);
        m_speedSpins[i]->setMinimumWidth(100);
        m_speedSpins[i]->setStyleSheet("font-size:16px;font-weight:bold;");
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
            m_angleSpins[i]->setValue(value);
            m_angleSpins[i]->blockSignals(false);
            // 关键：setAngleDrag 走 50ms timer 批量下发 6 路，不卡
            m_servo->setAngleDrag(i, static_cast<float>(value));
        });

        // 拖动结束：用 setAngleWithSpeed 兜底（带速度控制，确保最后一帧到位）
        connect(m_angleSliders[i], &QSlider::sliderReleased, this, [this, i]() {
            if (!m_servo) return;
            if (!m_checkedChannels[i]) return;   // 未勾选：松手也不动
            float angle = static_cast<float>(m_angleSpins[i]->value());
            float speed = static_cast<float>(m_speedSpins[i]->value());
            m_servo->setAngleWithSpeed(i, angle, speed);
        });

        // 角度输入框变化时同步滑块（不驱动舵机，避免拖动时死循环）
        connect(m_angleSpins[i], QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, [this, i](double value) {
            if (!m_checkedChannels[i]) return;   // 未勾选：改值也不动舵机
            m_angleSliders[i]->setValue(static_cast<int>(value));
        });

        // 速度输入框编辑结束后同步给舵机控制器作为默认速度
        connect(m_speedSpins[i], &QDoubleSpinBox::editingFinished, this, [this, i]() {
            if (!m_servo) return;
            if (!m_checkedChannels[i]) return;   // 未勾选：不同步默认速度
            m_servo->setDefaultSpeed(i, static_cast<float>(m_speedSpins[i]->value()));
        });

        // 初始化该行的锁定状态（默认未勾选 → 角度控件禁用）
        updateChannelLockState(i);
    }

    // 连接舵机角度变化信号
    if (m_servo) {
        connect(m_servo, &ServoController::angleChanged,
                this, &ServoTestWidget::onAngleChanged);
    }

    // 去掉底部弹性空间，界面紧凑贴合内容
}

/**
 * @brief 角度更新回调
 */
void ServoTestWidget::onAngleChanged(int channel, float angle)
{
    if (channel < 0 || channel >= 6) return;
    if (!m_servo) return;

    // 始终刷新"当前角度"显示（让用户看到舵机实际走到哪里）
    m_currentLabels[channel]->setText(QString("%1°").arg(angle, 0, 'f', 1));

    // 拖动中（用户正握着滑块）不更新滑块/spinbox，避免和用户手抖对抗
    if (m_servo->isMoving(channel)) {
        return;
    }

    // 非移动：把实际角度回写到滑块和输入框（勾选通道不打断用户的输入）
    if (!m_checkedChannels[channel]) {
        m_angleSliders[channel]->setValue(static_cast<int>(angle));
        m_angleSpins[channel]->setValue(angle);
    }
}

/**
 * @brief 刷新某通道的勾选/锁定状态
 *
 * - 勾选：滑块 + 角度输入框启用，灰底变白底，可拖可改
 * - 未勾选：滑块 + 角度输入框禁用，灰底变浅灰，拖动/改值都无效（实现"必须勾选才能控制"）
 *   速度输入框和当前角度显示始终启用（速度即使未勾选也可改默认速度，但只对勾选通道生效）
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
              "QDoubleSpinBox{font-family:Consolas,'Microsoft YaHei UI';font-size:16px;"
              "padding:1px 2px;border:1px solid #d0d5dd;border-radius:3px;background:#fafbfc;}"
              "QDoubleSpinBox:focus{border-color:#1565c0;background:#fff;}")
        : QStringLiteral(
              "QDoubleSpinBox{font-family:Consolas,'Microsoft YaHei UI';font-size:16px;"
              "padding:1px 2px;border:1px solid #e0e0e0;border-radius:3px;background:#f0f1f3;color:#aaa;}");
    m_angleSpins[channel]->setStyleSheet(angleStyle);
}
