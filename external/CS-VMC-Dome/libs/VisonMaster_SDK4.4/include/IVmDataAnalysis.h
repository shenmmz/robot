/***************************************************************************************************
 * File：IVmDataAnalysis.h
 * Note：@~chinese 协议解析模块接口声明 @~english Interface for the DataAnalysis tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_DATAANALYSIS_H_
#define _IVM_DATAANALYSIS_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace DataAnalysisModule
    {
        /// @~chinese 方式选择   @~english Handler Mode
        typedef enum _AnalysisTypeEnum
        {
            /// @~chinese 文本解析   @~english Text
            AnalysisType_TxtAnalytic = 0x0,

            /// @~chinese 脚本解析   @~english Script
            AnalysisType_ScriptAnalytic = 0x1,

            /// @~chinese 字节解析   @~english Byte
            AnalysisType_ByteAnalytic = 0x2,

        }AnalysisTypeEnum;

    /// \addtogroup 协议解析
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the DataAnalysisParams.  */
        ///协议解析参数类
        class DataAnalysisParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit DataAnalysisParams() {}
            virtual ~DataAnalysisParams() {}

        public:

            /// @~chinese 解析数据(仅当次执行起效)   @~english Analysis Data(Only valid for this execution)
            virtual void SetAnalysis(InputStringData* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 方式选择   @~english Handler Mode
            __declspec(property(put = SetAnalysisType, get = GetAnalysisType)) AnalysisTypeEnum AnalysisType;

            virtual AnalysisTypeEnum GetAnalysisType() = 0;

            virtual void SetAnalysisType(AnalysisTypeEnum value) = 0;

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
        /*  Note: Interface Classes of the result of the DataAnalysisResults.  */
        ///协议解析结果类
        class DataAnalysisResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit DataAnalysisResults() {}
            virtual ~DataAnalysisResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 结果数量
             */
            virtual int GetResultNum() = 0;
            /**
             * @brief 结果数据
             */
            virtual OutputResultInfo * GetResult(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the DataAnalysis tool.  */
        ///协议解析工具类
        class DataAnalysisModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit DataAnalysisModuleTool() {}
            virtual ~DataAnalysisModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual DataAnalysisParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual DataAnalysisResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            DataAnalysisModuleTool(DataAnalysisModuleTool&);
            DataAnalysisModuleTool& operator=(DataAnalysisModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for DataAnalysis.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::DataAnalysisModule::DataAnalysisModuleTool * __stdcall GetDataAnalysisToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_DATAANALYSIS_H_
