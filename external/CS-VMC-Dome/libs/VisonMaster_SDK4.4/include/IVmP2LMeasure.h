/***************************************************************************************************
 * File：IVmP2LMeasure.h
 * Note：@~chinese 点线测量模块接口声明 @~english Interface for the P2LMeasure tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_P2LMEASURE_H_
#define _IVM_P2LMEASURE_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSP2LMeasureModu
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

    /// \addtogroup 点线测量
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the P2LMeasureParams.  */
        ///点线测量参数类
        class P2LMeasureParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit P2LMeasureParams() {}
            virtual ~P2LMeasureParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入点(仅当次执行起效)   @~english Input Point(Only valid for this execution)
            virtual void SetInputPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入直线(仅当次执行起效)   @~english Input Line(Only valid for this execution)
            virtual void SetInputLine(Line* pInputValue, unsigned int nValueNum) = 0;

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

            /// @~chinese 垂直距离判断   @~english Vertical Distance Check
            __declspec(property(put = SetDistPerpendLimitEnable, get = GetDistPerpendLimitEnable)) bool DistPerpendLimitEnable;

            virtual bool GetDistPerpendLimitEnable() = 0;

            virtual void SetDistPerpendLimitEnable(bool value) = 0;

            /// @~chinese 垂直距离范围，范围：[0,99999]   @~english Vertical Distance Range，range:[0,99999]
            __declspec(property(put = SetDistPerpendLimitLow, get = GetDistPerpendLimitLow)) double DistPerpendLimitLow;

            virtual double GetDistPerpendLimitLow() = 0;

            virtual void SetDistPerpendLimitLow(double value) = 0;

            /// @~chinese 垂直距离范围，范围：[0,99999]   @~english Vertical Distance Range，range:[0,99999]
            __declspec(property(put = SetDistPerpendLimitHigh, get = GetDistPerpendLimitHigh)) double DistPerpendLimitHigh;

            virtual double GetDistPerpendLimitHigh() = 0;

            virtual void SetDistPerpendLimitHigh(double value) = 0;

            /// @~chinese 垂点X判断   @~english Foot Point X Check
            __declspec(property(put = SetProjXLimitEnable, get = GetProjXLimitEnable)) bool ProjXLimitEnable;

            virtual bool GetProjXLimitEnable() = 0;

            virtual void SetProjXLimitEnable(bool value) = 0;

            /// @~chinese 垂点X范围，范围：[-99999,99999]   @~english Foot Point X Range，range:[-99999,99999]
            __declspec(property(put = SetProjXLimitLow, get = GetProjXLimitLow)) double ProjXLimitLow;

            virtual double GetProjXLimitLow() = 0;

            virtual void SetProjXLimitLow(double value) = 0;

            /// @~chinese 垂点X范围，范围：[-99999,99999]   @~english Foot Point X Range，range:[-99999,99999]
            __declspec(property(put = SetProjXLimitHigh, get = GetProjXLimitHigh)) double ProjXLimitHigh;

            virtual double GetProjXLimitHigh() = 0;

            virtual void SetProjXLimitHigh(double value) = 0;

            /// @~chinese 垂点Y判断   @~english Foot Point Y Check
            __declspec(property(put = SetProjYLimitEnable, get = GetProjYLimitEnable)) bool ProjYLimitEnable;

            virtual bool GetProjYLimitEnable() = 0;

            virtual void SetProjYLimitEnable(bool value) = 0;

            /// @~chinese 垂点Y范围，范围：[-99999,99999]   @~english Foot Point Y Range，range:[-99999,99999]
            __declspec(property(put = SetProjYLimitLow, get = GetProjYLimitLow)) double ProjYLimitLow;

            virtual double GetProjYLimitLow() = 0;

            virtual void SetProjYLimitLow(double value) = 0;

            /// @~chinese 垂点Y范围，范围：[-99999,99999]   @~english Foot Point Y Range，range:[-99999,99999]
            __declspec(property(put = SetProjYLimitHigh, get = GetProjYLimitHigh)) double ProjYLimitHigh;

            virtual double GetProjYLimitHigh() = 0;

            virtual void SetProjYLimitHigh(double value) = 0;

            /// @~chinese 最近距离判断   @~english Closest Distance Check
            __declspec(property(put = SetDistMinLimitEnable, get = GetDistMinLimitEnable)) bool DistMinLimitEnable;

            virtual bool GetDistMinLimitEnable() = 0;

            virtual void SetDistMinLimitEnable(bool value) = 0;

            /// @~chinese 最近距离范围，范围：[0,99999]   @~english Closest Distance Range，range:[0,99999]
            __declspec(property(put = SetDistMinLimitLow, get = GetDistMinLimitLow)) double DistMinLimitLow;

            virtual double GetDistMinLimitLow() = 0;

            virtual void SetDistMinLimitLow(double value) = 0;

            /// @~chinese 最近距离范围，范围：[0,99999]   @~english Closest Distance Range，range:[0,99999]
            __declspec(property(put = SetDistMinLimitHigh, get = GetDistMinLimitHigh)) double DistMinLimitHigh;

            virtual double GetDistMinLimitHigh() = 0;

            virtual void SetDistMinLimitHigh(double value) = 0;

            /// @~chinese 最远距离判断   @~english Furthest Distance Check
            __declspec(property(put = SetDistMaxLimitEnable, get = GetDistMaxLimitEnable)) bool DistMaxLimitEnable;

            virtual bool GetDistMaxLimitEnable() = 0;

            virtual void SetDistMaxLimitEnable(bool value) = 0;

            /// @~chinese 最远距离范围，范围：[0,99999]   @~english Furthest Distance Range，range:[0,99999]
            __declspec(property(put = SetDistMaxLimitLow, get = GetDistMaxLimitLow)) double DistMaxLimitLow;

            virtual double GetDistMaxLimitLow() = 0;

            virtual void SetDistMaxLimitLow(double value) = 0;

            /// @~chinese 最远距离范围，范围：[0,99999]   @~english Furthest Distance Range，range:[0,99999]
            __declspec(property(put = SetDistMaxLimitHigh, get = GetDistMaxLimitHigh)) double DistMaxLimitHigh;

            virtual double GetDistMaxLimitHigh() = 0;

            virtual void SetDistMaxLimitHigh(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the P2LMeasureResults.  */
        ///点线测量结果类
        class P2LMeasureResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit P2LMeasureResults() {}
            virtual ~P2LMeasureResults() {}

        public:
			/**
             * @brief 模块状态
             */
            virtual int GetModuStatus() = 0;
            /**
             * @brief 测量直线角度
             */
            virtual float GetInputLineAngle() = 0;
            /**
             * @brief 垂足点
             */
            virtual MVD_POINT_F GetProjectPoint() = 0;
            /**
             * @brief 角度
             */
            virtual float GetAngle() = 0;
            /**
             * @brief 垂足距离
             */
            virtual float GetVerticalDist() = 0;
            /**
             * @brief 最近距离
             */
            virtual float GetCloestDist() = 0;
            /**
             * @brief 最远距离
             */
            virtual float GetFarthestDist() = 0;
            /**
             * @brief 测量点
             */
            virtual PointF GetMeasurePoint() = 0;
            /**
             * @brief 测量直线
             */
            virtual Line GetMeasureLine() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the P2LMeasure tool.  */
        ///点线测量工具类
        class IMVSP2LMeasureModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSP2LMeasureModuTool() {}
            virtual ~IMVSP2LMeasureModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual P2LMeasureParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual P2LMeasureResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSP2LMeasureModuTool(IMVSP2LMeasureModuTool&);
            IMVSP2LMeasureModuTool& operator=(IMVSP2LMeasureModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for P2LMeasure.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSP2LMeasureModu::IMVSP2LMeasureModuTool * __stdcall GetP2LMeasureToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_P2LMEASURE_H_
