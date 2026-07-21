/***************************************************************************************************
 * File：IVmSolution.h
 * Note：Interface for the solution tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_SOLUTION_H_
#define _IVM_SOLUTION_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "iMVS-6000PlatformSDKDefine.h"
#include "IVmProcedure.h"

namespace VisionMasterSDK
{
    namespace VmSolution
    {
    /// \addtogroup 方案
    /// @{
        ///方案事件类
        class IVmSolutionEvent
        {
        public:
            /**
            * @brief 相机取图信息回调，取图结束时触发
            * @param pstCameraCollect    [IN]  相机取图信息
            * @param pUser               [IN]  用户指针
            * @return 无，出错时抛出异常
            */
            virtual void OnCameraCollectInfoCallBack(IN const IMVS_PF_CAMERA_COLLECT_INFO * const pstCameraCollect, IN void * const pUser) = 0;
            /**
            * @brief 相机取图信息回调，取图开始时触发
            * @param pstCameraCollect   [IN] 相机取图信息
            * @param pUser              [IN]  用户指针
            */
			virtual void OnCameraCollectStartInfoCallBack(IN const IMVS_PF_CAMERA_COLLECT_INFO * const pstCameraCollect, IN void * const pUser) = 0;

            /**
            * @brief 通信连接状态回调
            * @param pstCommuConnect [IN] 通信连接状态
            */
            virtual void OnCommunicationConnectInfoCallBack(IN const IMVS_PF_COMMUNICATION_CONNECT_INFO * const pstCommuConnect, IN void * const pUser) = 0;

            /**
            * @brief 处理相机连接状态回调
            * @param pstCameraConnectStatus [IN] 相机连接状态
            * @param pUser               [IN]  用户指针
            */
            virtual void OnCameraConnectStatusCallBack(IN const IMVS_PF_CAMERA_CONNECT_STATUS_INFO * const pstCameraConnectStatus, IN void * const pUser) = 0;
        };

        ///方案类
        class IVmSolution
        {
        public:
            ///< Constructor and Destructor
            explicit IVmSolution() {}
            virtual ~IVmSolution() {}

        public:
            /**
            * @brief 执行一次
            * @param strCmd    [IN]  触发字符串
            * @return 无，出错时抛出异常
            * @remarks 全方案执行一次
            */
            virtual void Run(IN const char * const strCmd = NULL) = 0;

            /**
            * @brief 连续运行
            * @return 无，出错时抛出异常
            * @remarks 全方案连续运行
            */
            virtual void Runing(void) = 0;

            /**
            * @brief 停止连续运行
            * @param nWaitTime   [IN]  等待超时（单位：秒）
            * @return 无，出错时抛出异常
            * @remarks 全方案停止连续运行
            */
            virtual void StopRun(IN const unsigned int nWaitTime = 3) = 0;

            /**
            * @brief 设置连续运行时间间隔
            * @param nMilliSecond   [IN]  时间间隔（单位：ms）
            * @return 无，出错时抛出异常
            * @remarks 设置全方案连续运行时间间隔
            */
            virtual void SetRunInterval(IN const unsigned int nMilliSecond) = 0;

            /**
            * @brief 保存方案至文件
            * @param  strSolPath   [IN]  方案路径
            * @param  strPassword  [IN]  方案密码
            * @return 无，出错时抛出异常
            * @deprecated V4.2版本开始弃用，推荐 SaveAsSolution(char * strSolPath, char * strPassword)
            */
            virtual void ExportSolution(IN const char * const strSolPath, IN const char * const strPassword) = 0;

            /**
            * @brief 保存方案至文件，使用最新路径和密码
            * @return 无，出错时抛出异常
            * @remarks 保存方案至文件，使用最新路径和密码
            */
            virtual void SaveSolution() = 0;

            /**
            * @brief 保存方案至文件
            * @param  strSolPath   [IN]  方案路径
            * @param  strPassword  [IN]  方案密码
            * @return 无，出错时抛出异常
            * @remarks 保存方案至文件
            */
            virtual void SaveAsSolution(IN const char * const strSolPath, IN const char * const strPassword) = 0;

            /**
            * @brief 方案是否含有密码
            * @return 有密码返回true，无密码返回false
            */
            virtual bool HasPassword(void) = 0;

            /**
            * @brief 获取当前方案版本
            * @return 当前方案版本
            */
            virtual const char * GetSolutionVersion(void) = 0;

            /**
            * @brief 删除一个流程
            * @param strProcedureName  [IN]  流程显示名称
            */
            virtual void DeleteOneProcedure(IN const char * const strProcedureName) = 0;

            /**
            * @brief 禁用一个流程
            * @param strProcedureName  [IN]  流程显示名称
            */
            virtual void DisableProcedure(IN const char * const strProcedureName) = 0;

            /**
            * @brief 启用一个流程
            * @param strProcedureName  [IN]  流程显示名称
            */
            virtual void EnableProcedure(IN const char * const strProcedureName) = 0;

            /**
            * @brief 流程是否禁用
            * @param strProcedureName  [IN]  流程显示名称
            * @return 流程禁用返回true，未禁用返回false
            */
            virtual bool IsProcedureDisable(IN const char * const strProcedureName) = 0;

            /**
            * @brief 获取所有流程信息列表
            * @return 所有流程信息列表
            */
            virtual ProcessInfoList * GetAllProcedureList(void) = 0;

            /**
            * @brief 获取所有流程对象列表
            * @return 所有流程对象列表
            */
            virtual ObjectList GetAllProcedureObjects(void) = 0;

            /**
            * @brief 获取所有模块信息列表
            * @return 所有模块信息列表
            */
            virtual ModuleInfoList * GetAllModuleList(void) = 0;

            /**
            * @brief 获取当前方案路径
            * @return 当前方案路径
            */
            virtual const char * GetSolutionPath(void) = 0;
            
            /**
            * @brief 获取当前方案句柄
            * @return 当前方案句柄
            */
            virtual void * GetSolutionHandle(void) = 0;

            /**
            * @brief 禁用所有模块结果回调
            * @remarks 禁用后要把需要的模块回调使能单独打开 CxxxTool.EnableResultCallback() ，否则无法再回调函数中拿到模块结果
            */
            virtual void DisableModulesCallback(void) = 0;

            /**
            * @brief 启用所有模块结果回调，方案加载后，默认启用所有模块回调
            * @remarks 启用后，在回调函数中能收到模块结果,过滤条件： IMVS_ENUM_CTRLC_OUTPUT_PLATFORM_INFO_MODULE_RESULT == pstOutputInfo->nInfoType \n
                       模块结果： IMVS_PF_MODULE_RESULT_INFO_LIST* pstModuResultList = (IMVS_PF_MODULE_RESULT_INFO_LIST *)pstOutputInfo->pData;
            */
            virtual void EnableModulesCallback(void) = 0;

            /**
             * @brief 注册回调函数
             * @param cbOutputInfo [IN] 回调函数指针
             * @param pUser        [IN] 用户指针，注册时传入，回调时返回
             * @remarks 回调处理函数为子线程，函数中如需操作界面，建议使用自定义消息异步(MFC)或信号槽(QT)
             */
            virtual void RegisterCallBack(IN int(__stdcall * cbOutputInfo)(OUT OutputPlatformInfo * const pstOutputInfo, IN void * const pUser), IN void * const pUser) = 0;

            /**
            * @brief 解注册回调函数
            */
            virtual void UnRegisterCallBack() = 0;

            /**
            * @brief 注册方案回调事件对象
            * @param pSolutionEvent [IN] 事件对象指针
            * @param pUser          [IN]  用户指针，注册时传入，回调时返回
            * @remarks 回调处理函数为子线程，函数中如需操作界面，建议使用自定义消息异步（MFC）或信号槽（QT）
            */
            virtual void RegisterCallBackEvent(IN IVmSolutionEvent* pSolutionEvent, IN void * const pUser) = 0;

            /**
            * @brief 解注册方案回调事件对象
            * @return 无，出错时抛出异常
            */
            virtual void UnRegisterCallBackEvent() = 0;

            /**
            * @brief 设置模块结果缓存配置
            * @param bBufferEnable [IN]   缓存使能
            * @param nBufferNum    [IN]   缓存结果数量
            * @return 无，出错时抛出异常
            */
            virtual void SetModuleResultBuffer(IN bool bBufferEnable, IN unsigned int nBufferNum) = 0;

            /**
            * @brief 获取模块结果缓存配置
            * @param pbBufferEnable [IN]  缓存使能
            * @param pnBufferNum    [IN]  缓存结果数量
            * @return 无，出错时抛出异常
            */
            virtual void GetModuleResultBuffer(OUT bool * const pbBufferEnable, OUT unsigned int * const pnBufferNum) = 0;

        public:
            /**
            * @brief 根据名称获取流程模块对象
            * @param strKey  [IN]  模块流程显示名称
            * @return 流程模块对象指针
            */
            virtual void * operator[] (IN const char * const strKey) = 0;
        };
    /// @}
    }
}

/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

    /* CH: 弃用,推荐(LoadSolution(char * strSolPath, char * strPassword, bool bSilentRun)) | EN: Obsolete,recommend(LoadSolution(char * strSolPath, char * strPassword, bool bSilentRun)) */
    /*  Note: Create tool instance for solution.  */
    IMVS_6000_PLATFORMSDK_API VisionMasterSDK::VmSolution::IVmSolution * __stdcall ImportSolution(IN const char * const strSolPath, IN const char * const strPassword, IN bool bSilentRun = false);

    /*  Note: Create tool instance for solution.  */
    IMVS_6000_PLATFORMSDK_API VisionMasterSDK::VmSolution::IVmSolution * __stdcall LoadSolution(IN const char * const strSolPath, IN const char * const strPassword, IN bool bSilentRun = false);

    /*  Note: Create tool instance for empty solution. This solution has no procedure.  */
    IMVS_6000_PLATFORMSDK_API VisionMasterSDK::VmSolution::IVmSolution * __stdcall CreateSolutionInstance(IN void * pCtrlHandle = NULL);
    
    /*  Note: Get tool instance for existed solution.  */
    IMVS_6000_PLATFORMSDK_API VisionMasterSDK::VmSolution::IVmSolution * __stdcall GetSolutionExistedInstance();

    /*  Note: Destroy tool instance for solution.  */
    IMVS_6000_PLATFORMSDK_API void __stdcall DestroySolutionInstance(IN VisionMasterSDK::VmSolution::IVmSolution* pToolInstance);

    /*  Note: Get information of dongle. Currently only used to check dongle.  */
    IMVS_6000_PLATFORMSDK_API void __stdcall GetDongleInfo();

    /*  Note: Set server path.  */
    IMVS_6000_PLATFORMSDK_API void __stdcall SetServerPath(IN const char * const strServerPath);

    /*  Note: DeInit and destroy resources. Execute only once before exiting the program. Avoid calling in destructor */
    IMVS_6000_PLATFORMSDK_API void __stdcall DisposeResource();
#ifdef __cplusplus
}
#endif 

#endif    ///< _IVM_SOLUTION_H_