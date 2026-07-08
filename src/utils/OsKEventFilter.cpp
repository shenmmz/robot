/*
 * @file OsKEventFilter.cpp
 * @brief 全局 Windows 系统屏幕键盘事件过滤器实现
 *
 * 触发逻辑:
 *   - FocusIn:对 QAbstractSpinBox 兜底设置 Qt::WA_InputMethodEnabled
 *     (QLineEdit 默认就有,但 setEnabled 切换可能丢失,一并兜底),
 *     然后调 QGuiApplication::inputMethod()->show() —
 *     Qt 在 Windows 上通过 IMM32 / IInputMethod2 自动拉起 TabTip.exe
 *   - FocusOut:调 inputMethod()->hide()
 *
 * 不消费事件:return QObject::eventFilter(watched, ev) 放行,
 * 让原生控件的 keyPress / 鼠标事件正常处理,物理键盘仍可直接输入。
 */
#include "OsKEventFilter.h"

#include <QAbstractSpinBox>
#include <QEvent>
#include <QGuiApplication>
#include <QInputMethod>
#include <QLineEdit>
#include <QWidget>

OsKEventFilter::OsKEventFilter(QObject* parent)
    : QObject(parent)
{
}

bool OsKEventFilter::eventFilter(QObject* watched, QEvent* ev)
{
    // isEnabled() / setAttribute() 都是 QWidget 的方法,
    // eventFilter 拿到的是 QObject*,需要先 cast 一次
    auto* w = qobject_cast<QWidget*>(watched);
    if (!w) {
        return QObject::eventFilter(watched, ev);
    }

    // 精确匹配:QAbstractSpinBox (含 QSpinBox / QDoubleSpinBox) 和 QLineEdit
    // QPlainTextEdit 不在范围内 — 编程编辑器 / 输出控制台不要弹键盘
    const bool isSpin = (qobject_cast<QAbstractSpinBox*>(w) != nullptr);
    const bool isLine = (qobject_cast<QLineEdit*>(w) != nullptr);
    if (!isSpin && !isLine) {
        return QObject::eventFilter(watched, ev);
    }

    // 控件被禁用时不接收焦点,自然不会弹键盘 — 这里跳过即可
    if (!w->isEnabled()) {
        return QObject::eventFilter(watched, ev);
    }

    if (ev->type() == QEvent::FocusIn) {
        // 兜底:Qt::WA_InputMethodEnabled 在 setEnabled(false) → setEnabled(true)
        // 切换时可能丢失,每次 FocusIn 都强制设一次,确保一致性
        w->setAttribute(Qt::WA_InputMethodEnabled, true);
        if (auto* im = QGuiApplication::inputMethod()) {
            im->show();
        }
    } else if (ev->type() == QEvent::FocusOut) {
        if (auto* im = QGuiApplication::inputMethod()) {
            im->hide();
        }
    }
    return QObject::eventFilter(watched, ev);  // 不消费,放行
}