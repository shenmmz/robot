#pragma once

#include <QDateTime>
#include <QList>
#include <QMetaType>
#include <QVariantMap>

/**
 * @brief 视觉方案级命令类型。
 *
 * MainWindow 或其他上层程序通过该枚举告诉 VMControl 需要对整套方案执行的动作。
 */
enum class SolMotionType {
    Load,       ///< 加载方案；需要 params["solPath"] 和 params["password"]。
    Close,      ///< 关闭当前方案；无额外输入数据。
    Save,       ///< 保存当前方案到原路径；无额外输入数据。
    SaveAs,     ///< 方案另存为；需要 params["solPath"] 和 params["password"]。
    Run,        ///< 整套方案执行一次；无额外输入数据。
    Runing,     ///< 整套方案连续执行；无额外输入数据。
    Stop,       ///< 停止整套方案执行；无额外输入数据。
    Switching,  ///< 切换方案；需要 params["solPath"] 和 params["password"]。
};

/**
 * @brief 视觉流程级命令类型。
 *
 * 上层只传递命令类型和流程名，由 VMControl 统一分发到 VMbase。
 */
enum class ProcMotionType {
    Load,    ///< 预留：加载流程。
    Close,   ///< 预留：关闭流程。
    Set,     ///< 打开流程参数页；输入为 processName。
    Save,    ///< 预留：保存流程。
    SaveAs,  ///< 预留：流程另存为。
    Run,     ///< 流程执行一次；输入为 processName。
    Runing,  ///< 流程连续执行；输入为 processName。
    Stop,    ///< 停止流程执行；输入为 processName。
};

/**
 * @brief 视觉模块参数页命令类型。
 *
 * 模块枚举只保留通用动作，具体流程名、组合模块名、模块名全部由 QVariantMap 传入。
 */
enum class ModuMotionType {
    Set,       ///< 打开流程直属模块参数页；需要 processName 和 moduleName。
    GroupSet,  ///< 打开组合模块内部模块参数页；需要 processName、groupName 和 moduleName。
};

/**
 * @brief 视觉方案当前状态。
 *
 * VMControl 通过 statusChanged 信号把该状态发送给界面或其他业务模块。
 */
enum class VisionStatus {
    NotLoaded,   ///< 尚未加载方案，不能执行视觉命令。
    FailToLoad,  ///< 方案加载失败。
    Loaded,      ///< 方案已成功加载。
};

/**
 * @brief VMControl 对外发送的数据类型。
 *
 * MainWindow 根据该枚举决定如何解析 VisionDataEvent::pack。
 */
enum class VisionDataType {
    TargetData,  ///< 多目标数据；pack 中包含 TargetDataList。
    PointData,   ///< 单点数据；pack 中包含 PointData。
    SolData,     ///< 方案加载后的流程列表数据。
    RenderData,  ///< 渲染控件句柄数据，用于显示流程运行画面。
    SetData,     ///< 参数控件句柄数据，用于显示流程或模块参数页。
    procData,    ///< 流程通用回传数据。
    Timeout,     ///< 流程执行超时数据。
};

/**
 * @brief 视觉状态事件。
 *
 * 输出数据：由 VMControl::statusChanged 发出，描述方案状态变化。
 */
struct VisionStatusEvent {
    QDateTime timestamp;  ///< 状态生成时间。
    VisionStatus Status;  ///< 当前视觉方案状态。
    QString message;      ///< 给界面展示或日志记录的状态说明。
};

/**
 * @brief 视觉数据事件。
 *
 * 输出数据：由 VMControl::dataReady 发出，pack 内容由 DataType 决定。
 */
struct VisionDataEvent {
    QDateTime timestamp;    ///< 数据生成时间。
    VisionDataType DataType;///< 数据类型。
    QVariantMap pack;       ///< 数据包，字段含义随 DataType 变化。
};

/**
 * @brief 视觉消息事件。
 *
 * 输出数据：由 VMControl::messageReady 发出，用于显示 SDK 操作日志或错误提示。
 */
struct VisionMessageEvent {
    QDateTime timestamp;  ///< 消息生成时间。
    QString message;      ///< 消息正文。
};

/**
 * @brief 单点结果数据。
 *
 * 输出数据：通常由 VM 流程返回一个点位坐标时使用。
 */
struct PointData {
    double x = 0;  ///< X 坐标。
    double y = 0;  ///< Y 坐标。
    double r = 0;  ///< 角度或旋转值。
};

/**
 * @brief 多目标结果数据。
 *
 * 输出数据：通常由 VM 流程返回多个目标位置、状态时使用。
 */
struct TargetData {
    int id = -1;      ///< 目标编号；默认 -1 表示无效编号。
    int status = 0;   ///< 目标状态；具体含义由 VM 流程输出定义。
    double x = 0;     ///< X 坐标。
    double y = 0;     ///< Y 坐标。
    double r = 0;     ///< 角度或旋转值。
};

// 将自定义数据类型注册到 Qt 元对象系统，便于 QVariant 保存和跨线程信号槽传递。
Q_DECLARE_METATYPE(PointData)
Q_DECLARE_METATYPE(TargetData)
Q_DECLARE_METATYPE(QList<TargetData>)
Q_DECLARE_METATYPE(SolMotionType)
Q_DECLARE_METATYPE(ProcMotionType)
Q_DECLARE_METATYPE(ModuMotionType)
Q_DECLARE_METATYPE(VisionStatusEvent)
Q_DECLARE_METATYPE(VisionDataEvent)
Q_DECLARE_METATYPE(VisionMessageEvent)
