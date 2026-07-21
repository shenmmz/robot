/***************************************************************************************************
 * File：IVmMapCalib.h
 * Note：@~chinese 映射标定模块接口声明 @~english Interface for the MapCalib tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_MAPCALIB_H_
#define _IVM_MAPCALIB_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSMapCalibModu
    {
        /// @~chinese 输入方式   @~english Input Mode
        typedef enum _InputTypeEnum
        {
            /// @~chinese 图像输入   @~english Image Input
            InputType_InputImage = 0x1,

            /// @~chinese 文件输入   @~english File Input
            InputType_InputFile = 0x2,

        }InputTypeEnum;

        /// @~chinese 标定板类型   @~english Calibration Board Type
        typedef enum _CalibBoardTypeEnum
        {
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

        /// @~chinese 权重函数   @~english Weighting Function
        typedef enum _WeightFunEnum
        {
            /// @~chinese 最小二乘   @~english Least Squares
            WeightFun_LeastSqure = 0x1,

            /// @~chinese Huber   @~english Huber
            WeightFun_Huber = 0x2,

            /// @~chinese Tukey   @~english Tukey
            WeightFun_Tukey = 0x3,

        }WeightFunEnum;

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

        /// @~chinese 中值滤波状态   @~english Median Filtering State
        typedef enum _FilterStatus2Enum
        {
            /// @~chinese 执行滤波   @~english Execution Filtering
            FilterStatus2_FilterStateTure = 0x1,

            /// @~chinese 无滤波   @~english No Filter
            FilterStatus2_FilterStateFalse = 0x2,

        }FilterStatus2Enum;

        /// @~chinese 亚像素窗口   @~english Sub-pixel window
        typedef enum _SubpixelWinAuto2Enum
        {
            /// @~chinese 自适应   @~english Self-Adaption
            SubpixelWinAuto2_SubpixelWinSizeAuto = 0x1,

            /// @~chinese 设置值   @~english Set
            SubpixelWinAuto2_SubpixelWinSizeUser = 0x2,

        }SubpixelWinAuto2Enum;

    /// \addtogroup 映射标定
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the MapCalibParams.  */
        ///映射标定参数类
        class MapCalibParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MapCalibParams() {}
            virtual ~MapCalibParams() {}

        public:

            /// @~chinese 输入图像1(仅当次执行起效)   @~english InputImage1(Only valid for this execution)
            virtual void SetInputImage1(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入图像2(仅当次执行起效)   @~english InputImage2(Only valid for this execution)
            virtual void SetInputImage2(ImageBaseData* pInputValue) = 0;

            /// @~chinese 刷新信号(仅当次执行起效)   @~english Refresh Signal(Only valid for this execution)
            virtual void SetRefreshSignal(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 物理点(仅当次执行起效)   @~english Physical Point(Only valid for this execution)
            virtual void SetPhysicalPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 物理角度(仅当次执行起效)   @~english World Rotate Angle(Only valid for this execution)
            virtual void SetWorldRotateAngle(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 外部输入字符(仅当次执行起效)   @~english External Input Character(Only valid for this execution)
            virtual void SetTrigger(InputStringData* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 外部触发字符(仅当次执行起效)   @~english External Triggered Character(Only valid for this execution)
            virtual void SetTeachFlagInput(InputStringData* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入方式   @~english Input Mode
            __declspec(property(put = SetInputType, get = GetInputType)) InputTypeEnum InputType;

            virtual InputTypeEnum GetInputType() = 0;

            virtual void SetInputType(InputTypeEnum value) = 0;

            /// @~chinese 示教   @~english Demonstration
            __declspec(property(put = SetTeachEnable, get = GetTeachEnable)) bool TeachEnable;

            virtual bool GetTeachEnable() = 0;

            virtual void SetTeachEnable(bool value) = 0;

            /// @~chinese 外部触发字符   @~english External Triggered Character
            __declspec(property(put = SetTeachFlag, get = GetTeachFlag)) const char* TeachFlag;

            virtual const char* GetTeachFlag() = 0;

            virtual void SetTeachFlag(const char* value) = 0;

            /// @~chinese 标定文件1   @~english Calibration File1
            __declspec(property(put = SetCalibPath0, get = GetCalibPath0)) const char* CalibPath0;

            virtual const char* GetCalibPath0() = 0;

            virtual void SetCalibPath0(const char* value) = 0;

            /// @~chinese 标定文件2   @~english Calibration File2
            __declspec(property(put = SetCalibPath1, get = GetCalibPath1)) const char* CalibPath1;

            virtual const char* GetCalibPath1() = 0;

            virtual void SetCalibPath1(const char* value) = 0;

            /// @~chinese 标定文件路径   @~english Set Calibration File Path
            __declspec(property(put = SetCalibPathName, get = GetCalibPathName)) const char* CalibPathName;

            virtual const char* GetCalibPathName() = 0;

            virtual void SetCalibPathName(const char* value) = 0;

            /// @~chinese 更新文件   @~english Update File
            __declspec(property(put = SetRefreshFileEnable, get = GetRefreshFileEnable)) bool RefreshFileEnable;

            virtual bool GetRefreshFileEnable() = 0;

            virtual void SetRefreshFileEnable(bool value) = 0;

            /// @~chinese 标定板类型   @~english Calibration Board Type
            __declspec(property(put = SetCalibBoardType, get = GetCalibBoardType)) CalibBoardTypeEnum CalibBoardType;

            virtual CalibBoardTypeEnum GetCalibBoardType() = 0;

            virtual void SetCalibBoardType(CalibBoardTypeEnum value) = 0;

            /// @~chinese 自由度   @~english DOF
            __declspec(property(put = SetHomoFreedom, get = GetHomoFreedom)) HomoFreedomEnum HomoFreedom;

            virtual HomoFreedomEnum GetHomoFreedom() = 0;

            virtual void SetHomoFreedom(HomoFreedomEnum value) = 0;

            /// @~chinese 权重函数   @~english Weighting Function
            __declspec(property(put = SetWeightFun, get = GetWeightFun)) WeightFunEnum WeightFun;

            virtual WeightFunEnum GetWeightFun() = 0;

            virtual void SetWeightFun(WeightFunEnum value) = 0;

            /// @~chinese 权重系数，范围：[1,200]   @~english Weighting Coefficient，range:[1,200]
            __declspec(property(put = SetWeightFactor, get = GetWeightFactor)) int WeightFactor;

            virtual int GetWeightFactor() = 0;

            virtual void SetWeightFactor(int value) = 0;

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

            /// @~chinese 灰度对比度，范围：[1,255](弃用)   @~english Grayscale Contrast，range:[1,255](Obsolete)
            __declspec(property(put = SetGrayContrast2, get = GetGrayContrast2)) int GrayContrast2;

            virtual int GetGrayContrast2() = 0;

            virtual void SetGrayContrast2(int value) = 0;

            /// @~chinese 中值滤波状态   @~english Median Filtering State
            __declspec(property(put = SetFilterStatus2, get = GetFilterStatus2)) FilterStatus2Enum FilterStatus2;

            virtual FilterStatus2Enum GetFilterStatus2() = 0;

            virtual void SetFilterStatus2(FilterStatus2Enum value) = 0;

            /// @~chinese 亚像素窗口(弃用)   @~english Sub-pixel window(Obsolete)
            __declspec(property(put = SetSubpixelWinAuto2, get = GetSubpixelWinAuto2)) SubpixelWinAuto2Enum SubpixelWinAuto2;

            virtual SubpixelWinAuto2Enum GetSubpixelWinAuto2() = 0;

            virtual void SetSubpixelWinAuto2(SubpixelWinAuto2Enum value) = 0;

            /// @~chinese 设置窗口大小，范围：[3,150](弃用)   @~english Set window size，range:[3,150](Obsolete)
            __declspec(property(put = SetSubPixelWindowSize2, get = GetSubPixelWindowSize2)) int SubPixelWindowSize2;

            virtual int GetSubPixelWindowSize2() = 0;

            virtual void SetSubPixelWindowSize2(int value) = 0;

            /// @~chinese 生成标定文件   @~english Create Calibration File
            virtual int DoSaveFile(IN const char * const strValue) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
		/// 标定文件角点结果信息
        class ICalibPointsInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit ICalibPointsInfo() {}
            virtual ~ICalibPointsInfo() {}

        public:
            ///< Property
            /**
             * @brief 总角点数量
             */
            virtual int GetPointNum() = 0;

            /**
             * @brief 角点图像坐标集合
             */
            virtual MVD_POINT_F* GetImageCoordinates() = 0;
        };
		/// 使用自研标定板图像标定时的标定信息
        class ITwoImageMapCalibExtraInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit ITwoImageMapCalibExtraInfo() {}
            virtual ~ITwoImageMapCalibExtraInfo() {}

        public:
            ///< Property
            /**
             * @brief 图像的标定状态
             */
            virtual int GetSourceImageCalibStatus() = 0;

            /**
             * @brief 图像的标定状态
             */
            virtual int GetTargetImageCalibStatus() = 0;

            /**
             * @brief 图像的单像素精度
             */
            virtual float GetSourceImageScale() = 0;

            /**
             * @brief 图像的单像素精度
             */
            virtual float GetTargetImageScale() = 0;

            /**
             * @brief 映射误差
             */
            virtual float GetMapError() = 0;

            /**
             * @brief 当前图像的角点集
             */
            virtual ICalibPointsInfo* GetSourceImagePoints() = 0;

            /**
             * @brief 目标图像的角点集
             */
            virtual ICalibPointsInfo* GetTargetImagePoints() = 0;

            /**
             * @brief 当前图像角点映射到目标图像内的角点集
             */
            virtual ICalibPointsInfo* GetProjectionPoints() = 0;
        };

        /*  Note: Interface Classes of the result of the MapCalibResults.  */
        ///映射标定结果类
        class MapCalibResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MapCalibResults() {}
            virtual ~MapCalibResults() {}

        public:
            /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 标定状态
             */
             virtual int GetCalibStatus() = 0;
            /**
             * @brief 检测区域
             */
             virtual RectBox GetROI(int nIndex) = 0;
            ///< Property
            /**
             * @brief x方向比例
             */
            virtual float GetXRatio() = 0;

            /**
             * @brief y方向比例
             */
            virtual float GetYRatio() = 0;

            /**
             * @brief 两张图像映射标定模式下的额外输出信息
             */
            virtual ITwoImageMapCalibExtraInfo* GetTwoImageModeExtraInfo() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the MapCalib tool.  */
        ///映射标定工具类
        class IMVSMapCalibModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSMapCalibModuTool() {}
            virtual ~IMVSMapCalibModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual MapCalibParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual MapCalibResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSMapCalibModuTool(IMVSMapCalibModuTool&);
            IMVSMapCalibModuTool& operator=(IMVSMapCalibModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for MapCalib.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSMapCalibModu::IMVSMapCalibModuTool * __stdcall GetMapCalibToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_MAPCALIB_H_
