/***************************************************************************************************
 * File：IVmLineFit.h
 * Note：@~chinese 直线拟合模块接口声明 @~english Interface for the LineFit tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_LINEFIT_H_
#define _IVM_LINEFIT_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSLineFitModu
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

    /// \addtogroup 直线拟合
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the LineFitParams.  */
        ///直线拟合参数类
        class LineFitParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit LineFitParams() {}
            virtual ~LineFitParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 拟合点(仅当次执行起效)   @~english Fit Point(Only valid for this execution)
            virtual void SetFitPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 剔除点数，范围：[0,100]   @~english Reject Number，range:[0,100]
            __declspec(property(put = SetRejectNum, get = GetRejectNum)) int RejectNum;

            virtual int GetRejectNum() = 0;

            virtual void SetRejectNum(int value) = 0;

            /// @~chinese 剔除距离，范围：[1,1000]   @~english Distance to Remove，range:[1,1000]
            __declspec(property(put = SetRejectDist, get = GetRejectDist)) int RejectDist;

            virtual int GetRejectDist() = 0;

            virtual void SetRejectDist(int value) = 0;

            /// @~chinese 角度使能   @~english Line Angle Enable
            __declspec(property(put = SetLineAngleEnable, get = GetLineAngleEnable)) bool LineAngleEnable;

            virtual bool GetLineAngleEnable() = 0;

            virtual void SetLineAngleEnable(bool value) = 0;

            /// @~chinese 期望直线角度，范围：[-180,180]   @~english Expect Line Angle，range:[-180,180]
            __declspec(property(put = SetExpectLineAngle, get = GetExpectLineAngle)) double ExpectLineAngle;

            virtual double GetExpectLineAngle() = 0;

            virtual void SetExpectLineAngle(double value) = 0;

            /// @~chinese 角度旋转容忍，范围：[0,180]   @~english Rotate Tolerance，range:[0,180]
            __declspec(property(put = SetRotateTolerance, get = GetRotateTolerance)) double RotateTolerance;

            virtual double GetRotateTolerance() = 0;

            virtual void SetRotateTolerance(double value) = 0;

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

            /// @~chinese 角度范围，范围：[-90,90]   @~english Angle Range，range:[-90,90]
            __declspec(property(put = SetAngleLimitLow, get = GetAngleLimitLow)) double AngleLimitLow;

            virtual double GetAngleLimitLow() = 0;

            virtual void SetAngleLimitLow(double value) = 0;

            /// @~chinese 角度范围，范围：[-90,90]   @~english Angle Range，range:[-90,90]
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
        /*  Note: Interface Classes of the result of the LineFitResults.  */
        ///直线拟合结果类
        class LineFitResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit LineFitResults() {}
            virtual ~LineFitResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 输出直线
             */
            virtual Line GetFitLine() = 0;
            /**
             * @brief 直线角度
             */
             virtual float GetLineAngle() = 0;
            /**
             * @brief 拟合误差
             */
            virtual float GetFitError() = 0;
            /**
             * @brief 拟合状态
             */
            virtual int GetFitStatus() = 0;
            /**
             * @brief 拟合时使用点个数
            */
            virtual int GetFitPointNum() = 0;
            /**
             * @brief 匹配点数
             */
            virtual int GetNumOfPointsUsed() = 0;
            /**
             * @brief 获取指定索引处的输入点是否用于拟合;0:未使用 1:使用
             * @param nIndex [IN] 输入点索引，[0,InputPointNum)
             */
            virtual int GetPointStatus(int nIndex) = 0;
            /**
             * @brief 获取指定索引处的输入点的拟合距离
             * @param nIndex [IN] 输入点索引，[0,InputPointNum)
             */
            virtual float GetPointFitDistance(int nIndex) = 0;
            /**
             * @brief 输出拟合点
             * @param nIndex [IN] 输入点索引，[0,InputPointNum)
             */
            virtual PointF GetOutputFittingPoint(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the LineFit tool.  */
        ///直线拟合工具类
        class IMVSLineFitModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSLineFitModuTool() {}
            virtual ~IMVSLineFitModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual LineFitParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual LineFitResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSLineFitModuTool(IMVSLineFitModuTool&);
            IMVSLineFitModuTool& operator=(IMVSLineFitModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for LineFit.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSLineFitModu::IMVSLineFitModuTool * __stdcall GetLineFitToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_LINEFIT_H_
