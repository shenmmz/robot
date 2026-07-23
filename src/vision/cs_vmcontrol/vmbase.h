#ifndef VMBASE_H
#define VMBASE_H
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVector>
#include "IVmSolution.h"
#include "IVmProcedure.h"
#include "IVmExport.h"
#include "IVmCircleFind.h"
#include "IVmLineFind.h"
#include "IVmFastFeatureMatch.h"
#include "IVmFormat.h"
#include "IVmImageSource.h"
#include "IVmHPFeatureMatch.h"
#include "IVmIfBranch.h"
#include "IVmCalibTransform.h"
#include "IVmQuadrangleFind.h"
#include "IVmGlobalCamera.h"
#include "IVmFixture.h"
#include "IVmCalculator.h"
#include "VMException.h"
#include "IVmAnd.h"
#include "IVmImageBinary.h"
#include "IVmContourMatch.h"
#include "IVmGroup.h"
#include "IVmCalibBoardCalib.h"
#include "IVmCaliperCorner.h"
#include "IVmCaliperEdge.h"
#include "VmControlBaseInfo.h"

using namespace VisionMasterSDK;
using namespace VisionMasterSDK::VmSolution;
using namespace VisionMasterSDK::VmProcedure;


/**
 * @brief VMbase 通用返回结构。
 *
 * 所有 SDK 封装函数通过该结构统一返回执行状态、可读消息和可选数据包。
 */
struct VMbaseInfo {
    bool success;       ///< 本次操作是否成功。
    QString message;    ///< 操作结果说明或错误信息。
    QVariantMap pack;   ///< 附加输出数据，例如流程列表、控件句柄、结果对象等。
};


/**
 * @brief VisionMaster SDK 的基础封装类。
 *
 * 该类直接面向 SDK 接口，负责：
 * 1. 方案实例的加载、保存、关闭与运行。
 * 2. 流程对象的获取、执行、停止和结果读取。
 * 3. 模块参数页所需句柄数据的整理与返回。
 * 4. SDK 回调结果的提取，并通过 Qt 信号转发给上层业务类。
 */
class VMbase: public QObject, public IVmProcedureEvent
{
    Q_OBJECT

public:
    /**
     * @brief 默认构造函数。
     *
     * 输入数据：无。
     * 输出数据：创建未加载方案的 VMbase 对象。
     */
    VMbase();

    /**
     * @brief 构造 VMbase 并设置 Qt 父对象。
     * @param parent Qt 父对象。
     */
    explicit VMbase(QObject *parent = nullptr);

    /**
     * @brief 析构 VMbase 并释放 SDK 方案实例及资源。
     */
    virtual ~VMbase();

    // -------***** VM方案 *****-------
    /**
    * @brief 加载视觉方案，为解决方案创建工具实例。
    * @param solPath 方案路径。
    * @param password 方案密码。
    * @return success 表示是否加载成功，message 返回加载结果。
    * @remarks 未加载方案则无法执行流程、模块和结果读取函数。
    */
    virtual VMbaseInfo loadSol(const QString& solPath, const QString& password);

    /**
    * @brief 检查当前方案是否已经加载。
    * @return true 表示 SDK 方案实例存在，false 表示未加载。
    */
    virtual bool readSol();

    /**
    * @brief 将当前方案另存为新文件。
    * @param solPath 新方案路径。
    * @param password 新方案密码。
    * @return success 表示保存结果，pack["solPath"] 返回另存为路径。
    * @remarks 将解决方案另存为新文件，同时源文件不保存改动。
    */
    virtual VMbaseInfo SaveAsSol(const QString& solPath, const QString& password);

    /**
    * @brief 保存视觉方案到源文件。
    * @return success 表示保存结果，message 返回 SDK 执行信息。
    */
    virtual VMbaseInfo SaveSol();

    /**
    * @brief 关闭当前解决方案。
    * @return success 表示关闭结果，message 返回 SDK 执行信息。
    */
    virtual VMbaseInfo closeSol();

    /**
    * @brief 整套方案执行一次。
    * @return success 表示执行调用结果，message 返回 SDK 执行信息。
    */
    virtual VMbaseInfo runSol();

    /**
    * @brief 整套方案连续执行。
    * @return success 表示启动结果，message 返回 SDK 执行信息。
    */
    virtual VMbaseInfo runingSol();

    /**
    * @brief 停止方案执行。
    * @return success 表示停止结果，message 返回 SDK 执行信息。
    */
    virtual VMbaseInfo stopRunSol();

    /**
    * @brief 获取当前方案中的所有流程信息列表。
    * @return pack["number"] 返回流程数量，pack["groupNameN"] 返回每个流程名称。
    */
    virtual VMbaseInfo getprocedureObjList();

    /**
    * @brief 启用所有模块结果回调。
    * @return success 表示启用结果，message 返回 SDK 执行信息。
    */
    virtual VMbaseInfo EnableCallback();

    /**
    * @brief 禁用所有模块结果回调。
    * @return success 表示禁用结果，message 返回 SDK 执行信息。
    * @remarks 禁用后要把需要的模块回调使能单独打开 CxxxTool.EnableResultCallback() ，否则无法再回调函数中拿到模块结果
    */
    virtual VMbaseInfo DisableCallback();

    // -------***** VM流程 *****-------

    /**
     * @brief 使用绝对路径加载流程文件。
     * @param procPath 流程文件绝对路径。
     * @return pack["IVmProcedure"] 返回流程对象指针。
     */
    virtual VMbaseInfo loadProc(const QString& procPath);


    /**
    * @brief 根据流程名称获取流程对象。
    * @param processName 流程名称，必须与 VM 方案中的流程显示名一致。
    * @return pack["IVmProcedure"] 返回流程对象指针。
    */
    virtual VMbaseInfo getIVmProcedure(QString processName);

    /**
    * @brief 流程执行一次。
    * @param processName 流程名称。
    * @return pack["RenderData"] 返回流程渲染控件句柄。
    */
    virtual VMbaseInfo runPrc(QString processName);

    /**
    * @brief 流程连续执行。
    * @param processName 流程名称。
    * @return pack["RenderData"] 返回流程渲染控件句柄。
    */
    virtual VMbaseInfo runningPrc(QString processName);

    /**
    * @brief 停止流程执行。
    * @param processName 流程名称。
    * @return pack["processName"] 返回被停止的流程名。
    */
    virtual VMbaseInfo stopPrc(QString processName);

    /**
    * @brief 获取流程参数页所需控制句柄。
    * @param processName 流程名称。
    * @return pack["qlonglongData"] 返回参数控件可用的 ParamBaseEx 地址。
    */
    virtual VMbaseInfo setProcess(QString processName);


    /**
    * @brief 获取流程结果对象。
    * @param processName 流程名称。
    * @return pack["results"] 返回 IMVSProcedureResults 指针。
    */
    virtual VMbaseInfo getResult(QString processName);

    /**
     * @brief SDK工作开始回调
     * @param status 工作状态结构体指针
     * @param user 用户自定义数据指针（实际为IVmProcedure指针）
     */
    virtual void OnWorkBeginStatusCallBack(const IMVS_PF_MODULE_WORK_STAUS *status, void *user) override;

    /**
     * @brief SDK工作结束回调
     * @param status 工作状态结构体指针
     * @param user 用户自定义数据指针（实际为IVmProcedure指针）
     */
    virtual void OnWorkEndStatusCallBack(const IMVS_PF_MODULE_WORK_STAUS *status, void *user) override;


    // -------***** VM模块 *****-------

    /**
    * @brief 获取流程直属模块参数页所需控制句柄。
    * @param processName 流程名称。
    * @param moduName 模块名称。
    * @return pack["setData"] 返回参数控件可用的 ParamBaseEx 地址。
    */
    virtual VMbaseInfo ModuSet(
        const QString& processName
        , const QString& moduName);

    /**
    * @brief 获取组合模块内部模块参数页所需控制句柄。
    * @param processName 流程名称。
    * @param groupName 组合模块名称。
    * @param moduName 组合模块内部的模块名称。
    * @return pack["setData"] 返回参数控件可用的 ParamBaseEx 地址。
    */
    virtual VMbaseInfo GroupSet(
        const QString& processName
        , const QString& groupName
        , const QString& moduName);


signals:
    /**
     * @brief 流程开始回调信号。
     * @param pack SDK 回调数据包。
     */
    void processBeginCallBack(const QVariantMap& pack);

    /**
     * @brief 流程结束回调信号。
     * @param pack SDK 结果数据包。
     */
    void processEndCallBack(const QVariantMap& pack);

private:
    /**  注意：为空解决方案创建工具实例。此解决方案中没有过程。*/
    VisionMasterSDK::VmSolution::IVmSolution * pVmSol  = nullptr;
    bool m_solutionValid = false;           // 标记解决方案是否有效加载
    QString m_lastError;                    // 记录最后错误信息

    ParamBaseEx stParamData;    ///< 参数结构体成员变量，避免局部变量地址失效
};

#endif // VMBASE_H
