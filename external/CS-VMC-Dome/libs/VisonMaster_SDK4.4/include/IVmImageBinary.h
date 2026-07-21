/***************************************************************************************************
 * File：IVmImageBinary.h
 * Note：@~chinese 图像二值化模块接口声明 @~english Interface for the ImageBinary tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_IMAGEBINARY_H_
#define _IVM_IMAGEBINARY_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSBinaryModu
    {
        /// @~chinese ROI外处理   @~english Outside Of ROI
        typedef enum _NonROIPrcTypeEnum
        {
            /// @~chinese 黑色   @~english Black
            NonROIPrcType_Zero = 0,

            /// @~chinese 原图   @~english Original Image
            NonROIPrcType_Src = 1,

        }NonROIPrcTypeEnum;

        /// @~chinese 二值化类型   @~english Binarization Type
        typedef enum _BinaryTypeEnum
        {
            /// @~chinese 硬阈值二值化   @~english Hard Threshold Binarization
            BinaryType_HardThreshold = 0x1,

            /// @~chinese 均值二值化   @~english Mean Binarization
            BinaryType_MeanThreshold = 0x2,

            /// @~chinese 高斯二值化   @~english Gaussian Binarization
            BinaryType_GaussianThreshold = 0x3,

            /// @~chinese Sauvola二值化   @~english Sauvola Binarization
            BinaryType_Sauvola = 0x8,

            /// @~chinese 自动   @~english Auto
            BinaryType_OTSU = 0x4,

        }BinaryTypeEnum;

        /// @~chinese 比较类型   @~english Comparison Type
        typedef enum _CompareTypeEnum
        {
            /// @~chinese 大于等于   @~english Greater Than or Equal to
            CompareType_GreaterEq = 0x0,

            /// @~chinese 小于等于   @~english Less Than or Equal to
            CompareType_LessEq = 0x1,

            /// @~chinese 等于   @~english Equal to
            CompareType_Equal = 0x2,

            /// @~chinese 不等于   @~english Not Equal to
            CompareType_NotEqual = 0x3,

        }CompareTypeEnum;

        /// @~chinese 分割类型   @~english Segment Type
        typedef enum _SauvolaSegTypeEnum
        {
            /// @~chinese 暗于背景   @~english Darker Than Background
            SauvolaSegType_Dark = 0x0,

            /// @~chinese 亮于背景   @~english Brighter Than Bachground
            SauvolaSegType_Bright = 0x1,

        }SauvolaSegTypeEnum;

    /// \addtogroup 图像二值化
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///图像二值化ROI管理类
        class ImageBinaryRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
            virtual void SetRoiAnnulus(Annulus stRoiValue) = 0;

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoisRectangle(RectBox* pRoiValue, unsigned int nValueNum) = 0;

            /// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
            virtual void SetRoisAnnulus(Annulus* pRoiValue, unsigned int nValueNum) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

            /// @~chinese 掩膜图像（仅当次执行起效）   @~english Mask Image(Only valid for this execution)
            virtual void SetMaskImage(ImageBaseData stMaskImage) = 0;

        };

        /*  Note: Interface Classes of the parameter of the ImageBinaryParams.  */
        ///图像二值化参数类
        class ImageBinaryParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageBinaryParams() {}
            virtual ~ImageBinaryParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual ImageBinaryRoiManager* GetModuRoiManager() = 0;

            /// @~chinese ROI外处理   @~english Outside Of ROI
            __declspec(property(put = SetNonROIPrcType, get = GetNonROIPrcType)) NonROIPrcTypeEnum NonROIPrcType;

            virtual NonROIPrcTypeEnum GetNonROIPrcType() = 0;

            virtual void SetNonROIPrcType(NonROIPrcTypeEnum value) = 0;

            /// @~chinese 二值化类型   @~english Binarization Type
            __declspec(property(put = SetBinaryType, get = GetBinaryType)) BinaryTypeEnum BinaryType;

            virtual BinaryTypeEnum GetBinaryType() = 0;

            virtual void SetBinaryType(BinaryTypeEnum value) = 0;

            /// @~chinese 低阈值，范围：[0,255]   @~english Low Threshold，range:[0,255]
            __declspec(property(put = SetLowThreshold, get = GetLowThreshold)) int LowThreshold;

            virtual int GetLowThreshold() = 0;

            virtual void SetLowThreshold(int value) = 0;

            /// @~chinese 高阈值，范围：[0,255]   @~english High Threshold，range:[0,255]
            __declspec(property(put = SetHighThreshold, get = GetHighThreshold)) int HighThreshold;

            virtual int GetHighThreshold() = 0;

            virtual void SetHighThreshold(int value) = 0;

            /// @~chinese 滤波核宽度，范围：[1,51]   @~english Filter Kernel Width，range:[1,51]
            __declspec(property(put = SetKernelWidth, get = GetKernelWidth)) int KernelWidth;

            virtual int GetKernelWidth() = 0;

            virtual void SetKernelWidth(int value) = 0;

            /// @~chinese 滤波核高度，范围：[1,51]   @~english Filter Kernel Height，range:[1,51]
            __declspec(property(put = SetKernelHeight, get = GetKernelHeight)) int KernelHeight;

            virtual int GetKernelHeight() = 0;

            virtual void SetKernelHeight(int value) = 0;

            /// @~chinese 高斯滤波核，范围：[1,51]   @~english Gaussian Filter Kernel，range:[1,51]
            __declspec(property(put = SetGaussKernelSize, get = GetGaussKernelSize)) int GaussKernelSize;

            virtual int GetGaussKernelSize() = 0;

            virtual void SetGaussKernelSize(int value) = 0;

            /// @~chinese 高斯标准差，范围：[0.1,100]   @~english Gaussian Standard Deviation，range:[0.1,100]
            __declspec(property(put = SetGaussSigma, get = GetGaussSigma)) double GaussSigma;

            virtual double GetGaussSigma() = 0;

            virtual void SetGaussSigma(double value) = 0;

            /// @~chinese 比较类型   @~english Comparison Type
            __declspec(property(put = SetCompareType, get = GetCompareType)) CompareTypeEnum CompareType;

            virtual CompareTypeEnum GetCompareType() = 0;

            virtual void SetCompareType(CompareTypeEnum value) = 0;

            /// @~chinese 阈值偏移量，范围：[-255,255]   @~english Threshold Offset，range:[-255,255]
            __declspec(property(put = SetThresholdOffset, get = GetThresholdOffset)) int ThresholdOffset;

            virtual int GetThresholdOffset() = 0;

            virtual void SetThresholdOffset(int value) = 0;

            /// @~chinese 校正系数，范围：[-1.0,1.0]   @~english Adjust Coefficient，range:[-1.0,1.0]
            __declspec(property(put = SetSauvolaAdjustCoef, get = GetSauvolaAdjustCoef)) double SauvolaAdjustCoef;

            virtual double GetSauvolaAdjustCoef() = 0;

            virtual void SetSauvolaAdjustCoef(double value) = 0;

            /// @~chinese 动态范围，范围：[0.1,255]   @~english Range，range:[0.1,255]
            __declspec(property(put = SetSauvolaRange, get = GetSauvolaRange)) double SauvolaRange;

            virtual double GetSauvolaRange() = 0;

            virtual void SetSauvolaRange(double value) = 0;

            /// @~chinese 分割类型   @~english Segment Type
            __declspec(property(put = SetSauvolaSegType, get = GetSauvolaSegType)) SauvolaSegTypeEnum SauvolaSegType;

            virtual SauvolaSegTypeEnum GetSauvolaSegType() = 0;

            virtual void SetSauvolaSegType(SauvolaSegTypeEnum value) = 0;

            /// @~chinese 滤波核宽度，范围：[1,100]   @~english Filter Kernel Width，range:[1,100]
            __declspec(property(put = SetSauvolaWinWidth, get = GetSauvolaWinWidth)) int SauvolaWinWidth;

            virtual int GetSauvolaWinWidth() = 0;

            virtual void SetSauvolaWinWidth(int value) = 0;

            /// @~chinese 滤波核高度，范围：[1,100]   @~english Filter Kernel Height，range:[1,100]
            __declspec(property(put = SetSauvolaWinHeight, get = GetSauvolaWinHeight)) int SauvolaWinHeight;

            virtual int GetSauvolaWinHeight() = 0;

            virtual void SetSauvolaWinHeight(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ImageBinaryResults.  */
        ///图像二值化结果类
        class ImageBinaryResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageBinaryResults() {}
            virtual ~ImageBinaryResults() {}

        public:
             /**
             * @brief 屏蔽区字符串
             */
             virtual StringData* GetBlindPolygonString(unsigned int& nValueNum) = 0;

             /**
             * @brief 屏蔽区多边形点集
             */
             virtual PointF GetBlindPolygonPoints(int nIndex) = 0;

             /**
             * @brief 屏蔽区多边形点数
             */
             virtual int GetBlindPolygonPointNum(int nIndex) = 0;

             /**
             * @brief 输出掩膜
             */
             virtual ImageBaseData GetOutputMask() = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;
             /**
             * @brief ROI圆弧
             */
             virtual Annulus GetROIAnnulus() = 0;
			 /**
			 * @brief 检测区域
			 */
			 virtual RectBox GetROI(int nIndex) = 0;
			 /**
			 * @brief ROI圆弧
			 */
			 virtual Annulus GetROIAnnulus(int nIndex) = 0;
            /**
             * @brief 输出图像
             */
            virtual ImageBaseData GetOutputImage() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ImageBinary tool.  */
        ///图像二值化工具类
        class IMVSBinaryModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSBinaryModuTool() {}
            virtual ~IMVSBinaryModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ImageBinaryParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ImageBinaryResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSBinaryModuTool(IMVSBinaryModuTool&);
            IMVSBinaryModuTool& operator=(IMVSBinaryModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ImageBinary.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSBinaryModu::IMVSBinaryModuTool * __stdcall GetImageBinaryToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_IMAGEBINARY_H_
