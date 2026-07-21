/***************************************************************************************************
 * File：IVmBoxFilter.h
 * Note：@~chinese Box过滤模块接口声明 @~english Interface for the BoxFilter tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_BOXFILTER_H_
#define _IVM_BOXFILTER_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSBoxFilterModule
    {
    /// \addtogroup Box过滤
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the BoxFilterParams.  */
        ///Box过滤参数类
        class BoxFilterParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit BoxFilterParams() {}
            virtual ~BoxFilterParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 区域(仅当次执行起效)   @~english Region(Only valid for this execution)
            virtual void SetInputRegion(RectBox* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 最小重叠率，范围：[0.01,1.0]   @~english Min. Overlap Rate，range:[0.01,1.0]
            __declspec(property(put = SetMinOverlap, get = GetMinOverlap)) double MinOverlap;

            virtual double GetMinOverlap() = 0;

            virtual void SetMinOverlap(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the BoxFilterResults.  */
        ///Box过滤结果类
        class BoxFilterResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit BoxFilterResults() {}
            virtual ~BoxFilterResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

            /**
            * @brief 获取过滤后保留的Box数量
            */
            virtual unsigned int GetNumOfOutputBoxes() = 0;
            /**
            * @brief 根据索引获取输出Box的信息
            * @param nIndex [IN] Box索引，范围[0, NumOfOutputBoxes)
            * @return 指定索引处Box的信息，出错时抛出异常
            */
            virtual RectBox GetOutputBox(unsigned int nIndex) = 0;
            /**
            * @brief 根据索引获取输出Box的编号
            * @param nIndex [IN] Box索引，范围[0, NumOfOutputBoxes)
            * @return 指定索引处Box的编号，出错时抛出异常
            * @remarks 如0,2为保留第1和第3个输入的box
            */
            virtual unsigned int GetIndexOfOutputBox(unsigned int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the BoxFilter tool.  */
        ///Box过滤工具类
        class IMVSBoxFilterModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSBoxFilterModuleTool() {}
            virtual ~IMVSBoxFilterModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual BoxFilterParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual BoxFilterResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSBoxFilterModuleTool(IMVSBoxFilterModuleTool&);
            IMVSBoxFilterModuleTool& operator=(IMVSBoxFilterModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for BoxFilter.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSBoxFilterModule::IMVSBoxFilterModuleTool * __stdcall GetBoxFilterToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_BOXFILTER_H_
