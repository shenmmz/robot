/***************************************************************************************************
* File：IVmGlobalVariable.h
* Note：@~chinese 全局变量模块接口声明 @~english Interface for the GlobalVariable tool.
*
***************************************************************************************************/
#ifndef _IVM_GLOBALVARIABLE_H_
#define _IVM_GLOBALVARIABLE_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "Variable.h"

#define    MAX_VAR_NUM    2048        //全局变量最大数量
#define    COMM_NAME_MAX_LENGTH    256    
#define    COMM_TYPE_MAX_LENGTH    16    

using namespace VisionMasterSDK::VariableModule;

namespace VisionMasterSDK
{
    namespace GlobalVariableModule
    {
    /// \addtogroup 全局变量
    /// @{
        /// @~chinese 全局变量信息单个结构体  @~english Global module single variable information structure
        typedef struct GlobalVarInfo
        {
            /// @~chinese 通信初始化 @~english communicate initial
            bool bCommEnable;

            /// @~chinese 变量类型 string, int, float @~english variable type
            char strValueType[COMM_TYPE_MAX_LENGTH];

            /// @~chinese 变量长度 @~english variable length
            unsigned int  nDataLen;

            /// @~chinese 变量值 @~english variable value
            char *pDataValue;

            /// @~chinese 变量名称 @~english variable name
            char strValueName[COMM_NAME_MAX_LENGTH];

            /// @~chinese 变量注释信息 @~english variable remark info
            char strRemark[COMM_NAME_MAX_LENGTH];

            /// @~chinese 保留 @~english remark
            int nReserved[4];

        }GlobalVarInfo;

        /// @~chinese 全局变量模块信息列表结构 @~english Global module variable information list structure
        typedef struct GlobalVarInfoList
        {
            /// @~chinese 个数 @~english  count
            int nCount;

            /// @~chinese variable list @~english list
            GlobalVarInfo stGlobalValue[MAX_VAR_NUM];

            /// @~chinese 保留 @~english remark
            int nReserved[4];

        }GlobalVarInfoList;

        /*  Note: Note: This is the class for the GlobalVariable tool.  */
        ///全局变量工具
        class GlobalVariableModuleTool : public CVariable
        {
        protected:
            ///< Constructor and Destructor
            explicit GlobalVariableModuleTool() {}
            virtual ~GlobalVariableModuleTool() {}

        public:
            /**
            * @brief 设置全局变量
            * @param strParam    [IN]  参数
            * @param strVal      [IN]  值
            * @remarks 设置对应全局变量值，数据为字符串类型
            */
            virtual void SetGlobalVar(IN const char * const strParam, IN const char * const strVal) = 0;

            /**
            * @brief 获取全局变量
            * @param strParam    [IN]   参数
            * @param strVal      [OUT]  值
            * @remarks 获取对应全局变量值，数据为字符串类型
            */
            virtual void GetGlobalVar(IN const char * const strParam, OUT char strVal[8192]) = 0;

            /**
            * @brief 获取全局变量列表信息
            * @remarks 获取对应全局变量值
            */
            virtual GlobalVarInfoList* GetAllGlobalVar(void) = 0;

        private:
            GlobalVariableModuleTool(GlobalVariableModuleTool&);
            GlobalVariableModuleTool& operator=(GlobalVariableModuleTool&);
        };
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

    /*  Note: Get tool instance for GlobalVariable.  */
    IMVS_6000_PLATFORMSDK_API VisionMasterSDK::GlobalVariableModule::GlobalVariableModuleTool * __stdcall GetGlobalVariableToolInstance(void);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_GLOBALVARIABLE_H_