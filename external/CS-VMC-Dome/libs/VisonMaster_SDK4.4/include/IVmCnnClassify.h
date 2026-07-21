/***************************************************************************************************
 * File：IVmCnnClassify.h
 * Note：@~chinese DL分类模块接口声明 @~english Interface for the CnnClassify tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CNNCLASSIFY_H_
#define _IVM_CNNCLASSIFY_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCnnClassifyModu
    {
    /// \addtogroup DL分类
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///DL分类ROI管理类
        class CnnClassifyRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoisRectangle(RectBox* pRoiValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the CnnClassifyParams.  */
        ///DL分类参数类
        class CnnClassifyParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnClassifyParams() {}
            virtual ~CnnClassifyParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual CnnClassifyRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 模型文件路径   @~english Model File Path
            __declspec(property(put = SetLoadModelPath, get = GetLoadModelPath)) const char* LoadModelPath;

            virtual const char* GetLoadModelPath() = 0;

            virtual void SetLoadModelPath(const char* value) = 0;

            /// @~chinese 方案存模型   @~english Save Model Data
            __declspec(property(put = SetSaveModelDataEnable, get = GetSaveModelDataEnable)) bool SaveModelDataEnable;

            virtual bool GetSaveModelDataEnable() = 0;

            virtual void SetSaveModelDataEnable(bool value) = 0;

            /// @~chinese 获取模型ROI   @~english Get Roi From Model
            __declspec(property(put = SetRoiFromModelEnable, get = GetRoiFromModelEnable)) bool RoiFromModelEnable;

            virtual bool GetRoiFromModelEnable() = 0;

            virtual void SetRoiFromModelEnable(bool value) = 0;

            /// @~chinese 前K个类别，范围：[1,100]   @~english First K Categories，range:[1,100]
            __declspec(property(put = SetTopClassK, get = GetTopClassK)) int TopClassK;

            virtual int GetTopClassK() = 0;

            virtual void SetTopClassK(int value) = 0;

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

            /// @~chinese 概率判断   @~english Probability Check
            __declspec(property(put = SetProbLimitEnable, get = GetProbLimitEnable)) bool ProbLimitEnable;

            virtual bool GetProbLimitEnable() = 0;

            virtual void SetProbLimitEnable(bool value) = 0;

            /// @~chinese 概率范围，范围：[0.0,1.0]   @~english Probability Range，range:[0.0,1.0]
            __declspec(property(put = SetProbLimitLow, get = GetProbLimitLow)) double ProbLimitLow;

            virtual double GetProbLimitLow() = 0;

            virtual void SetProbLimitLow(double value) = 0;

            /// @~chinese 概率范围，范围：[0.0,1.0]   @~english Probability Range，range:[0.0,1.0]
            __declspec(property(put = SetProbLimitHigh, get = GetProbLimitHigh)) double ProbLimitHigh;

            virtual double GetProbLimitHigh() = 0;

            virtual void SetProbLimitHigh(double value) = 0;

            /// @~chinese 类别号判断   @~english Category No. Check
            __declspec(property(put = SetLabelLimitEnable, get = GetLabelLimitEnable)) bool LabelLimitEnable;

            virtual bool GetLabelLimitEnable() = 0;

            virtual void SetLabelLimitEnable(bool value) = 0;

            /// @~chinese 类别号范围，范围：[0,1000]   @~english No. Range，range:[0,1000]
            __declspec(property(put = SetLabelLimitLow, get = GetLabelLimitLow)) int LabelLimitLow;

            virtual int GetLabelLimitLow() = 0;

            virtual void SetLabelLimitLow(int value) = 0;

            /// @~chinese 类别号范围，范围：[0,1000]   @~english No. Range，range:[0,1000]
            __declspec(property(put = SetLabelLimitHigh, get = GetLabelLimitHigh)) int LabelLimitHigh;

            virtual int GetLabelLimitHigh() = 0;

            virtual void SetLabelLimitHigh(int value) = 0;

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
		/// 分类预测信息
        class ICNNClassifyPredInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit ICNNClassifyPredInfo() {}
            virtual ~ICNNClassifyPredInfo() {}

        public:
            /**
             * @brief 标签值，例如0，1，2......
             */
            virtual int GetLabelValue() = 0;
            /**
             * @brief 标签概率值
             */
            virtual float GetLabelProbability() = 0;
            /**
             * @brief 标签名称，例如狗，猫，鸟......
             */
            virtual void GetLabelName(char pcLabelName[128]) = 0;
        };

        /*  Note: Interface Classes of the result of the CnnClassifyResults.  */
        ///DL分类结果类
        class CnnClassifyResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnClassifyResults() {}
            virtual ~CnnClassifyResults() {}

        public:
            /**
            * @brief 检测区域
            * @param nIndex[IN] 数据索引
            */
            virtual RectBox GetDetectROIs(int nIndex) = 0;

             /**
             * @brief 检测区域
             * @deprecated V4.4废弃
             */
             virtual RectBox GetROI() = 0;

             /**
             * @brief ROI序号
             * @param nIndex[IN] 数据索引
             * @deprecated V4.4废弃
             */
             virtual int GetClassRoiIndex(int nIndex = 0) = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 给定索引的预测信息
             * @param nPredIndex[IN] 数据索引
             * @deprecated V4.4废弃
             */
            virtual ICNNClassifyPredInfo* GetPredInfo(int nPredIndex) = 0;
            /**
             * @brief 预测信息的数量
             * @deprecated V4.4废弃
             */
            virtual int GetPredNum() = 0;
            /**
             * @brief 预测信息的置信度
             * @deprecated V4.4废弃
             */
            virtual float GetPredConfidence() = 0;
			
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
             * @brief 类别概率
             * @param nIndex[IN] 数据索引
             */
             virtual float GetClassProbs(int nIndex) = 0;

             /**
             * @brief 类别标签
             * @param nIndex[IN] 数据索引
             */
             virtual int GetClassLabels(int nIndex) = 0;

             /**
             * @brief 置信度
             * @param nIndex[IN] 数据索引
             */
             virtual float GetConfidences(int nIndex) = 0;

             /**
             * @brief 类别个数
             * @param nIndex[IN] 数据索引
             */
             virtual int GetClassNums(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CnnClassify tool.  */
        ///DL分类工具类
        class IMVSCnnClassifyModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCnnClassifyModuTool() {}
            virtual ~IMVSCnnClassifyModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CnnClassifyParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CnnClassifyResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSCnnClassifyModuTool(IMVSCnnClassifyModuTool&);
            IMVSCnnClassifyModuTool& operator=(IMVSCnnClassifyModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CnnClassify.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSCnnClassifyModu::IMVSCnnClassifyModuTool * __stdcall GetCnnClassifyToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CNNCLASSIFY_H_
