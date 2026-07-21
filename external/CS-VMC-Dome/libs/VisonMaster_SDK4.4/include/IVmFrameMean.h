/***************************************************************************************************
 * File：IVmFrameMean.h
 * Note：@~chinese 帧平均模块接口声明 @~english Interface for the FrameMean tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_FRAMEMEAN_H_
#define _IVM_FRAMEMEAN_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSFrameMeanModu
    {
    /// \addtogroup 帧平均
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the FrameMeanParams.  */
        ///帧平均参数类
        class FrameMeanParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit FrameMeanParams() {}
            virtual ~FrameMeanParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the FrameMeanResults.  */
        ///帧平均结果类
        class FrameMeanResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit FrameMeanResults() {}
            virtual ~FrameMeanResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;
            /**
             * @brief 获取已统计图像数
             */
            virtual int GetImgCount() = 0;

            /**
             * @brief 获取输出图像
             */
            virtual ImageBaseData GetOutputImage() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the FrameMean tool.  */
        ///帧平均工具类
        class IMVSFrameMeanModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSFrameMeanModuTool() {}
            virtual ~IMVSFrameMeanModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual FrameMeanParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual FrameMeanResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSFrameMeanModuTool(IMVSFrameMeanModuTool&);
            IMVSFrameMeanModuTool& operator=(IMVSFrameMeanModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for FrameMean.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSFrameMeanModu::IMVSFrameMeanModuTool * __stdcall GetFrameMeanToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_FRAMEMEAN_H_
