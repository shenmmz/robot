/***************************************************************************************************
 * File：IVmSendDatas.h
 * Note：@~chinese 发送数据模块接口声明 @~english Interface for the SendDatas tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_SENDDATAS_H_
#define _IVM_SENDDATAS_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace SendDatasModule
    {
    /// \addtogroup 发送数据
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the SendDatasParams.  */
        ///发送数据参数类
        class SendDatasParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit SendDatasParams() {}
            virtual ~SendDatasParams() {}

        public:

            /// @~chinese 输入图像1(仅当次执行起效)   @~english InputImage1(Only valid for this execution)
            virtual void SetInputImage1(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入图像2(仅当次执行起效)   @~english InputImage2(Only valid for this execution)
            virtual void SetInputImage2(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入图像3(仅当次执行起效)   @~english InputImage3(Only valid for this execution)
            virtual void SetInputImage3(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入图像4(仅当次执行起效)   @~english InputImage4(Only valid for this execution)
            virtual void SetInputImage4(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入图像5(仅当次执行起效)   @~english InputImage5(Only valid for this execution)
            virtual void SetInputImage5(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入图像6(仅当次执行起效)   @~english InputImage6(Only valid for this execution)
            virtual void SetInputImage6(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入图像7(仅当次执行起效)   @~english InputImage7(Only valid for this execution)
            virtual void SetInputImage7(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入图像8(仅当次执行起效)   @~english InputImage8(Only valid for this execution)
            virtual void SetInputImage8(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入图像9(仅当次执行起效)   @~english InputImage9(Only valid for this execution)
            virtual void SetInputImage9(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入图像10(仅当次执行起效)   @~english InputImage10(Only valid for this execution)
            virtual void SetInputImage10(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入图像11(仅当次执行起效)   @~english InputImage11(Only valid for this execution)
            virtual void SetInputImage11(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入图像12(仅当次执行起效)   @~english InputImage12(Only valid for this execution)
            virtual void SetInputImage12(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入图像13(仅当次执行起效)   @~english InputImage13(Only valid for this execution)
            virtual void SetInputImage13(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入图像14(仅当次执行起效)   @~english InputImage14(Only valid for this execution)
            virtual void SetInputImage14(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入图像15(仅当次执行起效)   @~english InputImage15(Only valid for this execution)
            virtual void SetInputImage15(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入图像16(仅当次执行起效)   @~english InputImage16(Only valid for this execution)
            virtual void SetInputImage16(ImageBaseData* pInputValue) = 0;

            /// @~chinese 忽略错误数据   @~english Ignore Error Data
            __declspec(property(put = SetIgnoreErroneousDataEnable, get = GetIgnoreErroneousDataEnable)) bool IgnoreErroneousDataEnable;

            virtual bool GetIgnoreErroneousDataEnable() = 0;

            virtual void SetIgnoreErroneousDataEnable(bool value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the SendDatasResults.  */
        ///发送数据结果类
        class SendDatasResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit SendDatasResults() {}
            virtual ~SendDatasResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 发送数据
             */
            virtual const char * GetSendData() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the SendDatas tool.  */
        ///发送数据工具类
        class SendDatasModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit SendDatasModuleTool() {}
            virtual ~SendDatasModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual SendDatasParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual SendDatasResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            SendDatasModuleTool(SendDatasModuleTool&);
            SendDatasModuleTool& operator=(SendDatasModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for SendDatas.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::SendDatasModule::SendDatasModuleTool * __stdcall GetSendDatasToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_SENDDATAS_H_
