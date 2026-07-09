/*
 * @file OsKEventFilter.h
 * @brief 全局 Windows 系统屏幕键盘(TabTip / osk.exe)事件过滤器
 *
 * 设计目的:在工控机/触摸屏环境下,点击 QSpinBox / QDoubleSpinBox 时
 * 弹出 Windows 系统屏幕键盘。QLineEdit 默认已开启 Qt::WA_InputMethodEnabled,
 * 但 QAbstractSpinBox 默认不开启,必须显式设置才能触发 QInputMethod。
 *
 * 用法:在 main() 里 QApplication 创建后立即安装到 qApp 上即可,
 *      后续所有弹窗 / 控件自动接管。
 *
 *      qApp->installEventFilter(new OsKEventFilter(qApp));
 *
 * 注意:
 * - 只对 QAbstractSpinBox / QLineEdit 生效,QPlainTextEdit 等代码编辑器不弹键盘
 * - 不消费事件(放行),原生控件编辑逻辑不变
 * - 失焦时调 hide(),与系统软键盘标准行为一致
 */
#ifndef OSKEVENTFILTER_H
#define OSKEVENTFILTER_H

#include <QObject>

/**
 * @brief 全局系统屏幕键盘事件过滤器
 */
class OsKEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit OsKEventFilter(QObject* parent = nullptr);

protected:
    /**
     * @brief 拦截 QAbstractSpinBox / QLineEdit 的 FocusIn/FocusOut,触发 QInputMethod
     */
    bool eventFilter(QObject* watched, QEvent* ev) override;
};

#endif // OSKEVENTFILTER_H