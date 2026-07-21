/***************************************************************************************************
 * File：IVmImageNormlize.h
 * Note：@~chinese 图像归一化模块接口声明 @~english Interface for the ImageNormlize tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_IMAGENORMLIZE_H_
#define _IVM_IMAGENORMLIZE_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSImageNormlizeModu
    {
        /// @~chinese 归一化类型   @~english Normalization Type
        typedef enum _NormlizeTypeEnum
        {
            /// @~chinese 直方图均衡化   @~english Histogram Equalization
            NormlizeType_HistEqual = 0x1,

            /// @~chinese 直方图归一化   @~english Histogram Normalization
            NormlizeType_HistMatch = 0x2,

            /// @~chinese 均值标准差归一化   @~english Standard Deviation Normalization
            NormlizeType_MeanStd = 0x3,

        }NormlizeTypeEnum;

    /// \addtogroup 图像归一化
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///图像归一化ROI管理类
        class ImageNormlizeRoiManager
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

        /*  Note: Interface Classes of the parameter of the ImageNormlizeParams.  */
        ///图像归一化参数类
        class ImageNormlizeParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageNormlizeParams() {}
            virtual ~ImageNormlizeParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual ImageNormlizeRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 归一化类型   @~english Normalization Type
            __declspec(property(put = SetNormlizeType, get = GetNormlizeType)) NormlizeTypeEnum NormlizeType;

            virtual NormlizeTypeEnum GetNormlizeType() = 0;

            virtual void SetNormlizeType(NormlizeTypeEnum value) = 0;

            /// @~chinese 均值，范围：[0,255]   @~english LumMean，range:[0,255]
            __declspec(property(put = SetMeanVal, get = GetMeanVal)) double MeanVal;

            virtual double GetMeanVal() = 0;

            virtual void SetMeanVal(double value) = 0;

            /// @~chinese 标准差，范围：[0.0,255]   @~english LumStd，range:[0.0,255]
            __declspec(property(put = SetStdVal, get = GetStdVal)) double StdVal;

            virtual double GetStdVal() = 0;

            virtual void SetStdVal(double value) = 0;

            /// @~chinese 直方图比例，范围：[0.0,1]   @~english Histogram Ratio，range:[0.0,1]
            __declspec(property(put = SetLeftClipPercent, get = GetLeftClipPercent)) double LeftClipPercent;

            virtual double GetLeftClipPercent() = 0;

            virtual void SetLeftClipPercent(double value) = 0;

            /// @~chinese 直方图比例，范围：[0.0,1]   @~english Histogram Ratio，range:[0.0,1]
            __declspec(property(put = SetRightClipPercent, get = GetRightClipPercent)) double RightClipPercent;

            virtual double GetRightClipPercent() = 0;

            virtual void SetRightClipPercent(double value) = 0;

            /// @~chinese 灰度值范围，范围：[0.0,255]   @~english Gray Value Range，range:[0.0,255]
            __declspec(property(put = SetDstLeftPos, get = GetDstLeftPos)) double DstLeftPos;

            virtual double GetDstLeftPos() = 0;

            virtual void SetDstLeftPos(double value) = 0;

            /// @~chinese 灰度值范围，范围：[0.0,255]   @~english Gray Value Range，range:[0.0,255]
            __declspec(property(put = SetDstRightPos, get = GetDstRightPos)) double DstRightPos;

            virtual double GetDstRightPos() = 0;

            virtual void SetDstRightPos(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ImageNormlizeResults.  */
        ///图像归一化结果类
        class ImageNormlizeResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageNormlizeResults() {}
            virtual ~ImageNormlizeResults() {}

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
             * @brief 输出图像
             */
            virtual ImageBaseData GetOutputImage() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ImageNormlize tool.  */
        ///图像归一化工具类
        class IMVSImageNormlizeModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSImageNormlizeModuTool() {}
            virtual ~IMVSImageNormlizeModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ImageNormlizeParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ImageNormlizeResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSImageNormlizeModuTool(IMVSImageNormlizeModuTool&);
            IMVSImageNormlizeModuTool& operator=(IMVSImageNormlizeModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ImageNormlize.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSImageNormlizeModu::IMVSImageNormlizeModuTool * __stdcall GetImageNormlizeToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_IMAGENORMLIZE_H_
