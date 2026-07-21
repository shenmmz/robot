/***************************************************************************************************
 * File：IVmIntensityMeasure.h
 * Note：@~chinese 亮度测量模块接口声明 @~english Interface for the IntensityMeasure tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_INTENSITYMEASURE_H_
#define _IVM_INTENSITYMEASURE_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSIntensityMeasureModu
    {
    /// \addtogroup 亮度测量
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///亮度测量ROI管理类
        class IntensityMeasureRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
            virtual void SetRoiAnnulus(Annulus stRoiValue) = 0;

            /// @~chinese 掩膜图像（仅当次执行起效）   @~english Mask Image(Only valid for this execution)
            virtual void SetMaskImage(ImageBaseData stMaskImage) = 0;

        };

        /*  Note: Interface Classes of the parameter of the IntensityMeasureParams.  */
        ///亮度测量参数类
        class IntensityMeasureParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IntensityMeasureParams() {}
            virtual ~IntensityMeasureParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual IntensityMeasureRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 最大值判断   @~english Max Value Check
            __declspec(property(put = SetMaxValueLimitEnable, get = GetMaxValueLimitEnable)) bool MaxValueLimitEnable;

            virtual bool GetMaxValueLimitEnable() = 0;

            virtual void SetMaxValueLimitEnable(bool value) = 0;

            /// @~chinese 最大值范围，范围：[0,255]   @~english Max Value Range，range:[0,255]
            __declspec(property(put = SetMaxValueLimitLow, get = GetMaxValueLimitLow)) int MaxValueLimitLow;

            virtual int GetMaxValueLimitLow() = 0;

            virtual void SetMaxValueLimitLow(int value) = 0;

            /// @~chinese 最大值范围，范围：[0,255]   @~english Max Value Range，range:[0,255]
            __declspec(property(put = SetMaxValueLimitHigh, get = GetMaxValueLimitHigh)) int MaxValueLimitHigh;

            virtual int GetMaxValueLimitHigh() = 0;

            virtual void SetMaxValueLimitHigh(int value) = 0;

            /// @~chinese 最小值判断   @~english Min Value Check
            __declspec(property(put = SetMinValueLimitEnable, get = GetMinValueLimitEnable)) bool MinValueLimitEnable;

            virtual bool GetMinValueLimitEnable() = 0;

            virtual void SetMinValueLimitEnable(bool value) = 0;

            /// @~chinese 最小值范围，范围：[0,255]   @~english Min Value Range，range:[0,255]
            __declspec(property(put = SetMinValueLimitLow, get = GetMinValueLimitLow)) int MinValueLimitLow;

            virtual int GetMinValueLimitLow() = 0;

            virtual void SetMinValueLimitLow(int value) = 0;

            /// @~chinese 最小值范围，范围：[0,255]   @~english Min Value Range，range:[0,255]
            __declspec(property(put = SetMinValueLimitHigh, get = GetMinValueLimitHigh)) int MinValueLimitHigh;

            virtual int GetMinValueLimitHigh() = 0;

            virtual void SetMinValueLimitHigh(int value) = 0;

            /// @~chinese 均值判断   @~english Mean Value Check
            __declspec(property(put = SetMeanLimitEnable, get = GetMeanLimitEnable)) bool MeanLimitEnable;

            virtual bool GetMeanLimitEnable() = 0;

            virtual void SetMeanLimitEnable(bool value) = 0;

            /// @~chinese 均值范围，范围：[0,255]   @~english Mean Value Range，range:[0,255]
            __declspec(property(put = SetMeanLimitLow, get = GetMeanLimitLow)) double MeanLimitLow;

            virtual double GetMeanLimitLow() = 0;

            virtual void SetMeanLimitLow(double value) = 0;

            /// @~chinese 均值范围，范围：[0,255]   @~english Mean Value Range，range:[0,255]
            __declspec(property(put = SetMeanLimitHigh, get = GetMeanLimitHigh)) double MeanLimitHigh;

            virtual double GetMeanLimitHigh() = 0;

            virtual void SetMeanLimitHigh(double value) = 0;

            /// @~chinese 标准差判断   @~english SD Check
            __declspec(property(put = SetStdLimitEnable, get = GetStdLimitEnable)) bool StdLimitEnable;

            virtual bool GetStdLimitEnable() = 0;

            virtual void SetStdLimitEnable(bool value) = 0;

            /// @~chinese 标准差范围，范围：[0,255]   @~english SD Range，range:[0,255]
            __declspec(property(put = SetStdLimitLow, get = GetStdLimitLow)) double StdLimitLow;

            virtual double GetStdLimitLow() = 0;

            virtual void SetStdLimitLow(double value) = 0;

            /// @~chinese 标准差范围，范围：[0,255]   @~english SD Range，range:[0,255]
            __declspec(property(put = SetStdLimitHigh, get = GetStdLimitHigh)) double StdLimitHigh;

            virtual double GetStdLimitHigh() = 0;

            virtual void SetStdLimitHigh(double value) = 0;

            /// @~chinese 对比度判断   @~english Contrast Judge
            __declspec(property(put = SetContrastLimitEnable, get = GetContrastLimitEnable)) bool ContrastLimitEnable;

            virtual bool GetContrastLimitEnable() = 0;

            virtual void SetContrastLimitEnable(bool value) = 0;

            /// @~chinese 对比度范围，范围：[0,255]   @~english Contrast Range，range:[0,255]
            __declspec(property(put = SetContrastLimitLow, get = GetContrastLimitLow)) double ContrastLimitLow;

            virtual double GetContrastLimitLow() = 0;

            virtual void SetContrastLimitLow(double value) = 0;

            /// @~chinese 对比度范围，范围：[0,255]   @~english Contrast Range，range:[0,255]
            __declspec(property(put = SetContrastLimitHigh, get = GetContrastLimitHigh)) double ContrastLimitHigh;

            virtual double GetContrastLimitHigh() = 0;

            virtual void SetContrastLimitHigh(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the IntensityMeasureResults.  */
        ///亮度测量结果类
        class IntensityMeasureResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IntensityMeasureResults() {}
            virtual ~IntensityMeasureResults() {}

        public:
             /**
             * @brief 输出掩膜
             */
             virtual ImageBaseData GetOutputMask() = 0;

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
             * @brief 最小值
             */
            virtual int GetMinLum() = 0;
            /**
             * @brief 最大值
             */
            virtual int GetMaxLum() = 0;
            /**
             * @brief 均值
             */
            virtual float GetMeanLum() = 0;
            /**
             * @brief 标准差
             */
            virtual float GetStdLum() = 0;
            /**
             * @brief 对比度
             */
            virtual float GetHistContrast() = 0;
            /**
             * @brief 直方图
             */
            virtual void GetLumHist(int nLumHist[256]) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the IntensityMeasure tool.  */
        ///亮度测量工具类
        class IMVSIntensityMeasureModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSIntensityMeasureModuTool() {}
            virtual ~IMVSIntensityMeasureModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual IntensityMeasureParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual IntensityMeasureResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSIntensityMeasureModuTool(IMVSIntensityMeasureModuTool&);
            IMVSIntensityMeasureModuTool& operator=(IMVSIntensityMeasureModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for IntensityMeasure.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSIntensityMeasureModu::IMVSIntensityMeasureModuTool * __stdcall GetIntensityMeasureToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_INTENSITYMEASURE_H_
