/***************************************************************************************************
 * File：IVmFormat.h
 * Note：@~chinese 格式化模块接口声明 @~english Interface for the Format tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_FORMAT_H_
#define _IVM_FORMAT_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace FormatModule
    {
    /// \addtogroup 格式化
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the FormatParams.  */
        ///格式化参数类
        class FormatParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit FormatParams() {}
            virtual ~FormatParams() {}

        public:

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the FormatResults.  */
        ///格式化结果类
        class FormatResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit FormatResults() {}
            virtual ~FormatResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 获取格式化结果
             */
            /* Gets information of the FormatOut. */
            virtual const char * GetFormatOut() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the Format tool.  */
        ///格式化工具类
        class FormatModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit FormatModuleTool() {}
            virtual ~FormatModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual FormatParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual FormatResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            FormatModuleTool(FormatModuleTool&);
            FormatModuleTool& operator=(FormatModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for Format.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::FormatModule::FormatModuleTool * __stdcall GetFormatToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_FORMAT_H_
