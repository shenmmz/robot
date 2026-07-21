/***************************************************************************************************
 * File：IVmTimeStatistic.h
 * Note：@~chinese 耗时统计模块接口声明 @~english Interface for the TimeStatistic tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_TIMESTATISTIC_H_
#define _IVM_TIMESTATISTIC_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace TimeStatisticModule
    {
    /// \addtogroup 耗时统计
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the TimeStatisticParams.  */
        ///耗时统计参数类
        class TimeStatisticParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit TimeStatisticParams() {}
            virtual ~TimeStatisticParams() {}

        public:

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the TimeStatisticResults.  */
        ///耗时统计结果类
        class TimeStatisticResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit TimeStatisticResults() {}
            virtual ~TimeStatisticResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 获取Time
             * @deprecated V4.4废弃
             */
            virtual float GetTime() = 0;

            /**
            * @brief 获取浮点型输出
            * @param strParamName [IN] 参数名称
            * @return 浮点型数组，出错时抛出异常
            */
            virtual FloatDataArray GetOutputFloat(IN const char * const strParamName) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the TimeStatistic tool.  */
        ///耗时统计工具类
        class TimeStatisticModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit TimeStatisticModuleTool() {}
            virtual ~TimeStatisticModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual TimeStatisticParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual TimeStatisticResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            TimeStatisticModuleTool(TimeStatisticModuleTool&);
            TimeStatisticModuleTool& operator=(TimeStatisticModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for TimeStatistic.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::TimeStatisticModule::TimeStatisticModuleTool * __stdcall GetTimeStatisticToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_TIMESTATISTIC_H_
