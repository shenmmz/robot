/***************************************************************************************************
 * File：IVmCnnRetrieval.h
 * Note：@~chinese DL图像检索模块接口声明 @~english Interface for the CnnRetrieval tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CNNRETRIEVAL_H_
#define _IVM_CNNRETRIEVAL_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

#ifndef MVD_CNN_RETRIEVAL_LABEL_NAME_LEN_MAX
#define MVD_CNN_RETRIEVAL_LABEL_NAME_LEN_MAX    (1024)
#endif

namespace VisionMasterSDK
{
    namespace IMVSCnnRetrievalModu
    {
    /// \addtogroup DL图像检索
    /// @{
        /*************************** Params Class Start ***************************/
        ///DL图像检索ROI管理类
        class CnnRetrievalRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoisRectangle(RectBox* pRoiValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the CnnRetrievalParams.  */
        ///DL图像检索参数类
        class CnnRetrievalParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnRetrievalParams() {}
            virtual ~CnnRetrievalParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual CnnRetrievalRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 模型文件路径   @~english Model File Path
            __declspec(property(put = SetLoadModelPath, get = GetLoadModelPath)) const char* LoadModelPath;

            virtual const char* GetLoadModelPath() = 0;

            virtual void SetLoadModelPath(const char* value) = 0;

            /// @~chinese Gallery路径   @~english Gallery Path
            __declspec(property(put = SetLoadGalleryFilePath, get = GetLoadGalleryFilePath)) const char* LoadGalleryFilePath;

            virtual const char* GetLoadGalleryFilePath() = 0;

            virtual void SetLoadGalleryFilePath(const char* value) = 0;

            /// @~chinese 方案存模型   @~english Save Model Data
            __declspec(property(put = SetSaveModelDataEnable, get = GetSaveModelDataEnable)) bool SaveModelDataEnable;

            virtual bool GetSaveModelDataEnable() = 0;

            virtual void SetSaveModelDataEnable(bool value) = 0;

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

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        ///图像检索匹配信息类
        class ICNNRetrievalMatchInfo
        {
        protected:
            explicit ICNNRetrievalMatchInfo(){};
            virtual ~ICNNRetrievalMatchInfo(){};

        public:
            /**
             * @brief 获取检索出的样本相似度
             */
            virtual float GetSimilarity() = 0;
            /**
             * @brief 获取检索出的样本索引号
             */
            virtual int GetSampleIndex() = 0;
            /**
             * @brief 获取检索出的样本对应的标签名称
             */
            virtual void GetLabelName(char pcLabelName[MVD_CNN_RETRIEVAL_LABEL_NAME_LEN_MAX]) = 0;
        };

        /*  Note: Interface Classes of the result of the CnnRetrievalResults.  */
        ///DL图像检索结果类
        class CnnRetrievalResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnRetrievalResults() {}
            virtual ~CnnRetrievalResults() {}

        public:
            /**
            * @brief 检测区域
            * @param nIndex[IN] 数据索引
            */
            virtual RectBox GetDetectROIs(int nIndex) = 0;

            /**
            * @brief 模型标识
            */
            virtual const char * GetModelFlag() = 0;

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
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
             /**
             * @brief 检测区域
             * @deprecated V4.4废弃
             */
             virtual RectBox GetROI() = 0;
            /**
             * @brief 检索信息的数量
             * @deprecated V4.4废弃
             */
            virtual int GetTopNum() = 0;
            /**
             * @brief 根据给定的索引获取检索信息
             * @deprecated V4.4废弃
             */
            virtual ICNNRetrievalMatchInfo* GetMatchInfo(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CnnRetrieval tool.  */
        ///DL图像检索工具类
        class IMVSCnnRetrievalModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCnnRetrievalModuTool() {}
            virtual ~IMVSCnnRetrievalModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CnnRetrievalParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CnnRetrievalResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSCnnRetrievalModuTool(IMVSCnnRetrievalModuTool&);
            IMVSCnnRetrievalModuTool& operator=(IMVSCnnRetrievalModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CnnRetrieval.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSCnnRetrievalModu::IMVSCnnRetrievalModuTool * __stdcall GetCnnRetrievalToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CNNRETRIEVAL_H_
