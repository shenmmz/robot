/***************************************************************************************************
 * File：IVmP2PMeasure.h
 * Note：@~chinese 点点测量模块接口声明 @~english Interface for the P2PMeasure tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_P2PMEASURE_H_
#define _IVM_P2PMEASURE_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSP2PMeasureModu
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

    /// \addtogroup 点点测量
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the P2PMeasureParams.  */
        ///点点测量参数类
        class P2PMeasureParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit P2PMeasureParams() {}
            virtual ~P2PMeasureParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入点1(仅当次执行起效)   @~english Input Point1(Only valid for this execution)
            virtual void SetInputPoint1(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入点2(仅当次执行起效)   @~english Input Point2(Only valid for this execution)
            virtual void SetInputPoint2(PointF* pInputValue, unsigned int nValueNum) = 0;

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

            /// @~chinese 距离判断   @~english Distance Check
            __declspec(property(put = SetDistLimitEnable, get = GetDistLimitEnable)) bool DistLimitEnable;

            virtual bool GetDistLimitEnable() = 0;

            virtual void SetDistLimitEnable(bool value) = 0;

            /// @~chinese 距离范围，范围：[0,99999]   @~english Distance Range，range:[0,99999]
            __declspec(property(put = SetDistLimitLow, get = GetDistLimitLow)) double DistLimitLow;

            virtual double GetDistLimitLow() = 0;

            virtual void SetDistLimitLow(double value) = 0;

            /// @~chinese 距离范围，范围：[0,99999]   @~english Distance Range，range:[0,99999]
            __declspec(property(put = SetDistLimitHigh, get = GetDistLimitHigh)) double DistLimitHigh;

            virtual double GetDistLimitHigh() = 0;

            virtual void SetDistLimitHigh(double value) = 0;

            /// @~chinese 中点X判断   @~english Midpoint X Check
            __declspec(property(put = SetMidXLimitEnable, get = GetMidXLimitEnable)) bool MidXLimitEnable;

            virtual bool GetMidXLimitEnable() = 0;

            virtual void SetMidXLimitEnable(bool value) = 0;

            /// @~chinese 中点X范围，范围：[-99999,99999]   @~english Midpoint X Range，range:[-99999,99999]
            __declspec(property(put = SetMidXLimitLow, get = GetMidXLimitLow)) double MidXLimitLow;

            virtual double GetMidXLimitLow() = 0;

            virtual void SetMidXLimitLow(double value) = 0;

            /// @~chinese 中点X范围，范围：[-99999,99999]   @~english Midpoint X Range，range:[-99999,99999]
            __declspec(property(put = SetMidXLimitHigh, get = GetMidXLimitHigh)) double MidXLimitHigh;

            virtual double GetMidXLimitHigh() = 0;

            virtual void SetMidXLimitHigh(double value) = 0;

            /// @~chinese 中点Y判断   @~english Midpoint Y Check
            __declspec(property(put = SetMidYLimitEnable, get = GetMidYLimitEnable)) bool MidYLimitEnable;

            virtual bool GetMidYLimitEnable() = 0;

            virtual void SetMidYLimitEnable(bool value) = 0;

            /// @~chinese 中点Y范围，范围：[-99999,99999]   @~english Midpoint Y Range，range:[-99999,99999]
            __declspec(property(put = SetMidYLimitLow, get = GetMidYLimitLow)) double MidYLimitLow;

            virtual double GetMidYLimitLow() = 0;

            virtual void SetMidYLimitLow(double value) = 0;

            /// @~chinese 中点Y范围，范围：[-99999,99999]   @~english Midpoint Y Range，range:[-99999,99999]
            __declspec(property(put = SetMidYLimitHigh, get = GetMidYLimitHigh)) double MidYLimitHigh;

            virtual double GetMidYLimitHigh() = 0;

            virtual void SetMidYLimitHigh(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the P2PMeasureResults.  */
        ///点点测量结果类
        class P2PMeasureResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit P2PMeasureResults() {}
            virtual ~P2PMeasureResults() {}

        public:
			 /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 点点测量的中点
             */
            virtual MVD_POINT_F GetMidPoint() = 0;
            /**
             * @brief 点点连线与水平X轴之间的角度
             */
            virtual float GetAngle() = 0;
            /**
             * @brief 点点之间的距离
             */
            virtual float GetDist() = 0;
            /**
             * @brief 测量点
             * @param nIndex [IN] 范围(0,1)
             */
            virtual PointF GetMeasurePoint(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the P2PMeasure tool.  */
        ///点点测量工具类
        class IMVSP2PMeasureModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSP2PMeasureModuTool() {}
            virtual ~IMVSP2PMeasureModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual P2PMeasureParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual P2PMeasureResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSP2PMeasureModuTool(IMVSP2PMeasureModuTool&);
            IMVSP2PMeasureModuTool& operator=(IMVSP2PMeasureModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for P2PMeasure.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSP2PMeasureModu::IMVSP2PMeasureModuTool * __stdcall GetP2PMeasureToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_P2PMEASURE_H_
