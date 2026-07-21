/***************************************************************************************************
 * File：IVmEdgePairInspGroup.h
 * Note：@~chinese 边缘对组合缺陷检测模块接口声明 @~english Interface for the EdgePairInspGroup tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_EDGEPAIRINSPGROUP_H_
#define _IVM_EDGEPAIRINSPGROUP_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSEdgePairInspGroupModu
    {
    /// \addtogroup 边缘对组合缺陷检测
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the EdgePairInspGroupParams.  */
        ///边缘对组合缺陷检测参数类
        class EdgePairInspGroupParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit EdgePairInspGroupParams() {}
            virtual ~EdgePairInspGroupParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 中心点使能   @~english Midpoint Enable
            __declspec(property(put = SetMidPointEnable, get = GetMidPointEnable)) bool MidPointEnable;

            virtual bool GetMidPointEnable() = 0;

            virtual void SetMidPointEnable(bool value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /* @enum  MVD_EDGEINSP_FLAW_DEFECT_TYPE
         *  @brief 边缘对组合缺陷类型
         */
        /// @~chinese 边缘对缺陷类型   @~english Edge Insp Flaw Defect Type
        typedef _MVD_EDGEINSP_FLAW_DEFECT_TYPE__ MVD_EDGEINSP_FLAW_DEFECT_TYPE;
        
		///边缘对组合缺陷检测缺陷信息
        class IEdgePairFlawGroupInfo
        {
        protected:
            // 构造与析构函数
            explicit IEdgePairFlawGroupInfo() {}
            virtual ~IEdgePairFlawGroupInfo() {}

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
        
        /*  Note: Interface Classes of the result of the EdgePairInspGroupResults.  */
        ///边缘对组合缺陷检测结果类
        class EdgePairInspGroupResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit EdgePairInspGroupResults() {}
            virtual ~EdgePairInspGroupResults() {}

        public:
             /**
             * @brief 缺陷综合信息
             * @param nIndex[IN] 数据索引
             */
             virtual DefectFixture GetDefectGeneralInfo(int nIndex) = 0;

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
             * @param nIndex [IN] 缺陷的索引,[0, FlawNum)
             * @return 单缺陷信息
             */
            virtual IEdgePairFlawGroupInfo* GetFlawInfo(int nIndex) = 0;
            /**
             * @brief 获取边缘0点的数量
             * @return 边缘点个数
             */
            virtual int GetEdge0PointNum() = 0;
            /**
             * @brief 根据索引查询边缘点0
             * @param nIndex  [IN]  边缘点0的索引，[0, EdgePointNum)
             * @param stPoint0 [OUT] 边缘点0
             * @param nStatus [OUT] 边缘点0状态,状态1为正常，0为缺陷
             */
            virtual void GetEdge0Point(int nIndex, MVD_POINT_F &stPoint0, int &nStatus) = 0;
            /**
             * @brief 获取边缘1点的数量
             * @return 边缘点个数
             */
            virtual int GetEdge1PointNum() = 0;
            /**
             * @brief 根据索引查询边缘点1
             * @param nIndex  [IN]  边缘点1的索引，[0, EdgePointNum)
             * @param stPoint1 [OUT] 边缘点1
             * @param nStatus [OUT] 边缘点1状态,状态1为正常，0为缺陷
             */
            virtual void GetEdge1Point(int nIndex, MVD_POINT_F &stPoint1, int &nStatus) = 0;
			/**
             * @brief 获取中点的数量
             * @return 中点个数
             */
            virtual int GetEdgeMidPointNum() = 0;
            /**
             * @brief 根据索引查询边缘中点
             * @param nIndex     [IN]  边缘中点的索引，[0, EdgeMidPointNum)
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
             * @brief 获取卡尺点信息  个数为当前卡尺个数
             * @param nIndex   [IN]   卡尺点序号，[0, CaliperNum)
             * @param stPoint  [OUT]  卡尺点
             * @param nStatus  [OUT]  卡尺点状态,状态1为正常，0为缺陷
             */
            virtual void GetCaliperIdeaPoint(int nIndex, MVD_POINT_F &stPoint, int &nStatus) = 0;
            /**
             * @brief 最小宽度
             * @deprecated V4.4废弃
             */
            virtual float GetMinWidth() = 0;
            /**
             * @brief 最大宽度
             * @deprecated V4.4废弃
             */
            virtual float GetMaxWidth() = 0;
            /**
             * @brief 平均宽度
             * @deprecated V4.4废弃
             */
            virtual float GetAverageWidth() = 0;
            /**
             * @brief 近似面积
             * @deprecated V4.4废弃
             */
            virtual float GetApproximateArea() = 0;
            /**
             * @brief 获取内部拟合直线个数
             * @return 直线个数
             */
            virtual int GetLineNum() = 0;
            /**
             * @brief 根据索引获取内部拟合直线
             * @param nIndex [IN] 直线索引,[0, LineNum)
             * @return 内部拟合输出的直线
             */
            virtual Line GetInnerFitLine(int nIndex) = 0;
            /**
             * @brief 获取拟合圆弧信息 圆弧等价于内外径相等的扇环
             * @param nArcIndex [IN] 圆弧序号
             */

            virtual Circle GetArc(int nArcIndex) = 0;
            /**
             * @brief 获取拟合圆弧数目
             */

            virtual int GetArcNum() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the EdgePairInspGroup tool.  */
        ///边缘对组合缺陷检测工具类
        class IMVSEdgePairInspGroupModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSEdgePairInspGroupModuTool() {}
            virtual ~IMVSEdgePairInspGroupModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual EdgePairInspGroupParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual EdgePairInspGroupResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSEdgePairInspGroupModuTool(IMVSEdgePairInspGroupModuTool&);
            IMVSEdgePairInspGroupModuTool& operator=(IMVSEdgePairInspGroupModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for EdgePairInspGroup.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSEdgePairInspGroupModu::IMVSEdgePairInspGroupModuTool * __stdcall GetEdgePairInspGroupToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_EDGEPAIRINSPGROUP_H_
