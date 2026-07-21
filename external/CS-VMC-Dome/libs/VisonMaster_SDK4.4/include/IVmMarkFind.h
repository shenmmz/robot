/***************************************************************************************************
 * File：IVmMarkFind.h
 * Note：@~chinese 图形定位模块接口声明 @~english Interface for the MarkFind tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_MARKFIND_H_
#define _IVM_MARKFIND_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSMarkFindModu
    {
        /// @~chinese 匹配极性   @~english Match Polarity
        typedef enum _PolarityEnum
        {
            /// @~chinese 不考虑极性   @~english Ignored
            Polarity_No = 0x0,

            /// @~chinese 考虑极性   @~english Considered
            Polarity_Yes = 0x1,

        }PolarityEnum;

        /// @~chinese 定位类型   @~english Mark LocationType
        typedef enum _MarkLocationTypeEnum
        {
            /// @~chinese 直接映射   @~english Direct Mapping
            MarkLocationType_No = 0x0,

            /// @~chinese 二次修正   @~english Second Correction
            MarkLocationType_Yes = 0x1,

        }MarkLocationTypeEnum;

        /// @~chinese 排序类型   @~english Sort
        typedef enum _SortTypeEnum
        {
            /// @~chinese 按分数降序排序   @~english Score: High to Low
            SortType_Score = 0x2,

            /// @~chinese 按角度降序排序   @~english Angle: Big to Small
            SortType_Angle = 0x3,

            /// @~chinese 按X由小到大排序   @~english X: Small to Big
            SortType_X = 0x4,

            /// @~chinese 按Y由小到大排序   @~english Y: Small to Big
            SortType_Y = 0x5,

            /// @~chinese X由小到大，Y由小到大   @~english X: Small to Big, Y: Small to Big
            SortType_XY = 0x6,

            /// @~chinese Y由小到大，X由小到大   @~english Y: Small to Big, X: Small to Big
            SortType_YX = 0x7,

        }SortTypeEnum;

        /// @~chinese 阈值类型   @~english Threshold Type
        typedef enum _MatchThresholdFlagEnum
        {
            /// @~chinese 自动阈值   @~english Auto Threshold
            MatchThresholdFlag_Auto = 0x0,

            /// @~chinese 模板阈值   @~english Model Threshold
            MatchThresholdFlag_Model = 0x1,

            /// @~chinese 手动阈值   @~english Manual Threshold
            MatchThresholdFlag_Manual = 0x2,

        }MatchThresholdFlagEnum;

    /// \addtogroup 图形定位
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///图形定位ROI管理类
        class MarkFindRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

            /// @~chinese 掩膜图像（仅当次执行起效）   @~english Mask Image(Only valid for this execution)
            virtual void SetMaskImage(ImageBaseData stMaskImage) = 0;

        };

        /*  Note: Interface Classes of the parameter of the MarkFindParams.  */
        ///图形定位参数类
        class MarkFindParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MarkFindParams() {}
            virtual ~MarkFindParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual MarkFindRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 最小匹配分数，范围：[0,1.0]   @~english Min Match Score，range:[0,1.0]
            __declspec(property(put = SetMinScore, get = GetMinScore)) double MinScore;

            virtual double GetMinScore() = 0;

            virtual void SetMinScore(double value) = 0;

            /// @~chinese 最大匹配个数，范围：[1,50000]   @~english Max Number to find，range:[1,50000]
            __declspec(property(put = SetMaxMatchNum, get = GetMaxMatchNum)) int MaxMatchNum;

            virtual int GetMaxMatchNum() = 0;

            virtual void SetMaxMatchNum(int value) = 0;

            /// @~chinese 匹配极性   @~english Match Polarity
            __declspec(property(put = SetPolarity, get = GetPolarity)) PolarityEnum Polarity;

            virtual PolarityEnum GetPolarity() = 0;

            virtual void SetPolarity(PolarityEnum value) = 0;

            /// @~chinese 角度范围，范围：[-90,90]   @~english Angle Range，range:[-90,90]
            __declspec(property(put = SetAngleStart, get = GetAngleStart)) int AngleStart;

            virtual int GetAngleStart() = 0;

            virtual void SetAngleStart(int value) = 0;

            /// @~chinese 角度范围，范围：[-90,90]   @~english Angle Range，range:[-90,90]
            __declspec(property(put = SetAngleEnd, get = GetAngleEnd)) int AngleEnd;

            virtual int GetAngleEnd() = 0;

            virtual void SetAngleEnd(int value) = 0;

            /// @~chinese 尺度范围，范围：[0.1,10](弃用)   @~english Scale Range，range:[0.1,10](Obsolete)
            __declspec(property(put = SetScaleStart, get = GetScaleStart)) double ScaleStart;

            virtual double GetScaleStart() = 0;

            virtual void SetScaleStart(double value) = 0;

            /// @~chinese 尺度范围，范围：[0.1,10](弃用)   @~english Scale Range，range:[0.1,10](Obsolete)
            __declspec(property(put = SetScaleEnd, get = GetScaleEnd)) double ScaleEnd;

            virtual double GetScaleEnd() = 0;

            virtual void SetScaleEnd(double value) = 0;

            /// @~chinese 定位类型   @~english Mark LocationType
            __declspec(property(put = SetMarkLocationType, get = GetMarkLocationType)) MarkLocationTypeEnum MarkLocationType;

            virtual MarkLocationTypeEnum GetMarkLocationType() = 0;

            virtual void SetMarkLocationType(MarkLocationTypeEnum value) = 0;

            /// @~chinese X尺度范围，范围：[0.1,10](弃用)   @~english X Scale Range，range:[0.1,10](Obsolete)
            __declspec(property(put = SetScaleXStart, get = GetScaleXStart)) double ScaleXStart;

            virtual double GetScaleXStart() = 0;

            virtual void SetScaleXStart(double value) = 0;

            /// @~chinese X尺度范围，范围：[0.1,10](弃用)   @~english X Scale Range，range:[0.1,10](Obsolete)
            __declspec(property(put = SetScaleXEnd, get = GetScaleXEnd)) double ScaleXEnd;

            virtual double GetScaleXEnd() = 0;

            virtual void SetScaleXEnd(double value) = 0;

            /// @~chinese Y尺度范围，范围：[0.1,10](弃用)   @~english Y Scale Range，range:[0.1,10](Obsolete)
            __declspec(property(put = SetScaleYStart, get = GetScaleYStart)) double ScaleYStart;

            virtual double GetScaleYStart() = 0;

            virtual void SetScaleYStart(double value) = 0;

            /// @~chinese Y尺度范围，范围：[0.1,10](弃用)   @~english Y Scale Range，range:[0.1,10](Obsolete)
            __declspec(property(put = SetScaleYEnd, get = GetScaleYEnd)) double ScaleYEnd;

            virtual double GetScaleYEnd() = 0;

            virtual void SetScaleYEnd(double value) = 0;

            /// @~chinese 最大重叠率，范围：[0,100]   @~english Overlap Threshold，range:[0,100]
            __declspec(property(put = SetMaxOverlap, get = GetMaxOverlap)) int MaxOverlap;

            virtual int GetMaxOverlap() = 0;

            virtual void SetMaxOverlap(int value) = 0;

            /// @~chinese 排序类型   @~english Sort
            __declspec(property(put = SetSortType, get = GetSortType)) SortTypeEnum SortType;

            virtual SortTypeEnum GetSortType() = 0;

            virtual void SetSortType(SortTypeEnum value) = 0;

            /// @~chinese 阈值类型   @~english Threshold Type
            __declspec(property(put = SetMatchThresholdFlag, get = GetMatchThresholdFlag)) MatchThresholdFlagEnum MatchThresholdFlag;

            virtual MatchThresholdFlagEnum GetMatchThresholdFlag() = 0;

            virtual void SetMatchThresholdFlag(MatchThresholdFlagEnum value) = 0;

            /// @~chinese 边缘阈值，范围：[1,255]   @~english Edge Threshold，range:[1,255]
            __declspec(property(put = SetMatchThresholdHigh, get = GetMatchThresholdHigh)) int MatchThresholdHigh;

            virtual int GetMatchThresholdHigh() = 0;

            virtual void SetMatchThresholdHigh(int value) = 0;

            /// @~chinese 是否考虑噪点   @~english Noise Considered
            __declspec(property(put = SetSpotterFlag, get = GetSpotterFlag)) bool SpotterFlag;

            virtual bool GetSpotterFlag() = 0;

            virtual void SetSpotterFlag(bool value) = 0;

            /// @~chinese 延拓阈值，范围：[0,90]   @~english Extension Threshold，range:[0,90]
            __declspec(property(put = SetMatchExtentRate, get = GetMatchExtentRate)) int MatchExtentRate;

            virtual int GetMatchExtentRate() = 0;

            virtual void SetMatchExtentRate(int value) = 0;

            /// @~chinese 超时控制，范围：[0,10000]   @~english Overtime Control，range:[0,10000]
            __declspec(property(put = SetTimeOut, get = GetTimeOut)) int TimeOut;

            virtual int GetTimeOut() = 0;

            virtual void SetTimeOut(int value) = 0;

            /// @~chinese 轮廓使能   @~english Contour Enabled
            __declspec(property(put = SetOutLineEnable, get = GetOutLineEnable)) bool OutLineEnable;

            virtual bool GetOutLineEnable() = 0;

            virtual void SetOutLineEnable(bool value) = 0;

            /// @~chinese 数量判断   @~english Quantity Check
            __declspec(property(put = SetNumLimitEnable, get = GetNumLimitEnable)) bool NumLimitEnable;

            virtual bool GetNumLimitEnable() = 0;

            virtual void SetNumLimitEnable(bool value) = 0;

            /// @~chinese 数量范围，范围：[0,99999]   @~english Quantity Range，range:[0,99999]
            __declspec(property(put = SetNumLimitLow, get = GetNumLimitLow)) int NumLimitLow;

            virtual int GetNumLimitLow() = 0;

            virtual void SetNumLimitLow(int value) = 0;

            /// @~chinese 数量范围，范围：[0,99999]   @~english Quantity Range，range:[0,99999]
            __declspec(property(put = SetNumLimitHigh, get = GetNumLimitHigh)) int NumLimitHigh;

            virtual int GetNumLimitHigh() = 0;

            virtual void SetNumLimitHigh(int value) = 0;

            /// @~chinese 角度判断   @~english Angle Check
            __declspec(property(put = SetAngleLimitEnable, get = GetAngleLimitEnable)) bool AngleLimitEnable;

            virtual bool GetAngleLimitEnable() = 0;

            virtual void SetAngleLimitEnable(bool value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngleLimitLow, get = GetAngleLimitLow)) double AngleLimitLow;

            virtual double GetAngleLimitLow() = 0;

            virtual void SetAngleLimitLow(double value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngleLimitHigh, get = GetAngleLimitHigh)) double AngleLimitHigh;

            virtual double GetAngleLimitHigh() = 0;

            virtual void SetAngleLimitHigh(double value) = 0;

            /// @~chinese X尺度判断(弃用)   @~english X Scale Check(Obsolete)
            __declspec(property(put = SetScaleXLimitEnable, get = GetScaleXLimitEnable)) bool ScaleXLimitEnable;

            virtual bool GetScaleXLimitEnable() = 0;

            virtual void SetScaleXLimitEnable(bool value) = 0;

            /// @~chinese X尺度范围，范围：[0.0,10](弃用)   @~english X Scale Range，range:[0.0,10](Obsolete)
            __declspec(property(put = SetScaleXLimitLow, get = GetScaleXLimitLow)) double ScaleXLimitLow;

            virtual double GetScaleXLimitLow() = 0;

            virtual void SetScaleXLimitLow(double value) = 0;

            /// @~chinese X尺度范围，范围：[0.0,10](弃用)   @~english X Scale Range，range:[0.0,10](Obsolete)
            __declspec(property(put = SetScaleXLimitHigh, get = GetScaleXLimitHigh)) double ScaleXLimitHigh;

            virtual double GetScaleXLimitHigh() = 0;

            virtual void SetScaleXLimitHigh(double value) = 0;

            /// @~chinese Y尺度判断(弃用)   @~english Y Scale Check(Obsolete)
            __declspec(property(put = SetScaleYLimitEnable, get = GetScaleYLimitEnable)) bool ScaleYLimitEnable;

            virtual bool GetScaleYLimitEnable() = 0;

            virtual void SetScaleYLimitEnable(bool value) = 0;

            /// @~chinese Y尺度范围，范围：[0,10](弃用)   @~english Y Scale Range，range:[0,10](Obsolete)
            __declspec(property(put = SetScaleYLimitLow, get = GetScaleYLimitLow)) double ScaleYLimitLow;

            virtual double GetScaleYLimitLow() = 0;

            virtual void SetScaleYLimitLow(double value) = 0;

            /// @~chinese Y尺度范围，范围：[0,10](弃用)   @~english Y Scale Range，range:[0,10](Obsolete)
            __declspec(property(put = SetScaleYLimitHigh, get = GetScaleYLimitHigh)) double ScaleYLimitHigh;

            virtual double GetScaleYLimitHigh() = 0;

            virtual void SetScaleYLimitHigh(double value) = 0;

            /// @~chinese 分数判断   @~english Score Check
            __declspec(property(put = SetScoreLimitEnable, get = GetScoreLimitEnable)) bool ScoreLimitEnable;

            virtual bool GetScoreLimitEnable() = 0;

            virtual void SetScoreLimitEnable(bool value) = 0;

            /// @~chinese 分数范围，范围：[0,1]   @~english Score Range，range:[0,1]
            __declspec(property(put = SetScoreLimitLow, get = GetScoreLimitLow)) double ScoreLimitLow;

            virtual double GetScoreLimitLow() = 0;

            virtual void SetScoreLimitLow(double value) = 0;

            /// @~chinese 分数范围，范围：[0,1]   @~english Score Range，range:[0,1]
            __declspec(property(put = SetScoreLimitHigh, get = GetScoreLimitHigh)) double ScoreLimitHigh;

            virtual double GetScoreLimitHigh() = 0;

            virtual void SetScoreLimitHigh(double value) = 0;

            /// @~chinese 匹配点X判断   @~english Match Point X Check
            __declspec(property(put = SetMatchPointXLimitEnable, get = GetMatchPointXLimitEnable)) bool MatchPointXLimitEnable;

            virtual bool GetMatchPointXLimitEnable() = 0;

            virtual void SetMatchPointXLimitEnable(bool value) = 0;

            /// @~chinese 匹配点X范围，范围：[-99999,99999]   @~english Match Point X Range，range:[-99999,99999]
            __declspec(property(put = SetMatchPointXLimitLow, get = GetMatchPointXLimitLow)) double MatchPointXLimitLow;

            virtual double GetMatchPointXLimitLow() = 0;

            virtual void SetMatchPointXLimitLow(double value) = 0;

            /// @~chinese 匹配点X范围，范围：[-99999,99999]   @~english Match Point X Range，range:[-99999,99999]
            __declspec(property(put = SetMatchPointXLimitHigh, get = GetMatchPointXLimitHigh)) double MatchPointXLimitHigh;

            virtual double GetMatchPointXLimitHigh() = 0;

            virtual void SetMatchPointXLimitHigh(double value) = 0;

            /// @~chinese 匹配点Y判断   @~english Match Point Y Check
            __declspec(property(put = SetMatchPointYLimitEnable, get = GetMatchPointYLimitEnable)) bool MatchPointYLimitEnable;

            virtual bool GetMatchPointYLimitEnable() = 0;

            virtual void SetMatchPointYLimitEnable(bool value) = 0;

            /// @~chinese 匹配点Y范围，范围：[-99999,99999]   @~english Match Point Y Range，range:[-99999,99999]
            __declspec(property(put = SetMatchPointYLimitLow, get = GetMatchPointYLimitLow)) double MatchPointYLimitLow;

            virtual double GetMatchPointYLimitLow() = 0;

            virtual void SetMatchPointYLimitLow(double value) = 0;

            /// @~chinese 匹配点Y范围，范围：[-99999,99999]   @~english Match Point Y Range，range:[-99999,99999]
            __declspec(property(put = SetMatchPointYLimitHigh, get = GetMatchPointYLimitHigh)) double MatchPointYLimitHigh;

            virtual double GetMatchPointYLimitHigh() = 0;

            virtual void SetMatchPointYLimitHigh(double value) = 0;

            /// @~chinese 中心点X判断   @~english Central Point X Check
            __declspec(property(put = SetBoxPointXLimitEnable, get = GetBoxPointXLimitEnable)) bool BoxPointXLimitEnable;

            virtual bool GetBoxPointXLimitEnable() = 0;

            virtual void SetBoxPointXLimitEnable(bool value) = 0;

            /// @~chinese 中心点X范围，范围：[-99999,99999]   @~english Central Point X Range，range:[-99999,99999]
            __declspec(property(put = SetBoxPointXLimitLow, get = GetBoxPointXLimitLow)) double BoxPointXLimitLow;

            virtual double GetBoxPointXLimitLow() = 0;

            virtual void SetBoxPointXLimitLow(double value) = 0;

            /// @~chinese 中心点X范围，范围：[-99999,99999]   @~english Central Point X Range，range:[-99999,99999]
            __declspec(property(put = SetBoxPointXLimitHigh, get = GetBoxPointXLimitHigh)) double BoxPointXLimitHigh;

            virtual double GetBoxPointXLimitHigh() = 0;

            virtual void SetBoxPointXLimitHigh(double value) = 0;

            /// @~chinese 中心点Y判断   @~english Central Point Y Check
            __declspec(property(put = SetBoxPointYLimitEnable, get = GetBoxPointYLimitEnable)) bool BoxPointYLimitEnable;

            virtual bool GetBoxPointYLimitEnable() = 0;

            virtual void SetBoxPointYLimitEnable(bool value) = 0;

            /// @~chinese 中心点Y范围，范围：[-99999,99999]   @~english Central Point Y Range，range:[-99999,99999]
            __declspec(property(put = SetBoxPointYLimitLow, get = GetBoxPointYLimitLow)) double BoxPointYLimitLow;

            virtual double GetBoxPointYLimitLow() = 0;

            virtual void SetBoxPointYLimitLow(double value) = 0;

            /// @~chinese 中心点Y范围，范围：[-99999,99999]   @~english Central Point Y Range，range:[-99999,99999]
            __declspec(property(put = SetBoxPointYLimitHigh, get = GetBoxPointYLimitHigh)) double BoxPointYLimitHigh;

            virtual double GetBoxPointYLimitHigh() = 0;

            virtual void SetBoxPointYLimitHigh(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the MarkFindResults.  */
        ///图形定位结果类
        class MarkFindResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MarkFindResults() {}
            virtual ~MarkFindResults() {}

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
             * @brief 匹配状态
             */
            virtual int GetMatchStatus(int nIndex) = 0;            

            /**
             * @brief 匹配个数
             */
            virtual int GetMatchNum() = 0;

            /**
             * @brief 匹配框
             */
            virtual RectBox GetMatchRect(int nIndex) = 0;

            /**
             * @brief 匹配点
             */
            virtual MVD_POINT_F GetMatchPoint(int nIndex) = 0;

            /**
             * @brief 直线数量
             */
            virtual int GetLineNum() = 0;

            /**
             * @brief 直线状态
             */
            virtual int GetLineStatus(int nIndex) = 0;

            /**
             * @brief 输出直线
             */
            virtual Line GetOutputLine(int nIndex) = 0;

            /**
             * @brief 尺度X
             */
            virtual float GetMatchScaleX(int nIndex) = 0;

            /**
             * @brief 尺度Y
             */
            virtual float GetMatchScaleY(int nIndex) = 0;

            /**
             * @brief 分数
             */
            virtual float GetMatchScore(int nIndex) = 0;

            /**
             * @brief 匹配轮廓信息
             */
            virtual MatchOutline GetMatchOutline() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the MarkFind tool.  */
        ///图形定位工具类
        class IMVSMarkFindModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSMarkFindModuTool() {}
            virtual ~IMVSMarkFindModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual MarkFindParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual MarkFindResults* GetResult() = 0;

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
            IMVSMarkFindModuTool(IMVSMarkFindModuTool&);
            IMVSMarkFindModuTool& operator=(IMVSMarkFindModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for MarkFind.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSMarkFindModu::IMVSMarkFindModuTool * __stdcall GetMarkFindToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_MARKFIND_H_
