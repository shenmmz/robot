/***************************************************************************************************
 * File：IVmCircleEdgePairInsp.h
 * Note：@~chinese 圆弧对缺陷检测模块接口声明 @~english Interface for the CircleEdgePairInsp tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CIRCLEEDGEPAIRINSP_H_
#define _IVM_CIRCLEEDGEPAIRINSP_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCircleEdgePairInspModu
    {
        /// @~chinese 边缘查找类型   @~english Edge Search Type
        typedef enum _FindModeEnum
        {
            /// @~chinese 最宽边缘对   @~english Widest edge pair
            FindMode_Widest = 0x1,

            /// @~chinese 最接近边缘对   @~english Nearest edge pair
            FindMode_Nearest = 0x2,

            /// @~chinese 最强最接近边缘对   @~english Strongest & Nearest Edge Pair
            FindMode_StrongestNearest = 0x3,

        }FindModeEnum;

        /// @~chinese 边缘0极性   @~english Edge 0 polarity
        typedef enum _Edge0PolarityEnum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            Edge0Polarity_B2W = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            Edge0Polarity_W2B = 0x2,

            /// @~chinese 任意极性   @~english Both
            Edge0Polarity_All = 0x3,

        }Edge0PolarityEnum;

        /// @~chinese 边缘1极性   @~english Edge 1 polarity
        typedef enum _Edge1PolarityEnum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            Edge1Polarity_B2W = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            Edge1Polarity_W2B = 0x2,

            /// @~chinese 任意极性   @~english Both
            Edge1Polarity_All = 0x3,

        }Edge1PolarityEnum;

    /// \addtogroup 圆弧对缺陷检测
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///圆弧对缺陷检测ROI管理类
        class CircleEdgePairInspRoiManager
        {
        public:

            /// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
            virtual void SetRoiAnnulus(Annulus stRoiValue) = 0;

            /// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
            virtual void SetRoisAnnulus(Annulus* pRoiValue, unsigned int nValueNum) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the CircleEdgePairInspParams.  */
        ///圆弧对缺陷检测参数类
        class CircleEdgePairInspParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CircleEdgePairInspParams() {}
            virtual ~CircleEdgePairInspParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入圆弧1(仅当次执行起效)   @~english InputAnnulus1(Only valid for this execution)
            virtual void SetInputAnnulus1(Annulus* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入圆弧2(仅当次执行起效)   @~english InputAnnulus2(Only valid for this execution)
            virtual void SetInputAnnulus2(Annulus* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 标准输入   @~english Standard Input
            __declspec(property(put = SetStandardInput, get = GetStandardInput)) bool StandardInput;

            virtual bool GetStandardInput() = 0;

            virtual void SetStandardInput(bool value) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual CircleEdgePairInspRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 边缘查找类型   @~english Edge Search Type
            __declspec(property(put = SetFindMode, get = GetFindMode)) FindModeEnum FindMode;

            virtual FindModeEnum GetFindMode() = 0;

            virtual void SetFindMode(FindModeEnum value) = 0;

            /// @~chinese 边缘0极性   @~english Edge 0 polarity
            __declspec(property(put = SetEdge0Polarity, get = GetEdge0Polarity)) Edge0PolarityEnum Edge0Polarity;

            virtual Edge0PolarityEnum GetEdge0Polarity() = 0;

            virtual void SetEdge0Polarity(Edge0PolarityEnum value) = 0;

            /// @~chinese 边缘1极性   @~english Edge 1 polarity
            __declspec(property(put = SetEdge1Polarity, get = GetEdge1Polarity)) Edge1PolarityEnum Edge1Polarity;

            virtual Edge1PolarityEnum GetEdge1Polarity() = 0;

            virtual void SetEdge1Polarity(Edge1PolarityEnum value) = 0;

            /// @~chinese 边缘阈值，范围：[1,255]   @~english Edge Threshold，range:[1,255]
            __declspec(property(put = SetEdgeStrength, get = GetEdgeStrength)) int EdgeStrength;

            virtual int GetEdgeStrength() = 0;

            virtual void SetEdgeStrength(int value) = 0;

            /// @~chinese 卡尺宽度，范围：[1,500]   @~english CaliperWidth，range:[1,500]
            __declspec(property(put = SetCaliperWidth, get = GetCaliperWidth)) int CaliperWidth;

            virtual int GetCaliperWidth() = 0;

            virtual void SetCaliperWidth(int value) = 0;

            /// @~chinese 宽度合格范围，范围：[0,30000]   @~english Width Eligibility Range，range:[0,30000]
            __declspec(property(put = SetRoughMinDis, get = GetRoughMinDis)) int RoughMinDis;

            virtual int GetRoughMinDis() = 0;

            virtual void SetRoughMinDis(int value) = 0;

            /// @~chinese 宽度合格范围，范围：[0,30000]   @~english Width Eligibility Range，range:[0,30000]
            __declspec(property(put = SetRoughMaxDis, get = GetRoughMaxDis)) int RoughMaxDis;

            virtual int GetRoughMaxDis() = 0;

            virtual void SetRoughMaxDis(int value) = 0;

            /// @~chinese 缺陷尺寸使能   @~english Defect Size Enable
            __declspec(property(put = SetSizeEnable, get = GetSizeEnable)) bool SizeEnable;

            virtual bool GetSizeEnable() = 0;

            virtual void SetSizeEnable(bool value) = 0;

            /// @~chinese 缺陷尺寸阈值，范围：[0,100000]   @~english Rough Min Size，range:[0,100000]
            __declspec(property(put = SetRoughMinSize, get = GetRoughMinSize)) int RoughMinSize;

            virtual int GetRoughMinSize() = 0;

            virtual void SetRoughMinSize(int value) = 0;

            /// @~chinese 缺陷面积使能   @~english Defect Area Enable
            __declspec(property(put = SetAreaEnable, get = GetAreaEnable)) bool AreaEnable;

            virtual bool GetAreaEnable() = 0;

            virtual void SetAreaEnable(bool value) = 0;

            /// @~chinese 缺陷面积阈值，范围：[0,1000000]   @~english Rough Min Area，range:[0,1000000]
            __declspec(property(put = SetRoughMinArea, get = GetRoughMinArea)) int RoughMinArea;

            virtual int GetRoughMinArea() = 0;

            virtual void SetRoughMinArea(int value) = 0;

            /// @~chinese 卡尺高度，范围：[1,1500]   @~english CaliperHeight，range:[1,1500]
            __declspec(property(put = SetCaliperHeight, get = GetCaliperHeight)) int CaliperHeight;

            virtual int GetCaliperHeight() = 0;

            virtual void SetCaliperHeight(int value) = 0;

            /// @~chinese 理想宽度，范围：[1,20000]   @~english Ideal Width，range:[1,20000]
            __declspec(property(put = SetIdeaWidth, get = GetIdeaWidth)) int IdeaWidth;

            virtual int GetIdeaWidth() = 0;

            virtual void SetIdeaWidth(int value) = 0;

            /// @~chinese 滤波尺寸，范围：[1,50]   @~english Filter Size，range:[1,50]
            __declspec(property(put = SetHalfKernelSize, get = GetHalfKernelSize)) int HalfKernelSize;

            virtual int GetHalfKernelSize() = 0;

            virtual void SetHalfKernelSize(int value) = 0;

            /// @~chinese 卡尺间距，范围：[1,1000]   @~english Caliper Spacing，range:[1,1000]
            __declspec(property(put = SetCaliperDistTraj, get = GetCaliperDistTraj)) int CaliperDistTraj;

            virtual int GetCaliperDistTraj() = 0;

            virtual void SetCaliperDistTraj(int value) = 0;

            /// @~chinese 同心度容忍，范围：[0,1000]   @~english Angle Tolerance，range:[0,1000]
            __declspec(property(put = SetFitConcentricTol, get = GetFitConcentricTol)) int FitConcentricTol;

            virtual int GetFitConcentricTol() = 0;

            virtual void SetFitConcentricTol(int value) = 0;

            /// @~chinese 卡尺数量，范围：[3,1000]   @~english Caliper Number，range:[3,1000]
            __declspec(property(put = SetFitCaliperNum, get = GetFitCaliperNum)) int FitCaliperNum;

            virtual int GetFitCaliperNum() = 0;

            virtual void SetFitCaliperNum(int value) = 0;

            /// @~chinese 剔除点数，范围：[0,997]   @~english Reject Number，range:[0,997]
            __declspec(property(put = SetFitRejectNum, get = GetFitRejectNum)) int FitRejectNum;

            virtual int GetFitRejectNum() = 0;

            virtual void SetFitRejectNum(int value) = 0;

            /// @~chinese 剔除阈值，范围：[1,15000]   @~english Threshold to Remove，range:[1,15000]
            __declspec(property(put = SetFitRejectDist, get = GetFitRejectDist)) int FitRejectDist;

            virtual int GetFitRejectDist() = 0;

            virtual void SetFitRejectDist(int value) = 0;

            /// @~chinese 追踪容忍度，范围：[0,100]   @~english Track Tolerance，range:[0,100]
            __declspec(property(put = SetTrackDistTol, get = GetTrackDistTol)) int TrackDistTol;

            virtual int GetTrackDistTol() = 0;

            virtual void SetTrackDistTol(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /** @enum  MVD_EDGEINSP_FLAW_DEFECT_TYPE
        *  @brief defect type of the Arc pair inspect
        */
		/// @~chinese 边缘缺陷类型   @~english Edge flaw defect type
        typedef _MVD_EDGEINSP_FLAW_DEFECT_TYPE__ MVD_EDGEINSP_FLAW_DEFECT_TYPE;
        
        class IArcPairInspFlawInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IArcPairInspFlawInfo(){}
            virtual ~IArcPairInspFlawInfo(){}

        public:
            /**
             * @brief 缺陷类型
             */

            virtual MVD_EDGEINSP_FLAW_DEFECT_TYPE GetFlawType() = 0;
            /**
             * @brief 缺陷包围框
             */

            virtual RectBox GetFlawBox() = 0;
            /**
             * @brief 缺陷长度
             */

            virtual float GetFlawSize() = 0;
            /**
             * @brief 缺陷面积
             */

            virtual float GetFlawArea() = 0;
            /**
             * @brief 缺陷所在的卡尺的起始索引
             */

            virtual int GetCaliperStartIndex() = 0;
            /**
             * @brief 缺陷所在的卡尺的结束索引
             */

            virtual int GetCaliperEndIndex() = 0;
        };


        ///圆弧对缺陷检测结果类
        class CircleEdgePairInspResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CircleEdgePairInspResults() {}
            virtual ~CircleEdgePairInspResults() {}

        public:
             /**
             * @brief ROI理想卡尺点数量
             * @param nIndex[IN] 数据索引
             */
             virtual int GetCalipIdeaPointNums(int nIndex) = 0;

             /**
             * @brief ROI边缘1轮廓点数
             * @param nIndex[IN] 数据索引
             */
             virtual int GetEdge1PointNums(int nIndex) = 0;

             /**
             * @brief ROI边缘0轮廓点数
             * @param nIndex[IN] 数据索引
             */
             virtual int GetEdge0PointNums(int nIndex) = 0;

             /**
             * @brief 缺陷框索引
             * @param nIndex[IN] 数据索引
             */
             virtual int GetFlawRectIndex(int nIndex) = 0;

             /**
             * @brief ROI缺陷个数
             * @param nIndex[IN] 数据索引
             */
             virtual int GetFlawNums(int nIndex) = 0;
			 /**
             * @brief 最小宽度
			 * @param nIndex[IN] 数据索引
             */
			 virtual float GetMinWidths(int nIndex) = 0;
            /**
             * @brief 最大宽度
			 * @param nIndex[IN] 数据索引
             */
            virtual float GetMaxWidths(int nIndex) = 0;
            /**
             * @brief 平均宽度
			 * @param nIndex[IN] 数据索引
             */
            virtual float GetAverageWidths(int nIndex) = 0;
            /**
             * @brief 近似面积
			 * @param nIndex[IN] 数据索引
             */
            virtual float GetApproximateAreas(int nIndex) = 0;
            /**
            * @brief 最小边缘点0
			* @param nIndex[IN] 数据索引
            */
            virtual PointF GetMinEdgePoint0s(int nIndex) = 0;
            /**
            * @brief 最小边缘点0极性
			* @param nIndex[IN] 数据索引
            */
            virtual int GetMinEdgePolarity0s(int nIndex) = 0;
            /**
            * @brief 最小边缘点1
			* @param nIndex[IN] 数据索引
            */
            virtual PointF GetMinEdgePoint1s(int nIndex) = 0;
            /**
            * @brief 最小边缘点1极性
			* @param nIndex[IN] 数据索引
            */
            virtual int GetMinEdgePolarity1s(int nIndex) = 0;
            /**
            * @brief 最小边缘分数
			* @param nIndex[IN] 数据索引
            */
            virtual float GetMinEdgeScores(int nIndex) = 0;
            /**
            * @brief 最小边缘距离
			* @param nIndex[IN] 数据索引
            */
            virtual float GetMinEdgeDistances(int nIndex) = 0;
            /**
            * @brief 最小边缘状态
			* @param nIndex[IN] 数据索引
            */
            virtual int GetMinEdgeStatuses(int nIndex) = 0;
            /**
            * @brief 最大边缘点0
			* @param nIndex[IN] 数据索引
            */
            virtual PointF GetMaxEdgePoint0s(int nIndex) = 0;
            /**
            * @brief 最大边缘点0极性
			* @param nIndex[IN] 数据索引
            */
            virtual int GetMaxEdgePolarity0s(int nIndex) = 0;
            /**
            * @brief 最大边缘点1
			* @param nIndex[IN] 数据索引
            */
            virtual PointF GetMaxEdgePoint1s(int nIndex) = 0;
            /**
            * @brief 最大边缘点1极性
			* @param nIndex[IN] 数据索引
            */
            virtual int GetMaxEdgePolarity1s(int nIndex) = 0;
            /**
            * @brief 最大边缘分数
			* @param nIndex[IN] 数据索引
            */
            virtual float GetMaxEdgeScores(int nIndex) = 0;
            /**
            * @brief 最大边缘距离
			* @param nIndex[IN] 数据索引
            */
            virtual float GetMaxEdgeDistances(int nIndex) = 0;
            /**
            * @brief 最大边缘状态
			* @param nIndex[IN] 数据索引
            */
            virtual int GetMaxEdgeStatuses(int nIndex) = 0;

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
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
             /**
             * @brief 检测区域
             * @param nIndex[IN] 数据索引
             */
             virtual RectBox GetDetectROIs(int nIndex) = 0;
             /**
             * @brief 检测区域
             * @deprecated V4.4废弃
             */
             virtual RectBox GetROI() = 0;
             /**
             * @brief ROI圆弧
             */
             virtual Annulus GetDetectROIAnnulus(int nIndex) = 0;
             /**
             * @brief ROI圆弧
             * @deprecated V4.4废弃
             */
             virtual Annulus GetROIAnnulus() = 0;
            /**
             * @brief 获取边缘点个数 nPts01：边缘点0或者1
             */

            virtual int GetEdgePtsNum(int nPts01) = 0;
            /**
             * @brief 根据索引查询边缘点0
             * @param nIndex   [IN]  边缘点0的索引，[0, EdgePointNum)
             * @param stPoint0 [OUT] 边缘点0
             * @param nStatus  [OUT] 边缘点0状态,状态1为正常，0为缺陷
             */
            virtual void GetEdge0Point(int nIndex, MVD_POINT_F &stPoint0, int &nStatus) = 0;
            /**
            * @brief 根据索引查询边缘点1
            * @param nIndex   [IN]  边缘点1的索引，[0, EdgePointNum)
            * @param stPoint1 [OUT] 边缘点1
            * @param nStatus  [OUT] 边缘点1状态,状态1为正常，0为缺陷
            */
            virtual void GetEdge1Point(int nIndex, MVD_POINT_F &stPoint1, int &nStatus) = 0;
            /**
             * @brief 获取卡尺个数
             */

            virtual int GetIdealPointNum() = 0;
            /**
            * @brief 获取卡尺点信息  个数为当前卡尺个数
            * @param nIdealPtsIndex   [IN]   卡尺点序号，[0, CaliperNum)
            * @param stPoint          [OUT]  卡尺点
            * @param nStatus          [OUT]  卡尺点状态,状态1为正常，0为缺陷
            */
            virtual void GetIdealPoint(int nIdealPtsIndex, MVD_POINT_F &stPoint, int &nStatus) = 0;
            /**
             * @brief 获取拟合圆弧信息 圆弧等价于内外径相等的扇环 nArcIndex：圆弧序号
             */

            virtual Circle GetArc(int nArcIndex) = 0;
            /**
             * @brief 获取拟合圆弧数目
             */

            virtual int GetArcNum() = 0;
            /**
             * @brief 获取缺陷信息
             */
            virtual IArcPairInspFlawInfo * GetDefectInfo(int nIndex) = 0;
            /**
             * @brief 缺陷个数
             * @deprecated V4.4废弃
             */
            virtual int GetFlawNum() = 0;
            /**
             * @brief 最小宽度
             * @deprecated V4.4废弃
             */
            virtual float GetMinWidth() = 0;
            /**
             * @brief 最大宽度
             * @deprecated V4.4废弃
             */
            virtual float GetMaxWidth() = 0;
            /**
             * @brief 平均宽度
             * @deprecated V4.4废弃
             */
            virtual float GetAverageWidth() = 0;
            /**
             * @brief 近似面积
			 * @deprecated V4.4废弃
             */
            virtual float GetApproximateArea() = 0;
            /**
            * @brief 最小边缘点0
			* @deprecated V4.4废弃
            */
            virtual PointF GetMinEdgePoint0() = 0;
            /**
            * @brief 最小边缘点0极性
			* @deprecated V4.4废弃
            */
            virtual int GetMinEdgePolarity0() = 0;
            /**
            * @brief 最小边缘点1
			* @deprecated V4.4废弃
            */
            virtual PointF GetMinEdgePoint1() = 0;
            /**
            * @brief 最小边缘点1极性
			* @deprecated V4.4废弃
            */
            virtual int GetMinEdgePolarity1() = 0;
            /**
            * @brief 最小边缘分数
			* @deprecated V4.4废弃
            */
            virtual float GetMinEdgeScore() = 0;
            /**
            * @brief 最小边缘距离
			* @deprecated V4.4废弃
            */
            virtual float GetMinEdgeDistance() = 0;
            /**
            * @brief 最小边缘状态
			* @deprecated V4.4废弃
            */
            virtual int GetMinEdgeStatus() = 0;
            /**
            * @brief 最大边缘点0
			* @deprecated V4.4废弃
            */
            virtual PointF GetMaxEdgePoint0() = 0;
            /**
            * @brief 最大边缘点0极性
			* @deprecated V4.4废弃
            */
            virtual int GetMaxEdgePolarity0() = 0;
            /**
            * @brief 最大边缘点1
			* @deprecated V4.4废弃
            */
            virtual PointF GetMaxEdgePoint1() = 0;
            /**
            * @brief 最大边缘点1极性
			* @deprecated V4.4废弃
            */
            virtual int GetMaxEdgePolarity1() = 0;
            /**
            * @brief 最大边缘分数
			* @deprecated V4.4废弃
            */
            virtual float GetMaxEdgeScore() = 0;
            /**
            * @brief 最大边缘距离
			* @deprecated V4.4废弃
            */
            virtual float GetMaxEdgeDistance() = 0;
            /**
            * @brief 最大边缘状态
			* @deprecated V4.4废弃
            */
            virtual int GetMaxEdgeStatus() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CircleEdgePairInsp tool.  */
        ///圆弧对缺陷检测工具类
        class IMVSCircleEdgePairInspModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCircleEdgePairInspModuTool() {}
            virtual ~IMVSCircleEdgePairInspModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CircleEdgePairInspParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CircleEdgePairInspResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSCircleEdgePairInspModuTool(IMVSCircleEdgePairInspModuTool&);
            IMVSCircleEdgePairInspModuTool& operator=(IMVSCircleEdgePairInspModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CircleEdgePairInsp.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSCircleEdgePairInspModu::IMVSCircleEdgePairInspModuTool * __stdcall GetCircleEdgePairInspToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CIRCLEEDGEPAIRINSP_H_
