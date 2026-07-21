/***************************************************************************************************
 * File：IVmBranch.h
 * Note：@~chinese 分支模块模块接口声明 @~english Interface for the Branch tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_BRANCH_H_
#define _IVM_BRANCH_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace BranchModule_STD
    {
        /// @~chinese 索引选择   @~english Index Selection
        typedef enum _IndexUserSetEnum
        {
            /// @~chinese 按值索引   @~english Index by Value
            IndexUserSet_IndexByValue = 0,

            /// @~chinese 按位索引   @~english Index by Bit
            IndexUserSet_IndexByBit = 1,

        }IndexUserSetEnum;

    /// \addtogroup 分支模块
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the BranchParams.  */
        ///分支模块参数类
        class BranchParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit BranchParams() {}
            virtual ~BranchParams() {}

        public:

            /// @~chinese 结果标志(仅当次执行起效)   @~english inputint(Only valid for this execution)
            virtual void SetInputint(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 索引选择   @~english Index Selection
            __declspec(property(put = SetIndexUserSet, get = GetIndexUserSet)) IndexUserSetEnum IndexUserSet;

            virtual IndexUserSetEnum GetIndexUserSet() = 0;

            virtual void SetIndexUserSet(IndexUserSetEnum value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the BranchResults.  */
        ///分支模块结果类
        class BranchResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit BranchResults() {}
            virtual ~BranchResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 获取传输标志
             */
            virtual int GetOutputInt() = 0;

            /**
             * @brief 获取分支路线
             */
            virtual const char * GetBranchMatch() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the Branch tool.  */
        ///分支模块工具类
        class BranchModule_STDTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit BranchModule_STDTool() {}
            virtual ~BranchModule_STDTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual BranchParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual BranchResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            BranchModule_STDTool(BranchModule_STDTool&);
            BranchModule_STDTool& operator=(BranchModule_STDTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for Branch.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::BranchModule_STD::BranchModule_STDTool * __stdcall GetBranchToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_BRANCH_H_
