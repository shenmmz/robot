/***************************************************************************************************
 * File：IVmVerticalLineFind.h
 * Note：@~chinese 垂线查找模块接口声明 @~english Interface for the VerticalLineFind tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_VERTICALLINEFIND_H_
#define _IVM_VERTICALLINEFIND_H_

#include "MVD_ShapeParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSVerticalLineFindModu
    {
        /// @~chinese 垂线类型   @~english VerticalLine Type
        typedef enum _TypeEnum
        {
            /// @~chinese 过点垂线   @~english Point Perpendicular
            VerticalLinePoint = 0x0,

                /// @~chinese 中垂线   @~english Perpendicular Bisector
            VerticalLineMedian = 0x1,

        }TypeEnum;

    /// \addtogroup 垂线查找
    /// @{

        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the VerticalLineFindParams.  */
        ///垂线查找参数类
        class VerticalLineFindParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit VerticalLineFindParams() {}
            virtual ~VerticalLineFindParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入点(仅当次执行起效)   @~english Input Point(Only valid for this execution)
            virtual void SetInputPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入直线(仅当次执行起效)   @~english Input Line(Only valid for this execution)
            virtual void SetInputLine(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 垂线类型   @~english VerticalLine Type
            __declspec(property(put = SetType, get = GetType)) TypeEnum Type;

            virtual TypeEnum GetType() = 0;

            virtual void SetType(TypeEnum value) = 0;
        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the VerticalLineFindResults.  */
        ///垂线查找结果类
        class VerticalLineFindResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit VerticalLineFindResults() {}
            virtual ~VerticalLineFindResults() {}

        public:
            /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 测量点
             */
             virtual MVD_POINT_F GetMeasurePoint() = 0;
            /**
             * @brief 测量直线
             */
             virtual Line GetMeasureLine() = 0;
            /**
             * @brief 输出直线角度
             */
             virtual float GetOutputLineAngle() = 0;
            /**
             * @brief 垂足点
             */
             virtual MVD_POINT_F GetFootPoint() = 0;
            /**
             * @brief 垂线
             */
             virtual Line GetVerticalLine() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the VerticalLineFind tool.  */
        ///垂线查找工具类
        class IMVSVerticalLineFindModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSVerticalLineFindModuTool() {}
            virtual ~IMVSVerticalLineFindModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual VerticalLineFindParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual VerticalLineFindResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSVerticalLineFindModuTool(IMVSVerticalLineFindModuTool&);
            IMVSVerticalLineFindModuTool& operator=(IMVSVerticalLineFindModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for VerticalLineFind.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSVerticalLineFindModu::IMVSVerticalLineFindModuTool * __stdcall GetVerticalLineFindToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_VERTICALLINEFIND_H_
