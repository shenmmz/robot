/***************************************************************************************************
 * File：IVmCnnUnSupervisedClassify.h
 * Note：@~chinese DL无监督分类GPU模块接口声明 @~english Interface for the CnnUnSupervisedClassify tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CNNUNSUPERVISEDCLASSIFY_H_
#define _IVM_CNNUNSUPERVISEDCLASSIFY_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCnnUnSupervisedClassifyModu
    {
    /// \addtogroup DL无监督分类GPU
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///DL无监督分类GROI管理类
        class CnnUnSupervisedClassifyRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoisRectangle(RectBox* pRoiValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the CnnUnSupervisedClassifyParams.  */
        ///DL无监督分类G参数类
        class CnnUnSupervisedClassifyParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnUnSupervisedClassifyParams() {}
            virtual ~CnnUnSupervisedClassifyParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual CnnUnSupervisedClassifyRoiManager* GetModuRoiManager() = 0;

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

            /// @~chinese 置信度阈值，范围：[0,1]   @~english Confidence Threshold，range:[0,1]
            __declspec(property(put = SetConfidenceThreshold, get = GetConfidenceThreshold)) double ConfidenceThreshold;

            virtual double GetConfidenceThreshold() = 0;

            virtual void SetConfidenceThreshold(double value) = 0;
			
			/// @~chinese 个数判断   @~english Number Check
			__declspec(property(put = SetNumLimitEnable, get = GetNumLimitEnable)) bool NumLimitEnable;

			virtual bool GetNumLimitEnable() = 0;

			virtual void SetNumLimitEnable(bool value) = 0;

			/// @~chinese 个数范围，范围：[0,99999]   @~english Number Range，range:[0,99999]
			__declspec(property(put = SetNumLimitLow, get = GetNumLimitLow)) int NumLimitLow;

			virtual int GetNumLimitLow() = 0;

			virtual void SetNumLimitLow(int value) = 0;

			/// @~chinese 个数范围，范围：[0,99999]   @~english Number Range，range:[0,99999]
			__declspec(property(put = SetNumLimitHigh, get = GetNumLimitHigh)) int NumLimitHigh;

			virtual int GetNumLimitHigh() = 0;

			virtual void SetNumLimitHigh(int value) = 0;

			/// @~chinese 置信度判断   @~english Confidence Check
			__declspec(property(put = SetScoreLimitEnable, get = GetScoreLimitEnable)) bool ScoreLimitEnable;

			virtual bool GetScoreLimitEnable() = 0;

			virtual void SetScoreLimitEnable(bool value) = 0;

			/// @~chinese 置信度范围，范围：[0.0,1.0]   @~english Confidence Range，range:[0.0,1.0]
			__declspec(property(put = SetScoreLimitLow, get = GetScoreLimitLow)) double ScoreLimitLow;

			virtual double GetScoreLimitLow() = 0;

			virtual void SetScoreLimitLow(double value) = 0;

			/// @~chinese 置信度范围，范围：[0.0,1.0]   @~english Confidence Range，range:[0.0,1.0]
			__declspec(property(put = SetScoreLimitHigh, get = GetScoreLimitHigh)) double ScoreLimitHigh;

			virtual double GetScoreLimitHigh() = 0;

			virtual void SetScoreLimitHigh(double value) = 0;

			/// @~chinese 预测状态判断 @~english PredictStatus Check
			__declspec(property(put = SetPredictStatusLimitEnable, get = GetPredictStatusLimitEnable)) bool PredictStatusLimitEnable;

			virtual bool GetPredictStatusLimitEnable() = 0;

			virtual void SetPredictStatusLimitEnable(bool value) = 0;

			/// @~chinese 预测状态范围，范围：[0,1] @~english PredictStatus Range, Range:[0,1]
			__declspec(property(put = SetPredictStatusLimitLow, get = GetPredictStatusLimitLow)) int PredictStatusLimitLow;

			virtual int GetPredictStatusLimitLow() = 0;

			virtual void SetPredictStatusLimitLow(int value) = 0;

			/// @~chinese 预测状态范围，范围：[0,1] @~english PredictStatus Range, Range:[0,1]
			__declspec(property(put = SetPredictStatusLimitHigh, get = GetPredictStatusLimitHigh)) int PredictStatusLimitHigh;

			virtual int GetPredictStatusLimitHigh() = 0;

			virtual void SetPredictStatusLimitHigh(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the CnnUnSupervisedClassifyResults.  */
        ///DL无监督分类GPU结果类
        class CnnUnSupervisedClassifyResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnUnSupervisedClassifyResults() {}
            virtual ~CnnUnSupervisedClassifyResults() {}

        public:
             /**
             * @brief 检测区域
             * @param nIndex[IN] 数据索引
             */
             virtual RectBox GetROI(int nIndex) = 0;

             /**
             * @brief ROI序号
             * @param nIndex[IN] 数据索引
             */
             virtual int GetClassRoiIndexs(int nIndex) = 0;

             /**
             * @brief 检测区域个数
             */
             virtual int GetRoiNum() = 0;

             /**
             * @brief 预测状态
             * @param nIndex[IN] 数据索引
             */
             virtual int GetPredictStatus(int nIndex) = 0;

             /**
             * @brief 置信度
             * @param nIndex[IN] 数据索引
             */
             virtual float GetConfidence(int nIndex) = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CnnUnSupervisedClassify tool.  */
        ///DL无监督分类GPU工具类
        class IMVSCnnUnSupervisedClassifyModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCnnUnSupervisedClassifyModuTool() {}
            virtual ~IMVSCnnUnSupervisedClassifyModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CnnUnSupervisedClassifyParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CnnUnSupervisedClassifyResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSCnnUnSupervisedClassifyModuTool(IMVSCnnUnSupervisedClassifyModuTool&);
            IMVSCnnUnSupervisedClassifyModuTool& operator=(IMVSCnnUnSupervisedClassifyModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
#endif    ///< _IVM_CNNUNSUPERVISEDCLASSIFY_H_
