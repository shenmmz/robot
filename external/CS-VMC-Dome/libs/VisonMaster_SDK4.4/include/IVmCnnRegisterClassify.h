/***************************************************************************************************
 * File：IVmCnnRegisterClassify.h
 * Note：@~chinese 注册分类模块接口声明 @~english Interface for the CnnRegisterClassify tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CNNREGISTERCLASSIFY_H_
#define _IVM_CNNREGISTERCLASSIFY_H_

#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCnnRegisterClassifyModu
    {
    /// \addtogroup 注册分类
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///注册分类ROI管理类
        class CnnRegisterClassifyRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoisRectangle(RectBox* pRoiValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the CnnRegisterClassifyParams.  */
        ///注册分类参数类
        class CnnRegisterClassifyParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnRegisterClassifyParams() {}
            virtual ~CnnRegisterClassifyParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual CnnRegisterClassifyRoiManager* GetModuRoiManager() = 0;

			/// @~chinese Gallery路径(弃用)   @~english Gallery Path(Obsolete)
			__declspec(property(put = SetLoadGalleryFilePath, get = GetLoadGalleryFilePath)) const char* LoadGalleryFilePath;

			virtual const char* GetLoadGalleryFilePath() = 0;

			virtual void SetLoadGalleryFilePath(const char* value) = 0;

			/// @~chinese 前K个类别，范围：[1,10]   @~english First K Categories，range:[1,10]
            __declspec(property(put = SetTopClsK, get = GetTopClsK)) int TopClsK;

            virtual int GetTopClsK() = 0;

            virtual void SetTopClsK(int value) = 0;

            /// @~chinese 最小相似度，范围：[0.01,1.0]   @~english Min Similarity，range:[0.01,1.0]
            __declspec(property(put = SetMinSimilarity, get = GetMinSimilarity)) double MinSimilarity;

            virtual double GetMinSimilarity() = 0;

            virtual void SetMinSimilarity(double value) = 0;

            /// @~chinese 批处理使能(仅GPU模块生效)   @~english Batch Processing Enable(Takes effect only in GPU modules)
            __declspec(property(put = SetBatchProcessEnable, get = GetBatchProcessEnable)) bool BatchProcessEnable;

            virtual bool GetBatchProcessEnable() = 0;

            virtual void SetBatchProcessEnable(bool value) = 0;

            /// @~chinese 批处理等级，范围：[1,32](仅GPU模块生效)   @~english Batch Processing Level，range:[1,32](Takes effect only in GPU modules)
            __declspec(property(put = SetBatchProcessingLevel, get = GetBatchProcessingLevel)) int BatchProcessingLevel;

            virtual int GetBatchProcessingLevel() = 0;

            virtual void SetBatchProcessingLevel(int value) = 0;

            /// @~chinese 个数判断   @~english Number Check
            __declspec(property(put = SetNumLimitEnable, get = GetNumLimitEnable)) bool NumLimitEnable;

            virtual bool GetNumLimitEnable() = 0;

            virtual void SetNumLimitEnable(bool value) = 0;

            /// @~chinese 个数范围，范围：[1,99999]   @~english Number Range，range:[1,99999]
            __declspec(property(put = SetNumLimitLow, get = GetNumLimitLow)) int NumLimitLow;

            virtual int GetNumLimitLow() = 0;

            virtual void SetNumLimitLow(int value) = 0;

            /// @~chinese 个数范围，范围：[1,99999]   @~english Number Range，range:[1,99999]
            __declspec(property(put = SetNumLimitHigh, get = GetNumLimitHigh)) int NumLimitHigh;

            virtual int GetNumLimitHigh() = 0;

            virtual void SetNumLimitHigh(int value) = 0;

            /// @~chinese 相似度判断   @~english Similaritys Check
            __declspec(property(put = SetSimilarityLimitEnable, get = GetSimilarityLimitEnable)) bool SimilarityLimitEnable;

            virtual bool GetSimilarityLimitEnable() = 0;

            virtual void SetSimilarityLimitEnable(bool value) = 0;

            /// @~chinese 相似度范围，范围：[0.0,1.0]   @~english Similaritys Range，range:[0.0,1.0]
            __declspec(property(put = SetSimilarityLimitLow, get = GetSimilarityLimitLow)) double SimilarityLimitLow;

            virtual double GetSimilarityLimitLow() = 0;

            virtual void SetSimilarityLimitLow(double value) = 0;

            /// @~chinese 相似度范围，范围：[0.0,1.0]   @~english Similaritys Range，range:[0.0,1.0]
            __declspec(property(put = SetSimilarityLimitHigh, get = GetSimilarityLimitHigh)) double SimilarityLimitHigh;

            virtual double GetSimilarityLimitHigh() = 0;

            virtual void SetSimilarityLimitHigh(double value) = 0;

            /// @~chinese 图像索引判断   @~english Image Index Check
            __declspec(property(put = SetImageIndexLimitEnable, get = GetImageIndexLimitEnable)) bool ImageIndexLimitEnable;

            virtual bool GetImageIndexLimitEnable() = 0;

            virtual void SetImageIndexLimitEnable(bool value) = 0;

            /// @~chinese 图像索引范围，范围：[0,1000]   @~english Image Index Range，range:[0,1000]
            __declspec(property(put = SetImageIndexLimitLow, get = GetImageIndexLimitLow)) int ImageIndexLimitLow;

            virtual int GetImageIndexLimitLow() = 0;

            virtual void SetImageIndexLimitLow(int value) = 0;

            /// @~chinese 图像索引范围，范围：[0,1000]   @~english Image Index Range，range:[0,1000]
            __declspec(property(put = SetImageIndexLimitHigh, get = GetImageIndexLimitHigh)) int ImageIndexLimitHigh;

            virtual int GetImageIndexLimitHigh() = 0;

            virtual void SetImageIndexLimitHigh(int value) = 0;

            /// @~chinese 类别名称判断   @~english Category Name Check
            __declspec(property(put = SetCategoryNameLimitEnable, get = GetCategoryNameLimitEnable)) bool CategoryNameLimitEnable;

            virtual bool GetCategoryNameLimitEnable() = 0;

            virtual void SetCategoryNameLimitEnable(bool value) = 0;

            /// @~chinese 类别名称   @~english Class Name
            __declspec(property(put = SetCategoryNameLimit, get = GetCategoryNameLimit)) const char* CategoryNameLimit;

            virtual const char* GetCategoryNameLimit() = 0;

            virtual void SetCategoryNameLimit(const char* value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the CnnRegisterClassifyResults.  */
        ///注册分类结果类
        class CnnRegisterClassifyResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnRegisterClassifyResults() {}
            virtual ~CnnRegisterClassifyResults() {}

        public:
             /**
             * @brief 检测区域
             * @param nIndex[IN] 数据索引
             */
             virtual RectBox GetDetectROIs(int nIndex) = 0;

             /**
             * @brief ROI序号
             * @param nIndex[IN] 数据索引
             */
             virtual int GetClassRoiIndexs(int nIndex) = 0;

             /**
             * @brief 类别名称
             * @param nIndex[IN] 数据索引
             */
             virtual const char * GetClassNames(int nIndex) = 0;

             /**
             * @brief  图像索引
             * @param nIndex[IN] 数据索引
             */
             virtual int GetImageIndexs(int nIndex) = 0;

             /**
             * @brief  相似度 
             * @param nIndex[IN] 数据索引
             */
             virtual float GetSimilaritys(int nIndex) = 0;

             /**
             * @brief 类别个数
             * @param nIndex[IN] 数据索引
             */
             virtual int GetClassNums(int nIndex) = 0;

             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

			 /**
			 * @brief 类别个数
             * @deprecated V4.4废弃
			 */
			 virtual int GetClassNum() = 0;

			 /**
			 * @brief 相似度
             * @deprecated V4.4废弃
			 */
			 virtual float GetSimilarity(int nIndex) = 0;

			 /**
			 * @brief 图像索引
             * @deprecated V4.4废弃
			 */
			 virtual int GetImageIndex(int nIndex) = 0;

			 /**
			 * @brief 类别名称
             * @deprecated V4.4废弃
			 */
			 virtual string GetClassName(int nIndex) = 0;

			 /**
			 * @brief 缩略图_0
			 */
			 virtual ImageBaseData GetSampleImage_0() = 0;

			 /**
			 * @brief 缩略图_1
			 */
			 virtual ImageBaseData GetSampleImage_1() = 0;

			 /**
			 * @brief 缩略图_2
			 */
			 virtual ImageBaseData GetSampleImage_2() = 0;

			 /**
			 * @brief 缩略图_3
			 */
			 virtual ImageBaseData GetSampleImage_3() = 0;

			 /**
			 * @brief 缩略图_4
			 */
			 virtual ImageBaseData GetSampleImage_4() = 0;

			 /**
			 * @brief 缩略图_5
			 */
			 virtual ImageBaseData GetSampleImage_5() = 0;

			 /**
			 * @brief 缩略图_6
			 */
			 virtual ImageBaseData GetSampleImage_6() = 0;

			 /**
			 * @brief 缩略图_7
			 */
			 virtual ImageBaseData GetSampleImage_7() = 0;

			 /**
			 * @brief 缩略图_8
			 */
			 virtual ImageBaseData GetSampleImage_8() = 0;

			 /**
			 * @brief 缩略图_9
			 */
			 virtual ImageBaseData GetSampleImage_9() = 0;

        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CnnRegisterClassify tool.  */
        ///注册分类工具类
        class IMVSCnnRegisterClassifyModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCnnRegisterClassifyModuTool() {}
            virtual ~IMVSCnnRegisterClassifyModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CnnRegisterClassifyParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CnnRegisterClassifyResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

            /**
            * @brief 导入模型
            * @param strPath [IN] 模型路径
            * @return 无，出错时抛出异常
            */
            virtual void ImportModelData(IN const char * const strPath) = 0;

        private:
            IMVSCnnRegisterClassifyModuTool(IMVSCnnRegisterClassifyModuTool&);
            IMVSCnnRegisterClassifyModuTool& operator=(IMVSCnnRegisterClassifyModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CnnRegisterClassify.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSCnnRegisterClassifyModu::IMVSCnnRegisterClassifyModuTool * __stdcall GetCnnRegisterClassifyToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CNNREGISTERCLASSIFY_H_
