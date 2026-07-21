/***************************************************************************************************
 * File：IVmOcrDl.h
 * Note：@~chinese DL字符识别模块接口声明 @~english Interface for the OcrDl tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_OCRDL_H_
#define _IVM_OCRDL_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

#ifndef MVBOCRDL_MAX_TEXT_LEN
#define MVBOCRDL_MAX_TEXT_LEN (51)
#endif

#ifndef MVD_OCRDL_MAX_CHAR_NUM
#define MVD_OCRDL_MAX_CHAR_NUM (50)
#endif

namespace VisionMasterSDK
{
    namespace IMVSOcrDlModu
    {
    /// \addtogroup DL字符识别
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///DL字符识别GROI管理类
        class OcrDlRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoisRectangle(RectBox* pRoiValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the OcrDlParams.  */
        ///DL字符识别G参数类
        class OcrDlParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit OcrDlParams() {}
            virtual ~OcrDlParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual OcrDlRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 模型文件路径   @~english Model File Path
            __declspec(property(put = SetLoadModelPath, get = GetLoadModelPath)) const char* LoadModelPath;

            virtual const char* GetLoadModelPath() = 0;

            virtual void SetLoadModelPath(const char* value) = 0;

            /// @~chinese 方案存模型   @~english Save Model Data
            __declspec(property(put = SetSaveModelDataEnable, get = GetSaveModelDataEnable)) bool SaveModelDataEnable;

            virtual bool GetSaveModelDataEnable() = 0;

            virtual void SetSaveModelDataEnable(bool value) = 0;

            /// @~chinese 启用字符校验(仅对手绘模式下的ROI-1有效)   @~english Character Check Enable(Valid only for ROI-1 in draw mode)
            __declspec(property(put = SetFontFilterEnable, get = GetFontFilterEnable)) bool FontFilterEnable;

            virtual bool GetFontFilterEnable() = 0;

            virtual void SetFontFilterEnable(bool value) = 0;

            /// @~chinese 识别字符个数，范围：[1,32](仅对手绘模式下的ROI-1有效)   @~english Identify Character Quantity，range:[1,32](Valid only for ROI-1 in draw mode)
            __declspec(property(put = SetFontFilterNum, get = GetFontFilterNum)) int FontFilterNum;

            virtual int GetFontFilterNum() = 0;

            virtual void SetFontFilterNum(int value) = 0;

            /// @~chinese 字符过滤信息(仅对手绘模式下的ROI-1有效)   @~english Character Filtration Info(Valid only for ROI-1 in draw mode)
            __declspec(property(put = SetFontFilterInfo, get = GetFontFilterInfo)) const char* FontFilterInfo;

            virtual const char* GetFontFilterInfo() = 0;

            virtual void SetFontFilterInfo(const char* value) = 0;

            /// @~chinese 最小置信度，范围：[0.01,1.0]   @~english Min Confidence，range:[0.01,1.0]
            __declspec(property(put = SetMinConfidence, get = GetMinConfidence)) double MinConfidence;

            virtual double GetMinConfidence() = 0;

            virtual void SetMinConfidence(double value) = 0;

            /// @~chinese 最小翻转分数，范围：[0.0,1.0]   @~english Min Flip Score，range:[0.0,1.0]
            __declspec(property(put = SetFlipMinScore, get = GetFlipMinScore)) double FlipMinScore;

            virtual double GetFlipMinScore() = 0;

            virtual void SetFlipMinScore(double value) = 0;

            /// @~chinese 最大文本行查找个数，范围：[1,5]   @~english Max Text Line Number to Find，range:[1,5]
            __declspec(property(put = SetTextLineNum, get = GetTextLineNum)) int TextLineNum;

            virtual int GetTextLineNum() = 0;

            virtual void SetTextLineNum(int value) = 0;

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

            /// @~chinese 个数范围，范围：[0,99999]   @~english Number Range，range:[0,99999]
            __declspec(property(put = SetNumLimitLow, get = GetNumLimitLow)) int NumLimitLow;

            virtual int GetNumLimitLow() = 0;

            virtual void SetNumLimitLow(int value) = 0;

            /// @~chinese 个数范围，范围：[0,99999]   @~english Number Range，range:[0,99999]
            __declspec(property(put = SetNumLimitHigh, get = GetNumLimitHigh)) int NumLimitHigh;

            virtual int GetNumLimitHigh() = 0;

            virtual void SetNumLimitHigh(int value) = 0;

            /// @~chinese 字符验证   @~english Character Verification
            __declspec(property(put = SetVerifyEnable, get = GetVerifyEnable)) bool VerifyEnable;

            virtual bool GetVerifyEnable() = 0;

            virtual void SetVerifyEnable(bool value) = 0;

            /// @~chinese 数字集   @~english Number Set
            __declspec(property(put = SetNumVerifyEnable, get = GetNumVerifyEnable)) bool NumVerifyEnable;

            virtual bool GetNumVerifyEnable() = 0;

            virtual void SetNumVerifyEnable(bool value) = 0;

            /// @~chinese 小写字母集   @~english Lowercase Set
            __declspec(property(put = SetSmallAlphabetVerify, get = GetSmallAlphabetVerify)) bool SmallAlphabetVerify;

            virtual bool GetSmallAlphabetVerify() = 0;

            virtual void SetSmallAlphabetVerify(bool value) = 0;

            /// @~chinese 大写字母集   @~english Uppercase Set
            __declspec(property(put = SetBigAlphabetVerify, get = GetBigAlphabetVerify)) bool BigAlphabetVerify;

            virtual bool GetBigAlphabetVerify() = 0;

            virtual void SetBigAlphabetVerify(bool value) = 0;

            /// @~chinese 特殊字符集   @~english Special Character Set
            __declspec(property(put = SetSpecialCharVerify, get = GetSpecialCharVerify)) bool SpecialCharVerify;

            virtual bool GetSpecialCharVerify() = 0;

            virtual void SetSpecialCharVerify(bool value) = 0;

            /// @~chinese 用户字符验证   @~english UDC Verification
            __declspec(property(put = SetUserStringVerify, get = GetUserStringVerify)) bool UserStringVerify;

            virtual bool GetUserStringVerify() = 0;

            virtual void SetUserStringVerify(bool value) = 0;

            /// @~chinese 用户字符   @~english UDC
            __declspec(property(put = SetUserString, get = GetUserString)) const char* UserString;

            virtual const char* GetUserString() = 0;

            virtual void SetUserString(const char* value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
		/// DL字符识别单字符串信息
        class ICNNOCRStringInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit ICNNOCRStringInfo(){};
            virtual ~ICNNOCRStringInfo(){};
        public:
            /**
             * @brief 单个字符串的有效长度
             */
            virtual int GetInfoLen() = 0;
            /**
             * @brief 获取单个字符串内容
             */
            virtual void GetInfo(char pcCharInfo[MVBOCRDL_MAX_TEXT_LEN]) = 0;
            /**
             * @brief 获取字符串中单个字符的置信度
             */
            virtual void GetConfidence(float pfConfidence[MVD_OCRDL_MAX_CHAR_NUM]) = 0;
        };

        /*  Note: Interface Classes of the result of the OcrDlResults.  */
        ///DL字符识别结果类
        class OcrDlResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit OcrDlResults() {}
            virtual ~OcrDlResults() {}

        public:
            /**
            * @brief 模块状态
            */
            virtual int GetModuStatus() = 0;
            /**
            * @brief 检测区域
            */
            virtual RectBox GetROI(int nIndex) = 0;
            /**
             * @brief 检测区域状态
             */
            virtual int GetROIStatus(int nIndex) = 0;
            /**
             * @brief 识别字符串的总数量
             */
            virtual int GetPredictNum() = 0;
            /**
             * @brief 通过索引获取单个字符串信息
             */
            virtual ICNNOCRStringInfo* GetPredictInfo(int nPredictIndex) = 0;
			/**
             * @brief 获取模型标识
             */
            virtual const char * GetModelFlag() = 0;
            /**
             * @brief 获取top 1预测信息的总体置信度
             */
            virtual float GetTotalConfidence() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the OcrDl tool.  */
        ///DL字符识别工具类
        class IMVSOcrDlModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSOcrDlModuTool() {}
            virtual ~IMVSOcrDlModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual OcrDlParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual OcrDlResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSOcrDlModuTool(IMVSOcrDlModuTool&);
            IMVSOcrDlModuTool& operator=(IMVSOcrDlModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for OcrDl.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSOcrDlModu::IMVSOcrDlModuTool * __stdcall GetOcrDlToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_OCRDL_H_
