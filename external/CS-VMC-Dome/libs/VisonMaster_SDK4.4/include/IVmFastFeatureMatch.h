/***************************************************************************************************
 * File：IVmFastFeatureMatch.h
 * Note：@~chinese 快速匹配模块接口声明 @~english Interface for the FastFeatureMatch tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_FASTFEATUREMATCH_H_
#define _IVM_FASTFEATUREMATCH_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSFastFeatureMatchModu
    {
        /// @~chinese 匹配极性   @~english Match Polarity
        typedef enum _PolarityEnum
        {
            /// @~chinese 不考虑极性   @~english Ignored
            Polarity_No = 0x0,

            /// @~chinese 考虑极性   @~english Considered
            Polarity_Yes = 0x1,

        }PolarityEnum;

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

    /// \addtogroup 快速匹配
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///快速匹配ROI管理类
        class FastFeatureMatchRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

            /// @~chinese 掩膜图像（仅当次执行起效）   @~english Mask Image(Only valid for this execution)
            virtual void SetMaskImage(ImageBaseData stMaskImage) = 0;

        };

        /*  Note: Interface Classes of the parameter of the FastFeatureMatchParams.  */
        ///快速匹配参数类
        class FastFeatureMatchParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit FastFeatureMatchParams() {}
            virtual ~FastFeatureMatchParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual FastFeatureMatchRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 全部搜索模式   @~english Search All Mode
            __declspec(property(put = SetUseMatchAllMode, get = GetUseMatchAllMode)) bool UseMatchAllMode;

            virtual bool GetUseMatchAllMode() = 0;

            virtual void SetUseMatchAllMode(bool value) = 0;

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

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngleStart, get = GetAngleStart)) int AngleStart;

            virtual int GetAngleStart() = 0;

            virtual void SetAngleStart(int value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngleEnd, get = GetAngleEnd)) int AngleEnd;

            virtual int GetAngleEnd() = 0;

            virtual void SetAngleEnd(int value) = 0;

            /// @~chinese 尺度范围，范围：[0.1,10]   @~english Scale Range，range:[0.1,10]
            __declspec(property(put = SetScaleStart, get = GetScaleStart)) double ScaleStart;

            virtual double GetScaleStart() = 0;

            virtual void SetScaleStart(double value) = 0;

            /// @~chinese 尺度范围，范围：[0.1,10]   @~english Scale Range，range:[0.1,10]
            __declspec(property(put = SetScaleEnd, get = GetScaleEnd)) double ScaleEnd;

            virtual double GetScaleEnd() = 0;

            virtual void SetScaleEnd(double value) = 0;

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

            /// @~chinese 忽略匹配个数   @~english Ignore number of matches
            __declspec(property(put = SetOKWhenNumIsZero, get = GetOKWhenNumIsZero)) bool OKWhenNumIsZero;

            virtual bool GetOKWhenNumIsZero() = 0;

            virtual void SetOKWhenNumIsZero(bool value) = 0;

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

            /// @~chinese 尺度判断   @~english Scale Check
            __declspec(property(put = SetScaleLimitEnable, get = GetScaleLimitEnable)) bool ScaleLimitEnable;

            virtual bool GetScaleLimitEnable() = 0;

            virtual void SetScaleLimitEnable(bool value) = 0;

            /// @~chinese 尺度范围，范围：[0.0,10]   @~english Scale Range，range:[0.0,10]
            __declspec(property(put = SetScaleLimitLow, get = GetScaleLimitLow)) double ScaleLimitLow;

            virtual double GetScaleLimitLow() = 0;

            virtual void SetScaleLimitLow(double value) = 0;

            /// @~chinese 尺度范围，范围：[0.0,10]   @~english Scale Range，range:[0.0,10]
            __declspec(property(put = SetScaleLimitHigh, get = GetScaleLimitHigh)) double ScaleLimitHigh;

            virtual double GetScaleLimitHigh() = 0;

            virtual void SetScaleLimitHigh(double value) = 0;

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

            /**
            * @brief 获取单个模板选择状态
            * @param index [IN] 模板索引
            * @return 模板选择状态,出错时抛出异常
            */
            virtual bool GetModelStatus(int index) = 0;

            /**
            * @brief 设置单个模板选择状态
            * @param index [IN] 模板索引
            * @param status [IN] 模板选择状态
            */
            virtual void SetModelStatus(int index, bool status) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes for describing edge points Information.  */
		/// 快速匹配边缘点信息
        class IPatMatchEdgePoint
        {
        protected:
            ///< Constructor and Destructor
            explicit IPatMatchEdgePoint() {}
            virtual ~IPatMatchEdgePoint() {}

        public:
            /**
             * @brief 边缘点位置
             */
            virtual MVD_POINT_F GetPosition() = 0;
            /**
             * @brief 边缘点得分
             */
            virtual float GetScore() = 0;
            /**
             * @brief 边缘点索引（权重）
             */
            virtual unsigned int GetIndex() = 0;
        };

        /*  Note: Interface Classes for describing outline information.  */
		/// 快速匹配轮廓点信息
        class IPatMatchOutline
        {
        protected:
            ///< Constructor and Destructor
            explicit IPatMatchOutline() {}
            virtual ~IPatMatchOutline() {}

        public:
            /**
             * @brief 边缘点数
             */
            virtual int GetPointNum() = 0;
            /**
             * @brief 获取单个轮廓点信息
             * @param nPtIndex [IN] 点索引，从0开始计数
             * @return 无，出错时抛出异常
             */
            virtual IPatMatchEdgePoint* GetEdgePoint( unsigned int nPtIndex ) = 0;
        };

        /*  Note: Interface Classes for describing single match info.  */
		/// 快速匹配单个匹配信息
        class IFastFeatureMatchInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IFastFeatureMatchInfo() {}
            virtual ~IFastFeatureMatchInfo() {}

        public:
            /**
             * @brief 匹配框的位置
             */
            virtual RectBox GetMatchBox() = 0;
            /**
             * @brief 匹配点
             */
            virtual MVD_POINT_F GetMatchPoint() = 0;
            /**
             * @brief 尺度
             */
            virtual float GetScale() = 0;
            /**
             * @brief X方向尺度
             */
            virtual float GetScaleX() = 0;
            /**
             * @brief Y方向尺度
             */
            virtual float GetScaleY() = 0;
            /**
             * @brief 匹配得分
             */
            virtual float GetScore() = 0;
            /**
             * @brief 匹配模板编号
             */
            virtual int GetMatchModelIndex() = 0;
            /**
             * @brief 匹配模板名称
             */
            virtual const char * GetMatchModelName() = 0;
        };
        /*  Note: Interface Classes of the result of the FastFeatureMatchResults.  */
        ///快速匹配结果类
        class FastFeatureMatchResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit FastFeatureMatchResults() {}
            virtual ~FastFeatureMatchResults() {}

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
             * @brief 匹配个数
             */
            virtual int GetMatchNum() = 0;
            /**
             * @brief 获取匹配信息
             * @param nMatchIndex [IN] 匹配信息链表索引，从0开始计
             * @return 返回匹配信息
             */
            virtual IFastFeatureMatchInfo* GetMatchInfo( int nMatchIndex ) = 0;
            /**
             * @brief 获取匹配轮廓
             * @return 轮廓信息
             * @remarks a).出错时抛出异常 b).匹配基本参数设置启用轮廓，方可获取。
             */
            virtual IPatMatchOutline* GetOutline() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the FastFeatureMatch tool.  */
        ///快速匹配工具类
        class IMVSFastFeatureMatchModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSFastFeatureMatchModuTool() {}
            virtual ~IMVSFastFeatureMatchModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual FastFeatureMatchParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual FastFeatureMatchResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

           /**
			* @brief 导入模型
			* @param strPaths   [IN]  模型路径
            * @param nNum   [IN]  模型数量
			* @return 无，出错时抛出异常
			*/
            virtual void ImportModelData(IN ModuleImportData * strPaths, IN const int nNum) = 0;
        private:
            IMVSFastFeatureMatchModuTool(IMVSFastFeatureMatchModuTool&);
            IMVSFastFeatureMatchModuTool& operator=(IMVSFastFeatureMatchModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for FastFeatureMatch.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSFastFeatureMatchModu::IMVSFastFeatureMatchModuTool * __stdcall GetFastFeatureMatchToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_FASTFEATUREMATCH_H_
