/***************************************************************************************************
 * File：IVmMarkInspVA.h
 * Note：@~chinese 字符缺陷检测模块接口声明 @~english Interface for the MarkInspVA tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_MARKINSPVA_H_
#define _IVM_MARKINSPVA_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSMarkInspModuVA
    {
        /// @~chinese 缺陷类型   @~english DefectType
        typedef enum _FlawTypeEnum
        {
            /// @~chinese 亮缺陷   @~english Bright Defect
            FlawType_BrightDefect = 0x1,

            /// @~chinese 暗缺陷   @~english Dark Defect
            FlawType_DarkDefect = 0x2,

            /// @~chinese 亮暗缺陷   @~english Bright and Dark Defect
            FlawType_BothDefect = 0x3,

        }FlawTypeEnum;

        /// @~chinese 阈值类型   @~english Threshold Type
        typedef enum _ExactMatchMatchThresholdFlagEnum
        {
            /// @~chinese 自动阈值   @~english Auto Threshold
            ExactMatchMatchThresholdFlag_AutoThreshold = 0x0,

            /// @~chinese 模板阈值   @~english Model Threshold
            ExactMatchMatchThresholdFlag_ModelThreshold = 0x1,

            /// @~chinese 手动阈值   @~english Manual Threshold
            ExactMatchMatchThresholdFlag_ManualThreshold = 0x2,

        }ExactMatchMatchThresholdFlagEnum;

        /// @~chinese 匹配极性   @~english Match Polarity
        typedef enum _RoughMatchPolarityEnum
        {
            /// @~chinese 不考虑极性   @~english Ignored
            RoughMatchPolarity_No = 0x0,

            /// @~chinese 考虑极性   @~english Considered
            RoughMatchPolarity_Yes = 0x1,

        }RoughMatchPolarityEnum;

        /// @~chinese 阈值类型   @~english Threshold Type
        typedef enum _RoughMatchMatchThresholdFlagEnum
        {
            /// @~chinese 自动阈值   @~english Auto Threshold
            RoughMatchMatchThresholdFlag_AutoThreshold = 0x0,

            /// @~chinese 模板阈值   @~english Model Threshold
            RoughMatchMatchThresholdFlag_ModelThreshold = 0x1,

            /// @~chinese 手动阈值   @~english Manual Threshold
            RoughMatchMatchThresholdFlag_ManualThreshold = 0x2,

        }RoughMatchMatchThresholdFlagEnum;

    /// \addtogroup 字符缺陷检测
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///字符缺陷检测ROI管理类
        class MarkInspVARoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

            /// @~chinese 掩膜图像（仅当次执行起效）   @~english Mask Image(Only valid for this execution)
            virtual void SetMaskImage(ImageBaseData stMaskImage) = 0;

        };

        /*  Note: Interface Classes of the parameter of the MarkInspVAParams.  */
        ///字符缺陷检测参数类
        class MarkInspVAParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MarkInspVAParams() {}
            virtual ~MarkInspVAParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 运行点(仅当次执行起效)   @~english Unfixtured Point(Only valid for this execution)
            virtual void SetInheritRunPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 运行角度(仅当次执行起效)   @~english Unfixtured Angle(Only valid for this execution)
            virtual void SetInheritRunAngle(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 运行尺度X(仅当次执行起效)   @~english FixtureRunScaleX(Only valid for this execution)
            virtual void SetInheritRunScaleX(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 运行尺度Y(仅当次执行起效)   @~english FixtureRunScaleY(Only valid for this execution)
            virtual void SetInheritRunScaleY(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual MarkInspVARoiManager* GetModuRoiManager() = 0;

            /// @~chinese 缺陷类型   @~english DefectType
            __declspec(property(put = SetFlawType, get = GetFlawType)) FlawTypeEnum FlawType;

            virtual FlawTypeEnum GetFlawType() = 0;

            virtual void SetFlawType(FlawTypeEnum value) = 0;

            /// @~chinese 亮缺陷阈值，范围：[1,250]   @~english Bright Threshold，range:[1,250]
            __declspec(property(put = SetBrightThreshold, get = GetBrightThreshold)) int BrightThreshold;

            virtual int GetBrightThreshold() = 0;

            virtual void SetBrightThreshold(int value) = 0;

            /// @~chinese 暗缺陷阈值，范围：[1,250]   @~english Dark Threshold，range:[1,250]
            __declspec(property(put = SetDarkThreshold, get = GetDarkThreshold)) int DarkThreshold;

            virtual int GetDarkThreshold() = 0;

            virtual void SetDarkThreshold(int value) = 0;

            /// @~chinese 亮缺陷尺度，范围：[0.01,1]   @~english Bright Scale，range:[0.01,1]
            __declspec(property(put = SetBrightVarScale, get = GetBrightVarScale)) double BrightVarScale;

            virtual double GetBrightVarScale() = 0;

            virtual void SetBrightVarScale(double value) = 0;

            /// @~chinese 暗缺陷尺度，范围：[0.01,1]   @~english Dark Scale，range:[0.01,1]
            __declspec(property(put = SetDarkVarScale, get = GetDarkVarScale)) double DarkVarScale;

            virtual double GetDarkVarScale() = 0;

            virtual void SetDarkVarScale(double value) = 0;

            /// @~chinese 边缘容忍度，范围：[0,10]   @~english Edge Tolerance，range:[0,10]
            __declspec(property(put = SetEdgeTolerance, get = GetEdgeTolerance)) int EdgeTolerance;

            virtual int GetEdgeTolerance() = 0;

            virtual void SetEdgeTolerance(int value) = 0;

            /// @~chinese 面积大小阈值，范围：[1,9000000]   @~english Area Threshold，range:[1,9000000]
            __declspec(property(put = SetBlobMinArea, get = GetBlobMinArea)) int BlobMinArea;

            virtual int GetBlobMinArea() = 0;

            virtual void SetBlobMinArea(int value) = 0;

            /// @~chinese 最小匹配分数，范围：[0.1,1.0]   @~english Min Match Score，range:[0.1,1.0]
            __declspec(property(put = SetExactMatchMinScore, get = GetExactMatchMinScore)) double ExactMatchMinScore;

            virtual double GetExactMatchMinScore() = 0;

            virtual void SetExactMatchMinScore(double value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetExactMatchAngleStart, get = GetExactMatchAngleStart)) int ExactMatchAngleStart;

            virtual int GetExactMatchAngleStart() = 0;

            virtual void SetExactMatchAngleStart(int value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetExactMatchAngleEnd, get = GetExactMatchAngleEnd)) int ExactMatchAngleEnd;

            virtual int GetExactMatchAngleEnd() = 0;

            virtual void SetExactMatchAngleEnd(int value) = 0;

            /// @~chinese X尺度范围，范围：[0.5,1.5]   @~english X Scale Range，range:[0.5,1.5]
            __declspec(property(put = SetExactMatchScaleXStart, get = GetExactMatchScaleXStart)) double ExactMatchScaleXStart;

            virtual double GetExactMatchScaleXStart() = 0;

            virtual void SetExactMatchScaleXStart(double value) = 0;

            /// @~chinese X尺度范围，范围：[0.5,1.5]   @~english X Scale Range，range:[0.5,1.5]
            __declspec(property(put = SetExactMatchScaleXEnd, get = GetExactMatchScaleXEnd)) double ExactMatchScaleXEnd;

            virtual double GetExactMatchScaleXEnd() = 0;

            virtual void SetExactMatchScaleXEnd(double value) = 0;

            /// @~chinese Y尺度范围，范围：[0.5,1.5]   @~english Y Scale Range，range:[0.5,1.5]
            __declspec(property(put = SetExactMatchScaleYStart, get = GetExactMatchScaleYStart)) double ExactMatchScaleYStart;

            virtual double GetExactMatchScaleYStart() = 0;

            virtual void SetExactMatchScaleYStart(double value) = 0;

            /// @~chinese Y尺度范围，范围：[0.5,1.5]   @~english Y Scale Range，range:[0.5,1.5]
            __declspec(property(put = SetExactMatchScaleYEnd, get = GetExactMatchScaleYEnd)) double ExactMatchScaleYEnd;

            virtual double GetExactMatchScaleYEnd() = 0;

            virtual void SetExactMatchScaleYEnd(double value) = 0;

            /// @~chinese 阈值类型   @~english Threshold Type
            __declspec(property(put = SetExactMatchMatchThresholdFlag, get = GetExactMatchMatchThresholdFlag)) ExactMatchMatchThresholdFlagEnum ExactMatchMatchThresholdFlag;

            virtual ExactMatchMatchThresholdFlagEnum GetExactMatchMatchThresholdFlag() = 0;

            virtual void SetExactMatchMatchThresholdFlag(ExactMatchMatchThresholdFlagEnum value) = 0;

            /// @~chinese 边缘阈值，范围：[1,255]   @~english Edge Threshold，range:[1,255]
            __declspec(property(put = SetExactMatchMatchThresholdHigh, get = GetExactMatchMatchThresholdHigh)) int ExactMatchMatchThresholdHigh;

            virtual int GetExactMatchMatchThresholdHigh() = 0;

            virtual void SetExactMatchMatchThresholdHigh(int value) = 0;

            /// @~chinese 位置纠正   @~english Correct Flag
            __declspec(property(put = SetExactMatchCorrectFlag, get = GetExactMatchCorrectFlag)) bool ExactMatchCorrectFlag;

            virtual bool GetExactMatchCorrectFlag() = 0;

            virtual void SetExactMatchCorrectFlag(bool value) = 0;

            /// @~chinese 宽度方向容忍，范围：[0.01,10]   @~english Width Tolerance，range:[0.01,10]
            __declspec(property(put = SetExactMatchToleranceX, get = GetExactMatchToleranceX)) double ExactMatchToleranceX;

            virtual double GetExactMatchToleranceX() = 0;

            virtual void SetExactMatchToleranceX(double value) = 0;

            /// @~chinese 高度方向容忍，范围：[0.01,10]   @~english Height Tolerance，range:[0.01,10]
            __declspec(property(put = SetExactMatchToleranceY, get = GetExactMatchToleranceY)) double ExactMatchToleranceY;

            virtual double GetExactMatchToleranceY() = 0;

            virtual void SetExactMatchToleranceY(double value) = 0;

            /// @~chinese 最小匹配分数，范围：[0.1,1.0]   @~english Min Match Score，range:[0.1,1.0]
            __declspec(property(put = SetRoughMatchMinScore, get = GetRoughMatchMinScore)) double RoughMatchMinScore;

            virtual double GetRoughMatchMinScore() = 0;

            virtual void SetRoughMatchMinScore(double value) = 0;

            /// @~chinese 匹配极性   @~english Match Polarity
            __declspec(property(put = SetRoughMatchPolarity, get = GetRoughMatchPolarity)) RoughMatchPolarityEnum RoughMatchPolarity;

            virtual RoughMatchPolarityEnum GetRoughMatchPolarity() = 0;

            virtual void SetRoughMatchPolarity(RoughMatchPolarityEnum value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetRoughMatchAngleStart, get = GetRoughMatchAngleStart)) int RoughMatchAngleStart;

            virtual int GetRoughMatchAngleStart() = 0;

            virtual void SetRoughMatchAngleStart(int value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetRoughMatchAngleEnd, get = GetRoughMatchAngleEnd)) int RoughMatchAngleEnd;

            virtual int GetRoughMatchAngleEnd() = 0;

            virtual void SetRoughMatchAngleEnd(int value) = 0;

            /// @~chinese X尺度范围，范围：[0.5,1.5]   @~english X Scale Range，range:[0.5,1.5]
            __declspec(property(put = SetRoughMatchScaleXStart, get = GetRoughMatchScaleXStart)) double RoughMatchScaleXStart;

            virtual double GetRoughMatchScaleXStart() = 0;

            virtual void SetRoughMatchScaleXStart(double value) = 0;

            /// @~chinese X尺度范围，范围：[0.5,1.5]   @~english X Scale Range，range:[0.5,1.5]
            __declspec(property(put = SetRoughMatchScaleXEnd, get = GetRoughMatchScaleXEnd)) double RoughMatchScaleXEnd;

            virtual double GetRoughMatchScaleXEnd() = 0;

            virtual void SetRoughMatchScaleXEnd(double value) = 0;

            /// @~chinese Y尺度范围，范围：[0.5,1.5]   @~english Y Scale Range，range:[0.5,1.5]
            __declspec(property(put = SetRoughMatchScaleYStart, get = GetRoughMatchScaleYStart)) double RoughMatchScaleYStart;

            virtual double GetRoughMatchScaleYStart() = 0;

            virtual void SetRoughMatchScaleYStart(double value) = 0;

            /// @~chinese Y尺度范围，范围：[0.5,1.5]   @~english Y Scale Range，range:[0.5,1.5]
            __declspec(property(put = SetRoughMatchScaleYEnd, get = GetRoughMatchScaleYEnd)) double RoughMatchScaleYEnd;

            virtual double GetRoughMatchScaleYEnd() = 0;

            virtual void SetRoughMatchScaleYEnd(double value) = 0;

            /// @~chinese 阈值类型   @~english Threshold Type
            __declspec(property(put = SetRoughMatchMatchThresholdFlag, get = GetRoughMatchMatchThresholdFlag)) RoughMatchMatchThresholdFlagEnum RoughMatchMatchThresholdFlag;

            virtual RoughMatchMatchThresholdFlagEnum GetRoughMatchMatchThresholdFlag() = 0;

            virtual void SetRoughMatchMatchThresholdFlag(RoughMatchMatchThresholdFlagEnum value) = 0;

            /// @~chinese 边缘阈值，范围：[1,255]   @~english Edge Threshold，range:[1,255]
            __declspec(property(put = SetRoughMatchMatchThresholdHigh, get = GetRoughMatchMatchThresholdHigh)) int RoughMatchMatchThresholdHigh;

            virtual int GetRoughMatchMatchThresholdHigh() = 0;

            virtual void SetRoughMatchMatchThresholdHigh(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /// 字符缺陷检测方法
        typedef _MVD_MKINSP_METHOD_TYPE__ MVD_MKINSP_METHOD_TYPE;
        /*  Note: Interface Classes of the result of the MarkInspVAResults.  */
        ///字符缺陷检测结果类
        class MarkInspVAResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MarkInspVAResults() {}
            virtual ~MarkInspVAResults() {}

        public:
             /**
             * @brief 屏蔽区字符串
             */
             virtual StringData* GetBlindPolygonString(unsigned int& nValueNum) = 0;

             /**
             * @brief 输出掩膜
             */
             virtual ImageBaseData GetOutputMask() = 0;

             /**
             * @brief 屏蔽区多边形点集
             */
             virtual PointF GetBlindPolygonPoints(int nIndex) = 0;

             /**
             * @brief 屏蔽区多边形点数
             */
             virtual int GetBlindPolygonPointNum(int nIndex) = 0;

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
             * @param nIndex [in] 字符定位框索引
             * @return 返回字符定位框位置
             * @remarks nIndex从0开始计数
             */
            virtual RectBox GetLocateBox(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the MarkInspVA tool.  */
        ///字符缺陷检测工具类
        class IMVSMarkInspModuVATool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSMarkInspModuVATool() {}
            virtual ~IMVSMarkInspModuVATool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual MarkInspVAParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual MarkInspVAResults* GetResult() = 0;

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
            IMVSMarkInspModuVATool(IMVSMarkInspModuVATool&);
            IMVSMarkInspModuVATool& operator=(IMVSMarkInspModuVATool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for MarkInspVA.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSMarkInspModuVA::IMVSMarkInspModuVATool * __stdcall GetMarkInspVAToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_MARKINSPVA_H_
