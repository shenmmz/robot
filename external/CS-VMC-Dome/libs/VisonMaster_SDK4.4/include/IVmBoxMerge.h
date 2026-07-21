/***************************************************************************************************
 * File：IVmBoxMerge.h
 * Note：@~chinese Box融合模块接口声明 @~english Interface for the BoxMerge tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_BOXMERGE_H_
#define _IVM_BOXMERGE_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSBoxMergeModu
    {
        /// @~chinese 融合依据   @~english Merging Criterion
        typedef enum _MergeMethodEnum
        {
            /// @~chinese 重叠率   @~english Overlap Rate
            MergeMethod_Overlap = 0x1,

            /// @~chinese Box中心之间的距离   @~english Distance between Box Centers
            MergeMethod_Distance = 0x2,

        }MergeMethodEnum;

        /// @~chinese Box融合类型   @~english Box Merge Type
        typedef enum _BoxMergeTypeEnum
        {
            /// @~chinese 无条件融合并按缺陷优先级输出标签   @~english Merge Type Flaw Priority
            BoxMergeType_MergeTypeFlawPriority = 0x1,

            /// @~chinese 按标签类别融合并输出标签   @~english Merge Type Box Label
            BoxMergeType_MergeTypeBoxLabel = 0x2,

            /// @~chinese 无条件融合并随机输出标签   @~english Merge Type No Box Label
            BoxMergeType_MergeTypeNoBoxLabel = 0x3,

        }BoxMergeTypeEnum;

        /// @~chinese 标签分割符   @~english Split Char
        typedef enum _SplitCharEnum
        {
            /// @~chinese 逗号   @~english Dot
            SplitChar_Dot = 0x1,

            /// @~chinese 冒号   @~english Colon
            SplitChar_Colon = 0x2,

            /// @~chinese 分号   @~english Semicolon
            SplitChar_Semicolon = 0x3,

            /// @~chinese 下划线   @~english Underline
            SplitChar_Underline = 0x4,

        }SplitCharEnum;

        /// @~chinese 优先级排序   @~english Priority Sort Type
        typedef enum _PrioritySortTypeEnum
        {
            /// @~chinese 降序   @~english Descending
            PrioritySortType_PrioritySortDescend = 0x1,

            /// @~chinese 升序   @~english Ascending
            PrioritySortType_PrioritySortAscend = 0x2,

        }PrioritySortTypeEnum;

    /// \addtogroup Box融合
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the BoxMergeParams.  */
        ///Box融合参数类
        class BoxMergeParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit BoxMergeParams() {}
            virtual ~BoxMergeParams() {}

        public:

            /// @~chinese 输入Box(仅当次执行起效)   @~english Input Box(Only valid for this execution)
            virtual void SetInputBOX(RectBox* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese Box标签(仅当次执行起效)   @~english Box labels(Only valid for this execution)
            virtual void SetBoxLabel(InputStringData* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 缺陷优先级(仅当次执行起效)   @~english Flaw Priority(Only valid for this execution)
            virtual void SetFlawPriority(InputStringData* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 融合依据   @~english Merging Criterion
            __declspec(property(put = SetMergeMethod, get = GetMergeMethod)) MergeMethodEnum MergeMethod;

            virtual MergeMethodEnum GetMergeMethod() = 0;

            virtual void SetMergeMethod(MergeMethodEnum value) = 0;

            /// @~chinese 重叠率，范围：[1,100]   @~english Overlap Rate，range:[1,100]
            __declspec(property(put = SetOverlapThresh, get = GetOverlapThresh)) int OverlapThresh;

            virtual int GetOverlapThresh() = 0;

            virtual void SetOverlapThresh(int value) = 0;

            /// @~chinese X方向的距离，范围：[0,99999.9]   @~english X Distance，range:[0,99999.9]
            __declspec(property(put = SetDxThresh, get = GetDxThresh)) double DxThresh;

            virtual double GetDxThresh() = 0;

            virtual void SetDxThresh(double value) = 0;

            /// @~chinese Y方向的距离，范围：[0,99999.9]   @~english Y Distance，range:[0,99999.9]
            __declspec(property(put = SetDyThresh, get = GetDyThresh)) double DyThresh;

            virtual double GetDyThresh() = 0;

            virtual void SetDyThresh(double value) = 0;

            /// @~chinese Box融合类型   @~english Box Merge Type
            __declspec(property(put = SetBoxMergeType, get = GetBoxMergeType)) BoxMergeTypeEnum BoxMergeType;

            virtual BoxMergeTypeEnum GetBoxMergeType() = 0;

            virtual void SetBoxMergeType(BoxMergeTypeEnum value) = 0;

            /// @~chinese 标签分割符   @~english Split Char
            __declspec(property(put = SetSplitChar, get = GetSplitChar)) SplitCharEnum SplitChar;

            virtual SplitCharEnum GetSplitChar() = 0;

            virtual void SetSplitChar(SplitCharEnum value) = 0;

            /// @~chinese 优先级排序   @~english Priority Sort Type
            __declspec(property(put = SetPrioritySortType, get = GetPrioritySortType)) PrioritySortTypeEnum PrioritySortType;

            virtual PrioritySortTypeEnum GetPrioritySortType() = 0;

            virtual void SetPrioritySortType(PrioritySortTypeEnum value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the BoxMergeResults.  */
        ///Box融合结果类
        class BoxMergeResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit BoxMergeResults() {}
            virtual ~BoxMergeResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

            /**
            * @brief 根据索引获取输出Box的信息
            * @param nIndex [IN] Box索引，范围[0, NumOfOutputBoxes)
            * @param pcLabel [OUT] Box标签
            * @return 指定索引处Box的信息，出错时抛出异常
            */
            virtual RectBox GetOutputBoxData(unsigned int nIndex) = 0;
            /**
            * @brief 根据索引获取标签
            * @param pcLabel [IN] Box标签
            * @return 指定标签，出错时抛出异常
            */
            virtual const char * GetOutputBoxByLabel(unsigned int nIndex) = 0;
            /**
            * @brief 获取重合的box数量
            */
            virtual unsigned int GetBoxMergeNum() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the BoxMerge tool.  */
        ///Box融合工具类
        class IMVSBoxMergeModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSBoxMergeModuTool() {}
            virtual ~IMVSBoxMergeModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual BoxMergeParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual BoxMergeResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSBoxMergeModuTool(IMVSBoxMergeModuTool&);
            IMVSBoxMergeModuTool& operator=(IMVSBoxMergeModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for BoxMerge.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSBoxMergeModu::IMVSBoxMergeModuTool * __stdcall GetBoxMergeToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_BOXMERGE_H_
