/***************************************************************************************************
 * File：IVmLineAlign.h
 * Note：@~chinese 线对位模块接口声明 @~english Interface for the LineAlign tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_LINEALIGN_H_
#define _IVM_LINEALIGN_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSLineAlignModu
    {
        /// @~chinese 对位形状   @~english Alignment Shape
        typedef enum _AlignModeEnum
        {
            /// @~chinese 开   @~english Opening
            AlignMode_Open = 0,

            /// @~chinese 闭   @~english Closing
            AlignMode_Close = 1,

        }AlignModeEnum;

    /// \addtogroup 线对位
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the LineAlignParams.  */
        ///线对位参数类
        class LineAlignParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit LineAlignParams() {}
            virtual ~LineAlignParams() {}

        public:

            /// @~chinese 目标线输入1(仅当次执行起效)   @~english TargetLineInput1(Only valid for this execution)
            virtual void SetTargetLineInput1(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标线角度1(仅当次执行起效)   @~english TargetLineAngle1(Only valid for this execution)
            virtual void SetTargetLineAngle1(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象线输入1(仅当次执行起效)   @~english ObjectLineInput1(Only valid for this execution)
            virtual void SetObjectLineInput1(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象线角度1(仅当次执行起效)   @~english ObjectLineAngle1(Only valid for this execution)
            virtual void SetObjectLineAngle1(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标线输入2(仅当次执行起效)   @~english TargetLineInput2(Only valid for this execution)
            virtual void SetTargetLineInput2(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标线角度2(仅当次执行起效)   @~english TargetLineAngle2(Only valid for this execution)
            virtual void SetTargetLineAngle2(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象线输入2(仅当次执行起效)   @~english ObjectLineInput2(Only valid for this execution)
            virtual void SetObjectLineInput2(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象线角度2(仅当次执行起效)   @~english ObjectLineAngle2(Only valid for this execution)
            virtual void SetObjectLineAngle2(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标线输入3(仅当次执行起效)   @~english TargetLineInput3(Only valid for this execution)
            virtual void SetTargetLineInput3(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标线角度3(仅当次执行起效)   @~english TargetLineAngle3(Only valid for this execution)
            virtual void SetTargetLineAngle3(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象线输入3(仅当次执行起效)   @~english ObjectLineInput3(Only valid for this execution)
            virtual void SetObjectLineInput3(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象线角度3(仅当次执行起效)   @~english ObjectLineAngle3(Only valid for this execution)
            virtual void SetObjectLineAngle3(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标线输入4(仅当次执行起效)   @~english TargetLineInput4(Only valid for this execution)
            virtual void SetTargetLineInput4(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标线角度4(仅当次执行起效)   @~english TargetLineAngle4(Only valid for this execution)
            virtual void SetTargetLineAngle4(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象线输入4(仅当次执行起效)   @~english ObjectLineInput4(Only valid for this execution)
            virtual void SetObjectLineInput4(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象线角度4(仅当次执行起效)   @~english ObjectLineAngle4(Only valid for this execution)
            virtual void SetObjectLineAngle4(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标线输入5(仅当次执行起效)   @~english TargetLineInput5(Only valid for this execution)
            virtual void SetTargetLineInput5(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标线角度5(仅当次执行起效)   @~english TargetLineAngle5(Only valid for this execution)
            virtual void SetTargetLineAngle5(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象线输入5(仅当次执行起效)   @~english ObjectLineInput5(Only valid for this execution)
            virtual void SetObjectLineInput5(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象线角度5(仅当次执行起效)   @~english ObjectLineAngle5(Only valid for this execution)
            virtual void SetObjectLineAngle5(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标线输入6(仅当次执行起效)   @~english TargetLineInput6(Only valid for this execution)
            virtual void SetTargetLineInput6(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标线角度6(仅当次执行起效)   @~english TargetLineAngle6(Only valid for this execution)
            virtual void SetTargetLineAngle6(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象线输入6(仅当次执行起效)   @~english ObjectLineInput6(Only valid for this execution)
            virtual void SetObjectLineInput6(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象线角度6(仅当次执行起效)   @~english ObjectLineAngle6(Only valid for this execution)
            virtual void SetObjectLineAngle6(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标线输入7(仅当次执行起效)   @~english TargetLineInput7(Only valid for this execution)
            virtual void SetTargetLineInput7(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标线角度7(仅当次执行起效)   @~english TargetLineAngle7(Only valid for this execution)
            virtual void SetTargetLineAngle7(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象线输入7(仅当次执行起效)   @~english ObjectLineInput7(Only valid for this execution)
            virtual void SetObjectLineInput7(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象线角度7(仅当次执行起效)   @~english ObjectLineAngle7(Only valid for this execution)
            virtual void SetObjectLineAngle7(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标线输入8(仅当次执行起效)   @~english TargetLineInput8(Only valid for this execution)
            virtual void SetTargetLineInput8(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标线角度8(仅当次执行起效)   @~english TargetLineAngle8(Only valid for this execution)
            virtual void SetTargetLineAngle8(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象线输入8(仅当次执行起效)   @~english ObjectLineInput8(Only valid for this execution)
            virtual void SetObjectLineInput8(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象线角度8(仅当次执行起效)   @~english ObjectLineAngle8(Only valid for this execution)
            virtual void SetObjectLineAngle8(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对位形状   @~english Alignment Shape
            __declspec(property(put = SetAlignMode, get = GetAlignMode)) AlignModeEnum AlignMode;

            virtual AlignModeEnum GetAlignMode() = 0;

            virtual void SetAlignMode(AlignModeEnum value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the LineAlignResults.  */
        ///线对位结果类
        class LineAlignResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit LineAlignResults() {}
            virtual ~LineAlignResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief x偏移
             */
            virtual float GetDeltaX() = 0;

            /**
             * @brief y偏移
             */
            virtual float GetDeltaY() = 0;

            /**
             * @brief theta偏移
             */
            virtual float GetDeltaTheta() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the LineAlign tool.  */
        ///线对位工具类
        class IMVSLineAlignModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSLineAlignModuTool() {}
            virtual ~IMVSLineAlignModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual LineAlignParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual LineAlignResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSLineAlignModuTool(IMVSLineAlignModuTool&);
            IMVSLineAlignModuTool& operator=(IMVSLineAlignModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for LineAlign.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSLineAlignModu::IMVSLineAlignModuTool * __stdcall GetLineAlignToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_LINEALIGN_H_
