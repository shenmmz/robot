/**
 * @file EventTypes.h
 * @brief 事件总线系统的事件类型定义文件
 * @author
 * @date 2025-01-17
 * @version 2.0.0
 */

#pragma once

#include <QString>
#include <QDateTime>
#include <QFileInfo>
#include <QAxWidget>
#include <QVariantMap>
#include <QVector>
#include <QMetaType>

/* =========================================================
 * 视觉系统定义
 * ========================================================= */

/**
 * @brief 视觉方案命令枚举
 */
enum class SolMotionType{
    Load,               ///< 加载方案；pack 需要 solPath、password。
    Close,              ///< 关闭方案；无输入数据。
    Save,               ///< 保存方案；无输入数据。
    SaveAs,             ///< 方案另存为；pack 需要 solPath、password。
    Run,                ///< 执行一次；无输入数据。
    Runing,             ///< 连续执行；无输入数据。
    Stop,               ///< 停止执行；无输入数据。
    Switching,          ///< 切换方案；pack 需要 solPath、password。
};

/**
 * @brief 视觉流程命令枚举
 */
enum class ProcMotionType{
    Load,               ///< 预留：加载流程。
    Close,              ///< 预留：关闭流程。
    Set,                ///< 打开流程参数页；pack 需要 processName。
    Save,               ///< 预留：保存流程。
    SaveAs,             ///< 预留：流程另存为。
    Run,                ///< 执行一次；pack 需要 processName。
    Runing,             ///< 连续执行；pack 需要 processName。
    Stop,               ///< 停止执行；pack 需要 processName。
};

/**
 * @brief 视觉模块命令枚举
 */
enum class ModuMotionType{
    Set,                ///< 流程直属模块参数设置；pack 需要 processName、moduleName。
    GroupSet,           ///< group内部模块参数设置；pack 需要 processName、groupName、moduleName。
};

/**
 * @brief 视觉状态枚举
 */
enum class VisionStatus {
    NotLoaded,          ///< 视觉方案未加载
    FailToLoad,         ///< 视觉方案加载失败
    Loaded,             ///< 视觉方案已加载
};


/**
 * @enum VMDataType
 * @brief VM数据类型枚举
 * @details 定义VMControl系统中不同组件的数据类型
 */
enum class VisionDataType {
    TargetData,         ///< 固定划分型点位数据
    PointData,          ///< 点位数据
    SolData,            ///< 方案回传数据
    RenderData,         ///< 相机画面控件渲染数据
    SetData,            ///< 设置控件渲染数据
    procData,           ///< 流程回传数据
    Timeout,            ///< 拍照超时报错
};


/* =========================================================
 * 视觉接收广播事件
 * ========================================================= */
/**
 * @brief  SolCommandEvent 方案命令接收
 * @param  SolType 方案执行命令类型
 * @param  pack 携带数据
 */
struct SolCommandEvent {
    SolMotionType SolType;  ///< 命令类型
    QVariantMap pack;       ///< 携带数据
};

/**
 * @brief  ProcCommandEvent 流程命令接收
 * @param  ProcType 流程执行命令类型
 * @param  pack 携带数据
 */
struct ProcCommandEvent {
    ProcMotionType ProcType;///< 命令类型
    QVariantMap pack;       ///< 携带数据
};

/**
 * @brief  ModuCommandEvent 模块命令接收
 * @param  ModuType 模块执行命令类型
 * @param  pack 携带数据
 */
struct ModuCommandEvent {
    ModuMotionType ModuType;///< 命令类型
    QVariantMap pack;       ///< 携带数据
    // ModuleType Type;
};
/* =========================================================
 * 视觉发送广播事件
 * ========================================================= */
/**
 * @brief  VisionStatusEvent 视觉发送广播事件(视觉状态变化)
 * @param  VisionStatus 当前视觉状态
 * @param  timestamp 事件发送时间戳
 * @param  message 附带信息
 */
struct VisionStatusEvent {
    QDateTime timestamp;    ///< 时间戳
    VisionStatus Status;    ///< 当前视觉类状态
    QString message;        ///< 附带描述信息
};

/**
 * @brief  ComStatusEvent 视觉发送广播事件(视觉数据传递)
 * @param  DataType 数据类型
 * @param  pack 携带数据
 * @param  prcName 流程名称
 */
struct VisionDataEvent {
    QDateTime timestamp;    ///< 时间戳
    VisionDataType DataType;///< 数据类型
    QVariantMap pack;       ///< 携带数据
};

/**
 * @brief  VisionStatusEvent 视觉发送广播事件(信息回传)
 * @param  message 附带信息
 * @param  timestamp 事件发送时间戳
 */
struct VisionMessageEvent {
    QDateTime timestamp;    ///< 时间戳
    QString message;        ///< 附带描述信息
};


/* =========================================================
 * 通用数据结构（仅数据，不含逻辑）
 * ========================================================= */
/**
 * @brief 离散型点位数据
 * @param (X、Y、R) 坐标数据
*/
struct PointData {
    double x = 0;       ///< X 坐标。
    double y = 0;       ///< Y 坐标。
    double r = 0;       ///< 角度或旋转值。
};

/**
 * @brief 固定划分型点位数据
 * @param id 编号
 * @param isSuccess 状态 0:无、1:有、-1:异常
 * @param (X、Y、R) 坐标数据
*/
struct TargetData {
    int     id = -1;    ///< 目标编号。
    int     status = 0; ///< 目标状态。
    double  x = 0;      ///< X 坐标。
    double  y = 0;      ///< Y 坐标。
    double  r = 0;      ///< 角度或旋转值。
};



/* =========================================================
 * Qt 元类型注册（跨线程必备）
 * ========================================================= */

Q_DECLARE_METATYPE(PointData)
Q_DECLARE_METATYPE(TargetData)






