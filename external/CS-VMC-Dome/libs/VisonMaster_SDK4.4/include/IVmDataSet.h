/***************************************************************************************************
 * File：IVmDataSet.h
 * Note：@~chinese 数据集合模块接口声明 @~english Interface for the DataSet tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_DATASET_H_
#define _IVM_DATASET_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace DataSetModule
    {
    /// \addtogroup 数据集合
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the DataSetParams.  */
        ///数据集合参数类
        class DataSetParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit DataSetParams() {}
            virtual ~DataSetParams() {}

        public:

            /// @~chinese 清空信号(仅当次执行起效)   @~english Clear Signal(Only valid for this execution)
            virtual void SetClearSignal(int* pInputValue, unsigned int nValueNum) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the DataSetResults.  */
        ///数据集合结果类
        class DataSetResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit DataSetResults() {}
            virtual ~DataSetResults() {}

        public:
             /**
             * @brief 说模块状态
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
        /*  Note: Note: This is the class for the DataSet tool.  */
        ///数据集合工具类
        class DataSetModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit DataSetModuleTool() {}
            virtual ~DataSetModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual DataSetParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual DataSetResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            DataSetModuleTool(DataSetModuleTool&);
            DataSetModuleTool& operator=(DataSetModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for DataSet.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::DataSetModule::DataSetModuleTool * __stdcall GetDataSetToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_DATASET_H_
