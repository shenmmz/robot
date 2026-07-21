/***************************************************************************************************
 * File：IVmMedianLineFind.h
 * Note：@~chinese 中线查找模块接口声明 @~english Interface for the MedianLineFind tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_MEDIANLINEFIND_H_
#define _IVM_MEDIANLINEFIND_H_

#include "MVD_ShapeParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSMedianLineFindModu
    {
    /// \addtogroup 中线查找
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the MedianLineFindParams.  */
        ///中线查找参数类
        class MedianLineFindParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MedianLineFindParams() {}
            virtual ~MedianLineFindParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入直线1(仅当次执行起效)   @~english Input Line 1(Only valid for this execution)
            virtual void SetInputLine1(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入直线2(仅当次执行起效)   @~english Input Line 2(Only valid for this execution)
            virtual void SetInputLine2(Line* pInputValue, unsigned int nValueNum) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the MedianLineFindResults.  */
        ///中线查找结果类
        class MedianLineFindResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MedianLineFindResults() {}
            virtual ~MedianLineFindResults() {}

        public:
            /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 中线
             */
             virtual Line GetMedianLine() = 0;
            /**
             * @brief 测量直线1
             */
             virtual Line GetMeasureLine1() = 0;
            /**
             * @brief 测量直线2
             */
             virtual Line GetMeasureLine2() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the MedianLineFind tool.  */
        ///中线查找工具类
        class IMVSMedianLineFindModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSMedianLineFindModuTool() {}
            virtual ~IMVSMedianLineFindModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual MedianLineFindParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual MedianLineFindResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSMedianLineFindModuTool(IMVSMedianLineFindModuTool&);
            IMVSMedianLineFindModuTool& operator=(IMVSMedianLineFindModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for MedianLineFind.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSMedianLineFindModu::IMVSMedianLineFindModuTool * __stdcall GetMedianLineFindToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_MEDIANLINEFIND_H_
