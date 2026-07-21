/***************************************************************************************************
 *
 * 版权信息：版权所有 (c) 2020, 保留所有权利
 *
 * 文件名称： VmObjectBase.h
 * 摘    要: VM对象基础类声明
 *
 * 当前版本：4.4.0
 *
 * 日    期：2020-10-29 19:24:30
 * 备    注：新建
 ****************************************************************************************************/
#ifndef CVMOBJECTBASE_H__ // 2020-10-29 19:24:30
#define CVMOBJECTBASE_H__

#pragma warning(disable:4819)
#include <iostream>
#include <set>
#include <map>
#include "iMVS-6000PlatformSDKDefine.h"
#include "VmModuleResultBase.h"
//#include "EventManager.h"

namespace VisionMasterSDK
{
    /** @class CSolution
     *  @brief
     */
	class IMVS_6000_PLATFORMSDK_API CVmObjectBase
    {
    public:

        /** @fn     explicit CVmObjectBase();
         *  @brief  构造函数,初始化成员变量
         *  @param  无
         *  @return 无
         */
        explicit CVmObjectBase();

        /** @fn     CVmObjectBase* Instance();
         *  @brief  单例对象指针
         *  @param  无
         *  @return 无
         */
        static CVmObjectBase * Instance();

        /** @fn     DestroyHandle();
        *  @brief  销毁句柄
        *  @return 无
        */
        int DestroyHandle();

        /** @fn     virtual ~CSolution();
         *  @brief  析构函数,销毁成员变量
         *  @param  无
         *  @return 无
         */
        virtual ~CVmObjectBase();

        /** @fn     SetBackupHandle();
         *  @brief  设置备份句柄
         *  @param  pCtrlHandle
         *  @return 无
         */
        void SetBackupHandle(void * pCtrlHandle);

    private:

        /** @fn     CallBackModuRes
         *  @brief  回调函数
         *  @param  无
         *  @return 无
         */
        static int __stdcall CallBackModuRes(IMVS_PF_OUTPUT_PLATFORM_INFO * const pstOutputPlatformInfo, void * const pUser);

        /** @fn     CallBackModuResFunc
         *  @brief  回调具体实现函数
         *  @param  无
         *  @return 无
         */
        int CallBackModuResFunc(IMVS_PF_OUTPUT_PLATFORM_INFO * const pstOutputPlatformInfo);

    public:
        void * mhandle;
        bool mbCrash;
        std::map<unsigned int, CModuleResultBasePtr> mModuleResultMap;
        bool bIsContinuRun; // 后续改用map管理所有流程
        void * mBackupHandle; // 备份自身创建的句柄，为控件备份
        bool mbIsBackup; // 是否备份
        bool mbSolRunFlag; // 方案执行标志
        //EventObject m_SolutionExecuteEvent;
        //EventObject m_ProcessExecuteEvent;

        unsigned int m_nIsExecuteNormal[MAX_PROCESS_NUM]; // 流程内模块是否全部执行正常标识

    public:
        int(__stdcall * m_cbObjInfo)(OUT OutputPlatformInfo * const pstOutputPlatformInfo, IN void * const pUser); // 以数据类型方式回调模块结果数据
        void * m_pObjUser; // 回调函数pUser

        void* m_pSolutionObj;
        //std::map<unsigned int, void*> m_mapProcedureBuff;
    };
}

#endif // CVMOBJECTBASE_H__