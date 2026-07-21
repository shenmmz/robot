/***************************************************************************************************
 * File：IVmGeometricTransform.h
 * Note：@~chinese 几何变换模块接口声明 @~english Interface for the GeometricTransform tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_GEOMETRICTRANSFORM_H_
#define _IVM_GEOMETRICTRANSFORM_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSGeometricTransformModu
    {
        /// @~chinese 镜像方向   @~english Mirror Orientation
        typedef enum _MirrorOrientationEnum
        {
            /// @~chinese 无   @~english NoMirror
            MirrorOrientation_NoMirror = 0x0,

            /// @~chinese 水平   @~english Horizontal
            MirrorOrientation_Horizontal = 0x1,

            /// @~chinese 垂直   @~english Vertical
            MirrorOrientation_Vertical = 0x2,

            /// @~chinese 水平垂直   @~english HoriAndVert
            MirrorOrientation_HoriAndVert = 0x3,

        }MirrorOrientationEnum;

        /// @~chinese 旋转角度   @~english Rotation Angle
        typedef enum _RotateAngleEnum
        {
            /// @~chinese 0   @~english 0
            RotateAngle_ZeroQuarter = 0x0,

            /// @~chinese 90   @~english 90
            RotateAngle_OneQuarter = 0x1,

            /// @~chinese 180   @~english 180
            RotateAngle_TwoQuarter = 0x2,

            /// @~chinese 270   @~english 270
            RotateAngle_ThreeQuarter = 0x3,

        }RotateAngleEnum;

    /// \addtogroup 几何变换
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the GeometricTransformParams.  */
        ///几何变换参数类
        class GeometricTransformParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit GeometricTransformParams() {}
            virtual ~GeometricTransformParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 镜像方向   @~english Mirror Orientation
            __declspec(property(put = SetMirrorOrientation, get = GetMirrorOrientation)) MirrorOrientationEnum MirrorOrientation;

            virtual MirrorOrientationEnum GetMirrorOrientation() = 0;

            virtual void SetMirrorOrientation(MirrorOrientationEnum value) = 0;

            /// @~chinese 旋转角度   @~english Rotation Angle
            __declspec(property(put = SetRotateAngle, get = GetRotateAngle)) RotateAngleEnum RotateAngle;

            virtual RotateAngleEnum GetRotateAngle() = 0;

            virtual void SetRotateAngle(RotateAngleEnum value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the GeometricTransformResults.  */
        ///几何变换结果类
        class GeometricTransformResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit GeometricTransformResults() {}
            virtual ~GeometricTransformResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 获取输出图像
             */
            virtual ImageBaseData GetOutputImage() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the GeometricTransform tool.  */
        ///几何变换工具类
        class IMVSGeometricTransformModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSGeometricTransformModuTool() {}
            virtual ~IMVSGeometricTransformModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual GeometricTransformParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual GeometricTransformResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSGeometricTransformModuTool(IMVSGeometricTransformModuTool&);
            IMVSGeometricTransformModuTool& operator=(IMVSGeometricTransformModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for GeometricTransform.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSGeometricTransformModu::IMVSGeometricTransformModuTool * __stdcall GetGeometricTransformToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_GEOMETRICTRANSFORM_H_
