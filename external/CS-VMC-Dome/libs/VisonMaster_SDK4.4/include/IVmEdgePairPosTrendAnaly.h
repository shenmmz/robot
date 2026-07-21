/***************************************************************************************************
 * File：IVmEdgePairPosTrendAnaly.h
 * Note：@~chinese 边缘对位置趋势分析模块接口声明 @~english Interface for the EdgePairPosTrendAnaly tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_EDGEPAIRPOSTRENDANALY_H_
#define _IVM_EDGEPAIRPOSTRENDANALY_H_
#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;
#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSEdgePairPosTrendAnalyModu
    {
        /// @~chinese 边缘类型   @~english Edge Type
        typedef enum _FindModeEnum
        {
            /// @~chinese 最宽边缘对   @~english Widest edge pair
            FindMode_WidestEdgePair = 0x1,

            /// @~chinese 最窄边缘对   @~english Narrowest edge pair
            FindMode_NarrowestEdgePair = 0x2,

            /// @~chinese 最强边缘对   @~english Strongest edge pair
            FindMode_StrongestEdgePair = 0x3,

            /// @~chinese 最弱边缘对   @~english Weakest edge pair
            FindMode_WeakestEdgePair = 0x4,

            /// @~chinese 第一个边缘对   @~english First edge pair
            FindMode_FirstEdgePair = 0x5,

            /// @~chinese 最后一个边缘对   @~english Last edge pair
            FindMode_LastEdgePair = 0x6,

            /// @~chinese 最接近边缘对   @~english Nearest edge pair
            FindMode_NearestEdgePair = 0x7,

            /// @~chinese 最不接近边缘对   @~english Farthest edge pair
            FindMode_FarthestEdgePair = 0x8,

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

        /// @~chinese 边缘查找方向   @~english Edge Search Direction
        typedef enum _DetechOrientEnum
        {
            /// @~chinese 从上到下   @~english Top to Bottom
            DetechOrient_T2B = 0x1,

            /// @~chinese 从左到右   @~english Left to Right
            DetechOrient_L2R = 0x2,

        }DetechOrientEnum;

    /// \addtogroup 边缘对位置趋势分析
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///边缘对位置趋势分析ROI管理类
        class EdgePairPosTrendAnalyRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the EdgePairPosTrendAnalyParams.  */
        ///边缘对位置趋势分析参数类
        class EdgePairPosTrendAnalyParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit EdgePairPosTrendAnalyParams() {}
            virtual ~EdgePairPosTrendAnalyParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual EdgePairPosTrendAnalyRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 边缘类型   @~english Edge Type
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

            /// @~chinese 滤波尺寸，范围：[1,50]   @~english Filter Size，range:[1,50]
            __declspec(property(put = SetHalfKernelSize, get = GetHalfKernelSize)) int HalfKernelSize;

            virtual int GetHalfKernelSize() = 0;

            virtual void SetHalfKernelSize(int value) = 0;

            /// @~chinese 边缘阈值，范围：[1,255]   @~english Edge Threshold，range:[1,255]
            __declspec(property(put = SetEdgeStrength, get = GetEdgeStrength)) int EdgeStrength;

            virtual int GetEdgeStrength() = 0;

            virtual void SetEdgeStrength(int value) = 0;

            /// @~chinese 理想边缘宽度，范围：[1,1000]   @~english Idea edge width，range:[1,1000]
            __declspec(property(put = SetIdeaWid, get = GetIdeaWid)) int IdeaWid;

            virtual int GetIdeaWid() = 0;

            virtual void SetIdeaWid(int value) = 0;

            /// @~chinese 卡尺数量，范围：[1,1000]   @~english Caliper Number，range:[1,1000]
            __declspec(property(put = SetCaliperCount, get = GetCaliperCount)) int CaliperCount;

            virtual int GetCaliperCount() = 0;

            virtual void SetCaliperCount(int value) = 0;

            /// @~chinese 边缘查找方向   @~english Edge Search Direction
            __declspec(property(put = SetDetechOrient, get = GetDetechOrient)) DetechOrientEnum DetechOrient;

            virtual DetechOrientEnum GetDetechOrient() = 0;

            virtual void SetDetechOrient(DetechOrientEnum value) = 0;

            /// @~chinese 卡尺宽度，范围：[1,500]   @~english CaliperWidth，range:[1,500]
            __declspec(property(put = SetProjectionLength, get = GetProjectionLength)) int ProjectionLength;

            virtual int GetProjectionLength() = 0;

            virtual void SetProjectionLength(int value) = 0;

            /// @~chinese 距离低阈值，范围：[0,10000]   @~english Distance Lower Threshold，range:[0,10000]
            __declspec(property(put = SetDistLow, get = GetDistLow)) int DistLow;

            virtual int GetDistLow() = 0;

            virtual void SetDistLow(int value) = 0;

            /// @~chinese 高阈值使能   @~english High Threshold Enable
            __declspec(property(put = SetDistHighIsAutoEnable, get = GetDistHighIsAutoEnable)) bool DistHighIsAutoEnable;

            virtual bool GetDistHighIsAutoEnable() = 0;

            virtual void SetDistHighIsAutoEnable(bool value) = 0;

            /// @~chinese 距离高阈值，范围：[1,10000]   @~english Distance High Threshold，range:[1,10000]
            __declspec(property(put = SetDistHigh, get = GetDistHigh)) int DistHigh;

            virtual int GetDistHigh() = 0;

            virtual void SetDistHigh(int value) = 0;

            /// @~chinese 中心点使能   @~english Midpoint Enable
            __declspec(property(put = SetMidPointEnable, get = GetMidPointEnable)) bool MidPointEnable;

            virtual bool GetMidPointEnable() = 0;

            virtual void SetMidPointEnable(bool value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the EdgePairPosTrendAnalyResults.  */
        ///边缘对位置趋势分析结果类
        class EdgePairPosTrendAnalyResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit EdgePairPosTrendAnalyResults() {}
            virtual ~EdgePairPosTrendAnalyResults() {}

        public:
            /**
            * @brief 获取模块状态
            * @return 模块状态
            */
            virtual int GetModuStatus() = 0;


            /**
            * @brief 获取边缘对提取总数
            * @return 边缘对提取总数
            */
            virtual int GetEdgePairCount() = 0;
            /**
            * @brief 获取边缘对最小距离
            * @return 边缘对最小距离
            */
            virtual float GetEdgePairMinDistance() = 0;
            /**
            * @brief 获取边缘对最大距离
            * @return 边缘对最大距离
            */
            virtual float GetEdgePairMaxDistance() = 0;
            /**
            * @brief 获取边缘对平均距离
            * @return 边缘对平均距离
            */
            virtual float GetEdgePairAverageDistance() = 0;
            /**
            * @brief 获取目标边缘对总数
            * @return 目标边缘对总数
            */
            virtual int GetSelEdgePairCount() = 0;
            /**
            * @brief 获取目标边缘对最小距离
            * @return 目标边缘对最小距离
            */
            virtual float GetSelEdgePairMinDistance() = 0;
            /**
            * @brief 获取目标边缘对最大距离
            * @return 目标边缘对最大距离
            */
            virtual float GetSelEdgePairMaxDistance() = 0;
            /**
            * @brief 获取目标边缘对平均距离
            * @return 目标边缘对平均距离
            */
            virtual float GetSelEdgePairAverageDistance() = 0;
            /**
            * @brief 获取卡尺数量
            * @return 卡尺数量
            */
            virtual int GetCaliperCount() = 0;
            /**
            * @brief 获取边缘对最小距离拟合直线点0
            * @return 边缘对最小距离拟合直线点0
            */
            virtual PointF GetMinDistPoint0() = 0;
            /**
            * @brief 获取边缘对最小距离拟合直线点1
            * @return 边缘对最小距离拟合直线点1
            */
            virtual PointF GetMinDistPoint1() = 0;
            /**
            * @brief 获取边缘对最大距离拟合直线点0
            * @return 边缘对最大距离拟合直线点0
            */
            virtual PointF GetMaxDistPoint0() = 0;
            /**
            * @brief 获取边缘对最大距离拟合直线点1
            * @return 边缘对最大距离拟合直线点1
            */
            virtual PointF GetMaxDistPoint1() = 0;

            /**
            * @brief 获取边缘对个数
            * @return 边缘对个数
            */
            virtual int GetEdgePairPointCount() = 0;
            /**
            * @brief 获取边缘点0
            * @param nIndex [IN] 边缘对的索引,[0, 边缘对个数)
            * @return 边缘点0
            */
            virtual PointF GetEdgePoint0(int nIndex) = 0;
            /**
            * @brief 获取边缘点1
            * @param nIndex [IN] 边缘对的索引,[0, 边缘对个数)
            * @return 边缘点1
            */
            virtual PointF GetEdgePoint1(int nIndex) = 0;
            /**
             * @brief 根据索引查询边缘中点
             * @param nIndex     [IN]  边缘中点的索引，[0, EdgePointNum)
             * @param stMidPoint [OUT] 边缘点中点
             * @param nStatus    [OUT] 边缘点中点状态,状态1为正常，0为缺陷
             */
            virtual void GetEdgeMiddlePoint(int nIndex, MVD_POINT_F &stMidPoint, int &nStatus) = 0;
            /**
            * @brief 获取边缘得分
            * @param nIndex [IN] 边缘对的索引,[0, 边缘对个数)
            * @return 边缘得分
            */
            virtual float GetScore(int nIndex) = 0;
            /**
            * @brief 获取边缘0极性
            * @param nIndex [IN] 边缘对的索引,[0, 边缘对个数)
            * @return 边缘0极性
            */
            virtual int GetPolarity0(int nIndex) = 0;
            /**
            * @brief 获取边缘1极性
            * @param nIndex [IN] 边缘对的索引,[0, 边缘对个数)
            * @return 边缘1极性
            */
            virtual int GetPolarity1(int nIndex) = 0;
            /**
            * @brief 获取边缘距离
            * @param nIndex [in] 边缘对的索引,[0, 边缘对个数)
            * @return 边缘距离
            */
            virtual float GetDist(int nIndex) = 0;
            /**
            * @brief 获取边缘点查找状态，1为正常，其他为异常。
            * @param nIndex [IN] 边缘对的索引,[0, 边缘对个数)
            * @return 边缘点查找状态
            */
            virtual int GetStatus(int nIndex) = 0;
            /**
            * @brief 获取ROI区域
            * @return ROI区域信息
            */
            virtual RectBox GetROI() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the EdgePairPosTrendAnaly tool.  */
        ///边缘对位置趋势分析工具类
        class IMVSEdgePairPosTrendAnalyModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSEdgePairPosTrendAnalyModuTool() {}
            virtual ~IMVSEdgePairPosTrendAnalyModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual EdgePairPosTrendAnalyParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual EdgePairPosTrendAnalyResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSEdgePairPosTrendAnalyModuTool(IMVSEdgePairPosTrendAnalyModuTool&);
            IMVSEdgePairPosTrendAnalyModuTool& operator=(IMVSEdgePairPosTrendAnalyModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for EdgePairPosTrendAnaly.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSEdgePairPosTrendAnalyModu::IMVSEdgePairPosTrendAnalyModuTool * __stdcall GetEdgePairPosTrendAnalyToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_EDGEPAIRPOSTRENDANALY_H_
