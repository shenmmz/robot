/***************************************************************************************************
 * File：IVmGluePathConduct.h
 * Note：@~chinese 路径提取模块接口声明 @~english Interface for the GluePathConduct tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_GLUEPATHCONDUCT_H_
#define _IVM_GLUEPATHCONDUCT_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSGluePathConductModu
    {
        /// @~chinese 提取方式   @~english Extraction Method
        typedef enum _ModPointModeEnum
        {
            /// @~chinese 等间隔取点   @~english Take points at equal intervals
            ModPointMode_Best = 0x0,

            /// @~chinese 查找边缘点   @~english Find edge points
            ModPointMode_First = 0x1,

        }ModPointModeEnum;

    /// \addtogroup 路径提取
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the GluePathConductParams.  */
        ///路径提取参数类
        class GluePathConductParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit GluePathConductParams() {}
            virtual ~GluePathConductParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 提取方式   @~english Extraction Method
            __declspec(property(put = SetModPointMode, get = GetModPointMode)) ModPointModeEnum ModPointMode;

            virtual ModPointModeEnum GetModPointMode() = 0;

            virtual void SetModPointMode(ModPointModeEnum value) = 0;

            /// @~chinese 输出圆弧信息   @~english Output Arc Params
            __declspec(property(put = SetArcParamEnable, get = GetArcParamEnable)) bool ArcParamEnable;

            virtual bool GetArcParamEnable() = 0;

            virtual void SetArcParamEnable(bool value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /** @enum  MVD_EDGEPOINT_STATUS
        *  @brief Type of the polarity of the edge point
        */
		/// @~chinese 边缘极性 @~english edge polarity
        typedef _MVD_EDGE_POLARITY__ MVD_EDGE_POLARITY;

        /** @enum  MVD_EDGEPOINT_STATUS
        *  @brief Find status of the edge point
        */
		/// @~chinese 边缘点状态 @~english edge point status
        typedef _MVD_EDGEPOINT_STATUS__ MVD_EDGEPOINT_STATUS;

        /// 边缘点信息
        class IBeadGuideEdgePointInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IBeadGuideEdgePointInfo() {}
            virtual ~IBeadGuideEdgePointInfo() {}

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
        /*  Note: Interface Classes of the result of the GluePathConductResults.  */
        ///路径提取结果类
        class GluePathConductResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit GluePathConductResults() {}
            virtual ~GluePathConductResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 获取边缘点的数量
             * @return 边缘点个数
             */
            virtual int GetEdgePointNum() = 0;
            /**
             * @brief 根据索引获取边缘点的信息
             * @param nIndex [IN] 边缘点的索引,[0, EdgePointNum)
             * @return 单边缘点信息
             */
            virtual IBeadGuideEdgePointInfo* GetEdgePointInfo(int nIndex) = 0;
            /**
             * @brief 根据索引获取边缘点类型
             * @param nIndex [IN] 边缘点的索引,[0, EdgePointNum)
             * @return 边缘点类型
             */
            virtual int GetGluePointTypes(int nIndex) = 0;
            /**
             * @brief 获取路径1边缘点的数量
             * @return 边缘点个数
             */
            virtual int GetEdgePoint1Num() = 0;
            /**
             * @brief 根据索引获取路径1边缘点的信息
             * @param nIndex [IN] 边缘点的索引,[0, EdgePointNum)
             * @return 单边缘点信息
             */
            virtual IBeadGuideEdgePointInfo* GetEdgePoint1Info(int nIndex) = 0;
            /**
             * @brief 获取路径2边缘点的数量
             * @return 边缘点个数
             */
            virtual int GetEdgePoint2Num() = 0;
            /**
             * @brief 根据索引获取路径2边缘点的信息
             * @param nIndex [IN] 边缘点的索引,[0, EdgePointNum)
             * @return 单边缘点信息
             */
            virtual IBeadGuideEdgePointInfo* GetEdgePoint2Info(int nIndex) = 0;
            /**
             * @brief 获取路径3边缘点的数量
             * @return 边缘点个数
             */
            virtual int GetEdgePoint3Num() = 0;
            /**
             * @brief 根据索引获取路径3边缘点的信息
             * @param nIndex [IN] 边缘点的索引,[0, EdgePointNum)
             * @return 单边缘点信息
             */
            virtual IBeadGuideEdgePointInfo* GetEdgePoint3Info(int nIndex) = 0;
            /**
             * @brief 获取路径4边缘点的数量
             * @return 边缘点个数
             */
            virtual int GetEdgePoint4Num() = 0;
            /**
             * @brief 根据索引路径4获取边缘点的信息
             * @param nIndex [IN] 边缘点的索引,[0, EdgePointNum)
             * @return 单边缘点信息
             */
            virtual IBeadGuideEdgePointInfo* GetEdgePoint4Info(int nIndex) = 0;
            /**
             * @brief 获取路径5边缘点的数量
             * @return 边缘点个数
             */
            virtual int GetEdgePoint5Num() = 0;
            /**
             * @brief 根据索引获取路径5边缘点的信息
             * @param nIndex [IN] 边缘点的索引,[0, EdgePointNum)
             * @return 单边缘点信息
             */
            virtual IBeadGuideEdgePointInfo* GetEdgePoint5Info(int nIndex) = 0;
            /**
             * @brief 获取路径6边缘点的数量
             * @return 边缘点个数
             */
            virtual int GetEdgePoint6Num() = 0;
            /**
             * @brief 根据索引获取路径6边缘点的信息
             * @param nIndex [IN] 边缘点的索引,[0, EdgePointNum)
             * @return 单边缘点信息
             */
            virtual IBeadGuideEdgePointInfo* GetEdgePoint6Info(int nIndex) = 0;
            /**
             * @brief 获取路径7边缘点的数量
             * @return 边缘点个数
             */
            virtual int GetEdgePoint7Num() = 0;
            /**
             * @brief 根据索引获取路径7边缘点的信息
             * @param nIndex [IN] 边缘点的索引,[0, EdgePointNum)
             * @return 单边缘点信息
             */
            virtual IBeadGuideEdgePointInfo* GetEdgePoint7Info(int nIndex) = 0;
            /**
             * @brief 获取路径8边缘点的数量
             * @return 边缘点个数
             */
            virtual int GetEdgePoint8Num() = 0;
            /**
             * @brief 根据索引获取路径8边缘点的信息
             * @param nIndex [IN] 边缘点的索引,[0, EdgePointNum)
             * @return 单边缘点信息
             */
            virtual IBeadGuideEdgePointInfo* GetEdgePoint8Info(int nIndex) = 0;
            /**
             * @brief 获取路径9边缘点的数量
             * @return 边缘点个数
             */
            virtual int GetEdgePoint9Num() = 0;
            /**
             * @brief 根据索引获取路径9边缘点的信息
             * @param nIndex [IN] 边缘点的索引,[0, EdgePointNum)
             * @return 单边缘点信息
             */
            virtual IBeadGuideEdgePointInfo* GetEdgePoint9Info(int nIndex) = 0;
            /**
             * @brief 获取路径10边缘点的数量
             * @return 边缘点个数
             */
            virtual int GetEdgePoint10Num() = 0;
            /**
             * @brief 根据索引获取路径10边缘点的信息
             * @param nIndex [IN] 边缘点的索引,[0, EdgePointNum)
             * @return 单边缘点信息
             */
            virtual IBeadGuideEdgePointInfo* GetEdgePoint10Info(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the GluePathConduct tool.  */
        ///路径提取工具类
        class IMVSGluePathConductModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSGluePathConductModuTool() {}
            virtual ~IMVSGluePathConductModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual GluePathConductParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual GluePathConductResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

            /**
            * @brief 导入模型
            * @param strPath [IN] 模型路径
            * @return 无，出错时抛出异常
            * @remarks 导入模型
            */
            virtual void ImportModelData(IN const char * const strPath) = 0;
        private:
            IMVSGluePathConductModuTool(IMVSGluePathConductModuTool&);
            IMVSGluePathConductModuTool& operator=(IMVSGluePathConductModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for GluePathConduct.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSGluePathConductModu::IMVSGluePathConductModuTool * __stdcall GetGluePathConductToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_GLUEPATHCONDUCT_H_
