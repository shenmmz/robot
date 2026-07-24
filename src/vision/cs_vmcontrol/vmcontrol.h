#ifndef VMCONTROL_H
#define VMCONTROL_H

#include <QMap>
#include <QObject>
#include <QSet>
#include <QSharedPointer>
#include <QTimer>
#include <QVector>

#include "vmbase.h"
#include "vmtypes.h"

using TargetDataList = QVector<TargetData>;
using TargetDataListPtr = QSharedPointer<TargetDataList>;

/**
 * @brief VM 多目标结果缓存包。
 *
 * 该结构体用于需要在模块内部暂存目标列表时携带流程、相机和有效性信息。
 */
struct VM_TargetPacket
{
    qint64 timestamp = 0;          ///< 数据生成时间戳。
    int flowId = 0;                ///< 流程编号。
    int cameraId = 0;              ///< 相机编号。
    bool valid = false;            ///< 数据是否有效。
    QVector<TargetData> targets;   ///< 多目标结果列表。
};

/**
 * @brief 将视觉状态枚举转换为界面可显示文本。
 * @param status 视觉状态枚举。
 * @return 状态说明文本。
 */
inline QString userRoleToString(VisionStatus status)
{
    switch (status) {
    case VisionStatus::NotLoaded:
        return QStringLiteral("视觉方案未加载");
    case VisionStatus::Loaded:
        return QStringLiteral("视觉方案已加载");
    case VisionStatus::FailToLoad:
        return QStringLiteral("视觉方案加载失败");
    default:
        return QStringLiteral("未知");
    }
}

/**
 * @brief VisionMaster 模块控制类。
 *
 * VMControl 是对外可复用的模块入口。上层程序通过信号槽调用三个统一命令入口，
 * VMControl 负责命令分发、调用 VMbase、处理回调结果并通过信号返回状态和数据。
 */
class VMControl : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 创建 VMControl 并初始化底层 VMbase。
     * @param parent Qt 父对象。
     */
    explicit VMControl(QObject *parent = nullptr);

    /**
     * @brief 析构 VMControl，释放流程超时定时器。
     */
    ~VMControl() override;

    /**
     * @brief 初始化 VMbase 回调信号连接。
     *
     * 输入数据：无。
     * 输出数据：无直接返回值，内部连接流程开始/结束回调。
     */
    void initializeConnect();

public slots:
    /**
     * @brief 执行方案级命令。
     * @param command 方案操作类型。
     * @param params 附加参数；Load/SaveAs/Switching 使用 solPath、password。
     */
    void executeSolutionCommand(SolMotionType command, const QVariantMap& params = QVariantMap());

    /**
     * @brief 执行流程级命令。
     * @param command 流程操作类型。
     * @param processName 流程名称。
     */
    void executeProcedureCommand(ProcMotionType command, const QString& processName);

    /** @brief 设置指定流程的连续采集模式，阻止 hanlePointData 自动 StopRun。 */
    void setContinuousCollection(const QString &processName, bool enabled);

    /** @brief 重置指定流程的成功标记，使后续回调不被拦截。 */
    void resetProcSuccessFlag(const QString &processName);

    /**
     * @brief 执行模块参数页命令。
     * @param command 模块操作类型，仅支持 Set 和 GroupSet。
     * @param params 模块参数，包含 processName、moduleName，GroupSet 还包含 groupName。
     */
    void executeModuleCommand(ModuMotionType command, const QVariantMap& params);

private slots:
    /** @brief 加载方案。输入方案路径和密码，输出状态、流程列表和日志信号。 */
    void loadSolution(const QString& solPath, const QString& password);
    /** @brief 关闭方案。输入无，输出状态和日志信号。 */
    void closeSolution();
    /** @brief 保存当前方案。输入无，输出日志信号。 */
    void saveSolution();
    /** @brief 方案另存为。输入保存路径和密码，输出日志信号。 */
    void saveSolutionAs(const QString& solPath, const QString& password);
    /** @brief 切换方案。输入新方案路径和密码，输出状态、流程列表和日志信号。 */
    void switchSolution(const QString& solPath, const QString& password);
    /** @brief 整套方案执行一次。输入无，输出日志信号。 */
    void runSolution();
    /** @brief 整套方案连续执行。输入无，输出日志信号。 */
    void runSolutionContinuously();
    /** @brief 停止整套方案执行。输入无，输出日志信号。 */
    void stopSolution();

    /** @brief 流程执行一次。输入流程名，输出渲染数据和日志信号。 */
    void runProcedure(const QString& processName);
    /** @brief 流程连续执行。输入流程名，输出渲染数据、超时数据和日志信号。 */
    void runProcedureContinuously(const QString& processName);
    /** @brief 停止流程执行。输入流程名，输出渲染数据和日志信号。 */
    void stopProcedure(const QString& processName);
    /** @brief 打开流程参数页。输入流程名，输出参数页句柄数据和日志信号。 */
    void setProcedure(const QString& processName);

    /** @brief 打开流程直属模块参数页。输入流程名和模块名，输出参数页句柄数据。 */
    void setModule(const QString& processName, const QString& moduleName);
    /** @brief 打开组合模块内部参数页。输入流程名、组合模块名和模块名，输出参数页句柄数据。 */
    void setGroupModule(const QString& processName, const QString& groupName, const QString& moduleName);

    /** @brief 接收 VMbase 的流程开始回调。输入回调包，当前仅占位。 */
    void proBeginReceive(const QVariantMap &backData);
    /** @brief 接收 VMbase 的流程结束回调。输入回调包，按数据类型分发处理。 */
    void proEndReceive(const QVariantMap &backData);
    /** @brief 处理多目标数据。输入 VM 回调包，输出 TargetData 数据事件。 */
    void hanleTargetData(const QVariantMap &backData);
    /** @brief 处理单点数据。输入 VM 回调包，输出 PointData 数据事件。 */
    void hanlePointData(const QVariantMap &backData);
    /** @brief 处理无业务数据的流程结束事件。输入 VM 回调包，停止流程和定时器。 */
    void hanleprocData(const QVariantMap &backData);
    /** @brief 处理流程超时。输入流程名，输出超时数据事件。 */
    void onTimeoutForProc(const QString& procName);

signals:
    /** @brief 状态变化信号。输出 VisionStatusEvent。 */
    void statusChanged(const VisionStatusEvent &event);
    /** @brief 数据就绪信号。输出 VisionDataEvent。 */
    void dataReady(const VisionDataEvent &event);
    /** @brief 日志消息信号。输出 VisionMessageEvent。 */
    void messageReady(const VisionMessageEvent &event);

private:
    /** @brief 统一发送日志消息。输入消息文本，输出 messageReady 信号。 */
    void emitMessage(const QString& message);
    /** @brief 统一发送数据事件。输入数据类型和数据包，输出 dataReady 信号。 */
    void emitData(VisionDataType dataType, const QVariantMap &pack);
    /** @brief 统一发送状态事件。输入状态和说明文本，输出 statusChanged 信号。 */
    void emitStatus(VisionStatus status, const QString& message);

    VMbase *m_vmbase = nullptr;                         ///< 底层 VisionMaster SDK 封装对象。
    VisionStatus oldStatus = VisionStatus::NotLoaded;   ///< 上一次视觉状态。
    VisionStatus nowStatus = VisionStatus::NotLoaded;   ///< 当前视觉状态。

    QMap<QString, QTimer*> m_procTimers;                ///< 每个连续执行流程对应的超时定时器。
    QMap<QString, bool> m_procSuccessFlags;             ///< 每个流程是否已成功收到有效数据。
    QSet<QString> m_continuousProcs;                ///< 处于连续采集模式（不自动 StopRun）的流程集合。
};

#endif // VMCONTROL_H
