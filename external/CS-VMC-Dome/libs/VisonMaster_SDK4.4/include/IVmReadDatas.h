/***************************************************************************************************
 * File：IVmReadDatas.h
 * Note：@~chinese 接收数据模块接口声明 @~english Interface for the ReadDatas tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_READDATAS_H_
#define _IVM_READDATAS_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"
#include "IVmDynamicIoDefine.h"

namespace VisionMasterSDK
{
    namespace ReadDatasModule
    {
    /// \addtogroup 接收数据
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the ReadDatasParams.  */
        ///接收数据参数类
        class ReadDatasParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ReadDatasParams() {}
            virtual ~ReadDatasParams() {}

        public:

            /// @~chinese 获取行数，范围：[1,256]   @~english Row Number Acquisition，range:[1,256]
            __declspec(property(put = SetRowNum, get = GetRowNum)) int RowNum;

            virtual int GetRowNum() = 0;

            virtual void SetRowNum(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ReadDatasResults.  */
        ///接收数据结果类
        class ReadDatasResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ReadDatasResults() {}
            virtual ~ReadDatasResults() {}

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
             * @brief 结果数据，推荐使用动态接口（GetOutputInt、GetOutputFloat、GetOutputString、GetOutputByteArray、GetOutputImage）获取参数输出
             */
            virtual OutputResultInfo * GetResult(int nIndex) = 0;

			/**
			* @brief 根据模块输出参数名称，获取对应整型数据
			* @param strParamName [IN] 参数名称
			* @return IntDataArray型数据，出错时抛出异常
			*/
			virtual IntDataArray GetOutputInt(IN const char * const strParamName) = 0;

			/**
			* @brief 根据模块输出参数名称，获取对应浮点型数据
            * @param strParamName [IN] 参数名称
			* @return FloatDataArray型数据，出错时抛出异常
			*/
			virtual FloatDataArray GetOutputFloat(IN const char * const strParamName) = 0;

			/**
			* @brief 根据模块输出参数名称，获取对应字符串型数据
            * @param strParamName [IN] 参数名称
			* @return StringDataArray型数据，出错时抛出异常
			*/
			virtual StringDataArray GetOutputString(IN const char * const strParamName) = 0;

			/**
			* @brief 根据模块输出参数名称，获取对应二进制数据型数据
            * @param strParamName [IN] 参数名称
			* @return ByteArrayData型数据，出错时抛出异常
			*/
			virtual ByteArrayData GetOutputByteArray(IN const char * const strParamName) = 0;

			/**
			* @brief 根据模块输出参数名称，获取对应图像结果（单个图像）
            * @param strParamName [IN] 参数名称
			* @return IoImage型数据，出错时抛出异常
			*/
			virtual IoImage GetOutputImage(IN const char * const strParamName) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ReadDatas tool.  */
        ///接收数据工具类
        class ReadDatasModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ReadDatasModuleTool() {}
            virtual ~ReadDatasModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ReadDatasParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ReadDatasResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            ReadDatasModuleTool(ReadDatasModuleTool&);
            ReadDatasModuleTool& operator=(ReadDatasModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ReadDatas.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::ReadDatasModule::ReadDatasModuleTool * __stdcall GetReadDatasToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_READDATAS_H_
