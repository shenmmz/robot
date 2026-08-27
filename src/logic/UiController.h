/*
 * @file UiController.h
 * @brief UI 辅助功能控制器，管理 IO 监控 / 报警日志 / 编程运行 / 设置页面 / 手动调试。
 */
#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include <QObject>
#include <QEvent>
#include <QTimer>
#include <QHash>
#include <functional>
#include "TargetDef.h"      // MotionParams:点位级 speed/accel/decel/timeoutMs
#include "ScriptCommand.h"  // ScriptCmdType / ScriptCommand（parseScript 及辅助方法使用）

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
    /// 编程"运行"按钮回调：解析编辑器脚本并交给 TaskExecutor 执行。
    /// MainWindow 的快捷流程按钮（如打钉子）也调用它复用整段执行链路。
    void onProgRun();

    /// 运行指定路径的脚本文件（直接读磁盘，绕过编辑器当前内容）。
    /// 供"打钉子"等固定流程按钮调用：读取 .rbt 全文，按 "\n---\n"
    /// 拆分点位区/程序区后复用 runText() 链路执行；任务执行中调用会被忽略。
    /// 返回是否成功启动。
    bool runScriptFile(const QString& filePath);

    /// 退出单步会话（在"运行"/"停止"按钮回调里调用，使 finish 后走正常分支）
    void exitStepMode() { m_stepMode = false; }

    /// 单步执行一条指令：
    ///   - fullText   完整脚本文本（含 TARGET 定义与指令）
    ///   - startLine  -1 = 继续 TaskExecutor 当前的 m_currentIdx；>=0 = 从指定源行号开始
    /// 连续调用本函数即可实现"点一次走一步"。
    void runStep(const QString& fullText, int startLine);

    /// 插入文本到编辑器光标，自动换行
    void insertAtCursor(const QString& text);

    /// 内部直接保存脚本到指定路径(不弹文件对话框),返回成功与否
    /// 用于点位管理弹窗 OK 后自动保存场景
    bool saveScriptToFile(const QString& filePath);

    // 编程-指令按钮（public，供 MainWindow 动态按钮连接）
    void onProgInsertWaitIdle();
    void onProgInsertDelay();
    void onProgInsertSetDO();
    void onProgInsertWaitDI();
    void onProgInsertWaitDO();
    void onProgInsertPrint();
    void onProgInsertComment();
    void onProgInsertCall();
    void onProgInsertSet();
    void onProgInsertFor();

    /// 读取当前位置并输出到控制台（关节 + 笛卡尔）
    void onProgReadPosition();

    /// 弹出点位编辑对话框：列出当前流程引用的所有 TARGET，可逐个修改坐标值
    void showTargetEditDialog();

    /// 同步程序注释：扫描点位编辑器中 TARGET 的行尾注释，追加到程序编辑器引用该点位的 MOVEJ/MOVEL 行
    void syncProgramComments();

    /// 同步程序编辑器中 MOVEJ/MOVEL 行的 speed/acc/dec/timeout 关键字参数,
    /// 把点位 m_targetMotion 的值同步过去,保证点位管理修改后编辑器行级参数同步
    void syncMotionInMoveLines();

    /// 同步程序编辑器中的 SERVO 行(舵机角度):用 m_servoSlots 的所有数据
    /// 更新程序中的 SERVO 行。SERVO 是独立行，不再绑定到 MOVE 指令。
    /// 支持增加、修改、删除操作，保持顺序不变。
    void syncServoInMoveLines();

    /// 按序号删除程序编辑器中的 SERVO 行（从 0 开始计数）
    void removeServoByIndex(int servoIndex);

    // === 新增指令 ===
    void onProgInsertMoveOff();
    void onProgInsertMoveLOff();
    void onProgInsertSpeed();
    void onProgInsertAccel();
    void onProgInsertDecel();
    void onProgInsertTimeout();
    void onProgInsertIfIO();
    void onProgInsertElse();
    void onProgInsertEndIf();
    void onProgInsertServo();     // SERVO(ch=ang:spd) 单/多路均可（多路：逗号分隔）

private slots:
    // === IO监控 ===
    void refreshIO();
    /// 事件过滤器：处理 DI/DO QFrame 鼠标点击（DO 单击翻转，DI/DO 双击重命名）
    bool eventFilter(QObject* obj, QEvent* event) override;
    /// 加载 IO 自定义名称配置
    void loadIOLabels();
    /// 保存 IO 自定义名称配置
    void saveIOLabels();
    /// 弹出重命名对话框，修改单个 IO 标签
    void renameIOLabel(const QString& frameName);

    // === 任务执行器回调 ===
    void onTaskProgress(int current, int total);
    void onTaskPaused(int current, int total);
    void onTaskResumed(int current, int total);
    void onTaskFinished();
    void onTaskError(const QString& msg);
    /// 当前执行行变化（编辑器绿色高亮 + 圆点）
    void onCurrentLineChanged(int line);

    /// 点位编辑器文本变化回调：把 TARGET 行增量解析进 m_targets（已存在则覆盖值）
    void onTargetEditorChanged();

    // === 报警日志 ===
    void onRobotError(int errorCode, const QString& errorMsg);
    void onHardwareError(int errorCode, const QString& errorMsg);
    void onClearAlarms();

    // === 编程-运行/停止/文件操作 ===
    void onProgStop();
    void onProgSave();
    void onProgLoad();
    void onProgNew();
    void onProgDeleteLine();
    void onProgCopyLine();
    void onProgPasteLine();

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
    QString m_copiedLine;              // 复制暂存的行文本
    int m_targetCounter = 0;           // 目标编号自增
    QHash<QString, QList<double>> m_targets;  // 目标名 → 6个坐标值
    /// @brief 目标名 → 点位级运动参数(speed/accel/decel/timeoutMs,0 表示未设)
    /// 与 m_targets 并行存在;在解析 TARGET 行 `}` 之后的 `#speed=..` 段时填入
    QHash<QString, MotionParams> m_targetMotion;
    /// @brief SERVO 舵机序列(按程序中出现顺序)，每条为一个独立的 SERVO 指令
    QList<ServoSlotArray> m_servoSlots;
    bool m_syncTargetsBusy = false;           // syncTargets 防重入
    bool m_syncingComments = false;          // syncProgramComments 防重入
    int m_currentExecLine = -1;               // 当前执行源代码行号（-1=无）
    bool m_stepMode = false;                   // 当前是否处于单步会话中（用于决定 finish 后是否自动重置）
    QHash<QString, QString> m_ioLabels;       // IO 自定义名称（frameName → 自定义名称）

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

    /// @brief 解析 TARGET 行 `}` 之后到行尾的 motion 后缀段
    /// @param tail `}` 之后的全部字符串(可能含 `//` 注释)
    /// @return 解析得到的 MotionParams;若不存在 motion 段则返回 hasAny()=false 的默认值
    /// @note 期望格式:`// #speed=N acc=N dec=N tmo=N` 或 `#speed=N acc=N dec=N tmo=N`
    ///       `#` 必须出现在 `}` 紧邻位置(允许之前有空白);`//` 注释里的 `#` 不会被识别
    static MotionParams parseMotionSuffix(const QString& tail);

    /// @brief 把 MotionParams 格式化为 `#speed=N acc=N dec=N tmo=N` 段;若全 0 则返回空串
    static QString formatMotionSuffix(const MotionParams& mp);

    void insertAtCursor(QPlainTextEdit* editor, const QString& text);

    /// 获取编辑器控件指针
    QPlainTextEdit* getEditor() const;
    QPlainTextEdit* getTargetEditor() const;

    /// 追加到控制台
    void appendConsole(const QString& text);

    // parseScript 辅助方法
    void applyKeywordParams(ScriptCommand& cmd, const QString& line);
    ScriptCommand parseMoveCmd(ScriptCmdType type, const QString& line, double defSpd, double defAcc, double defDecel, int defTimeout);
    ScriptCommand parseParamSet(ScriptCmdType type, int prefixLen, const QString& line, double defSpd, double defAcc, double defDecel, int defTimeout);
    ScriptCommand parseIO(ScriptCmdType type, int prefixLen, const QString& line, double defSpd, double defAcc, double defDecel, int defTimeout);
    ScriptCommand parseQuoted(ScriptCmdType type, const QString& line, double defSpd, double defAcc, double defDecel, int defTimeout);
    ScriptCommand parseOffCmd(ScriptCmdType type, const QString& line, double defSpd, double defAcc, double defDecel, int defTimeout);

    /// 解析脚本为指令列表
    QList<struct ScriptCommand> parseScript(const QString& text);
};

#endif // UICONTROLLER_H
