/***************************************************************************************************
 * File：IVmEdgeFlawInsp.h
 * Note：@~chinese 边缘模型缺陷检测模块接口声明 @~english Interface for the EdgeFlawInsp tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_EDGEFLAWINSP_H_
#define _IVM_EDGEFLAWINSP_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSEdgeFlawInspModu
    {
        /* @enum  MVD_SINGLE_EDGE_FLAW_TYPE
         *  @brief Type of the flaw of the edge inspection
         */
		/// @~chinese 边缘缺陷类型   @~english Type of the flaw of the edge inspection
        typedef _MVD_EDGEINSP_SINGLE_EDGE_FLAW_TYPE__ MVD_SINGLE_EDGE_FLAW_TYPE;
        /// @~chinese 边缘类型   @~english Edge Type
        typedef enum _EdgePointFindModeEnum
        {
            /// @~chinese 最强   @~english Strongest Edge
            EdgePointFindMode_Best = 0x1,

            /// @~chinese 第一条   @~english First Edge
            EdgePointFindMode_First = 0x2,

            /// @~chinese 最后一条   @~english Last Edge
            EdgePointFindMode_Last = 0x3,

        }EdgePointFindModeEnum;

        /// @~chinese 边缘极性   @~english EdgePolarity
        typedef enum _EdgePointPolarityEnum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            EdgePointPolarity_B2W = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            EdgePointPolarity_W2B = 0x2,

            /// @~chinese 任意极性   @~english Both
            EdgePointPolarity_All = 0x3,

        }EdgePointPolarityEnum;

    /// \addtogroup 边缘模型缺陷检测
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the EdgeFlawInspParams.  */
        ///边缘模型缺陷检测参数类
        class EdgeFlawInspParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit EdgeFlawInspParams() {}
            virtual ~EdgeFlawInspParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 边缘类型   @~english Edge Type
            __declspec(property(put = SetEdgePointFindMode, get = GetEdgePointFindMode)) EdgePointFindModeEnum EdgePointFindMode;

            virtual EdgePointFindModeEnum GetEdgePointFindMode() = 0;

            virtual void SetEdgePointFindMode(EdgePointFindModeEnum value) = 0;

            /// @~chinese 边缘极性   @~english EdgePolarity
            __declspec(property(put = SetEdgePointPolarity, get = GetEdgePointPolarity)) EdgePointPolarityEnum EdgePointPolarity;

            virtual EdgePointPolarityEnum GetEdgePointPolarity() = 0;

            virtual void SetEdgePointPolarity(EdgePointPolarityEnum value) = 0;

            /// @~chinese 边缘强度，范围：[1,255]   @~english Edge Intensity，range:[1,255]
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

            /// @~chinese 缺陷长度阈值，范围：[1,10000]   @~english Defect Length Threshold，range:[1,10000]
            __declspec(property(put = SetLenThresh, get = GetLenThresh)) int LenThresh;

            virtual int GetLenThresh() = 0;

            virtual void SetLenThresh(int value) = 0;

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
        /*  Note: Interface classes for describing each edge flaw information.  */
        ///边缘缺陷信息类
        class IEdgeFlawInfo
        {
        protected:
            // Constructor and Destructor
            explicit IEdgeFlawInfo() {}
            virtual ~IEdgeFlawInfo() {}

        public:
            /**
             * @brief 缺陷类型
             */
            virtual MVD_SINGLE_EDGE_FLAW_TYPE GetFlawType() = 0;
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

        /*  Note: Interface Classes of the result of the EdgeFlawInspResults.  */
        ///边缘模型缺陷检测结果类
        class EdgeFlawInspResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit EdgeFlawInspResults() {}
            virtual ~EdgeFlawInspResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 缺陷数量
             */
            virtual int GetEdgeFlawNum() = 0;
            /**
             * @brief 根据索引获取缺陷的信息
             * @param nIndex [IN] 缺陷的索引,[0, FlawNum)
             * @return 单缺陷信息
             */
            virtual IEdgeFlawInfo* GetEdgeFlawInfo(int nIndex) = 0;
            /**
             * @brief 获取边缘点的数量
             * @return 边缘点个数
             */
            virtual int GetEdgePointNum() = 0;
            /**
            * @brief 根据索引查询边缘点
            * @param nIndex  [IN]  边缘点的索引，[0, EdgePointNum)
            * @param stPoint [OUT] 边缘点
            * @param nStatus [OUT] 边缘点状态,状态1为正常，0为缺陷
            */
            virtual void GetEdgePoint(int nIndex, MVD_POINT_F &stPoint, int &nStatus) = 0;
            /**
             * @brief 获取卡尺数量
             * @return 卡尺数量
             */
            virtual int GetCaliperNum() = 0;
            /**
             * @brief 根据索引获取卡尺信息
             * @param nIndex [IN] 卡尺索引,[0, Caliper)
             * @return 单卡尺信息
             */
            virtual RectBox GetCaliperBox(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the EdgeFlawInsp tool.  */
        ///边缘模型缺陷检测工具类
        class IMVSEdgeFlawInspModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSEdgeFlawInspModuTool() {}
            virtual ~IMVSEdgeFlawInspModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual EdgeFlawInspParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual EdgeFlawInspResults* GetResult() = 0;

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
            IMVSEdgeFlawInspModuTool(IMVSEdgeFlawInspModuTool&);
            IMVSEdgeFlawInspModuTool& operator=(IMVSEdgeFlawInspModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for EdgeFlawInsp.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSEdgeFlawInspModu::IMVSEdgeFlawInspModuTool * __stdcall GetEdgeFlawInspToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_EDGEFLAWINSP_H_
