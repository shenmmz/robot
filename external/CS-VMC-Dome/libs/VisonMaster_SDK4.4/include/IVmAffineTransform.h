/***************************************************************************************************
 * File：IVmAffineTransform.h
 * Note：@~chinese 仿射变换模块接口声明 @~english Interface for the AffineTransform tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_AFFINETRANSFORM_H_
#define _IVM_AFFINETRANSFORM_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSAffineTransformModu
    {
        /// @~chinese 仿射变换类型   @~english Transformation Type
        typedef enum _TransformationTypeEnum
        {
            /// @~chinese 裁剪缩放   @~english Crop Zoom
            TransformationType_CropZoom = 0x1,

            /// @~chinese 镜像   @~english Mirrored
            TransformationType_Mirror = 0x2,

            /// @~chinese 平移   @~english Translation
            TransformationType_Translation = 0x3,

        }TransformationTypeEnum;

        /// @~chinese 插值方法   @~english Interpolation Mode
        typedef enum _InterpolationEnum
        {
            /// @~chinese 最近邻   @~english NN
            Interpolation_Neareast = 0x1,

            /// @~chinese 双线性   @~english Bilinear
            Interpolation_Bilinear = 0x2,

        }InterpolationEnum;

        /// @~chinese 填充方式   @~english Fill Mode
        typedef enum _ExtensionTypeEnum
        {
            /// @~chinese 常数   @~english Constant
            ExtensionType_Constant = 0x1,

            /// @~chinese 邻近复制   @~english Copy Nearby
            ExtensionType_Copy = 0x2,

        }ExtensionTypeEnum;

        /// @~chinese 镜像方向   @~english Mirror Orientation
        typedef enum _MirrorOrientationEnum
        {
            /// @~chinese 水平   @~english Horizontal
            MirrorOrientation_Horizontal = 0x1,

            /// @~chinese 垂直   @~english Vertical
            MirrorOrientation_Vertical = 0x2,

            /// @~chinese 水平垂直   @~english HoriAndVert
            MirrorOrientation_HoriAndVert = 0x3,

        }MirrorOrientationEnum;

    /// \addtogroup 仿射变换
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///仿射变换ROI管理类
        class AffineTransformRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the AffineTransformParams.  */
        ///仿射变换参数类
        class AffineTransformParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit AffineTransformParams() {}
            virtual ~AffineTransformParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual AffineTransformRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 仿射变换类型   @~english Transformation Type
            __declspec(property(put = SetTransformationType, get = GetTransformationType)) TransformationTypeEnum TransformationType;

            virtual TransformationTypeEnum GetTransformationType() = 0;

            virtual void SetTransformationType(TransformationTypeEnum value) = 0;

            /// @~chinese 尺度，范围：[0.1,10.0]   @~english Scale，range:[0.1,10.0]
            __declspec(property(put = SetScale, get = GetScale)) double Scale;

            virtual double GetScale() = 0;

            virtual void SetScale(double value) = 0;

            /// @~chinese 宽高比，范围：[0.1,2.0]   @~english Aspect，range:[0.1,2.0]
            __declspec(property(put = SetAspect, get = GetAspect)) double Aspect;

            virtual double GetAspect() = 0;

            virtual void SetAspect(double value) = 0;

            /// @~chinese 插值方法   @~english Interpolation Mode
            __declspec(property(put = SetInterpolation, get = GetInterpolation)) InterpolationEnum Interpolation;

            virtual InterpolationEnum GetInterpolation() = 0;

            virtual void SetInterpolation(InterpolationEnum value) = 0;

            /// @~chinese 填充方式   @~english Fill Mode
            __declspec(property(put = SetExtensionType, get = GetExtensionType)) ExtensionTypeEnum ExtensionType;

            virtual ExtensionTypeEnum GetExtensionType() = 0;

            virtual void SetExtensionType(ExtensionTypeEnum value) = 0;

            /// @~chinese 填充值，范围：[0,255]   @~english Fill Value，range:[0,255]
            __declspec(property(put = SetExtensionValue, get = GetExtensionValue)) int ExtensionValue;

            virtual int GetExtensionValue() = 0;

            virtual void SetExtensionValue(int value) = 0;

            /// @~chinese 镜像方向   @~english Mirror Orientation
            __declspec(property(put = SetMirrorOrientation, get = GetMirrorOrientation)) MirrorOrientationEnum MirrorOrientation;

            virtual MirrorOrientationEnum GetMirrorOrientation() = 0;

            virtual void SetMirrorOrientation(MirrorOrientationEnum value) = 0;

            /// @~chinese 旋转角度，范围：[0.0,360.0]   @~english Rotation Angle，range:[0.0,360.0]
            __declspec(property(put = SetRotateAngle, get = GetRotateAngle)) double RotateAngle;

            virtual double GetRotateAngle() = 0;

            virtual void SetRotateAngle(double value) = 0;

            /// @~chinese X移动距离，范围：[-30000,30000]   @~english Moving distance in X，range:[-30000,30000]
            __declspec(property(put = SetMoveXValue, get = GetMoveXValue)) int MoveXValue;

            virtual int GetMoveXValue() = 0;

            virtual void SetMoveXValue(int value) = 0;

            /// @~chinese Y移动距离，范围：[-20000,20000]   @~english Moving distance in Y，range:[-20000,20000]
            __declspec(property(put = SetMoveYValue, get = GetMoveYValue)) int MoveYValue;

            virtual int GetMoveYValue() = 0;

            virtual void SetMoveYValue(int value) = 0;

            /// @~chinese 锁定输出尺寸   @~english Lock Output Size
            __declspec(property(put = SetLockOutputImageSize, get = GetLockOutputImageSize)) bool LockOutputImageSize;

            virtual bool GetLockOutputImageSize() = 0;

            virtual void SetLockOutputImageSize(bool value) = 0;

            /// @~chinese 宽度，范围：[8,30000]   @~english Width，range:[8,30000]
            __declspec(property(put = SetWidthValue, get = GetWidthValue)) int WidthValue;

            virtual int GetWidthValue() = 0;

            virtual void SetWidthValue(int value) = 0;

            /// @~chinese 高度，范围：[8,20000]   @~english MergeBoxHeight，range:[8,20000]
            __declspec(property(put = SetHeightValue, get = GetHeightValue)) int HeightValue;

            virtual int GetHeightValue() = 0;

            virtual void SetHeightValue(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the AffineTransformResults.  */
        ///仿射变换结果类
        class AffineTransformResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit AffineTransformResults() {}
            virtual ~AffineTransformResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;
            /**
             * @brief 获取仿射变换后的图片
             * @return 无，出错时抛出异常
             */
            virtual ImageBaseData GetOutputImage() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the AffineTransform tool.  */
        ///仿射变换工具类
        class IMVSAffineTransformModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSAffineTransformModuTool() {}
            virtual ~IMVSAffineTransformModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual AffineTransformParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual AffineTransformResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSAffineTransformModuTool(IMVSAffineTransformModuTool&);
            IMVSAffineTransformModuTool& operator=(IMVSAffineTransformModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for AffineTransform.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSAffineTransformModu::IMVSAffineTransformModuTool * __stdcall GetAffineTransformToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_AFFINETRANSFORM_H_
