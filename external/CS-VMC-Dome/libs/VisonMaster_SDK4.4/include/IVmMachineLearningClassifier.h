/***************************************************************************************************
 * File：IVmMachineLearningClassifier.h
 * Note：@~chinese ML分类模块接口声明 @~english Interface for the MachineLearningClassifier tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_MACHINELEARNINGCLASSIFIER_H_
#define _IVM_MACHINELEARNINGCLASSIFIER_H_

#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSMachineLearningClassifierModu
    {
        /// @~chinese 分类器类型   @~english Classify Type
        typedef enum _ClassifyTypeEnum
        {
            /// @~chinese SVM分类器   @~english SVM Classify
            ClassifyType_SVM = 0x0,

            /// @~chinese RF分类器   @~english RF Classify
            ClassifyType_RF = 0x1,

        }ClassifyTypeEnum;

    /// \addtogroup ML分类
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///ML分类ROI管理类
        class MachineLearningClassifierRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the MachineLearningClassifierParams.  */
        ///ML分类参数类
        class MachineLearningClassifierParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MachineLearningClassifierParams() {}
            virtual ~MachineLearningClassifierParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual MachineLearningClassifierRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 模型文件路径   @~english Model File Path
            __declspec(property(put = SetLoadModelPath, get = GetLoadModelPath)) const char* LoadModelPath;

            virtual const char* GetLoadModelPath() = 0;

            virtual void SetLoadModelPath(const char* value) = 0;

            /// @~chinese 方案存模型   @~english Save Model Data
            __declspec(property(put = SetSaveModelDataEnable, get = GetSaveModelDataEnable)) bool SaveModelDataEnable;

            virtual bool GetSaveModelDataEnable() = 0;

            virtual void SetSaveModelDataEnable(bool value) = 0;

            /// @~chinese 最大迭代次数，范围：[10,100000](弃用)   @~english Svm MaxIter，range:[10,100000](Obsolete)
            __declspec(property(put = SetSvmMaxIter, get = GetSvmMaxIter)) int SvmMaxIter;

            virtual int GetSvmMaxIter() = 0;

            virtual void SetSvmMaxIter(int value) = 0;

            /// @~chinese 最小迭代误差，范围：[1,100000](弃用)   @~english minimum iteration error，range:[1,100000](Obsolete)
            __declspec(property(put = SetSvmEsp, get = GetSvmEsp)) int SvmEsp;

            virtual int GetSvmEsp() = 0;

            virtual void SetSvmEsp(int value) = 0;

            /// @~chinese 分类器类型(弃用)   @~english Classify Type(Obsolete)
            __declspec(property(put = SetClassifyType, get = GetClassifyType)) ClassifyTypeEnum ClassifyType;

            virtual ClassifyTypeEnum GetClassifyType() = 0;

            virtual void SetClassifyType(ClassifyTypeEnum value) = 0;

            /// @~chinese 纹理特征(弃用)   @~english Texture Feature(Obsolete)
            __declspec(property(put = SetTextureFeature, get = GetTextureFeature)) bool TextureFeature;

            virtual bool GetTextureFeature() = 0;

            virtual void SetTextureFeature(bool value) = 0;

            /// @~chinese HOG(弃用)   @~english HOG(Obsolete)
            __declspec(property(put = SetHogFeature, get = GetHogFeature)) bool HogFeature;

            virtual bool GetHogFeature() = 0;

            virtual void SetHogFeature(bool value) = 0;

            /// @~chinese LBP(弃用)   @~english LBP(Obsolete)
            __declspec(property(put = SetLbpFeature, get = GetLbpFeature)) bool LbpFeature;

            virtual bool GetLbpFeature() = 0;

            virtual void SetLbpFeature(bool value) = 0;

            /// @~chinese GLCM(弃用)   @~english GLCM(Obsolete)
            __declspec(property(put = SetGlcmFeature, get = GetGlcmFeature)) bool GlcmFeature;

            virtual bool GetGlcmFeature() = 0;

            virtual void SetGlcmFeature(bool value) = 0;

            /// @~chinese 边缘密度(弃用)   @~english Edge(Obsolete)
            __declspec(property(put = SetEdgeFeature, get = GetEdgeFeature)) bool EdgeFeature;

            virtual bool GetEdgeFeature() = 0;

            virtual void SetEdgeFeature(bool value) = 0;

            /// @~chinese 极坐标投影(弃用)   @~english Polar(Obsolete)
            __declspec(property(put = SetPolarFeature, get = GetPolarFeature)) bool PolarFeature;

            virtual bool GetPolarFeature() = 0;

            virtual void SetPolarFeature(bool value) = 0;

            /// @~chinese 灰度特征(弃用)   @~english Gray Feature(Obsolete)
            __declspec(property(put = SetGrayFeature, get = GetGrayFeature)) bool GrayFeature;

            virtual bool GetGrayFeature() = 0;

            virtual void SetGrayFeature(bool value) = 0;

            /// @~chinese 灰度范围(弃用)   @~english GrayRange(Obsolete)
            __declspec(property(put = SetGrayRangeFeature, get = GetGrayRangeFeature)) bool GrayRangeFeature;

            virtual bool GetGrayRangeFeature() = 0;

            virtual void SetGrayRangeFeature(bool value) = 0;

            /// @~chinese 灰度均值(弃用)   @~english GrayMean(Obsolete)
            __declspec(property(put = SetGrayMeanFeature, get = GetGrayMeanFeature)) bool GrayMeanFeature;

            virtual bool GetGrayMeanFeature() = 0;

            virtual void SetGrayMeanFeature(bool value) = 0;

            /// @~chinese 灰度方差(弃用)   @~english Gray Deviation(Obsolete)
            __declspec(property(put = SetGrayDeviationFeature, get = GetGrayDeviationFeature)) bool GrayDeviationFeature;

            virtual bool GetGrayDeviationFeature() = 0;

            virtual void SetGrayDeviationFeature(bool value) = 0;

            /// @~chinese 灰度能量(弃用)   @~english GrayEntropy(Obsolete)
            __declspec(property(put = SetGrayEntropyFeature, get = GetGrayEntropyFeature)) bool GrayEntropyFeature;

            virtual bool GetGrayEntropyFeature() = 0;

            virtual void SetGrayEntropyFeature(bool value) = 0;

            /// @~chinese 各向异性(弃用)   @~english GrayAnisotropy(Obsolete)
            __declspec(property(put = SetGrayAnisotropyFeature, get = GetGrayAnisotropyFeature)) bool GrayAnisotropyFeature;

            virtual bool GetGrayAnisotropyFeature() = 0;

            virtual void SetGrayAnisotropyFeature(bool value) = 0;

            /// @~chinese 水平投影(弃用)   @~english GrayProjHor(Obsolete)
            __declspec(property(put = SetGrayProjHorFeature, get = GetGrayProjHorFeature)) bool GrayProjHorFeature;

            virtual bool GetGrayProjHorFeature() = 0;

            virtual void SetGrayProjHorFeature(bool value) = 0;

            /// @~chinese 垂直投影(弃用)   @~english GrayProjVert(Obsolete)
            __declspec(property(put = SetGrayProjVertFeature, get = GetGrayProjVertFeature)) bool GrayProjVertFeature;

            virtual bool GetGrayProjVertFeature() = 0;

            virtual void SetGrayProjVertFeature(bool value) = 0;

            /// @~chinese 灰度直方图(弃用)   @~english Gray His(Obsolete)
            __declspec(property(put = SetGrayHis, get = GetGrayHis)) bool GrayHis;

            virtual bool GetGrayHis() = 0;

            virtual void SetGrayHis(bool value) = 0;

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

            /// @~chinese 标签名判断   @~english LabelName Check
            __declspec(property(put = SetLabelNameLimitEnable, get = GetLabelNameLimitEnable)) bool LabelNameLimitEnable;

            virtual bool GetLabelNameLimitEnable() = 0;

            virtual void SetLabelNameLimitEnable(bool value) = 0;

            /// @~chinese 标签名   @~english Label Name
            __declspec(property(put = SetLabelNameLimit, get = GetLabelNameLimit)) const char* LabelNameLimit;

            virtual const char* GetLabelNameLimit() = 0;

            virtual void SetLabelNameLimit(const char* value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the MachineLearningClassifierResults.  */
        ///ML分类结果类
        class MachineLearningClassifierResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MachineLearningClassifierResults() {}
            virtual ~MachineLearningClassifierResults() {}

        public:
             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

			 /**
			 * @brief 预测信息个数
			 */
			 virtual int GetPredictNum() = 0;

			 /**
			 * @brief 标签索引
			 */
			 virtual int GetLabelsIndex(int nIndex) = 0;

			 /**
			 * @brief 标签名
			 */
			 virtual string GetLabelName(int nIndex) = 0;

        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the MachineLearningClassifier tool.  */
        ///ML分类工具类
        class IMVSMachineLearningClassifierModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSMachineLearningClassifierModuTool() {}
            virtual ~IMVSMachineLearningClassifierModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual MachineLearningClassifierParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual MachineLearningClassifierResults* GetResult() = 0;

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
            IMVSMachineLearningClassifierModuTool(IMVSMachineLearningClassifierModuTool&);
            IMVSMachineLearningClassifierModuTool& operator=(IMVSMachineLearningClassifierModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for MachineLearningClassifier.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSMachineLearningClassifierModu::IMVSMachineLearningClassifierModuTool * __stdcall GetMachineLearningClassifierToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_MACHINELEARNINGCLASSIFIER_H_
