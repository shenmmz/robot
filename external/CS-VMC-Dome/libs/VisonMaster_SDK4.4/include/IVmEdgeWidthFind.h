/***************************************************************************************************
 * File：IVmEdgeWidthFind.h
 * Note：@~chinese 间距检测模块接口声明 @~english Interface for the EdgeWidthFind tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_EDGEWIDTHFIND_H_
#define _IVM_EDGEWIDTHFIND_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSEdgeWidthFindModu
    {
        /// @~chinese 边缘0极性   @~english Edge 0 polarity
        typedef enum _Edge0PolarityEnum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            Edge0Polarity_BlackToWhite = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            Edge0Polarity_WhiteToBlack = 0x2,

            /// @~chinese 任意极性   @~english Both
            Edge0Polarity_Both = 0x3,

        }Edge0PolarityEnum;

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

        /// @~chinese 边缘对类型   @~english Edge Pair Type
        typedef enum _FindModeEnum
        {
            /// @~chinese 最宽   @~english Widest Rect
            FindMode_Widest = 0x1,

            /// @~chinese 最窄   @~english Narrowest Rect
            FindMode_Narrowest = 0x2,

            /// @~chinese 最强   @~english Strongest Edge
            FindMode_Strongest = 0x3,

            /// @~chinese 最弱   @~english Min Contrast
            FindMode_Worst = 0x4,

            /// @~chinese 第一对   @~english The First Pair
            FindMode_First = 0x5,

            /// @~chinese 最后一对   @~english The Last Pair
            FindMode_Last = 0x6,

            /// @~chinese 最接近   @~english Best Rectangularity
            FindMode_Nearest = 0x7,

            /// @~chinese 最不接近   @~english Worst Rectangularity
            FindMode_Farthest = 0x8,

            /// @~chinese 全部   @~english All
            FindMode_All = 0x9,

        }FindModeEnum;

        /// @~chinese 查找方向   @~english Search Direction
        typedef enum _FindOrientEnum
        {
            /// @~chinese 从上到下   @~english Top to Bottom
            FindOrient_UpToDown = 0x1,

            /// @~chinese 从左到右   @~english Left to Right
            FindOrient_LeftToRight = 0x2,

        }FindOrientEnum;

    /// \addtogroup 间距检测
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///间距检测ROI管理类
        class EdgeWidthFindRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;
			/// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
			virtual void SetRoiAnnulus(Annulus pRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the EdgeWidthFindParams.  */
        ///间距检测参数类
        class EdgeWidthFindParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit EdgeWidthFindParams() {}
            virtual ~EdgeWidthFindParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual EdgeWidthFindRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 滤波尺寸，范围：[1,50]   @~english Filter Size，range:[1,50]
            __declspec(property(put = SetHalfKernelSize, get = GetHalfKernelSize)) int HalfKernelSize;

            virtual int GetHalfKernelSize() = 0;

            virtual void SetHalfKernelSize(int value) = 0;

            /// @~chinese 边缘阈值，范围：[0,255]   @~english Edge Threshold，range:[0,255]
            __declspec(property(put = SetContrastTH, get = GetContrastTH)) int ContrastTH;

            virtual int GetContrastTH() = 0;

            virtual void SetContrastTH(int value) = 0;

            /// @~chinese 边缘0极性   @~english Edge 0 polarity
            __declspec(property(put = SetEdge0Polarity, get = GetEdge0Polarity)) Edge0PolarityEnum Edge0Polarity;

            virtual Edge0PolarityEnum GetEdge0Polarity() = 0;

            virtual void SetEdge0Polarity(Edge0PolarityEnum value) = 0;

            /// @~chinese 边缘1极性   @~english Edge 1 polarity
            __declspec(property(put = SetEdge1Polarity, get = GetEdge1Polarity)) Edge1PolarityEnum Edge1Polarity;

            virtual Edge1PolarityEnum GetEdge1Polarity() = 0;

            virtual void SetEdge1Polarity(Edge1PolarityEnum value) = 0;

            /// @~chinese 最大结果数，范围：[1,1000]   @~english Max Result Number，range:[1,1000]
            __declspec(property(put = SetMaximum, get = GetMaximum)) int Maximum;

            virtual int GetMaximum() = 0;

            virtual void SetMaximum(int value) = 0;

            /// @~chinese 排序方式   @~english Sort Type
            __declspec(property(put = SetSortType, get = GetSortType)) SortTypeEnum SortType;

            virtual SortTypeEnum GetSortType() = 0;

            virtual void SetSortType(SortTypeEnum value) = 0;

            /// @~chinese 边缘对类型   @~english Edge Pair Type
            __declspec(property(put = SetFindMode, get = GetFindMode)) FindModeEnum FindMode;

            virtual FindModeEnum GetFindMode() = 0;

            virtual void SetFindMode(FindModeEnum value) = 0;

            /// @~chinese 理想间距，范围：[1,10000]   @~english Ideal Distance，range:[1,10000]
            __declspec(property(put = SetIdeaWidth, get = GetIdeaWidth)) int IdeaWidth;

            virtual int GetIdeaWidth() = 0;

            virtual void SetIdeaWidth(int value) = 0;

            /// @~chinese 查找方向   @~english Search Direction
            __declspec(property(put = SetFindOrient, get = GetFindOrient)) FindOrientEnum FindOrient;

            virtual FindOrientEnum GetFindOrient() = 0;

            virtual void SetFindOrient(FindOrientEnum value) = 0;

            /// @~chinese 最小边缘分数，范围：[0,1.0]   @~english Min Edge Score，range:[0,1.0]
            __declspec(property(put = SetMinScore, get = GetMinScore)) double MinScore;

            virtual double GetMinScore() = 0;

            virtual void SetMinScore(double value) = 0;

            /// @~chinese 数量判断   @~english Quantity Check
            __declspec(property(put = SetNumLimitEnable, get = GetNumLimitEnable)) bool NumLimitEnable;

            virtual bool GetNumLimitEnable() = 0;

            virtual void SetNumLimitEnable(bool value) = 0;

            /// @~chinese 数量判断，范围：[0,99999]   @~english Quantity Check，range:[0,99999]
            __declspec(property(put = SetNumLimitLow, get = GetNumLimitLow)) int NumLimitLow;

            virtual int GetNumLimitLow() = 0;

            virtual void SetNumLimitLow(int value) = 0;

            /// @~chinese 数量判断，范围：[0,99999]   @~english Quantity Check，range:[0,99999]
            __declspec(property(put = SetNumLimitHigh, get = GetNumLimitHigh)) int NumLimitHigh;

            virtual int GetNumLimitHigh() = 0;

            virtual void SetNumLimitHigh(int value) = 0;

            /// @~chinese 宽度判断   @~english Width Check
            __declspec(property(put = SetEdgeWidthLimitEnable, get = GetEdgeWidthLimitEnable)) bool EdgeWidthLimitEnable;

            virtual bool GetEdgeWidthLimitEnable() = 0;

            virtual void SetEdgeWidthLimitEnable(bool value) = 0;

            /// @~chinese 宽度判断，范围：[0.1,99999]   @~english Width Check，range:[0.1,99999]
            __declspec(property(put = SetEdgeWidthLimitLow, get = GetEdgeWidthLimitLow)) double EdgeWidthLimitLow;

            virtual double GetEdgeWidthLimitLow() = 0;

            virtual void SetEdgeWidthLimitLow(double value) = 0;

            /// @~chinese 宽度判断，范围：[0.1,99999]   @~english Width Check，range:[0.1,99999]
            __declspec(property(put = SetEdgeWidthLimitHigh, get = GetEdgeWidthLimitHigh)) double EdgeWidthLimitHigh;

            virtual double GetEdgeWidthLimitHigh() = 0;

            virtual void SetEdgeWidthLimitHigh(double value) = 0;

            /// @~chinese 边缘点0X判断   @~english Edge Point 0X Check
            __declspec(property(put = SetEdge0PointXLimitEnable, get = GetEdge0PointXLimitEnable)) bool Edge0PointXLimitEnable;

            virtual bool GetEdge0PointXLimitEnable() = 0;

            virtual void SetEdge0PointXLimitEnable(bool value) = 0;

            /// @~chinese 边缘点0X范围，范围：[0,99999]   @~english Edge Point 0X Range，range:[0,99999]
            __declspec(property(put = SetEdge0PointXLimitLow, get = GetEdge0PointXLimitLow)) double Edge0PointXLimitLow;

            virtual double GetEdge0PointXLimitLow() = 0;

            virtual void SetEdge0PointXLimitLow(double value) = 0;

            /// @~chinese 边缘点0X范围，范围：[0,99999]   @~english Edge Point 0X Range，range:[0,99999]
            __declspec(property(put = SetEdge0PointXLimitHigh, get = GetEdge0PointXLimitHigh)) double Edge0PointXLimitHigh;

            virtual double GetEdge0PointXLimitHigh() = 0;

            virtual void SetEdge0PointXLimitHigh(double value) = 0;

            /// @~chinese 边缘点0Y判断   @~english Edge Point 0Y Check
            __declspec(property(put = SetEdge0PointYLimitEnable, get = GetEdge0PointYLimitEnable)) bool Edge0PointYLimitEnable;

            virtual bool GetEdge0PointYLimitEnable() = 0;

            virtual void SetEdge0PointYLimitEnable(bool value) = 0;

            /// @~chinese 边缘点0Y范围，范围：[0,99999]   @~english Edge Point 0Y Range，range:[0,99999]
            __declspec(property(put = SetEdge0PointYLimitLow, get = GetEdge0PointYLimitLow)) double Edge0PointYLimitLow;

            virtual double GetEdge0PointYLimitLow() = 0;

            virtual void SetEdge0PointYLimitLow(double value) = 0;

            /// @~chinese 边缘点0Y范围，范围：[0,99999]   @~english Edge Point 0Y Range，range:[0,99999]
            __declspec(property(put = SetEdge0PointYLimitHigh, get = GetEdge0PointYLimitHigh)) double Edge0PointYLimitHigh;

            virtual double GetEdge0PointYLimitHigh() = 0;

            virtual void SetEdge0PointYLimitHigh(double value) = 0;

            /// @~chinese 边缘点1X判断   @~english Edge Point 1X Check
            __declspec(property(put = SetEdge1PointXLimitEnable, get = GetEdge1PointXLimitEnable)) bool Edge1PointXLimitEnable;

            virtual bool GetEdge1PointXLimitEnable() = 0;

            virtual void SetEdge1PointXLimitEnable(bool value) = 0;

            /// @~chinese 边缘点1X范围，范围：[0,99999]   @~english Edge Point 1X Range，range:[0,99999]
            __declspec(property(put = SetEdge1PointXLimitLow, get = GetEdge1PointXLimitLow)) double Edge1PointXLimitLow;

            virtual double GetEdge1PointXLimitLow() = 0;

            virtual void SetEdge1PointXLimitLow(double value) = 0;

            /// @~chinese 边缘点1X范围，范围：[0,99999]   @~english Edge Point 1X Range，range:[0,99999]
            __declspec(property(put = SetEdge1PointXLimitHigh, get = GetEdge1PointXLimitHigh)) double Edge1PointXLimitHigh;

            virtual double GetEdge1PointXLimitHigh() = 0;

            virtual void SetEdge1PointXLimitHigh(double value) = 0;

            /// @~chinese 边缘点1Y判断   @~english Edge Point 1Y Check
            __declspec(property(put = SetEdge1PointYLimitEnable, get = GetEdge1PointYLimitEnable)) bool Edge1PointYLimitEnable;

            virtual bool GetEdge1PointYLimitEnable() = 0;

            virtual void SetEdge1PointYLimitEnable(bool value) = 0;

            /// @~chinese 边缘点1Y范围，范围：[0,99999]   @~english Edge Point 1Y Range，range:[0,99999]
            __declspec(property(put = SetEdge1PointYLimitLow, get = GetEdge1PointYLimitLow)) double Edge1PointYLimitLow;

            virtual double GetEdge1PointYLimitLow() = 0;

            virtual void SetEdge1PointYLimitLow(double value) = 0;

            /// @~chinese 边缘点1Y范围，范围：[0,99999]   @~english Edge Point 1Y Range，range:[0,99999]
            __declspec(property(put = SetEdge1PointYLimitHigh, get = GetEdge1PointYLimitHigh)) double Edge1PointYLimitHigh;

            virtual double GetEdge1PointYLimitHigh() = 0;

            virtual void SetEdge1PointYLimitHigh(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /** @enum  MVD_EDGEPOINT_STATUS
        *  @brief Type of the polarity of the edge point
        */
		/// @~chinese 边缘点极性的类型 @~english Type of the polarity of the edge point
        typedef _MVD_EDGE_POLARITY__ MVD_EDGE_POLARITY;
        
        /*  Note: Interface Classes for describing single edge information.  */
        /// 间距检测边缘信息
        class IEdgeWidthSingleEdgeInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IEdgeWidthSingleEdgeInfo() {}
            virtual ~IEdgeWidthSingleEdgeInfo() {}

        public:
            /**
             * @brief 边缘点信息
             */
            virtual MVD_POINT_F GetEdgePoint() = 0;
            /**
             * @brief 边缘点偏移
             */
            virtual float GetBias() = 0;
            /**
             * @brief 边缘极性
             */
            virtual MVD_EDGE_POLARITY GetPolarity() = 0;
        };

        /*  Note: Interface Classes for describing a pair of edges information.  */
        /// 间距检测边缘对信息
        class IEdgeWidthEdgePairInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IEdgeWidthEdgePairInfo() {}
            virtual ~IEdgeWidthEdgePairInfo() {}

        public:
            /**
             * @brief 边缘0的基本信息
             */
            virtual IEdgeWidthSingleEdgeInfo* GetEdge0Info() = 0;
            /**
             * @brief 边缘1的基本信息
             */
            virtual IEdgeWidthSingleEdgeInfo* GetEdge1Info() = 0;
            /**
             * @brief 边缘间距
             */
            virtual float GetDistance() = 0;
            /**
             * @brief 分数
             */
            virtual float GetScore() = 0;
            /**
             * @brief 直线0
             */
            virtual Line GetLine0() = 0;
            /**
             * @brief 直线1
             */
            virtual Line GetLine1() = 0;
            /**
            * @brief 圆弧0
            */
            virtual Annulus GetAnnulus0() = 0;
            /**
            * @brief 圆弧1
            */
            virtual Annulus GetAnnulus1() = 0;
			/**
			* @brief 边缘状态
			*/
			virtual int GetEdgeStatus() = 0;
        };

        /*  Note: Interface Classes of the result of the EdgeWidthFindResults.  */
        ///间距检测结果类
        class EdgeWidthFindResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit EdgeWidthFindResults() {}
            virtual ~EdgeWidthFindResults() {}

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
			 * @brief ROI圆弧
			 */
			 virtual Annulus GetROIAnnulus() = 0;
            /**
             * @brief 边缘信息
             */
            virtual IEdgeWidthEdgePairInfo* GetEdgePairInfo( int nPtIndex ) = 0;
            /**
             * @brief 边缘点数量
             */
            virtual int GetEdgePairNum() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the EdgeWidthFind tool.  */
        ///间距检测工具类
        class IMVSEdgeWidthFindModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSEdgeWidthFindModuTool() {}
            virtual ~IMVSEdgeWidthFindModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual EdgeWidthFindParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual EdgeWidthFindResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSEdgeWidthFindModuTool(IMVSEdgeWidthFindModuTool&);
            IMVSEdgeWidthFindModuTool& operator=(IMVSEdgeWidthFindModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for EdgeWidthFind.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSEdgeWidthFindModu::IMVSEdgeWidthFindModuTool * __stdcall GetEdgeWidthFindToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_EDGEWIDTHFIND_H_
