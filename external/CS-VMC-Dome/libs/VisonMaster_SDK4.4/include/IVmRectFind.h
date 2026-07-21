/***************************************************************************************************
 * File：IVmRectFind.h
 * Note：@~chinese 矩形检测模块接口声明 @~english Interface for the RectFind tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_RECTFIND_H_
#define _IVM_RECTFIND_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSRectFindModu
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

        /// @~chinese 上边缘极性   @~english Upper Edge Polarity
        typedef enum _EdgeUpPolarityEnum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            EdgeUpPolarity_BlackToWhite = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            EdgeUpPolarity_WhiteToBlack = 0x2,

            /// @~chinese 任意极性   @~english Both
            EdgeUpPolarity_Both = 0x3,

        }EdgeUpPolarityEnum;

        /// @~chinese 下边缘极性   @~english Lower Edge Polarity
        typedef enum _EdgeDownPolarityEnum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            EdgeDownPolarity_BlackToWhite = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            EdgeDownPolarity_WhiteToBlack = 0x2,

            /// @~chinese 任意极性   @~english Both
            EdgeDownPolarity_Both = 0x3,

        }EdgeDownPolarityEnum;

        /// @~chinese 左边缘极性   @~english Left Edge Polarity
        typedef enum _EdgeLeftPolarityEnum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            EdgeLeftPolarity_BlackToWhite = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            EdgeLeftPolarity_WhiteToBlack = 0x2,

            /// @~chinese 任意极性   @~english Both
            EdgeLeftPolarity_Both = 0x3,

        }EdgeLeftPolarityEnum;

        /// @~chinese 右边缘极性   @~english Right Edge Polarity
        typedef enum _EdgeRightPolarityEnum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            EdgeRightPolarity_BlackToWhite = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            EdgeRightPolarity_WhiteToBlack = 0x2,

            /// @~chinese 任意极性   @~english Both
            EdgeRightPolarity_Both = 0x3,

        }EdgeRightPolarityEnum;

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

    /// \addtogroup 矩形检测
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///矩形检测ROI管理类
        class RectFindRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

            /// @~chinese 掩膜图像（仅当次执行起效）   @~english Mask Image(Only valid for this execution)
            virtual void SetMaskImage(ImageBaseData stMaskImage) = 0;

        };

        /*  Note: Interface Classes of the parameter of the RectFindParams.  */
        ///矩形检测参数类
        class RectFindParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit RectFindParams() {}
            virtual ~RectFindParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual RectFindRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 边缘对类型   @~english Edge Pair Type
            __declspec(property(put = SetEdgeType, get = GetEdgeType)) EdgeTypeEnum EdgeType;

            virtual EdgeTypeEnum GetEdgeType() = 0;

            virtual void SetEdgeType(EdgeTypeEnum value) = 0;

            /// @~chinese 上边缘极性   @~english Upper Edge Polarity
            __declspec(property(put = SetEdgeUpPolarity, get = GetEdgeUpPolarity)) EdgeUpPolarityEnum EdgeUpPolarity;

            virtual EdgeUpPolarityEnum GetEdgeUpPolarity() = 0;

            virtual void SetEdgeUpPolarity(EdgeUpPolarityEnum value) = 0;

            /// @~chinese 下边缘极性   @~english Lower Edge Polarity
            __declspec(property(put = SetEdgeDownPolarity, get = GetEdgeDownPolarity)) EdgeDownPolarityEnum EdgeDownPolarity;

            virtual EdgeDownPolarityEnum GetEdgeDownPolarity() = 0;

            virtual void SetEdgeDownPolarity(EdgeDownPolarityEnum value) = 0;

            /// @~chinese 左边缘极性   @~english Left Edge Polarity
            __declspec(property(put = SetEdgeLeftPolarity, get = GetEdgeLeftPolarity)) EdgeLeftPolarityEnum EdgeLeftPolarity;

            virtual EdgeLeftPolarityEnum GetEdgeLeftPolarity() = 0;

            virtual void SetEdgeLeftPolarity(EdgeLeftPolarityEnum value) = 0;

            /// @~chinese 右边缘极性   @~english Right Edge Polarity
            __declspec(property(put = SetEdgeRightPolarity, get = GetEdgeRightPolarity)) EdgeRightPolarityEnum EdgeRightPolarity;

            virtual EdgeRightPolarityEnum GetEdgeRightPolarity() = 0;

            virtual void SetEdgeRightPolarity(EdgeRightPolarityEnum value) = 0;

            /// @~chinese 边缘阈值，范围：[0,255]   @~english Edge Threshold，range:[0,255]
            __declspec(property(put = SetEdgeStrength, get = GetEdgeStrength)) int EdgeStrength;

            virtual int GetEdgeStrength() = 0;

            virtual void SetEdgeStrength(int value) = 0;

            /// @~chinese 卡尺数量，范围：[2,1000]   @~english Caliper Number，range:[2,1000]
            __declspec(property(put = SetCaliperNum, get = GetCaliperNum)) int CaliperNum;

            virtual int GetCaliperNum() = 0;

            virtual void SetCaliperNum(int value) = 0;

            /// @~chinese 理想宽度，范围：[1,10000]   @~english Ideal Width，range:[1,10000]
            __declspec(property(put = SetIdeaWidth, get = GetIdeaWidth)) int IdeaWidth;

            virtual int GetIdeaWidth() = 0;

            virtual void SetIdeaWidth(int value) = 0;

            /// @~chinese 理想高度，范围：[1,10000]   @~english Ideal Height，range:[1,10000]
            __declspec(property(put = SetIdeaHeight, get = GetIdeaHeight)) int IdeaHeight;

            virtual int GetIdeaHeight() = 0;

            virtual void SetIdeaHeight(int value) = 0;

            /// @~chinese 滤波尺寸，范围：[1,50]   @~english Filter Size，range:[1,50]
            __declspec(property(put = SetKernelSize, get = GetKernelSize)) int KernelSize;

            virtual int GetKernelSize() = 0;

            virtual void SetKernelSize(int value) = 0;

            /// @~chinese 投影宽度，范围：[1,100]   @~english Projection Width，range:[1,100]
            __declspec(property(put = SetProjectLen, get = GetProjectLen)) int ProjectLen;

            virtual int GetProjectLen() = 0;

            virtual void SetProjectLen(int value) = 0;

            /// @~chinese 剔除点数，范围：[0,100]   @~english Reject Number，range:[0,100]
            __declspec(property(put = SetRejectNum, get = GetRejectNum)) int RejectNum;

            virtual int GetRejectNum() = 0;

            virtual void SetRejectNum(int value) = 0;

            /// @~chinese 剔除距离，范围：[1,1000]   @~english Distance to Remove，range:[1,1000]
            __declspec(property(put = SetRejectDist, get = GetRejectDist)) int RejectDist;

            virtual int GetRejectDist() = 0;

            virtual void SetRejectDist(int value) = 0;

            /// @~chinese 初始拟合   @~english Initial Fit
            __declspec(property(put = SetFitInitType, get = GetFitInitType)) FitInitTypeEnum FitInitType;

            virtual FitInitTypeEnum GetFitInitType() = 0;

            virtual void SetFitInitType(FitInitTypeEnum value) = 0;

            /// @~chinese 拟合方式   @~english Fit Mode
            __declspec(property(put = SetFitFun, get = GetFitFun)) FitFunEnum FitFun;

            virtual FitFunEnum GetFitFun() = 0;

            virtual void SetFitFun(FitFunEnum value) = 0;

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

            /// @~chinese 高度判断   @~english Height Check
            __declspec(property(put = SetRectHeightLimitEnable, get = GetRectHeightLimitEnable)) bool RectHeightLimitEnable;

            virtual bool GetRectHeightLimitEnable() = 0;

            virtual void SetRectHeightLimitEnable(bool value) = 0;

            /// @~chinese 高度范围，范围：[1,99999]   @~english Height Range，range:[1,99999]
            __declspec(property(put = SetRectHeightLimitLow, get = GetRectHeightLimitLow)) double RectHeightLimitLow;

            virtual double GetRectHeightLimitLow() = 0;

            virtual void SetRectHeightLimitLow(double value) = 0;

            /// @~chinese 高度范围，范围：[1,99999]   @~english Height Range，range:[1,99999]
            __declspec(property(put = SetRectHeightLimitHigh, get = GetRectHeightLimitHigh)) double RectHeightLimitHigh;

            virtual double GetRectHeightLimitHigh() = 0;

            virtual void SetRectHeightLimitHigh(double value) = 0;

            /// @~chinese 宽度判断   @~english Width Check
            __declspec(property(put = SetRectWidthLimitEnable, get = GetRectWidthLimitEnable)) bool RectWidthLimitEnable;

            virtual bool GetRectWidthLimitEnable() = 0;

            virtual void SetRectWidthLimitEnable(bool value) = 0;

            /// @~chinese 宽度范围，范围：[1,99999]   @~english Width Range，range:[1,99999]
            __declspec(property(put = SetRectWidthLimitLow, get = GetRectWidthLimitLow)) double RectWidthLimitLow;

            virtual double GetRectWidthLimitLow() = 0;

            virtual void SetRectWidthLimitLow(double value) = 0;

            /// @~chinese 宽度范围，范围：[1,99999]   @~english Width Range，range:[1,99999]
            __declspec(property(put = SetRectWidthLimitHigh, get = GetRectWidthLimitHigh)) double RectWidthLimitHigh;

            virtual double GetRectWidthLimitHigh() = 0;

            virtual void SetRectWidthLimitHigh(double value) = 0;

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
        /// 单个边缘点信息
        class IRectFindEdgePointInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IRectFindEdgePointInfo() {}
            virtual ~IRectFindEdgePointInfo() {}

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
        /*  Note: Interface Classes of the result of the RectFindResults.  */
        ///矩形检测结果类
        class RectFindResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit RectFindResults() {}
            virtual ~RectFindResults() {}

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
             * @brief 检测状态
             */
            virtual int GetStatus() = 0;
            /**
             * @brief 矩形检测框
             */
            virtual RectBox GetRectBox() = 0;
            /**
             * @brief 边缘点点数
             */
            virtual int GetEdgePointNum() = 0;
            /**
             * @brief 获取结果中的边缘点信息
             * @param nPtIndex [IN] 边缘点索引
             * @return 返回结果边缘点序
             * @remarks nPtIndex从0开始计数。
             */
            virtual IRectFindEdgePointInfo* GetEdgePointInfo(int nPtIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the RectFind tool.  */
        ///矩形检测工具类
        class IMVSRectFindModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSRectFindModuTool() {}
            virtual ~IMVSRectFindModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual RectFindParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual RectFindResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSRectFindModuTool(IMVSRectFindModuTool&);
            IMVSRectFindModuTool& operator=(IMVSRectFindModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for RectFind.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSRectFindModu::IMVSRectFindModuTool * __stdcall GetRectFindToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_RECTFIND_H_
