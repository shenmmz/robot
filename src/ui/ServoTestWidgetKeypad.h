/*
 * @file ServoTestWidgetKeypad.h
 * @brief 舵机调试弹窗嵌入式数字软键盘
 *
 * 用于在 ServoTestWidget 中为 QDoubleSpinBox 提供数字输入,
 * 避免依赖 Qt VirtualKeyboard 模块。
 *
 * 行为:
 *   - 调用 showFor(spin) 后,以 Qt::Popup 方式在 spinbox 旁边弹出
 *   - 弹出期间把当前值加载到内部 m_text,用户可编辑
 *   - 点 "确定" 把 m_text 解析为 double 并写回 spin,然后隐藏
 *   - 点 "取消" 丢弃改动直接隐藏
 *   - 点 spin 外部自动隐藏(Qt::Popup 默认行为)
 */
#ifndef SERVOTESTWIDGETKEYPAD_H
#define SERVOTESTWIDGETKEYPAD_H

#include <QWidget>

class QDoubleSpinBox;
class QLabel;
class QPushButton;

class ServoTestWidgetKeypad : public QWidget
{
    Q_OBJECT

public:
    explicit ServoTestWidgetKeypad(QWidget* parent = nullptr);
    ~ServoTestWidgetKeypad() override;

    /**
     * @brief 在 spinbox 旁边弹出软键盘,自动加载其当前值。
     * @param spin 目标输入框;若为空则忽略
     */
    void showFor(QDoubleSpinBox* spin);

    /**
     * @brief 主动隐藏软键盘(若有目标,也保留 spin 当前值不变)
     */
    void hide();

protected:
    /**
     * @brief 点击窗口外时,Qt::Popup 会发出 mousePressEvent,
     *        关闭自身(默认行为是隐藏)
     */
    bool event(QEvent* e) override;

private slots:
    void onDigitClicked();
    void onBackspaceClicked();
    void onClearClicked();
    void onDotClicked();
    void onOkClicked();
    void onCancelClicked();

private:
    /**
     * @brief 重新把 m_text 渲染到显示标签上
     */
    void refreshDisplay();

    /**
     * @brief 根据目标 spinbox 的最小值格式化显示(超过最大值会截断)
     */
    void commitToTarget();

    /**
     * @brief 创建一个统一风格的按键按钮
     */
    QPushButton* makeButton(const QString& text, const char* slot,
                            const QString& bgColor = QStringLiteral("#fafbfc"),
                            const QString& fgColor = QStringLiteral("#222"));

private:
    QDoubleSpinBox* m_target = nullptr;   // 当前正在编辑的 spinbox
    QString         m_text;              // 内部维护的字符串,作为 "编辑缓冲"

    QLabel*       m_display = nullptr;   // 顶部显示当前编辑内容的标签
    QPushButton*  m_btnOk    = nullptr;
    QPushButton*  m_btnCancel = nullptr;
};

#endif // SERVOTESTWIDGETKEYPAD_H