/***************************************************************************************************
 * File：IVmColorExtract.h
 * Note：@~chinese 颜色抽取模块接口声明 @~english Interface for the ColorExtract tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_COLOREXTRACT_H_
#define _IVM_COLOREXTRACT_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSColorExtractModu
    {
    /// \addtogroup 颜色抽取
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///颜色抽取ROI管理类
        class ColorExtractRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
            virtual void SetRoiAnnulus(Annulus stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the ColorExtractParams.  */
        ///颜色抽取参数类
        class ColorExtractParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ColorExtractParams() {}
            virtual ~ColorExtractParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual ColorExtractRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 颜色面积判断   @~english Color Total Area Check
            __declspec(property(put = SetAreaLimitEnable, get = GetAreaLimitEnable)) bool AreaLimitEnable;

            virtual bool GetAreaLimitEnable() = 0;

            virtual void SetAreaLimitEnable(bool value) = 0;

            /// @~chinese 面积范围，范围：[0,9999999]   @~english Area Range，range:[0,9999999]
            __declspec(property(put = SetAreaLimitLow, get = GetAreaLimitLow)) double AreaLimitLow;

            virtual double GetAreaLimitLow() = 0;

            virtual void SetAreaLimitLow(double value) = 0;

            /// @~chinese 面积范围，范围：[0,9999999]   @~english Area Range，range:[0,9999999]
            __declspec(property(put = SetAreaLimitHigh, get = GetAreaLimitHigh)) double AreaLimitHigh;

            virtual double GetAreaLimitHigh() = 0;

            virtual void SetAreaLimitHigh(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ColorExtractResults.  */
        ///颜色抽取结果类
        class ColorExtractResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ColorExtractResults() {}
            virtual ~ColorExtractResults() {}

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
             * @brief ROI圆弧
             */
             virtual Annulus GetROIAnnulus() = 0;
            /**
             * @brief 输出图像
             */
            virtual ImageBaseData GetOutputImage() = 0;
            /**
             * @brief 总面积
             */
            virtual float GetTotalArea() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ColorExtract tool.  */
        ///颜色抽取工具类
        class IMVSColorExtractModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSColorExtractModuTool() {}
            virtual ~IMVSColorExtractModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ColorExtractParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ColorExtractResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSColorExtractModuTool(IMVSColorExtractModuTool&);
            IMVSColorExtractModuTool& operator=(IMVSColorExtractModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ColorExtract.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSColorExtractModu::IMVSColorExtractModuTool * __stdcall GetColorExtractToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_COLOREXTRACT_H_
