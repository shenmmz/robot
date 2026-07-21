/***************************************************************************************************
 * File：IVmColorImageGeneration.h
 * Note：@~chinese 彩图生成模块接口声明 @~english Interface for the ColorImageGeneration tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_COLORIMAGEGENERATION_H_
#define _IVM_COLORIMAGEGENERATION_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSColorImageGenerationModu
    {
        /// @~chinese 输出图像格式   @~english Output Image Format
        typedef enum _ImageFormatEnum
        {
            /// RGB P3   RGB P3
            ImageFormat_RGBP3 = 200,

            /// RGB C3   RGB C3
            ImageFormat_RGBC3 = 201,

        }ImageFormatEnum;

    /// \addtogroup 彩图生成
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the ColorImageGenerationParams.  */
        ///彩图生成参数类
        class ColorImageGenerationParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ColorImageGenerationParams() {}
            virtual ~ColorImageGenerationParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入图像1(仅当次执行起效)   @~english InputImage1(Only valid for this execution)
            virtual void SetInputImage1(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入图像2(仅当次执行起效)   @~english InputImage2(Only valid for this execution)
            virtual void SetInputImage2(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输出图像格式   @~english Output Image Format
            __declspec(property(put = SetImageFormat, get = GetImageFormat)) ImageFormatEnum ImageFormat;

            virtual ImageFormatEnum GetImageFormat() = 0;

            virtual void SetImageFormat(ImageFormatEnum value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ColorImageGenerationResults.  */
        ///彩图生成结果类
        class ColorImageGenerationResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ColorImageGenerationResults() {}
            virtual ~ColorImageGenerationResults() {}

        public:
             /**
             * @brief 输出图像
             */
             virtual ImageBaseData GetOutputImage() = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;


        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ColorImageGeneration tool.  */
        ///彩图生成工具类
        class IMVSColorImageGenerationModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSColorImageGenerationModuTool() {}
            virtual ~IMVSColorImageGenerationModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ColorImageGenerationParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ColorImageGenerationResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSColorImageGenerationModuTool(IMVSColorImageGenerationModuTool&);
            IMVSColorImageGenerationModuTool& operator=(IMVSColorImageGenerationModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
#endif    ///< _IVM_COLORIMAGEGENERATION_H_
