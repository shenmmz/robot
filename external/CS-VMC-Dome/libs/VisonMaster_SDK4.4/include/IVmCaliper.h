/***************************************************************************************************
 * File：IVmCaliper.h
 * Note：@~chinese 卡尺工具模块接口声明 @~english Interface for the Caliper tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CALIPER_H_
#define _IVM_CALIPER_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCaliperModu
    {
        /// @~chinese 边缘模式   @~english Edge Mode
        typedef enum _EdgeModeEnum
        {
            /// @~chinese 单边缘   @~english Single Edge
            EdgeMode_SINGLE_EDGE = 0x1,

            /// @~chinese 边缘对   @~english Edge Pair
            EdgeMode_EDGE_PAIR = 0x2,

        }EdgeModeEnum;

        /// @~chinese 边缘0极性   @~english Edge 0 polarity
        typedef enum _EdgePolarityEnum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            EdgePolarity_BlackToWhite = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            EdgePolarity_WhiteToBlack = 0x2,

            /// @~chinese 任意极性   @~english Both
            EdgePolarity_Both = 0x3,

        }EdgePolarityEnum;

        /// @~chinese 边缘1极性   @~english Edge 1 polarity
        typedef enum _Edge1PolarityEnum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            Edge1Polarity_BlackToWhite = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            Edge1Polarity_WhiteToBlack = 0x2,

            /// @~chinese 任意极性   @~english Both
            Edge1Polarity_Both = 0x3,

        }Edge1PolarityEnum;

        /// @~chinese 排序方式   @~english Sort Type
        typedef enum _SortTypeEnum
        {
            /// @~chinese 分数降序   @~english Score Descend
            SortType_ScoreDescend = 0x0,

            /// @~chinese 分数升序   @~english Score Ascend
            SortType_ScoreAscend = 0x1,

            /// @~chinese 方向正向   @~english Orient Forward
            SortType_OrientForward = 0x2,

            /// @~chinese 方向逆向   @~english Orient Backward
            SortType_OrientBackward = 0x3,

        }SortTypeEnum;

        /// @~chinese 曲线类型1   @~english Scoring1 Function
        typedef enum _ContrastDropEnum
        {
            /// @~chinese 递增   @~english Ascending
            ContrastDrop_Rise = 0x0,

            /// @~chinese 递减   @~english Descending
            ContrastDrop_Drop = 0x1,

        }ContrastDropEnum;

        /// @~chinese 曲线类型2   @~english Scoring2 Function
        typedef enum _GrayscaleDropEnum
        {
            /// @~chinese 递增   @~english Ascending
            GrayscaleDrop_Rise = 0x0,

            /// @~chinese 递减   @~english Descending
            GrayscaleDrop_Drop = 0x1,

        }GrayscaleDropEnum;

        /// @~chinese 曲线类型3   @~english Scoring3 Function
        typedef enum _PositionDropEnum
        {
            /// @~chinese 递增   @~english Ascending
            PositionDrop_Rise = 0x0,

            /// @~chinese 递减   @~english Descending
            PositionDrop_Drop = 0x1,

        }PositionDropEnum;

        /// @~chinese 曲线类型4   @~english Scoring4 Function
        typedef enum _PositionNegDropEnum
        {
            /// @~chinese 递增   @~english Ascending
            PositionNegDrop_Rise = 0x0,

            /// @~chinese 递减   @~english Descending
            PositionNegDrop_Drop = 0x1,

        }PositionNegDropEnum;

        /// @~chinese 曲线类型   @~english Scoring Function
        typedef enum _PositionNormDropEnum
        {
            /// @~chinese 递增   @~english Ascending
            PositionNormDrop_Rise = 0x0,

            /// @~chinese 递减   @~english Descending
            PositionNormDrop_Drop = 0x1,

        }PositionNormDropEnum;

        /// @~chinese 曲线类型   @~english Scoring Function
        typedef enum _PositionNormNegDropEnum
        {
            /// @~chinese 递增   @~english Ascending
            PositionNormNegDrop_Rise = 0x0,

            /// @~chinese 递减   @~english Descending
            PositionNormNegDrop_Drop = 0x1,

        }PositionNormNegDropEnum;

        /// @~chinese 曲线类型   @~english Scoring Function
        typedef enum _SizeNormDropEnum
        {
            /// @~chinese 递增   @~english Ascending
            SizeNormDrop_Rise = 0x0,

            /// @~chinese 递减   @~english Descending
            SizeNormDrop_Drop = 0x1,

        }SizeNormDropEnum;

        /// @~chinese 曲线类型   @~english Scoring Function
        typedef enum _SizeDiffNormDropEnum
        {
            /// @~chinese 递增   @~english Ascending
            SizeDiffNormDrop_Rise = 0x0,

            /// @~chinese 递减   @~english Descending
            SizeDiffNormDrop_Drop = 0x1,

        }SizeDiffNormDropEnum;

        /// @~chinese 左曲线类型   @~english Left Scoring Function
        typedef enum _SizeDiffNormAsymDropEnum
        {
            /// @~chinese 递增   @~english Ascending
            SizeDiffNormAsymDrop_Rise = 0x0,

            /// @~chinese 递减   @~english Descending
            SizeDiffNormAsymDrop_Drop = 0x1,

        }SizeDiffNormAsymDropEnum;

        /// @~chinese 右曲线类型   @~english Right Scoring Function
        typedef enum _SizeDiffNormAsymDropHEnum
        {
            /// @~chinese 递增   @~english Ascending
            SizeDiffNormAsymDropH_Rise = 0x0,

            /// @~chinese 递减   @~english Descending
            SizeDiffNormAsymDropH_Drop = 0x1,

        }SizeDiffNormAsymDropHEnum;

        /// @~chinese 投影方向   @~english Projection Direction
        typedef enum _ProjectionTypeEnum
        {
            /// @~chinese 从左到右   @~english Left to Right
            ProjectionType_ProjectionToHeight = 0x0,

            /// @~chinese 从上到下   @~english Top to Bottom
            ProjectionType_ProjectionToWidth = 0x1,

        }ProjectionTypeEnum;

    /// \addtogroup 卡尺工具
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///卡尺工具ROI管理类
        class CaliperRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
            virtual void SetRoiAnnulus(Annulus stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the CaliperParams.  */
        ///卡尺工具参数类
        class CaliperParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CaliperParams() {}
            virtual ~CaliperParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual CaliperRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 边缘模式   @~english Edge Mode
            __declspec(property(put = SetEdgeMode, get = GetEdgeMode)) EdgeModeEnum EdgeMode;

            virtual EdgeModeEnum GetEdgeMode() = 0;

            virtual void SetEdgeMode(EdgeModeEnum value) = 0;

            /// @~chinese 滤波尺寸，范围：[1,50]   @~english Filter Size，range:[1,50]
            __declspec(property(put = SetHalfKernelSize, get = GetHalfKernelSize)) int HalfKernelSize;

            virtual int GetHalfKernelSize() = 0;

            virtual void SetHalfKernelSize(int value) = 0;

            /// @~chinese 边缘阈值，范围：[0,255]   @~english Edge Threshold，range:[0,255]
            __declspec(property(put = SetContrastTH, get = GetContrastTH)) int ContrastTH;

            virtual int GetContrastTH() = 0;

            virtual void SetContrastTH(int value) = 0;

            /// @~chinese 边缘0极性   @~english Edge 0 polarity
            __declspec(property(put = SetEdgePolarity, get = GetEdgePolarity)) EdgePolarityEnum EdgePolarity;

            virtual EdgePolarityEnum GetEdgePolarity() = 0;

            virtual void SetEdgePolarity(EdgePolarityEnum value) = 0;

            /// @~chinese 边缘1极性   @~english Edge 1 polarity
            __declspec(property(put = SetEdge1Polarity, get = GetEdge1Polarity)) Edge1PolarityEnum Edge1Polarity;

            virtual Edge1PolarityEnum GetEdge1Polarity() = 0;

            virtual void SetEdge1Polarity(Edge1PolarityEnum value) = 0;

            /// @~chinese 边缘对宽度，范围：[1,10000]   @~english EdgePairWidth，range:[1,10000]
            __declspec(property(put = SetEdgePairWidth, get = GetEdgePairWidth)) int EdgePairWidth;

            virtual int GetEdgePairWidth() = 0;

            virtual void SetEdgePairWidth(int value) = 0;

            /// @~chinese 最大结果数，范围：[1,1000]   @~english Max Result Number，range:[1,1000]
            __declspec(property(put = SetMaximum, get = GetMaximum)) int Maximum;

            virtual int GetMaximum() = 0;

            virtual void SetMaximum(int value) = 0;

            /// @~chinese 排序方式   @~english Sort Type
            __declspec(property(put = SetSortType, get = GetSortType)) SortTypeEnum SortType;

            virtual SortTypeEnum GetSortType() = 0;

            virtual void SetSortType(SortTypeEnum value) = 0;

            /// @~chinese 对比度   @~english HistContrast
            __declspec(property(put = SetContrastEnable, get = GetContrastEnable)) bool ContrastEnable;

            virtual bool GetContrastEnable() = 0;

            virtual void SetContrastEnable(bool value) = 0;

            /// @~chinese 边缘对对比度   @~english Edge Pair Contrast
            __declspec(property(put = SetContrastPairEnable, get = GetContrastPairEnable)) bool ContrastPairEnable;

            virtual bool GetContrastPairEnable() = 0;

            virtual void SetContrastPairEnable(bool value) = 0;

            /// @~chinese 曲线类型1   @~english Scoring1 Function
            __declspec(property(put = SetContrastDrop, get = GetContrastDrop)) ContrastDropEnum ContrastDrop;

            virtual ContrastDropEnum GetContrastDrop() = 0;

            virtual void SetContrastDrop(ContrastDropEnum value) = 0;

            /// @~chinese 起点，范围：[-100000,100000]   @~english Startpoint，range:[-100000,100000]
            __declspec(property(put = SetContrastX0, get = GetContrastX0)) int ContrastX0;

            virtual int GetContrastX0() = 0;

            virtual void SetContrastX0(int value) = 0;

            /// @~chinese X中点，范围：[-100000,100000]   @~english Midpoint，range:[-100000,100000]
            __declspec(property(put = SetContrastX1, get = GetContrastX1)) int ContrastX1;

            virtual int GetContrastX1() = 0;

            virtual void SetContrastX1(int value) = 0;

            /// @~chinese 终点，范围：[-100000,100000]   @~english Endpoint，range:[-100000,100000]
            __declspec(property(put = SetContrastXC, get = GetContrastXC)) int ContrastXC;

            virtual int GetContrastXC() = 0;

            virtual void SetContrastXC(int value) = 0;

            /// @~chinese Y计分，范围：[0,100]   @~english Scoring，range:[0,100]
            __declspec(property(put = SetContrastY0, get = GetContrastY0)) int ContrastY0;

            virtual int GetContrastY0() = 0;

            virtual void SetContrastY0(int value) = 0;

            /// @~chinese Y计分，范围：[0,100]   @~english Scoring，range:[0,100]
            __declspec(property(put = SetContrastY1, get = GetContrastY1)) int ContrastY1;

            virtual int GetContrastY1() = 0;

            virtual void SetContrastY1(int value) = 0;

            /// @~chinese 灰度   @~english Grayscale
            __declspec(property(put = SetGrayscaleEnable, get = GetGrayscaleEnable)) bool GrayscaleEnable;

            virtual bool GetGrayscaleEnable() = 0;

            virtual void SetGrayscaleEnable(bool value) = 0;

            /// @~chinese 曲线类型2   @~english Scoring2 Function
            __declspec(property(put = SetGrayscaleDrop, get = GetGrayscaleDrop)) GrayscaleDropEnum GrayscaleDrop;

            virtual GrayscaleDropEnum GetGrayscaleDrop() = 0;

            virtual void SetGrayscaleDrop(GrayscaleDropEnum value) = 0;

            /// @~chinese 起点，范围：[-100000,100000]   @~english Startpoint，range:[-100000,100000]
            __declspec(property(put = SetGrayscaleX0, get = GetGrayscaleX0)) int GrayscaleX0;

            virtual int GetGrayscaleX0() = 0;

            virtual void SetGrayscaleX0(int value) = 0;

            /// @~chinese X中点，范围：[-100000,100000]   @~english Midpoint，range:[-100000,100000]
            __declspec(property(put = SetGrayscaleX1, get = GetGrayscaleX1)) int GrayscaleX1;

            virtual int GetGrayscaleX1() = 0;

            virtual void SetGrayscaleX1(int value) = 0;

            /// @~chinese 终点，范围：[-100000,100000]   @~english Endpoint，range:[-100000,100000]
            __declspec(property(put = SetGrayscaleXC, get = GetGrayscaleXC)) int GrayscaleXC;

            virtual int GetGrayscaleXC() = 0;

            virtual void SetGrayscaleXC(int value) = 0;

            /// @~chinese Y计分，范围：[0,100]   @~english Scoring，range:[0,100]
            __declspec(property(put = SetGrayscaleY0, get = GetGrayscaleY0)) int GrayscaleY0;

            virtual int GetGrayscaleY0() = 0;

            virtual void SetGrayscaleY0(int value) = 0;

            /// @~chinese Y计分，范围：[0,100]   @~english Scoring，range:[0,100]
            __declspec(property(put = SetGrayscaleY1, get = GetGrayscaleY1)) int GrayscaleY1;

            virtual int GetGrayscaleY1() = 0;

            virtual void SetGrayscaleY1(int value) = 0;

            /// @~chinese 位置   @~english Position
            __declspec(property(put = SetPositionEnable, get = GetPositionEnable)) bool PositionEnable;

            virtual bool GetPositionEnable() = 0;

            virtual void SetPositionEnable(bool value) = 0;

            /// @~chinese 曲线类型3   @~english Scoring3 Function
            __declspec(property(put = SetPositionDrop, get = GetPositionDrop)) PositionDropEnum PositionDrop;

            virtual PositionDropEnum GetPositionDrop() = 0;

            virtual void SetPositionDrop(PositionDropEnum value) = 0;

            /// @~chinese 起点，范围：[-100000,100000]   @~english Startpoint，range:[-100000,100000]
            __declspec(property(put = SetPositionX0, get = GetPositionX0)) int PositionX0;

            virtual int GetPositionX0() = 0;

            virtual void SetPositionX0(int value) = 0;

            /// @~chinese X中点，范围：[-100000,100000]   @~english Midpoint，range:[-100000,100000]
            __declspec(property(put = SetPositionX1, get = GetPositionX1)) int PositionX1;

            virtual int GetPositionX1() = 0;

            virtual void SetPositionX1(int value) = 0;

            /// @~chinese 终点，范围：[-100000,100000]   @~english Endpoint，range:[-100000,100000]
            __declspec(property(put = SetPositionXC, get = GetPositionXC)) int PositionXC;

            virtual int GetPositionXC() = 0;

            virtual void SetPositionXC(int value) = 0;

            /// @~chinese Y计分，范围：[0,100]   @~english Scoring，range:[0,100]
            __declspec(property(put = SetPositionY0, get = GetPositionY0)) int PositionY0;

            virtual int GetPositionY0() = 0;

            virtual void SetPositionY0(int value) = 0;

            /// @~chinese Y计分，范围：[0,100]   @~english Scoring，range:[0,100]
            __declspec(property(put = SetPositionY1, get = GetPositionY1)) int PositionY1;

            virtual int GetPositionY1() = 0;

            virtual void SetPositionY1(int value) = 0;

            /// @~chinese 相对位置   @~english Relative Position
            __declspec(property(put = SetPositionNegEnable, get = GetPositionNegEnable)) bool PositionNegEnable;

            virtual bool GetPositionNegEnable() = 0;

            virtual void SetPositionNegEnable(bool value) = 0;

            /// @~chinese 曲线类型4   @~english Scoring4 Function
            __declspec(property(put = SetPositionNegDrop, get = GetPositionNegDrop)) PositionNegDropEnum PositionNegDrop;

            virtual PositionNegDropEnum GetPositionNegDrop() = 0;

            virtual void SetPositionNegDrop(PositionNegDropEnum value) = 0;

            /// @~chinese 起点，范围：[-100000,100000]   @~english Startpoint，range:[-100000,100000]
            __declspec(property(put = SetPositionNegX0, get = GetPositionNegX0)) int PositionNegX0;

            virtual int GetPositionNegX0() = 0;

            virtual void SetPositionNegX0(int value) = 0;

            /// @~chinese X中点，范围：[-100000,100000]   @~english Midpoint，range:[-100000,100000]
            __declspec(property(put = SetPositionNegX1, get = GetPositionNegX1)) int PositionNegX1;

            virtual int GetPositionNegX1() = 0;

            virtual void SetPositionNegX1(int value) = 0;

            /// @~chinese 终点，范围：[-100000,100000]   @~english Endpoint，range:[-100000,100000]
            __declspec(property(put = SetPositionNegXC, get = GetPositionNegXC)) int PositionNegXC;

            virtual int GetPositionNegXC() = 0;

            virtual void SetPositionNegXC(int value) = 0;

            /// @~chinese Y计分，范围：[0,100]   @~english Scoring，range:[0,100]
            __declspec(property(put = SetPositionNegY0, get = GetPositionNegY0)) int PositionNegY0;

            virtual int GetPositionNegY0() = 0;

            virtual void SetPositionNegY0(int value) = 0;

            /// @~chinese Y计分，范围：[0,100]   @~english Scoring，range:[0,100]
            __declspec(property(put = SetPositionNegY1, get = GetPositionNegY1)) int PositionNegY1;

            virtual int GetPositionNegY1() = 0;

            virtual void SetPositionNegY1(int value) = 0;

            /// @~chinese 归一位置   @~english Normalized Position
            __declspec(property(put = SetPositionNormEnable, get = GetPositionNormEnable)) bool PositionNormEnable;

            virtual bool GetPositionNormEnable() = 0;

            virtual void SetPositionNormEnable(bool value) = 0;

            /// @~chinese 曲线类型   @~english Scoring Function
            __declspec(property(put = SetPositionNormDrop, get = GetPositionNormDrop)) PositionNormDropEnum PositionNormDrop;

            virtual PositionNormDropEnum GetPositionNormDrop() = 0;

            virtual void SetPositionNormDrop(PositionNormDropEnum value) = 0;

            /// @~chinese 起点，范围：[-10000,10000]   @~english Startpoint，range:[-10000,10000]
            __declspec(property(put = SetPositionNormX0, get = GetPositionNormX0)) int PositionNormX0;

            virtual int GetPositionNormX0() = 0;

            virtual void SetPositionNormX0(int value) = 0;

            /// @~chinese X中点，范围：[-10000,10000]   @~english Midpoint，range:[-10000,10000]
            __declspec(property(put = SetPositionNormX1, get = GetPositionNormX1)) int PositionNormX1;

            virtual int GetPositionNormX1() = 0;

            virtual void SetPositionNormX1(int value) = 0;

            /// @~chinese 终点，范围：[-10000,10000]   @~english Endpoint，range:[-10000,10000]
            __declspec(property(put = SetPositionNormXC, get = GetPositionNormXC)) int PositionNormXC;

            virtual int GetPositionNormXC() = 0;

            virtual void SetPositionNormXC(int value) = 0;

            /// @~chinese Y计分，范围：[0,100]   @~english Scoring，range:[0,100]
            __declspec(property(put = SetPositionNormY0, get = GetPositionNormY0)) int PositionNormY0;

            virtual int GetPositionNormY0() = 0;

            virtual void SetPositionNormY0(int value) = 0;

            /// @~chinese Y计分，范围：[0,100]   @~english Scoring，range:[0,100]
            __declspec(property(put = SetPositionNormY1, get = GetPositionNormY1)) int PositionNormY1;

            virtual int GetPositionNormY1() = 0;

            virtual void SetPositionNormY1(int value) = 0;

            /// @~chinese 归一相对位置   @~english Normalized Relative Pos.
            __declspec(property(put = SetPositionNormNegEnable, get = GetPositionNormNegEnable)) bool PositionNormNegEnable;

            virtual bool GetPositionNormNegEnable() = 0;

            virtual void SetPositionNormNegEnable(bool value) = 0;

            /// @~chinese 曲线类型   @~english Scoring Function
            __declspec(property(put = SetPositionNormNegDrop, get = GetPositionNormNegDrop)) PositionNormNegDropEnum PositionNormNegDrop;

            virtual PositionNormNegDropEnum GetPositionNormNegDrop() = 0;

            virtual void SetPositionNormNegDrop(PositionNormNegDropEnum value) = 0;

            /// @~chinese 起点，范围：[-10000,10000]   @~english Startpoint，range:[-10000,10000]
            __declspec(property(put = SetPositionNormNegX0, get = GetPositionNormNegX0)) int PositionNormNegX0;

            virtual int GetPositionNormNegX0() = 0;

            virtual void SetPositionNormNegX0(int value) = 0;

            /// @~chinese X中点，范围：[-10000,10000]   @~english Midpoint，range:[-10000,10000]
            __declspec(property(put = SetPositionNormNegX1, get = GetPositionNormNegX1)) int PositionNormNegX1;

            virtual int GetPositionNormNegX1() = 0;

            virtual void SetPositionNormNegX1(int value) = 0;

            /// @~chinese 终点，范围：[-10000,10000]   @~english Endpoint，range:[-10000,10000]
            __declspec(property(put = SetPositionNormNegXC, get = GetPositionNormNegXC)) int PositionNormNegXC;

            virtual int GetPositionNormNegXC() = 0;

            virtual void SetPositionNormNegXC(int value) = 0;

            /// @~chinese Y计分，范围：[0,100]   @~english Scoring，range:[0,100]
            __declspec(property(put = SetPositionNormNegY0, get = GetPositionNormNegY0)) int PositionNormNegY0;

            virtual int GetPositionNormNegY0() = 0;

            virtual void SetPositionNormNegY0(int value) = 0;

            /// @~chinese Y计分，范围：[0,100]   @~english Scoring，range:[0,100]
            __declspec(property(put = SetPositionNormNegY1, get = GetPositionNormNegY1)) int PositionNormNegY1;

            virtual int GetPositionNormNegY1() = 0;

            virtual void SetPositionNormNegY1(int value) = 0;

            /// @~chinese 间距   @~english Interval
            __declspec(property(put = SetSizeNormEnable, get = GetSizeNormEnable)) bool SizeNormEnable;

            virtual bool GetSizeNormEnable() = 0;

            virtual void SetSizeNormEnable(bool value) = 0;

            /// @~chinese 曲线类型   @~english Scoring Function
            __declspec(property(put = SetSizeNormDrop, get = GetSizeNormDrop)) SizeNormDropEnum SizeNormDrop;

            virtual SizeNormDropEnum GetSizeNormDrop() = 0;

            virtual void SetSizeNormDrop(SizeNormDropEnum value) = 0;

            /// @~chinese 起点，范围：[-10000,10000]   @~english Startpoint，range:[-10000,10000]
            __declspec(property(put = SetSizeNormX0, get = GetSizeNormX0)) int SizeNormX0;

            virtual int GetSizeNormX0() = 0;

            virtual void SetSizeNormX0(int value) = 0;

            /// @~chinese X中点，范围：[-10000,10000]   @~english Midpoint，range:[-10000,10000]
            __declspec(property(put = SetSizeNormX1, get = GetSizeNormX1)) int SizeNormX1;

            virtual int GetSizeNormX1() = 0;

            virtual void SetSizeNormX1(int value) = 0;

            /// @~chinese 终点，范围：[-10000,10000]   @~english Endpoint，range:[-10000,10000]
            __declspec(property(put = SetSizeNormXC, get = GetSizeNormXC)) int SizeNormXC;

            virtual int GetSizeNormXC() = 0;

            virtual void SetSizeNormXC(int value) = 0;

            /// @~chinese Y计分，范围：[0,100]   @~english Scoring，range:[0,100]
            __declspec(property(put = SetSizeNormY0, get = GetSizeNormY0)) int SizeNormY0;

            virtual int GetSizeNormY0() = 0;

            virtual void SetSizeNormY0(int value) = 0;

            /// @~chinese Y计分，范围：[0,100]   @~english Scoring，range:[0,100]
            __declspec(property(put = SetSizeNormY1, get = GetSizeNormY1)) int SizeNormY1;

            virtual int GetSizeNormY1() = 0;

            virtual void SetSizeNormY1(int value) = 0;

            /// @~chinese 间距差   @~english Interval Difference
            __declspec(property(put = SetSizeDiffNormEnable, get = GetSizeDiffNormEnable)) bool SizeDiffNormEnable;

            virtual bool GetSizeDiffNormEnable() = 0;

            virtual void SetSizeDiffNormEnable(bool value) = 0;

            /// @~chinese 曲线类型   @~english Scoring Function
            __declspec(property(put = SetSizeDiffNormDrop, get = GetSizeDiffNormDrop)) SizeDiffNormDropEnum SizeDiffNormDrop;

            virtual SizeDiffNormDropEnum GetSizeDiffNormDrop() = 0;

            virtual void SetSizeDiffNormDrop(SizeDiffNormDropEnum value) = 0;

            /// @~chinese 起点，范围：[-10000,10000]   @~english Startpoint，range:[-10000,10000]
            __declspec(property(put = SetSizeDiffNormX0, get = GetSizeDiffNormX0)) int SizeDiffNormX0;

            virtual int GetSizeDiffNormX0() = 0;

            virtual void SetSizeDiffNormX0(int value) = 0;

            /// @~chinese X中点，范围：[-10000,10000]   @~english Midpoint，range:[-10000,10000]
            __declspec(property(put = SetSizeDiffNormX1, get = GetSizeDiffNormX1)) int SizeDiffNormX1;

            virtual int GetSizeDiffNormX1() = 0;

            virtual void SetSizeDiffNormX1(int value) = 0;

            /// @~chinese 终点，范围：[-10000,10000]   @~english Endpoint，range:[-10000,10000]
            __declspec(property(put = SetSizeDiffNormXC, get = GetSizeDiffNormXC)) int SizeDiffNormXC;

            virtual int GetSizeDiffNormXC() = 0;

            virtual void SetSizeDiffNormXC(int value) = 0;

            /// @~chinese Y计分，范围：[0,100]   @~english Scoring，range:[0,100]
            __declspec(property(put = SetSizeDiffNormY0, get = GetSizeDiffNormY0)) int SizeDiffNormY0;

            virtual int GetSizeDiffNormY0() = 0;

            virtual void SetSizeDiffNormY0(int value) = 0;

            /// @~chinese Y计分，范围：[0,100]   @~english Scoring，range:[0,100]
            __declspec(property(put = SetSizeDiffNormY1, get = GetSizeDiffNormY1)) int SizeDiffNormY1;

            virtual int GetSizeDiffNormY1() = 0;

            virtual void SetSizeDiffNormY1(int value) = 0;

            /// @~chinese 相对间距差   @~english Relative Interval Diff.
            __declspec(property(put = SetSizeDiffNormAsymEnable, get = GetSizeDiffNormAsymEnable)) bool SizeDiffNormAsymEnable;

            virtual bool GetSizeDiffNormAsymEnable() = 0;

            virtual void SetSizeDiffNormAsymEnable(bool value) = 0;

            /// @~chinese 左曲线类型   @~english Left Scoring Function
            __declspec(property(put = SetSizeDiffNormAsymDrop, get = GetSizeDiffNormAsymDrop)) SizeDiffNormAsymDropEnum SizeDiffNormAsymDrop;

            virtual SizeDiffNormAsymDropEnum GetSizeDiffNormAsymDrop() = 0;

            virtual void SetSizeDiffNormAsymDrop(SizeDiffNormAsymDropEnum value) = 0;

            /// @~chinese 起点，范围：[-10000,10000]   @~english Startpoint，range:[-10000,10000]
            __declspec(property(put = SetSizeDiffNormAsymX0, get = GetSizeDiffNormAsymX0)) int SizeDiffNormAsymX0;

            virtual int GetSizeDiffNormAsymX0() = 0;

            virtual void SetSizeDiffNormAsymX0(int value) = 0;

            /// @~chinese X中点，范围：[-10000,10000]   @~english Midpoint，range:[-10000,10000]
            __declspec(property(put = SetSizeDiffNormAsymX1, get = GetSizeDiffNormAsymX1)) int SizeDiffNormAsymX1;

            virtual int GetSizeDiffNormAsymX1() = 0;

            virtual void SetSizeDiffNormAsymX1(int value) = 0;

            /// @~chinese 终点，范围：[-10000,10000]   @~english Endpoint，range:[-10000,10000]
            __declspec(property(put = SetSizeDiffNormAsymXC, get = GetSizeDiffNormAsymXC)) int SizeDiffNormAsymXC;

            virtual int GetSizeDiffNormAsymXC() = 0;

            virtual void SetSizeDiffNormAsymXC(int value) = 0;

            /// @~chinese Y计分，范围：[0,100]   @~english Scoring，range:[0,100]
            __declspec(property(put = SetSizeDiffNormAsymY0, get = GetSizeDiffNormAsymY0)) int SizeDiffNormAsymY0;

            virtual int GetSizeDiffNormAsymY0() = 0;

            virtual void SetSizeDiffNormAsymY0(int value) = 0;

            /// @~chinese Y计分，范围：[0,100]   @~english Scoring，range:[0,100]
            __declspec(property(put = SetSizeDiffNormAsymY1, get = GetSizeDiffNormAsymY1)) int SizeDiffNormAsymY1;

            virtual int GetSizeDiffNormAsymY1() = 0;

            virtual void SetSizeDiffNormAsymY1(int value) = 0;

            /// @~chinese 右曲线类型   @~english Right Scoring Function
            __declspec(property(put = SetSizeDiffNormAsymDropH, get = GetSizeDiffNormAsymDropH)) SizeDiffNormAsymDropHEnum SizeDiffNormAsymDropH;

            virtual SizeDiffNormAsymDropHEnum GetSizeDiffNormAsymDropH() = 0;

            virtual void SetSizeDiffNormAsymDropH(SizeDiffNormAsymDropHEnum value) = 0;

            /// @~chinese 起点，范围：[-10000,10000]   @~english Startpoint，range:[-10000,10000]
            __declspec(property(put = SetSizeDiffNormAsymX0H, get = GetSizeDiffNormAsymX0H)) int SizeDiffNormAsymX0H;

            virtual int GetSizeDiffNormAsymX0H() = 0;

            virtual void SetSizeDiffNormAsymX0H(int value) = 0;

            /// @~chinese X中点，范围：[-10000,10000]   @~english Midpoint，range:[-10000,10000]
            __declspec(property(put = SetSizeDiffNormAsymX1H, get = GetSizeDiffNormAsymX1H)) int SizeDiffNormAsymX1H;

            virtual int GetSizeDiffNormAsymX1H() = 0;

            virtual void SetSizeDiffNormAsymX1H(int value) = 0;

            /// @~chinese 终点，范围：[-10000,10000]   @~english Endpoint，range:[-10000,10000]
            __declspec(property(put = SetSizeDiffNormAsymXCH, get = GetSizeDiffNormAsymXCH)) int SizeDiffNormAsymXCH;

            virtual int GetSizeDiffNormAsymXCH() = 0;

            virtual void SetSizeDiffNormAsymXCH(int value) = 0;

            /// @~chinese Y计分，范围：[0,100]   @~english Scoring，range:[0,100]
            __declspec(property(put = SetSizeDiffNormAsymY0H, get = GetSizeDiffNormAsymY0H)) int SizeDiffNormAsymY0H;

            virtual int GetSizeDiffNormAsymY0H() = 0;

            virtual void SetSizeDiffNormAsymY0H(int value) = 0;

            /// @~chinese Y计分，范围：[0,100]   @~english Scoring，range:[0,100]
            __declspec(property(put = SetSizeDiffNormAsymY1H, get = GetSizeDiffNormAsymY1H)) int SizeDiffNormAsymY1H;

            virtual int GetSizeDiffNormAsymY1H() = 0;

            virtual void SetSizeDiffNormAsymY1H(int value) = 0;

            /// @~chinese 投影方向   @~english Projection Direction
            __declspec(property(put = SetProjectionType, get = GetProjectionType)) ProjectionTypeEnum ProjectionType;

            virtual ProjectionTypeEnum GetProjectionType() = 0;

            virtual void SetProjectionType(ProjectionTypeEnum value) = 0;

            /// @~chinese 模糊边缘   @~english Fuzzy Edge
            __declspec(property(put = SetFuzzyedgeFlag, get = GetFuzzyedgeFlag)) bool FuzzyedgeFlag;

            virtual bool GetFuzzyedgeFlag() = 0;

            virtual void SetFuzzyedgeFlag(bool value) = 0;

            /// @~chinese 数量判断   @~english Quantity Check
            __declspec(property(put = SetNumLimitEnable, get = GetNumLimitEnable)) bool NumLimitEnable;

            virtual bool GetNumLimitEnable() = 0;

            virtual void SetNumLimitEnable(bool value) = 0;

            /// @~chinese 数量范围，范围：[0,99999]   @~english Quantity Range，range:[0,99999]
            __declspec(property(put = SetNumLimitLow, get = GetNumLimitLow)) int NumLimitLow;

            virtual int GetNumLimitLow() = 0;

            virtual void SetNumLimitLow(int value) = 0;

            /// @~chinese 数量范围，范围：[0,99999]   @~english Quantity Range，range:[0,99999]
            __declspec(property(put = SetNumLimitHigh, get = GetNumLimitHigh)) int NumLimitHigh;

            virtual int GetNumLimitHigh() = 0;

            virtual void SetNumLimitHigh(int value) = 0;

            /// @~chinese 宽度判断   @~english Width Check
            __declspec(property(put = SetEdgeWidthLimitEnable, get = GetEdgeWidthLimitEnable)) bool EdgeWidthLimitEnable;

            virtual bool GetEdgeWidthLimitEnable() = 0;

            virtual void SetEdgeWidthLimitEnable(bool value) = 0;

            /// @~chinese 宽度范围，范围：[0,99999]   @~english Width Range，range:[0,99999]
            __declspec(property(put = SetEdgeWidthLimitLow, get = GetEdgeWidthLimitLow)) double EdgeWidthLimitLow;

            virtual double GetEdgeWidthLimitLow() = 0;

            virtual void SetEdgeWidthLimitLow(double value) = 0;

            /// @~chinese 宽度范围，范围：[0,99999]   @~english Width Range，range:[0,99999]
            __declspec(property(put = SetEdgeWidthLimitHigh, get = GetEdgeWidthLimitHigh)) double EdgeWidthLimitHigh;

            virtual double GetEdgeWidthLimitHigh() = 0;

            virtual void SetEdgeWidthLimitHigh(double value) = 0;

            /// @~chinese 边缘点0X判断   @~english Edge Point 0X Check
            __declspec(property(put = SetEdge0PointXLimitEnable, get = GetEdge0PointXLimitEnable)) bool Edge0PointXLimitEnable;

            virtual bool GetEdge0PointXLimitEnable() = 0;

            virtual void SetEdge0PointXLimitEnable(bool value) = 0;

            /// @~chinese 边缘点0X范围，范围：[-99999,99999]   @~english Edge Point 0X Range，range:[-99999,99999]
            __declspec(property(put = SetEdge0PointXLimitLow, get = GetEdge0PointXLimitLow)) double Edge0PointXLimitLow;

            virtual double GetEdge0PointXLimitLow() = 0;

            virtual void SetEdge0PointXLimitLow(double value) = 0;

            /// @~chinese 边缘点0X范围，范围：[-99999,99999]   @~english Edge Point 0X Range，range:[-99999,99999]
            __declspec(property(put = SetEdge0PointXLimitHigh, get = GetEdge0PointXLimitHigh)) double Edge0PointXLimitHigh;

            virtual double GetEdge0PointXLimitHigh() = 0;

            virtual void SetEdge0PointXLimitHigh(double value) = 0;

            /// @~chinese 边缘点0Y判断   @~english Edge Point 0Y Check
            __declspec(property(put = SetEdge0PointYLimitEnable, get = GetEdge0PointYLimitEnable)) bool Edge0PointYLimitEnable;

            virtual bool GetEdge0PointYLimitEnable() = 0;

            virtual void SetEdge0PointYLimitEnable(bool value) = 0;

            /// @~chinese 边缘点0Y范围，范围：[-99999,99999]   @~english Edge Point 0Y Range，range:[-99999,99999]
            __declspec(property(put = SetEdge0PointYLimitLow, get = GetEdge0PointYLimitLow)) double Edge0PointYLimitLow;

            virtual double GetEdge0PointYLimitLow() = 0;

            virtual void SetEdge0PointYLimitLow(double value) = 0;

            /// @~chinese 边缘点0Y范围，范围：[-99999,99999]   @~english Edge Point 0Y Range，range:[-99999,99999]
            __declspec(property(put = SetEdge0PointYLimitHigh, get = GetEdge0PointYLimitHigh)) double Edge0PointYLimitHigh;

            virtual double GetEdge0PointYLimitHigh() = 0;

            virtual void SetEdge0PointYLimitHigh(double value) = 0;

            /// @~chinese 边缘点1X判断   @~english Edge Point 1X Check
            __declspec(property(put = SetEdge1PointXLimitEnable, get = GetEdge1PointXLimitEnable)) bool Edge1PointXLimitEnable;

            virtual bool GetEdge1PointXLimitEnable() = 0;

            virtual void SetEdge1PointXLimitEnable(bool value) = 0;

            /// @~chinese 边缘点1X范围，范围：[-99999,99999]   @~english Edge Point 1X Range，range:[-99999,99999]
            __declspec(property(put = SetEdge1PointXLimitLow, get = GetEdge1PointXLimitLow)) double Edge1PointXLimitLow;

            virtual double GetEdge1PointXLimitLow() = 0;

            virtual void SetEdge1PointXLimitLow(double value) = 0;

            /// @~chinese 边缘点1X范围，范围：[-99999,99999]   @~english Edge Point 1X Range，range:[-99999,99999]
            __declspec(property(put = SetEdge1PointXLimitHigh, get = GetEdge1PointXLimitHigh)) double Edge1PointXLimitHigh;

            virtual double GetEdge1PointXLimitHigh() = 0;

            virtual void SetEdge1PointXLimitHigh(double value) = 0;

            /// @~chinese 边缘点1Y判断   @~english Edge Point 1Y Check
            __declspec(property(put = SetEdge1PointYLimitEnable, get = GetEdge1PointYLimitEnable)) bool Edge1PointYLimitEnable;

            virtual bool GetEdge1PointYLimitEnable() = 0;

            virtual void SetEdge1PointYLimitEnable(bool value) = 0;

            /// @~chinese 边缘点1Y范围，范围：[-99999,99999]   @~english Edge Point 1Y Range，range:[-99999,99999]
            __declspec(property(put = SetEdge1PointYLimitLow, get = GetEdge1PointYLimitLow)) double Edge1PointYLimitLow;

            virtual double GetEdge1PointYLimitLow() = 0;

            virtual void SetEdge1PointYLimitLow(double value) = 0;

            /// @~chinese 边缘点1Y范围，范围：[-99999,99999]   @~english Edge Point 1Y Range，range:[-99999,99999]
            __declspec(property(put = SetEdge1PointYLimitHigh, get = GetEdge1PointYLimitHigh)) double Edge1PointYLimitHigh;

            virtual double GetEdge1PointYLimitHigh() = 0;

            virtual void SetEdge1PointYLimitHigh(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /** @enum  MVD_EDGEPOINT_STATUS
        *  @brief Type of the polarity of the edge point
        */
		/// 边缘极性
        typedef _MVD_EDGE_POLARITY__ MVD_EDGE_POLARITY;
        
        /*  Note: Interface Classes for describing single edge Information.  */
		/// 卡尺工具边缘信息
        class ICaliperSingleEdgeInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit ICaliperSingleEdgeInfo() {}
            virtual ~ICaliperSingleEdgeInfo() {}

        public:
            /**
             * @brief 边缘点坐标
             */
            virtual MVD_POINT_F GetEdgePoint() = 0;
            /**
             * @brief 边线相对box中心线相对距离（可正可负）
             */
            virtual float GetBias() = 0;
            /**
             * @brief 分数
             */
            virtual float GetScore() = 0;
            /**
             * @brief 边缘极性
             */
            virtual MVD_EDGE_POLARITY GetPolarity() = 0;
        };
		/// 卡尺边缘
        class ICaliperEdge
        {
        protected:
            ///< Constructor and Destructor
            explicit ICaliperEdge() {}
            virtual ~ICaliperEdge() {}

        public:
            ///< Properties
            /**
             * @brief 边缘点坐标
             */
            virtual MVD_POINT_F GetEdgePoint() = 0;
            /**
             * @brief 
             */
            virtual float GetBias() = 0;
            /**
             * @brief 边缘极性
             */
            virtual MVD_EDGE_POLARITY GetPolarity() = 0;
        };
		/// 卡尺边缘对信息
        class ICaliperEdgePairInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit ICaliperEdgePairInfo() {}
            virtual ~ICaliperEdgePairInfo() {}

        public:
            ///< Properties
            /**
             * @brief 边缘0信息
             */
            virtual ICaliperEdge* GetEdge0Info() = 0;
            /**
             * @brief 边缘1信息
             */
            virtual ICaliperEdge* GetEdge1Info() = 0;
            /**
             * @brief 距离
             */
            virtual float GetDistance() = 0;
            /**
             * @brief 得分
             */
            virtual float GetScore() = 0;
        };

        /*  Note: Interface Classes for describing single edge find result Information.  */
		/// 单边缘查找结果
        class ISingleEdgeFindResult
        {
        protected:
            ///< Constructor and Destructor
            explicit ISingleEdgeFindResult() {}
            virtual ~ISingleEdgeFindResult() {}

        public:
            ///< Properties
            /**
             * @brief 边缘个数
             */
            virtual int GetEdgeNum() = 0;
            /**
             * @brief 获取结果中的单个边缘点信息
             * @param nPtIndex [OUT] 边缘点信息
             * @return 返回结果边缘点序
             * @remarks nPtIndex从0开始计数
             */
            virtual ICaliperSingleEdgeInfo* GetSingleEdgeInfo( int nIndex ) = 0;
        };
        /// 边缘对查找结果
        class IEdgePairFindResult
        {
        protected:
            ///< Constructor and Destructor
            explicit IEdgePairFindResult() {}
            virtual ~IEdgePairFindResult() {}

        public:
            ///< Properties
            /**
             * @brief 边缘对个数
             */
            virtual int GetEdgePairNum() = 0;
            /**
             * @brief 获取结果中的边缘点信息
             * @param nIndex [OUT] 边缘点信息
             * @return 返回结果边缘点序
             * @remarks nIndex从0开始计数
             */
            virtual ICaliperEdgePairInfo* GetEdgePairInfo( int nIndex ) = 0;
        };

        /*  Note: Interface Classes of the result of the CaliperResults.  */
        ///卡尺工具结果类
        class CaliperResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CaliperResults() {}
            virtual ~CaliperResults() {}

        public:
             /**
             * @brief 圆弧1
             * @param nIndex[IN] 数据索引
             */
             virtual Annulus GetAnnulus1(int nIndex) = 0;

             /**
             * @brief 圆弧0
             * @param nIndex[IN] 数据索引
             */
             virtual Annulus GetAnnulus0(int nIndex) = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;

            /**
             * @brief 单边缘结果信息
             */
            virtual ISingleEdgeFindResult* GetSingleEdge() = 0;
            /**
             * @brief 边缘对结果信息
             */
            virtual IEdgePairFindResult* GetEdgePair() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the Caliper tool.  */
        ///卡尺工具工具类
        class IMVSCaliperModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCaliperModuTool() {}
            virtual ~IMVSCaliperModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CaliperParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CaliperResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSCaliperModuTool(IMVSCaliperModuTool&);
            IMVSCaliperModuTool& operator=(IMVSCaliperModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for Caliper.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSCaliperModu::IMVSCaliperModuTool * __stdcall GetCaliperToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CALIPER_H_
