/***************************************************************************************************
 * File：IVmIfBranch.h
 * Note：@~chinese 条件分支模块接口声明 @~english Interface for the IfBranch tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_IFBRANCH_H_
#define _IVM_IFBRANCH_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IfBranchModule
    {
        /// @~chinese 分支结果输出判断 @~english Branch result output judgment
        typedef enum BranchStateEnum
        {
            /// @~chinese OK输出 @~english OK output
            BranchState_OK = 0x1,

            /// @~chinese NG输出 @~english NG output
            BranchState_NG = 0x2,

        }BranchStateEnum;

    /// \addtogroup 条件分支
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the IfBranchParams.  */
        ///条件分支参数类
        class IfBranchParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IfBranchParams() {}
            virtual ~IfBranchParams() {}

        public:
            /// @~chinese 设置整型条件项的最小值 @~english Set the minimum value of an integer condition item
            virtual void SetIntMinValue(const char * const strName, int nMin) = 0;

            /// @~chinese 获取整型条件项的最小值 @~english Get the minimum value of an integer condition item
            virtual int GetIntMinValue(const char * const strName) = 0;

            /// @~chinese 设置浮点条件项的最小值 @~english Set the minimum value of a float condition item
            virtual void SetFloatMinValue(const char * const strName, double dMin) = 0;

            /// @~chinese 获取浮点条件项的最小值 @~english Get the minimum value of a float condition item
            virtual double GetFloatMinValue(const char * const strName) = 0;

            /// @~chinese 设置整型条件项的最大值 @~english Set the maximum value of an integer condition item
            virtual void SetIntMaxValue(const char * const strName, int nMax) = 0;

            /// @~chinese 获取整型条件项的最大值 @~english Get the maximum value of an integer condition item
            virtual int GetIntMaxValue(const char * const strName) = 0;

            /// @~chinese 设置浮点条件项的最大值 @~english Set the maximum value of a float condition item
            virtual void SetFloatMaxValue(const char * const strName, double dMax) = 0;

            /// @~chinese 获取浮点条件项的最大值 @~english Get the maximum value of a float condition item
            virtual double GetFloatMaxValue(const char * const strName) = 0;

            /// @~chinese 设置条件项的判断条件 @~english  Set the judgment condition of the condition item
            virtual void SetStateValue(const char * const strName, BranchStateEnum eState) = 0;

            /// @~chinese 获取条件项的判断条件 @~english Get the judgment condition of the condition item
            virtual BranchStateEnum GetStateValue(const char * const strName) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the IfBranchResults.  */
        ///条件分支结果类
        class IfBranchResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IfBranchResults() {}
            virtual ~IfBranchResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

             /**
             * @brief 分支路线
             */
             virtual const char * GetBranchMatch() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the IfBranch tool.  */
        ///条件分支工具类
        class IfBranchModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IfBranchModuleTool() {}
            virtual ~IfBranchModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual IfBranchParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual IfBranchResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IfBranchModuleTool(IfBranchModuleTool&);
            IfBranchModuleTool& operator=(IfBranchModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for IfBranch.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IfBranchModule::IfBranchModuleTool * __stdcall GetIfBranchToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_IFBRANCH_H_
