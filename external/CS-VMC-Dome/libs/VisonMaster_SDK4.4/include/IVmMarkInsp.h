/***************************************************************************************************
 * File：IVmMarkInsp.h
 * Note：@~chinese 字符缺陷检测模块接口声明 @~english Interface for the MarkInsp tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_MARKINSP_H_
#define _IVM_MARKINSP_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSMarkInspModu
    {
        /// @~chinese 归一化类型   @~english Normalization Type
        typedef enum _NormlizeTypeEnum
        {
            /// @~chinese 不处理   @~english Not Deal
            NormlizeType_Processless = 0x0,

            /// @~chinese 直方图均衡化   @~english Histogram Equalization
            NormlizeType_HistEqualize = 0x1,

            /// @~chinese 直方图归一化   @~english Histogram Normalization
            NormlizeType_HistNormalize = 0x2,

            /// @~chinese 均值标准差归一化   @~english Standard Deviation Normalization
            NormlizeType_StdDevNormalize = 0x3,

        }NormlizeTypeEnum;

        /// @~chinese 统计训练方法   @~english Statistical Training Type
        typedef enum _MarkModelTypeEnum
        {
            /// @~chinese 均值标准差法   @~english Mean Standard Deviation Type
            MarkModelType_MeanStdDev = 0x1,

            /// @~chinese 相关法   @~english Correlation Type
            MarkModelType_Correlation = 0x2,

        }MarkModelTypeEnum;

        /// @~chinese 缺陷类型   @~english DefectType
        typedef enum _FlawTypeEnum
        {
            /// @~chinese 亮缺陷   @~english Bright Defect
            FlawType_NonNormlize = 0x1,

            /// @~chinese 暗缺陷   @~english Dark Defect
            FlawType_HistEqual = 0x2,

            /// @~chinese 亮暗缺陷   @~english Bright and Dark Defect
            FlawType_HistMatch = 0x3,

        }FlawTypeEnum;

        /// @~chinese 阈值类型   @~english Threshold Type
        typedef enum _ExactThresholdFlagEnum
        {
            /// @~chinese 自动阈值   @~english Auto Threshold
            ExactThresholdFlag_AutoThreshold = 0x0,

            /// @~chinese 模板阈值   @~english Model Threshold
            ExactThresholdFlag_ModelThreshold = 0x1,

            /// @~chinese 手动阈值   @~english Manual Threshold
            ExactThresholdFlag_ManualThreshold = 0x2,

        }ExactThresholdFlagEnum;

        /// @~chinese 匹配极性   @~english Match Polarity
        typedef enum _MatchPolarityEnum
        {
            /// @~chinese 不考虑极性   @~english Ignored
            MatchPolarity_No = 0x0,

            /// @~chinese 考虑极性   @~english Considered
            MatchPolarity_Yes = 0x1,

        }MatchPolarityEnum;

        /// @~chinese 阈值类型   @~english Threshold Type
        typedef enum _MatchThresholdFlagEnum
        {
            /// @~chinese 自动阈值   @~english Auto Threshold
            MatchThresholdFlag_AutoThreshold = 0x0,

            /// @~chinese 模板阈值   @~english Model Threshold
            MatchThresholdFlag_ModelThreshold = 0x1,

            /// @~chinese 手动阈值   @~english Manual Threshold
            MatchThresholdFlag_ManualThreshold = 0x2,

        }MatchThresholdFlagEnum;

    /// \addtogroup 字符缺陷检测
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///字符缺陷检测ROI管理类
        class MarkInspRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the MarkInspParams.  */
        ///字符缺陷检测参数类
        class MarkInspParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MarkInspParams() {}
            virtual ~MarkInspParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual MarkInspRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 归一化类型   @~english Normalization Type
            __declspec(property(put = SetNormlizeType, get = GetNormlizeType)) NormlizeTypeEnum NormlizeType;

            virtual NormlizeTypeEnum GetNormlizeType() = 0;

            virtual void SetNormlizeType(NormlizeTypeEnum value) = 0;

            /// @~chinese 直方图比例，范围：[0.0,1]   @~english Histogram Ratio，range:[0.0,1]
            __declspec(property(put = SetNormLeftPercent, get = GetNormLeftPercent)) double NormLeftPercent;

            virtual double GetNormLeftPercent() = 0;

            virtual void SetNormLeftPercent(double value) = 0;

            /// @~chinese 直方图比例，范围：[0.0,1]   @~english Histogram Ratio，range:[0.0,1]
            __declspec(property(put = SetNormRightPercent, get = GetNormRightPercent)) double NormRightPercent;

            virtual double GetNormRightPercent() = 0;

            virtual void SetNormRightPercent(double value) = 0;

            /// @~chinese 统计训练方法   @~english Statistical Training Type
            __declspec(property(put = SetMarkModelType, get = GetMarkModelType)) MarkModelTypeEnum MarkModelType;

            virtual MarkModelTypeEnum GetMarkModelType() = 0;

            virtual void SetMarkModelType(MarkModelTypeEnum value) = 0;

            /// @~chinese 相关分数阈值，范围：[0,100]   @~english Correlation Score Threshold，range:[0,100]
            __declspec(property(put = SetCorreScore, get = GetCorreScore)) int CorreScore;

            virtual int GetCorreScore() = 0;

            virtual void SetCorreScore(int value) = 0;

            /// @~chinese 高度方向区块，范围：[1,5]   @~english Blocks in Height，range:[1,5]
            __declspec(property(put = SetBlockNumX, get = GetBlockNumX)) int BlockNumX;

            virtual int GetBlockNumX() = 0;

            virtual void SetBlockNumX(int value) = 0;

            /// @~chinese 宽度方向区块，范围：[1,5]   @~english Blocks in Width，range:[1,5]
            __declspec(property(put = SetBlockNumY, get = GetBlockNumY)) int BlockNumY;

            virtual int GetBlockNumY() = 0;

            virtual void SetBlockNumY(int value) = 0;

            /// @~chinese 高阈值比例，范围：[0.0,8.0]   @~english High Threshold Ratio，range:[0.0,8.0]
            __declspec(property(put = SetHighScale, get = GetHighScale)) double HighScale;

            virtual double GetHighScale() = 0;

            virtual void SetHighScale(double value) = 0;

            /// @~chinese 高阈值容忍，范围：[0.0,255.0]   @~english High Threshold Tolerance，range:[0.0,255.0]
            __declspec(property(put = SetHighOffset, get = GetHighOffset)) double HighOffset;

            virtual double GetHighOffset() = 0;

            virtual void SetHighOffset(double value) = 0;

            /// @~chinese 低阈值比例，范围：[0.0,8.0]   @~english Low Threshold Ratio，range:[0.0,8.0]
            __declspec(property(put = SetLowScale, get = GetLowScale)) double LowScale;

            virtual double GetLowScale() = 0;

            virtual void SetLowScale(double value) = 0;

            /// @~chinese 低阈值容忍，范围：[0.0,255.0]   @~english Low Threshold Tolerance，range:[0.0,255.0]
            __declspec(property(put = SetLowOffset, get = GetLowOffset)) double LowOffset;

            virtual double GetLowOffset() = 0;

            virtual void SetLowOffset(double value) = 0;

            /// @~chinese 缺陷类型   @~english DefectType
            __declspec(property(put = SetFlawType, get = GetFlawType)) FlawTypeEnum FlawType;

            virtual FlawTypeEnum GetFlawType() = 0;

            virtual void SetFlawType(FlawTypeEnum value) = 0;

            /// @~chinese 容忍度开关   @~english Tolerance Flag
            __declspec(property(put = SetToleranceFlag, get = GetToleranceFlag)) bool ToleranceFlag;

            virtual bool GetToleranceFlag() = 0;

            virtual void SetToleranceFlag(bool value) = 0;

            /// @~chinese 容忍度数值，范围：[1,10]   @~english Tolerance Value，range:[1,10]
            __declspec(property(put = SetToleranceValue, get = GetToleranceValue)) int ToleranceValue;

            virtual int GetToleranceValue() = 0;

            virtual void SetToleranceValue(int value) = 0;

            /// @~chinese 面积大小阈值，范围：[1,1000]   @~english Area Threshold，range:[1,1000]
            __declspec(property(put = SetAreaThresh, get = GetAreaThresh)) int AreaThresh;

            virtual int GetAreaThresh() = 0;

            virtual void SetAreaThresh(int value) = 0;

            /// @~chinese 最小匹配分数，范围：[0.1,1.0]   @~english Min Match Score，range:[0.1,1.0]
            __declspec(property(put = SetMatchMarkMinScore, get = GetMatchMarkMinScore)) double MatchMarkMinScore;

            virtual double GetMatchMarkMinScore() = 0;

            virtual void SetMatchMarkMinScore(double value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetMatchMarkAngleStart, get = GetMatchMarkAngleStart)) int MatchMarkAngleStart;

            virtual int GetMatchMarkAngleStart() = 0;

            virtual void SetMatchMarkAngleStart(int value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetMatchMarkAngleEnd, get = GetMatchMarkAngleEnd)) int MatchMarkAngleEnd;

            virtual int GetMatchMarkAngleEnd() = 0;

            virtual void SetMatchMarkAngleEnd(int value) = 0;

            /// @~chinese 阈值类型   @~english Threshold Type
            __declspec(property(put = SetExactThresholdFlag, get = GetExactThresholdFlag)) ExactThresholdFlagEnum ExactThresholdFlag;

            virtual ExactThresholdFlagEnum GetExactThresholdFlag() = 0;

            virtual void SetExactThresholdFlag(ExactThresholdFlagEnum value) = 0;

            /// @~chinese 边缘阈值，范围：[0,255]   @~english Edge Threshold，range:[0,255]
            __declspec(property(put = SetExactThreshold, get = GetExactThreshold)) int ExactThreshold;

            virtual int GetExactThreshold() = 0;

            virtual void SetExactThreshold(int value) = 0;

            /// @~chinese 速度使能   @~english Speed Enable
            __declspec(property(put = SetMatchMarkRoughFlag, get = GetMatchMarkRoughFlag)) bool MatchMarkRoughFlag;

            virtual bool GetMatchMarkRoughFlag() = 0;

            virtual void SetMatchMarkRoughFlag(bool value) = 0;

            /// @~chinese 速度阈值，范围：[0.1,1.0]   @~english Speed Threshold，range:[0.1,1.0]
            __declspec(property(put = SetMatchMarkRoughThresh, get = GetMatchMarkRoughThresh)) double MatchMarkRoughThresh;

            virtual double GetMatchMarkRoughThresh() = 0;

            virtual void SetMatchMarkRoughThresh(double value) = 0;

            /// @~chinese 位置纠正   @~english Correct Flag
            __declspec(property(put = SetMatchCorrectFlag, get = GetMatchCorrectFlag)) bool MatchCorrectFlag;

            virtual bool GetMatchCorrectFlag() = 0;

            virtual void SetMatchCorrectFlag(bool value) = 0;

            /// @~chinese 宽度方向容忍，范围：[5,200]   @~english Width Tolerance，range:[5,200]
            __declspec(property(put = SetMatchToleranceX, get = GetMatchToleranceX)) int MatchToleranceX;

            virtual int GetMatchToleranceX() = 0;

            virtual void SetMatchToleranceX(int value) = 0;

            /// @~chinese 高度方向容忍，范围：[5,200]   @~english Height Tolerance，range:[5,200]
            __declspec(property(put = SetMatchToleranceY, get = GetMatchToleranceY)) int MatchToleranceY;

            virtual int GetMatchToleranceY() = 0;

            virtual void SetMatchToleranceY(int value) = 0;

            /// @~chinese 最小匹配分数，范围：[0.1,1.0]   @~english Min Match Score，range:[0.1,1.0]
            __declspec(property(put = SetMatchMinScore, get = GetMatchMinScore)) double MatchMinScore;

            virtual double GetMatchMinScore() = 0;

            virtual void SetMatchMinScore(double value) = 0;

            /// @~chinese 匹配极性   @~english Match Polarity
            __declspec(property(put = SetMatchPolarity, get = GetMatchPolarity)) MatchPolarityEnum MatchPolarity;

            virtual MatchPolarityEnum GetMatchPolarity() = 0;

            virtual void SetMatchPolarity(MatchPolarityEnum value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetMatchAngleStart, get = GetMatchAngleStart)) int MatchAngleStart;

            virtual int GetMatchAngleStart() = 0;

            virtual void SetMatchAngleStart(int value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetMatchAngleEnd, get = GetMatchAngleEnd)) int MatchAngleEnd;

            virtual int GetMatchAngleEnd() = 0;

            virtual void SetMatchAngleEnd(int value) = 0;

            /// @~chinese 尺度范围，范围：[0.1,10]   @~english Scale Range，range:[0.1,10]
            __declspec(property(put = SetMatchScaleStart, get = GetMatchScaleStart)) double MatchScaleStart;

            virtual double GetMatchScaleStart() = 0;

            virtual void SetMatchScaleStart(double value) = 0;

            /// @~chinese 尺度范围，范围：[0.1,10]   @~english Scale Range，range:[0.1,10]
            __declspec(property(put = SetMatchScaleEnd, get = GetMatchScaleEnd)) double MatchScaleEnd;

            virtual double GetMatchScaleEnd() = 0;

            virtual void SetMatchScaleEnd(double value) = 0;

            /// @~chinese 阈值类型   @~english Threshold Type
            __declspec(property(put = SetMatchThresholdFlag, get = GetMatchThresholdFlag)) MatchThresholdFlagEnum MatchThresholdFlag;

            virtual MatchThresholdFlagEnum GetMatchThresholdFlag() = 0;

            virtual void SetMatchThresholdFlag(MatchThresholdFlagEnum value) = 0;

            /// @~chinese 边缘阈值，范围：[0,255]   @~english Edge Threshold，range:[0,255]
            __declspec(property(put = SetMatchThreshold, get = GetMatchThreshold)) int MatchThreshold;

            virtual int GetMatchThreshold() = 0;

            virtual void SetMatchThreshold(int value) = 0;

            /// @~chinese 速度使能   @~english Speed Enable
            __declspec(property(put = SetMatchRoughFlag, get = GetMatchRoughFlag)) bool MatchRoughFlag;

            virtual bool GetMatchRoughFlag() = 0;

            virtual void SetMatchRoughFlag(bool value) = 0;

            /// @~chinese 速度阈值，范围：[0.0,1.0]   @~english Speed Threshold，range:[0.0,1.0]
            __declspec(property(put = SetMatchRoughThresh, get = GetMatchRoughThresh)) double MatchRoughThresh;

            virtual double GetMatchRoughThresh() = 0;

            virtual void SetMatchRoughThresh(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /// 字符缺陷检测方法
        typedef _MVD_MKINSP_METHOD_TYPE__ MVD_MKINSP_METHOD_TYPE;
        /*  Note: Interface Classes of the result of the MarkInspResults.  */
        ///字符缺陷检测结果类
        class MarkInspResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MarkInspResults() {}
            virtual ~MarkInspResults() {}

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
             * @brief 匹配框
             */
             virtual RectBox GetMatchRect() = 0;
             /**
             * @brief 获取缺陷图像
             */
             virtual ImageBaseData GetMarkFlawImage() = 0;
             /**
              * @brief 匹配状态
              */
             virtual int GetMatchStatus() = 0;
             /**
              * @brief 字符状态
              * @param nIndex [IN] 字符状态索引
              */
             virtual int GetMarkStatus(int nIndex) = 0;
            /**
             * @brief 获取字符缺陷框个数
             * @return 返回缺陷框个数
             */
            virtual int GetNumOfFlawBox() = 0;
            /**
             * @brief 获取缺陷框位置
             * @param nIndex [IN] 缺陷框索引
             * @return 返回缺陷框位置
             * @remarks nIndex从0开始计数
             */
            virtual RectBox GetFlawBox(int nIndex) = 0;
            /**
             * @brief 获取字符定位框个数
             * @return 返回字符定位框个数
             */
            virtual int GetNumOfLocateBox() = 0;
            /**
             * @brief 获取字符定位框位置
             * @param nIndex [IN] 字符定位框索引
             * @return 返回字符定位框位置
             * @remarks nIndex从0开始计数
             */
            virtual RectBox GetLocateBox(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the MarkInsp tool.  */
        ///字符缺陷检测工具类
        class IMVSMarkInspModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSMarkInspModuTool() {}
            virtual ~IMVSMarkInspModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual MarkInspParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual MarkInspResults* GetResult() = 0;

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
            * @remarks 导入模型
            */
            virtual void ImportModelData(IN const char * const strPath) = 0;

        private:
            IMVSMarkInspModuTool(IMVSMarkInspModuTool&);
            IMVSMarkInspModuTool& operator=(IMVSMarkInspModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for MarkInsp.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSMarkInspModu::IMVSMarkInspModuTool * __stdcall GetMarkInspToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_MARKINSP_H_
