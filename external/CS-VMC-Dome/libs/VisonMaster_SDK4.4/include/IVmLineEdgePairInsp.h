/***************************************************************************************************
 * File：IVmLineEdgePairInsp.h
 * Note：@~chinese 直线对缺陷检测模块接口声明 @~english Interface for the LineEdgePairInsp tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_LINEEDGEPAIRINSP_H_
#define _IVM_LINEEDGEPAIRINSP_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSLineEdgePairInspModu
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

    /// \addtogroup 直线对缺陷检测
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///直线对缺陷检测ROI管理类
        class LineEdgePairInspRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoisRectangle(RectBox* pRoiValue, unsigned int nValueNum) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the LineEdgePairInspParams.  */
        ///直线对缺陷检测参数类
        class LineEdgePairInspParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit LineEdgePairInspParams() {}
            virtual ~LineEdgePairInspParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入直线1(仅当次执行起效)   @~english InputLine1(Only valid for this execution)
            virtual void SetInputLine1(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入直线2(仅当次执行起效)   @~english InputLine2(Only valid for this execution)
            virtual void SetInputLine2(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 标准输入   @~english Standard Input
            __declspec(property(put = SetStandardInput, get = GetStandardInput)) bool StandardInput;

            virtual bool GetStandardInput() = 0;

            virtual void SetStandardInput(bool value) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual LineEdgePairInspRoiManager* GetModuRoiManager() = 0;

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

            /// @~chinese 容忍角度，范围：[0,180]   @~english Angle Tolerance，range:[0,180]
            __declspec(property(put = SetAngleTol, get = GetAngleTol)) int AngleTol;

            virtual int GetAngleTol() = 0;

            virtual void SetAngleTol(int value) = 0;

            /// @~chinese 卡尺数量，范围：[2,1000]   @~english Caliper Number，range:[2,1000]
            __declspec(property(put = SetLineCaliperNum, get = GetLineCaliperNum)) int LineCaliperNum;

            virtual int GetLineCaliperNum() = 0;

            virtual void SetLineCaliperNum(int value) = 0;

            /// @~chinese 剔除点数，范围：[0,998]   @~english Reject Number，range:[0,998]
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
        /* @enum  MVD_EDGEINSP_FLAW_DEFECT_TYPE
         *  @brief 边缘对缺陷类型
         */
        /// @~chinese 边缘缺陷类型   @~english Edge Insp Flaw Defect Type
        typedef _MVD_EDGEINSP_FLAW_DEFECT_TYPE__ MVD_EDGEINSP_FLAW_DEFECT_TYPE;
        
        ///直线对缺陷检测缺陷信息类
        class ILineEdgePairFlawInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit ILineEdgePairFlawInfo() {}
            virtual ~ILineEdgePairFlawInfo() {}

        public:
            /**
             * @brief 缺陷包围框
             */
            virtual RectBox GetFlawBox() = 0;
            /**
             * @brief 缺陷长度
             */
            virtual float GetFlawLength() = 0;
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
            /**
             * @brief 缺陷类型
             */
            virtual MVD_EDGEINSP_FLAW_DEFECT_TYPE GetFlawType() = 0;
        };

        /*  Note: Interface Classes of the result of the LineEdgePairInspResults.  */
        ///直线对缺陷检测结果类
        class LineEdgePairInspResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit LineEdgePairInspResults() {}
            virtual ~LineEdgePairInspResults() {}

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
             * @brief 缺陷数量
             */
            virtual int GetFlawNum() = 0;
            /**
             * @brief 根据索引获取缺陷的信息
             * @param nIndex [IN] 缺陷的索引,[0, FlawNum)
             * @return 单缺陷信息
             */
            virtual ILineEdgePairFlawInfo* GetFlawInfo(int nIndex) = 0;
            /**
             * @brief 获取边缘0点的数量
             * @return 边缘点个数
             */
            virtual int GetEdge0PointNum() = 0;
            /**
             * @brief 根据索引查询边缘点0
             * @param nIndex   [IN]  边缘点0的索引，[0, EdgePointNum)
             * @param stPoint0 [OUT] 边缘点0
             * @param nStatus  [OUT] 边缘点0状态,状态1为正常，0为缺陷
             */
            virtual void GetEdge0Point(int nIndex, MVD_POINT_F &stPoint0, int &nStatus) = 0;
            /**
             * @brief 获取边缘1点的数量
             * @return 边缘点个数
             */
            virtual int GetEdge1PointNum() = 0;
            /**
             * @brief 根据索引查询边缘点1
             * @param nIndex   [IN]  边缘点1的索引，[0, EdgePointNum)
             * @param stPoint1 [OUT] 边缘点1
             * @param nStatus  [OUT] 边缘点1状态,状态1为正常，0为缺陷
             */
            virtual void GetEdge1Point(int nIndex, MVD_POINT_F &stPoint1, int &nStatus) = 0;
            /**
             * @brief 获取卡尺数量
             * @return 卡尺数量
             */
            virtual int GetCaliperNum() = 0;
            /**
             * @brief 获取卡尺点信息  个数为当前卡尺个数
             * @param nIndex   [IN]   卡尺点序号，[0, CaliperNum)
             * @param stPoint  [OUT]  卡尺点
             * @param nStatus  [OUT]  卡尺点状态,状态1为正常，0为缺陷
             */
            virtual void GetCaliperIdeaPoint(int nIndex, MVD_POINT_F &stPoint, int &nStatus) = 0;
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
            /**
             * @brief 获取内部拟合直线个数
             * @return 直线个数
             */
            virtual int GetLineNum() = 0;
            /**
             * @brief 根据索引获取内部拟合直线
             * @param nIndex [IN] 直线索引,[0, LineNum)
             * @return 内部拟合输出的直线
             */
            virtual Line GetInnerFitLine(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the LineEdgePairInsp tool.  */
        ///直线对缺陷检测工具类
        class IMVSLineEdgePairInspModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSLineEdgePairInspModuTool() {}
            virtual ~IMVSLineEdgePairInspModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual LineEdgePairInspParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual LineEdgePairInspResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSLineEdgePairInspModuTool(IMVSLineEdgePairInspModuTool&);
            IMVSLineEdgePairInspModuTool& operator=(IMVSLineEdgePairInspModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for LineEdgePairInsp.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSLineEdgePairInspModu::IMVSLineEdgePairInspModuTool * __stdcall GetLineEdgePairInspToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_LINEEDGEPAIRINSP_H_
