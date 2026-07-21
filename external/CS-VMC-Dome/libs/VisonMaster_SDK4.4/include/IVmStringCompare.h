/***************************************************************************************************
 * File：IVmStringCompare.h
 * Note：@~chinese 字符比较模块接口声明 @~english Interface for the StringCompare tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_STRINGCOMPARE_H_
#define _IVM_STRINGCOMPARE_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace StringCompareModule
    {
    /// \addtogroup 字符比较
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the StringCompareParams.  */
        ///字符比较参数类
        class StringCompareParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit StringCompareParams() {}
            virtual ~StringCompareParams() {}

        public:

            /// @~chinese 输入文本(仅当次执行起效)   @~english Input Text(Only valid for this execution)
            virtual void SetStringA(InputStringData* pInputValue, unsigned int nValueNum) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the StringCompareResults.  */
        ///字符比较结果类
        class StringCompareResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit StringCompareResults() {}
            virtual ~StringCompareResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 获取分支条件字符
             */
            virtual const char * GetStringMatch() = 0;

            /**
             * @brief 获取Index
             */
            virtual int GetIndex() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the StringCompare tool.  */
        ///字符比较工具类
        class StringCompareModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit StringCompareModuleTool() {}
            virtual ~StringCompareModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual StringCompareParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual StringCompareResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            StringCompareModuleTool(StringCompareModuleTool&);
            StringCompareModuleTool& operator=(StringCompareModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for StringCompare.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::StringCompareModule::StringCompareModuleTool * __stdcall GetStringCompareToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_STRINGCOMPARE_H_
