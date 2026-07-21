/***************************************************************************************************
 * File：IVmEllipseFit.h
 * Note：@~chinese 椭圆拟合模块接口声明 @~english Interface for the EllipseFit tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_ELLIPSEFIT_H_
#define _IVM_ELLIPSEFIT_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSEllipseFitModu
    {
    /// \addtogroup 椭圆拟合
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the EllipseFitParams.  */
        ///椭圆拟合参数类
        class EllipseFitParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit EllipseFitParams() {}
            virtual ~EllipseFitParams() {}

        public:

            /// @~chinese 输入拟合点(仅当次执行起效)   @~english InputFittingPoint(Only valid for this execution)
            virtual void SetInputFittingPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 误差容忍度，范围：[1,200]   @~english ErrorTolerance，range:[1,200]
            __declspec(property(put = SetErrorTolerance, get = GetErrorTolerance)) int ErrorTolerance;

            virtual int GetErrorTolerance() = 0;

            virtual void SetErrorTolerance(int value) = 0;

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
        /*  Note: Interface Classes of the result of the EllipseFitResults.  */
        ///椭圆拟合结果类
        class EllipseFitResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit EllipseFitResults() {}
            virtual ~EllipseFitResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
			 /**
             * @brief 输出椭圆
			 */
			 virtual Ellipse GetFitEllipse() = 0;
			 /**
             * @brief 拟合误差
			 */
			 virtual float GetFitError() = 0;
			 /**
             * @brief 拟合状态
			 */
			 virtual int GetFitStatus() = 0;
			 /**
             * @brief 拟合点
			 * @param nIndex [IN] 输入点索引，[0,InputPointNum)
			 */
			 virtual PointF GetOutputFittingPoint(int nIndex) = 0;
			 /**
             * @brief 拟合点数
			 */
			 virtual int GetFitPointNum() = 0;
             /**
             * @brief 匹配点
             */
             virtual int GetFitUsedPoint(int nIndex) = 0;
             /**
             * @brief 匹配点数
             */
             virtual int GetFitUsedPointNum() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the EllipseFit tool.  */
        ///椭圆拟合工具类
        class IMVSEllipseFitModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSEllipseFitModuTool() {}
            virtual ~IMVSEllipseFitModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual EllipseFitParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual EllipseFitResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSEllipseFitModuTool(IMVSEllipseFitModuTool&);
            IMVSEllipseFitModuTool& operator=(IMVSEllipseFitModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for EllipseFit.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSEllipseFitModu::IMVSEllipseFitModuTool * __stdcall GetEllipseFitToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_ELLIPSEFIT_H_
