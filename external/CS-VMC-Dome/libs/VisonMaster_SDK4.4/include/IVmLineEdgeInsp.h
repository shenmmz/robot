/***************************************************************************************************
 * File：IVmLineEdgeInsp.h
 * Note：@~chinese 直线边缘缺陷检测模块接口声明 @~english Interface for the LineEdgeInsp tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_LINEEDGEINSP_H_
#define _IVM_LINEEDGEINSP_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSLineEdgeInspModu
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

    /// \addtogroup 直线边缘缺陷检测
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///直线边缘缺陷检测ROI管理类
        class LineEdgeInspRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoisRectangle(RectBox* pRoiValue, unsigned int nValueNum) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the LineEdgeInspParams.  */
        ///直线边缘缺陷检测参数类
        class LineEdgeInspParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit LineEdgeInspParams() {}
            virtual ~LineEdgeInspParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入直线(仅当次执行起效)   @~english Input Line(Only valid for this execution)
            virtual void SetInputLine(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 标准输入   @~english Standard Input
            __declspec(property(put = SetStandardInput, get = GetStandardInput)) bool StandardInput;

            virtual bool GetStandardInput() = 0;

            virtual void SetStandardInput(bool value) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual LineEdgeInspRoiManager* GetModuRoiManager() = 0;

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
         *  @brief 边缘缺陷类型
         */
        /// @~chinese 边缘缺陷类型   @~english Edge Insp Flaw Defect Type
        typedef _MVD_EDGEINSP_FLAW_DEFECT_TYPE__ MVD_EDGEINSP_FLAW_DEFECT_TYPE;
        
        ///直线边缘缺陷检测缺陷信息类
        class ILineEdgeFlawInfo
        {
        protected:
            // 构造与析构函数
            explicit ILineEdgeFlawInfo() {}
            virtual ~ILineEdgeFlawInfo() {}

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

        /*  Note: Interface Classes of the result of the LineEdgeInspResults.  */
        ///直线边缘缺陷检测结果类
        class LineEdgeInspResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit LineEdgeInspResults() {}
            virtual ~LineEdgeInspResults() {}

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
            // Property
            /**
             * @brief 缺陷数量
             */
            virtual int GetFlawNum() = 0;
            /**
             * @brief 根据索引获取缺陷的信息
             * @param nIndex [IN] 缺陷的索引,[0, FlawNum)
             * @return 单缺陷信息
             */
            virtual ILineEdgeFlawInfo* GetFlawInfo(int nIndex) = 0;
            /**
             * @brief 获取边缘点的数量
             * @return 边缘点个数
             */
            virtual int GetEdgePointNum() = 0;
            /**
             * @brief 根据索引查询边缘点
             * @param nIndex [IN] 边缘点的索引,[0, EdgePointNum)
             * @return 单边缘点
             */
            virtual MVD_POINT_F GetEdgePoint(int nIndex) = 0;
            /**
             * @brief 根据索引查询边缘点状态，0表示为正常状态 1表示为缺陷状态
             * @param nIndex [IN] 边缘点的索引,[0, EdgePointNum)
             * @return 单边缘点
             */
            virtual int GetEdgePointStatus(int nIndex) = 0;
            /**
            * @brief 根据索引获取卡尺信息
            * @param nIndex [IN] 卡尺索引,[0, Caliper)
            * @return 单卡尺信息
            */
            virtual RectBox GetCaliperBox(int nIndex) = 0;
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
             * @brief 获取内部拟合直线个数
             * @return 直线个数
             */
            virtual int GetLineNum() = 0;
            /**
             * @brief 根据索引获取内部拟合直线
             * @param nIndex [IN] 直线索引,[0, LineNum)
             * @return 内部拟合输出直线
             */
            virtual Line GetInnerFitLine(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the LineEdgeInsp tool.  */
        ///直线边缘缺陷检测工具类
        class IMVSLineEdgeInspModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSLineEdgeInspModuTool() {}
            virtual ~IMVSLineEdgeInspModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual LineEdgeInspParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual LineEdgeInspResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSLineEdgeInspModuTool(IMVSLineEdgeInspModuTool&);
            IMVSLineEdgeInspModuTool& operator=(IMVSLineEdgeInspModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for LineEdgeInsp.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSLineEdgeInspModu::IMVSLineEdgeInspModuTool * __stdcall GetLineEdgeInspToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_LINEEDGEINSP_H_
