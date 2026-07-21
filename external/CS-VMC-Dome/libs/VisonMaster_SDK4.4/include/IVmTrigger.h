/***************************************************************************************************
 * File：IVmTrigger.h
 * Note：@~chinese 触发模块模块接口声明 @~english Interface for the Trigger tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_TRIGGER_H_
#define _IVM_TRIGGER_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace TriggerModule
    {
    /// \addtogroup 触发模块
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the TriggerParams.  */
        ///触发模块参数类
        class TriggerParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit TriggerParams() {}
            virtual ~TriggerParams() {}

        public:

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the TriggerResults.  */
        ///触发模块结果类
        class TriggerResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit TriggerResults() {}
            virtual ~TriggerResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the Trigger tool.  */
        ///触发模块工具类
        class TriggerModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit TriggerModuleTool() {}
            virtual ~TriggerModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual TriggerParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual TriggerResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            TriggerModuleTool(TriggerModuleTool&);
            TriggerModuleTool& operator=(TriggerModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for Trigger.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::TriggerModule::TriggerModuleTool * __stdcall GetTriggerToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_TRIGGER_H_
