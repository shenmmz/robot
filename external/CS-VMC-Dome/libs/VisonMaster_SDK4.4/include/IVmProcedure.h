/***************************************************************************************************
 * File：IVmProcedure.h
 * Note：Interface for the procedure tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_PROCEDURE_H_
#define _IVM_PROCEDURE_H_

#pragma warning(disable:4819)

#include "Variable.h"
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"
#include "IVmDynamicIoDefine.h"


using namespace VisionMasterSDK::VariableModule;

namespace VisionMasterSDK
{
    namespace VmProcedure
    {
    /// \addtogroup 流程
    /// @{
        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the IMVSProcedureResults.  */
        ///流程结果类
        class IMVSProcedureResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSProcedureResults() {}
            virtual ~IMVSProcedureResults() {}

        public:
            /**
            * @brief 获取整型输入
            * @param strParamName  [IN]  参数名称
            * @return 整型数组
            */
            virtual IntDataArray GetOutputInt(IN const char * const strParamName) = 0;

            /**
            * @brief 获取浮点型输入
            * @param strParamName  [IN]  参数名称
            * @return 浮点型数组
            */
            virtual FloatDataArray GetOutputFloat(IN const char * const strParamName) = 0;

            /**
            * @brief 获取字符串型输入
            * @param strParamName  [IN]  参数名称
            * @return 字符串型数组
            */
            virtual StringDataArray GetOutputString(IN const char * const strParamName) = 0;

            /**
            * @brief 获取二进制数据型输入
            * @param strParamName  [IN]  参数名称
            * @return 二进制数据
            */
            virtual ByteArrayData GetOutputByteArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取图像结果（整个图像）
            * @param strParamName  [IN]  参数名称
            * @return 图像结果
            */
            virtual IoImage GetOutputImageV2(IN const char * const strParamName) = 0;

            /**
            * @brief 获取点输出数组
            * @param strParamName  [IN]  参数名称
            * @return 点输出数组
            */
            virtual IoPointArray GetOutputPointArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取直线输出数组
            * @param strParamName  [IN]  参数名称
            * @return 直线输出数组
            */
            virtual IoLineArray GetOutputLineArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取圆输出数组
            * @param strParamName  [IN]  参数名称
            * @return 圆输出数组
            */
            virtual IoCircleArray GetOutputCircleArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取椭圆输出数组
            * @param strParamName  [IN]  参数名称
            */
			virtual IoEllipseArray GetOutputEllipseArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取带角度矩形输出数组
            * @param strParamName  [IN]  参数名称
            * @return 带角度矩形输出数组
            */
            virtual IoRectBoxArray GetOutputBoxArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取无角度矩形（浮点型）输出数组
            * @param strParamName  [IN]  参数名称
            * @return 无角度矩形（浮点型）输出数组
            */
            virtual IoRectFArray GetOutputRectArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取位置修正输出数组
            * @param strParamName  [IN]  参数名称
            * @return 位置修正输出数组
            */
            virtual IoFixtureArray GetOutputFixtureArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取圆环输出数组
            * @param strParamName  [IN]  参数名称
            * @return 圆环输出数组
            */
            virtual IoAnnulusArray GetOutputAnnulusArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取类别信息输出数组
            * @param strParamName  [IN]  参数名称
            * @return 类别信息输出数组
            */
            virtual IoClassInfoArray GetOutputClassInfoArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取带类别信息的图（界面可根据类别信息将灰度图显示成彩图）
            * @param strParamName  [IN]  参数名称
            * @return 带类别信息的图
            */
            virtual IoPixelImage GetOutputPixelImage(IN const char * const strParamName) = 0;

            /**
            * @brief 获取位姿数组
            * @param strParamName  [IN]  参数名称
            * @return 位姿数组
            */
            virtual IoPostureArray GetOutputPostureArray(IN const char * const strParamName) = 0;
            /**
            * @brief 获取多边形输出数组
            * @param strParamName  [IN]  参数名称
            * @return 多边形输出数组
            */
            virtual IoPolygonArray GetOutputPolygonArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取所有输出名称信息（CONTOURPOINTS、POINTSET这两种类型不支持）
            * @return 所有输出名称信息
            */
            virtual IoNameInfoArray GetAllOutputNameInfo() = 0;

            /**
            * @brief 根据名称信息获取输出对象基类指针
            * @param stNameInfo   [IN]  所有输出名称信息
            * @return 输出对象基类指针
            */
            virtual VmIoBase* GetOutputObject(IN const IoNameInfo& stNameInfo) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the IMVSProcedureParams.  */
        ///流程参数类
        class IMVSProcedureParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSProcedureParams() {}
            virtual ~IMVSProcedureParams() {}

        public:

            /**
            *  @brief  设置动态输入图像数据（仅当次执行起效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstImageData  [IN] IoImage结构体指针数组
            */
            virtual void SetInputImageV2(const char * const strParamName, const IoImage * const pstImageData) = 0;

            /**
            *  @brief  设置动态输入矩形框数据（仅当次执行起效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstBox        [IN] IoRectBoxArray结构体指针数组
            */
            virtual void SetInputBox(const char * const strParamName, const IoRectBoxArray * const pstBox) = 0;

            /**
            *  @brief  设置动态输入圆环ROI数据（仅当次执行起效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstAnnulus    [IN] IoAnnulusArray结构体指针数组
            */
            virtual void SetInputAnnulus(const char * const strParamName, const IoAnnulusArray * const pstAnnulus) = 0;

            /**
            *  @brief  设置动态输入圆形数据（仅当次执行起效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstCircle     [IN] IoCircleArray结构体指针数组
            */
            virtual void SetInputCircle(const char * const strParamName, const IoCircleArray * const pstCircle) = 0;

            /**
            *  @brief  设置动态输入椭圆形数据（仅当次执行起效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstCircle     [IN] IoCircleArray结构体指针数组
            */
			virtual void SetInputEllipse(const char * const strParamName, const IoEllipseArray * const pstEllipse) = 0;


            /**
            *  @brief  设置动态输入线型数据（仅当次执行起效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstLine       [IN] IoLineArray结构体指针数组
            */
            virtual void SetInputLine(const char * const strParamName, const IoLineArray * const pstLine) = 0;

            /**
            *  @brief  设置动态输入点型数据（仅当次执行起效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstPointF     [IN] IoPointArray结构体指针数组
            */
            virtual void SetInputPoint(const char * const strParamName, const IoPointArray * const pstPointF) = 0;

            /**
            *  @brief  设置动态输入位置修正数据（仅当次执行起效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstFixture    [IN] IoFixtureArray结构体指针数组
            */
            virtual void SetInputFixture(const char * const strParamName, const IoFixtureArray * const pstFixture) = 0;

            /**
            *  @brief  设置动态输入矩形数据（仅当次执行起效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstRectF      [IN] IoRectFArray结构体指针数组
            */
            virtual void SetInputRect(const char * const strParamName, const IoRectFArray * const pstRectF) = 0;

            /**
            *  @brief  设置动态输入类别信息数据（仅当次执行起效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstClassInfo  [IN] IoClassInfoArray结构体指针数组
            */
            virtual void SetInputClassInfo(const char * const strParamName, const IoClassInfoArray * const pstClassInfo) = 0;

            /**
            *  @brief  设置动态输入像素图像数据（仅当次执行起效）
            *  @param  strParamName   [IN] 参数名称
            *  @param  pstPixelImage  [IN] IoPixelImage结构体指针数组
            */
            virtual void SetInputPixelImage(const char * const strParamName, const IoPixelImage * const pstPixelImage) = 0;
            /**
            *  @brief  设置动态输入多边形数据（仅当次执行起效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstPolygon    [IN] IoPolygonArray结构体指针数组
            */
            virtual void SetInputPolygon(const char * const strParamName, const IoPolygonArray * const pstPolygon) = 0;
            /**
            *  @brief  设置动态输入位姿数据（仅当次执行起效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstPosture    [IN] IoPostureArray结构体指针数组
            */
            virtual void SetInputPosture(const char * const strParamName, const IoPostureArray * const pstPosture) = 0;
            /**
            *  @brief  获取所有输入名称信息
            */
			virtual IoNameInfoArray GetAllInputNameInfo() = 0;
        };
        /*************************** Params Class End ***************************/

        /*************************** Tool Class Start ***************************/
        ///流程事件类
        class IVmProcedureEvent
        {
        public:
            /**
            * @brief 流程开始执行状态回调
            * @param pstWorkStatus [IN] 流程状态
            * @param pUser [IN] 用户指针
            */
            virtual void OnWorkBeginStatusCallBack(IN const IMVS_PF_MODULE_WORK_STAUS * const pstWorkStatus, IN void * const pUser) = 0;

            /**
            * @brief 流程结束执行状态回调
            * @param pstWorkStatus [IN] 流程状态
            * @param pUser [IN] 用户指针
            */
            virtual void OnWorkEndStatusCallBack(IN const IMVS_PF_MODULE_WORK_STAUS * const pstWorkStatus, IN void * const pUser) = 0;
        };

        ///流程类
        class IVmProcedure : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IVmProcedure() {}
            virtual ~IVmProcedure() {}

        public:
            /**
            * @brief 执行一次
            * @param strCmd  [IN] 触发字符串
            * @param bIsWait [IN] 是否等待执行结束
            * @param bIsRespondToUI [IN] 接口执行期间是否尝试响应界面操作，在UI线程中设置为true可避免阻塞界面，在非UI线程中设置为false可减小耗时波动
            * @return 无，出错时抛出异常
            */
            virtual void Run(IN const char * const strCmd = NULL, IN bool bIsWait = true, IN bool bIsRespondToUI = true) = 0;

            /**
            * @brief 执行一次
            * @param pnRunCount  [OUT]		当前执行次数
            * @param strCmd      [IN]		触发字符串
            * @param bIsWait     [IN]		是否等待执行结束
            * @param bIsRespondToUI [IN] 接口执行期间是否尝试响应界面操作，在UI线程中设置为true可避免阻塞界面，在非UI线程中设置为false可减小耗时波动
            * @return 无，出错时抛出异常
            */
            virtual void Run(OUT unsigned int * const pnRunCount, IN const char * const strCmd = NULL, IN bool bIsWait = true, IN bool bIsRespondToUI = true) = 0;

            /**
            * @brief 执行一次点对点模式
            * @param nTimeout  [IN]  超时时间
            * @param bIsRespondToUI [IN] 接口执行期间是否尝试响应界面操作，在UI线程中设置为true可避免阻塞界面，在非UI线程中设置为false可减小耗时波动
            * @return 无，出错时抛出异常
            */
            virtual void RunP2P(IN unsigned int nTimeout, IN bool bIsRespondToUI = true) = 0;

            /**
            * @brief 连续运行
            * @return 无，出错时抛出异常
            * @remarks 流程连续运行
            */
            virtual void Runing(void) = 0;

            /**
            * @brief 停止连续运行
            * @param nWaitTime  [IN]  等待超时（单位：秒）
            * @return 无，出错时抛出异常
            * @remarks 流程停止连续运行
            */
            virtual void StopRun(IN const unsigned int nWaitTime = 3) = 0;

            /**
            * @brief 设置连续运行时间间隔
            * @param nMilliSecond  [IN]   时间间隔（单位：ms）
            * @return 无，出错时抛出异常
            * @remarks 设置流程连续运行时间间隔
            */
            virtual void SetRunInterval(IN const unsigned int nMilliSecond) = 0;

            /**
            * @brief 禁用流程
            */
            virtual void DisableProcedure(void) = 0;

            /**
            * @brief 启用流程
            */
            virtual void EnableProcedure(void) = 0;

            /**
            * @brief 流程是否禁用
            * @return 流程禁用返回true，未禁用返回false
            */
            virtual bool IsProcedureDisable(void) = 0;

            /**
            * @brief 保存流程至文件
            * @param strPrcPath  [IN]  流程路径
            * @param strPassword [IN]  流程文件密码（预留）
            * @return 无，出错时抛出异常
            * @deprecated 弃用,推荐(SaveAsProcedure(const char * const strPrcPath, const char * const strPassword)
            */

            virtual void ExportProcess(IN const char * const strPrcPath, IN const char * const strPassword = NULL) = 0;

            /**
            * @brief 保存流程至文件
            * @param strPrcPath [IN] 流程路径
            * @param strPassword [IN] 流程文件密码（预留）
            * @return 无，出错时抛出异常
            */
            virtual void SaveAsProcedure(IN const char * const strPrcPath, IN const char * const strPassword = NULL) = 0;

            /**
            * @brief 获取所有模块信息列表
            * @return 所有模块信息列表
            */
            virtual ModuleInfoList * GetAllModuleList(void) = 0;

            /**
            * @brief 获取流程第一层级模块列表（不包含Group内部模块）
            * @return 流程第一层级模块列表
            */
            virtual ModuleInfoList * GetProcedureModuleList(void) = 0;

            /**
            * @brief 设置流程运行策略接口
            * @param pstRunPolicyInfo [IN] 流程运行策略信息
            */
            virtual void SetProcedureRunPolicy(IN const ProcedureRunPolicy * const pstRunPolicyInfo) = 0;

            /**
            * @brief 注册流程回调事件对象
            * @param pProcedureEvent [IN] 事件对象指针
            * @param pUser           [IN] 用户指针，注册时传入，回调时返回
            * @return 无，出错时抛出异常
            * @remarks 回调处理函数为子线程，函数中如需操作界面，建议使用自定义消息异步(MFC)或信号槽(QT)
            */
            virtual void RegisterCallBackEvent(IN IVmProcedureEvent* pProcedureEvent, IN void * const pUser) = 0;

            /**
            * @brief 解注册流程回调事件对象
            */
            virtual void UnRegisterCallBackEvent() = 0;

            /**
            * @brief 获取结果输出配置
            * @param strParam  [IN]  输出配置参数
            * @return 整型结果
            * @deprecated V4.2废弃
            */
            virtual IntResultInfo * GetIntOutputResult(IN const char * const strParam) = 0;

            /**
            * @brief 获取结果输出配置
            * @param strParam  [IN] 输出配置参数
            * @return 浮点型结果
            * @deprecated V4.2废弃
            */
            virtual FloatResultInfo * GetFloatOutputResult(IN const char * const strParam) = 0;

            /**
            * @brief 获取结果输出配置
            * @param strParam  [IN] 输出配置参数
            * @return 字符串型结果
            * @deprecated V4.2废弃
            */
            virtual StringResultInfo * GetStringOutputResult(IN const char * const strParam) = 0;

            /**
            * @brief 获取结果输出配置
            * @param strParam  [IN] 输出配置参数
            * @return 图像像素结果
            * @deprecated V4.2废弃
            */
            virtual ImageResultInfo * GetImageOutputResult(IN const char * const strParam) = 0;

            /**
            * @brief 获取结果输出配置
            * @param strParam  [IN] 输出配置参数
            * @return 点集型结果
            * @deprecated V4.2废弃
            */
            virtual PointsetResultInfo * GetPointsetOutputResult(IN const char * const strParam) = 0;

            /**
            * @brief 获取参数对象
            * @return 失败返回NULL
            * @remarks 通过参数对象可设置获取模块参数
            */
            virtual const char * GetName(void) = 0;

            /**
            * @brief 获取内部所需信息(预留)
            * @return 流程内部信息
            */
            virtual void * GetControlInfo(void) = 0;

            /**
            * @brief 获取参数对象
            * @return 失败返回NULL
            * @remarks 通过参数对象可设置获取模块参数
            */
            virtual IMVSProcedureParams* GetParamObj() = 0;

            /**
            * @brief 查找结果
            * @return 未执行或执行失败返回NULL
            */
            virtual IMVSProcedureResults* GetResult() = 0;

            /**
            * @brief 获取流程内模块是否全部执行正常
            * @return 是否正常 1正常 0异常 默认为1
            */
            virtual unsigned int GetIsExecuteNormal(void) = 0;

            /**
            * @brief 获取流程内模块运行错误信息
            * @return 错误信息，数量上限16个
            */
            virtual ModuRunErrorInfoList* GetModuErrorInfoList(void) = 0;

            /**
            * @brief 获取局部变量
            * @return 局部变量工具指针
            */
            virtual CVariable* GetLocalVariable() = 0;

        public:
            /**
            * @brief 根据名称获取模块对象
            * @param strKey  [IN]  模块显示名称
            * @return 模块对象指针
            */
            virtual void * operator[] (IN const char * const strKey) = 0;
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}

/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

    /*  Note: Create tool instance for procedure. 
        brief: EN: Password parameter is reserved. | CH: 密码参数预留，暂不支持 */
    /* CH: 弃用,推荐(LoadProcedure(const char * const strPrcPath, const char * const strPassword, bool bIsOverrideGlobal = true)) | EN: Obsolete,recommend(LoadProcedure(const char * const strPrcPath, const char * const strPassword, bool bIsOverrideGlobal = true)) */
	IMVS_6000_PLATFORMSDK_API VisionMasterSDK::VmProcedure::IVmProcedure * __stdcall ImportProcess(IN const char * const strPrcPath, IN const char * const strPassword = NULL);
    /*  Note: Create tool instance for procedure.
    brief: EN: Password parameter is reserved. | CH: 密码参数预留，暂不支持 */
	IMVS_6000_PLATFORMSDK_API VisionMasterSDK::VmProcedure::IVmProcedure * __stdcall LoadProcedure(IN const char * const strPrcPath, IN const char * const strPassword = NULL, IN bool bIsOverrideGlobal = true);
    /*  Note: Destroy tool instance for procedure.  */
    IMVS_6000_PLATFORMSDK_API void __stdcall DestroyProcedureInstance(IN VisionMasterSDK::VmProcedure::IVmProcedure* pToolInstance);

#ifdef __cplusplus
}
#endif 

#endif    ///< _IVM_PROCEDURE_H_