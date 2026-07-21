/***************************************************************************************************
 * File：IVmBoxOverlapCalculation.h
 * Note：@~chinese Box重叠率计算模块接口声明 @~english Interface for the BoxOverlapCalculation tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_BOXOVERLAPCALCULATION_H_
#define _IVM_BOXOVERLAPCALCULATION_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSBoxOverlapCalculationModu
    {
    /// \addtogroup Box重叠率计算
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the BoxOverlapCalculationParams.  */
        ///Box重叠率计算参数类
        class BoxOverlapCalculationParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit BoxOverlapCalculationParams() {}
            virtual ~BoxOverlapCalculationParams() {}

        public:

            /// @~chinese 区域1(仅当次执行起效)   @~english region1(Only valid for this execution)
            virtual void SetInputBOX1(RectBox* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 区域2(仅当次执行起效)   @~english region2(Only valid for this execution)
            virtual void SetInputBOX2(RectBox* pInputValue, unsigned int nValueNum) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the BoxOverlapCalculationResults.  */
        ///Box重叠率计算结果类
        class BoxOverlapCalculationResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit BoxOverlapCalculationResults() {}
            virtual ~BoxOverlapCalculationResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

            /**
            * @brief 输出重叠率
            */
            virtual float GetOverlap() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the BoxOverlapCalculation tool.  */
        ///Box重叠率计算工具类
        class IMVSBoxOverlapCalculationModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSBoxOverlapCalculationModuTool() {}
            virtual ~IMVSBoxOverlapCalculationModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual BoxOverlapCalculationParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual BoxOverlapCalculationResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSBoxOverlapCalculationModuTool(IMVSBoxOverlapCalculationModuTool&);
            IMVSBoxOverlapCalculationModuTool& operator=(IMVSBoxOverlapCalculationModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for BoxOverlapCalculation.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSBoxOverlapCalculationModu::IMVSBoxOverlapCalculationModuTool * __stdcall GetBoxOverlapCalculationToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_BOXOVERLAPCALCULATION_H_
