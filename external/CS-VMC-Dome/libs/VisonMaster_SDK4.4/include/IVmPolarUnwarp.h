/***************************************************************************************************
 * File：IVmPolarUnwarp.h
 * Note：@~chinese 圆环展开模块接口声明 @~english Interface for the PolarUnwarp tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_POLARUNWARP_H_
#define _IVM_POLARUNWARP_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSPolarUnwarpModu
    {
        /// @~chinese 角度方向   @~english Angle Direction
        typedef enum _AngleDirTypeEnum
        {
            /// @~chinese 逆时针   @~english Anticlockwise
            AngleDirType_AntiClockWise = 0x1,

            /// @~chinese 顺时针   @~english Clockwise
            AngleDirType_ClockWise = 0x2,

        }AngleDirTypeEnum;

        /// @~chinese 半径方向   @~english Radius Orientation
        typedef enum _RadiusDirTypeEnum
        {
            /// @~chinese 从内往外   @~english Outward
            RadiusDirType_InnerToOuter = 0x1,

            /// @~chinese 从外往内   @~english Inward
            RadiusDirType_OuterToInner = 0x2,

        }RadiusDirTypeEnum;

        /// @~chinese 插值方法   @~english Interpolation Mode
        typedef enum _InterpolationEnum
        {
            /// @~chinese 最近邻   @~english NN
            Interpolation_Neareast = 0x1,

            /// @~chinese 双线性   @~english Bilinear
            Interpolation_Bilinear = 0x2,

        }InterpolationEnum;

    /// \addtogroup 圆环展开
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///圆环展开ROI管理类
        class PolarUnwarpRoiManager
        {
        public:

            /// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
            virtual void SetRoiAnnulus(Annulus stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the PolarUnwarpParams.  */
        ///圆环展开参数类
        class PolarUnwarpParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit PolarUnwarpParams() {}
            virtual ~PolarUnwarpParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual PolarUnwarpRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 角度方向   @~english Angle Direction
            __declspec(property(put = SetAngleDirType, get = GetAngleDirType)) AngleDirTypeEnum AngleDirType;

            virtual AngleDirTypeEnum GetAngleDirType() = 0;

            virtual void SetAngleDirType(AngleDirTypeEnum value) = 0;

            /// @~chinese 半径方向   @~english Radius Orientation
            __declspec(property(put = SetRadiusDirType, get = GetRadiusDirType)) RadiusDirTypeEnum RadiusDirType;

            virtual RadiusDirTypeEnum GetRadiusDirType() = 0;

            virtual void SetRadiusDirType(RadiusDirTypeEnum value) = 0;

            /// @~chinese 插值方法   @~english Interpolation Mode
            __declspec(property(put = SetInterpolation, get = GetInterpolation)) InterpolationEnum Interpolation;

            virtual InterpolationEnum GetInterpolation() = 0;

            virtual void SetInterpolation(InterpolationEnum value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the PolarUnwarpResults.  */
        ///圆环展开结果类
        class PolarUnwarpResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit PolarUnwarpResults() {}
            virtual ~PolarUnwarpResults() {}

        public:
             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
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
        /*  Note: Note: This is the class for the PolarUnwarp tool.  */
        ///圆环展开工具类
        class IMVSPolarUnwarpModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSPolarUnwarpModuTool() {}
            virtual ~IMVSPolarUnwarpModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual PolarUnwarpParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual PolarUnwarpResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSPolarUnwarpModuTool(IMVSPolarUnwarpModuTool&);
            IMVSPolarUnwarpModuTool& operator=(IMVSPolarUnwarpModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for PolarUnwarp.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSPolarUnwarpModu::IMVSPolarUnwarpModuTool * __stdcall GetPolarUnwarpToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_POLARUNWARP_H_
