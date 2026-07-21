/***************************************************************************************************
 * File：IVmImageCorrectCalib.h
 * Note：@~chinese 畸变校正模块接口声明 @~english Interface for the ImageCorrectCalib tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_IMAGECORRECTCALIB_H_
#define _IVM_IMAGECORRECTCALIB_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSImageCorrectCalibModu
    {
        /// @~chinese 校正模式   @~english Correction Mode
        typedef enum _CorrectModelEnum
        {
            /// @~chinese 透视畸变校正   @~english Perspective Distortion Correction
            CorrectModel_CorrectModelLinear = 0x1,

            /// @~chinese 径向畸变校正   @~english Radial Distortion Correction
            CorrectModel_CorrectModelRadial = 0x2,

            /// @~chinese 径向透视畸变校正   @~english Radial Perspective Distortion Correction
            CorrectModel_CorrectModelPersRadial = 0x3,

        }CorrectModelEnum;

    /// \addtogroup 畸变校正
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the ImageCorrectCalibParams.  */
        ///畸变校正参数类
        class ImageCorrectCalibParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageCorrectCalibParams() {}
            virtual ~ImageCorrectCalibParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 刷新信号(仅当次执行起效)   @~english Refresh Signal(Only valid for this execution)
            virtual void SetRefreshSignal(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 标定文件路径   @~english Set Calibration File Path
            __declspec(property(put = SetCalibPath, get = GetCalibPath)) const char* CalibPath;

            virtual const char* GetCalibPath() = 0;

            virtual void SetCalibPath(const char* value) = 0;

            /// @~chinese 校正模式   @~english Correction Mode
            __declspec(property(put = SetCorrectModel, get = GetCorrectModel)) CorrectModelEnum CorrectModel;

            virtual CorrectModelEnum GetCorrectModel() = 0;

            virtual void SetCorrectModel(CorrectModelEnum value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ImageCorrectCalibResults.  */
        ///畸变校正结果类
        class ImageCorrectCalibResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageCorrectCalibResults() {}
            virtual ~ImageCorrectCalibResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 获取畸变矫正图像
             * @return 畸变矫正图像
             */
            virtual ImageBaseData GetOutputImage() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ImageCorrectCalib tool.  */
        ///畸变校正工具类
        class IMVSImageCorrectCalibModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSImageCorrectCalibModuTool() {}
            virtual ~IMVSImageCorrectCalibModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ImageCorrectCalibParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ImageCorrectCalibResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSImageCorrectCalibModuTool(IMVSImageCorrectCalibModuTool&);
            IMVSImageCorrectCalibModuTool& operator=(IMVSImageCorrectCalibModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ImageCorrectCalib.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSImageCorrectCalibModu::IMVSImageCorrectCalibModuTool * __stdcall GetImageCorrectCalibToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_IMAGECORRECTCALIB_H_
