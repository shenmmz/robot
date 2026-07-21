/***************************************************************************************************
 * File：IVmSinglePointAlign.h
 * Note：@~chinese 单点对位模块接口声明 @~english Interface for the SinglePointAlign tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_SINGLEPOINTALIGN_H_
#define _IVM_SINGLEPOINTALIGN_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSSinglePointAlignModu
    {
    /// \addtogroup 单点对位
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the SinglePointAlignParams.  */
        ///单点对位参数类
        class SinglePointAlignParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit SinglePointAlignParams() {}
            virtual ~SinglePointAlignParams() {}

        public:

            /// @~chinese 目标点输入(仅当次执行起效)   @~english TargetPointInput(Only valid for this execution)
            virtual void SetTargetPointInput(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标角度(仅当次执行起效)   @~english TargetAngle(Only valid for this execution)
            virtual void SetTargetAngle(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象点输入(仅当次执行起效)   @~english ObjectPointInput(Only valid for this execution)
            virtual void SetObjectPointInput(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象角度(仅当次执行起效)   @~english ObjectAngle(Only valid for this execution)
            virtual void SetObjectAngle(float* pInputValue, unsigned int nValueNum) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the SinglePointAlignResults.  */
        ///单点对位结果类
        class SinglePointAlignResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit SinglePointAlignResults() {}
            virtual ~SinglePointAlignResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief theta方向偏移
             */
            virtual float GetDeltaTheta() = 0;
            /**
             * @brief x方向偏移
             */
            virtual float GetDeltaX() = 0;
            /**
             * @brief y方向偏移
             */
            virtual float GetDeltaY() = 0;
            /**
             * @brief 单点对位误差
             */
            virtual float GetAlignError() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the SinglePointAlign tool.  */
        ///单点对位工具类
        class IMVSSinglePointAlignModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSSinglePointAlignModuTool() {}
            virtual ~IMVSSinglePointAlignModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual SinglePointAlignParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual SinglePointAlignResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSSinglePointAlignModuTool(IMVSSinglePointAlignModuTool&);
            IMVSSinglePointAlignModuTool& operator=(IMVSSinglePointAlignModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for SinglePointAlign.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSSinglePointAlignModu::IMVSSinglePointAlignModuTool * __stdcall GetSinglePointAlignToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_SINGLEPOINTALIGN_H_
