/***************************************************************************************************
 * File：IVmArray2dCorrect.h
 * Note：@~chinese 二维阵列模块接口声明 @~english Interface for the Array2dCorrect tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_ARRAY2DCORRECT_H_
#define _IVM_ARRAY2DCORRECT_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVS2dArrayCorrectModu
    {
    /// \addtogroup 二维阵列
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the Array2dCorrectParams.  */
        ///二维阵列参数类
        class Array2dCorrectParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit Array2dCorrectParams() {}
            virtual ~Array2dCorrectParams() {}

        public:

            /// @~chinese 区域(仅当次执行起效)   @~english Region(Only valid for this execution)
            virtual void SetInputBox(RectBox* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 阵列行数，范围：[1,100]   @~english Array Rows，range:[1,100]
            __declspec(property(put = SetArrayRowsNum, get = GetArrayRowsNum)) int ArrayRowsNum;

            virtual int GetArrayRowsNum() = 0;

            virtual void SetArrayRowsNum(int value) = 0;

            /// @~chinese 阵列列数，范围：[1,100]   @~english Array Columns，range:[1,100]
            __declspec(property(put = SetArrayColsNum, get = GetArrayColsNum)) int ArrayColsNum;

            virtual int GetArrayColsNum() = 0;

            virtual void SetArrayColsNum(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the Array2dCorrectResults.  */
        ///二维阵列结果类
        class Array2dCorrectResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit Array2dCorrectResults() {}
            virtual ~Array2dCorrectResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

            /**
            * @brief 获取修正后的Box数量
            */
            virtual unsigned int GetNumOfArrayBoxes() = 0;
            /**
            * @brief 说明获取最小包围行的Box数量
            */
            virtual unsigned int GetNumOfRowBoxes() = 0;
            /**
            * @brief 根据索引获取修正后的Box信息
            * @param nIndex [IN] 修正后的Box索引，范围[0, NumOfArrayBoxes)
            * @return 指定索引处Box的信息，出错时抛出异常
            */
            virtual RectBox GetArrayBoxAt(unsigned int nIndex) = 0;
            /**
            * @brief 根据索引获取最小包围行的Box信息
            * @param nIndex [IN] 最小包围行的Box索引，范围[0, NumOfRowBox)
            * @return 指定索引处Box的信息，出错时抛出异常
            */
            virtual RectBox GetRowBoxAt(unsigned int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the Array2dCorrect tool.  */
        ///二维阵列工具类
        class IMVS2dArrayCorrectModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVS2dArrayCorrectModuTool() {}
            virtual ~IMVS2dArrayCorrectModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual Array2dCorrectParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual Array2dCorrectResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVS2dArrayCorrectModuTool(IMVS2dArrayCorrectModuTool&);
            IMVS2dArrayCorrectModuTool& operator=(IMVS2dArrayCorrectModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for Array2dCorrect.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVS2dArrayCorrectModu::IMVS2dArrayCorrectModuTool * __stdcall GetArray2dCorrectToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_ARRAY2DCORRECT_H_
