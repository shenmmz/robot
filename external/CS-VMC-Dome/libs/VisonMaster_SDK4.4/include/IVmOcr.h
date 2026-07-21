/***************************************************************************************************
 * File：IVmOcr.h
 * Note：@~chinese 字符识别模块接口声明 @~english Interface for the Ocr tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_OCR_H_
#define _IVM_OCR_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSOcrModu
    {
        /// @~chinese 字符极性   @~english Character Polarity
        typedef enum _PolarityEnum
        {
            /// @~chinese 白底黑字   @~english Dark On Light
            Polarity_DarkOnBright = 0x1,

            /// @~chinese 黑底白字   @~english Light On Dark
            Polarity_BrightOnDark = 0x2,

        }PolarityEnum;

        /// @~chinese 宽度类型   @~english Width Type
        typedef enum _WidthTypeEnum
        {
            /// @~chinese 可变类型   @~english Changable Type
            WidthType_Changable = 0x0,

            /// @~chinese 等宽类型   @~english Aequilatus Type
            WidthType_Aequilatus = 0x1,

        }WidthTypeEnum;

        /// @~chinese 忽略边框   @~english Ignore Borders
        typedef enum _IgnoreBorderFragmentEnableEnum
        {
            /// @~chinese 否   @~english No
            IgnoreBorderFragmentEnable_Tab_No = 0x0,

            /// @~chinese 是   @~english Yes
            IgnoreBorderFragmentEnable_Tab_Yes = 0x1,

        }IgnoreBorderFragmentEnableEnum;

        /// @~chinese 分类方法   @~english Classification Method
        typedef enum _ClassMethodEnum
        {
            /// @~chinese 距离最近   @~english Nearest Distance
            ClassMethod_NearestDistance = 0x1,

            /// @~chinese 权重最高   @~english Weight Highest
            ClassMethod_WeightHighest = 0x2,

            /// @~chinese 频率最高   @~english Rate Fastest
            ClassMethod_RateFastest = 0x3,

        }ClassMethodEnum;

        /// @~chinese 字宽滤波使能   @~english Stroke Width Filter
        typedef enum _StrokeFilterEnableEnum
        {
            /// @~chinese 不使能   @~english Disabled
            StrokeFilterEnable_No = 0x0,

            /// @~chinese 使能   @~english Enabled
            StrokeFilterEnable_Yes = 0x1,

        }StrokeFilterEnableEnum;

        /// @~chinese 相似度类型   @~english Similarity Type
        typedef enum _SimilarFunEnum
        {
            /// @~chinese 欧式距离   @~english Euclidean Distance
            SimilarFun_Euclid = 0x1,

            /// @~chinese 余弦距离   @~english Cosine Distance
            SimilarFun_Cosine = 0x2,

        }SimilarFunEnum;

    /// \addtogroup 字符识别
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///字符识别ROI管理类
        class OcrRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the OcrParams.  */
        ///字符识别参数类
        class OcrParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit OcrParams() {}
            virtual ~OcrParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual OcrRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 启用字符过滤   @~english Character Filtration Enable
            __declspec(property(put = SetFontFilterEnable, get = GetFontFilterEnable)) bool FontFilterEnable;

            virtual bool GetFontFilterEnable() = 0;

            virtual void SetFontFilterEnable(bool value) = 0;

            /// @~chinese 识别字符个数，范围：[1,50]   @~english Identify Character Quantity，range:[1,50]
            __declspec(property(put = SetFontFilterNum, get = GetFontFilterNum)) int FontFilterNum;

            virtual int GetFontFilterNum() = 0;

            virtual void SetFontFilterNum(int value) = 0;

            /// @~chinese 字符过滤信息   @~english Character Filtration Info.
            __declspec(property(put = SetFontFilterInfo, get = GetFontFilterInfo)) const char* FontFilterInfo;

            virtual const char* GetFontFilterInfo() = 0;

            virtual void SetFontFilterInfo(const char* value) = 0;

            /// @~chinese 字符极性   @~english Character Polarity
            __declspec(property(put = SetPolarity, get = GetPolarity)) PolarityEnum Polarity;

            virtual PolarityEnum GetPolarity() = 0;

            virtual void SetPolarity(PolarityEnum value) = 0;

            /// @~chinese 字符宽度范围，范围：[1,512]   @~english Character Width Range，range:[1,512]
            __declspec(property(put = SetMinCharWidth, get = GetMinCharWidth)) int MinCharWidth;

            virtual int GetMinCharWidth() = 0;

            virtual void SetMinCharWidth(int value) = 0;

            /// @~chinese 字符宽度范围，范围：[1,512]   @~english Character Width Range，range:[1,512]
            __declspec(property(put = SetMaxCharWidth, get = GetMaxCharWidth)) int MaxCharWidth;

            virtual int GetMaxCharWidth() = 0;

            virtual void SetMaxCharWidth(int value) = 0;

            /// @~chinese 宽度类型   @~english Width Type
            __declspec(property(put = SetWidthType, get = GetWidthType)) WidthTypeEnum WidthType;

            virtual WidthTypeEnum GetWidthType() = 0;

            virtual void SetWidthType(WidthTypeEnum value) = 0;

            /// @~chinese 字符高度范围，范围：[1,512]   @~english Character Height Range，range:[1,512]
            __declspec(property(put = SetMinCharHeight, get = GetMinCharHeight)) int MinCharHeight;

            virtual int GetMinCharHeight() = 0;

            virtual void SetMinCharHeight(int value) = 0;

            /// @~chinese 字符高度范围，范围：[1,512]   @~english Character Height Range，range:[1,512]
            __declspec(property(put = SetMaxCharHeight, get = GetMaxCharHeight)) int MaxCharHeight;

            virtual int GetMaxCharHeight() = 0;

            virtual void SetMaxCharHeight(int value) = 0;

            /// @~chinese 二值化系数，范围：[1,99]   @~english Binary Ratio，range:[1,99]
            __declspec(property(put = SetBinaryCoef, get = GetBinaryCoef)) int BinaryCoef;

            virtual int GetBinaryCoef() = 0;

            virtual void SetBinaryCoef(int value) = 0;

            /// @~chinese 片段面积范围，范围：[0,100000]   @~english Area of fragment area，range:[0,100000]
            __declspec(property(put = SetMinCharArea, get = GetMinCharArea)) int MinCharArea;

            virtual int GetMinCharArea() = 0;

            virtual void SetMinCharArea(int value) = 0;

            /// @~chinese 片段面积范围，范围：[0,100000]   @~english Area of fragment area，range:[0,100000]
            __declspec(property(put = SetMaxCharArea, get = GetMaxCharArea)) int MaxCharArea;

            virtual int GetMaxCharArea() = 0;

            virtual void SetMaxCharArea(int value) = 0;

            /// @~chinese 合格阈值，范围：[0,100]   @~english Accept Threshold，range:[0,100]
            __declspec(property(put = SetAcceptThreshold, get = GetAcceptThreshold)) int AcceptThreshold;

            virtual int GetAcceptThreshold() = 0;

            virtual void SetAcceptThreshold(int value) = 0;

            /// @~chinese 距离阈值，范围：[0,100]   @~english Distance Threshold，range:[0,100]
            __declspec(property(put = SetMainLineDistThresh, get = GetMainLineDistThresh)) int MainLineDistThresh;

            virtual int GetMainLineDistThresh() = 0;

            virtual void SetMainLineDistThresh(int value) = 0;

            /// @~chinese 忽略边框   @~english Ignore Borders
            __declspec(property(put = SetIgnoreBorderFragmentEnable, get = GetIgnoreBorderFragmentEnable)) IgnoreBorderFragmentEnableEnum IgnoreBorderFragmentEnable;

            virtual IgnoreBorderFragmentEnableEnum GetIgnoreBorderFragmentEnable() = 0;

            virtual void SetIgnoreBorderFragmentEnable(IgnoreBorderFragmentEnableEnum value) = 0;

            /// @~chinese 主方向范围，范围：[0,45]   @~english Main Direction Range，range:[0,45]
            __declspec(property(put = SetOrientHalfRange, get = GetOrientHalfRange)) int OrientHalfRange;

            virtual int GetOrientHalfRange() = 0;

            virtual void SetOrientHalfRange(int value) = 0;

            /// @~chinese 倾斜角范围，范围：[0,45]   @~english Tilt Range，range:[0,45]
            __declspec(property(put = SetSlantHalfRange, get = GetSlantHalfRange)) int SlantHalfRange;

            virtual int GetSlantHalfRange() = 0;

            virtual void SetSlantHalfRange(int value) = 0;

            /// @~chinese 字符最小间隙，范围：[0,1000]   @~english Min Char Gap，range:[0,1000]
            __declspec(property(put = SetMinInterCharGap, get = GetMinInterCharGap)) int MinInterCharGap;

            virtual int GetMinInterCharGap() = 0;

            virtual void SetMinInterCharGap(int value) = 0;

            /// @~chinese 行间最小间隙，范围：[0,1000]   @~english Min Text Gap，range:[0,1000]
            __declspec(property(put = SetMinInterTextGap, get = GetMinInterTextGap)) int MinInterTextGap;

            virtual int GetMinInterTextGap() = 0;

            virtual void SetMinInterTextGap(int value) = 0;

            /// @~chinese 最大宽高比，范围：[1,1000]   @~english Max Aspect Ratio，range:[1,1000]
            __declspec(property(put = SetMaxLengthWidthRatio, get = GetMaxLengthWidthRatio)) int MaxLengthWidthRatio;

            virtual int GetMaxLengthWidthRatio() = 0;

            virtual void SetMaxLengthWidthRatio(int value) = 0;

            /// @~chinese 分类方法   @~english Classification Method
            __declspec(property(put = SetClassMethod, get = GetClassMethod)) ClassMethodEnum ClassMethod;

            virtual ClassMethodEnum GetClassMethod() = 0;

            virtual void SetClassMethod(ClassMethodEnum value) = 0;

            /// @~chinese 字宽滤波使能   @~english Stroke Width Filter
            __declspec(property(put = SetStrokeFilterEnable, get = GetStrokeFilterEnable)) StrokeFilterEnableEnum StrokeFilterEnable;

            virtual StrokeFilterEnableEnum GetStrokeFilterEnable() = 0;

            virtual void SetStrokeFilterEnable(StrokeFilterEnableEnum value) = 0;

            /// @~chinese 笔画宽度范围，范围：[1,64]   @~english Stroke Width Range，range:[1,64]
            __declspec(property(put = SetMinStrokeWidth, get = GetMinStrokeWidth)) int MinStrokeWidth;

            virtual int GetMinStrokeWidth() = 0;

            virtual void SetMinStrokeWidth(int value) = 0;

            /// @~chinese 笔画宽度范围，范围：[1,64]   @~english Stroke Width Range，range:[1,64]
            __declspec(property(put = SetMaxStrokeWidth, get = GetMaxStrokeWidth)) int MaxStrokeWidth;

            virtual int GetMaxStrokeWidth() = 0;

            virtual void SetMaxStrokeWidth(int value) = 0;

            /// @~chinese 相似度类型   @~english Similarity Type
            __declspec(property(put = SetSimilarFun, get = GetSimilarFun)) SimilarFunEnum SimilarFun;

            virtual SimilarFunEnum GetSimilarFun() = 0;

            virtual void SetSimilarFun(SimilarFunEnum value) = 0;

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
        /// 字符识别信息
        class IOCRPredictInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IOCRPredictInfo() {}
            virtual ~IOCRPredictInfo() {}

        public:
            /**
             * @brief 位置信息
             */
            virtual RectBox GetBox() = 0;
            /**
             * @brief 倾斜角度
             */
            virtual float GetSkewAngle() = 0;
            /**
             * @brief 字符标签信息
             */
            virtual char GetLabel() = 0;
            /**
             * @brief 备选字符标签信息
             */
            virtual char GetAlternativeLabel() = 0;
            /**
             * @brief 字符标签得分
             */
            virtual float GetLabelScore() = 0;
            /**
             * @brief 备选字符标签得分
             */
            virtual float GetAlternativeLabelScore() = 0;
            /**
             * @brief 置信度
             */
            virtual float GetConfidence() = 0;
        };

        /*  Note: Interface Classes of the result of the OcrResults.  */
        ///字符识别结果类
        class OcrResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit OcrResults() {}
            virtual ~OcrResults() {}

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
             * @brief 获取字符数量
             * @return 字符数量
             */
            virtual int GetCharacterNum() = 0;

            /**
             * @brief 获取识别的字符信息
             * @param nIndex [IN] 拟查询的字符索引，从0开始计数
             * @return 字符信息,出错时抛出异常
             */
            virtual IOCRPredictInfo* GetPredictInfo(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the Ocr tool.  */
        ///字符识别工具类
        class IMVSOcrModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSOcrModuTool() {}
            virtual ~IMVSOcrModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual OcrParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual OcrResults* GetResult() = 0;

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
            IMVSOcrModuTool(IMVSOcrModuTool&);
            IMVSOcrModuTool& operator=(IMVSOcrModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for Ocr.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSOcrModu::IMVSOcrModuTool * __stdcall GetOcrToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_OCR_H_
