/***************************************************************************************************
 * File：IVmImageFilter.h
 * Note：@~chinese 图像滤波模块接口声明 @~english Interface for the ImageFilter tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_IMAGEFILTER_H_
#define _IVM_IMAGEFILTER_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSImageFilterModu
    {
        /// @~chinese 图像滤波类型   @~english Filter Type
        typedef enum _FilterTypeEnum
        {
            /// @~chinese 高斯   @~english Gaussian Filter
            FilterType_Gaussian = 0x1,

            /// @~chinese 中值   @~english LumMedian
            FilterType_Median = 0x2,

            /// @~chinese 均值   @~english LumMean
            FilterType_Mean = 0x3,

            /// @~chinese 取反   @~english Inverse Filter
            FilterType_Invert = 0x4,

            /// @~chinese 边缘提取   @~english Edge Extraction
            FilterType_Edge = 0x5,

        }FilterTypeEnum;

    /// \addtogroup 图像滤波
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///图像滤波ROI管理类
        class ImageFilterRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
            virtual void SetRoiAnnulus(Annulus stRoiValue) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

            /// @~chinese 掩膜图像（仅当次执行起效）   @~english Mask Image(Only valid for this execution)
            virtual void SetMaskImage(ImageBaseData stMaskImage) = 0;

        };

        /*  Note: Interface Classes of the parameter of the ImageFilterParams.  */
        ///图像滤波参数类
        class ImageFilterParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageFilterParams() {}
            virtual ~ImageFilterParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual ImageFilterRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 图像滤波类型   @~english Filter Type
            __declspec(property(put = SetFilterType, get = GetFilterType)) FilterTypeEnum FilterType;

            virtual FilterTypeEnum GetFilterType() = 0;

            virtual void SetFilterType(FilterTypeEnum value) = 0;

            /// @~chinese 滤波核宽度，范围：[1,101]   @~english Filter Kernel Width，range:[1,101]
            __declspec(property(put = SetKernelWidth, get = GetKernelWidth)) int KernelWidth;

            virtual int GetKernelWidth() = 0;

            virtual void SetKernelWidth(int value) = 0;

            /// @~chinese 滤波核高度，范围：[1,101]   @~english Filter Kernel Height，range:[1,101]
            __declspec(property(put = SetKernelHeight, get = GetKernelHeight)) int KernelHeight;

            virtual int GetKernelHeight() = 0;

            virtual void SetKernelHeight(int value) = 0;

            /// @~chinese 高斯滤波核，范围：[1,101]   @~english Gaussian Filter Kernel，range:[1,101]
            __declspec(property(put = SetGaussKernelSize, get = GetGaussKernelSize)) int GaussKernelSize;

            virtual int GetGaussKernelSize() = 0;

            virtual void SetGaussKernelSize(int value) = 0;

            /// @~chinese 边缘阈值范围，范围：[0,255]   @~english Edge Threshold Range，range:[0,255]
            __declspec(property(put = SetEdegLowThreshold, get = GetEdegLowThreshold)) int EdegLowThreshold;

            virtual int GetEdegLowThreshold() = 0;

            virtual void SetEdegLowThreshold(int value) = 0;

            /// @~chinese 边缘阈值范围，范围：[0,255]   @~english Edge Threshold Range，range:[0,255]
            __declspec(property(put = SetEdegHighThreshold, get = GetEdegHighThreshold)) int EdegHighThreshold;

            virtual int GetEdegHighThreshold() = 0;

            virtual void SetEdegHighThreshold(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ImageFilterResults.  */
        ///图像滤波结果类
        class ImageFilterResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageFilterResults() {}
            virtual ~ImageFilterResults() {}

        public:
             /**
             * @brief 屏蔽区字符串
             */
             virtual StringData* GetBlindPolygonString(unsigned int& nValueNum) = 0;

             /**
             * @brief 输出掩膜
             */
             virtual ImageBaseData GetOutputMask() = 0;

             /**
             * @brief 屏蔽区多边形点集
             */
             virtual PointF GetBlindPolygonPoints(int nIndex) = 0;

             /**
             * @brief 屏蔽区多边形点数
             */
             virtual int GetBlindPolygonPointNum(int nIndex) = 0;

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
             * @brief 输出图像
             */
            virtual ImageBaseData GetOutputImage() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ImageFilter tool.  */
        ///图像滤波工具类
        class IMVSImageFilterModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSImageFilterModuTool() {}
            virtual ~IMVSImageFilterModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ImageFilterParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ImageFilterResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSImageFilterModuTool(IMVSImageFilterModuTool&);
            IMVSImageFilterModuTool& operator=(IMVSImageFilterModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ImageFilter.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSImageFilterModu::IMVSImageFilterModuTool * __stdcall GetImageFilterToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_IMAGEFILTER_H_
