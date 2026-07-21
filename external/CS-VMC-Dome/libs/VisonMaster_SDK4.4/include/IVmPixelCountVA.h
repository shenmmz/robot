/***************************************************************************************************
 * File：IVmPixelCountVA.h
 * Note：@~chinese 像素统计模块接口声明 @~english Interface for the PixelCountVA tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_PIXELCOUNTVA_H_
#define _IVM_PIXELCOUNTVA_H_

#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSPixelCountModuVA
    {
    /// \addtogroup 像素统计
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///像素统计ROI管理类
        class PixelCountVARoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
            virtual void SetRoiAnnulus(Annulus stRoiValue) = 0;

            /// @~chinese 掩膜图像（仅当次执行起效）   @~english Mask Image(Only valid for this execution)
            virtual void SetMaskImage(ImageBaseData stMaskImage) = 0;

        };

        /*  Note: Interface Classes of the parameter of the ThresholdRangeItemParam.  */
        class ThresholdRangeItemParam
        {
        protected:
            ///< Constructor and Destructor
            explicit ThresholdRangeItemParam() {}
            virtual ~ThresholdRangeItemParam() {}

        public:

            /// @~chinese 最大值   @~english Max Value
            __declspec(property(put = SetMaxValue, get = GetMaxValue)) int MaxValue;

            virtual int GetMaxValue() = 0;

            virtual void SetMaxValue(int value) = 0;

            /// @~chinese最小值   @~english Min Value
            __declspec(property(put = SetMinValue, get = GetMinValue)) int MinValue;

            virtual int GetMinValue() = 0;

            virtual void SetMinValue(int value) = 0;
        };

        /*  Note: Interface Classes of the parameter of the PixelCountVAParams.  */
        ///像素统计参数类
        class PixelCountVAParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit PixelCountVAParams() {}
            virtual ~PixelCountVAParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual PixelCountVARoiManager* GetModuRoiManager() = 0;

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

			/**
             * @brief 根据索引名称获取阈值范围动态参数项（最大值/最小值等）（单例模式）
			 * @param strName   [IN]  参数名称为阈值范围下的序号，如"0"
             * @return 阈值范围参数项
             */
            virtual ThresholdRangeItemParam * GetThresholdRangeItemParam(const char * const strName) = 0;
        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the PixelCountVAResults.  */
        ///像素统计结果类
        class PixelCountVAResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit PixelCountVAResults() {}
            virtual ~PixelCountVAResults() {}

        public:
             /**
             * @brief 输出掩膜
             */
             virtual ImageBaseData GetOutputMask() = 0;

             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;

			 /**
             * @brief ROI圆弧
             */
             virtual Annulus GetROIAnnulus() = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

			 /**
              * @brief 像素数量
			 */
			 virtual int GetCount(int nIndex) = 0;

			 /**
             * @brief 比率
			 */
			 virtual float GetRatio(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the PixelCountVA tool.  */
        ///像素统计工具类
        class IMVSPixelCountModuVATool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSPixelCountModuVATool() {}
            virtual ~IMVSPixelCountModuVATool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual PixelCountVAParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual PixelCountVAResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSPixelCountModuVATool(IMVSPixelCountModuVATool&);
            IMVSPixelCountModuVATool& operator=(IMVSPixelCountModuVATool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for PixelCountVA.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSPixelCountModuVA::IMVSPixelCountModuVATool * __stdcall GetPixelCountVAToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_PIXELCOUNTVA_H_
