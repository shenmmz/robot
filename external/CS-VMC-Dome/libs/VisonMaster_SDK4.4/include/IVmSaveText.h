/***************************************************************************************************
 * File：IVmSaveText.h
 * Note：@~chinese 文本保存模块接口声明 @~english Interface for the SaveText tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_SAVETEXT_H_
#define _IVM_SAVETEXT_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace SaveTextModule
    {
        /// @~chinese 数据源   @~english Data Source
        typedef enum _OutputTypeEnum
        {
            /// @~chinese TXT   @~english TXT
            OutputType_TXT = 0x0,

            /// @~chinese CSV   @~english CSV
            OutputType_CSV = 0x1,

        }OutputTypeEnum;

        /// @~chinese 保存条件   @~english Save Condition
        typedef enum _SaveConditionEnum
        {
            /// @~chinese 全部保存   @~english Save All
            SaveCondition_ALL = 0x0,

            /// @~chinese OK时保存   @~english Save OK
            SaveCondition_OK = 0x1,

            /// @~chinese NG时保存   @~english Save NG
            SaveCondition_NG = 0x2,

            /// @~chinese 不保存   @~english Not Save
            SaveCondition_NONE = 0x3,

        }SaveConditionEnum;

        /// @~chinese 存储方式   @~english Save Mode
        typedef enum _SaveTypeEnum
        {
            /// @~chinese 覆盖存储   @~english Overwrite
            SaveType_OverWrite = 0x0,

            /// @~chinese 停止存储   @~english Stop Saving
            SaveType_NoSave = 0x1,

        }SaveTypeEnum;

        /// @~chinese 文件命名   @~english File Naming
        typedef enum _NameTypeEnum
        {
            /// @~chinese 序号   @~english Number
            NameType_Index = 0x0,

            /// @~chinese 时间   @~english Time
            NameType_Time = 0x1,

            /// @~chinese 无   @~english None
            NameType_None = 0x2,

        }NameTypeEnum;

    /// \addtogroup 文本保存
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the SaveTextParams.  */
        ///文本保存参数类
        class SaveTextParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit SaveTextParams() {}
            virtual ~SaveTextParams() {}

        public:

            /// @~chinese 数据源   @~english Data Source
            __declspec(property(put = SetOutputType, get = GetOutputType)) OutputTypeEnum OutputType;

            virtual OutputTypeEnum GetOutputType() = 0;

            virtual void SetOutputType(OutputTypeEnum value) = 0;

            /// @~chinese 保存列名   @~english Save column name
            __declspec(property(put = SetSaveColumnNames, get = GetSaveColumnNames)) bool SaveColumnNames;

            virtual bool GetSaveColumnNames() = 0;

            virtual void SetSaveColumnNames(bool value) = 0;

            /// @~chinese 触发保存   @~english Save Trigger
            __declspec(property(put = SetSaveTrigger, get = GetSaveTrigger)) bool SaveTrigger;

            virtual bool GetSaveTrigger() = 0;

            virtual void SetSaveTrigger(bool value) = 0;

            /// @~chinese 保存条件   @~english Save Condition
            __declspec(property(put = SetSaveCondition, get = GetSaveCondition)) SaveConditionEnum SaveCondition;

            virtual SaveConditionEnum GetSaveCondition() = 0;

            virtual void SetSaveCondition(SaveConditionEnum value) = 0;

            /// @~chinese 保存路径   @~english SavePath
            __declspec(property(put = SetPath, get = GetPath)) const char* Path;

            virtual const char* GetPath() = 0;

            virtual void SetPath(const char* value) = 0;

            /// @~chinese 文件保存数量，范围：[1,65530]   @~english File Save Number，range:[1,65530]
            __declspec(property(put = SetFileCount, get = GetFileCount)) int FileCount;

            virtual int GetFileCount() = 0;

            virtual void SetFileCount(int value) = 0;

            /// @~chinese 文件容量(K)，范围：[100,10240]   @~english Document Size(K)，range:[100,10240]
            __declspec(property(put = SetFileLen, get = GetFileLen)) int FileLen;

            virtual int GetFileLen() = 0;

            virtual void SetFileLen(int value) = 0;

            /// @~chinese 存储方式   @~english Save Mode
            __declspec(property(put = SetSaveType, get = GetSaveType)) SaveTypeEnum SaveType;

            virtual SaveTypeEnum GetSaveType() = 0;

            virtual void SetSaveType(SaveTypeEnum value) = 0;

            /// @~chinese 时间戳设置   @~english Timestamp Setting
            __declspec(property(put = SetTimeStamp, get = GetTimeStamp)) const char* TimeStamp;

            virtual const char* GetTimeStamp() = 0;

            virtual void SetTimeStamp(const char* value) = 0;

            /// @~chinese 生成日期目录   @~english Generate Directory By Date
            __declspec(property(put = SetSaveByDateTrigger, get = GetSaveByDateTrigger)) bool SaveByDateTrigger;

            virtual bool GetSaveByDateTrigger() = 0;

            virtual void SetSaveByDateTrigger(bool value) = 0;

            /// @~chinese 文件保存天数，范围：[1,500]   @~english File Retention Days，range:[1,500]
            __declspec(property(put = SetMaxDayCount, get = GetMaxDayCount)) int MaxDayCount;

            virtual int GetMaxDayCount() = 0;

            virtual void SetMaxDayCount(int value) = 0;

            /// @~chinese 文件命名   @~english File Naming
            __declspec(property(put = SetNameType, get = GetNameType)) NameTypeEnum NameType;

            virtual NameTypeEnum GetNameType() = 0;

            virtual void SetNameType(NameTypeEnum value) = 0;

            /// @~chinese 实时存储   @~english Real Time Save
            __declspec(property(put = SetRealTimeSave, get = GetRealTimeSave)) bool RealTimeSave;

            virtual bool GetRealTimeSave() = 0;

            virtual void SetRealTimeSave(bool value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the SaveTextResults.  */
        ///文本保存结果类
        class SaveTextResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit SaveTextResults() {}
            virtual ~SaveTextResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 获取保存路径
             */
            virtual const char * GetSavePath() = 0;

            /**
             * @brief 获取文件名称
             */
            virtual const char * GetFileName() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the SaveText tool.  */
        ///文本保存工具类
        class SaveTextModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit SaveTextModuleTool() {}
            virtual ~SaveTextModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual SaveTextParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual SaveTextResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            SaveTextModuleTool(SaveTextModuleTool&);
            SaveTextModuleTool& operator=(SaveTextModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for SaveText.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::SaveTextModule::SaveTextModuleTool * __stdcall GetSaveTextToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_SAVETEXT_H_
