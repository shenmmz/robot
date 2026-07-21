/***************************************************************************************************
 * File：IVmCnnDetect.h
 * Note：@~chinese DL目标检测模块接口声明 @~english Interface for the CnnDetect tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CNNDETECT_H_
#define _IVM_CNNDETECT_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCnnDetectModu
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

    /// \addtogroup DL目标检测
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///DL目标检测ROI管理类
        class CnnDetectRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the CnnDetectParams.  */
        ///DL目标检测参数类
        class CnnDetectParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnDetectParams() {}
            virtual ~CnnDetectParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual CnnDetectRoiManager* GetModuRoiManager() = 0;

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

            /// @~chinese 最大查找个数，范围：[1,1000]   @~english Max Number to Find，range:[1,1000]
            __declspec(property(put = SetMaxObjNum, get = GetMaxObjNum)) int MaxObjNum;

            virtual int GetMaxObjNum() = 0;

            virtual void SetMaxObjNum(int value) = 0;

            /// @~chinese 最小置信度，范围：[0.01,1.0]   @~english Min Confidence，range:[0.01,1.0]
            __declspec(property(put = SetMinScore, get = GetMinScore)) double MinScore;

            virtual double GetMinScore() = 0;

            virtual void SetMinScore(double value) = 0;

            /// @~chinese 最大重叠率，范围：[0,1.0]   @~english Overlap Threshold，range:[0,1.0]
            __declspec(property(put = SetMaxOverlap, get = GetMaxOverlap)) double MaxOverlap;

            virtual double GetMaxOverlap() = 0;

            virtual void SetMaxOverlap(double value) = 0;

            /// @~chinese 目标排序   @~english Sort Object
            __declspec(property(put = SetSortObjectMode, get = GetSortObjectMode)) SortObjectModeEnum SortObjectMode;

            virtual SortObjectModeEnum GetSortObjectMode() = 0;

            virtual void SetSortObjectMode(SortObjectModeEnum value) = 0;

            /// @~chinese 批处理使能(仅GPU模块生效)   @~english Batch Processing Enable(Takes effect only in GPU modules)
            __declspec(property(put = SetBatchProcessEnable, get = GetBatchProcessEnable)) bool BatchProcessEnable;

            virtual bool GetBatchProcessEnable() = 0;

            virtual void SetBatchProcessEnable(bool value) = 0;

            /// @~chinese 批处理等级，范围：[1,32](仅GPU模块生效)   @~english Batch Processing Level，range:[1,32](Takes effect only in GPU modules)
            __declspec(property(put = SetBatchProcessingLevel, get = GetBatchProcessingLevel)) int BatchProcessingLevel;

            virtual int GetBatchProcessingLevel() = 0;

            virtual void SetBatchProcessingLevel(int value) = 0;

            /// @~chinese 小目标模式   @~english Small Object Mode
            __declspec(property(put = SetSODEnable, get = GetSODEnable)) bool SODEnable;

            virtual bool GetSODEnable() = 0;

            virtual void SetSODEnable(bool value) = 0;

            /// @~chinese 横向滑窗数，范围：[1,16]   @~english Moving Window Number (Horizontally)，range:[1,16]
            __declspec(property(put = SetXSlidingWinNumOfSOD, get = GetXSlidingWinNumOfSOD)) int XSlidingWinNumOfSOD;

            virtual int GetXSlidingWinNumOfSOD() = 0;

            virtual void SetXSlidingWinNumOfSOD(int value) = 0;

            /// @~chinese 纵向滑窗数，范围：[1,16]   @~english Moving Window Number (Vertically)，range:[1,16]
            __declspec(property(put = SetYSlidingWinNumOfSOD, get = GetYSlidingWinNumOfSOD)) int YSlidingWinNumOfSOD;

            virtual int GetYSlidingWinNumOfSOD() = 0;

            virtual void SetYSlidingWinNumOfSOD(int value) = 0;

            /// @~chinese 滑窗重叠率，范围：[0,0.6]   @~english Moving Window Overlap Rate，range:[0,0.6]
            __declspec(property(put = SetSlidingWinOverlap, get = GetSlidingWinOverlap)) double SlidingWinOverlap;

            virtual double GetSlidingWinOverlap() = 0;

            virtual void SetSlidingWinOverlap(double value) = 0;

            /// @~chinese 边缘筛选使能   @~english Edge Filter Enable
            __declspec(property(put = SetOutRoiFilterEnable, get = GetOutRoiFilterEnable)) bool OutRoiFilterEnable;

            virtual bool GetOutRoiFilterEnable() = 0;

            virtual void SetOutRoiFilterEnable(bool value) = 0;

            /// @~chinese 最小边缘分数，范围：[0.01,1.0]   @~english Min Edge Score，range:[0.01,1.0]
            __declspec(property(put = SetMinEdgeScore, get = GetMinEdgeScore)) double MinEdgeScore;

            virtual double GetMinEdgeScore() = 0;

            virtual void SetMinEdgeScore(double value) = 0;

            /// @~chinese 角度使能   @~english Angle Enable
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

            /// @~chinese 宽度使能   @~english Width Enable
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

            /// @~chinese 高度使能   @~english Height Enable
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

            /// @~chinese 不同类别过滤   @~english Filter of Different Categories
            __declspec(property(put = SetDiffClassNMSEnable, get = GetDiffClassNMSEnable)) bool DiffClassNMSEnable;

            virtual bool GetDiffClassNMSEnable() = 0;

            virtual void SetDiffClassNMSEnable(bool value) = 0;

            /// @~chinese 面积使能   @~english Area Enable
            __declspec(property(put = SetAreaEnable, get = GetAreaEnable)) bool AreaEnable;

            virtual bool GetAreaEnable() = 0;

            virtual void SetAreaEnable(bool value) = 0;

            /// @~chinese 面积范围，范围：[1,16000000]   @~english Area Range，range:[1,16000000]
            __declspec(property(put = SetMinArea, get = GetMinArea)) int MinArea;

            virtual int GetMinArea() = 0;

            virtual void SetMinArea(int value) = 0;

            /// @~chinese 面积范围，范围：[1,16000000]   @~english Area Range，range:[1,16000000]
            __declspec(property(put = SetMaxArea, get = GetMaxArea)) int MaxArea;

            virtual int GetMaxArea() = 0;

            virtual void SetMaxArea(int value) = 0;

            /// @~chinese 长短边比使能   @~english Aspect Ratio Enable
            __declspec(property(put = SetWHRatioEnable, get = GetWHRatioEnable)) bool WHRatioEnable;

            virtual bool GetWHRatioEnable() = 0;

            virtual void SetWHRatioEnable(bool value) = 0;

            /// @~chinese 长短边比范围，范围：[1,100]   @~english Aspect Ratio Range，range:[1,100]
            __declspec(property(put = SetMinWHRatio, get = GetMinWHRatio)) double MinWHRatio;

            virtual double GetMinWHRatio() = 0;

            virtual void SetMinWHRatio(double value) = 0;

            /// @~chinese 长短边比范围，范围：[1,100]   @~english Aspect Ratio Range，range:[1,100]
            __declspec(property(put = SetMaxWHRatio, get = GetMaxWHRatio)) double MaxWHRatio;

            virtual double GetMaxWHRatio() = 0;

            virtual void SetMaxWHRatio(double value) = 0;

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

            /// @~chinese 类别号判断   @~english Category No. Check
            __declspec(property(put = SetClassLimitEnable, get = GetClassLimitEnable)) bool ClassLimitEnable;

            virtual bool GetClassLimitEnable() = 0;

            virtual void SetClassLimitEnable(bool value) = 0;

            /// @~chinese 类别号范围，范围：[0,99999]   @~english No. Range，range:[0,99999]
            __declspec(property(put = SetClassLimitLow, get = GetClassLimitLow)) int ClassLimitLow;

            virtual int GetClassLimitLow() = 0;

            virtual void SetClassLimitLow(int value) = 0;

            /// @~chinese 类别号范围，范围：[0,99999]   @~english No. Range，range:[0,99999]
            __declspec(property(put = SetClassLimitHigh, get = GetClassLimitHigh)) int ClassLimitHigh;

            virtual int GetClassLimitHigh() = 0;

            virtual void SetClassLimitHigh(int value) = 0;

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
    /// 目标预测信息
        class ICNNDetectPredInfo
        {
        protected:
            explicit ICNNDetectPredInfo(){};
            virtual ~ICNNDetectPredInfo(){};

        public:
            /**
             * @brief 预测信息的标签值
             */
            virtual int GetLabel() = 0;
            /**
             * @brief 预测信息的置信度
             */
            virtual float GetConfidence() = 0;
            /**
            * @brief 预测信息的状态
            */
            virtual int GetObjStatus() = 0;
            /**
            * @brief 预测信息的半径
            */
            virtual float GetObjRadius() = 0;
            /**
             * @brief 预测信息的矩形框
             */
            virtual RectBox GetPredictBox() = 0;
            /**
             * @brief 预测信息的标签名称
             */
            virtual void GetLabelName(char pcLabelName[128]) = 0;
        };

        /*  Note: Interface Classes of the result of the CnnDetectResults.  */
        ///DL目标检测结果类
        class CnnDetectResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnDetectResults() {}
            virtual ~CnnDetectResults() {}

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
            virtual ICNNDetectPredInfo * GetPredictInfo(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CnnDetect tool.  */
        ///DL目标检测工具类
        class IMVSCnnDetectModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCnnDetectModuTool() {}
            virtual ~IMVSCnnDetectModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CnnDetectParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CnnDetectResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSCnnDetectModuTool(IMVSCnnDetectModuTool&);
            IMVSCnnDetectModuTool& operator=(IMVSCnnDetectModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CnnDetect.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSCnnDetectModu::IMVSCnnDetectModuTool * __stdcall GetCnnDetectToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CNNDETECT_H_
