/***************************************************************************************************
 * File：IVmAnd.h
 * Note：@~chinese 逻辑模块接口声明 @~english Interface for the And tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_AND_H_
#define _IVM_AND_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace AndModule
    {
        /// @~chinese 逻辑处理模式   @~english Logic Processing Mode
        typedef enum _LogicalModeEnum
        {
            /// @~chinese 与   @~english AND
            LogicalMode_And = 0x1,

            /// @~chinese 或   @~english OR
            LogicalMode_Or = 0x2,

            /// @~chinese 非   @~english NOT
            LogicalMode_Not = 0x5,

            /// @~chinese 与非   @~english NAND
            LogicalMode_AndNot = 0x3,

            /// @~chinese 或非   @~english NOR
            LogicalMode_OrNot = 0x4,

        }LogicalModeEnum;

    /// \addtogroup 逻辑
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the AndParams.  */
        ///逻辑参数类
        class AndParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit AndParams() {}
            virtual ~AndParams() {}

        public:

            /// @~chinese 逻辑处理模式   @~english Logic Processing Mode
            __declspec(property(put = SetLogicalMode, get = GetLogicalMode)) LogicalModeEnum LogicalMode;

            virtual LogicalModeEnum GetLogicalMode() = 0;

            virtual void SetLogicalMode(LogicalModeEnum value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the AndResults.  */
        ///逻辑结果类
        class AndResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit AndResults() {}
            virtual ~AndResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 运算结果（BOOL）
             */
            virtual int GetAndResult() = 0;

            /**
             * @brief 运算结果（STRING）
             */
            virtual const char * GetStrAndResult() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the And tool.  */
        ///逻辑工具类
        class AndModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit AndModuleTool() {}
            virtual ~AndModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual AndParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual AndResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            AndModuleTool(AndModuleTool&);
            AndModuleTool& operator=(AndModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for And.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::AndModule::AndModuleTool * __stdcall GetAndToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_AND_H_
