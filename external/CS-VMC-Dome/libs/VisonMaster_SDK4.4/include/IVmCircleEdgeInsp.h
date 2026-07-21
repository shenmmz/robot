/***************************************************************************************************
 * File：IVmCircleEdgeInsp.h
 * Note：@~chinese 圆弧边缘缺陷检测模块接口声明 @~english Interface for the CircleEdgeInsp tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CIRCLEEDGEINSP_H_
#define _IVM_CIRCLEEDGEINSP_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCircleEdgeInspModu
    {
        /// @~chinese 边缘类型   @~english Edge Type
        typedef enum _FindModeEnum
        {
            /// @~chinese 最强   @~english Strongest Edge
            FindMode_Best = 0x1,

            /// @~chinese 第一条   @~english First Edge
            FindMode_First = 0x2,

            /// @~chinese 最后一条   @~english Last Edge
            FindMode_Last = 0x3,

        }FindModeEnum;

        /// @~chinese 边缘极性   @~english EdgePolarity
        typedef enum _EdgePolarityEnum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            EdgePolarity_B2W = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            EdgePolarity_W2B = 0x2,

            /// @~chinese 任意极性   @~english Both
            EdgePolarity_All = 0x3,

        }EdgePolarityEnum;

        /// @~chinese 缺陷极性   @~english Defect Polarity
        typedef enum _FlawPolarityEnableEnum
        {
            /// @~chinese 轨迹两侧缺陷   @~english Trajectory Both Defect
            FlawPolarityEnable_All = 0x0,

            /// @~chinese 轨迹右侧缺陷   @~english Trajectory Right Defect
            FlawPolarityEnable_RightBottom = 0x1,

            /// @~chinese 轨迹左侧缺陷   @~english Trajectory Left Defect
            FlawPolarityEnable_LeftTop = 0x2,

        }FlawPolarityEnableEnum;

    /// \addtogroup 圆弧边缘缺陷检测
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///圆弧边缘缺陷检测ROI管理类
        class CircleEdgeInspRoiManager
        {
        public:

            /// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
            virtual void SetRoiAnnulus(Annulus stRoiValue) = 0;

            /// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
            virtual void SetRoisAnnulus(Annulus* pRoiValue, unsigned int nValueNum) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the CircleEdgeInspParams.  */
        ///圆弧边缘缺陷检测参数类
        class CircleEdgeInspParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CircleEdgeInspParams() {}
            virtual ~CircleEdgeInspParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入圆弧(仅当次执行起效)   @~english InputAnnulus(Only valid for this execution)
            virtual void SetInputAnnulus(Annulus* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 标准输入   @~english Standard Input
            __declspec(property(put = SetStandardInput, get = GetStandardInput)) bool StandardInput;

            virtual bool GetStandardInput() = 0;

            virtual void SetStandardInput(bool value) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual CircleEdgeInspRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 边缘类型   @~english Edge Type
            __declspec(property(put = SetFindMode, get = GetFindMode)) FindModeEnum FindMode;

            virtual FindModeEnum GetFindMode() = 0;

            virtual void SetFindMode(FindModeEnum value) = 0;

            /// @~chinese 边缘极性   @~english EdgePolarity
            __declspec(property(put = SetEdgePolarity, get = GetEdgePolarity)) EdgePolarityEnum EdgePolarity;

            virtual EdgePolarityEnum GetEdgePolarity() = 0;

            virtual void SetEdgePolarity(EdgePolarityEnum value) = 0;

            /// @~chinese 滤波尺寸，范围：[1,50]   @~english Filter Size，range:[1,50]
            __declspec(property(put = SetHalfKernelSize, get = GetHalfKernelSize)) int HalfKernelSize;

            virtual int GetHalfKernelSize() = 0;

            virtual void SetHalfKernelSize(int value) = 0;

            /// @~chinese 边缘阈值，范围：[1,255]   @~english Edge Threshold，range:[1,255]
            __declspec(property(put = SetEdgeStrength, get = GetEdgeStrength)) int EdgeStrength;

            virtual int GetEdgeStrength() = 0;

            virtual void SetEdgeStrength(int value) = 0;

            /// @~chinese 卡尺高度，范围：[1,1500]   @~english CaliperHeight，range:[1,1500]
            __declspec(property(put = SetCaliperHeight, get = GetCaliperHeight)) int CaliperHeight;

            virtual int GetCaliperHeight() = 0;

            virtual void SetCaliperHeight(int value) = 0;

            /// @~chinese 卡尺宽度，范围：[1,500]   @~english CaliperWidth，range:[1,500]
            __declspec(property(put = SetCaliperWidth, get = GetCaliperWidth)) int CaliperWidth;

            virtual int GetCaliperWidth() = 0;

            virtual void SetCaliperWidth(int value) = 0;

            /// @~chinese 卡尺间距，范围：[1,1000]   @~english Caliper Spacing，range:[1,1000]
            __declspec(property(put = SetCaliperDistTraj, get = GetCaliperDistTraj)) int CaliperDistTraj;

            virtual int GetCaliperDistTraj() = 0;

            virtual void SetCaliperDistTraj(int value) = 0;

            /// @~chinese 缺陷极性   @~english Defect Polarity
            __declspec(property(put = SetFlawPolarityEnable, get = GetFlawPolarityEnable)) FlawPolarityEnableEnum FlawPolarityEnable;

            virtual FlawPolarityEnableEnum GetFlawPolarityEnable() = 0;

            virtual void SetFlawPolarityEnable(FlawPolarityEnableEnum value) = 0;

            /// @~chinese 缺陷距离阈值，范围：[0,10000]   @~english Rough Min Distance，range:[0,10000]
            __declspec(property(put = SetRoughMinDis, get = GetRoughMinDis)) int RoughMinDis;

            virtual int GetRoughMinDis() = 0;

            virtual void SetRoughMinDis(int value) = 0;

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

            /// @~chinese 卡尺数量，范围：[3,1000]   @~english Caliper Number，range:[3,1000]
            __declspec(property(put = SetCircleCaliperNum, get = GetCircleCaliperNum)) int CircleCaliperNum;

            virtual int GetCircleCaliperNum() = 0;

            virtual void SetCircleCaliperNum(int value) = 0;

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
		
        ///圆弧边缘缺陷检测缺陷信息类
        class IArcFlawInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IArcFlawInfo(){}
            virtual ~IArcFlawInfo(){}

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

        /*  Note: Interface Classes of the result of the CircleEdgeInspResults.  */
        ///圆弧边缘缺陷检测结果类
        class CircleEdgeInspResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CircleEdgeInspResults() {}
            virtual ~CircleEdgeInspResults() {}

        public:
             /**
             * @brief ROI理想卡尺点数量
             * @param nIndex[IN] 数据索引
             */
             virtual int GetCalipIdeaPointNums(int nIndex) = 0;

             /**
             * @brief ROI边缘点个数
             * @param nIndex[IN] 数据索引
             */
             virtual int GetEdgePointNums(int nIndex) = 0;

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
             * @brief 获取边缘点个数
             */

            virtual int GetEdgePtsNum() = 0;

            /**
             * @brief 根据索引查询边缘点
             * @param nIndex  [IN]  边缘点的索引，[0, EdgePointNum)
             * @param stPoint [OUT] 边缘点
             * @param nStatus [OUT] 边缘点状态,状态1为正常，0为缺陷
             */
            virtual void GetEdgePoint(int nIndex, MVD_POINT_F &stPoint, int &nStatus) = 0;

            /**
             * @brief 获取卡尺个数
             */

            virtual int GetIdealPointNum() = 0;

            /**
             * @brief 获取卡尺点信息，个数为当前卡尺个数
             * @param nIdealPtsIndex   [IN]   卡尺点序号，[0, CaliperNum)
             * @param stPoint          [OUT]  卡尺点
             * @param nStatus          [OUT]  卡尺点状态,状态1为正常，0为缺陷
             */
            virtual void GetIdealPoint(int nIdealPtsIndex, MVD_POINT_F &stPoint, int &nStatus) = 0;

            /**
             * @brief 获取拟合圆弧信息 圆弧等价于内外径相等的扇环  边检测为1条
             */

            virtual Circle GetArc() = 0;

            /**
             * @brief 拟合圆弧数目
             */

            virtual int GetArcNum() = 0;
            /**
             * @brief 获取缺陷信息
             */
            virtual IArcFlawInfo * GetFlawInfo(int nIndex) = 0;
            /**
             * @brief 缺陷个数
             */
            virtual int GetFlawNum() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CircleEdgeInsp tool.  */
        ///圆弧边缘缺陷检测工具类
        class IMVSCircleEdgeInspModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCircleEdgeInspModuTool() {}
            virtual ~IMVSCircleEdgeInspModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CircleEdgeInspParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CircleEdgeInspResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSCircleEdgeInspModuTool(IMVSCircleEdgeInspModuTool&);
            IMVSCircleEdgeInspModuTool& operator=(IMVSCircleEdgeInspModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CircleEdgeInsp.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSCircleEdgeInspModu::IMVSCircleEdgeInspModuTool * __stdcall GetCircleEdgeInspToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CIRCLEEDGEINSP_H_
