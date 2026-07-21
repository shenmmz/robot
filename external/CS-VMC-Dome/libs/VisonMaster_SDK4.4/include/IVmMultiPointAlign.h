/***************************************************************************************************
 * File：IVmMultiPointAlign.h
 * Note：@~chinese 点集对位模块接口声明 @~english Interface for the MultiPointAlign tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_MULTIPOINTALIGN_H_
#define _IVM_MULTIPOINTALIGN_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSMultiPointAlignModu
    {
    /// \addtogroup 点集对位
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the MultiPointAlignParams.  */
        ///点集对位参数类
        class MultiPointAlignParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MultiPointAlignParams() {}
            virtual ~MultiPointAlignParams() {}

        public:

            /// @~chinese 目标点输入1(仅当次执行起效)   @~english TargetPointInput1(Only valid for this execution)
            virtual void SetTargetPointInput1(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象点输入1(仅当次执行起效)   @~english ObjectPointInput1(Only valid for this execution)
            virtual void SetObjectPointInput1(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标点输入2(仅当次执行起效)   @~english TargetPointInput2(Only valid for this execution)
            virtual void SetTargetPointInput2(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象点输入2(仅当次执行起效)   @~english ObjectPointInput2(Only valid for this execution)
            virtual void SetObjectPointInput2(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标点输入3(仅当次执行起效)   @~english TargetPointInput3(Only valid for this execution)
            virtual void SetTargetPointInput3(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象点输入3(仅当次执行起效)   @~english ObjectPointInput3(Only valid for this execution)
            virtual void SetObjectPointInput3(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标点输入4(仅当次执行起效)   @~english TargetPointInput4(Only valid for this execution)
            virtual void SetTargetPointInput4(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象点输入4(仅当次执行起效)   @~english ObjectPointInput4(Only valid for this execution)
            virtual void SetObjectPointInput4(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标点输入5(仅当次执行起效)   @~english TargetPointInput5(Only valid for this execution)
            virtual void SetTargetPointInput5(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象点输入5(仅当次执行起效)   @~english ObjectPointInput5(Only valid for this execution)
            virtual void SetObjectPointInput5(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标点输入6(仅当次执行起效)   @~english TargetPointInput6(Only valid for this execution)
            virtual void SetTargetPointInput6(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象点输入6(仅当次执行起效)   @~english ObjectPointInput6(Only valid for this execution)
            virtual void SetObjectPointInput6(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标点输入7(仅当次执行起效)   @~english TargetPointInput7(Only valid for this execution)
            virtual void SetTargetPointInput7(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象点输入7(仅当次执行起效)   @~english ObjectPointInput7(Only valid for this execution)
            virtual void SetObjectPointInput7(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标点输入8(仅当次执行起效)   @~english TargetPointInput8(Only valid for this execution)
            virtual void SetTargetPointInput8(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象点输入8(仅当次执行起效)   @~english ObjectPointInput8(Only valid for this execution)
            virtual void SetObjectPointInput8(PointF* pInputValue, unsigned int nValueNum) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the MultiPointAlignResults.  */
        ///点集对位结果类
        class MultiPointAlignResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MultiPointAlignResults() {}
            virtual ~MultiPointAlignResults() {}

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
             * @brief 对位误差
             */
            virtual float GetAlignError() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the MultiPointAlign tool.  */
        ///点集对位工具类
        class IMVSMultiPointAlignModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSMultiPointAlignModuTool() {}
            virtual ~IMVSMultiPointAlignModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual MultiPointAlignParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual MultiPointAlignResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSMultiPointAlignModuTool(IMVSMultiPointAlignModuTool&);
            IMVSMultiPointAlignModuTool& operator=(IMVSMultiPointAlignModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for MultiPointAlign.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSMultiPointAlignModu::IMVSMultiPointAlignModuTool * __stdcall GetMultiPointAlignToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_MULTIPOINTALIGN_H_
