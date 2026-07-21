/***************************************************************************************************
 * File：IVmL2LMeasure.h
 * Note：@~chinese 线线测量模块接口声明 @~english Interface for the L2LMeasure tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_L2LMEASURE_H_
#define _IVM_L2LMEASURE_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSL2LMeasureModu
    {
        /// @~chinese 坐标系选择   @~english Choose Coordinate
        typedef enum _ChooseCoordinateEnum
        {
            /// @~chinese 图像坐标系   @~english Image Coordinate
            ChooseCoordinate_ImageCor = 0x1,

            /// @~chinese 特定坐标系   @~english Special Coordinate
            ChooseCoordinate_SpecCor = 0x2,

        }ChooseCoordinateEnum;

        /// @~chinese 边缘类型1   @~english Edge Type1
        typedef enum _FindMode1Enum
        {
            /// @~chinese 最强   @~english Strongest Edge
            FindMode1_Best = 0x1,

            /// @~chinese 第一条   @~english First Edge
            FindMode1_First = 0x2,

            /// @~chinese 最后一条   @~english Last Edge
            FindMode1_Last = 0x3,

            /// @~chinese 接近中线   @~english Close To Mid
            FindMode1_Mid = 0x4,

        }FindMode1Enum;

        /// @~chinese 边缘极性1   @~english Edge Polarity1
        typedef enum _EdgePolarity1Enum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            EdgePolarity1_BlackToWhite = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            EdgePolarity1_WhiteToBlack = 0x2,

            /// @~chinese 任意极性   @~english Both
            EdgePolarity1_Both = 0x3,

        }EdgePolarity1Enum;

        /// @~chinese 初始拟合1   @~english Initial Fit1
        typedef enum _InitType1Enum
        {
            /// @~chinese 全局   @~english Global
            InitType1_ALS = 0x1,

            /// @~chinese 局部   @~english Local
            InitType1_LLS = 0x2,

        }InitType1Enum;

        /// @~chinese 拟合方式1   @~english Fit Mode1
        typedef enum _FitFun1Enum
        {
            /// @~chinese 最小二乘   @~english Least Squares
            FitFun1_LS = 0x1,

            /// @~chinese huber   @~english huber
            FitFun1_Huber = 0x2,

            /// @~chinese tukey   @~english tukey
            FitFun1_Tukey = 0x3,

        }FitFun1Enum;

        /// @~chinese 边缘类型2   @~english Edge Type2
        typedef enum _FindMode2Enum
        {
            /// @~chinese 最强   @~english Strongest Edge
            FindMode2_Best = 0x1,

            /// @~chinese 第一条   @~english First Edge
            FindMode2_First = 0x2,

            /// @~chinese 最后一条   @~english Last Edge
            FindMode2_Last = 0x3,

            /// @~chinese 接近中线   @~english Close To Mid
            FindMode2_Mid = 0x4,

        }FindMode2Enum;

        /// @~chinese 边缘极性2   @~english Edge Polarity2
        typedef enum _EdgePolarity2Enum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            EdgePolarity2_BlackToWhite = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            EdgePolarity2_WhiteToBlack = 0x2,

            /// @~chinese 任意极性   @~english Both
            EdgePolarity2_Both = 0x3,

        }EdgePolarity2Enum;

        /// @~chinese 初始拟合2   @~english Initial Fit2
        typedef enum _InitType2Enum
        {
            /// @~chinese 全局   @~english Global
            InitType2_ALS = 0x1,

            /// @~chinese 局部   @~english Local
            InitType2_LLS = 0x2,

        }InitType2Enum;

        /// @~chinese 拟合方式2   @~english Fit Mode2
        typedef enum _FitFun2Enum
        {
            /// @~chinese 最小二乘   @~english Least Squares
            FitFun2_LS = 0x1,

            /// @~chinese huber   @~english huber
            FitFun2_Huber = 0x2,

            /// @~chinese tukey   @~english tukey
            FitFun2_Tukey = 0x3,

        }FitFun2Enum;

        /// @~chinese 输出角度范围   @~english Output Angle Range
        typedef enum _OutputAngleRangeEnum
        {
            /// @~chinese -90°-90°   @~english -90°-90°
            OutputAngleRange_RangeSegment = 0x0,

            /// @~chinese -180°-180°   @~english 180°-180°
            OutputAngleRange_RangeLinear = 0x1,

        }OutputAngleRangeEnum;

    /// \addtogroup 线线测量
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the L2LMeasureParams.  */
        ///线线测量参数类
        class L2LMeasureParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit L2LMeasureParams() {}
            virtual ~L2LMeasureParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入直线1(仅当次执行起效)   @~english Input Line 1(Only valid for this execution)
            virtual void SetInputLine1(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入直线2(仅当次执行起效)   @~english Input Line 2(Only valid for this execution)
            virtual void SetInputLine2(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 标定矩阵(仅当次执行起效)   @~english Calibration Matrix(Only valid for this execution)
            virtual void SetCalibMatrix(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 坐标系选择   @~english Choose Coordinate
            __declspec(property(put = SetChooseCoordinate, get = GetChooseCoordinate)) ChooseCoordinateEnum ChooseCoordinate;

            virtual ChooseCoordinateEnum GetChooseCoordinate() = 0;

            virtual void SetChooseCoordinate(ChooseCoordinateEnum value) = 0;

            /// @~chinese 边缘类型1   @~english Edge Type1
            __declspec(property(put = SetFindMode1, get = GetFindMode1)) FindMode1Enum FindMode1;

            virtual FindMode1Enum GetFindMode1() = 0;

            virtual void SetFindMode1(FindMode1Enum value) = 0;

            /// @~chinese 边缘极性1   @~english Edge Polarity1
            __declspec(property(put = SetEdgePolarity1, get = GetEdgePolarity1)) EdgePolarity1Enum EdgePolarity1;

            virtual EdgePolarity1Enum GetEdgePolarity1() = 0;

            virtual void SetEdgePolarity1(EdgePolarity1Enum value) = 0;

            /// @~chinese 边缘阈值1，范围：[1,255]   @~english Contrast Threshold1，range:[1,255]
            __declspec(property(put = SetEdgeThreshold1, get = GetEdgeThreshold1)) int EdgeThreshold1;

            virtual int GetEdgeThreshold1() = 0;

            virtual void SetEdgeThreshold1(int value) = 0;

            /// @~chinese 滤波尺寸1，范围：[1,50]   @~english Filter Size1，range:[1,50]
            __declspec(property(put = SetKernelSize1, get = GetKernelSize1)) int KernelSize1;

            virtual int GetKernelSize1() = 0;

            virtual void SetKernelSize1(int value) = 0;

            /// @~chinese 剔除点数1，范围：[0,998]   @~english Reject Number1，range:[0,998]
            __declspec(property(put = SetRejectNum1, get = GetRejectNum1)) int RejectNum1;

            virtual int GetRejectNum1() = 0;

            virtual void SetRejectNum1(int value) = 0;

            /// @~chinese 剔除距离1，范围：[1,1000]   @~english Distance to remove1，range:[1,1000]
            __declspec(property(put = SetRejectDist1, get = GetRejectDist1)) int RejectDist1;

            virtual int GetRejectDist1() = 0;

            virtual void SetRejectDist1(int value) = 0;

            /// @~chinese 初始拟合1   @~english Initial Fit1
            __declspec(property(put = SetInitType1, get = GetInitType1)) InitType1Enum InitType1;

            virtual InitType1Enum GetInitType1() = 0;

            virtual void SetInitType1(InitType1Enum value) = 0;

            /// @~chinese 拟合方式1   @~english Fit Mode1
            __declspec(property(put = SetFitFun1, get = GetFitFun1)) FitFun1Enum FitFun1;

            virtual FitFun1Enum GetFitFun1() = 0;

            virtual void SetFitFun1(FitFun1Enum value) = 0;

            /// @~chinese 边缘类型2   @~english Edge Type2
            __declspec(property(put = SetFindMode2, get = GetFindMode2)) FindMode2Enum FindMode2;

            virtual FindMode2Enum GetFindMode2() = 0;

            virtual void SetFindMode2(FindMode2Enum value) = 0;

            /// @~chinese 边缘极性2   @~english Edge Polarity2
            __declspec(property(put = SetEdgePolarity2, get = GetEdgePolarity2)) EdgePolarity2Enum EdgePolarity2;

            virtual EdgePolarity2Enum GetEdgePolarity2() = 0;

            virtual void SetEdgePolarity2(EdgePolarity2Enum value) = 0;

            /// @~chinese 边缘阈值2，范围：[1,255]   @~english Contrast Threshold2，range:[1,255]
            __declspec(property(put = SetEdgeThreshold2, get = GetEdgeThreshold2)) int EdgeThreshold2;

            virtual int GetEdgeThreshold2() = 0;

            virtual void SetEdgeThreshold2(int value) = 0;

            /// @~chinese 滤波尺寸2，范围：[1,50]   @~english Filter Size2，range:[1,50]
            __declspec(property(put = SetKernelSize2, get = GetKernelSize2)) int KernelSize2;

            virtual int GetKernelSize2() = 0;

            virtual void SetKernelSize2(int value) = 0;

            /// @~chinese 剔除点数2，范围：[0,998]   @~english Reject Number2，range:[0,998]
            __declspec(property(put = SetRejectNum2, get = GetRejectNum2)) int RejectNum2;

            virtual int GetRejectNum2() = 0;

            virtual void SetRejectNum2(int value) = 0;

            /// @~chinese 剔除距离2，范围：[1,1000]   @~english Distance to remove2，range:[1,1000]
            __declspec(property(put = SetRejectDist2, get = GetRejectDist2)) int RejectDist2;

            virtual int GetRejectDist2() = 0;

            virtual void SetRejectDist2(int value) = 0;

            /// @~chinese 初始拟合2   @~english Initial Fit2
            __declspec(property(put = SetInitType2, get = GetInitType2)) InitType2Enum InitType2;

            virtual InitType2Enum GetInitType2() = 0;

            virtual void SetInitType2(InitType2Enum value) = 0;

            /// @~chinese 拟合方式2   @~english Fit Mode2
            __declspec(property(put = SetFitFun2, get = GetFitFun2)) FitFun2Enum FitFun2;

            virtual FitFun2Enum GetFitFun2() = 0;

            virtual void SetFitFun2(FitFun2Enum value) = 0;

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

            /// @~chinese 交点X判断   @~english Intersection X Check
            __declspec(property(put = SetInterXLimitEnable, get = GetInterXLimitEnable)) bool InterXLimitEnable;

            virtual bool GetInterXLimitEnable() = 0;

            virtual void SetInterXLimitEnable(bool value) = 0;

            /// @~chinese 交点X范围，范围：[-99999,99999]   @~english Intersection X Range，range:[-99999,99999]
            __declspec(property(put = SetInterXLimitLow, get = GetInterXLimitLow)) double InterXLimitLow;

            virtual double GetInterXLimitLow() = 0;

            virtual void SetInterXLimitLow(double value) = 0;

            /// @~chinese 交点X范围，范围：[-99999,99999]   @~english Intersection X Range，range:[-99999,99999]
            __declspec(property(put = SetInterXLimitHigh, get = GetInterXLimitHigh)) double InterXLimitHigh;

            virtual double GetInterXLimitHigh() = 0;

            virtual void SetInterXLimitHigh(double value) = 0;

            /// @~chinese 交点Y判断   @~english Intersection Y Check
            __declspec(property(put = SetInterYLimitEnable, get = GetInterYLimitEnable)) bool InterYLimitEnable;

            virtual bool GetInterYLimitEnable() = 0;

            virtual void SetInterYLimitEnable(bool value) = 0;

            /// @~chinese 交点Y范围，范围：[-99999,99999]   @~english Intersection Y Range，range:[-99999,99999]
            __declspec(property(put = SetInterYLimitLow, get = GetInterYLimitLow)) double InterYLimitLow;

            virtual double GetInterYLimitLow() = 0;

            virtual void SetInterYLimitLow(double value) = 0;

            /// @~chinese 交点Y范围，范围：[-99999,99999]   @~english Intersection Y Range，range:[-99999,99999]
            __declspec(property(put = SetInterYLimitHigh, get = GetInterYLimitHigh)) double InterYLimitHigh;

            virtual double GetInterYLimitHigh() = 0;

            virtual void SetInterYLimitHigh(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the L2LMeasureResults.  */
        ///线线测量结果类
        class L2LMeasureResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit L2LMeasureResults() {}
            virtual ~L2LMeasureResults() {}

        public:
			 /**
             * @brief 卡尺检测区1
             * @param nIndex [IN] 范围(0,5)
             */
             virtual RectBox GetDetectCaliperBox1(int nIndex) = 0;
             /**
             * @brief 卡尺检测区2
             * @param nIndex [IN] 范围(0,5)
             */
             virtual RectBox GetDetectCaliperBox2(int nIndex) = 0;
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 交点
             */
            virtual MVD_POINT_F GetInterPoint() = 0;
            /**
             * @brief 夹角
             */
            virtual float GetAngle() = 0;
            /**
             * @brief 绝对距离
             */
            virtual float GetVerticalAbsDist() = 0;
            /**
             * @brief 测量直线
             * @param nIndex [IN] 范围(0,1)
             */
            virtual Line GetMeasureLine(int nIndex) = 0;
 			/**
             * @brief 测量直线1角度
             */
             virtual float GetInputLine1Angle() = 0;
            /**
             * @brief 测量直线2角度
             */
             virtual float GetInputLine2Angle() = 0;
            /**
             * @brief 角度标识点
             * @param nIndex [IN] 范围(0,4)
             */
            virtual PointF GetShowAnglePoint(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the L2LMeasure tool.  */
        ///线线测量工具类
        class IMVSL2LMeasureModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSL2LMeasureModuTool() {}
            virtual ~IMVSL2LMeasureModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual L2LMeasureParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual L2LMeasureResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSL2LMeasureModuTool(IMVSL2LMeasureModuTool&);
            IMVSL2LMeasureModuTool& operator=(IMVSL2LMeasureModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for L2LMeasure.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSL2LMeasureModu::IMVSL2LMeasureModuTool * __stdcall GetL2LMeasureToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_L2LMEASURE_H_
