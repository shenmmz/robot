/***************************************************************************************************
 * File：IVmImageCalib.h
 * Note：@~chinese 畸变标定模块接口声明 @~english Interface for the ImageCalib tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_IMAGECALIB_H_
#define _IVM_IMAGECALIB_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSImageCalibModu
    {
        /// @~chinese 畸变类型   @~english Distortion Type
        typedef enum _CorrectModelEnum
        {
            /// @~chinese 透视畸变   @~english Perspective Distortion
            CorrectModel_CorrectModelLinear = 0x1,

            /// @~chinese 径向畸变   @~english Radial Distortion
            CorrectModel_CorrectModelRadial = 0x2,

            /// @~chinese 径向透视畸变   @~english Radial Perspective Distortion
            CorrectModel_CorrectModelPersRadial = 0x3,

        }CorrectModelEnum;

        /// @~chinese 标定板类型   @~english Calibration Board Type
        typedef enum _CalibBoardTypeEnum
        {
            /// @~chinese 棋盘格标定板   @~english Checkboard Calibration Board
            CalibBoardType_TypeChecker = 0x1,

            /// @~chinese 圆标定板   @~english Round Calibration Board
            CalibBoardType_TypeCircle = 0x2,

            /// @~chinese 海康I型标定板   @~english Hikvision I Calibration Board
            CalibBoardType_TypeHKI = 0x7,

            /// @~chinese 海康II型标定板   @~english Hikvision II Calibration Board
            CalibBoardType_TypeHKII = 0x6,

            /// @~chinese 海康扩展I型标定板   @~english Hikvision_IV2 Calibration Board
            CalibBoardType_TypeHKIV2 = 0x9,

            /// @~chinese 海康扩展II型标定板   @~english Hikvision_IIV2 Calibration Board
            CalibBoardType_TypeHKIIV2 = 0x8,

        }CalibBoardTypeEnum;

        /// @~chinese 自由度   @~english DOF
        typedef enum _HomoFreedomEnum
        {
            /// @~chinese 缩放、旋转、纵横比、倾斜、平移及透射   @~english Scale、Rotation、Aspect Ratio、Tilt、Translation and Transmission
            HomoFreedom_Perspective = 0x1,

            /// @~chinese 缩放、旋转、纵横比、倾斜及平移   @~english Scale、Rotation、Aspect Ratio、Tilt and Translation
            HomoFreedom_Affine = 0x2,

            /// @~chinese 缩放、旋转及平移   @~english Scale、Rotation and Translation
            HomoFreedom_Similarity = 0x3,

        }HomoFreedomEnum;

        /// @~chinese 中值滤波状态   @~english Median Filtering State
        typedef enum _FilterStatusEnum
        {
            /// @~chinese 执行滤波   @~english Execution Filtering
            FilterStatus_FilterStateTure = 0x1,

            /// @~chinese 无滤波   @~english No Filter
            FilterStatus_FilterStateFalse = 0x2,

        }FilterStatusEnum;

        /// @~chinese 亚像素窗口   @~english Sub-pixel window
        typedef enum _SubpixelWinAutoEnum
        {
            /// @~chinese 自适应   @~english Self-Adaption
            SubpixelWinAuto_SubpixelWinSizeAuto = 0x1,

            /// @~chinese 设置值   @~english Set
            SubpixelWinAuto_SubpixelWinSizeUser = 0x2,

        }SubpixelWinAutoEnum;

        /// @~chinese 圆点类型   @~english Dot Type
        typedef enum _CircleBoardCircleModeEnum
        {
            /// @~chinese 白底黑圆   @~english Black Dot with White Background
            CircleBoardCircleMode_CircleModeBlack = 0x1,

            /// @~chinese 黑底白圆   @~english White Dot with Black Background
            CircleBoardCircleMode_CircleModeWhite = 0x2,

        }CircleBoardCircleModeEnum;

        /// @~chinese 权重函数   @~english Weighting Function
        typedef enum _CalibBoardWeightFunEnum
        {
            /// @~chinese 最小二乘   @~english Least Squares
            CalibBoardWeightFun_LeastSqure = 0x1,

            /// @~chinese Huber   @~english Huber
            CalibBoardWeightFun_Huber = 0x2,

            /// @~chinese Tukey   @~english Tukey
            CalibBoardWeightFun_Tukey = 0x3,

        }CalibBoardWeightFunEnum;

    /// \addtogroup 畸变标定
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the ImageCalibParams.  */
        ///畸变标定参数类
        class ImageCalibParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageCalibParams() {}
            virtual ~ImageCalibParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 校正中心点(仅当次执行起效)   @~english CorrectCenterPoint(Only valid for this execution)
            virtual void SetCorrectCenterPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 标定文件路径   @~english Set Calibration File Path
            __declspec(property(put = SetCalibPathName, get = GetCalibPathName)) const char* CalibPathName;

            virtual const char* GetCalibPathName() = 0;

            virtual void SetCalibPathName(const char* value) = 0;

            /// @~chinese 更新文件   @~english Update File
            __declspec(property(put = SetRefreshFileEnable, get = GetRefreshFileEnable)) bool RefreshFileEnable;

            virtual bool GetRefreshFileEnable() = 0;

            virtual void SetRefreshFileEnable(bool value) = 0;

            /// @~chinese 畸变类型   @~english Distortion Type
            __declspec(property(put = SetCorrectModel, get = GetCorrectModel)) CorrectModelEnum CorrectModel;

            virtual CorrectModelEnum GetCorrectModel() = 0;

            virtual void SetCorrectModel(CorrectModelEnum value) = 0;

            /// @~chinese 矫正评估使能   @~english Enable Distortion Evaluation
            __declspec(property(put = SetDistortionEvaluationEnable, get = GetDistortionEvaluationEnable)) bool DistortionEvaluationEnable;

            virtual bool GetDistortionEvaluationEnable() = 0;

            virtual void SetDistortionEvaluationEnable(bool value) = 0;

            /// @~chinese 输出矫正后数据使能   @~english Enable Output Post-Correction Data
            __declspec(property(put = SetOutputPostCorrectionEnable, get = GetOutputPostCorrectionEnable)) bool OutputPostCorrectionEnable;

            virtual bool GetOutputPostCorrectionEnable() = 0;

            virtual void SetOutputPostCorrectionEnable(bool value) = 0;

            /// @~chinese 评估误差阈值，范围：[0,10]   @~english Evaluation Error Threshold，range:[0,10]
            __declspec(property(put = SetPointErrorThreshold, get = GetPointErrorThreshold)) double PointErrorThreshold;

            virtual double GetPointErrorThreshold() = 0;

            virtual void SetPointErrorThreshold(double value) = 0;

            /// @~chinese 标定板类型   @~english Calibration Board Type
            __declspec(property(put = SetCalibBoardType, get = GetCalibBoardType)) CalibBoardTypeEnum CalibBoardType;

            virtual CalibBoardTypeEnum GetCalibBoardType() = 0;

            virtual void SetCalibBoardType(CalibBoardTypeEnum value) = 0;

            /// @~chinese 自由度   @~english DOF
            __declspec(property(put = SetHomoFreedom, get = GetHomoFreedom)) HomoFreedomEnum HomoFreedom;

            virtual HomoFreedomEnum GetHomoFreedom() = 0;

            virtual void SetHomoFreedom(HomoFreedomEnum value) = 0;

            /// @~chinese 灰度对比度，范围：[1,255](弃用)   @~english Grayscale Contrast，range:[1,255](Obsolete)
            __declspec(property(put = SetGrayContrast, get = GetGrayContrast)) int GrayContrast;

            virtual int GetGrayContrast() = 0;

            virtual void SetGrayContrast(int value) = 0;

            /// @~chinese 中值滤波状态   @~english Median Filtering State
            __declspec(property(put = SetFilterStatus, get = GetFilterStatus)) FilterStatusEnum FilterStatus;

            virtual FilterStatusEnum GetFilterStatus() = 0;

            virtual void SetFilterStatus(FilterStatusEnum value) = 0;

            /// @~chinese 亚像素窗口(弃用)   @~english Sub-pixel window(Obsolete)
            __declspec(property(put = SetSubpixelWinAuto, get = GetSubpixelWinAuto)) SubpixelWinAutoEnum SubpixelWinAuto;

            virtual SubpixelWinAutoEnum GetSubpixelWinAuto() = 0;

            virtual void SetSubpixelWinAuto(SubpixelWinAutoEnum value) = 0;

            /// @~chinese 设置窗口大小，范围：[3,150](弃用)   @~english Set window size，range:[3,150](Obsolete)
            __declspec(property(put = SetSubPixelWindowSize, get = GetSubPixelWindowSize)) int SubPixelWindowSize;

            virtual int GetSubPixelWindowSize() = 0;

            virtual void SetSubPixelWindowSize(int value) = 0;

            /// @~chinese 点圆度，范围：[20,100]   @~english Dot Circularity，range:[20,100]
            __declspec(property(put = SetCircularity, get = GetCircularity)) int Circularity;

            virtual int GetCircularity() = 0;

            virtual void SetCircularity(int value) = 0;

            /// @~chinese 边缘提取阈值，范围：[0,255]   @~english Edge Threshold，range:[0,255]
            __declspec(property(put = SetEdgeThreshLow, get = GetEdgeThreshLow)) int EdgeThreshLow;

            virtual int GetEdgeThreshLow() = 0;

            virtual void SetEdgeThreshLow(int value) = 0;

            /// @~chinese 边缘提取阈值，范围：[0,255]   @~english Edge Threshold，range:[0,255]
            __declspec(property(put = SetEdgeThreshHigh, get = GetEdgeThreshHigh)) int EdgeThreshHigh;

            virtual int GetEdgeThreshHigh() = 0;

            virtual void SetEdgeThreshHigh(int value) = 0;

            /// @~chinese 圆点类型   @~english Dot Type
            __declspec(property(put = SetCircleBoardCircleMode, get = GetCircleBoardCircleMode)) CircleBoardCircleModeEnum CircleBoardCircleMode;

            virtual CircleBoardCircleModeEnum GetCircleBoardCircleMode() = 0;

            virtual void SetCircleBoardCircleMode(CircleBoardCircleModeEnum value) = 0;

            /// @~chinese 权重函数   @~english Weighting Function
            __declspec(property(put = SetCalibBoardWeightFun, get = GetCalibBoardWeightFun)) CalibBoardWeightFunEnum CalibBoardWeightFun;

            virtual CalibBoardWeightFunEnum GetCalibBoardWeightFun() = 0;

            virtual void SetCalibBoardWeightFun(CalibBoardWeightFunEnum value) = 0;

            /// @~chinese 权重系数，范围：[1,200]   @~english Weighting Coefficient，range:[1,200]
            __declspec(property(put = SetWeightFactor, get = GetWeightFactor)) int WeightFactor;

            virtual int GetWeightFactor() = 0;

            virtual void SetWeightFactor(int value) = 0;

            /// @~chinese 距离阈值，范围：[1,100]   @~english Distance Threshold，range:[1,100]
            __declspec(property(put = SetDistThreshold, get = GetDistThreshold)) int DistThreshold;

            virtual int GetDistThreshold() = 0;

            virtual void SetDistThreshold(int value) = 0;

            /// @~chinese 采样率，范围：[1,100]   @~english Sampling Rate，range:[1,100]
            __declspec(property(put = SetSampleRatio, get = GetSampleRatio)) int SampleRatio;

            virtual int GetSampleRatio() = 0;

            virtual void SetSampleRatio(int value) = 0;

            /// @~chinese 生成标定文件   @~english Create Calibration File
            virtual int DoSaveFile(IN const char * const strValue) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes for describing distort IImage locate process result information.  */
		/// 角点定位结果类
        class IImageCalibLocateInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IImageCalibLocateInfo() {}
            virtual ~IImageCalibLocateInfo() {}

        public:
            ///< Property
            /**
             * @brief 标定点数量
             */
            virtual int GetCalibPointNum() = 0;

            /**
             * @brief 获取标定点的图像坐标
             * @param nIndex [IN] 查询点的索引值，从0开始编码
             * @return 点坐标
             */
            virtual MVD_POINT_F GetCalibPointImageCoordinate(int nIndex) = 0;
        };

        /*  Note: Interface Classes for describing distort IImage homo process result information.  */
        class IImageCalibHomoEstInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IImageCalibHomoEstInfo() {}
            virtual ~IImageCalibHomoEstInfo() {}

        public:
            /**
             * @brief 估计误差
             */
            virtual float GetEstimationError() = 0;
        };

        /*  Note: Interface Classes of the result of the ImageCalibResults.  */
        ///畸变标定结果类
        class ImageCalibResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageCalibResults() {}
            virtual ~ImageCalibResults() {}

        public:
             /**
             * @brief 矫正后的图像
             */
            virtual ImageBaseData GetPostCorrectionImage() = 0;

             /**
             * @brief 标定点评估误差
             */
             virtual float GetCalibPointError() = 0;

             /**
             * @brief 评估误差
             */
             virtual float GetEvaluationError() = 0;

             /**
             * @brief 标定点状态
             * @param nIndex[IN] 数据索引
             */
             virtual float GetCalibPointStatus(int nIndex) = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;
            /**
             * @brief 描述标定板位置信息
             * @return 标定板位置信息
             */
            virtual IImageCalibLocateInfo* GetLocateInfo() = 0;
            /**
             * @brief 标定板Homo估计信息
             * @return Homo估计信息
             */
            virtual IImageCalibHomoEstInfo* GetHomoEstInfo() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ImageCalib tool.  */
        ///畸变标定工具类
        class IMVSImageCalibModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSImageCalibModuTool() {}
            virtual ~IMVSImageCalibModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ImageCalibParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ImageCalibResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSImageCalibModuTool(IMVSImageCalibModuTool&);
            IMVSImageCalibModuTool& operator=(IMVSImageCalibModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ImageCalib.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSImageCalibModu::IMVSImageCalibModuTool * __stdcall GetImageCalibToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_IMAGECALIB_H_
