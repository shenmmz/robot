/***************************************************************************************************
 * File：IVmCnnCharDetect.h
 * Note：@~chinese DL字符定位模块接口声明 @~english Interface for the CnnCharDetect tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CNNCHARDETECT_H_
#define _IVM_CNNCHARDETECT_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCnnCharDetectModu
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

    /// \addtogroup DL字符定位
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///DL字符定位ROI管理类
        class CnnCharDetectRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the CnnCharDetectParams.  */
        ///DL字符定位参数类
        class CnnCharDetectParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnCharDetectParams() {}
            virtual ~CnnCharDetectParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual CnnCharDetectRoiManager* GetModuRoiManager() = 0;

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

            /// @~chinese 按ROI裁图   @~english Cut Via Roi
            __declspec(property(put = SetCutViaROIEnable, get = GetCutViaROIEnable)) bool CutViaROIEnable;

            virtual bool GetCutViaROIEnable() = 0;

            virtual void SetCutViaROIEnable(bool value) = 0;

            /// @~chinese 最大查找个数，范围：[1,100]   @~english Max Number to Find，range:[1,100]
            __declspec(property(put = SetMaxObjNum, get = GetMaxObjNum)) int MaxObjNum;

            virtual int GetMaxObjNum() = 0;

            virtual void SetMaxObjNum(int value) = 0;

            /// @~chinese 最小置信度，范围：[0.01,1.0](弃用)   @~english Min Confidence，range:[0.01,1.0](Obsoleted)
            __declspec(property(put = SetMinScore, get = GetMinScore)) double MinScore;

            virtual double GetMinScore() = 0;

            virtual void SetMinScore(double value) = 0;

            /// @~chinese 最小分数，范围：[0.01,1.0]   @~english Min Box Score，range:[0.01,1.0]
            __declspec(property(put = SetBoxScore, get = GetBoxScore)) double BoxScore;

            virtual double GetBoxScore() = 0;

            virtual void SetBoxScore(double value) = 0;

            /// @~chinese 最大重叠率，范围：[0.01,1.0]   @~english Overlap Threshold，range:[0.01,1.0]
            __declspec(property(put = SetMaxOverlap, get = GetMaxOverlap)) double MaxOverlap;

            virtual double GetMaxOverlap() = 0;

            virtual void SetMaxOverlap(double value) = 0;

            /// @~chinese 链接阈值，范围：[0.01,1.0]   @~english Link Threshold，range:[0.01,1.0]
            __declspec(property(put = SetLinkThreshold, get = GetLinkThreshold)) double LinkThreshold;

            virtual double GetLinkThreshold() = 0;

            virtual void SetLinkThreshold(double value) = 0;

            /// @~chinese 目标排序   @~english Sort Object
            __declspec(property(put = SetSortObjectMode, get = GetSortObjectMode)) SortObjectModeEnum SortObjectMode;

            virtual SortObjectModeEnum GetSortObjectMode() = 0;

            virtual void SetSortObjectMode(SortObjectModeEnum value) = 0;

            /// @~chinese 缩放系数，范围：[0.01,2.0]   @~english ShrinkBack，range:[0.01,2.0]
            __declspec(property(put = SetShrinkBack, get = GetShrinkBack)) double ShrinkBack;

            virtual double GetShrinkBack() = 0;

            virtual void SetShrinkBack(double value) = 0;

            /// @~chinese 边缘筛选使能   @~english Edge Filter Enable
            __declspec(property(put = SetOutRoiFilterEnable, get = GetOutRoiFilterEnable)) bool OutRoiFilterEnable;

            virtual bool GetOutRoiFilterEnable() = 0;

            virtual void SetOutRoiFilterEnable(bool value) = 0;

            /// @~chinese 最小边缘分数，范围：[0.01,1.0]   @~english Min Edge Score，range:[0.01,1.0]
            __declspec(property(put = SetMinEdgeScore, get = GetMinEdgeScore)) double MinEdgeScore;

            virtual double GetMinEdgeScore() = 0;

            virtual void SetMinEdgeScore(double value) = 0;

            /// 文本对比   文本对比
            __declspec(property(put = SetTextComparisonEnable, get = GetTextComparisonEnable)) bool TextComparisonEnable;

            virtual bool GetTextComparisonEnable() = 0;

            virtual void SetTextComparisonEnable(bool value) = 0;

            /// @~chinese 字符角度使能   @~english Character Angle Enable
            __declspec(property(put = SetAngleEnable, get = GetAngleEnable)) bool AngleEnable;

            virtual bool GetAngleEnable() = 0;

            virtual void SetAngleEnable(bool value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetStartAngle, get = GetStartAngle)) int StartAngle;

            virtual int GetStartAngle() = 0;

            virtual void SetStartAngle(int value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetEndAngle, get = GetEndAngle)) int EndAngle;

            virtual int GetEndAngle() = 0;

            virtual void SetEndAngle(int value) = 0;

            /// @~chinese 字符宽度使能   @~english Character Width Enable
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

            /// @~chinese 字符高度使能   @~english Character Height Enable
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

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
		/// DL字符定位预测信息类
        class ICNNCharDetectPredInfo
        {
        protected:
            ///< Constructor and Destructor
            //构造函数和析构函数
            explicit ICNNCharDetectPredInfo() {};
            virtual ~ICNNCharDetectPredInfo() {};

        public:
            /**
             * @brief 预测信息的矩形框
             */
            virtual RectBox GetPredictBox() = 0;
            /**
             * @brief 预测信息的置信度
             */
            virtual float GetConfidence() = 0;
            /**
             * @brief 目标状态
             */
            virtual float GetObjStatus() = 0;
        };

        /*  Note: Interface Classes of the result of the CnnCharDetectResults.  */
        ///DL字符定位结果类
        class CnnCharDetectResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnCharDetectResults() {}
            virtual ~CnnCharDetectResults() {}

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
             * @brief 预测信息的数量
             */
            virtual int GetPredictNumber() = 0;
            /**
             * @brief 根据给定的索引获取预测信息
             */
            virtual ICNNCharDetectPredInfo* GetPredictInfo(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CnnCharDetect tool.  */
        ///DL字符定位工具类
        class IMVSCnnCharDetectModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCnnCharDetectModuTool() {}
            virtual ~IMVSCnnCharDetectModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CnnCharDetectParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CnnCharDetectResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSCnnCharDetectModuTool(IMVSCnnCharDetectModuTool&);
            IMVSCnnCharDetectModuTool& operator=(IMVSCnnCharDetectModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CnnCharDetect.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSCnnCharDetectModu::IMVSCnnCharDetectModuTool * __stdcall GetCnnCharDetectToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CNNCHARDETECT_H_
