/***************************************************************************************************
 * File：IVmDataQueue.h
 * Note：CH: 数据队列模块接口声明 | En: Interface for the DataQueue tool.
 *
***************************************************************************************************/
#ifndef _IVM_DATAQUEUE_H_
#define _IVM_DATAQUEUE_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"

namespace VisionMasterSDK
{
    namespace DataQueueModule
    {
    /// \addtogroup 数据队列
    /// @{
		/// 整型数据信息结构
        typedef struct _IntValue {
            int   nIndex;       ///< 整型数据索引
            int   nNum;         ///< 整型数据个数
            int   nValue[256];  ///< 整型数据内容
        } IntValue;

		/// 浮点型数据信息结构
        typedef struct _FloatValue {
            int   nIndex;        ///< 浮点数据索引
            int   nNum;          ///< 浮点数据个数
            float fValue[256];   ///< 浮点数据内容
        } FloatValue;

		/// 字符串型数据内容结构
        typedef struct _StringInfo {
            char  strValue[4096];   ///< 字符串数据
        } StringInfo;

		/// 字符串型数据信息结构
        typedef struct _StringValue {
            int   nIndex;               ///< 字符串数据索引             
            int   nNum;                 ///< 字符串数据个数
            StringInfo astValue[256];   ///< 字符串数据内容
        } StringValue;

        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the DataQueueParams.  */
        ///数据队列参数
        class DataQueueParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit DataQueueParams() {}
            virtual ~DataQueueParams() {}

        public:

            /// @~chinese 异步触发 @~english Asynchronous Trigger
            __declspec(property(put = SetAsynchronousTrigger, get = GetAsynchronousTrigger)) bool AsynchronousTrigger;

            virtual bool GetAsynchronousTrigger() = 0;

            virtual void SetAsynchronousTrigger(bool value) = 0;

            /// @~chinese 清空 @~english Clear
            virtual int DoClearData() = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the DataQueue tool.  */
        ///数据队列工具
        class DataQueueModuleTool
        {
        protected:
            ///< Constructor and Destructor
            explicit DataQueueModuleTool() {}
            virtual ~DataQueueModuleTool() {}

        public:
            /**
             * @brief 参数对象，失败返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual DataQueueParams* GetParamObj() = 0;

            /**
            * @brief 设置整型数据
            * @param pstIntVal  [IN]  整型数据
            */
            virtual void SetIntData(IN const IntValue * const pstIntVal) = 0;

            /**
            * @brief 设置浮点型数据
            * @param pstFloatVal  [IN]  浮点型数据
            */
            virtual void SetFloatData(IN const FloatValue * const pstFloatVal) = 0;

            /**
            * @brief 设置字符串型数据
            * @param pstStringVal  [IN]  字符串型数据
            */
            virtual void SetStringData(IN const StringValue * const pstStringVal) = 0;

            /**
            * @brief 获取整型数据
            * @param nIndex  [IN]  索引
            * @remarks 获取指定索引的整型数据
            */
            virtual IntValue GetIntData(IN const int nIndex) = 0;

            /**
            * @brief 获取浮点型数据
            * @param nIndex  [IN]  索引
            * @remarks 获取指定索引的浮点型数据
            */
            virtual FloatValue GetFloatData(IN const int nIndex) = 0;

            /**
            * @brief 获取字符串型数据
            * @param nIndex  [IN]  索引
            * @remarks 获取指定索引的字符串型数据
            */
            virtual StringValue * GetStringData(IN const int nIndex) = 0;

        private:
            DataQueueModuleTool(DataQueueModuleTool&);
            DataQueueModuleTool& operator=(DataQueueModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for DataQueue.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::DataQueueModule::DataQueueModuleTool * __stdcall GetDataQueueToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_DATAQUEUE_H_
