/***************************************************************************************************
 * File：IVmFixture.h
 * Note：@~chinese 位置修正模块接口声明 @~english Interface for the Fixture tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_FIXTURE_H_
#define _IVM_FIXTURE_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSFixtureModu
    {
    /// \addtogroup 位置修正
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the FixtureParams.  */
        ///位置修正参数类
        class FixtureParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit FixtureParams() {}
            virtual ~FixtureParams() {}

        public:

            /// @~chinese 原点(仅当次执行起效)   @~english Origin(Only valid for this execution)
            virtual void SetOrigin(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 角度(仅当次执行起效)   @~english InAngle(Only valid for this execution)
            virtual void SetInAngle(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 尺度X(仅当次执行起效)   @~english MatchScaleX(Only valid for this execution)
            virtual void SetMatchScaleX(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 尺度Y(仅当次执行起效)   @~english MatchScaleY(Only valid for this execution)
            virtual void SetMatchScaleY(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 创建基准   @~english Create Reference
            virtual int DoFixtureInit() = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the FixtureResults.  */
        ///位置修正结果类
        class FixtureResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit FixtureResults() {}
            virtual ~FixtureResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 基准点
             */
            virtual MVD_POINT_F GetFixturedPoint() = 0;

            /**
             * @brief 基准角度
             */
            virtual float GetFixtureInitAngle() = 0;

            /**
             * @brief 基准尺度X
             */
            virtual float GetFixtureInitScaleX() = 0;

            /**
             * @brief 基准尺度Y
             */
            virtual float GetFixtureInitScaleY() = 0;

            /**
             * @brief 运行点
             */
            virtual MVD_POINT_F GetFixtureRunPoint() = 0;

            /**
             * @brief 运行角度
             */
            virtual float GetFixtureRunAngle() = 0;

            /**
             * @brief 运行尺度X
             */
            virtual float GetFixtureRunScaleX() = 0;

            /**
             * @brief 运行尺度Y
             */
            virtual float GetFixtureRunScaleY() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the Fixture tool.  */
        ///位置修正工具类
        class IMVSFixtureModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSFixtureModuTool() {}
            virtual ~IMVSFixtureModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual FixtureParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual FixtureResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSFixtureModuTool(IMVSFixtureModuTool&);
            IMVSFixtureModuTool& operator=(IMVSFixtureModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for Fixture.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSFixtureModu::IMVSFixtureModuTool * __stdcall GetFixtureToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_FIXTURE_H_
