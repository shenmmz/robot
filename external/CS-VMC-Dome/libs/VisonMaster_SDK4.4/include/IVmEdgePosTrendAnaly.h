/***************************************************************************************************
 * File：IVmEdgePosTrendAnaly.h
 * Note：@~chinese 边缘位置趋势分析模块接口声明 @~english Interface for the EdgePosTrendAnaly tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_EDGEPOSTRENDANALY_H_
#define _IVM_EDGEPOSTRENDANALY_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSEdgePosTrendAnalyModu
    {
        /// @~chinese 边缘类型   @~english Edge Type
        typedef enum _FindModeEnum
        {
            /// @~chinese 最强   @~english Strongest Edge
            FindMode_Best = 0x1,

            /// @~chinese 第一条边缘   @~english Fisrt Edge
            FindMode_First = 0x2,

            /// @~chinese 最后一条边缘   @~english Last Edge
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

        /// @~chinese 边缘查找方向   @~english Edge Search Direction
        typedef enum _DetechOrientEnum
        {
            /// @~chinese 从上到下   @~english Top to Bottom
            DetechOrient_T2B = 0x1,

            /// @~chinese 从左到右   @~english Left to Right
            DetechOrient_L2R = 0x2,

        }DetechOrientEnum;

    /// \addtogroup 边缘位置趋势分析
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///边缘位置趋势分析ROI管理类
        class EdgePosTrendAnalyRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the EdgePosTrendAnalyParams.  */
        ///边缘位置趋势分析参数类
        class EdgePosTrendAnalyParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit EdgePosTrendAnalyParams() {}
            virtual ~EdgePosTrendAnalyParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual EdgePosTrendAnalyRoiManager* GetModuRoiManager() = 0;

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

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the EdgePosTrendAnalyResults.  */
        ///边缘位置趋势分析结果类
        class EdgePosTrendAnalyResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit EdgePosTrendAnalyResults() {}
            virtual ~EdgePosTrendAnalyResults() {}

        public:
            /**
            * @brief 获取模块状态
            * @return 模块状态
            */
             virtual int GetModuStatus() = 0;

            /**
            * @brief 获取边缘点提取总数
            * @return 边缘点提取总数
            */
            virtual int GetEdgePosCount() = 0;
            /**
            * @brief 获取边缘点最小距离
            * @return 边缘点最小距离
            */
            virtual float GetEdgePosMinDistance() = 0;
            /**
            * @brief 获取边缘点最大距离
            * @return 边缘点最大距离
            */
            virtual float GetEdgePosMaxDistance() = 0;
            /**
            * @brief 获取边缘点平均距离
            * @return 边缘点平均距离
            */
            virtual float GetEdgePosAverageDistance() = 0;
            /**
            * @brief 获取目标边缘点总数
            * @return 目标边缘点总数
            */
            virtual int GetSelEdgePosCount() = 0;
            /**
            * @brief 获取目标边缘点最小距离
            * @return 目标边缘点最小距离
            */
            virtual float GetSelEdgePosMinDistance() = 0;
            /**
            * @brief 获取目标边缘点最大距离
            * @return 目标边缘点最大距离
            */
            virtual float GetSelEdgePosMaxDistance() = 0;
            /**
            * @brief 获取目标边缘点平均距离
            * @return 目标边缘点平均距离
            */
            virtual float GetSelEdgePosAverageDistance() = 0;
            /**
            * @brief 获取卡尺数量
            * @return 卡尺数量
            */
            virtual int GetCaliperCount() = 0;
            /**
            * @brief 获取边缘点最小距离索引点
            * @return 边缘点最小距离索引点
            */
            virtual PointF GetMinDistPoint() = 0;
            /**
            * @brief 获取边缘点最大距离索引点
            * @return 边缘点最大距离索引点
            */
            virtual PointF GetMaxDistPoint() = 0;

            /**
            * @brief 获取边缘点个数
            * @return 边缘点个数
            */
            virtual int GetEdgePointCount() = 0;
            /**
            * @brief 获取边缘点
            * @param nIndex [IN] 边缘点的索引,[0, 边缘点个数)
            * @return 边缘点
            */
            virtual PointF GetEdgePoint(int nIndex) = 0;
            /**
            * @brief 获取边缘得分
            * @param nIndex [IN] 边缘点的索引,[0, 边缘点个数)
            * @return 边缘得分
            */
            virtual float GetScore(int nIndex) = 0;
            /**
            * @brief 获取边缘极性
            * @param nIndex [IN] 边缘点的索引,[0, 边缘点个数)
            * @return 边缘极性
            */
            virtual int GetPolarity(int nIndex) = 0;
            /**
            * @brief 获取边缘距离
            * @param nIndex [IN] 边缘点的索引,[0, 边缘点个数)
            * @return 边缘距离
            */
            virtual float GetDist(int nIndex) = 0;
            /**
            * @brief 获取边缘点查找状态，1为正常，其他为异常。
            * @param nIndex [IN] 边缘点的索引,[0, 边缘点个数)
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
        /*  Note: Note: This is the class for the EdgePosTrendAnaly tool.  */
        ///边缘位置趋势分析工具类
        class IMVSEdgePosTrendAnalyModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSEdgePosTrendAnalyModuTool() {}
            virtual ~IMVSEdgePosTrendAnalyModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual EdgePosTrendAnalyParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual EdgePosTrendAnalyResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSEdgePosTrendAnalyModuTool(IMVSEdgePosTrendAnalyModuTool&);
            IMVSEdgePosTrendAnalyModuTool& operator=(IMVSEdgePosTrendAnalyModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for EdgePosTrendAnaly.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSEdgePosTrendAnalyModu::IMVSEdgePosTrendAnalyModuTool * __stdcall GetEdgePosTrendAnalyToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_EDGEPOSTRENDANALY_H_
