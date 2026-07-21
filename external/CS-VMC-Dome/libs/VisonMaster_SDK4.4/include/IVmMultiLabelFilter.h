/***************************************************************************************************
 * File：IVmMultiLabelFilter.h
 * Note：@~chinese 多标签筛选模块接口声明 @~english Interface for the MultiLabelFilter tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_MULTILABELFILTER_H_
#define _IVM_MULTILABELFILTER_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSMultiLabelFilterModu
    {
    /// \addtogroup 多标签筛选
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the MultiLabelFilterParams.  */
        ///多标签筛选参数类
        class MultiLabelFilterParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MultiLabelFilterParams() {}
            virtual ~MultiLabelFilterParams() {}

        public:

            /// @~chinese 类别编号(仅当次执行起效)   @~english Label(Only valid for this execution)
            virtual void SetClassIndex(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 类别个数(仅当次执行起效)   @~english ClassNum(Only valid for this execution)
            virtual void SetClassNum(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the MultiLabelFilterResults.  */
        ///多标签筛选结果类
        class MultiLabelFilterResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MultiLabelFilterResults() {}
            virtual ~MultiLabelFilterResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

            /**
            * @brief 输出图像
            */
            virtual ImageBaseData GetOutputImage() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the MultiLabelFilter tool.  */
        ///多标签筛选工具类
        class IMVSMultiLabelFilterModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSMultiLabelFilterModuTool() {}
            virtual ~IMVSMultiLabelFilterModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual MultiLabelFilterParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual MultiLabelFilterResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSMultiLabelFilterModuTool(IMVSMultiLabelFilterModuTool&);
            IMVSMultiLabelFilterModuTool& operator=(IMVSMultiLabelFilterModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for MultiLabelFilter.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSMultiLabelFilterModu::IMVSMultiLabelFilterModuTool * __stdcall GetMultiLabelFilterToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_MULTILABELFILTER_H_
