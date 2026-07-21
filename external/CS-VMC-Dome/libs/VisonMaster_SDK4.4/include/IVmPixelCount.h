/***************************************************************************************************
 * File：IVmPixelCount.h
 * Note：@~chinese 像素统计模块接口声明 @~english Interface for the PixelCount tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_PIXELCOUNT_H_
#define _IVM_PIXELCOUNT_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSPixelCountModu
    {
    /// \addtogroup 像素统计
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///像素统计ROI管理类
        class PixelCountRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
            virtual void SetRoiAnnulus(Annulus stRoiValue) = 0;

            /// @~chinese 掩膜图像（仅当次执行起效）   @~english Mask Image(Only valid for this execution)
            virtual void SetMaskImage(ImageBaseData stMaskImage) = 0;

        };

        /*  Note: Interface Classes of the parameter of the PixelCountParams.  */
        ///像素统计参数类
        class PixelCountParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit PixelCountParams() {}
            virtual ~PixelCountParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual PixelCountRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 低阈值，范围：[0,255]   @~english Low Threshold，range:[0,255]
            __declspec(property(put = SetLowThresh, get = GetLowThresh)) int LowThresh;

            virtual int GetLowThresh() = 0;

            virtual void SetLowThresh(int value) = 0;

            /// @~chinese 高阈值，范围：[0,255]   @~english High Threshold，range:[0,255]
            __declspec(property(put = SetHighThresh, get = GetHighThresh)) int HighThresh;

            virtual int GetHighThresh() = 0;

            virtual void SetHighThresh(int value) = 0;

            /// @~chinese 数量判断   @~english Quantity Check
            __declspec(property(put = SetCountLimitEnable, get = GetCountLimitEnable)) bool CountLimitEnable;

            virtual bool GetCountLimitEnable() = 0;

            virtual void SetCountLimitEnable(bool value) = 0;

            /// @~chinese 数量范围，范围：[0,999999999]   @~english Quantity Range，range:[0,999999999]
            __declspec(property(put = SetCountLimitLow, get = GetCountLimitLow)) int CountLimitLow;

            virtual int GetCountLimitLow() = 0;

            virtual void SetCountLimitLow(int value) = 0;

            /// @~chinese 数量范围，范围：[0,999999999]   @~english Quantity Range，range:[0,999999999]
            __declspec(property(put = SetCountLimitHigh, get = GetCountLimitHigh)) int CountLimitHigh;

            virtual int GetCountLimitHigh() = 0;

            virtual void SetCountLimitHigh(int value) = 0;

            /// @~chinese 比率判断   @~english Ratio Check
            __declspec(property(put = SetRatioLimitEnable, get = GetRatioLimitEnable)) bool RatioLimitEnable;

            virtual bool GetRatioLimitEnable() = 0;

            virtual void SetRatioLimitEnable(bool value) = 0;

            /// @~chinese 比率范围，范围：[0,1]   @~english Ratio Range，range:[0,1]
            __declspec(property(put = SetRatioLimitLow, get = GetRatioLimitLow)) double RatioLimitLow;

            virtual double GetRatioLimitLow() = 0;

            virtual void SetRatioLimitLow(double value) = 0;

            /// @~chinese 比率范围，范围：[0,1]   @~english Ratio Range，range:[0,1]
            __declspec(property(put = SetRatioLimitHigh, get = GetRatioLimitHigh)) double RatioLimitHigh;

            virtual double GetRatioLimitHigh() = 0;

            virtual void SetRatioLimitHigh(double value) = 0;

            /// @~chinese 输出图像   @~english Output Image
            __declspec(property(put = SetBinaryEnable, get = GetBinaryEnable)) bool BinaryEnable;

            virtual bool GetBinaryEnable() = 0;

            virtual void SetBinaryEnable(bool value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the PixelCountResults.  */
        ///像素统计结果类
        class PixelCountResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit PixelCountResults() {}
            virtual ~PixelCountResults() {}

        public:
             /**
             * @brief 输出掩膜
             */
             virtual ImageBaseData GetOutputMask() = 0;

             /**
             * @brief 输出图像
             */
             virtual ImageBaseData GetOutputImage() = 0;

             /**
             * @brief ROI圆弧
             */
             virtual Annulus GetROIAnnulus() = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;
            /**
             * @brief 像素个数
             */
            virtual int GetPixelCount() = 0;
            /**
             * @brief 比率
             */
            virtual float GetRatio() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the PixelCount tool.  */
        ///像素统计工具类
        class IMVSPixelCountModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSPixelCountModuTool() {}
            virtual ~IMVSPixelCountModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual PixelCountParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual PixelCountResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSPixelCountModuTool(IMVSPixelCountModuTool&);
            IMVSPixelCountModuTool& operator=(IMVSPixelCountModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for PixelCount.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSPixelCountModu::IMVSPixelCountModuTool * __stdcall GetPixelCountToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_PIXELCOUNT_H_
