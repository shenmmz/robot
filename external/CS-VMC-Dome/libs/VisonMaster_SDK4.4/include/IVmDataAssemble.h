/***************************************************************************************************
 * File：IVmDataAssemble.h
 * Note：@~chinese 协议组装模块接口声明 @~english Interface for the DataAssemble tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_DATAASSEMBLE_H_
#define _IVM_DATAASSEMBLE_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace DataAssembleModule
    {
        /// @~chinese 方式选择   @~english Handler Mode
        typedef enum _AssembleTypeEnum
        {
            /// @~chinese 文本组装   @~english Text
            AssembleType_TxtAssemble = 0x0,

            /// @~chinese 脚本组装   @~english Script
            AssembleType_ScriptAssemble = 0x1,

        }AssembleTypeEnum;

    /// \addtogroup 协议组装
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the DataAssembleParams.  */
        ///协议组装参数类
        class DataAssembleParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit DataAssembleParams() {}
            virtual ~DataAssembleParams() {}

        public:

            /// @~chinese 方式选择   @~english Handler Mode
            __declspec(property(put = SetAssembleType, get = GetAssembleType)) AssembleTypeEnum AssembleType;

            virtual AssembleTypeEnum GetAssembleType() = 0;

            virtual void SetAssembleType(AssembleTypeEnum value) = 0;

            /// @~chinese 分隔符   @~english Split Symbol
            __declspec(property(put = SetSeparator, get = GetSeparator)) const char* Separator;

            virtual const char* GetSeparator() = 0;

            virtual void SetSeparator(const char* value) = 0;

            /// @~chinese 路径选择   @~english Select Path
            __declspec(property(put = SetPyScriptPath, get = GetPyScriptPath)) const char* PyScriptPath;

            virtual const char* GetPyScriptPath() = 0;

            virtual void SetPyScriptPath(const char* value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the DataAssembleResults.  */
        ///协议组装结果类
        class DataAssembleResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit DataAssembleResults() {}
            virtual ~DataAssembleResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

            /**
             * @brief 获取组装结果
             */
            /* Gets information of the AssembleResult. */
            virtual const char * GetAssembleResult() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the DataAssemble tool.  */
        ///协议组装工具类
        class DataAssembleModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit DataAssembleModuleTool() {}
            virtual ~DataAssembleModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual DataAssembleParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual DataAssembleResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            DataAssembleModuleTool(DataAssembleModuleTool&);
            DataAssembleModuleTool& operator=(DataAssembleModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for DataAssemble.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::DataAssembleModule::DataAssembleModuleTool * __stdcall GetDataAssembleToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_DATAASSEMBLE_H_
