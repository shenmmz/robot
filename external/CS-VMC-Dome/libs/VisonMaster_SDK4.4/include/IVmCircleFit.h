/***************************************************************************************************
 * File：IVmCircleFit.h
 * Note：@~chinese 圆拟合模块接口声明 @~english Interface for the CircleFit tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CIRCLEFIT_H_
#define _IVM_CIRCLEFIT_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCircleFitModu
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

    /// \addtogroup 圆拟合
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the CircleFitParams.  */
        ///圆拟合参数类
        class CircleFitParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CircleFitParams() {}
            virtual ~CircleFitParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入拟合点(仅当次执行起效)   @~english InputFittingPoint(Only valid for this execution)
            virtual void SetInputFittingPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

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

            /// @~chinese 半径判断   @~english Radius Check
            __declspec(property(put = SetRadiusLimitEnable, get = GetRadiusLimitEnable)) bool RadiusLimitEnable;

            virtual bool GetRadiusLimitEnable() = 0;

            virtual void SetRadiusLimitEnable(bool value) = 0;

            /// @~chinese 半径范围，范围：[0,99999]   @~english Radius Range，range:[0,99999]
            __declspec(property(put = SetRadiusLimitLow, get = GetRadiusLimitLow)) double RadiusLimitLow;

            virtual double GetRadiusLimitLow() = 0;

            virtual void SetRadiusLimitLow(double value) = 0;

            /// @~chinese 半径范围，范围：[0,99999]   @~english Radius Range，range:[0,99999]
            __declspec(property(put = SetRadiusLimitHigh, get = GetRadiusLimitHigh)) double RadiusLimitHigh;

            virtual double GetRadiusLimitHigh() = 0;

            virtual void SetRadiusLimitHigh(double value) = 0;

            /// @~chinese 中心X判断   @~english Center X Check
            __declspec(property(put = SetCenterXLimitEnable, get = GetCenterXLimitEnable)) bool CenterXLimitEnable;

            virtual bool GetCenterXLimitEnable() = 0;

            virtual void SetCenterXLimitEnable(bool value) = 0;

            /// @~chinese 中心X范围，范围：[-99999,99999]   @~english Center X Range，range:[-99999,99999]
            __declspec(property(put = SetCenterXLimitLow, get = GetCenterXLimitLow)) double CenterXLimitLow;

            virtual double GetCenterXLimitLow() = 0;

            virtual void SetCenterXLimitLow(double value) = 0;

            /// @~chinese 中心X范围，范围：[-99999,99999]   @~english Center X Range，range:[-99999,99999]
            __declspec(property(put = SetCenterXLimitHigh, get = GetCenterXLimitHigh)) double CenterXLimitHigh;

            virtual double GetCenterXLimitHigh() = 0;

            virtual void SetCenterXLimitHigh(double value) = 0;

            /// @~chinese 中心Y判断   @~english Center Y Check
            __declspec(property(put = SetCenterYLimitEnable, get = GetCenterYLimitEnable)) bool CenterYLimitEnable;

            virtual bool GetCenterYLimitEnable() = 0;

            virtual void SetCenterYLimitEnable(bool value) = 0;

            /// @~chinese 中心Y范围，范围：[-99999,99999]   @~english Center Y Range，range:[-99999,99999]
            __declspec(property(put = SetCenterYLimitLow, get = GetCenterYLimitLow)) double CenterYLimitLow;

            virtual double GetCenterYLimitLow() = 0;

            virtual void SetCenterYLimitLow(double value) = 0;

            /// @~chinese 中心Y范围，范围：[-99999,99999]   @~english Center Y Range，range:[-99999,99999]
            __declspec(property(put = SetCenterYLimitHigh, get = GetCenterYLimitHigh)) double CenterYLimitHigh;

            virtual double GetCenterYLimitHigh() = 0;

            virtual void SetCenterYLimitHigh(double value) = 0;

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
        /*  Note: Interface Classes of the result of the CircleFitResults.  */
        ///圆拟合结果类
        class CircleFitResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CircleFitResults() {}
            virtual ~CircleFitResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 输出圆
             */
            virtual Circle GetFitCircle() = 0;
            /**
            * @brief 输出圆环
            */
            virtual Annulus GetFitCircleAnnulus() = 0;
            /**
             * @brief 拟合误差
             */
            virtual float GetFitError() = 0;
            /**
             * @brief 拟合状态
             */
            virtual int GetFitStatus() = 0;
            /**
             * @brief 匹配点数
             */
            virtual int GetNumOfPointsUsed() = 0;
            /**
             * @brief 拟合点
             * @param nIndex [IN] 输入点索引，[0,InputPointNum)
             */
            virtual PointF GetOutputFittingPoint(int nIndex) = 0;
            /**
             * @brief 获取指定索引处的输入点的拟合距离
             * @param nIndex [IN] 输入点索引，[0,InputPointNum)
             */
            virtual float GetFitDist(int nIndex) = 0;
            /**
             * @brief 获取指定索引处的匹配点
             * @param nIndex [IN] 输入点索引，[0,InputPointNum)
             */
            virtual int GetFitUsedPoint(int nIndex) = 0;
            /**
             * @brief 拟合点数
             */
            virtual int GetFitPointNum() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CircleFit tool.  */
        ///圆拟合工具类
        class IMVSCircleFitModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCircleFitModuTool() {}
            virtual ~IMVSCircleFitModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CircleFitParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CircleFitResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSCircleFitModuTool(IMVSCircleFitModuTool&);
            IMVSCircleFitModuTool& operator=(IMVSCircleFitModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CircleFit.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSCircleFitModu::IMVSCircleFitModuTool * __stdcall GetCircleFitToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CIRCLEFIT_H_
