/***************************************************************************************************
 * File：IVmQuadrangleFind.h
 * Note：@~chinese 四边形查找模块接口声明 @~english Interface for the QuadrangleFind tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_QUADRANGLEFIND_H_
#define _IVM_QUADRANGLEFIND_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSQuadrangleFindModu
    {
        /// @~chinese 边缘类型1   @~english Edge Type1
        typedef enum _FindMode0Enum
        {
            /// @~chinese 最强   @~english Strongest Edge
            FindMode0_Best = 0x1,

            /// @~chinese 第一条   @~english First Edge
            FindMode0_First = 0x2,

            /// @~chinese 最后一条   @~english Last Edge
            FindMode0_Last = 0x3,

        }FindMode0Enum;

        /// @~chinese 边缘极性1   @~english Edge Polarity1
        typedef enum _EdgePolarity0Enum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            EdgePolarity0_BlackToWhite = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            EdgePolarity0_WhiteToBlack = 0x2,

            /// @~chinese 任意极性   @~english Both
            EdgePolarity0_Both = 0x3,

        }EdgePolarity0Enum;

        /// @~chinese 初始拟合1   @~english Initial Fit1
        typedef enum _InitType0Enum
        {
            /// @~chinese 全局   @~english Global
            InitType0_ALS = 0x1,

            /// @~chinese 局部   @~english Local
            InitType0_LLS = 0x2,

        }InitType0Enum;

        /// @~chinese 拟合方式1   @~english Fit Mode1
        typedef enum _FitFun0Enum
        {
            /// @~chinese 最小二乘   @~english Least Squares
            FitFun0_LS = 0x1,

            /// @~chinese huber   @~english huber
            FitFun0_Huber = 0x2,

            /// @~chinese tukey   @~english tukey
            FitFun0_Tukey = 0x3,

        }FitFun0Enum;

        /// @~chinese 边缘类型2   @~english Edge Type2
        typedef enum _FindMode1Enum
        {
            /// @~chinese 最强   @~english Strongest Edge
            FindMode1_Best = 0x1,

            /// @~chinese 第一条   @~english First Edge
            FindMode1_First = 0x2,

            /// @~chinese 最后一条   @~english Last Edge
            FindMode1_Last = 0x3,

        }FindMode1Enum;

        /// @~chinese 边缘极性2   @~english Edge Polarity2
        typedef enum _EdgePolarity1Enum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            EdgePolarity1_BlackToWhite = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            EdgePolarity1_WhiteToBlack = 0x2,

            /// @~chinese 任意极性   @~english Both
            EdgePolarity1_Both = 0x3,

        }EdgePolarity1Enum;

        /// @~chinese 初始拟合2   @~english Initial Fit2
        typedef enum _InitType1Enum
        {
            /// @~chinese 全局   @~english Global
            InitType1_ALS = 0x1,

            /// @~chinese 局部   @~english Local
            InitType1_LLS = 0x2,

        }InitType1Enum;

        /// @~chinese 拟合方式2   @~english Fit Mode2
        typedef enum _FitFun1Enum
        {
            /// @~chinese 最小二乘   @~english Least Squares
            FitFun1_LS = 0x1,

            /// @~chinese huber   @~english huber
            FitFun1_Huber = 0x2,

            /// @~chinese tukey   @~english tukey
            FitFun1_Tukey = 0x3,

        }FitFun1Enum;

        /// @~chinese 边缘类型3   @~english Edge Type3
        typedef enum _FindMode2Enum
        {
            /// @~chinese 最强   @~english Strongest Edge
            FindMode2_Best = 0x1,

            /// @~chinese 第一条   @~english First Edge
            FindMode2_First = 0x2,

            /// @~chinese 最后一条   @~english Last Edge
            FindMode2_Last = 0x3,

        }FindMode2Enum;

        /// @~chinese 边缘极性3   @~english Edge Polarity3
        typedef enum _EdgePolarity2Enum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            EdgePolarity2_BlackToWhite = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            EdgePolarity2_WhiteToBlack = 0x2,

            /// @~chinese 任意极性   @~english Both
            EdgePolarity2_Both = 0x3,

        }EdgePolarity2Enum;

        /// @~chinese 初始拟合3   @~english Initial Fit3
        typedef enum _InitType2Enum
        {
            /// @~chinese 全局   @~english Global
            InitType2_ALS = 0x1,

            /// @~chinese 局部   @~english Local
            InitType2_LLS = 0x2,

        }InitType2Enum;

        /// @~chinese 拟合方式3   @~english Fit Mode3
        typedef enum _FitFun2Enum
        {
            /// @~chinese 最小二乘   @~english Least Squares
            FitFun2_LS = 0x1,

            /// @~chinese huber   @~english huber
            FitFun2_Huber = 0x2,

            /// @~chinese tukey   @~english tukey
            FitFun2_Tukey = 0x3,

        }FitFun2Enum;

        /// @~chinese 边缘类型4   @~english Edge Type4
        typedef enum _FindMode3Enum
        {
            /// @~chinese 最强   @~english Strongest Edge
            FindMode3_Best = 0x1,

            /// @~chinese 第一条   @~english First Edge
            FindMode3_First = 0x2,

            /// @~chinese 最后一条   @~english Last Edge
            FindMode3_Last = 0x3,

        }FindMode3Enum;

        /// @~chinese 边缘极性4   @~english Edge Polarity4
        typedef enum _EdgePolarity3Enum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            EdgePolarity3_BlackToWhite = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            EdgePolarity3_WhiteToBlack = 0x2,

            /// @~chinese 任意极性   @~english Both
            EdgePolarity3_Both = 0x3,

        }EdgePolarity3Enum;

        /// @~chinese 初始拟合4   @~english Initial Fit4
        typedef enum _InitType3Enum
        {
            /// @~chinese 全局   @~english Global
            InitType3_ALS = 0x1,

            /// @~chinese 局部   @~english Local
            InitType3_LLS = 0x2,

        }InitType3Enum;

        /// @~chinese 拟合方式4   @~english Fit Mode4
        typedef enum _FitFun3Enum
        {
            /// @~chinese 最小二乘   @~english Least Squares
            FitFun3_LS = 0x1,

            /// @~chinese huber   @~english huber
            FitFun3_Huber = 0x2,

            /// @~chinese tukey   @~english tukey
            FitFun3_Tukey = 0x3,

        }FitFun3Enum;

    /// \addtogroup 四边形查找
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///四边形查找ROI管理类
        class QuadrangleFindRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox* pRoiValue, unsigned int nValueNum) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the QuadrangleFindParams.  */
        ///四边形查找参数类
        class QuadrangleFindParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit QuadrangleFindParams() {}
            virtual ~QuadrangleFindParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual QuadrangleFindRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 边缘类型1   @~english Edge Type1
            __declspec(property(put = SetFindMode0, get = GetFindMode0)) FindMode0Enum FindMode0;

            virtual FindMode0Enum GetFindMode0() = 0;

            virtual void SetFindMode0(FindMode0Enum value) = 0;

            /// @~chinese 边缘极性1   @~english Edge Polarity1
            __declspec(property(put = SetEdgePolarity0, get = GetEdgePolarity0)) EdgePolarity0Enum EdgePolarity0;

            virtual EdgePolarity0Enum GetEdgePolarity0() = 0;

            virtual void SetEdgePolarity0(EdgePolarity0Enum value) = 0;

            /// @~chinese 边缘阈值1，范围：[1,255]   @~english Contrast Threshold1，range:[1,255]
            __declspec(property(put = SetEdgeThreshold0, get = GetEdgeThreshold0)) int EdgeThreshold0;

            virtual int GetEdgeThreshold0() = 0;

            virtual void SetEdgeThreshold0(int value) = 0;

            /// @~chinese 滤波尺寸1，范围：[1,50]   @~english Filter Size1，range:[1,50]
            __declspec(property(put = SetKernelSize0, get = GetKernelSize0)) int KernelSize0;

            virtual int GetKernelSize0() = 0;

            virtual void SetKernelSize0(int value) = 0;

            /// @~chinese 卡尺数量1，范围：[2,1000]   @~english Caliper Number1，range:[2,1000]
            __declspec(property(put = SetCaliperNum0, get = GetCaliperNum0)) int CaliperNum0;

            virtual int GetCaliperNum0() = 0;

            virtual void SetCaliperNum0(int value) = 0;

            /// @~chinese 剔除点数1，范围：[0,998]   @~english Reject Number1，range:[0,998]
            __declspec(property(put = SetRejectNum0, get = GetRejectNum0)) int RejectNum0;

            virtual int GetRejectNum0() = 0;

            virtual void SetRejectNum0(int value) = 0;

            /// @~chinese 剔除距离1，范围：[1,1000]   @~english Distance to remove1，range:[1,1000]
            __declspec(property(put = SetRejectDist0, get = GetRejectDist0)) int RejectDist0;

            virtual int GetRejectDist0() = 0;

            virtual void SetRejectDist0(int value) = 0;

            /// @~chinese 投影宽度1，范围：[1,100]   @~english Projection Width1，range:[1,100]
            __declspec(property(put = SetRegionWidth0, get = GetRegionWidth0)) int RegionWidth0;

            virtual int GetRegionWidth0() = 0;

            virtual void SetRegionWidth0(int value) = 0;

            /// @~chinese 初始拟合1   @~english Initial Fit1
            __declspec(property(put = SetInitType0, get = GetInitType0)) InitType0Enum InitType0;

            virtual InitType0Enum GetInitType0() = 0;

            virtual void SetInitType0(InitType0Enum value) = 0;

            /// @~chinese 拟合方式1   @~english Fit Mode1
            __declspec(property(put = SetFitFun0, get = GetFitFun0)) FitFun0Enum FitFun0;

            virtual FitFun0Enum GetFitFun0() = 0;

            virtual void SetFitFun0(FitFun0Enum value) = 0;

            /// @~chinese 边缘类型2   @~english Edge Type2
            __declspec(property(put = SetFindMode1, get = GetFindMode1)) FindMode1Enum FindMode1;

            virtual FindMode1Enum GetFindMode1() = 0;

            virtual void SetFindMode1(FindMode1Enum value) = 0;

            /// @~chinese 边缘极性2   @~english Edge Polarity2
            __declspec(property(put = SetEdgePolarity1, get = GetEdgePolarity1)) EdgePolarity1Enum EdgePolarity1;

            virtual EdgePolarity1Enum GetEdgePolarity1() = 0;

            virtual void SetEdgePolarity1(EdgePolarity1Enum value) = 0;

            /// @~chinese 边缘阈值2，范围：[1,255]   @~english Contrast Threshold2，range:[1,255]
            __declspec(property(put = SetEdgeThreshold1, get = GetEdgeThreshold1)) int EdgeThreshold1;

            virtual int GetEdgeThreshold1() = 0;

            virtual void SetEdgeThreshold1(int value) = 0;

            /// @~chinese 滤波尺寸2，范围：[1,50]   @~english Filter Size2，range:[1,50]
            __declspec(property(put = SetKernelSize1, get = GetKernelSize1)) int KernelSize1;

            virtual int GetKernelSize1() = 0;

            virtual void SetKernelSize1(int value) = 0;

            /// @~chinese 卡尺数量2，范围：[2,1000]   @~english Caliper Number2，range:[2,1000]
            __declspec(property(put = SetCaliperNum1, get = GetCaliperNum1)) int CaliperNum1;

            virtual int GetCaliperNum1() = 0;

            virtual void SetCaliperNum1(int value) = 0;

            /// @~chinese 剔除点数2，范围：[0,998]   @~english Reject Number2，range:[0,998]
            __declspec(property(put = SetRejectNum1, get = GetRejectNum1)) int RejectNum1;

            virtual int GetRejectNum1() = 0;

            virtual void SetRejectNum1(int value) = 0;

            /// @~chinese 剔除距离2，范围：[1,1000]   @~english Distance to remove2，range:[1,1000]
            __declspec(property(put = SetRejectDist1, get = GetRejectDist1)) int RejectDist1;

            virtual int GetRejectDist1() = 0;

            virtual void SetRejectDist1(int value) = 0;

            /// @~chinese 投影宽度2，范围：[1,100]   @~english Projection Width2，range:[1,100]
            __declspec(property(put = SetRegionWidth1, get = GetRegionWidth1)) int RegionWidth1;

            virtual int GetRegionWidth1() = 0;

            virtual void SetRegionWidth1(int value) = 0;

            /// @~chinese 初始拟合2   @~english Initial Fit2
            __declspec(property(put = SetInitType1, get = GetInitType1)) InitType1Enum InitType1;

            virtual InitType1Enum GetInitType1() = 0;

            virtual void SetInitType1(InitType1Enum value) = 0;

            /// @~chinese 拟合方式2   @~english Fit Mode2
            __declspec(property(put = SetFitFun1, get = GetFitFun1)) FitFun1Enum FitFun1;

            virtual FitFun1Enum GetFitFun1() = 0;

            virtual void SetFitFun1(FitFun1Enum value) = 0;

            /// @~chinese 边缘类型3   @~english Edge Type3
            __declspec(property(put = SetFindMode2, get = GetFindMode2)) FindMode2Enum FindMode2;

            virtual FindMode2Enum GetFindMode2() = 0;

            virtual void SetFindMode2(FindMode2Enum value) = 0;

            /// @~chinese 边缘极性3   @~english Edge Polarity3
            __declspec(property(put = SetEdgePolarity2, get = GetEdgePolarity2)) EdgePolarity2Enum EdgePolarity2;

            virtual EdgePolarity2Enum GetEdgePolarity2() = 0;

            virtual void SetEdgePolarity2(EdgePolarity2Enum value) = 0;

            /// @~chinese 边缘阈值3，范围：[1,255]   @~english Contrast Threshold3，range:[1,255]
            __declspec(property(put = SetEdgeThreshold2, get = GetEdgeThreshold2)) int EdgeThreshold2;

            virtual int GetEdgeThreshold2() = 0;

            virtual void SetEdgeThreshold2(int value) = 0;

            /// @~chinese 滤波尺寸3，范围：[1,50]   @~english Filter Kernel Half-Width3，range:[1,50]
            __declspec(property(put = SetKernelSize2, get = GetKernelSize2)) int KernelSize2;

            virtual int GetKernelSize2() = 0;

            virtual void SetKernelSize2(int value) = 0;

            /// @~chinese 卡尺数量3，范围：[2,1000]   @~english Caliper Number3，range:[2,1000]
            __declspec(property(put = SetCaliperNum2, get = GetCaliperNum2)) int CaliperNum2;

            virtual int GetCaliperNum2() = 0;

            virtual void SetCaliperNum2(int value) = 0;

            /// @~chinese 剔除点数3，范围：[0,998]   @~english Reject Number3，range:[0,998]
            __declspec(property(put = SetRejectNum2, get = GetRejectNum2)) int RejectNum2;

            virtual int GetRejectNum2() = 0;

            virtual void SetRejectNum2(int value) = 0;

            /// @~chinese 剔除距离3，范围：[1,1000]   @~english Distance to remove3，range:[1,1000]
            __declspec(property(put = SetRejectDist2, get = GetRejectDist2)) int RejectDist2;

            virtual int GetRejectDist2() = 0;

            virtual void SetRejectDist2(int value) = 0;

            /// @~chinese 投影宽度3，范围：[1,100]   @~english Projection Width3，range:[1,100]
            __declspec(property(put = SetRegionWidth2, get = GetRegionWidth2)) int RegionWidth2;

            virtual int GetRegionWidth2() = 0;

            virtual void SetRegionWidth2(int value) = 0;

            /// @~chinese 初始拟合3   @~english Initial Fit3
            __declspec(property(put = SetInitType2, get = GetInitType2)) InitType2Enum InitType2;

            virtual InitType2Enum GetInitType2() = 0;

            virtual void SetInitType2(InitType2Enum value) = 0;

            /// @~chinese 拟合方式3   @~english Fit Mode3
            __declspec(property(put = SetFitFun2, get = GetFitFun2)) FitFun2Enum FitFun2;

            virtual FitFun2Enum GetFitFun2() = 0;

            virtual void SetFitFun2(FitFun2Enum value) = 0;

            /// @~chinese 边缘类型4   @~english Edge Type4
            __declspec(property(put = SetFindMode3, get = GetFindMode3)) FindMode3Enum FindMode3;

            virtual FindMode3Enum GetFindMode3() = 0;

            virtual void SetFindMode3(FindMode3Enum value) = 0;

            /// @~chinese 边缘极性4   @~english Edge Polarity4
            __declspec(property(put = SetEdgePolarity3, get = GetEdgePolarity3)) EdgePolarity3Enum EdgePolarity3;

            virtual EdgePolarity3Enum GetEdgePolarity3() = 0;

            virtual void SetEdgePolarity3(EdgePolarity3Enum value) = 0;

            /// @~chinese 边缘阈值4，范围：[1,255]   @~english Contrast Threshold4，range:[1,255]
            __declspec(property(put = SetEdgeThreshold3, get = GetEdgeThreshold3)) int EdgeThreshold3;

            virtual int GetEdgeThreshold3() = 0;

            virtual void SetEdgeThreshold3(int value) = 0;

            /// @~chinese 滤波尺寸4，范围：[1,50]   @~english Filter Kernel Half-Width4，range:[1,50]
            __declspec(property(put = SetKernelSize3, get = GetKernelSize3)) int KernelSize3;

            virtual int GetKernelSize3() = 0;

            virtual void SetKernelSize3(int value) = 0;

            /// @~chinese 卡尺数量4，范围：[2,1000]   @~english Caliper Number4，range:[2,1000]
            __declspec(property(put = SetCaliperNum3, get = GetCaliperNum3)) int CaliperNum3;

            virtual int GetCaliperNum3() = 0;

            virtual void SetCaliperNum3(int value) = 0;

            /// @~chinese 剔除点数4，范围：[0,998]   @~english Reject Number4，range:[0,998]
            __declspec(property(put = SetRejectNum3, get = GetRejectNum3)) int RejectNum3;

            virtual int GetRejectNum3() = 0;

            virtual void SetRejectNum3(int value) = 0;

            /// @~chinese 剔除距离4，范围：[1,1000]   @~english Distance to remove4，range:[1,1000]
            __declspec(property(put = SetRejectDist3, get = GetRejectDist3)) int RejectDist3;

            virtual int GetRejectDist3() = 0;

            virtual void SetRejectDist3(int value) = 0;

            /// @~chinese 投影宽度4，范围：[1,100]   @~english Projection Width4，range:[1,100]
            __declspec(property(put = SetRegionWidth3, get = GetRegionWidth3)) int RegionWidth3;

            virtual int GetRegionWidth3() = 0;

            virtual void SetRegionWidth3(int value) = 0;

            /// @~chinese 初始拟合4   @~english Initial Fit4
            __declspec(property(put = SetInitType3, get = GetInitType3)) InitType3Enum InitType3;

            virtual InitType3Enum GetInitType3() = 0;

            virtual void SetInitType3(InitType3Enum value) = 0;

            /// @~chinese 拟合方式4   @~english Fit Mode4
            __declspec(property(put = SetFitFun3, get = GetFitFun3)) FitFun3Enum FitFun3;

            virtual FitFun3Enum GetFitFun3() = 0;

            virtual void SetFitFun3(FitFun3Enum value) = 0;

            /// @~chinese 中心点X判断   @~english Central Point X Check
            __declspec(property(put = SetCentralPointXLimitEnable, get = GetCentralPointXLimitEnable)) bool CentralPointXLimitEnable;

            virtual bool GetCentralPointXLimitEnable() = 0;

            virtual void SetCentralPointXLimitEnable(bool value) = 0;

            /// @~chinese 中心点X范围，范围：[-99999,99999]   @~english Central Point X Range，range:[-99999,99999]
            __declspec(property(put = SetCentralPointXLimitLow, get = GetCentralPointXLimitLow)) double CentralPointXLimitLow;

            virtual double GetCentralPointXLimitLow() = 0;

            virtual void SetCentralPointXLimitLow(double value) = 0;

            /// @~chinese 中心点X范围，范围：[-99999,99999]   @~english Central Point X Range，range:[-99999,99999]
            __declspec(property(put = SetCentralPointXLimitHigh, get = GetCentralPointXLimitHigh)) double CentralPointXLimitHigh;

            virtual double GetCentralPointXLimitHigh() = 0;

            virtual void SetCentralPointXLimitHigh(double value) = 0;

            /// @~chinese 中心点Y判断   @~english Central Point Y Check
            __declspec(property(put = SetCentralPointYLimitEnable, get = GetCentralPointYLimitEnable)) bool CentralPointYLimitEnable;

            virtual bool GetCentralPointYLimitEnable() = 0;

            virtual void SetCentralPointYLimitEnable(bool value) = 0;

            /// @~chinese 中心点Y范围，范围：[-99999,99999]   @~english Central Point Y Range，range:[-99999,99999]
            __declspec(property(put = SetCentralPointYLimitLow, get = GetCentralPointYLimitLow)) double CentralPointYLimitLow;

            virtual double GetCentralPointYLimitLow() = 0;

            virtual void SetCentralPointYLimitLow(double value) = 0;

            /// @~chinese 中心点Y范围，范围：[-99999,99999]   @~english Central Point Y Range，range:[-99999,99999]
            __declspec(property(put = SetCentralPointYLimitHigh, get = GetCentralPointYLimitHigh)) double CentralPointYLimitHigh;

            virtual double GetCentralPointYLimitHigh() = 0;

            virtual void SetCentralPointYLimitHigh(double value) = 0;

            /// @~chinese 直线1角度判断   @~english Line 1 Angle Check
            __declspec(property(put = SetAngleLimitFirstEnable, get = GetAngleLimitFirstEnable)) bool AngleLimitFirstEnable;

            virtual bool GetAngleLimitFirstEnable() = 0;

            virtual void SetAngleLimitFirstEnable(bool value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngleLimitFirstLow, get = GetAngleLimitFirstLow)) double AngleLimitFirstLow;

            virtual double GetAngleLimitFirstLow() = 0;

            virtual void SetAngleLimitFirstLow(double value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngleLimitFirstHigh, get = GetAngleLimitFirstHigh)) double AngleLimitFirstHigh;

            virtual double GetAngleLimitFirstHigh() = 0;

            virtual void SetAngleLimitFirstHigh(double value) = 0;

            /// @~chinese 直线2角度判断   @~english Line 2 Angle Check
            __declspec(property(put = SetAngleLimitSecondEnable, get = GetAngleLimitSecondEnable)) bool AngleLimitSecondEnable;

            virtual bool GetAngleLimitSecondEnable() = 0;

            virtual void SetAngleLimitSecondEnable(bool value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngleLimitSecondLow, get = GetAngleLimitSecondLow)) double AngleLimitSecondLow;

            virtual double GetAngleLimitSecondLow() = 0;

            virtual void SetAngleLimitSecondLow(double value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngleLimitSecondHigh, get = GetAngleLimitSecondHigh)) double AngleLimitSecondHigh;

            virtual double GetAngleLimitSecondHigh() = 0;

            virtual void SetAngleLimitSecondHigh(double value) = 0;

            /// @~chinese 直线3角度判断   @~english Line 3 Angle Check
            __declspec(property(put = SetAngleLimitThirdEnable, get = GetAngleLimitThirdEnable)) bool AngleLimitThirdEnable;

            virtual bool GetAngleLimitThirdEnable() = 0;

            virtual void SetAngleLimitThirdEnable(bool value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngleLimitThirdLow, get = GetAngleLimitThirdLow)) double AngleLimitThirdLow;

            virtual double GetAngleLimitThirdLow() = 0;

            virtual void SetAngleLimitThirdLow(double value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngleLimitThirdHigh, get = GetAngleLimitThirdHigh)) double AngleLimitThirdHigh;

            virtual double GetAngleLimitThirdHigh() = 0;

            virtual void SetAngleLimitThirdHigh(double value) = 0;

            /// @~chinese 直线4角度判断   @~english Line 4 Angle Check
            __declspec(property(put = SetAngleLimitFourthEnable, get = GetAngleLimitFourthEnable)) bool AngleLimitFourthEnable;

            virtual bool GetAngleLimitFourthEnable() = 0;

            virtual void SetAngleLimitFourthEnable(bool value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngleLimitFourthLow, get = GetAngleLimitFourthLow)) double AngleLimitFourthLow;

            virtual double GetAngleLimitFourthLow() = 0;

            virtual void SetAngleLimitFourthLow(double value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngleLimitFourthHigh, get = GetAngleLimitFourthHigh)) double AngleLimitFourthHigh;

            virtual double GetAngleLimitFourthHigh() = 0;

            virtual void SetAngleLimitFourthHigh(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /** @enum  MVD_EDGEPOINT_STATUS
        *  @brief 边缘点状态
        */
		/// @~chinese 边缘点状态 @~english Find status of the edge point
        typedef _MVD_EDGEPOINT_STATUS__ MVD_EDGEPOINT_STATUS;

        /** @enum  MVD_EDGE_POLARITY
        *  @brief 边缘点极性
        */
		/// @~chinese 边缘点极性 @~english Type of the polarity of the edge point
        typedef _MVD_EDGE_POLARITY__ MVD_EDGE_POLARITY;

        ///边缘点信息描述类
        class IQuadrangleFindEdgePointInfo
        {
        protected:
            ///< 构造与析构
            explicit IQuadrangleFindEdgePointInfo() {}
            virtual ~IQuadrangleFindEdgePointInfo() {}

        public:
            /**
             * @brief 边缘点坐标
             */
            virtual MVD_POINT_F GetEdgePoint() = 0;
            /**
             * @brief 边缘点状态
             */
            virtual MVD_EDGEPOINT_STATUS GetStatus() = 0;
        };

        ///四边形查找边缘线信息描述类
        class IQuadrangleFindLineInfo
        {
        protected:
            ///< 构造与析构
            explicit IQuadrangleFindLineInfo() {}
            virtual ~IQuadrangleFindLineInfo() {}

        public:
            /**
             * @brief 获取边缘点数量
            */
            virtual int GetEdgePointNum() = 0;
            /**
            * @brief 获取边缘点信息
            * @param nPtIndex [IN] 点的索引值，从0开始计数
            */
            virtual IQuadrangleFindEdgePointInfo* GetEdgePointInfo(int nPtIndex) = 0;
            /**
             * @brief 获取直线
            */
            virtual Line GetLine() = 0;
            /**
             * @brief 获取直线拟合误差
            */
            virtual float GetFitError() = 0;
        };

        /*  Note: Interface Classes of the result of the QuadrangleFindResults.  */
        ///四边形查找结果类
        class QuadrangleFindResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit QuadrangleFindResults() {}
            virtual ~QuadrangleFindResults() {}

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
             * @brief 直线1状态
             */
             virtual int GetLineStatusFirst() = 0;
             /**
             * @brief 直线2状态
             */
             virtual int GetLineStatusSecond() = 0;
             /**
             * @brief 直线3状态
             */
             virtual int GetLineStatusThird() = 0;
             /**
             * @brief 直线4状态
             */
             virtual int GetLineStatusFourth() = 0;
             /**
             * @brief 检测区域1
             */
             virtual RectBox GetROI0() = 0;
             /**
             * @brief 检测区域2
             */
             virtual RectBox GetROI1() = 0;
             /**
             * @brief 检测区域3
             */
             virtual RectBox GetROI2() = 0;
             /**
             * @brief 检测区域4
             */
             virtual RectBox GetROI3() = 0;
            /**
             * @brief 获取四边形四条边所在直线的直线段信息
             * @param nIndex [IN] 线的索引值,取值范围[0,3],依次为top/right/bottom/left边
             */
            virtual IQuadrangleFindLineInfo* GetLineInfo(int nIndex) = 0;
            /**
             * @brief 获取四边形四个顶点
             * @param stVertex [IN] 四个顶点信息，依次为top&right/right&bottom/bottom&left/left&top边交点
             */
            virtual void GetVertex(MVD_POINT_F stVertex[4]) = 0;
            /**
             * @brief 获取四边形四个相邻边的夹角,依次为top&right/right&bottom/bottom&left/left&top边夹角
             */
            virtual void GetAngle(float fAngle[4]) = 0;
            /**
             * @brief 获取对角线
             * @param nIndex [IN] 对角线的索引值, 取值范围[0,1],依次为left&top/right&bottom顶点连线、top&right/bottom&left顶点连线
             */
            virtual Line GetDiagonalLine(int nIndex) = 0;
            /**
             * @brief 获取中点线
             * @param nIndex [IN] 中点线的索引值, 取值范围[0,1],依次为top/bottom边中点连线、right/left边中点连线
             */
            virtual Line GetMedianLine(int nIndex) = 0;
            /**
             * @brief 获取四边形对边角平分线,即四边形对边延长线的角平分线
             * @param nIndex [IN] 角平分线的索引值, 取值范围[0,1],依次为top/bottom对边角平分线、right/left对边角平分线
             */
            virtual Line GetAngleBisectorLine(int nIndex) = 0;
            /**
             * @brief 获取中点(重心点)
             */
            virtual MVD_POINT_F GetCenter() = 0;
            /**
             * @brief 获取对角线间的交点
             */
            virtual MVD_POINT_F GetDiagonalLineIntersection() = 0;
            /**
             * @brief 获取中点线间的交点（即四个顶点坐标均值）
             */
            virtual MVD_POINT_F GetMedianLineIntersection() = 0;
            /**
             * @brief 获取对边角平分线交点
             */
            virtual MVD_POINT_F GetAngleBisectorIntersection() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the QuadrangleFind tool.  */
        ///四边形查找工具类
        class IMVSQuadrangleFindModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSQuadrangleFindModuTool() {}
            virtual ~IMVSQuadrangleFindModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual QuadrangleFindParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual QuadrangleFindResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSQuadrangleFindModuTool(IMVSQuadrangleFindModuTool&);
            IMVSQuadrangleFindModuTool& operator=(IMVSQuadrangleFindModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for QuadrangleFind.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSQuadrangleFindModu::IMVSQuadrangleFindModuTool * __stdcall GetQuadrangleFindToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_QUADRANGLEFIND_H_
