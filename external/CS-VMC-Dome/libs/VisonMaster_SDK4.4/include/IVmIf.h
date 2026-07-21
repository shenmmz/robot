/***************************************************************************************************
 * File：IVmIf.h
 * Note：@~chinese 条件检测模块接口声明 @~english Interface for the If tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_IF_H_
#define _IVM_IF_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include <map>
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IfModule
    {
        /// @~chinese 逻辑处理模式   @~english Logic Processing Mode
        typedef enum _LogicalModeEnum
        {
            /// @~chinese 全部   @~english All
            LogicalMode_And = 0x1,

            /// @~chinese 任意   @~english Any
            LogicalMode_Or = 0x2,

        }LogicalModeEnum;

    /// \addtogroup 条件检测
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the Condition Item.  */
        class IfItemParam
        {
        protected:
            ///< Constructor and Destructor
            explicit IfItemParam() {}
            virtual ~IfItemParam() {}

        public:

            /// @~chinese 全部   @~english Max Value
            __declspec(property(put = SetMaxValue, get = GetMaxValue)) double MaxValue;

            virtual double GetMaxValue() = 0;

            virtual void SetMaxValue(double value) = 0;

            /// @~chinese 全部   @~english Min Value
            __declspec(property(put = SetMinValue, get = GetMinValue)) double MinValue;

            virtual double GetMinValue() = 0;

            virtual void SetMinValue(double value) = 0;
        };
        /*  Note: Interface Classes of the parameter of the IfParams.  */
        ///条件检测参数类
        class IfParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IfParams() {}
            virtual ~IfParams() {}

        public:

            /// @~chinese 逻辑处理模式   @~english Logic Processing Mode
            __declspec(property(put = SetLogicalMode, get = GetLogicalMode)) LogicalModeEnum LogicalMode;

            virtual LogicalModeEnum GetLogicalMode() = 0;

            virtual void SetLogicalMode(LogicalModeEnum value) = 0;

            /**
            * @brief 获取条件项参数（最大值/最小值等）（单例模式）
            * @param strName  [IN] 参数名称为界面变量名 如"int0"
            * @return  返回条件项参数IfItemParam
            */
            virtual IfItemParam * GetIfItemParam(const char * const strName) = 0;

            /**
            * @brief 获取动态参数项（单例模式）
            * @param strName  [IN] 参数名称为"界面变量名_参数用途" ，如"int0_Max"
            * @return   ParamItem
            * @deprecated 4.2.0版本废弃
            */
            virtual ParamItem * GetDynamicParam(const char * const strName) = 0;
        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the IfResults.  */
        ///条件检测结果类
        class IfResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IfResults() {}
            virtual ~IfResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 获取字符串结果
             */
            virtual const char * GetStrResult() = 0;
            /**
             * @brief 获取整型结果
             */
            virtual int GetNIfResult() = 0;
            /**
             * @brief 获取条件项结果个数
             */
            virtual int GetItemResultNum() = 0;
            /**
             * @brief 获取条件项结果
             * @param nIndex [IN] 索引
             */
            virtual const char * GetIfItemResult(int nIndex) = 0;
            /**
             * @brief 获取条件项状态
             * @param nIndex [IN] 索引
             */
            virtual int GetIfItemStatus(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the If tool.  */
        ///条件检测工具类
        class IfModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IfModuleTool() {}
            virtual ~IfModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual IfParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual IfResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IfModuleTool(IfModuleTool&);
            IfModuleTool& operator=(IfModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for If.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IfModule::IfModuleTool * __stdcall GetIfToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_IF_H_
