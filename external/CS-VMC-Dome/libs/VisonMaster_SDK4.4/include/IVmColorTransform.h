/***************************************************************************************************
 * File：IVmColorTransform.h
 * Note：@~chinese 颜色转换模块接口声明 @~english Interface for the ColorTransform tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_COLORTRANSFORM_H_
#define _IVM_COLORTRANSFORM_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSColorTransformModu
    {
        /// @~chinese 转换类型   @~english Transfer Type
        typedef enum _ColorTransformTypeEnum
        {
            /// @~chinese RGB转灰度   @~english RGB to Grayscale
            ColorTransformType_RGB2GRAY = 0x1,

            /// @~chinese RGB转HSV   @~english RGB to HSV
            ColorTransformType_RGB2HSV = 0x2,

            /// @~chinese RGB转HSI   @~english RGB to HSI
            ColorTransformType_RGB2HSI = 0x3,

            /// @~chinese RGB转YUV   @~english RGB to YUV
            ColorTransformType_RGB2YUV = 0x4,

        }ColorTransformTypeEnum;

        /// @~chinese 显示通道   @~english Displayed Channel
        typedef enum _ShowChannelEnum
        {
            /// @~chinese 第一通道   @~english 1st Channel
            ShowChannel_FirstChannel = 0x0,

            /// @~chinese 第二通道   @~english 2nd Channel
            ShowChannel_SecondChannel = 0x1,

            /// @~chinese 第三通道   @~english 3rd Channel
            ShowChannel_ThirdChannel = 0x2,

        }ShowChannelEnum;

        /// @~chinese 转换比例   @~english Transfer Ratio
        typedef enum _RGB2GrayTypeEnum
        {
            /// @~chinese 通用转换比例   @~english General Transfer Ratio
            RGB2GrayType_GeneralRatio = 0x1,

            /// @~chinese 平均转换比例   @~english Average Transfer Ratio
            RGB2GrayType_AverageRatio = 0x2,

            /// @~chinese 通道最小值   @~english Channel Min Value
            RGB2GrayType_ChannelMin = 0x3,

            /// @~chinese 通道最大值   @~english Channel Max Value
            RGB2GrayType_ChannelMax = 0x4,

            /// @~chinese 自设转换比例   @~english User-defined Transfer Ratio
            RGB2GrayType_UserRatio = 0x5,

            /// @~chinese R通道   @~english Channel R
            RGB2GrayType_RChannel = 0x6,

            /// @~chinese B通道   @~english Channel B
            RGB2GrayType_BChannel = 0x7,

            /// @~chinese G通道   @~english Channel G
            RGB2GrayType_GChannel = 0x8,

            /// @~chinese 无约束转换比例   @~english Unlimited Transfer Ratio
            RGB2GrayType_SetRatioNs = 0x9,

        }RGB2GrayTypeEnum;

    /// \addtogroup 颜色转换
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the ColorTransformParams.  */
        ///颜色转换参数类
        class ColorTransformParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ColorTransformParams() {}
            virtual ~ColorTransformParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 转换类型   @~english Transfer Type
            __declspec(property(put = SetColorTransformType, get = GetColorTransformType)) ColorTransformTypeEnum ColorTransformType;

            virtual ColorTransformTypeEnum GetColorTransformType() = 0;

            virtual void SetColorTransformType(ColorTransformTypeEnum value) = 0;

            /// @~chinese 显示通道   @~english Displayed Channel
            __declspec(property(put = SetShowChannel, get = GetShowChannel)) ShowChannelEnum ShowChannel;

            virtual ShowChannelEnum GetShowChannel() = 0;

            virtual void SetShowChannel(ShowChannelEnum value) = 0;

            /// @~chinese 转换比例   @~english Transfer Ratio
            __declspec(property(put = SetRGB2GrayType, get = GetRGB2GrayType)) RGB2GrayTypeEnum RGB2GrayType;

            virtual RGB2GrayTypeEnum GetRGB2GrayType() = 0;

            virtual void SetRGB2GrayType(RGB2GrayTypeEnum value) = 0;

            /// @~chinese R转换比例，范围：[0,100]   @~english R Transfer Ratio，range:[0,100]
            __declspec(property(put = SetRratio, get = GetRratio)) int Rratio;

            virtual int GetRratio() = 0;

            virtual void SetRratio(int value) = 0;

            /// @~chinese G转换比例，范围：[0,100]   @~english G Transfer Ratio，range:[0,100]
            __declspec(property(put = SetGratio, get = GetGratio)) int Gratio;

            virtual int GetGratio() = 0;

            virtual void SetGratio(int value) = 0;

            /// @~chinese B转换比例，范围：[0,100]   @~english B Transfer Ratio，range:[0,100]
            __declspec(property(put = SetBratio, get = GetBratio)) int Bratio;

            virtual int GetBratio() = 0;

            virtual void SetBratio(int value) = 0;

            /// @~chinese R转换比例，范围：[-1000,1000]   @~english R Transfer Ratio，range:[-1000,1000]
            __declspec(property(put = SetRRatioNs, get = GetRRatioNs)) double RRatioNs;

            virtual double GetRRatioNs() = 0;

            virtual void SetRRatioNs(double value) = 0;

            /// @~chinese G转换比例，范围：[-1000,1000]   @~english G Transfer Ratio，range:[-1000,1000]
            __declspec(property(put = SetGRatioNs, get = GetGRatioNs)) double GRatioNs;

            virtual double GetGRatioNs() = 0;

            virtual void SetGRatioNs(double value) = 0;

            /// @~chinese B转换比例，范围：[-1000,1000]   @~english B Transfer Ratio，range:[-1000,1000]
            __declspec(property(put = SetBRatioNs, get = GetBRatioNs)) double BRatioNs;

            virtual double GetBRatioNs() = 0;

            virtual void SetBRatioNs(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ColorTransformResults.  */
        ///颜色转换结果类
        class ColorTransformResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ColorTransformResults() {}
            virtual ~ColorTransformResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 输出图像
             */
            virtual ImageBaseData GetOutputImage() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ColorTransform tool.  */
        ///颜色转换工具类
        class IMVSColorTransformModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSColorTransformModuTool() {}
            virtual ~IMVSColorTransformModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ColorTransformParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ColorTransformResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSColorTransformModuTool(IMVSColorTransformModuTool&);
            IMVSColorTransformModuTool& operator=(IMVSColorTransformModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ColorTransform.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSColorTransformModu::IMVSColorTransformModuTool * __stdcall GetColorTransformToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_COLORTRANSFORM_H_
