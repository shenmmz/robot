/***************************************************************************************************
 * File：IVmDataRecord.h
 * Note：@~chinese 数据记录模块接口声明 @~english Interface for the DataRecord tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_DATARECORD_H_
#define _IVM_DATARECORD_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace DataRecordModule
    {
    /// \addtogroup 数据记录
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the DataRecordParams.  */
        ///数据记录参数类
        class DataRecordParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit DataRecordParams() {}
            virtual ~DataRecordParams() {}

        public:

            ///@~chinese 清空信号(仅当次执行起效)   @~english Clear Signal(Only valid for this execution)
            virtual void SetClearSignal(int* pInputValue, unsigned int nValueNum) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the DataRecordResults.  */
        ///数据记录结果类
        class DataRecordResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit DataRecordResults() {}
            virtual ~DataRecordResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

             /**
             * @brief 获取整型输出
             * @param strParamName [IN] 参数名称
             * @return 整型数组，出错时抛出异常
             */
             virtual IntDataArray GetOutputInt(IN const char * const strParamName) = 0;

             /**
             * @brief 获取浮点型输出
             * @param strParamName [IN] 参数名称
             * @return 浮点型数组，出错时抛出异常
             */
             virtual FloatDataArray GetOutputFloat(IN const char * const strParamName) = 0;


        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the DataRecord tool.  */
        ///数据记录工具类
        class DataRecordModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit DataRecordModuleTool() {}
            virtual ~DataRecordModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 禁止使用参数基类的方法
             */
            virtual DataRecordParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual DataRecordResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            DataRecordModuleTool(DataRecordModuleTool&);
            DataRecordModuleTool& operator=(DataRecordModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
#endif    ///< _IVM_DATARECORD_H_
