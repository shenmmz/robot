/***************************************************************************************************
 * File：IVmPairLine.h
 * Note：@~chinese 平行线查找模块接口声明 @~english Interface for the PairLine tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_PAIRLINE_H_
#define _IVM_PAIRLINE_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSPairLineModu
    {
        /// @~chinese 边缘对类型   @~english Edge Pair Type
        typedef enum _EdgeTypeEnum
        {
            /// @~chinese 最宽   @~english Widest Rect
            EdgeType_Widest = 0x1,

            /// @~chinese 最窄   @~english Narrowest Rect
            EdgeType_Narrowest = 0x2,

            /// @~chinese 最强   @~english Strongest Edge
            EdgeType_Strongest = 0x3,

            /// @~chinese 最弱   @~english Min Contrast
            EdgeType_Worst = 0x4,

            /// @~chinese 第一对   @~english The First Pair
            EdgeType_First = 0x5,

            /// @~chinese 最后一对   @~english The Last Pair
            EdgeType_Last = 0x6,

            /// @~chinese 最接近   @~english Best Rectangularity
            EdgeType_Nearest = 0x7,

            /// @~chinese 最不接近   @~english Worst Rectangularity
            EdgeType_Farthest = 0x8,

        }EdgeTypeEnum;

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

        /// @~chinese 初始拟合   @~english Initial Fit
        typedef enum _FitInitTypeEnum
        {
            /// @~chinese 全局   @~english Global
            FitInitType_ALS = 0x1,

            /// @~chinese 局部   @~english Local
            FitInitType_LLS = 0x2,

        }FitInitTypeEnum;

        /// @~chinese 拟合方式   @~english Fit Mode
        typedef enum _FitFunEnum
        {
            /// @~chinese 最小二乘   @~english Least Squares
            FitFun_LS = 0x1,

            /// @~chinese huber   @~english huber
            FitFun_Huber = 0x2,

            /// @~chinese tukey   @~english tukey
            FitFun_Tukey = 0x3,

        }FitFunEnum;

    /// \addtogroup 平行线查找
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///平行线查找ROI管理类
        class PairLineRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

            /// @~chinese 掩膜图像（仅当次执行起效）   @~english Mask Image(Only valid for this execution)
            virtual void SetMaskImage(ImageBaseData stMaskImage) = 0;

        };

        /*  Note: Interface Classes of the parameter of the PairLineParams.  */
        ///平行线查找参数类
        class PairLineParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit PairLineParams() {}
            virtual ~PairLineParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual PairLineRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 边缘对类型   @~english Edge Pair Type
            __declspec(property(put = SetEdgeType, get = GetEdgeType)) EdgeTypeEnum EdgeType;

            virtual EdgeTypeEnum GetEdgeType() = 0;

            virtual void SetEdgeType(EdgeTypeEnum value) = 0;

            /// @~chinese 边缘0极性   @~english Edge 0 polarity
            __declspec(property(put = SetEdge0Polarity, get = GetEdge0Polarity)) Edge0PolarityEnum Edge0Polarity;

            virtual Edge0PolarityEnum GetEdge0Polarity() = 0;

            virtual void SetEdge0Polarity(Edge0PolarityEnum value) = 0;

            /// @~chinese 边缘1极性   @~english Edge 1 polarity
            __declspec(property(put = SetEdge1Polarity, get = GetEdge1Polarity)) Edge1PolarityEnum Edge1Polarity;

            virtual Edge1PolarityEnum GetEdge1Polarity() = 0;

            virtual void SetEdge1Polarity(Edge1PolarityEnum value) = 0;

            /// @~chinese 边缘阈值，范围：[0,255]   @~english Edge Threshold，range:[0,255]
            __declspec(property(put = SetEdgeStrength, get = GetEdgeStrength)) int EdgeStrength;

            virtual int GetEdgeStrength() = 0;

            virtual void SetEdgeStrength(int value) = 0;

            /// @~chinese 卡尺数量，范围：[2,1000]   @~english Caliper Number，range:[2,1000]
            __declspec(property(put = SetCaliperNum, get = GetCaliperNum)) int CaliperNum;

            virtual int GetCaliperNum() = 0;

            virtual void SetCaliperNum(int value) = 0;

            /// @~chinese 理想间距，范围：[1,10000]   @~english Ideal Distance，range:[1,10000]
            __declspec(property(put = SetIdeaWidth, get = GetIdeaWidth)) int IdeaWidth;

            virtual int GetIdeaWidth() = 0;

            virtual void SetIdeaWidth(int value) = 0;

            /// @~chinese 最大角度差，范围：[0,180]   @~english Max. Angle Difference，range:[0,180]
            __declspec(property(put = SetAngleTol, get = GetAngleTol)) int AngleTol;

            virtual int GetAngleTol() = 0;

            virtual void SetAngleTol(int value) = 0;

            /// @~chinese 剔除点数，范围：[0,100]   @~english Reject Number，range:[0,100]
            __declspec(property(put = SetRejectNum, get = GetRejectNum)) int RejectNum;

            virtual int GetRejectNum() = 0;

            virtual void SetRejectNum(int value) = 0;

            /// @~chinese 剔除距离，范围：[1,1000]   @~english Distance to Remove，range:[1,1000]
            __declspec(property(put = SetRejectDist, get = GetRejectDist)) int RejectDist;

            virtual int GetRejectDist() = 0;

            virtual void SetRejectDist(int value) = 0;

            /// @~chinese 滤波尺寸，范围：[1,50]   @~english Filter Size，range:[1,50]
            __declspec(property(put = SetKernelSize, get = GetKernelSize)) int KernelSize;

            virtual int GetKernelSize() = 0;

            virtual void SetKernelSize(int value) = 0;

            /// @~chinese 投影宽度，范围：[1,100]   @~english Projection Width，range:[1,100]
            __declspec(property(put = SetProjectLen, get = GetProjectLen)) int ProjectLen;

            virtual int GetProjectLen() = 0;

            virtual void SetProjectLen(int value) = 0;

            /// @~chinese 初始拟合   @~english Initial Fit
            __declspec(property(put = SetFitInitType, get = GetFitInitType)) FitInitTypeEnum FitInitType;

            virtual FitInitTypeEnum GetFitInitType() = 0;

            virtual void SetFitInitType(FitInitTypeEnum value) = 0;

            /// @~chinese 拟合方式   @~english Fit Mode
            __declspec(property(put = SetFitFun, get = GetFitFun)) FitFunEnum FitFun;

            virtual FitFunEnum GetFitFun() = 0;

            virtual void SetFitFun(FitFunEnum value) = 0;

            /// @~chinese 线对宽度   @~english Line Pair Width
            __declspec(property(put = SetLineWidthLimitEnable, get = GetLineWidthLimitEnable)) bool LineWidthLimitEnable;

            virtual bool GetLineWidthLimitEnable() = 0;

            virtual void SetLineWidthLimitEnable(bool value) = 0;

            /// @~chinese 宽度范围，范围：[0,9999]   @~english Width Range，range:[0,9999]
            __declspec(property(put = SetLineWidthLimitLow, get = GetLineWidthLimitLow)) double LineWidthLimitLow;

            virtual double GetLineWidthLimitLow() = 0;

            virtual void SetLineWidthLimitLow(double value) = 0;

            /// @~chinese 宽度范围，范围：[0,9999]   @~english Width Range，range:[0,9999]
            __declspec(property(put = SetLineWidthLimitHigh, get = GetLineWidthLimitHigh)) double LineWidthLimitHigh;

            virtual double GetLineWidthLimitHigh() = 0;

            virtual void SetLineWidthLimitHigh(double value) = 0;

            /// @~chinese 直线0角度   @~english Line0Angle
            __declspec(property(put = SetAngle0LimitEnable, get = GetAngle0LimitEnable)) bool Angle0LimitEnable;

            virtual bool GetAngle0LimitEnable() = 0;

            virtual void SetAngle0LimitEnable(bool value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngle0LimitLow, get = GetAngle0LimitLow)) double Angle0LimitLow;

            virtual double GetAngle0LimitLow() = 0;

            virtual void SetAngle0LimitLow(double value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngle0LimitHigh, get = GetAngle0LimitHigh)) double Angle0LimitHigh;

            virtual double GetAngle0LimitHigh() = 0;

            virtual void SetAngle0LimitHigh(double value) = 0;

            /// @~chinese 直线1角度   @~english Line1Angle
            __declspec(property(put = SetAngle1LimitEnable, get = GetAngle1LimitEnable)) bool Angle1LimitEnable;

            virtual bool GetAngle1LimitEnable() = 0;

            virtual void SetAngle1LimitEnable(bool value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngle1LimitLow, get = GetAngle1LimitLow)) double Angle1LimitLow;

            virtual double GetAngle1LimitLow() = 0;

            virtual void SetAngle1LimitLow(double value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngle1LimitHigh, get = GetAngle1LimitHigh)) double Angle1LimitHigh;

            virtual double GetAngle1LimitHigh() = 0;

            virtual void SetAngle1LimitHigh(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /** @enum  MVD_EDGEPOINT_STATUS
        *  @brief Find status of the edge point
        */
		/// @~chinese 边缘点状态 @~english Find status of the edge point
        typedef _MVD_EDGEPOINT_STATUS__ MVD_EDGEPOINT_STATUS;

        /** @enum  MVD_EDGEPOINT_STATUS
        *  @brief Type of the polarity of the edge point
        */
		/// @~chinese 边缘点极性 @~english Type of the polarity of the edge point
        typedef _MVD_EDGE_POLARITY__ MVD_EDGE_POLARITY;
        
        /*  Note: Interface Classes for describing a edge point Information.  */
		/// 边缘点信息描述类
        class IPairLineFindEdgePointInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IPairLineFindEdgePointInfo() {}
            virtual ~IPairLineFindEdgePointInfo() {}

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

        /*  Note: Interface Classes of the result of the PairLineResults.  */
        ///平行线查找结果类
        class PairLineResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit PairLineResults() {}
            virtual ~PairLineResults() {}

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
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;
             /**
              * @brief 边缘直线0
              */
            virtual Line GetLine0Info() = 0;
            /**
             * @brief 边缘直线1
             */
            virtual Line GetLine1Info() = 0;
            /**
             * @brief 边缘中线
             */
            virtual Line GetLineMidInfo() = 0;
            /**
             * @brief 检测状态
             */
            virtual int GetStatus() = 0;
            /**
             * @brief 获取间距
             */
            virtual float GetLineWidth() = 0;
            /**
             * @brief 获取边缘点数量
             */
            virtual int GetEdgePointNum() = 0;
            /**
             * @brief 获取边缘0的点信息
             * @param nPtIndex [IN] 点的索引值，从0开始计数
             */
            virtual IPairLineFindEdgePointInfo* GetEdge0PointInfo(int nPtIndex) = 0;
            /**
             * @brief 获取边缘1的点信息
             * @param nPtIndex [IN] 点的索引值，从0开始计数
             */
            virtual IPairLineFindEdgePointInfo* GetEdge1PointInfo(int nPtIndex) = 0;
            /**
             * @brief 获取中间点的信息
             * @param nPtIndex [IN] 点的索引值，从0开始计数
             */
            virtual IPairLineFindEdgePointInfo* GetEdgeMidPointInfo(int nPtIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the PairLine tool.  */
        ///平行线查找工具类
        class IMVSPairLineModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSPairLineModuTool() {}
            virtual ~IMVSPairLineModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual PairLineParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual PairLineResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSPairLineModuTool(IMVSPairLineModuTool&);
            IMVSPairLineModuTool& operator=(IMVSPairLineModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for PairLine.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSPairLineModu::IMVSPairLineModuTool * __stdcall GetPairLineToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_PAIRLINE_H_
