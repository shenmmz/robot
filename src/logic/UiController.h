/*
 * @file UiController.h
 * @brief UI 辅助功能控制器，管理 IO 监控 / 报警日志 / 编程运行 / 设置页面 / 手动调试。
 */
#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QHash>
#include <functional>

class QWidget;
class QPlainTextEdit;
class QPushButton;
class RobotController;
class TaskExecutor;

class UiController : public QObject
{
    Q_OBJECT

public:
    explicit UiController(QWidget* mainWindow, RobotController* robotCtrl, QObject* parent = nullptr);
    ~UiController();

    void init();

signals:
    void jogCompleted();

public slots:
    void runTestPoints();
    void pauseOrResumeTask();
    bool isTaskRunning() const;

    // 编程-插入指令（供 MainWindow 调用）
    void onProgInsertMoveJ();
    void onProgInsertMoveL();

    /// 同步点位编辑器——按程序引用排序
    void syncTargets();

    /// 运行单行文本（单步调试）
    void runText(const QString& fullText, const QString& sourceLine);

    /// 退出单步会话（在"运行"/"停止"按钮回调里调用，使 finish 后走正常分支）
    void exitStepMode() { m_stepMode = false; }

    /// 单步执行一条指令：
    ///   - fullText   完整脚本文本（含 TARGET 定义与指令）
    ///   - startLine  -1 = 继续 TaskExecutor 当前的 m_currentIdx；>=0 = 从指定源行号开始
    /// 连续调用本函数即可实现"点一次走一步"。
    void runStep(const QString& fullText, int startLine);

    /// 插入文本到编辑器光标，自动换行
    void insertAtCursor(const QString& text);

    // 编程-指令按钮（public，供 MainWindow 动态按钮连接）
    void onProgInsertWaitIdle();
    void onProgInsertDelay();
    void onProgInsertSetDO();
    void onProgInsertWaitDI();
    void onProgInsertPrint();
    void onProgInsertComment();
    void onProgInsertCall();
    void onProgInsertSet();
    void onProgInsertFor();

    /// 读取当前位置并输出到控制台（关节 + 笛卡尔）
    void onProgReadPosition();

    // === 新增指令 ===
    void onProgInsertMoveOff();
    void onProgInsertMoveLOff();
    void onProgInsertSpeed();
    void onProgInsertAccel();
    void onProgInsertDecel();
    void onProgInsertTimeout();
    void onProgInsertIfDI();
    void onProgInsertElse();
    void onProgInsertEndIf();
    void onProgInsertServo();     // SERVO(ch=ang:spd) 单/多路均可（多路：逗号分隔）

private slots:
    // === IO监控 ===
    void refreshIO();

    // === 任务执行器回调 ===
    void onTaskProgress(int current, int total);
    void onTaskPaused(int current, int total);
    void onTaskResumed(int current, int total);
    void onTaskFinished();
    void onTaskError(const QString& msg);
    /// 当前执行行变化（编辑器绿色高亮 + 圆点）
    void onCurrentLineChanged(int line);

    // === 报警日志 ===
    void onRobotError(int errorCode, const QString& errorMsg);
    void onHardwareError(int errorCode, const QString& errorMsg);
    void onClearAlarms();

    // === 编程-运行/停止/文件操作 ===
    void onProgRun();
    void onProgStop();
    void onProgSave();
    void onProgLoad();
    void onProgNew();
    void onProgDeleteLine();

    // === 编程-指令按钮回调 ===
    void onConsoleOutput(const QString& text);

    // === 设置 ===
    void onLoadSettings();
    void onSaveSettings();

    // === 手动点动 ===
    /// @brief 关节点动回调:按指定轴的正负方向移动一个步长(调试模式下才生效)
    void onJogJoint(int axis, bool positive);
    /// @brief 笛卡尔点动回调:按指定轴的正负方向移动一个步长(调试模式下才生效)
    void onJogCart(int index, bool positive);
    /// @brief 调试模式开关:勾选后启用手动点动控件和调试参数输入框;未勾选时全部禁用
    void onDebugModeToggled(bool checked);

private:
    QWidget* m_mainWindow;
    RobotController* m_robotController;
    TaskExecutor* m_taskExecutor = nullptr;
    QWidget* m_centralWidget = nullptr;
    QTimer* m_ioTimer = nullptr;
    QHash<QString, bool> m_ioStateCache;
    bool m_pauseDialogActive = false;
    QString m_currentScriptPath;       // 当前打开的文件路径
    int m_targetCounter = 0;           // 目标编号自增
    QHash<QString, QList<double>> m_targets;  // 目标名 → 6个坐标值
    bool m_syncTargetsBusy = false;           // syncTargets 防重入
    int m_currentExecLine = -1;               // 当前执行源代码行号（-1=无）
    bool m_stepMode = false;                   // 当前是否处于单步会话中（用于决定 finish 后是否自动重置）

    // 辅助方法
    void setIOIndicator(const QString& frameName, bool active);
    void addAlarmRow(const QString& type, int code, const QString& desc);

    /// 给编辑器当前行加绿色高亮（line < 0 时清除）
    void highlightCurrentLine(int line);

    /// 通用"按住自动重复"按钮绑定：按下立即执行一次 + 定时重复，松开停止
    /// - intervalMs: 重复触发周期（默认 50ms）
    /// - onTick  : 每次触发的回调
    /// - onStop : 松开时触发的回调（可空）
    void bindHoldButton(QPushButton* btn, int intervalMs,
                        std::function<void()> onTick,
                        std::function<void()> onStop = nullptr);

    /// 读取默认运动参数（优先调试面板，否则设置页）
    void readDefaultMotionParams(double& speed, double& accel, double& decel, int& timeoutMs);

    void insertAtCursor(QPlainTextEdit* editor, const QString& text);

    /// 获取编辑器控件指针
    QPlainTextEdit* getEditor() const;
    QPlainTextEdit* getTargetEditor() const;

    /// 追加到控制台
    void appendConsole(const QString& text);

    /// 解析脚本为指令列表
    QList<struct ScriptCommand> parseScript(const QString& text);
};

#endif // UICONTROLLER_H
