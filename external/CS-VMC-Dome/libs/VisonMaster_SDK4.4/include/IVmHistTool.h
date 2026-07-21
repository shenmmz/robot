/***************************************************************************************************
 * File：IVmHistTool.h
 * Note：@~chinese 直方图工具模块接口声明 @~english Interface for the HistTool tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_HISTTOOL_H_
#define _IVM_HISTTOOL_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSHistToolModu
    {
    /// \addtogroup 直方图工具
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///直方图工具ROI管理类
        class HistToolRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
            virtual void SetRoiAnnulus(Annulus stRoiValue) = 0;

            /// @~chinese 掩膜图像（仅当次执行起效）   @~english Mask Image(Only valid for this execution)
            virtual void SetMaskImage(ImageBaseData stMaskImage) = 0;

        };

        /*  Note: Interface Classes of the parameter of the HistToolParams.  */
        ///直方图工具参数类
        class HistToolParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit HistToolParams() {}
            virtual ~HistToolParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual HistToolRoiManager* GetModuRoiManager() = 0;

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

            /// @~chinese 中值判断   @~english Mid Value Check
            __declspec(property(put = SetMedianValueLimitEnable, get = GetMedianValueLimitEnable)) bool MedianValueLimitEnable;

            virtual bool GetMedianValueLimitEnable() = 0;

            virtual void SetMedianValueLimitEnable(bool value) = 0;

            /// @~chinese 中值范围，范围：[0,255]   @~english Mid Value Range，range:[0,255]
            __declspec(property(put = SetMedianValueLimitLow, get = GetMedianValueLimitLow)) int MedianValueLimitLow;

            virtual int GetMedianValueLimitLow() = 0;

            virtual void SetMedianValueLimitLow(int value) = 0;

            /// @~chinese 中值范围，范围：[0,255]   @~english Mid Value Range，range:[0,255]
            __declspec(property(put = SetMedianValueLimitHigh, get = GetMedianValueLimitHigh)) int MedianValueLimitHigh;

            virtual int GetMedianValueLimitHigh() = 0;

            virtual void SetMedianValueLimitHigh(int value) = 0;

            /// @~chinese 峰值判断   @~english Peak Value Check
            __declspec(property(put = SetModeValueLimitEnable, get = GetModeValueLimitEnable)) bool ModeValueLimitEnable;

            virtual bool GetModeValueLimitEnable() = 0;

            virtual void SetModeValueLimitEnable(bool value) = 0;

            /// @~chinese 峰值范围，范围：[0,255]   @~english Peak Value Range，range:[0,255]
            __declspec(property(put = SetModeValueLimitLow, get = GetModeValueLimitLow)) int ModeValueLimitLow;

            virtual int GetModeValueLimitLow() = 0;

            virtual void SetModeValueLimitLow(int value) = 0;

            /// @~chinese 峰值范围，范围：[0,255]   @~english Peak Value Range，range:[0,255]
            __declspec(property(put = SetModeValueLimitHigh, get = GetModeValueLimitHigh)) int ModeValueLimitHigh;

            virtual int GetModeValueLimitHigh() = 0;

            virtual void SetModeValueLimitHigh(int value) = 0;

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
        /*  Note: Interface Classes of the result of the HistToolResults.  */
        ///直方图工具结果类
        class HistToolResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit HistToolResults() {}
            virtual ~HistToolResults() {}

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
             * @brief 像素数量
             */
            virtual int GetPixelCount() = 0;
            /**
             * @brief 最小值
             */
            virtual int GetMinLum() = 0;
            /**
             * @brief 最大值
             */
            virtual int GetMaxLum() = 0;
            /**
             * @brief 中值
             */
            virtual int GetMedianLum() = 0;
            /**
             * @brief 峰值
             */
            virtual int GetModeLum() = 0;
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
            /**
             * @brief 累积直方图
             */
            virtual void GetCumHist(int nCumHist[256]) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the HistTool tool.  */
        ///直方图工具工具类
        class IMVSHistToolModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSHistToolModuTool() {}
            virtual ~IMVSHistToolModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual HistToolParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual HistToolResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSHistToolModuTool(IMVSHistToolModuTool&);
            IMVSHistToolModuTool& operator=(IMVSHistToolModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for HistTool.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSHistToolModu::IMVSHistToolModuTool * __stdcall GetHistToolToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_HISTTOOL_H_
