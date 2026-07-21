#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "VisionController/vmcontrol.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QCloseEvent;

/**
 * @brief 示例程序主窗口。
 *
 * MainWindow 只作为功能演示层：负责界面输入、按钮命令发送和结果展示；
 * 视觉方案、流程、模块参数页等实际操作全部由 VMControl 完成。
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造主窗口并创建 VMControl 模块实例。
     * @param parent Qt 父对象。
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief 析构主窗口，释放 ui 对象。
     */
    ~MainWindow() override;

    /**
     * @brief 初始化界面控件状态。
     *
     * 输入数据：无。
     * 输出数据：无直接返回值，内部设置 ActiveX 控件和密码输入框状态。
     */
    void initializeUI();

    /**
     * @brief 建立 MainWindow 与 VMControl 之间的信号槽连接。
     *
     * 输入数据：无。
     * 输出数据：无直接返回值，连接命令信号与结果信号。
     */
    void connectVMControl();

signals:
    /**
     * @brief 请求 VMControl 执行方案命令。
     * @param command 方案操作类型。
     * @param params 附加参数，Load/SaveAs/Switching 使用 solPath、password。
     */
    void requestSolutionCommand(SolMotionType command
                                , const QVariantMap& params = QVariantMap());

    /**
     * @brief 请求 VMControl 执行流程命令。
     * @param command 流程操作类型。
     * @param processName 流程名称。
     */
    void requestProcedureCommand(ProcMotionType command
                                 , const QString& processName);

    /**
     * @brief 请求 VMControl 打开模块参数页。
     * @param command 模块操作类型，仅支持 Set 或 GroupSet。
     * @param params 模块参数，包含 processName、moduleName，GroupSet 还包含 groupName。
     */
    void requestModuleCommand(ModuMotionType command
                                 , const QVariantMap& params);

private slots:
    /** @brief 处理 VMControl 返回的状态事件。输入 event，输出到状态文本框。 */
    void handleVisionStatusEvent(const VisionStatusEvent &event);

    /** @brief 处理 VMControl 返回的数据事件。输入 event，按类型分发到界面控件。 */
    void handleVisionDataEvent(const VisionDataEvent &event);

    /** @brief 处理 VMControl 返回的日志消息。输入 event，输出到消息文本框。 */
    void handleVisionMessageEvent(const VisionMessageEvent &event);

    /** @brief 处理方案加载后的流程列表数据。输入 pack，输出到流程下拉框。 */
    void handleSolData(const QVariantMap &pack);

    /** @brief 选择方案文件按钮槽函数。输入来自文件对话框，输出到路径输入框。 */
    void on_pushButton_chosse_clicked();

    /** @brief 加载方案按钮槽函数。输入路径和密码，输出方案加载命令。 */
    void on_pushButton_load_clicked();

    /** @brief 方案执行一次按钮槽函数。输出 Run 命令。 */
    void on_pushButton_run_clicked();

    /** @brief 方案连续执行按钮槽函数。输出 Runing 命令。 */
    void on_pushButton_rining_clicked();

    /** @brief 保存方案按钮槽函数。输出 Save 命令。 */
    void on_pushButton_save_clicked();

    /** @brief 方案另存为按钮槽函数。输入保存路径和密码，输出 SaveAs 命令。 */
    void on_pushButton_saveAS_clicked();

    /** @brief 当前流程执行一次按钮槽函数。输出流程 Run 命令。 */
    void on_runPro_Button_clicked();

    /** @brief 当前流程连续执行按钮槽函数。输出流程 Runing 命令。 */
    void on_runingPro_Button_clicked();

    /** @brief 当前流程停止按钮槽函数。输出流程 Stop 命令。 */
    void on_stopPro_Button_clicked();

    /** @brief 方案停止按钮槽函数。输出 Stop 命令。 */
    void on_pushButton_stop_clicked();

    /** @brief 关闭方案按钮槽函数。输出 Close 命令。 */
    void on_pushButton_close_clicked();

    /** @brief 演示打开组合模块中的粗定位模块参数页。 */
    void on_pushButton_cuSet_clicked();

    /** @brief 演示打开组合模块中的精定位模块参数页。 */
    void on_pushButton_jingSet_clicked();

    /** @brief 演示打开组合模块中的直线查找模块参数页。 */
    void on_pushButton_lineSet_clicked();

    /** @brief 演示打开组合模块中的标定转换模块参数页。 */
    void on_pushButton_turnSet_clicked();

    /** @brief 演示打开流程直属图像源模块参数页。 */
    void on_runPro_Button_6_clicked();

private:
    /**
     * @brief 获取界面当前选择的流程名。
     * @return 当前流程下拉框文本。
     */
    QString selectedProcessName() const;

    /**
     * @brief 把多目标数据追加显示到数据窗口。
     * @param pack VMControl 返回的数据包，包含 processName 和 TargetDataList。
     */
    void appendTargetData(const QVariantMap& pack);

    /**
     * @brief 把单点数据追加显示到数据窗口。
     * @param pack VMControl 返回的数据包，包含 processName 和 PointData。
     */
    void appendPointData(const QVariantMap& pack);

    /**
     * @brief 展示多目标列表中的每个目标。
     * @param dataList 多目标结果列表。
     */
    void dataShow(const QList<TargetData>& dataList);

    Ui::MainWindow *ui = nullptr;       ///< Qt Designer 生成的界面对象。
    VMControl *m_vmcontrol = nullptr;   ///< 视觉模块实例，承载实际 VM 操作。

protected:

};

#endif // MAINWINDOW_H
