/***************************************************************************************************
 * File：IVmCaliperCorner.h
 * Note：@~chinese 边缘交点模块接口声明 @~english Interface for the CaliperCorner tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CALIPERCORNER_H_
#define _IVM_CALIPERCORNER_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCaliperCornerModu
    {
        /// @~chinese 边缘1类型   @~english Edge 1 Type
        typedef enum _Edge0TypeEnum
        {
            /// @~chinese 最强   @~english Strongest Edge
            Edge0Type_Best = 0x1,

            /// @~chinese 第一条   @~english First Edge
            Edge0Type_First = 0x2,

            /// @~chinese 最后一条   @~english Last Edge
            Edge0Type_Last = 0x3,

        }Edge0TypeEnum;

        /// @~chinese 边缘1极性   @~english Edge 1 polarity
        typedef enum _Edge0PolarityEnum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            Edge0Polarity_BlackToWhite = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            Edge0Polarity_WhiteToBlack = 0x2,

            /// @~chinese 任意极性   @~english Both
            Edge0Polarity_Both = 0x3,

        }Edge0PolarityEnum;

        /// @~chinese 边缘2类型   @~english Edge 2 Type
        typedef enum _Edge1TypeEnum
        {
            /// @~chinese 最强   @~english Strongest Edge
            Edge1Type_Best = 0x1,

            /// @~chinese 第一条   @~english First Edge
            Edge1Type_First = 0x2,

            /// @~chinese 最后一条   @~english Last Edge
            Edge1Type_Last = 0x3,

        }Edge1TypeEnum;

        /// @~chinese 边缘2极性   @~english Edge 2 Polarity
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

    /// \addtogroup 边缘交点
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///边缘交点ROI管理类
        class CaliperCornerRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox* pRoiValue, unsigned int nValueNum) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the CaliperCornerParams.  */
        ///边缘交点参数类
        class CaliperCornerParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CaliperCornerParams() {}
            virtual ~CaliperCornerParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual CaliperCornerRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 边缘1类型   @~english Edge 1 Type
            __declspec(property(put = SetEdge0Type, get = GetEdge0Type)) Edge0TypeEnum Edge0Type;

            virtual Edge0TypeEnum GetEdge0Type() = 0;

            virtual void SetEdge0Type(Edge0TypeEnum value) = 0;

            /// @~chinese 边缘1极性   @~english Edge 1 polarity
            __declspec(property(put = SetEdge0Polarity, get = GetEdge0Polarity)) Edge0PolarityEnum Edge0Polarity;

            virtual Edge0PolarityEnum GetEdge0Polarity() = 0;

            virtual void SetEdge0Polarity(Edge0PolarityEnum value) = 0;

            /// @~chinese 边缘2类型   @~english Edge 2 Type
            __declspec(property(put = SetEdge1Type, get = GetEdge1Type)) Edge1TypeEnum Edge1Type;

            virtual Edge1TypeEnum GetEdge1Type() = 0;

            virtual void SetEdge1Type(Edge1TypeEnum value) = 0;

            /// @~chinese 边缘2极性   @~english Edge 2 Polarity
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

            /// @~chinese 交点X判断   @~english Intersection X Check
            __declspec(property(put = SetCornerPointXLimitEnable, get = GetCornerPointXLimitEnable)) bool CornerPointXLimitEnable;

            virtual bool GetCornerPointXLimitEnable() = 0;

            virtual void SetCornerPointXLimitEnable(bool value) = 0;

            /// @~chinese 交点X范围，范围：[-99999,99999]   @~english Intersection X Range，range:[-99999,99999]
            __declspec(property(put = SetCornerPointXLimitLow, get = GetCornerPointXLimitLow)) double CornerPointXLimitLow;

            virtual double GetCornerPointXLimitLow() = 0;

            virtual void SetCornerPointXLimitLow(double value) = 0;

            /// @~chinese 交点X范围，范围：[-99999,99999]   @~english Intersection X Range，range:[-99999,99999]
            __declspec(property(put = SetCornerPointXLimitHigh, get = GetCornerPointXLimitHigh)) double CornerPointXLimitHigh;

            virtual double GetCornerPointXLimitHigh() = 0;

            virtual void SetCornerPointXLimitHigh(double value) = 0;

            /// @~chinese 交点Y判断   @~english Intersection Y Check
            __declspec(property(put = SetCornerPointYLimitEnable, get = GetCornerPointYLimitEnable)) bool CornerPointYLimitEnable;

            virtual bool GetCornerPointYLimitEnable() = 0;

            virtual void SetCornerPointYLimitEnable(bool value) = 0;

            /// @~chinese 交点Y范围，范围：[-99999,99999]   @~english Intersection Y Range，range:[-99999,99999]
            __declspec(property(put = SetCornerPointYLimitLow, get = GetCornerPointYLimitLow)) double CornerPointYLimitLow;

            virtual double GetCornerPointYLimitLow() = 0;

            virtual void SetCornerPointYLimitLow(double value) = 0;

            /// @~chinese 交点Y范围，范围：[-99999,99999]   @~english Intersection Y Range，range:[-99999,99999]
            __declspec(property(put = SetCornerPointYLimitHigh, get = GetCornerPointYLimitHigh)) double CornerPointYLimitHigh;

            virtual double GetCornerPointYLimitHigh() = 0;

            virtual void SetCornerPointYLimitHigh(double value) = 0;

            /// @~chinese 交点角度判断   @~english Intersection Angle Check
            __declspec(property(put = SetCornerAngleLimitEnable, get = GetCornerAngleLimitEnable)) bool CornerAngleLimitEnable;

            virtual bool GetCornerAngleLimitEnable() = 0;

            virtual void SetCornerAngleLimitEnable(bool value) = 0;

            /// @~chinese 交点角度范围，范围：[-180,180]   @~english Intersection Angle Range，range:[-180,180]
            __declspec(property(put = SetCornerAngleLimitLow, get = GetCornerAngleLimitLow)) double CornerAngleLimitLow;

            virtual double GetCornerAngleLimitLow() = 0;

            virtual void SetCornerAngleLimitLow(double value) = 0;

            /// @~chinese 交点角度范围，范围：[-180,180]   @~english Intersection Angle Range，range:[-180,180]
            __declspec(property(put = SetCornerAngleLimitHigh, get = GetCornerAngleLimitHigh)) double CornerAngleLimitHigh;

            virtual double GetCornerAngleLimitHigh() = 0;

            virtual void SetCornerAngleLimitHigh(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /** @enum  MVD_EDGEPOINT_STATUS
         *  @brief 边缘点状态
         */
		/// @~chinese 边缘点状态 @~english edge point status
        typedef _MVD_EDGEPOINT_STATUS__ MVD_EDGEPOINT_STATUS;

        /** @enum  MVD_EDGE_POLARITY
         *  @brief 边缘极性
         */
		/// @~chinese 边缘极性 @~english edge polarity
        typedef _MVD_EDGE_POLARITY__ MVD_EDGE_POLARITY;

        ///边缘点信息描述类
        class ICornerFindEdgePointInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit ICornerFindEdgePointInfo() {}
            virtual ~ICornerFindEdgePointInfo() {}

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

        ///查找结果中边缘线信息
        class ICornerFindLineInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit ICornerFindLineInfo() {}
            virtual ~ICornerFindLineInfo() {}

        public:
            /**
             * @brief 直线位置
             */
            virtual Line GetLine() = 0;
            /**
             * @brief 边缘点点数
             */
            virtual int GetEdgePointNum() = 0;
            /**
             * @brief 获取结果中的边缘点信息
             * @param nPtIndex [IN] 边缘点索引
             * @return 返回结果边缘点序
             * @remarks nPtIndex从0开始计数
             */
            virtual ICornerFindEdgePointInfo* GetEdgePointInfo(int nPtIndex) = 0;
        };

        /*  Note: Interface Classes of the result of the CaliperCornerResults.  */
        ///边缘交点结果类
        class CaliperCornerResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CaliperCornerResults() {}
            virtual ~CaliperCornerResults() {}

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
             * @brief 检测区域1
             */
             virtual RectBox GetROI1() = 0;
             /**
             * @brief 检测区域2
             */
             virtual RectBox GetROI2() = 0;
            /**
             * @brief 检测状态,1:有效、其他：无效
             */
            virtual int GetStatus() = 0;
            /**
             * @brief 交叉点
             */
            virtual MVD_POINT_F GetIntersectionPoint() = 0;
            /**
             * @brief 交叉角度
             */
            virtual float GetIntersectionAngle() = 0;
            /**
             * @brief 直线0信息
             */
            virtual ICornerFindLineInfo* GetLine0Info() = 0;
            /**
             * @brief 直线1信息
             */
            virtual ICornerFindLineInfo* GetLine1Info() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CaliperCorner tool.  */
        ///边缘交点工具类
        class IMVSCaliperCornerModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCaliperCornerModuTool() {}
            virtual ~IMVSCaliperCornerModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CaliperCornerParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CaliperCornerResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSCaliperCornerModuTool(IMVSCaliperCornerModuTool&);
            IMVSCaliperCornerModuTool& operator=(IMVSCaliperCornerModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CaliperCorner.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSCaliperCornerModu::IMVSCaliperCornerModuTool * __stdcall GetCaliperCornerToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CALIPERCORNER_H_
