/***************************************************************************************************
 * File：IVmEdgeInspGroup.h
 * Note：@~chinese 边缘组合缺陷检测模块接口声明 @~english Interface for the EdgeInspGroup tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_EDGEINSPGROUP_H_
#define _IVM_EDGEINSPGROUP_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSEdgeInspGroupModu
    {
    /// \addtogroup 边缘组合缺陷检测
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the EdgeInspGroupParams.  */
        ///边缘组合缺陷检测参数类
        class EdgeInspGroupParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit EdgeInspGroupParams() {}
            virtual ~EdgeInspGroupParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /* @enum  MVD_EDGEINSP_FLAW_DEFECT_TYPE
         *  @brief 边缘缺陷类型
         */
        /// @~chinese 边缘缺陷类型   @~english Edge flaw defect type
        typedef _MVD_EDGEINSP_FLAW_DEFECT_TYPE__ MVD_EDGEINSP_FLAW_DEFECT_TYPE;
        
        /// 边缘组合缺陷检测缺陷信息类
        class IEdgeFlawGroupInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IEdgeFlawGroupInfo() {}
            virtual ~IEdgeFlawGroupInfo() {}

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

        /*  Note: Interface Classes of the result of the EdgeInspGroupResults.  */
        ///边缘组合缺陷检测结果类
        class EdgeInspGroupResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit EdgeInspGroupResults() {}
            virtual ~EdgeInspGroupResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
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
            virtual IEdgeFlawGroupInfo* GetFlawInfo(int nIndex) = 0;
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
             * @brief 获取卡尺数量
             * @return 卡尺数量
             */
            virtual int GetCaliperNum() = 0;
            /**
             * @brief 根据索引获取卡尺信息（预留）
             * @param nIndex [IN] 卡尺索引,[0, CaliperNum)
             * @return 单卡尺信息
             */
            virtual RectBox GetCaliperBox(int nIndex) = 0;
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
            /**
             * @brief 获取内部拟合圆个数
             * @return 拟合圆个数
             */
            virtual int GetArcNum() = 0;
            /**
             * @par 说明
             * 获取拟合圆弧信息 圆弧等价于内外径相等的扇环  边检测为1条
             */

            virtual Circle GetArc(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the EdgeInspGroup tool.  */
        ///边缘组合缺陷检测工具类
        class IMVSEdgeInspGroupModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSEdgeInspGroupModuTool() {}
            virtual ~IMVSEdgeInspGroupModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual EdgeInspGroupParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual EdgeInspGroupResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSEdgeInspGroupModuTool(IMVSEdgeInspGroupModuTool&);
            IMVSEdgeInspGroupModuTool& operator=(IMVSEdgeInspGroupModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for EdgeInspGroup.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSEdgeInspGroupModu::IMVSEdgeInspGroupModuTool * __stdcall GetEdgeInspGroupToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_EDGEINSPGROUP_H_
