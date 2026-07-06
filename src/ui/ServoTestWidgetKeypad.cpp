/*
 * @file ServoTestWidgetKeypad.cpp
 * @brief ServoTestWidgetKeypad 实现
 */
#include "ServoTestWidgetKeypad.h"

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QEvent>
#include <QScreen>
#include <QCursor>
#include <QDebug>

ServoTestWidgetKeypad::ServoTestWidgetKeypad(QWidget* parent)
    : QWidget(parent)
{
    // 设为 Popup:点击窗口外自动关闭,且不会抢主窗口焦点
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose, false);

    setStyleSheet(QStringLiteral(
        "ServoTestWidgetKeypad{background:#ffffff;border:2px solid #1565c0;"
        "border-radius:6px;}"));

    // ===== 顶部:输入显示标签 =====
    m_display = new QLabel(this);
    m_display->setMinimumHeight(36);
    m_display->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_display->setStyleSheet(QStringLiteral(
        "QLabel{background:#f0f4fa;color:#0d47a1;font-size:18px;font-weight:bold;"
        "padding:4px 10px;border:1px solid #c0d0e8;border-radius:4px;"
        "font-family:Consolas,'Microsoft YaHei UI';}"));

    // ===== 数字键 0-9、小数点 =====
    QGridLayout* grid = new QGridLayout();
    grid->setSpacing(6);
    grid->setContentsMargins(6, 6, 6, 6);

    // 第 0 行:7 8 9
    grid->addWidget(makeButton(QStringLiteral("7"), SLOT(onDigitClicked())), 0, 0);
    grid->addWidget(makeButton(QStringLiteral("8"), SLOT(onDigitClicked())), 0, 1);
    grid->addWidget(makeButton(QStringLiteral("9"), SLOT(onDigitClicked())), 0, 2);
    // 第 1 行:4 5 6
    grid->addWidget(makeButton(QStringLiteral("4"), SLOT(onDigitClicked())), 1, 0);
    grid->addWidget(makeButton(QStringLiteral("5"), SLOT(onDigitClicked())), 1, 1);
    grid->addWidget(makeButton(QStringLiteral("6"), SLOT(onDigitClicked())), 1, 2);
    // 第 2 行:1 2 3
    grid->addWidget(makeButton(QStringLiteral("1"), SLOT(onDigitClicked())), 2, 0);
    grid->addWidget(makeButton(QStringLiteral("2"), SLOT(onDigitClicked())), 2, 1);
    grid->addWidget(makeButton(QStringLiteral("3"), SLOT(onDigitClicked())), 2, 2);
    // 第 3 行:小数点 0 退格
    grid->addWidget(makeButton(QStringLiteral("."), SLOT(onDotClicked()),
                               QStringLiteral("#fff7e0"), QStringLiteral("#a06000")), 3, 0);
    grid->addWidget(makeButton(QStringLiteral("0"), SLOT(onDigitClicked())), 3, 1);
    grid->addWidget(makeButton(QStringLiteral("⌫"), SLOT(onBackspaceClicked()),
                               QStringLiteral("#ffeaea"), QStringLiteral("#b71c1c")), 3, 2);

    // ===== 底部:清除 / 取消 / 确定 =====
    m_btnCancel = makeButton(QStringLiteral("取消"), SLOT(onCancelClicked()),
                              QStringLiteral("#eeeeee"), QStringLiteral("#444"));
    m_btnCancel->setMinimumHeight(40);

    QPushButton* btnClear = makeButton(QStringLiteral("清空"), SLOT(onClearClicked()),
                                       QStringLiteral("#fff7e0"), QStringLiteral("#a06000"));
    btnClear->setMinimumHeight(40);

    m_btnOk = makeButton(QStringLiteral("✓ 确定"), SLOT(onOkClicked()),
                          QStringLiteral("#1565c0"), QStringLiteral("#ffffff"));
    m_btnOk->setMinimumHeight(40);

    QHBoxLayout* bottomRow = new QHBoxLayout();
    bottomRow->setSpacing(6);
    bottomRow->setContentsMargins(6, 0, 6, 6);
    bottomRow->addWidget(m_btnCancel, 1);
    bottomRow->addWidget(btnClear,   1);
    bottomRow->addWidget(m_btnOk,    2);

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setSpacing(6);
    root->setContentsMargins(6, 6, 6, 6);
    root->addWidget(m_display);
    root->addLayout(grid);
    root->addLayout(bottomRow);

    // 让布局生效,获取推荐尺寸
    adjustSize();
}

ServoTestWidgetKeypad::~ServoTestWidgetKeypad()
{
}

QPushButton* ServoTestWidgetKeypad::makeButton(const QString& text, const char* slot,
                                               const QString& bgColor, const QString& fgColor)
{
    QPushButton* b = new QPushButton(text, this);
    b->setMinimumSize(64, 48);
    b->setCursor(Qt::PointingHandCursor);
    b->setStyleSheet(QStringLiteral(
        "QPushButton{background:%1;color:%2;border:1px solid #c0d0e8;"
        "border-radius:4px;font-size:18px;font-weight:bold;"
        "font-family:Consolas,'Microsoft YaHei UI';}"
        "QPushButton:hover{background:#dbe8f8;}"
        "QPushButton:pressed{background:#bbd2ee;}").arg(bgColor, fgColor));
    connect(b, SIGNAL(clicked()), this, slot);
    return b;
}

void ServoTestWidgetKeypad::showFor(QDoubleSpinBox* spin)
{
    if (!spin) {
        return;
    }
    m_target = spin;

    // 初始化内部编辑缓冲为当前值
    m_text = QString::number(spin->value(), 'f', spin->decimals());
    // 去掉无效尾随零,例如 500.000 → 500
    while (m_text.contains('.') && m_text.endsWith('0')) {
        m_text.chop(1);
    }
    if (m_text.endsWith('.')) {
        m_text.chop(1);
    }
    refreshDisplay();

    // 计算位置:放在 spinbox 正下方(若下方放不下,放到上方)
    QPoint spinBottomLeft = spin->mapToGlobal(QPoint(0, spin->height()));
    QSize mySize = sizeHint();
    QPoint targetPos = spinBottomLeft + QPoint(0, 4);

    // 若超出屏幕下方,则放到 spinbox 上方
    QPoint screenBottomRight = QPoint(QCursor::pos().x() + 400, 99999);
    Q_UNUSED(screenBottomRight);
    QRect avail = spin->screen()->availableGeometry();
    if (targetPos.y() + mySize.height() > avail.bottom()) {
        QPoint spinTopLeft = spin->mapToGlobal(QPoint(0, 0));
        targetPos = QPoint(spinTopLeft.x(), spinTopLeft.y() - mySize.height() - 4);
    }

    // 防止右侧越界
    if (targetPos.x() + mySize.width() > avail.right()) {
        targetPos.setX(avail.right() - mySize.width());
    }
    if (targetPos.x() < avail.left()) {
        targetPos.setX(avail.left());
    }

    move(targetPos);
    show();
    raise();
    // 不要用 activateWindow:会强制抢焦点,导致 spin 失去 FocusIn 后的状态不一致
}

void ServoTestWidgetKeypad::hide()
{
    m_target = nullptr;
    QWidget::hide();
}

bool ServoTestWidgetKeypad::event(QEvent* e)
{
    if (e->type() == QEvent::MouseButtonPress && m_target) {
        // Qt::Popup 在鼠标点击窗口外时会自动隐藏,这里不需要额外处理
    }
    return QWidget::event(e);
}

void ServoTestWidgetKeypad::onDigitClicked()
{
    QPushButton* b = qobject_cast<QPushButton*>(sender());
    if (!b) return;
    const QString d = b->text();

    // 限制最大长度(避免无限输入)
    if (m_text.size() >= 12) return;

    // 防止前导零过多(但允许 ".5" / ".123" 这种合法小数)
    if (m_text == QStringLiteral("0")) {
        m_text = d;
    } else {
        m_text.append(d);
    }
    refreshDisplay();
}

void ServoTestWidgetKeypad::onDotClicked()
{
    if (m_text.contains('.')) return;
    if (m_text.isEmpty()) {
        m_text = QStringLiteral("0.");
    } else {
        m_text.append('.');
    }
    refreshDisplay();
}

void ServoTestWidgetKeypad::onBackspaceClicked()
{
    if (!m_text.isEmpty()) {
        m_text.chop(1);
    }
    refreshDisplay();
}

void ServoTestWidgetKeypad::onClearClicked()
{
    m_text.clear();
    refreshDisplay();
}

void ServoTestWidgetKeypad::onOkClicked()
{
    commitToTarget();
    hide();
}

void ServoTestWidgetKeypad::onCancelClicked()
{
    // 不写回 spin,直接关闭
    hide();
}

void ServoTestWidgetKeypad::refreshDisplay()
{
    if (m_text.isEmpty()) {
        m_display->setText(QStringLiteral("0"));
    } else {
        m_display->setText(m_text);
    }
}

void ServoTestWidgetKeypad::commitToTarget()
{
    if (!m_target) return;
    if (m_text.isEmpty()) {
        // 空输入 = 不变
        return;
    }
    bool ok = false;
    double v = m_text.toDouble(&ok);
    if (!ok) {
        // 无法解析,忽略
        return;
    }
    // 利用 spinbox 自身的范围与 decimals 自动夹紧
    m_target->setValue(v);
}