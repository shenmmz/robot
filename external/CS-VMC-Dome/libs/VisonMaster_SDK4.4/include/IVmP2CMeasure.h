/***************************************************************************************************
 * File：IVmP2CMeasure.h
 * Note：@~chinese 点圆测量模块接口声明 @~english Interface for the P2CMeasure tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_P2CMEASURE_H_
#define _IVM_P2CMEASURE_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSP2CMeasureModu
    {
        /// @~chinese 坐标系选择   @~english Choose Coordinate
        typedef enum _ChooseCoordinateEnum
        {
            /// @~chinese 图像坐标系   @~english Image Coordinate
            ChooseCoordinate_ImageCor = 0x1,

            /// @~chinese 特定坐标系   @~english Special Coordinate
            ChooseCoordinate_SpecCor = 0x2,

        }ChooseCoordinateEnum;

        /// @~chinese 输出角度范围   @~english Output Angle Range
        typedef enum _OutputAngleRangeEnum
        {
            /// @~chinese -90°-90°   @~english -90°-90°
            OutputAngleRange_RangeSegment = 0x0,

            /// @~chinese -180°-180°   @~english 180°-180°
            OutputAngleRange_RangeLinear = 0x1,

        }OutputAngleRangeEnum;

    /// \addtogroup 点圆测量
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the P2CMeasureParams.  */
        ///点圆测量参数类
        class P2CMeasureParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit P2CMeasureParams() {}
            virtual ~P2CMeasureParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入点(仅当次执行起效)   @~english Input Point(Only valid for this execution)
            virtual void SetInputPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入圆(仅当次执行起效)   @~english InputCircleAnnulus(Only valid for this execution)
            virtual void SetInputCircleAnnulus(Annulus* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 标定矩阵(仅当次执行起效)   @~english Calibration Matrix(Only valid for this execution)
            virtual void SetCalibMatrix(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 坐标系选择   @~english Choose Coordinate
            __declspec(property(put = SetChooseCoordinate, get = GetChooseCoordinate)) ChooseCoordinateEnum ChooseCoordinate;

            virtual ChooseCoordinateEnum GetChooseCoordinate() = 0;

            virtual void SetChooseCoordinate(ChooseCoordinateEnum value) = 0;

            /// @~chinese 输出角度范围   @~english Output Angle Range
            __declspec(property(put = SetOutputAngleRange, get = GetOutputAngleRange)) OutputAngleRangeEnum OutputAngleRange;

            virtual OutputAngleRangeEnum GetOutputAngleRange() = 0;

            virtual void SetOutputAngleRange(OutputAngleRangeEnum value) = 0;

            /// @~chinese 角度判断   @~english Angle Check
            __declspec(property(put = SetAngleLimitEnable, get = GetAngleLimitEnable)) bool AngleLimitEnable;

            virtual bool GetAngleLimitEnable() = 0;

            virtual void SetAngleLimitEnable(bool value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngleLimitLow, get = GetAngleLimitLow)) double AngleLimitLow;

            virtual double GetAngleLimitLow() = 0;

            virtual void SetAngleLimitLow(double value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngleLimitHigh, get = GetAngleLimitHigh)) double AngleLimitHigh;

            virtual double GetAngleLimitHigh() = 0;

            virtual void SetAngleLimitHigh(double value) = 0;

            /// @~chinese 中心距离判断   @~english Center Distance Check
            __declspec(property(put = SetDistCenterLimitEnable, get = GetDistCenterLimitEnable)) bool DistCenterLimitEnable;

            virtual bool GetDistCenterLimitEnable() = 0;

            virtual void SetDistCenterLimitEnable(bool value) = 0;

            /// @~chinese 中心距离范围，范围：[0,99999]   @~english Center Distance Range，range:[0,99999]
            __declspec(property(put = SetDistCenterLimitLow, get = GetDistCenterLimitLow)) double DistCenterLimitLow;

            virtual double GetDistCenterLimitLow() = 0;

            virtual void SetDistCenterLimitLow(double value) = 0;

            /// @~chinese 中心距离范围，范围：[0,99999]   @~english Center Distance Range，range:[0,99999]
            __declspec(property(put = SetDistCenterLimitHigh, get = GetDistCenterLimitHigh)) double DistCenterLimitHigh;

            virtual double GetDistCenterLimitHigh() = 0;

            virtual void SetDistCenterLimitHigh(double value) = 0;

            /// @~chinese 最近距离判断   @~english Closest Distance Check
            __declspec(property(put = SetDistClosestLimitEnable, get = GetDistClosestLimitEnable)) bool DistClosestLimitEnable;

            virtual bool GetDistClosestLimitEnable() = 0;

            virtual void SetDistClosestLimitEnable(bool value) = 0;

            /// @~chinese 最近距离范围，范围：[0,99999]   @~english Closest Distance Range，range:[0,99999]
            __declspec(property(put = SetDistClosestLimitLow, get = GetDistClosestLimitLow)) double DistClosestLimitLow;

            virtual double GetDistClosestLimitLow() = 0;

            virtual void SetDistClosestLimitLow(double value) = 0;

            /// @~chinese 最近距离范围，范围：[0,99999]   @~english Closest Distance Range，range:[0,99999]
            __declspec(property(put = SetDistClosestLimitHigh, get = GetDistClosestLimitHigh)) double DistClosestLimitHigh;

            virtual double GetDistClosestLimitHigh() = 0;

            virtual void SetDistClosestLimitHigh(double value) = 0;

            /// @~chinese 最远距离判断   @~english Furthest Distance Check
            __declspec(property(put = SetDistFarthestLimitEnable, get = GetDistFarthestLimitEnable)) bool DistFarthestLimitEnable;

            virtual bool GetDistFarthestLimitEnable() = 0;

            virtual void SetDistFarthestLimitEnable(bool value) = 0;

            /// @~chinese 最远距离范围，范围：[0,99999]   @~english Furthest Distance Range，range:[0,99999]
            __declspec(property(put = SetDistFarthestLimitLow, get = GetDistFarthestLimitLow)) double DistFarthestLimitLow;

            virtual double GetDistFarthestLimitLow() = 0;

            virtual void SetDistFarthestLimitLow(double value) = 0;

            /// @~chinese 最远距离范围，范围：[0,99999]   @~english Furthest Distance Range，range:[0,99999]
            __declspec(property(put = SetDistFarthestLimitHigh, get = GetDistFarthestLimitHigh)) double DistFarthestLimitHigh;

            virtual double GetDistFarthestLimitHigh() = 0;

            virtual void SetDistFarthestLimitHigh(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the P2CMeasureResults.  */
        ///点圆测量结果类
        class P2CMeasureResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit P2CMeasureResults() {}
            virtual ~P2CMeasureResults() {}

        public:
			 /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 点与圆心之间的角度
             */
            virtual float GetAngle() = 0;
            /**
             * @brief 点到圆心的距离
             */
            virtual float GetCenterDist() = 0;
            /**
             * @brief 点到圆上点的最近距离
             */
            virtual float GetCloestDist() = 0;
            /**
             * @brief 点到圆上点的最远距离
             */
            virtual float GetFarthestDist() = 0;
            /**
             * @brief 测量点
             */
            virtual PointF GetMeasurePoint() = 0;
            /**
             * @brief 测量圆
             */
            virtual Circle GetMeasureCircle() = 0;
            /**
            * @brief 测量圆环
            */
            virtual Annulus GetMeasureCircleAnnulus() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the P2CMeasure tool.  */
        ///点圆测量工具类
        class IMVSP2CMeasureModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSP2CMeasureModuTool() {}
            virtual ~IMVSP2CMeasureModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual P2CMeasureParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual P2CMeasureResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSP2CMeasureModuTool(IMVSP2CMeasureModuTool&);
            IMVSP2CMeasureModuTool& operator=(IMVSP2CMeasureModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for P2CMeasure.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSP2CMeasureModu::IMVSP2CMeasureModuTool * __stdcall GetP2CMeasureToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_P2CMEASURE_H_
