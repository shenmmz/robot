/***************************************************************************************************
 * File：IVmGrayMatch.h
 * Note：@~chinese 灰度匹配模块接口声明 @~english Interface for the GrayMatch tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_GRAYMATCH_H_
#define _IVM_GRAYMATCH_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSGrayMatchModu
    {
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

        /// @~chinese 匹配极性   @~english Match Polarity
        typedef enum _PolarityEnum
        {
            /// @~chinese 不考虑极性   @~english Ignored
            Polarity_No = 0x0,

            /// @~chinese 考虑极性   @~english Considered
            Polarity_Yes = 0x1,

        }PolarityEnum;

    /// \addtogroup 灰度匹配
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///灰度匹配ROI管理类
        class GrayMatchRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the GrayMatchParams.  */
        ///灰度匹配参数类
        class GrayMatchParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit GrayMatchParams() {}
            virtual ~GrayMatchParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual GrayMatchRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 最小匹配分数，范围：[0.1,1.0]   @~english Min Match Score，range:[0.1,1.0]
            __declspec(property(put = SetMinScore, get = GetMinScore)) double MinScore;

            virtual double GetMinScore() = 0;

            virtual void SetMinScore(double value) = 0;

            /// @~chinese 最大匹配个数，范围：[1,1000]   @~english Max Number to find，range:[1,1000]
            __declspec(property(put = SetMaxMatchNum, get = GetMaxMatchNum)) int MaxMatchNum;

            virtual int GetMaxMatchNum() = 0;

            virtual void SetMaxMatchNum(int value) = 0;

            /// @~chinese 角度步长，范围：[1,360]   @~english Step Of Angle，range:[1,360]
            __declspec(property(put = SetAngleStep, get = GetAngleStep)) int AngleStep;

            virtual int GetAngleStep() = 0;

            virtual void SetAngleStep(int value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngleStart, get = GetAngleStart)) int AngleStart;

            virtual int GetAngleStart() = 0;

            virtual void SetAngleStart(int value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngleEnd, get = GetAngleEnd)) int AngleEnd;

            virtual int GetAngleEnd() = 0;

            virtual void SetAngleEnd(int value) = 0;

            /// @~chinese 最大重叠率，范围：[0,100]   @~english Overlap Threshold，range:[0,100]
            __declspec(property(put = SetMaxOverlap, get = GetMaxOverlap)) int MaxOverlap;

            virtual int GetMaxOverlap() = 0;

            virtual void SetMaxOverlap(int value) = 0;

            /// @~chinese 排序类型   @~english Sort
            __declspec(property(put = SetSortType, get = GetSortType)) SortTypeEnum SortType;

            virtual SortTypeEnum GetSortType() = 0;

            virtual void SetSortType(SortTypeEnum value) = 0;

            /// @~chinese 匹配极性   @~english Match Polarity
            __declspec(property(put = SetPolarity, get = GetPolarity)) PolarityEnum Polarity;

            virtual PolarityEnum GetPolarity() = 0;

            virtual void SetPolarity(PolarityEnum value) = 0;

            /// @~chinese 超时控制，范围：[0,10000]   @~english Overtime Control，range:[0,10000]
            __declspec(property(put = SetTimeOut, get = GetTimeOut)) int TimeOut;

            virtual int GetTimeOut() = 0;

            virtual void SetTimeOut(int value) = 0;

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
        ///Note: 灰度模板匹配信息类
        class IGrayMatchInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IGrayMatchInfo() {}
            virtual ~IGrayMatchInfo() {}

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

        /*  Note: Interface Classes of the result of the GrayMatchResults.  */
        ///灰度匹配结果类
        class GrayMatchResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit GrayMatchResults() {}
            virtual ~GrayMatchResults() {}

        public:
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
             * @param nMatchIndex [IN] 匹配信息链表索引，从0开始计数
             * @return 返回匹配信息

             */
            virtual IGrayMatchInfo* GetMatchInfo(int nMatchIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the GrayMatch tool.  */
        ///灰度匹配工具类
        class IMVSGrayMatchModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSGrayMatchModuTool() {}
            virtual ~IMVSGrayMatchModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual GrayMatchParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual GrayMatchResults* GetResult() = 0;

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
            IMVSGrayMatchModuTool(IMVSGrayMatchModuTool&);
            IMVSGrayMatchModuTool& operator=(IMVSGrayMatchModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for GrayMatch.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSGrayMatchModu::IMVSGrayMatchModuTool * __stdcall GetGrayMatchToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_GRAYMATCH_H_
