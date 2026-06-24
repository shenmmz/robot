/*
 * @file CodeEditor.h
 * @brief 带行号显示区域的 QPlainTextEdit 子类。
 *
 * 在编辑器左侧绘制行号区域，支持动态宽度调整。
 * 用法：替换 QPlainTextEdit，无需额外配置即可自动显示行号。
 */
#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QWidget>

class LineNumberArea;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    int lineNumberAreaWidth();

    /// 行号区域绘制（由 LineNumberArea::paintEvent 调用）
    void lineNumberAreaPaintEvent(QPaintEvent *event);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    LineNumberArea *m_lineNumberArea;

    friend class LineNumberArea;
};

/// 行号区域绘制控件（仅由 CodeEditor 内部使用）
class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor)
        : QWidget(editor), m_codeEditor(editor)
    {
    }

    QSize sizeHint() const override
    {
        return QSize(m_codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        m_codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *m_codeEditor;
};

#endif // CODEEDITOR_H
