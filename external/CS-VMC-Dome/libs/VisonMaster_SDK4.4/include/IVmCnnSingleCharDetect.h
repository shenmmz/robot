/***************************************************************************************************
 * File：IVmCnnSingleCharDetect.h
 * Note：@~chinese DL单字符检测模块接口声明 @~english Interface for the CnnSingleCharDetect tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CNNSINGLECHARDETECT_H_
#define _IVM_CNNSINGLECHARDETECT_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCnnSingleCharDetectModu
    {
        /// @~chinese 目标排序   @~english Sort Object
        typedef enum _SortObjectModeEnum
        {
            /// @~chinese 按中心点X坐标升序   @~english Sort by X low to high
            SortObjectMode_XCoordinate = 0x1,

            /// @~chinese 按中心点Y坐标升序   @~english Sort by Y low to high
            SortObjectMode_YCoordinate = 0x2,

            /// @~chinese 按最小分数降序   @~english Sort by Score High to low
            SortObjectMode_Confidence = 0x3,

        }SortObjectModeEnum;

    /// \addtogroup DL单字符检测
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///DL单字符检测ROI管理类
        class CnnSingleCharDetectRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the CnnSingleCharDetectParams.  */
        ///DL单字符检测参数类
        class CnnSingleCharDetectParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnSingleCharDetectParams() {}
            virtual ~CnnSingleCharDetectParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual CnnSingleCharDetectRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 模型文件路径   @~english Model File Path
            __declspec(property(put = SetLoadModelPath, get = GetLoadModelPath)) const char* LoadModelPath;

            virtual const char* GetLoadModelPath() = 0;

            virtual void SetLoadModelPath(const char* value) = 0;

            /// @~chinese 方案存模型   @~english Save Model Data
            __declspec(property(put = SetSaveModelDataEnable, get = GetSaveModelDataEnable)) bool SaveModelDataEnable;

            virtual bool GetSaveModelDataEnable() = 0;

            virtual void SetSaveModelDataEnable(bool value) = 0;

            /// @~chinese 启用字符过滤   @~english Character Filtration Enable
            __declspec(property(put = SetFontFilterEnable, get = GetFontFilterEnable)) bool FontFilterEnable;

            virtual bool GetFontFilterEnable() = 0;

            virtual void SetFontFilterEnable(bool value) = 0;

            /// @~chinese 识别字符个数，范围：[1,32]   @~english Identify Character Quantity，range:[1,32]
            __declspec(property(put = SetFontFilterNum, get = GetFontFilterNum)) int FontFilterNum;

            virtual int GetFontFilterNum() = 0;

            virtual void SetFontFilterNum(int value) = 0;

            /// @~chinese 字符过滤信息   @~english Character Filtration Info.
            __declspec(property(put = SetFontFilterInfo, get = GetFontFilterInfo)) const char* FontFilterInfo;

            virtual const char* GetFontFilterInfo() = 0;

            virtual void SetFontFilterInfo(const char* value) = 0;

            /// @~chinese 最大查找个数，范围：[1,100]   @~english Max Number to Find，range:[1,100]
            __declspec(property(put = SetMaxObjNum, get = GetMaxObjNum)) int MaxObjNum;

            virtual int GetMaxObjNum() = 0;

            virtual void SetMaxObjNum(int value) = 0;

            /// @~chinese 最小置信度，范围：[0.01,1.0]   @~english Min Confidence，range:[0.01,1.0]
            __declspec(property(put = SetMinScore, get = GetMinScore)) double MinScore;

            virtual double GetMinScore() = 0;

            virtual void SetMinScore(double value) = 0;

            /// @~chinese 最大重叠率，范围：[0.01,1.0]   @~english Overlap Threshold，range:[0.01,1.0]
            __declspec(property(put = SetMaxOverlap, get = GetMaxOverlap)) double MaxOverlap;

            virtual double GetMaxOverlap() = 0;

            virtual void SetMaxOverlap(double value) = 0;

            /// @~chinese 目标排序   @~english Sort Object
            __declspec(property(put = SetSortObjectMode, get = GetSortObjectMode)) SortObjectModeEnum SortObjectMode;

            virtual SortObjectModeEnum GetSortObjectMode() = 0;

            virtual void SetSortObjectMode(SortObjectModeEnum value) = 0;

            /// @~chinese 边缘筛选使能   @~english Edge Filter Enable
            __declspec(property(put = SetOutRoiFilterEnable, get = GetOutRoiFilterEnable)) bool OutRoiFilterEnable;

            virtual bool GetOutRoiFilterEnable() = 0;

            virtual void SetOutRoiFilterEnable(bool value) = 0;

            /// @~chinese 最小边缘分数，范围：[0.01,1.0]   @~english Min Edge Score，range:[0.01,1.0]
            __declspec(property(put = SetMinEdgeScore, get = GetMinEdgeScore)) double MinEdgeScore;

            virtual double GetMinEdgeScore() = 0;

            virtual void SetMinEdgeScore(double value) = 0;

            /// @~chinese 文本宽度使能   @~english Text Line Width Enable
            __declspec(property(put = SetWidthEnable, get = GetWidthEnable)) bool WidthEnable;

            virtual bool GetWidthEnable() = 0;

            virtual void SetWidthEnable(bool value) = 0;

            /// @~chinese 宽度范围，范围：[1,4000]   @~english Width Range，range:[1,4000]
            __declspec(property(put = SetMinWidth, get = GetMinWidth)) int MinWidth;

            virtual int GetMinWidth() = 0;

            virtual void SetMinWidth(int value) = 0;

            /// @~chinese 宽度范围，范围：[1,4000]   @~english Width Range，range:[1,4000]
            __declspec(property(put = SetMaxWidth, get = GetMaxWidth)) int MaxWidth;

            virtual int GetMaxWidth() = 0;

            virtual void SetMaxWidth(int value) = 0;

            /// @~chinese 文本高度使能   @~english Text Line Height Enable
            __declspec(property(put = SetHeightEnable, get = GetHeightEnable)) bool HeightEnable;

            virtual bool GetHeightEnable() = 0;

            virtual void SetHeightEnable(bool value) = 0;

            /// @~chinese 高度范围，范围：[1,4000]   @~english Height Range，range:[1,4000]
            __declspec(property(put = SetMinHeight, get = GetMinHeight)) int MinHeight;

            virtual int GetMinHeight() = 0;

            virtual void SetMinHeight(int value) = 0;

            /// @~chinese 高度范围，范围：[1,4000]   @~english Height Range，range:[1,4000]
            __declspec(property(put = SetMaxHeight, get = GetMaxHeight)) int MaxHeight;

            virtual int GetMaxHeight() = 0;

            virtual void SetMaxHeight(int value) = 0;

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

            /// @~chinese 置信度范围，范围：[0,1]   @~english Confidence Range，range:[0,1]
            __declspec(property(put = SetScoreLimitLow, get = GetScoreLimitLow)) double ScoreLimitLow;

            virtual double GetScoreLimitLow() = 0;

            virtual void SetScoreLimitLow(double value) = 0;

            /// @~chinese 置信度范围，范围：[0,1]   @~english Confidence Range，range:[0,1]
            __declspec(property(put = SetScoreLimitHigh, get = GetScoreLimitHigh)) double ScoreLimitHigh;

            virtual double GetScoreLimitHigh() = 0;

            virtual void SetScoreLimitHigh(double value) = 0;

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
        #define MVD_CNN_CHAR_DETECT_TOP_N_MAX     (100)   // TOPN最大值

        /// 深度学习字符识别信息
        class IDLOCRPredictInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IDLOCRPredictInfo() {}
            virtual ~IDLOCRPredictInfo() {}

        public:
            /**
             * @brief TopN输出
             */
            virtual int GetTopN() = 0;
            /**
             * @brief 字符个数
             */
            virtual int GetCharNum(int nTopNIndex) = 0;
            /**
             * @brief TopN的字符串
             */
            virtual void GetTopNString(char pcCharInfo[MVD_CNN_CHAR_DETECT_TOP_N_MAX], int nTopNIndex) = 0;
            /**
             * @brief TopN的单字符置信度
             */
            virtual float GetTopNCharConfidence(int nTopNIndex) = 0;
            /**
             * @brief TopN的单字符位置信息
             */
            virtual RectBox GetTopNCharBox(int nTopNIndex) = 0;
            /**
             * @brief 每个字符框的置信度
             */
            virtual float GetBoxConfidence(int nTopNIndex) = 0;
            /**
             * @brief 字符校验是否生效
             */
            virtual int GetCorrectOKStatus(int nTopNIndex) = 0;
            /**
             * @brief 整体字符串置信度[0, 1]
             */
            virtual float GetStrConfidence(int nTopNIndex) = 0;
        };

        /*  Note: Interface Classes of the result of the CnnSingleCharDetectResults.  */
        ///DL单字符检测结果类
        class CnnSingleCharDetectResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnSingleCharDetectResults() {}
            virtual ~CnnSingleCharDetectResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;
            /**
             * @brief 文本行个数
             */
            virtual int GetTextLineNum() = 0;

            /**
             * @brief 文本框
             */
            virtual RectBox GetTextBox(int nTextIndex) = 0;

            /**
             * @brief 获取识别的字符信息
             * @param nTextIndex [IN] 拟查询的文本行索引，从0开始计数
             * @return 字符信息,出错时抛出异常
             */
            virtual IDLOCRPredictInfo* GetCharacterInfo(int nTextIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CnnSingleCharDetect tool.  */
        ///DL单字符检测工具类
        class IMVSCnnSingleCharDetectModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCnnSingleCharDetectModuTool() {}
            virtual ~IMVSCnnSingleCharDetectModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CnnSingleCharDetectParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CnnSingleCharDetectResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSCnnSingleCharDetectModuTool(IMVSCnnSingleCharDetectModuTool&);
            IMVSCnnSingleCharDetectModuTool& operator=(IMVSCnnSingleCharDetectModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CnnSingleCharDetect.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSCnnSingleCharDetectModu::IMVSCnnSingleCharDetectModuTool * __stdcall GetCnnSingleCharDetectToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CNNSINGLECHARDETECT_H_
