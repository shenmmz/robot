/***************************************************************************************************
 * File：IVmBlobFind.h
 * Note：@~chinese Blob分析模块接口声明 @~english Interface for the BlobFind tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_BLOBFIND_H_
#define _IVM_BLOBFIND_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSBlobFindModu
    {
        /// @~chinese 阈值方式   @~english Threshold Mode
        typedef enum _ThresholdTypeEnum
        {
            /// @~chinese 不进行二值化   @~english No Binarization
            ThresholdType_BinaryImage = 0x1,

            /// @~chinese 单阈值   @~english Single Threshold
            ThresholdType_SingleThreshold = 2,

            /// @~chinese 双阈值   @~english Double Threshold
            ThresholdType_DoubleThreshold = 3,

            /// @~chinese 自动阈值   @~english Auto Threshold
            ThresholdType_AutoThreshold = 4,

            /// @~chinese 软阈值（固定）   @~english Soft Threshld(Fixed)
            ThresholdType_FixedSoftThreshold = 5,

            /// @~chinese 软阈值（相对）   @~english Soft Threshold(Relative)
            ThresholdType_RelativeSoftThreshold = 6,

        }ThresholdTypeEnum;

        /// @~chinese 极性   @~english Polarity
        typedef enum _PolarityEnum
        {
            /// @~chinese 亮于背景   @~english Brighter Than Background
            Polarity_DarkOnBright = 0,

            /// @~chinese 暗于背景   @~english Darker Than Background
            Polarity_BrightOnDark = 1,

        }PolarityEnum;

        /// @~chinese 排序特征   @~english Sort Feature
        typedef enum _SortFeatureEnum
        {
            /// @~chinese 面积   @~english Area
            SortFeature_SortFeatureArea = 0x1,

            /// @~chinese 周长   @~english Perimeter
            SortFeature_SortFeaturePerimeter = 0x2,

            /// @~chinese 圆形度   @~english CircleScore
            SortFeature_SortFeatureCircularity = 0x3,

            /// @~chinese 矩形度   @~english RectScore
            SortFeature_SortFeatureRect = 0x4,

            /// @~chinese 连通域中心x   @~english Connectivity center X
            SortFeature_SortFeatureCentroidX = 0x5,

            /// @~chinese 连通域中心y   @~english Connectivity center Y
            SortFeature_SortFeatureCentroidY = 0x6,

            /// @~chinese box角度   @~english box angle
            SortFeature_SortFeatureBoxAngle = 0x7,

            /// @~chinese box宽   @~english box width
            SortFeature_SortFeatureBoxWidth = 0x8,

            /// @~chinese box高   @~english box height
            SortFeature_SortFeatureBoxHeight = 0x9,

            /// @~chinese 矩形左上顶点x   @~english Rectangle left upper x
            SortFeature_SortFeatureRectX = 0xA,

            /// @~chinese 矩形左上顶点y   @~english Rectangle left upper y
            SortFeature_SortFeatureRectY = 0xB,

            /// @~chinese 二阶中心距主轴角度   @~english Two step spindle angle
            SortFeature_SortFeatureAxisAngle = 0xC,

            /// @~chinese 轴比(box短轴/box长轴)   @~english MinAxis / LongAxis
            SortFeature_SortFeatureAxisRatio = 0xD,

        }SortFeatureEnum;

        /// @~chinese 排序方式   @~english Sort Type
        typedef enum _SortModeEnum
        {
            /// @~chinese 升序   @~english Ascending
            SortMode_SortModeAscend = 0x1,

            /// @~chinese 降序   @~english Descending
            SortMode_SortModeDecend = 0x2,

            /// @~chinese 不排序   @~english Not Sorted
            SortMode_SortModeNotSort = 0x3,

        }SortModeEnum;

        /// @~chinese 连通性   @~english Connectivity 
        typedef enum _ConnectivityEnum
        {
            /// @~chinese 8连通   @~english 8-connected
            Connectivity_Connected_8 = 0x8,

            /// @~chinese 4连通   @~english 4-connected
            Connectivity_Connected_4 = 0x4,

        }ConnectivityEnum;
        
		/// @~chinese 输出轮廓类型   @~english Output Contour Type
        typedef enum _BlobContourTypeEnum
        {
            /// @~chinese 不输出轮廓   @~english No Contour Output
            BlobContourType_NoneContour = 0,

            /// @~chinese 输出无序轮廓   @~english None-Sequential Contour
            BlobContourType_NoneSequentialContour = 1,

            /// @~chinese 输出有序轮廓   @~english Sequential Contour
            BlobContourType_SequentialContour = 2,

        }BlobContourTypeEnum;
    /// \addtogroup Blob分析
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///Blob分析ROI管理类
        class BlobFindRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox* pRoiValue, unsigned int nValueNum) = 0;

            /// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
            virtual void SetRoiAnnulus(Annulus* pRoiValue, unsigned int nValueNum) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

            /// @~chinese 掩膜图像（仅当次执行起效）   @~english Mask Image(Only valid for this execution)
            virtual void SetMaskImage(ImageBaseData stMaskImage) = 0;

        };

        /*  Note: Interface Classes of the parameter of the BlobFindParams.  */
        ///Blob分析参数类
        class BlobFindParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit BlobFindParams() {}
            virtual ~BlobFindParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual BlobFindRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 阈值方式   @~english Threshold Mode
            __declspec(property(put = SetThresholdType, get = GetThresholdType)) ThresholdTypeEnum ThresholdType;

            virtual ThresholdTypeEnum GetThresholdType() = 0;

            virtual void SetThresholdType(ThresholdTypeEnum value) = 0;

            /// @~chinese 极性   @~english Polarity
            __declspec(property(put = SetPolarity, get = GetPolarity)) PolarityEnum Polarity;

            virtual PolarityEnum GetPolarity() = 0;

            virtual void SetPolarity(PolarityEnum value) = 0;

            /// @~chinese 低阈值，范围：[0,255]   @~english Low Threshold，range:[0,255]
            __declspec(property(put = SetLowThreshold, get = GetLowThreshold)) int LowThreshold;

            virtual int GetLowThreshold() = 0;

            virtual void SetLowThreshold(int value) = 0;

            /// @~chinese 高阈值，范围：[0,255]   @~english High Threshold，range:[0,255]
            __declspec(property(put = SetHightThreshold, get = GetHightThreshold)) int HightThreshold;

            virtual int GetHightThreshold() = 0;

            virtual void SetHightThreshold(int value) = 0;

            /// @~chinese 阈值范围，范围：[0,255]   @~english Threshold Range，range:[0,255]
            __declspec(property(put = SetLowSoftThreshold, get = GetLowSoftThreshold)) int LowSoftThreshold;

            virtual int GetLowSoftThreshold() = 0;

            virtual void SetLowSoftThreshold(int value) = 0;

            /// @~chinese 阈值范围，范围：[0,255]   @~english Threshold Range，range:[0,255]
            __declspec(property(put = SetHightSoftThreshold, get = GetHightSoftThreshold)) int HightSoftThreshold;

            virtual int GetHightSoftThreshold() = 0;

            virtual void SetHightSoftThreshold(int value) = 0;

            /// @~chinese 软阈值柔和度，范围：[0,254]   @~english Soft Threshold Softness，range:[0,254]
            __declspec(property(put = SetSoftness, get = GetSoftness)) int Softness;

            virtual int GetSoftness() = 0;

            virtual void SetSoftness(int value) = 0;

            /// @~chinese 低尾部比例，范围：[0,100]   @~english Low Tail Ratio，range:[0,100]
            __declspec(property(put = SetSoftLeftRatio, get = GetSoftLeftRatio)) int SoftLeftRatio;

            virtual int GetSoftLeftRatio() = 0;

            virtual void SetSoftLeftRatio(int value) = 0;

            /// @~chinese 高尾部比例，范围：[0,100]   @~english High Tail Ratio，range:[0,100]
            __declspec(property(put = SetSoftRightRatio, get = GetSoftRightRatio)) int SoftRightRatio;

            virtual int GetSoftRightRatio() = 0;

            virtual void SetSoftRightRatio(int value) = 0;

            /// @~chinese 低阈值比例，范围：[0,100]   @~english Low Threshold Ratio，range:[0,100]
            __declspec(property(put = SetSoftLowRatio, get = GetSoftLowRatio)) int SoftLowRatio;

            virtual int GetSoftLowRatio() = 0;

            virtual void SetSoftLowRatio(int value) = 0;

            /// @~chinese 高阈值比例，范围：[0,100]   @~english High Threshold Ratio，range:[0,100]
            __declspec(property(put = SetSoftHighRatio, get = GetSoftHighRatio)) int SoftHighRatio;

            virtual int GetSoftHighRatio() = 0;

            virtual void SetSoftHighRatio(int value) = 0;

            /// @~chinese 查找个数，范围：[1,20000]   @~english Number of Fund Blobs，range:[1,20000]
            __declspec(property(put = SetFindNum, get = GetFindNum)) int FindNum;

            virtual int GetFindNum() = 0;

            virtual void SetFindNum(int value) = 0;

            /// @~chinese 填充面积阈值，范围：[0,999999999]   @~english Fill Area Threshold，range:[0,999999999]
            __declspec(property(put = SetHoleMinArea, get = GetHoleMinArea)) int HoleMinArea;

            virtual int GetHoleMinArea() = 0;

            virtual void SetHoleMinArea(int value) = 0;

            /// @~chinese 面积使能   @~english Area Enable
            __declspec(property(put = SetSelectByArea, get = GetSelectByArea)) bool SelectByArea;

            virtual bool GetSelectByArea() = 0;

            virtual void SetSelectByArea(bool value) = 0;

            /// @~chinese 面积范围，范围：[1,999999999]   @~english Area Range，range:[1,999999999]
            __declspec(property(put = SetMinArea, get = GetMinArea)) int MinArea;

            virtual int GetMinArea() = 0;

            virtual void SetMinArea(int value) = 0;

            /// @~chinese 面积范围，范围：[1,999999999]   @~english Area Range，range:[1,999999999]
            __declspec(property(put = SetMaxArea, get = GetMaxArea)) int MaxArea;

            virtual int GetMaxArea() = 0;

            virtual void SetMaxArea(int value) = 0;

            /// @~chinese 轮廓输出使能(废弃)   @~english Contour Output(Obsoleted)
            __declspec(property(put = SetBolbOutLineEnable, get = GetBolbOutLineEnable)) bool BolbOutLineEnable;

            virtual bool GetBolbOutLineEnable() = 0;

            virtual void SetBolbOutLineEnable(bool value) = 0;

            /// @~chinese 输出轮廓类型   @~english Output Contour Type
            __declspec(property(put = SetBlobContourType, get = GetBlobContourType)) BlobContourTypeEnum BlobContourType;

            virtual BlobContourTypeEnum GetBlobContourType() = 0;

            virtual void SetBlobContourType(BlobContourTypeEnum value) = 0;

            /// @~chinese Blob图像输出   @~english Blob Image Output
            __declspec(property(put = SetBolbImageEnable, get = GetBolbImageEnable)) bool BolbImageEnable;

            virtual bool GetBolbImageEnable() = 0;

            virtual void SetBolbImageEnable(bool value) = 0;

            /// @~chinese 二值化图输出   @~english Binary Image Enable
            __declspec(property(put = SetBinaryImageEnable, get = GetBinaryImageEnable)) bool BinaryImageEnable;

            virtual bool GetBinaryImageEnable() = 0;

            virtual void SetBinaryImageEnable(bool value) = 0;

            /// @~chinese 直方图信息输出   @~english Hist Info Output
            __declspec(property(put = SetSelectByHistInfo, get = GetSelectByHistInfo)) bool SelectByHistInfo;

            virtual bool GetSelectByHistInfo() = 0;

            virtual void SetSelectByHistInfo(bool value) = 0;

            /// @~chinese 角度使能   @~english Angle Enable
            __declspec(property(put = SetSelectByBoxAngle, get = GetSelectByBoxAngle)) bool SelectByBoxAngle;

            virtual bool GetSelectByBoxAngle() = 0;

            virtual void SetSelectByBoxAngle(bool value) = 0;

            /// @~chinese 角度范围，范围：[-9000,9000]   @~english Angle Range，range:[-9000,9000]
            __declspec(property(put = SetMinBoxAngle, get = GetMinBoxAngle)) int MinBoxAngle;

            virtual int GetMinBoxAngle() = 0;

            virtual void SetMinBoxAngle(int value) = 0;

            /// @~chinese 角度范围，范围：[-9000,9000]   @~english Angle Range，range:[-9000,9000]
            __declspec(property(put = SetMaxBoxAngle, get = GetMaxBoxAngle)) int MaxBoxAngle;

            virtual int GetMaxBoxAngle() = 0;

            virtual void SetMaxBoxAngle(int value) = 0;

            /// @~chinese RECT宽使能   @~english RectWidth Enable
            __declspec(property(put = SetSelectByRectWidth, get = GetSelectByRectWidth)) bool SelectByRectWidth;

            virtual bool GetSelectByRectWidth() = 0;

            virtual void SetSelectByRectWidth(bool value) = 0;

            /// @~chinese RECT宽范围，范围：[1,1000000]   @~english RectWidth Range，range:[1,1000000]
            __declspec(property(put = SetMinRectWidth, get = GetMinRectWidth)) int MinRectWidth;

            virtual int GetMinRectWidth() = 0;

            virtual void SetMinRectWidth(int value) = 0;

            /// @~chinese RECT宽范围，范围：[1,1000000]   @~english RectWidth Range，range:[1,1000000]
            __declspec(property(put = SetMaxRectWidth, get = GetMaxRectWidth)) int MaxRectWidth;

            virtual int GetMaxRectWidth() = 0;

            virtual void SetMaxRectWidth(int value) = 0;

            /// @~chinese RECT高使能   @~english RectHeight Enable
            __declspec(property(put = SetSelectByRectHeight, get = GetSelectByRectHeight)) bool SelectByRectHeight;

            virtual bool GetSelectByRectHeight() = 0;

            virtual void SetSelectByRectHeight(bool value) = 0;

            /// @~chinese RECT高范围，范围：[1,1000000]   @~english RectHeight Range，range:[1,1000000]
            __declspec(property(put = SetMinRectHeight, get = GetMinRectHeight)) int MinRectHeight;

            virtual int GetMinRectHeight() = 0;

            virtual void SetMinRectHeight(int value) = 0;

            /// @~chinese RECT高范围，范围：[1,1000000]   @~english RectHeight Range，range:[1,1000000]
            __declspec(property(put = SetMaxRectHeight, get = GetMaxRectHeight)) int MaxRectHeight;

            virtual int GetMaxRectHeight() = 0;

            virtual void SetMaxRectHeight(int value) = 0;

            /// @~chinese 周长使能   @~english Perimeter
            __declspec(property(put = SetSelectByPerimeter, get = GetSelectByPerimeter)) bool SelectByPerimeter;

            virtual bool GetSelectByPerimeter() = 0;

            virtual void SetSelectByPerimeter(bool value) = 0;

            /// @~chinese 周长范围，范围：[1,999999999]   @~english Perimeter Range，range:[1,999999999]
            __declspec(property(put = SetMinPerimeter, get = GetMinPerimeter)) int MinPerimeter;

            virtual int GetMinPerimeter() = 0;

            virtual void SetMinPerimeter(int value) = 0;

            /// @~chinese 周长范围，范围：[1,999999999]   @~english Perimeter Range，range:[1,999999999]
            __declspec(property(put = SetMaxPerimeter, get = GetMaxPerimeter)) int MaxPerimeter;

            virtual int GetMaxPerimeter() = 0;

            virtual void SetMaxPerimeter(int value) = 0;

            /// @~chinese 短轴使能   @~english Minor Axis
            __declspec(property(put = SetSelectByShortAxis, get = GetSelectByShortAxis)) bool SelectByShortAxis;

            virtual bool GetSelectByShortAxis() = 0;

            virtual void SetSelectByShortAxis(bool value) = 0;

            /// @~chinese 短轴范围，范围：[1,999999999]   @~english Minor Axis Range，range:[1,999999999]
            __declspec(property(put = SetMinShortAxis, get = GetMinShortAxis)) int MinShortAxis;

            virtual int GetMinShortAxis() = 0;

            virtual void SetMinShortAxis(int value) = 0;

            /// @~chinese 短轴范围，范围：[1,999999999]   @~english Minor Axis Range，range:[1,999999999]
            __declspec(property(put = SetMaxShortAxis, get = GetMaxShortAxis)) int MaxShortAxis;

            virtual int GetMaxShortAxis() = 0;

            virtual void SetMaxShortAxis(int value) = 0;

            /// @~chinese 长轴使能   @~english Major Axis
            __declspec(property(put = SetSelectByLongAxis, get = GetSelectByLongAxis)) bool SelectByLongAxis;

            virtual bool GetSelectByLongAxis() = 0;

            virtual void SetSelectByLongAxis(bool value) = 0;

            /// @~chinese 长轴范围，范围：[1,999999999]   @~english Major Axis Range，range:[1,999999999]
            __declspec(property(put = SetMinLongAxis, get = GetMinLongAxis)) int MinLongAxis;

            virtual int GetMinLongAxis() = 0;

            virtual void SetMinLongAxis(int value) = 0;

            /// @~chinese 长轴范围，范围：[1,999999999]   @~english Major Axis Range，range:[1,999999999]
            __declspec(property(put = SetMaxLongAxis, get = GetMaxLongAxis)) int MaxLongAxis;

            virtual int GetMaxLongAxis() = 0;

            virtual void SetMaxLongAxis(int value) = 0;

            /// @~chinese 圆形度使能   @~english Circularity
            __declspec(property(put = SetSelectByCircularuty, get = GetSelectByCircularuty)) bool SelectByCircularuty;

            virtual bool GetSelectByCircularuty() = 0;

            virtual void SetSelectByCircularuty(bool value) = 0;

            /// @~chinese 圆形度范围，范围：[0,1]   @~english Circularity Range，range:[0,1]
            __declspec(property(put = SetMinCircularity, get = GetMinCircularity)) double MinCircularity;

            virtual double GetMinCircularity() = 0;

            virtual void SetMinCircularity(double value) = 0;

            /// @~chinese 圆形度范围，范围：[0,1]   @~english Circularity Range，range:[0,1]
            __declspec(property(put = SetMaxCircularity, get = GetMaxCircularity)) double MaxCircularity;

            virtual double GetMaxCircularity() = 0;

            virtual void SetMaxCircularity(double value) = 0;

            /// @~chinese 矩形度使能   @~english Rectangularity
            __declspec(property(put = SetSelectByRectangularity, get = GetSelectByRectangularity)) bool SelectByRectangularity;

            virtual bool GetSelectByRectangularity() = 0;

            virtual void SetSelectByRectangularity(bool value) = 0;

            /// @~chinese 矩形度范围，范围：[0,1]   @~english Rectangularity Range，range:[0,1]
            __declspec(property(put = SetMinRectangularity, get = GetMinRectangularity)) double MinRectangularity;

            virtual double GetMinRectangularity() = 0;

            virtual void SetMinRectangularity(double value) = 0;

            /// @~chinese 矩形度范围，范围：[0,1]   @~english Rectangularity Range，range:[0,1]
            __declspec(property(put = SetMaxRectangularity, get = GetMaxRectangularity)) double MaxRectangularity;

            virtual double GetMaxRectangularity() = 0;

            virtual void SetMaxRectangularity(double value) = 0;

            /// @~chinese 质心偏移使能   @~english Barycenter Offset
            __declspec(property(put = SetSelectByCentraBias, get = GetSelectByCentraBias)) bool SelectByCentraBias;

            virtual bool GetSelectByCentraBias() = 0;

            virtual void SetSelectByCentraBias(bool value) = 0;

            /// @~chinese 质心偏移范围，范围：[0,9999999.99]   @~english Barycenter Offset Range，range:[0,9999999.99]
            __declspec(property(put = SetMinCenterBias, get = GetMinCenterBias)) double MinCenterBias;

            virtual double GetMinCenterBias() = 0;

            virtual void SetMinCenterBias(double value) = 0;

            /// @~chinese 质心偏移范围，范围：[0,9999999.99]   @~english Barycenter Offset Range，range:[0,9999999.99]
            __declspec(property(put = SetMaxCenterBias, get = GetMaxCenterBias)) double MaxCenterBias;

            virtual double GetMaxCenterBias() = 0;

            virtual void SetMaxCenterBias(double value) = 0;

            /// @~chinese 轴比范围   @~english Axial Ratio
            __declspec(property(put = SetAxisRatioEnable, get = GetAxisRatioEnable)) bool AxisRatioEnable;

            virtual bool GetAxisRatioEnable() = 0;

            virtual void SetAxisRatioEnable(bool value) = 0;

            /// @~chinese 轴比范围，范围：[0.01,1]   @~english Axial Ratio，range:[0.01,1]
            __declspec(property(put = SetMinAxisRatio, get = GetMinAxisRatio)) double MinAxisRatio;

            virtual double GetMinAxisRatio() = 0;

            virtual void SetMinAxisRatio(double value) = 0;

            /// @~chinese 轴比范围，范围：[0.01,1]   @~english Axial Ratio，range:[0.01,1]
            __declspec(property(put = SetMaxAxisRatio, get = GetMaxAxisRatio)) double MaxAxisRatio;

            virtual double GetMaxAxisRatio() = 0;

            virtual void SetMaxAxisRatio(double value) = 0;

            /// @~chinese 排序特征   @~english Sort Feature
            __declspec(property(put = SetSortFeature, get = GetSortFeature)) SortFeatureEnum SortFeature;

            virtual SortFeatureEnum GetSortFeature() = 0;

            virtual void SetSortFeature(SortFeatureEnum value) = 0;

            /// @~chinese 排序方式   @~english Sort Type
            __declspec(property(put = SetSortMode, get = GetSortMode)) SortModeEnum SortMode;

            virtual SortModeEnum GetSortMode() = 0;

            virtual void SetSortMode(SortModeEnum value) = 0;

            /// @~chinese 连通性   @~english Connectivity 
            __declspec(property(put = SetConnectivity, get = GetConnectivity)) ConnectivityEnum Connectivity;

            virtual ConnectivityEnum GetConnectivity() = 0;

            virtual void SetConnectivity(ConnectivityEnum value) = 0;

            /// @~chinese 最大内接矩形输出数量，范围：[0,10]   @~english Max Area Inner Rect Output Number，range:[0,10]
            __declspec(property(put = SetMaxAreaInnerRectNum, get = GetMaxAreaInnerRectNum)) int MaxAreaInnerRectNum;

            virtual int GetMaxAreaInnerRectNum() = 0;

            virtual void SetMaxAreaInnerRectNum(int value) = 0;

            /// @~chinese 最小重叠率，范围：[0,100]   @~english Min. Overlap Rate，range:[0,100]
            __declspec(property(put = SetOverlapRatio, get = GetOverlapRatio)) int OverlapRatio;

            virtual int GetOverlapRatio() = 0;

            virtual void SetOverlapRatio(int value) = 0;

            /// @~chinese 像素超界使能   @~english RunParam_MaxOutPixelNumEnable
            __declspec(property(put = SetMaxOutPixelNumEnable, get = GetMaxOutPixelNumEnable)) bool MaxOutPixelNumEnable;

            virtual bool GetMaxOutPixelNumEnable() = 0;

            virtual void SetMaxOutPixelNumEnable(bool value) = 0;

            /// @~chinese 最大超界像素，范围：[0,9000000]   @~english MaxOutPixelNum，range:[0,9000000]
            __declspec(property(put = SetMaxOutPixelNum, get = GetMaxOutPixelNum)) int MaxOutPixelNum;

            virtual int GetMaxOutPixelNum() = 0;

            virtual void SetMaxOutPixelNum(int value) = 0;

            /// @~chinese Blob个数为0   @~english OKWhenNumIsZero
            __declspec(property(put = SetOKWhenNumIsZero, get = GetOKWhenNumIsZero)) bool OKWhenNumIsZero;

            virtual bool GetOKWhenNumIsZero() = 0;

            virtual void SetOKWhenNumIsZero(bool value) = 0;

            /// @~chinese Blob个数判断   @~english Blob Number Check
            __declspec(property(put = SetBlobNumLimitEnable, get = GetBlobNumLimitEnable)) bool BlobNumLimitEnable;

            virtual bool GetBlobNumLimitEnable() = 0;

            virtual void SetBlobNumLimitEnable(bool value) = 0;

            /// @~chinese Blob个数范围，范围：[1,99999]   @~english Blob Number Range，range:[1,99999]
            __declspec(property(put = SetBlobNumLimitLow, get = GetBlobNumLimitLow)) int BlobNumLimitLow;

            virtual int GetBlobNumLimitLow() = 0;

            virtual void SetBlobNumLimitLow(int value) = 0;

            /// @~chinese Blob个数范围，范围：[1,99999]   @~english Blob Number Range，range:[1,99999]
            __declspec(property(put = SetBlobNumLimitHigh, get = GetBlobNumLimitHigh)) int BlobNumLimitHigh;

            virtual int GetBlobNumLimitHigh() = 0;

            virtual void SetBlobNumLimitHigh(int value) = 0;

            /// @~chinese Blob面积判断   @~english Blob Area Check
            __declspec(property(put = SetBlobAreaLimitEnable, get = GetBlobAreaLimitEnable)) bool BlobAreaLimitEnable;

            virtual bool GetBlobAreaLimitEnable() = 0;

            virtual void SetBlobAreaLimitEnable(bool value) = 0;

            /// @~chinese Blob面积范围，范围：[1,999999999]   @~english Blob Area Range，range:[1,999999999]
            __declspec(property(put = SetBlobAreaLimitLow, get = GetBlobAreaLimitLow)) double BlobAreaLimitLow;

            virtual double GetBlobAreaLimitLow() = 0;

            virtual void SetBlobAreaLimitLow(double value) = 0;

            /// @~chinese Blob面积范围，范围：[1,999999999]   @~english Blob Area Range，range:[1,999999999]
            __declspec(property(put = SetBlobAreaLimitHigh, get = GetBlobAreaLimitHigh)) double BlobAreaLimitHigh;

            virtual double GetBlobAreaLimitHigh() = 0;

            virtual void SetBlobAreaLimitHigh(double value) = 0;

            /// @~chinese Blob总面积   @~english Blob Total Area Check
            __declspec(property(put = SetBlobTotalAreaLimitEnable, get = GetBlobTotalAreaLimitEnable)) bool BlobTotalAreaLimitEnable;

            virtual bool GetBlobTotalAreaLimitEnable() = 0;

            virtual void SetBlobTotalAreaLimitEnable(bool value) = 0;

            /// @~chinese 总面积范围，范围：[1,999999999]   @~english Blob Total Area Range，range:[1,999999999]
            __declspec(property(put = SetBlobTotalAreaLimitLow, get = GetBlobTotalAreaLimitLow)) double BlobTotalAreaLimitLow;

            virtual double GetBlobTotalAreaLimitLow() = 0;

            virtual void SetBlobTotalAreaLimitLow(double value) = 0;

            /// @~chinese 总面积范围，范围：[1,999999999]   @~english Blob Total Area Range，range:[1,999999999]
            __declspec(property(put = SetBlobTotalAreaLimitHigh, get = GetBlobTotalAreaLimitHigh)) double BlobTotalAreaLimitHigh;

            virtual double GetBlobTotalAreaLimitHigh() = 0;

            virtual void SetBlobTotalAreaLimitHigh(double value) = 0;

            /// @~chinese 质心X判断   @~english Barycenter X Check
            __declspec(property(put = SetCentroidXLimitEnable, get = GetCentroidXLimitEnable)) bool CentroidXLimitEnable;

            virtual bool GetCentroidXLimitEnable() = 0;

            virtual void SetCentroidXLimitEnable(bool value) = 0;

            /// @~chinese 质心X范围，范围：[-99999,99999]   @~english Barycenter X Range，range:[-99999,99999]
            __declspec(property(put = SetCentroidXLimitLow, get = GetCentroidXLimitLow)) double CentroidXLimitLow;

            virtual double GetCentroidXLimitLow() = 0;

            virtual void SetCentroidXLimitLow(double value) = 0;

            /// @~chinese 质心X范围，范围：[-99999,99999]   @~english Barycenter X Range，range:[-99999,99999]
            __declspec(property(put = SetCentroidXLimitHigh, get = GetCentroidXLimitHigh)) double CentroidXLimitHigh;

            virtual double GetCentroidXLimitHigh() = 0;

            virtual void SetCentroidXLimitHigh(double value) = 0;

            /// @~chinese 质心Y判断   @~english Barycenter Y Check
            __declspec(property(put = SetCentroidYLimitEnable, get = GetCentroidYLimitEnable)) bool CentroidYLimitEnable;

            virtual bool GetCentroidYLimitEnable() = 0;

            virtual void SetCentroidYLimitEnable(bool value) = 0;

            /// @~chinese 质心Y范围，范围：[-99999,99999]   @~english Barycenter Y Range，range:[-99999,99999]
            __declspec(property(put = SetCentroidYLimitLow, get = GetCentroidYLimitLow)) double CentroidYLimitLow;

            virtual double GetCentroidYLimitLow() = 0;

            virtual void SetCentroidYLimitLow(double value) = 0;

            /// @~chinese 质心Y范围，范围：[-99999,99999]   @~english Barycenter Y Range，range:[-99999,99999]
            __declspec(property(put = SetCentroidYLimitHigh, get = GetCentroidYLimitHigh)) double CentroidYLimitHigh;

            virtual double GetCentroidYLimitHigh() = 0;

            virtual void SetCentroidYLimitHigh(double value) = 0;

            /// @~chinese Box角度判断   @~english BoxAngle Check
            __declspec(property(put = SetAngleLimitEnable, get = GetAngleLimitEnable)) bool AngleLimitEnable;

            virtual bool GetAngleLimitEnable() = 0;

            virtual void SetAngleLimitEnable(bool value) = 0;

            /// @~chinese Box角度范围，范围：[-180,180]   @~english BoxAngle Range，range:[-180,180]
            __declspec(property(put = SetAngleLimitLow, get = GetAngleLimitLow)) double AngleLimitLow;

            virtual double GetAngleLimitLow() = 0;

            virtual void SetAngleLimitLow(double value) = 0;

            /// @~chinese Box角度范围，范围：[-180,180]   @~english BoxAngle Range，range:[-180,180]
            __declspec(property(put = SetAngleLimitHigh, get = GetAngleLimitHigh)) double AngleLimitHigh;

            virtual double GetAngleLimitHigh() = 0;

            virtual void SetAngleLimitHigh(double value) = 0;

            /// @~chinese 中心X判断   @~english Center X Check
            __declspec(property(put = SetCenterXLimitEnable, get = GetCenterXLimitEnable)) bool CenterXLimitEnable;

            virtual bool GetCenterXLimitEnable() = 0;

            virtual void SetCenterXLimitEnable(bool value) = 0;

            /// @~chinese 中心X范围，范围：[-99999,99999]   @~english Center X Range，range:[-99999,99999]
            __declspec(property(put = SetCenterXLimitLow, get = GetCenterXLimitLow)) double CenterXLimitLow;

            virtual double GetCenterXLimitLow() = 0;

            virtual void SetCenterXLimitLow(double value) = 0;

            /// @~chinese 中心X范围，范围：[-99999,99999]   @~english Center X Range，range:[-99999,99999]
            __declspec(property(put = SetCenterXLimitHigh, get = GetCenterXLimitHigh)) double CenterXLimitHigh;

            virtual double GetCenterXLimitHigh() = 0;

            virtual void SetCenterXLimitHigh(double value) = 0;

            /// @~chinese 中心Y判断   @~english Center Y Check
            __declspec(property(put = SetCenterYLimitEnable, get = GetCenterYLimitEnable)) bool CenterYLimitEnable;

            virtual bool GetCenterYLimitEnable() = 0;

            virtual void SetCenterYLimitEnable(bool value) = 0;

            /// @~chinese 中心Y范围，范围：[-99999,99999]   @~english Center Y Range，range:[-99999,99999]
            __declspec(property(put = SetCenterYLimitLow, get = GetCenterYLimitLow)) double CenterYLimitLow;

            virtual double GetCenterYLimitLow() = 0;

            virtual void SetCenterYLimitLow(double value) = 0;

            /// @~chinese 中心Y范围，范围：[-99999,99999]   @~english Center Y Range，range:[-99999,99999]
            __declspec(property(put = SetCenterYLimitHigh, get = GetCenterYLimitHigh)) double CenterYLimitHigh;

            virtual double GetCenterYLimitHigh() = 0;

            virtual void SetCenterYLimitHigh(double value) = 0;

            /// @~chinese Box高度判断   @~english Box Height Check
            __declspec(property(put = SetBoxHeightLimitEnable, get = GetBoxHeightLimitEnable)) bool BoxHeightLimitEnable;

            virtual bool GetBoxHeightLimitEnable() = 0;

            virtual void SetBoxHeightLimitEnable(bool value) = 0;

            /// @~chinese Box高度范围，范围：[1,99999]   @~english Box Height Range，range:[1,99999]
            __declspec(property(put = SetBoxHeightLimitLow, get = GetBoxHeightLimitLow)) double BoxHeightLimitLow;

            virtual double GetBoxHeightLimitLow() = 0;

            virtual void SetBoxHeightLimitLow(double value) = 0;

            /// @~chinese Box高度范围，范围：[1,99999]   @~english Box Height Range，range:[1,99999]
            __declspec(property(put = SetBoxHeightLimitHigh, get = GetBoxHeightLimitHigh)) double BoxHeightLimitHigh;

            virtual double GetBoxHeightLimitHigh() = 0;

            virtual void SetBoxHeightLimitHigh(double value) = 0;

            /// @~chinese Box宽度判断   @~english Box Width Check
            __declspec(property(put = SetBoxWidthLimitEnable, get = GetBoxWidthLimitEnable)) bool BoxWidthLimitEnable;

            virtual bool GetBoxWidthLimitEnable() = 0;

            virtual void SetBoxWidthLimitEnable(bool value) = 0;

            /// @~chinese Box宽度范围，范围：[1,99999]   @~english Box Width Range，range:[1,99999]
            __declspec(property(put = SetBoxWidthLimitLow, get = GetBoxWidthLimitLow)) double BoxWidthLimitLow;

            virtual double GetBoxWidthLimitLow() = 0;

            virtual void SetBoxWidthLimitLow(double value) = 0;

            /// @~chinese Box宽度范围，范围：[1,99999]   @~english Box Width Range，range:[1,99999]
            __declspec(property(put = SetBoxWidthLimitHigh, get = GetBoxWidthLimitHigh)) double BoxWidthLimitHigh;

            virtual double GetBoxWidthLimitHigh() = 0;

            virtual void SetBoxWidthLimitHigh(double value) = 0;

            /// @~chinese 周长判断   @~english Perimeter Check
            __declspec(property(put = SetPerimeterLimitEnable, get = GetPerimeterLimitEnable)) bool PerimeterLimitEnable;

            virtual bool GetPerimeterLimitEnable() = 0;

            virtual void SetPerimeterLimitEnable(bool value) = 0;

            /// @~chinese 周长范围，范围：[1,999999999]   @~english Perimeter Range，range:[1,999999999]
            __declspec(property(put = SetPerimeterLimitLow, get = GetPerimeterLimitLow)) double PerimeterLimitLow;

            virtual double GetPerimeterLimitLow() = 0;

            virtual void SetPerimeterLimitLow(double value) = 0;

            /// @~chinese 周长范围，范围：[1,999999999]   @~english Perimeter Range，range:[1,999999999]
            __declspec(property(put = SetPerimeterLimitHigh, get = GetPerimeterLimitHigh)) double PerimeterLimitHigh;

            virtual double GetPerimeterLimitHigh() = 0;

            virtual void SetPerimeterLimitHigh(double value) = 0;

            /// @~chinese 长轴判断   @~english Major Axis Check
            __declspec(property(put = SetLongAxisLimitEnable, get = GetLongAxisLimitEnable)) bool LongAxisLimitEnable;

            virtual bool GetLongAxisLimitEnable() = 0;

            virtual void SetLongAxisLimitEnable(bool value) = 0;

            /// @~chinese 长轴范围，范围：[1,999999999]   @~english Major Axis Range，range:[1,999999999]
            __declspec(property(put = SetLongAxisLimitLow, get = GetLongAxisLimitLow)) double LongAxisLimitLow;

            virtual double GetLongAxisLimitLow() = 0;

            virtual void SetLongAxisLimitLow(double value) = 0;

            /// @~chinese 长轴范围，范围：[1,999999999]   @~english Major Axis Range，range:[1,999999999]
            __declspec(property(put = SetLongAxisLimitHigh, get = GetLongAxisLimitHigh)) double LongAxisLimitHigh;

            virtual double GetLongAxisLimitHigh() = 0;

            virtual void SetLongAxisLimitHigh(double value) = 0;

            /// @~chinese 短轴判断   @~english Minor Axis Check
            __declspec(property(put = SetShortAxisLimitEnable, get = GetShortAxisLimitEnable)) bool ShortAxisLimitEnable;

            virtual bool GetShortAxisLimitEnable() = 0;

            virtual void SetShortAxisLimitEnable(bool value) = 0;

            /// @~chinese 短轴范围，范围：[1,999999999]   @~english Minor Axis Range，range:[1,999999999]
            __declspec(property(put = SetShortAxisLimitLow, get = GetShortAxisLimitLow)) double ShortAxisLimitLow;

            virtual double GetShortAxisLimitLow() = 0;

            virtual void SetShortAxisLimitLow(double value) = 0;

            /// @~chinese 短轴范围，范围：[1,999999999]   @~english Minor Axis Range，range:[1,999999999]
            __declspec(property(put = SetShortAxisLimitHigh, get = GetShortAxisLimitHigh)) double ShortAxisLimitHigh;

            virtual double GetShortAxisLimitHigh() = 0;

            virtual void SetShortAxisLimitHigh(double value) = 0;

            /// @~chinese 圆形度判断   @~english Circularity Check
            __declspec(property(put = SetCircularityLimitEnable, get = GetCircularityLimitEnable)) bool CircularityLimitEnable;

            virtual bool GetCircularityLimitEnable() = 0;

            virtual void SetCircularityLimitEnable(bool value) = 0;

            /// @~chinese 圆形度范围，范围：[0,1]   @~english Circularity Range，range:[0,1]
            __declspec(property(put = SetCircularityLimitLow, get = GetCircularityLimitLow)) double CircularityLimitLow;

            virtual double GetCircularityLimitLow() = 0;

            virtual void SetCircularityLimitLow(double value) = 0;

            /// @~chinese 圆形度范围，范围：[0,1]   @~english Circularity Range，range:[0,1]
            __declspec(property(put = SetCircularityLimitHigh, get = GetCircularityLimitHigh)) double CircularityLimitHigh;

            virtual double GetCircularityLimitHigh() = 0;

            virtual void SetCircularityLimitHigh(double value) = 0;

            /// @~chinese 矩形度判断   @~english Rectangularity Check
            __declspec(property(put = SetRectangularityLimitEnable, get = GetRectangularityLimitEnable)) bool RectangularityLimitEnable;

            virtual bool GetRectangularityLimitEnable() = 0;

            virtual void SetRectangularityLimitEnable(bool value) = 0;

            /// @~chinese 矩形度范围，范围：[0,1]   @~english Rectangularity Range，range:[0,1]
            __declspec(property(put = SetRectangularityLimitLow, get = GetRectangularityLimitLow)) double RectangularityLimitLow;

            virtual double GetRectangularityLimitLow() = 0;

            virtual void SetRectangularityLimitLow(double value) = 0;

            /// @~chinese 矩形度范围，范围：[0,1]   @~english Rectangularity Range，range:[0,1]
            __declspec(property(put = SetRectangularityLimitHigh, get = GetRectangularityLimitHigh)) double RectangularityLimitHigh;

            virtual double GetRectangularityLimitHigh() = 0;

            virtual void SetRectangularityLimitHigh(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes for describing information of each blob.  */
        ///Blob信息描述类
        class IBlobInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IBlobInfo() {}
            virtual ~IBlobInfo() {}

        public:
            /**
             * @brief 单体状态
             */
            virtual int GetBlobStatus() = 0;
            /**
             * @brief 面积.V3.2.0之后推荐调用AreaF属性查询
             */
            virtual int GetArea() = 0;
            /**
             * @brief 周长
             */
            virtual float GetPerimeter() = 0;
            /**
             * @brief 圆形度
             */
            virtual float GetCircularity() = 0;
            /**
             * @brief 最小外接矩形短轴
             */
            virtual float GetShortAxis() = 0;
            /**
             * @brief 最小外接矩形长轴
             */
            virtual float GetLongAxis() = 0;
            /**
             * @brief 角度
             */
            virtual float GetAngle() = 0;
            /**
             * @brief 矩形度
             */
            virtual float GetRectangularity() = 0;
            /**
             * @brief Blob得分
             */
            virtual float GetScore() = 0;
            /**
             * @brief 最小面积外接矩形
             */
            virtual RectBox GetBoxInfo() = 0;
            /**
             * @brief 连通域的边界框
             */
            virtual RectBox GetRectInfo() = 0;
            /**
            * @brief 连通域的左上点
            */
            virtual PointF GetRectUpLeftPoint() = 0;
            /**
            * @brief 连通域的右下点
            */
            virtual PointF GetRectBottomRightPoint() = 0;
            /**
             * @brief 连通域中心（质心）
             */
            virtual MVD_POINT_F GetCenteroid() = 0;
            /**
             * @brief 面积,值同Area，V3.2.0新增浮点类型重载提高精度
             */
            virtual float GetAreaF() = 0;
        };

        /*  Note: Interface Classes of the result of the BlobFindResults.  */
        ///Blob分析结果类
        class BlobFindResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit BlobFindResults() {}
            virtual ~BlobFindResults() {}

        public:
             /**
             * @brief 屏蔽区字符串
             */
             virtual StringData* GetBlindPolygonString(unsigned int& nValueNum) = 0;

             /**
             * @brief 输出掩膜
             */
             virtual ImageBaseData GetOutputMask() = 0;

             /**
             * @brief 屏蔽区多边形点集
             */
             virtual PointF GetBlindPolygonPoints(int nIndex) = 0;

             /**
             * @brief 屏蔽区多边形点数
             */
             virtual int GetBlindPolygonPointNum(int nIndex) = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
             /**
             * @brief 最大内接矩形
             * @param nIndex[IN] 数据索引
             */
             virtual RectBox GetMaxAreaInnerRect(int nIndex) = 0;

             /**
             * @brief 对比度
             * @param nIndex[IN] 数据索引
             */
             virtual float GetGrayContrast(int nIndex) = 0;

             /**
             * @brief 最大灰度值
             * @param nIndex[IN] 数据索引
             */
             virtual float GetMaxGrayValue(int nIndex) = 0;

             /**
             * @brief 最小灰度值
             * @param nIndex[IN] 数据索引
             */
             virtual float GetMinGrayValue(int nIndex) = 0;
             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI(int nIndex) = 0;
             /**
             * @brief ROI圆弧
             */
             virtual Annulus GetROIAnnulus(int nIndex) = 0;
            /**
             * @brief Blob数量
             */
            virtual int GetBlobNum() = 0;
            /**
             * @brief 获取Blob分析的结果信息
             */
            virtual IBlobInfo* GetBlobInfo(int nBlobIndex) = 0;
            /**
             * @brief 二值化图像
             */
            virtual ImageBaseData GetBinaryImage() = 0;
            /**
             * @brief 获取Blob图像
             */
            virtual ImageBaseData GetBlobImage() = 0;
            /**
             * @brief 获取Blob总面积
             */
            virtual int GetBlobTotalArea() = 0;
            /**
             * @brief 轮廓点数量
             */
            virtual int GetContourPointNum() = 0;
            /**
             * @brief 轮廓点
             */
            virtual MVD_POINT_S* GetContourPoints() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the BlobFind tool.  */
        ///Blob分析工具类
        class IMVSBlobFindModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSBlobFindModuTool() {}
            virtual ~IMVSBlobFindModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual BlobFindParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual BlobFindResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSBlobFindModuTool(IMVSBlobFindModuTool&);
            IMVSBlobFindModuTool& operator=(IMVSBlobFindModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for BlobFind.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSBlobFindModu::IMVSBlobFindModuTool * __stdcall GetBlobFindToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_BLOBFIND_H_
