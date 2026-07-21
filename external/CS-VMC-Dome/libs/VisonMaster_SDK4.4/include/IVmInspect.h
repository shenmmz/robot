/***************************************************************************************************
 * File：IVmInspect.h
 * Note：@~chinese 异常检测模块接口声明 @~english Interface for the Inspect tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_INSPECT_H_
#define _IVM_INSPECT_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSInspectModu
    {
        /// @~chinese 匹配极性   @~english Match Polarity
        typedef enum _MatchPolarityEnum
        {
            /// @~chinese 不考虑极性   @~english Ignored
            MatchPolarity_No = 0x0,

            /// @~chinese 考虑极性   @~english Considered
            MatchPolarity_Yes = 0x1,

        }MatchPolarityEnum;

        /// @~chinese 尺度范围类型   @~english RangeScaleType
        typedef enum _RangeScaleTypeEnum
        {
            /// @~chinese 尺度范围   @~english Scale Range
            RangeScaleType_ScaleRange = 0x0,

            /// @~chinese 尺度范围XY   @~english Scale RangeXY
            RangeScaleType_ScaleRangeXY = 0x1,

        }RangeScaleTypeEnum;

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

    /// \addtogroup 异常检测
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the InspectParams.  */
        ///异常检测参数类
        class InspectParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit InspectParams() {}
            virtual ~InspectParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 分数阈值，范围：[0.01,0.99]   @~english Score Threshold，range:[0.01,0.99]
            __declspec(property(put = SetInspectScore, get = GetInspectScore)) double InspectScore;

            virtual double GetInspectScore() = 0;

            virtual void SetInspectScore(double value) = 0;

            /// @~chinese 下采样率，范围：[10,100]   @~english Down Sample Rate，range:[10,100]
            __declspec(property(put = SetDownSampleRate, get = GetDownSampleRate)) int DownSampleRate;

            virtual int GetDownSampleRate() = 0;

            virtual void SetDownSampleRate(int value) = 0;

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

            /// @~chinese 尺度范围类型   @~english RangeScaleType
            __declspec(property(put = SetRangeScaleType, get = GetRangeScaleType)) RangeScaleTypeEnum RangeScaleType;

            virtual RangeScaleTypeEnum GetRangeScaleType() = 0;

            virtual void SetRangeScaleType(RangeScaleTypeEnum value) = 0;

            /// @~chinese 尺度范围，范围：[0.1,10]   @~english Scale Range，range:[0.1,10]
            __declspec(property(put = SetScaleStart, get = GetScaleStart)) double ScaleStart;

            virtual double GetScaleStart() = 0;

            virtual void SetScaleStart(double value) = 0;

            /// @~chinese 尺度范围，范围：[0.1,10]   @~english Scale Range，range:[0.1,10]
            __declspec(property(put = SetScaleEnd, get = GetScaleEnd)) double ScaleEnd;

            virtual double GetScaleEnd() = 0;

            virtual void SetScaleEnd(double value) = 0;

            /// @~chinese X尺度范围，范围：[0.1,10]   @~english X Scale Range，range:[0.1,10]
            __declspec(property(put = SetScaleXStart, get = GetScaleXStart)) double ScaleXStart;

            virtual double GetScaleXStart() = 0;

            virtual void SetScaleXStart(double value) = 0;

            /// @~chinese X尺度范围，范围：[0.1,10]   @~english X Scale Range，range:[0.1,10]
            __declspec(property(put = SetScaleXEnd, get = GetScaleXEnd)) double ScaleXEnd;

            virtual double GetScaleXEnd() = 0;

            virtual void SetScaleXEnd(double value) = 0;

            /// @~chinese Y尺度范围，范围：[0.1,10]   @~english Y Scale Range，range:[0.1,10]
            __declspec(property(put = SetScaleYStart, get = GetScaleYStart)) double ScaleYStart;

            virtual double GetScaleYStart() = 0;

            virtual void SetScaleYStart(double value) = 0;

            /// @~chinese Y尺度范围，范围：[0.1,10]   @~english Y Scale Range，range:[0.1,10]
            __declspec(property(put = SetScaleYEnd, get = GetScaleYEnd)) double ScaleYEnd;

            virtual double GetScaleYEnd() = 0;

            virtual void SetScaleYEnd(double value) = 0;

            /// @~chinese 阈值类型   @~english Threshold Type
            __declspec(property(put = SetMatchThresholdFlag, get = GetMatchThresholdFlag)) MatchThresholdFlagEnum MatchThresholdFlag;

            virtual MatchThresholdFlagEnum GetMatchThresholdFlag() = 0;

            virtual void SetMatchThresholdFlag(MatchThresholdFlagEnum value) = 0;

            /// @~chinese 边缘阈值，范围：[1,255]   @~english Edge Threshold，range:[1,255]
            __declspec(property(put = SetMatchThreshold, get = GetMatchThreshold)) int MatchThreshold;

            virtual int GetMatchThreshold() = 0;

            virtual void SetMatchThreshold(int value) = 0;

            /// @~chinese 延拓阈值，范围：[0,90]   @~english Extension Threshold，range:[0,90]
            __declspec(property(put = SetMatchExtentRate, get = GetMatchExtentRate)) int MatchExtentRate;

            virtual int GetMatchExtentRate() = 0;

            virtual void SetMatchExtentRate(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the InspectResults.  */
        ///异常检测结果类
        class InspectResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit InspectResults() {}
            virtual ~InspectResults() {}

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
             * @brief 标签
             */
            virtual const char * GetInspectLabel() = 0;

            /**
             * @brief 分数
             */
            virtual float GetInspectScore() = 0;

            /**
             * @brief 对比状态
             */
            virtual int GetInspectStatus() = 0;

            /**
             * @brief Roi数量
             */
            virtual int GetInspRoiNum() = 0;

            /**
             * @brief 匹配框
             */
            virtual RectBox GetMatchRect() = 0;

            /**
             * @brief 匹配状态
             */
            virtual int GetMatchStatus() = 0;

             /**
             * @brief 缺陷框
             */
             virtual RectBox GetDefectBox() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the Inspect tool.  */
        ///异常检测工具类
        class IMVSInspectModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSInspectModuTool() {}
            virtual ~IMVSInspectModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual InspectParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual InspectResults* GetResult() = 0;

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
            IMVSInspectModuTool(IMVSInspectModuTool&);
            IMVSInspectModuTool& operator=(IMVSInspectModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for Inspect.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSInspectModu::IMVSInspectModuTool * __stdcall GetInspectToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_INSPECT_H_
