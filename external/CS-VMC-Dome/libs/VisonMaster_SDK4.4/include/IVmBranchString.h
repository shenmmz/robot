/***************************************************************************************************
 * File：IVmBranchString.h
 * Note：@~chinese 分支字符模块接口声明 @~english Interface for the BranchString tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_BRANCHSTRING_H_
#define _IVM_BRANCHSTRING_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

#define MAX_BRANCH_VAR_NUM  32
#define MAX_BRANCH_PARAM_LEN  128
namespace VisionMasterSDK
{
    namespace BranchStringCpmL
    {
    /// \addtogroup 分支字符
    /// @{
        /*************************** Params Class Start ***************************/

        /*  Note: Interface Classes of the parameter of the branch string Item.  */
        /// 分支字符单个参数
        class BranchStringItemParam
        {
            protected:
            ///< Constructor and Destructor
            explicit BranchStringItemParam() {}
            virtual ~BranchStringItemParam() {}

        public:

            /// @~chinese 分支id @~english branch id
            __declspec(property(get = GetBranchID)) int BranchID;

            virtual int GetBranchID() = 0;

            /// @~chinese 分支输入字符 @~english branch input value
            __declspec(property(put = SetInputValue, get = GetInputValue)) const char* InputValue;

            virtual const char* GetInputValue() = 0;

            virtual void SetInputValue(const char* value) = 0;

            /// @~chinese 调试模式 @~english Debug mode
            __declspec(property(put = SetIsDebugMode, get = GetIsDebugMode)) bool IsDebugMode;

            virtual bool GetIsDebugMode() = 0;

            virtual void SetIsDebugMode(bool value) = 0;
        };

        ///分支字符信息结构体
        typedef struct _BranchStringItemlist
        {
            unsigned int nNum;                                                     ///< 元素数量
            BranchStringItemParam *astBranchStringItemInfo[MAX_BRANCH_VAR_NUM];    ///< 元素信息
            unsigned int nReserved[4];                                             ///< 保留字段

        }BranchStringItemList;

        /*  Note: Interface Classes of the parameter of the BranchStringParams.  */
        ///分支字符参数类
        class BranchStringParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit BranchStringParams() {}
            virtual ~BranchStringParams() {}

        public:

            /// @~chinese 输入文本(仅当次执行起效)   @~english Input Text(Only valid for this execution)
            virtual void SetStringA(InputStringData* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 获取分支模块单项配置参数 @~english Get single branch item config
            virtual BranchStringItemParam * GetBranchStrItemParam(int branchID) = 0;

            /// @~chinese 获取所有分支字符项 @~english Get all branch string item
            virtual BranchStringItemList*  GetBranchStrItemParamList() = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the BranchStringResults.  */
        ///分支字符结果类
        class BranchStringResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit BranchStringResults() {}
            virtual ~BranchStringResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 获取分支路线
             */
            virtual const char * GetBranchMatch() = 0;

            /**
             * @brief 获取分支条件字符
             */
            virtual const char * GetStringMatch() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the BranchString tool.  */
        ///分支字符工具类
        class BranchStringCpmLTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit BranchStringCpmLTool() {}
            virtual ~BranchStringCpmLTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual BranchStringParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual BranchStringResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            BranchStringCpmLTool(BranchStringCpmLTool&);
            BranchStringCpmLTool& operator=(BranchStringCpmLTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for BranchString.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::BranchStringCpmL::BranchStringCpmLTool * __stdcall GetBranchStringToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_BRANCHSTRING_H_
