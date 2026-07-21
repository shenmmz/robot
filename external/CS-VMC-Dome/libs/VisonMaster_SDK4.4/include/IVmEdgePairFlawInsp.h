/***************************************************************************************************
 * File：IVmEdgePairFlawInsp.h
 * Note：@~chinese 边缘对模型缺陷检测模块接口声明 @~english Interface for the EdgePairFlawInsp tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_EDGEPAIRFLAWINSP_H_
#define _IVM_EDGEPAIRFLAWINSP_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSEdgePairFlawInspModu
    {
        /* @enum  MVD_EDGE_PAIR_FLAW_TYPE
         *  @brief Type of the flaw of the edge pair insp
         */
		/// @~chinese 边缘缺陷对类型   @~english Edge flaw pair defect type
        typedef _MVD_EDGEINSP_EDGE_PAIR_FLAW_TYPE__ MVD_EDGE_PAIR_FLAW_TYPE;
        /// @~chinese 边缘查找类型   @~english Edge Search Type
        typedef enum _EdgePairFindModeEnum
        {
            /// @~chinese 最宽边缘对   @~english Widest edge pair
            EdgePairFindMode_Widest = 0x1,

            /// @~chinese 最接近边缘对   @~english Nearest edge pair
            EdgePairFindMode_Nearest = 0x2,

            /// @~chinese 最强最接近边缘对   @~english Strongest & Nearest Edge Pair
            EdgePairFindMode_StrongestNearest = 0x3,

        }EdgePairFindModeEnum;

        /// @~chinese 边缘0极性   @~english Edge 0 polarity
        typedef enum _EdgePairEdge0PolarityEnum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            EdgePairEdge0Polarity_B2W = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            EdgePairEdge0Polarity_W2B = 0x2,

            /// @~chinese 任意极性   @~english Both
            EdgePairEdge0Polarity_All = 0x3,

        }EdgePairEdge0PolarityEnum;

        /// @~chinese 边缘1极性   @~english Edge 1 polarity
        typedef enum _EdgePairEdge1PolarityEnum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            EdgePairEdge1Polarity_B2W = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            EdgePairEdge1Polarity_W2B = 0x2,

            /// @~chinese 任意极性   @~english Both
            EdgePairEdge1Polarity_All = 0x3,

        }EdgePairEdge1PolarityEnum;

    /// \addtogroup 边缘对模型缺陷检测
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the EdgePairFlawInspParams.  */
        ///边缘对模型缺陷检测参数类
        class EdgePairFlawInspParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit EdgePairFlawInspParams() {}
            virtual ~EdgePairFlawInspParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 边缘查找类型   @~english Edge Search Type
            __declspec(property(put = SetEdgePairFindMode, get = GetEdgePairFindMode)) EdgePairFindModeEnum EdgePairFindMode;

            virtual EdgePairFindModeEnum GetEdgePairFindMode() = 0;

            virtual void SetEdgePairFindMode(EdgePairFindModeEnum value) = 0;

            /// @~chinese 卡尺高度，范围：[1,1500]   @~english CaliperHeight，range:[1,1500]
            __declspec(property(put = SetCaliperHeight, get = GetCaliperHeight)) int CaliperHeight;

            virtual int GetCaliperHeight() = 0;

            virtual void SetCaliperHeight(int value) = 0;

            /// @~chinese 卡尺宽度，范围：[1,500]   @~english CaliperWidth，range:[1,500]
            __declspec(property(put = SetCaliperWidth, get = GetCaliperWidth)) int CaliperWidth;

            virtual int GetCaliperWidth() = 0;

            virtual void SetCaliperWidth(int value) = 0;

            /// @~chinese 理想宽度，范围：[1,20000]   @~english Ideal Width，range:[1,20000]
            __declspec(property(put = SetEdgePairIdealWidth, get = GetEdgePairIdealWidth)) int EdgePairIdealWidth;

            virtual int GetEdgePairIdealWidth() = 0;

            virtual void SetEdgePairIdealWidth(int value) = 0;

            /// @~chinese 边缘强度，范围：[1,255]   @~english Edge Intensity，range:[1,255]
            __declspec(property(put = SetEdgeStrength, get = GetEdgeStrength)) int EdgeStrength;

            virtual int GetEdgeStrength() = 0;

            virtual void SetEdgeStrength(int value) = 0;

            /// @~chinese 边缘0极性   @~english Edge 0 polarity
            __declspec(property(put = SetEdgePairEdge0Polarity, get = GetEdgePairEdge0Polarity)) EdgePairEdge0PolarityEnum EdgePairEdge0Polarity;

            virtual EdgePairEdge0PolarityEnum GetEdgePairEdge0Polarity() = 0;

            virtual void SetEdgePairEdge0Polarity(EdgePairEdge0PolarityEnum value) = 0;

            /// @~chinese 边缘1极性   @~english Edge 1 polarity
            __declspec(property(put = SetEdgePairEdge1Polarity, get = GetEdgePairEdge1Polarity)) EdgePairEdge1PolarityEnum EdgePairEdge1Polarity;

            virtual EdgePairEdge1PolarityEnum GetEdgePairEdge1Polarity() = 0;

            virtual void SetEdgePairEdge1Polarity(EdgePairEdge1PolarityEnum value) = 0;

            /// @~chinese 中心点使能   @~english Midpoint Enable
            __declspec(property(put = SetMidPointEnable, get = GetMidPointEnable)) bool MidPointEnable;

            virtual bool GetMidPointEnable() = 0;

            virtual void SetMidPointEnable(bool value) = 0;

            /// @~chinese 缺陷长度阈值，范围：[1,10000]   @~english Defect Length Threshold，range:[1,10000]
            __declspec(property(put = SetLenThresh, get = GetLenThresh)) int LenThresh;

            virtual int GetLenThresh() = 0;

            virtual void SetLenThresh(int value) = 0;

            /// @~chinese 宽度缺陷使能   @~english Width Defect Enable
            __declspec(property(put = SetWidthEnable, get = GetWidthEnable)) bool WidthEnable;

            virtual bool GetWidthEnable() = 0;

            virtual void SetWidthEnable(bool value) = 0;

            /// @~chinese 宽度合格比例，范围：[1,500]   @~english Width Eligibility Ratio，range:[1,500]
            __declspec(property(put = SetWidthLowOffset, get = GetWidthLowOffset)) int WidthLowOffset;

            virtual int GetWidthLowOffset() = 0;

            virtual void SetWidthLowOffset(int value) = 0;

            /// @~chinese 宽度合格比例，范围：[1,500]   @~english Width Eligibility Ratio，range:[1,500]
            __declspec(property(put = SetWidthHighOffset, get = GetWidthHighOffset)) int WidthHighOffset;

            virtual int GetWidthHighOffset() = 0;

            virtual void SetWidthHighOffset(int value) = 0;

            /// @~chinese 位置缺陷使能   @~english Location Defect Enable
            __declspec(property(put = SetOffsetEnable, get = GetOffsetEnable)) bool OffsetEnable;

            virtual bool GetOffsetEnable() = 0;

            virtual void SetOffsetEnable(bool value) = 0;

            /// @~chinese 位置偏移阈值，范围：[1,10000]   @~english Location Deviation Threshold，range:[1,10000]
            __declspec(property(put = SetOffsetThresh, get = GetOffsetThresh)) int OffsetThresh;

            virtual int GetOffsetThresh() = 0;

            virtual void SetOffsetThresh(int value) = 0;

            /// @~chinese 断裂缺陷使能   @~english Fracture Defect Enable
            __declspec(property(put = SetCrackEnable, get = GetCrackEnable)) bool CrackEnable;

            virtual bool GetCrackEnable() = 0;

            virtual void SetCrackEnable(bool value) = 0;

            /// @~chinese 阶梯缺陷使能   @~english Enable Hierarchical Defect
            __declspec(property(put = SetGradEnable, get = GetGradEnable)) bool GradEnable;

            virtual bool GetGradEnable() = 0;

            virtual void SetGradEnable(bool value) = 0;

            /// @~chinese 阶梯偏离高度，范围：[1,10000]   @~english Hierarchy Deviation Height，range:[1,10000]
            __declspec(property(put = SetGradThresh, get = GetGradThresh)) int GradThresh;

            virtual int GetGradThresh() = 0;

            virtual void SetGradThresh(int value) = 0;

            /// @~chinese 最小阶梯长度，范围：[1,10000]   @~english Min. Hierarchy Length，range:[1,10000]
            __declspec(property(put = SetGradLen, get = GetGradLen)) int GradLen;

            virtual int GetGradLen() = 0;

            virtual void SetGradLen(int value) = 0;

            /// @~chinese 气泡缺陷检测   @~english Bubble Defect Detect
            __declspec(property(put = SetBubbleEnable, get = GetBubbleEnable)) bool BubbleEnable;

            virtual bool GetBubbleEnable() = 0;

            virtual void SetBubbleEnable(bool value) = 0;

            /// @~chinese 灰度合格阈值，范围：[1,500]   @~english Grayscale acceptance threshold，range:[1,500]
            __declspec(property(put = SetBubbleLowOffset, get = GetBubbleLowOffset)) int BubbleLowOffset;

            virtual int GetBubbleLowOffset() = 0;

            virtual void SetBubbleLowOffset(int value) = 0;

            /// @~chinese 灰度合格阈值，范围：[1,500]   @~english Grayscale acceptance threshold，range:[1,500]
            __declspec(property(put = SetBubbleHighOffset, get = GetBubbleHighOffset)) int BubbleHighOffset;

            virtual int GetBubbleHighOffset() = 0;

            virtual void SetBubbleHighOffset(int value) = 0;

            /// @~chinese 灰度突变阈值，范围：[10,255]   @~english Grayscale change threshold，range:[10,255]
            __declspec(property(put = SetBubbleChangeThresh, get = GetBubbleChangeThresh)) int BubbleChangeThresh;

            virtual int GetBubbleChangeThresh() = 0;

            virtual void SetBubbleChangeThresh(int value) = 0;

            /// @~chinese 气泡缺陷长度，范围：[0,10000]   @~english Bubble defect length，range:[0,10000]
            __declspec(property(put = SetBubbleLen, get = GetBubbleLen)) int BubbleLen;

            virtual int GetBubbleLen() = 0;

            virtual void SetBubbleLen(int value) = 0;

            /// @~chinese 最大突变次数，范围：[0,100]   @~english Maximum number of mutations，range:[0,100]
            __declspec(property(put = SetBubbleGrayChangeNum, get = GetBubbleGrayChangeNum)) int BubbleGrayChangeNum;

            virtual int GetBubbleGrayChangeNum() = 0;

            virtual void SetBubbleGrayChangeNum(int value) = 0;

            /// @~chinese 灰度辅助检测   @~english Grayscale assisted detection
            __declspec(property(put = SetGrayTrackEnable, get = GetGrayTrackEnable)) bool GrayTrackEnable;

            virtual bool GetGrayTrackEnable() = 0;

            virtual void SetGrayTrackEnable(bool value) = 0;

            /// @~chinese 滤波尺寸，范围：[1,50]   @~english Filter Size，range:[1,50]
            __declspec(property(put = SetHalfKernelSize, get = GetHalfKernelSize)) int HalfKernelSize;

            virtual int GetHalfKernelSize() = 0;

            virtual void SetHalfKernelSize(int value) = 0;

            /// @~chinese 追踪容忍，范围：[0,100]   @~english Track Distol，range:[0,100]
            __declspec(property(put = SetGrayTrackDistol, get = GetGrayTrackDistol)) int GrayTrackDistol;

            virtual int GetGrayTrackDistol() = 0;

            virtual void SetGrayTrackDistol(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes for describing edge pair width info.  */
		///边缘对模型缺陷宽度信息
        class IEdgePairWidthInfo
        {
        protected:
            // Constructor and Destructor
            explicit IEdgePairWidthInfo() {}
            virtual ~IEdgePairWidthInfo() {}

        public:
            // Property
            /**
             * @brief 最大宽度
             */
            virtual float GetMaxWidth() = 0;
            /**
             * @brief 最小宽度
             */
            virtual float GetMinWidth() = 0;
            /**
             * @brief 平均宽度
             */
            virtual float GetAverageWidth() = 0;
            /**
             * @brief 边缘对的数量
             */
            virtual int GetNumOfEdgePair() = 0;
            /**
             * @brief 获取边缘对宽度信息
             * @return 边缘对宽度信息数组的首地址,数组长度为NumOfEdgePair
             */
            virtual float* GetEdgePairWidthArray() = 0;
        };

        /*  Note: Interface Classes for describing single match info.  */
		///边缘对模型缺陷信息
        class IEdgePairFlawInfo
        {
        protected:
            // Constructor and Destructor
            explicit IEdgePairFlawInfo() {}
            virtual ~IEdgePairFlawInfo() {}

        public:

            /**
             * @brief 缺陷类型
             */
            virtual MVD_EDGE_PAIR_FLAW_TYPE GetFlawType() = 0;
            /**
             * @brief 缺陷包围框
             */
            virtual RectBox GetFlawBox() = 0;
            /**
             * @brief 缺陷长度
             */
            virtual float GetFlawLength() = 0;
            /**
             * @brief 缺陷所在的卡尺的起始索引
             */
            virtual int GetCaliperStartIndex() = 0;
            /**
             * @brief 缺陷所在的卡尺的结束索引
             */
            virtual int GetCaliperEndIndex() = 0;
        };

        /*  Note: Interface Classes of the result of the EdgePairFlawInspResults.  */
        ///边缘对模型缺陷检测结果类
        class EdgePairFlawInspResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit EdgePairFlawInspResults() {}
            virtual ~EdgePairFlawInspResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 缺陷数量
             */
            virtual int GetFlawNum() = 0;
            /**
             * @brief 根据索引获取缺陷的信息
             * @param nFlawIndex [IN] 缺陷的索引，[0, FlawNum)
             * @return 无，出错时抛出异常
             */
            virtual IEdgePairFlawInfo* GetFlawInfo( int nFlawIndex ) = 0;
            /**
             * @brief 获取边缘对宽度信息
             * @return 无，出错时抛出异常
             */
            virtual IEdgePairWidthInfo* GetFlawWidthInfo() = 0;
            /**
             * @brief 获取边缘点的数量
             * @return 边缘点个数
             */
            virtual int GetEdgePointNum() = 0;
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
             * @brief 根据索引查询边缘中点
             * @param nIndex     [IN]  边缘中点的索引，[0, EdgePointNum)
             * @param stMidPoint [OUT] 边缘点中点
             * @param nStatus    [OUT] 边缘点中点状态,状态1为正常，0为缺陷
             */
            virtual void GetEdgeMiddlePoint(int nIndex, MVD_POINT_F &stMidPoint, int &nStatus) = 0;
            /**
             * @brief 获取卡尺数量
             * @return 卡尺数量
             */
            virtual int GetCaliperNum() = 0;
            /**
             * @brief 获取卡尺中心点
             * @param nIndex [IN] 卡尺box的索引[0, CaliperNum)
             * @return 卡尺中心点
             */
            virtual MVD_POINT_F GetCaliperCenterPoint(int nIndex) = 0;
            /**
             * @brief 获取卡尺box
             * @param nIndex [IN] 卡尺box的索引[0, CaliperNum)
             * @return 卡尺
             */
            virtual RectBox GetCaliperBox(int nIndex) = 0;

            /**
             * @brief 人工绘制每条轨迹的点个数 
             * @param nTrajIndex [IN] 轨迹索引,[0, IdeaTrajNum)
             */
            virtual int GetTrajPtsNum(int nTrajIndex) = 0;

            /**
             * @brief 获取特定卡尺 
             * @param nTrajCaliperIndex [IN] 卡尺索引,[0, sum(TrajPtsNum))
             */
            virtual RectBox GetIdeaTrajCaliper(int nTrajCaliperIndex) = 0;

            /**
             * @brief 获取理想边缘点0信息
             * @param nIndex [IN] 边缘点索引,[0, EdgePointNum)
             * @return 边缘0的边缘点
             */
            virtual MVD_POINT_F GetIdeaEdge0Point(int nIndex) = 0;

            /**
             * @brief 获取理想边缘点1信息
             * @param nIndex [IN] 边缘点索引,[0, EdgePointNum)
             * @return 边缘1的边缘点
             */
            virtual MVD_POINT_F GetIdeaEdge1Point(int nIndex) = 0;

            /**
             * @brief 获取人工轨迹信息
             * @param nIndex [IN] 人工轨迹索引,[0, sum(TrajPtsNum))
             * @return 边缘1的边缘点
             */
            virtual MVD_POINT_F GetIdeaTraj(int nIndex) = 0;

            /**
             * @brief 根据索引获取优化轨迹的信息
             * @param nCalTrajIndex [IN] 优化轨迹的索引，[0, sum(TrajPtsNum))
			 * @param stCalTraj [OUT]优化轨迹信息CalTrajFlag [out]优化轨迹状态
			 * @param CalTrajFlag [OUT]优化轨迹状态
             * @return 返回索引处轨迹信息，当前仅支持一条即nPolygonIndex为0
             */
            virtual void GetCalTraj(int nCalTrajIndex ,MVD_POINT_F &stCalTraj, int &CalTrajFlag) = 0;

            /**
             * @brief 人工绘制轨迹条数
             */
            virtual int GetIdeaTrajNum() = 0;

        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the EdgePairFlawInsp tool.  */
        ///边缘对模型缺陷检测工具类
        class IMVSEdgePairFlawInspModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSEdgePairFlawInspModuTool() {}
            virtual ~IMVSEdgePairFlawInspModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual EdgePairFlawInspParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual EdgePairFlawInspResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

            /**
            * @brief 导入模型
            * @param 模型路径
            * @return 无，出错时抛出异常
            * @remarks 导入模型
            */
            virtual void ImportModelData(IN const char * const strPath) = 0;
        private:
            IMVSEdgePairFlawInspModuTool(IMVSEdgePairFlawInspModuTool&);
            IMVSEdgePairFlawInspModuTool& operator=(IMVSEdgePairFlawInspModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for EdgePairFlawInsp.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSEdgePairFlawInspModu::IMVSEdgePairFlawInspModuTool * __stdcall GetEdgePairFlawInspToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_EDGEPAIRFLAWINSP_H_
