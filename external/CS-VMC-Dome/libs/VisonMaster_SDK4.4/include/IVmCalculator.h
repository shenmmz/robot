/***************************************************************************************************
 * File：IVmCalculator.h
 * Note：@~chinese 变量计算模块接口声明 @~english Interface for the Calculator tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CALCULATOR_H_
#define _IVM_CALCULATOR_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace CalculatorModule
    {
    /// \addtogroup 变量计算
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the CalculatorParams.  */
        ///变量计算参数类
        class CalculatorParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CalculatorParams() {}
            virtual ~CalculatorParams() {}

        public:

            /// @~chinese 重置   @~english Reset
            virtual int DoResetValue() = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the CalculatorResults.  */
        ///变量计算结果类
        class CalculatorResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CalculatorResults() {}
            virtual ~CalculatorResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 获取结果显示
             */
            /* Gets information of the ResultShow. */
            virtual const char * GetResultShow() = 0;
            /**
             * @brief 结果数量
             */
            virtual int GetResultNum() = 0;
            /**
             * @brief 获取结果显示
             */
            virtual CalOutputResultInfo * GetResult(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the Calculator tool.  */
        ///变量计算工具类
        class CalculatorModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CalculatorModuleTool() {}
            virtual ~CalculatorModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CalculatorParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CalculatorResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            CalculatorModuleTool(CalculatorModuleTool&);
            CalculatorModuleTool& operator=(CalculatorModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for Calculator.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::CalculatorModule::CalculatorModuleTool * __stdcall GetCalculatorToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CALCULATOR_H_
