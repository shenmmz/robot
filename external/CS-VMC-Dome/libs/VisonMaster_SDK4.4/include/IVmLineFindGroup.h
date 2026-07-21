/***************************************************************************************************
 * File：IVmLineFindGroup.h
 * Note：@~chinese 直线查找组合模块接口声明 @~english Interface for the LineFindGroup tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_LINEFINDGROUP_H_
#define _IVM_LINEFINDGROUP_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSLineFindGroupModu
    {
        /// @~chinese 初始化类型   @~english Init Type
        typedef enum _InitTypeEnum
        {
            /// @~chinese 全局法   @~english Global Optimum
            InitType_Global = 0x1,

            /// @~chinese 局部最优   @~english Local Optimum
            InitType_Exhaustion = 0x2,

        }InitTypeEnum;

        /// @~chinese 权重函数   @~english Weighting Function
        typedef enum _WeightFuncEnum
        {
            /// @~chinese 最小二乘   @~english Least Squares
            WeightFunc_LeastSquare = 0x1,

            /// @~chinese Huber函数   @~english Huber Function
            WeightFunc_Huber = 0x2,

            /// @~chinese Tukey函数   @~english Tukey Function
            WeightFunc_Tukey = 0x3,

        }WeightFuncEnum;

    /// \addtogroup 直线查找组合
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the LineFindGroupParams.  */
        ///直线查找组合参数类
        class LineFindGroupParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit LineFindGroupParams() {}
            virtual ~LineFindGroupParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 剔除点数，范围：[0,100]   @~english Reject Number，range:[0,100]
            __declspec(property(put = SetRejectNum, get = GetRejectNum)) int RejectNum;

            virtual int GetRejectNum() = 0;

            virtual void SetRejectNum(int value) = 0;

            /// @~chinese 剔除距离，范围：[1,1000]   @~english Distance to Remove，range:[1,1000]
            __declspec(property(put = SetRejectDist, get = GetRejectDist)) int RejectDist;

            virtual int GetRejectDist() = 0;

            virtual void SetRejectDist(int value) = 0;

            /// @~chinese 初始化类型   @~english Init Type
            __declspec(property(put = SetInitType, get = GetInitType)) InitTypeEnum InitType;

            virtual InitTypeEnum GetInitType() = 0;

            virtual void SetInitType(InitTypeEnum value) = 0;

            /// @~chinese 权重函数   @~english Weighting Function
            __declspec(property(put = SetWeightFunc, get = GetWeightFunc)) WeightFuncEnum WeightFunc;

            virtual WeightFuncEnum GetWeightFunc() = 0;

            virtual void SetWeightFunc(WeightFuncEnum value) = 0;

            /// @~chinese 最大迭代次数，范围：[1,100]   @~english Max Iteration，range:[1,100]
            __declspec(property(put = SetMaxIters, get = GetMaxIters)) int MaxIters;

            virtual int GetMaxIters() = 0;

            virtual void SetMaxIters(int value) = 0;

            /// @~chinese 匹配点数判断   @~english Fit Points Check
            __declspec(property(put = SetNumLimitEnable, get = GetNumLimitEnable)) bool NumLimitEnable;

            virtual bool GetNumLimitEnable() = 0;

            virtual void SetNumLimitEnable(bool value) = 0;

            /// @~chinese 拟合点数范围，范围：[0,100]   @~english Fit Points Range，range:[0,100]
            __declspec(property(put = SetNumLimitLow, get = GetNumLimitLow)) int NumLimitLow;

            virtual int GetNumLimitLow() = 0;

            virtual void SetNumLimitLow(int value) = 0;

            /// @~chinese 拟合点数范围，范围：[0,100]   @~english Fit Points Range，range:[0,100]
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

            /// @~chinese 拟合误差判断   @~english Fit Error Check
            __declspec(property(put = SetScoreLimitEnable, get = GetScoreLimitEnable)) bool ScoreLimitEnable;

            virtual bool GetScoreLimitEnable() = 0;

            virtual void SetScoreLimitEnable(bool value) = 0;

            /// @~chinese 拟合误差范围，范围：[0,9999]   @~english Fit Error Range，range:[0,9999]
            __declspec(property(put = SetScoreLimitLow, get = GetScoreLimitLow)) double ScoreLimitLow;

            virtual double GetScoreLimitLow() = 0;

            virtual void SetScoreLimitLow(double value) = 0;

            /// @~chinese 拟合误差范围，范围：[0,9999]   @~english Fit Error Range，range:[0,9999]
            __declspec(property(put = SetScoreLimitHigh, get = GetScoreLimitHigh)) double ScoreLimitHigh;

            virtual double GetScoreLimitHigh() = 0;

            virtual void SetScoreLimitHigh(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the LineFindGroupResults.  */
        ///直线查找组合结果类
        class LineFindGroupResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit LineFindGroupResults() {}
            virtual ~LineFindGroupResults() {}

        public:
             /**
             * @brief 卡尺检测区
             */
             virtual RectBox GetDetectCaliperBox(int nIndex) = 0;
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 输出直线
             */
            virtual Line GetOutputLine() = 0;

            /**
             * @brief 输出拟合点
             */
            virtual MVD_POINT_F * GetOutputFittingPoint() = 0;

            /**
             * @brief 拟合误差
             */
            virtual float GetLineFitScore() = 0;

            /**
             * @brief 拟合状态
             */
            virtual int GetLineFitStatus() = 0;

            /**
             * @brief 拟合点数
             */
            virtual int GetFitPointNum() = 0;

            /**
             * @brief 拟合距离
             */
            virtual float GetFitDist(int nIndex) = 0;

            /**
             * @brief 匹配点
             */
            virtual int GetFitUsedPoint(int nIndex) = 0;

            /**
             * @brief 匹配点数
             */
            virtual int GetFitUsedPointNum() = 0;

            /**
             * @brief 最小外接矩形数量
             */
            virtual int GetMinBoudingRectNum() = 0;

            /**
             * @brief 最小外接矩形
             */
            virtual RectBox GetMinBoudingRect(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the LineFindGroup tool.  */
        ///直线查找组合工具类
        class IMVSLineFindGroupModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSLineFindGroupModuTool() {}
            virtual ~IMVSLineFindGroupModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual LineFindGroupParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual LineFindGroupResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSLineFindGroupModuTool(IMVSLineFindGroupModuTool&);
            IMVSLineFindGroupModuTool& operator=(IMVSLineFindGroupModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for LineFindGroup.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSLineFindGroupModu::IMVSLineFindGroupModuTool * __stdcall GetLineFindGroupToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_LINEFINDGROUP_H_
