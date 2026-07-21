/***************************************************************************************************
 * File：IVmMultiLineFind.h
 * Note：@~chinese 多直线查找模块接口声明 @~english Interface for the MultiLineFind tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_MULTILINEFIND_H_
#define _IVM_MULTILINEFIND_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSMultiLineFindModu
    {
        /// @~chinese 直线边缘极性   @~english Line Edge Polarity
        typedef enum _EdgePolarityEnum
        {
            /// @~chinese 由黑到白   @~english Dark To Bright
            EdgePolarity_DarkToBright = 1,

            /// @~chinese 由白到黑   @~english Bright To Dark
            EdgePolarity_BrightToDark = 2,

            /// @~chinese 任意(同一条线段中点集有两种极性点)   @~english Any (There are 2 polarity points in midpoint sets of the same line segment.)
            EdgePolarity_Mixed = 3,

            /// @~chinese 黑到白或白到黑(同一条线段中点集有一种极性点)   @~english From black to white or from white to black (There are 1 polarity point in midpoint sets of the same line segment.)
            EdgePolarity_Either = 4,

        }EdgePolarityEnum;

        /// @~chinese 拟合方式   @~english Fit Mode
        typedef enum _LineFitFunEnum
        {
            /// @~chinese 最小二乘   @~english Least Squares
            LineFitFun_LS = 0x1,

            /// @~chinese huber   @~english huber
            LineFitFun_Huber = 0x2,

            /// @~chinese tukey   @~english tukey
            LineFitFun_Tukey = 0x3,

        }LineFitFunEnum;

        /// @~chinese 排序类型   @~english SortType
        typedef enum _SortTypeEnum
        {
            /// @~chinese 得分   @~english InGroupPtNumDscend
            SortType_InGroupPtNumDscend = 0x1,

            /// @~chinese ROI方向排序   @~english RhoAscend
            SortType_RhoAscend = 0x2,

            /// @~chinese ROI反方向排序   @~english RhoDscend
            SortType_RhoDscend = 0x3,

        }SortTypeEnum;

    /// \addtogroup 多直线查找
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///多直线查找ROI管理类
        class MultiLineFindRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the MultiLineFindParams.  */
        ///多直线查找参数类
        class MultiLineFindParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MultiLineFindParams() {}
            virtual ~MultiLineFindParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual MultiLineFindRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 滤波核半宽，范围：[1,50]   @~english Filter Kernel Half-Width，range:[1,50]
            __declspec(property(put = SetGradientFieldSize, get = GetGradientFieldSize)) int GradientFieldSize;

            virtual int GetGradientFieldSize() = 0;

            virtual void SetGradientFieldSize(int value) = 0;

            /// @~chinese 投影长度，范围：[3,100]   @~english Projection Length，range:[3,100]
            __declspec(property(put = SetProjectionLength, get = GetProjectionLength)) int ProjectionLength;

            virtual int GetProjectionLength() = 0;

            virtual void SetProjectionLength(int value) = 0;

            /// @~chinese 绝对边缘阈值，范围：[0,255]   @~english Absolute Edge Threshold，range:[0,255]
            __declspec(property(put = SetEdgeThreshold, get = GetEdgeThreshold)) int EdgeThreshold;

            virtual int GetEdgeThreshold() = 0;

            virtual void SetEdgeThreshold(int value) = 0;

            /// @~chinese 相对边缘阈值，范围：[0,100]   @~english Relative Edge Threshold，range:[0,100]
            __declspec(property(put = SetNormalContrast, get = GetNormalContrast)) int NormalContrast;

            virtual int GetNormalContrast() = 0;

            virtual void SetNormalContrast(int value) = 0;

            /// @~chinese 直线边缘极性   @~english Line Edge Polarity
            __declspec(property(put = SetEdgePolarity, get = GetEdgePolarity)) EdgePolarityEnum EdgePolarity;

            virtual EdgePolarityEnum GetEdgePolarity() = 0;

            virtual void SetEdgePolarity(EdgePolarityEnum value) = 0;

            /// @~chinese 边缘角度容忍，范围：[0,90]   @~english Edge Angle Tolerance，range:[0,90]
            __declspec(property(put = SetEdgeAngleTolerance, get = GetEdgeAngleTolerance)) int EdgeAngleTolerance;

            virtual int GetEdgeAngleTolerance() = 0;

            virtual void SetEdgeAngleTolerance(int value) = 0;

            /// @~chinese 边缘距离容忍，范围：[0,100]   @~english Edge Dist Tolerance，range:[0,100]
            __declspec(property(put = SetEdgeDistTolerance, get = GetEdgeDistTolerance)) int EdgeDistTolerance;

            virtual int GetEdgeDistTolerance() = 0;

            virtual void SetEdgeDistTolerance(int value) = 0;

            /// @~chinese 多线最大条数，范围：[1,1000]   @~english Line Max Num，range:[1,1000]
            __declspec(property(put = SetLineMaxNum, get = GetLineMaxNum)) int LineMaxNum;

            virtual int GetLineMaxNum() = 0;

            virtual void SetLineMaxNum(int value) = 0;

            /// @~chinese 覆盖率阈值，范围：[0,100]   @~english Line Coverage，range:[0,100]
            __declspec(property(put = SetLineCoverage, get = GetLineCoverage)) int LineCoverage;

            virtual int GetLineCoverage() = 0;

            virtual void SetLineCoverage(int value) = 0;

            /// @~chinese 旋转角度容忍，范围：[0,90]   @~english Line Rotation Tolerance，range:[0,90]
            __declspec(property(put = SetLineRotationTolerance, get = GetLineRotationTolerance)) int LineRotationTolerance;

            virtual int GetLineRotationTolerance() = 0;

            virtual void SetLineRotationTolerance(int value) = 0;

            /// @~chinese 拟合方式   @~english Fit Mode
            __declspec(property(put = SetLineFitFun, get = GetLineFitFun)) LineFitFunEnum LineFitFun;

            virtual LineFitFunEnum GetLineFitFun() = 0;

            virtual void SetLineFitFun(LineFitFunEnum value) = 0;

            /// @~chinese 排序类型   @~english SortType
            __declspec(property(put = SetSortType, get = GetSortType)) SortTypeEnum SortType;

            virtual SortTypeEnum GetSortType() = 0;

            virtual void SetSortType(SortTypeEnum value) = 0;

            /// @~chinese 边缘点数限制最大值，范围：[1,9999999]   @~english Edge Num Max，range:[1,9999999]
            __declspec(property(put = SetEdgesNumMax, get = GetEdgesNumMax)) int EdgesNumMax;

            virtual int GetEdgesNumMax() = 0;

            virtual void SetEdgesNumMax(int value) = 0;

            /// @~chinese 边缘点数判断   @~english Edge Points Num Check
            __declspec(property(put = SetEdgePointsNumLimitEnable, get = GetEdgePointsNumLimitEnable)) bool EdgePointsNumLimitEnable;

            virtual bool GetEdgePointsNumLimitEnable() = 0;

            virtual void SetEdgePointsNumLimitEnable(bool value) = 0;

            /// @~chinese 边缘点数范围，范围：[2,99999]   @~english Edge Points Num Range，range:[2,99999]
            __declspec(property(put = SetEdgePointsNumLimitLow, get = GetEdgePointsNumLimitLow)) int EdgePointsNumLimitLow;

            virtual int GetEdgePointsNumLimitLow() = 0;

            virtual void SetEdgePointsNumLimitLow(int value) = 0;

            /// @~chinese 边缘点数范围，范围：[2,99999]   @~english Edge Points Num Range，range:[2,99999]
            __declspec(property(put = SetEdgePointsNumLimitHigh, get = GetEdgePointsNumLimitHigh)) int EdgePointsNumLimitHigh;

            virtual int GetEdgePointsNumLimitHigh() = 0;

            virtual void SetEdgePointsNumLimitHigh(int value) = 0;

            /// @~chinese 直线数量判断   @~english Line Num Check
            __declspec(property(put = SetLineNumLimitEnable, get = GetLineNumLimitEnable)) bool LineNumLimitEnable;

            virtual bool GetLineNumLimitEnable() = 0;

            virtual void SetLineNumLimitEnable(bool value) = 0;

            /// @~chinese 直线数量范围，范围：[1,99999]   @~english Line Num Range，range:[1,99999]
            __declspec(property(put = SetLineNumLimitLow, get = GetLineNumLimitLow)) int LineNumLimitLow;

            virtual int GetLineNumLimitLow() = 0;

            virtual void SetLineNumLimitLow(int value) = 0;

            /// @~chinese 直线数量范围，范围：[1,99999]   @~english Line Num Range，range:[1,99999]
            __declspec(property(put = SetLineNumLimitHigh, get = GetLineNumLimitHigh)) int LineNumLimitHigh;

            virtual int GetLineNumLimitHigh() = 0;

            virtual void SetLineNumLimitHigh(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /** @enum  MVD_EDGEPOINT_STATUS
        *  @brief Find status of the edge point
        */
		/// @~chinese 拟合点状态 @~english status of the edge point
        typedef _MVD_EDGEPOINT_STATUS__ MVD_EDGEPOINT_STATUS;

        /** @enum  MVD_EDGEPOINT_STATUS
        *  @brief Type of the polarity of the edge point
        */
		/// @~chinese 边缘极性 @~english edge polarity
        typedef _MVD_EDGE_POLARITY__ MVD_EDGE_POLARITY;
        
        /*  Note: Interface Classes for describing a edge point Information.  */
		/// 多直线查找轮廓点信息类
        class IMultiLineFindEdgePointInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IMultiLineFindEdgePointInfo() {}
            virtual ~IMultiLineFindEdgePointInfo() {}

        public:
            /**
             * @brief 边缘点坐标
             */
            virtual MVD_POINT_F GetEdgePoint() = 0;
            /**
             * @brief 边缘点极性
            */
            virtual MVD_EDGE_POLARITY GetPolarity() = 0;
            /**
             * @brief 梯度幅值
            */
            virtual float GetMagnitude() = 0;
            /**
             * @brief 梯度方向
            */
            virtual float GetOrientation() = 0;
            /**
             * @brief 边缘点距离
            */
            virtual float GetDistance() = 0;
            /**
             * @brief 边缘点状态(枚举值1-边缘点已使用，对应界面状态1；枚举值3-边缘点未使用，对应界面状态0)
            */
            virtual MVD_EDGEPOINT_STATUS GetStatus() = 0;
            /**
             * @brief 投影区域索引
             */
            virtual int GetCaliperIndex() = 0;
            /**
             * @brief 所属线段索引
            */
            virtual int GetLineIndex() = 0;
        };

        /*  Note: Interface Classes for describing a line Information.  */
		/// 多直线查找直线信息类
        class IMultiLineFindLineInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IMultiLineFindLineInfo() {}
            virtual ~IMultiLineFindLineInfo() {}

        public:
            /**
             * @brief 当前直线段
            */
            virtual Line GetLine() = 0;
            /**
             * @brief 当前直线段拟合误差
            */
            virtual float GetFitError() = 0;
            /**
             * @brief 当前直线段对比度强度
            */
            virtual float GetIntensity() = 0;
            /**
             * @brief 覆盖率分数
            */
            virtual float GetCoverageScore() = 0;
            /**
             * @brief 在群点数量
            */
            virtual int GetInliersNum() = 0;
            /**
             * @brief 线段索引
            */
            virtual int GetLineIndex() = 0;
        };

        /*  Note: Interface Classes of the result of the MultiLineFindResults.  */
        ///多直线查找结果类
        class MultiLineFindResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MultiLineFindResults() {}
            virtual ~MultiLineFindResults() {}

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
             * @brief 边缘点点数
             */
            virtual int GetEdgePointNum() = 0;
            /**
             * @brief 获取结果中的边缘点信息
             * @param nPtIndex [IN] 边缘点索引
             * @return 返回结果边缘点序
             * @remarks nPtIndex从0开始计数。             */
            virtual IMultiLineFindEdgePointInfo* GetEdgePointInfo(int nPtIndex) = 0;
            /**
             * @brief 直线段总数
             */
            virtual int GetLineNum() = 0;
            /**
             * @brief 获取结果中的直线段信息
             * @param nLineIndex [in] 直线段索引
             * @return 返回结果边缘点序
             * @remarks nPtIndex从0开始计数。
             */
            virtual IMultiLineFindLineInfo* GetLineInfo(int nLineIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the MultiLineFind tool.  */
        ///多直线查找工具类
        class IMVSMultiLineFindModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSMultiLineFindModuTool() {}
            virtual ~IMVSMultiLineFindModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual MultiLineFindParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual MultiLineFindResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSMultiLineFindModuTool(IMVSMultiLineFindModuTool&);
            IMVSMultiLineFindModuTool& operator=(IMVSMultiLineFindModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for MultiLineFind.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSMultiLineFindModu::IMVSMultiLineFindModuTool * __stdcall GetMultiLineFindToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_MULTILINEFIND_H_
