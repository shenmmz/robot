/***************************************************************************************************
 * File：IVmPointSet.h
 * Note：@~chinese 点集模块接口声明 @~english Interface for the PointSet tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_POINTSET_H_
#define _IVM_POINTSET_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace PointSetMODU_STD
    {
    /// \addtogroup 点集
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the PointSetParams.  */
        ///点集参数类
        class PointSetParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit PointSetParams() {}
            virtual ~PointSetParams() {}

        public:

            /// @~chinese 循环使能   @~english Loop Enable
            __declspec(property(put = SetLoopEnabled, get = GetLoopEnabled)) bool LoopEnabled;

            virtual bool GetLoopEnabled() = 0;

            virtual void SetLoopEnabled(bool value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the PointSetResults.  */
        ///点集结果类
        class PointSetResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit PointSetResults() {}
            virtual ~PointSetResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
 			/**
             * @brief 获取点数量
             */
            virtual int GetPointsNum() = 0;

			/**
             * @brief 获取点集
             */
            virtual MVD_POINT_F GetPointsAssemble(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the PointSet tool.  */
        ///点集工具类
        class PointSetMODU_STDTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit PointSetMODU_STDTool() {}
            virtual ~PointSetMODU_STDTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual PointSetParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual PointSetResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            PointSetMODU_STDTool(PointSetMODU_STDTool&);
            PointSetMODU_STDTool& operator=(PointSetMODU_STDTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for PointSet.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::PointSetMODU_STD::PointSetMODU_STDTool * __stdcall GetPointSetToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_POINTSET_H_
