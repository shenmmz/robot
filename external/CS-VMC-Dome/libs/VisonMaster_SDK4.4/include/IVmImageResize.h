/***************************************************************************************************
 * File：IVmImageResize.h
 * Note：@~chinese 图像缩放模块接口声明 @~english Interface for the ImageResize tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_IMAGERESIZE_H_
#define _IVM_IMAGERESIZE_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSImageResizeModu
    {
        /// @~chinese 插值类型   @~english Inter_Mode
        typedef enum _InterTypeEnum
        {
            /// @~chinese 最近邻   @~english NN
            InterType_Neareast = 0x1,

            /// @~chinese 双线性   @~english Bilinear
            InterType_Bilinear = 0x2,

            /// @~chinese 双三次   @~english RunParam_Cubic
            InterType_Cubic = 0x4,

        }InterTypeEnum;

    /// \addtogroup 图像缩放
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///图像缩放ROI管理类
        class ImageResizeRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the ImageResizeParams.  */
        ///图像缩放参数类
        class ImageResizeParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageResizeParams() {}
            virtual ~ImageResizeParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual ImageResizeRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 输出图像宽度，范围：[8,65000]   @~english OutImageWidth，range:[8,65000]
            __declspec(property(put = SetWidthValue, get = GetWidthValue)) int WidthValue;

            virtual int GetWidthValue() = 0;

            virtual void SetWidthValue(int value) = 0;

            /// @~chinese 输出图像高度，范围：[8,65000]   @~english OutImageHeight，range:[8,65000]
            __declspec(property(put = SetHeightValue, get = GetHeightValue)) int HeightValue;

            virtual int GetHeightValue() = 0;

            virtual void SetHeightValue(int value) = 0;

            /// @~chinese 插值类型   @~english Inter_Mode
            __declspec(property(put = SetInterType, get = GetInterType)) InterTypeEnum InterType;

            virtual InterTypeEnum GetInterType() = 0;

            virtual void SetInterType(InterTypeEnum value) = 0;

            /// @~chinese 抗混叠   @~english RunParam_AntiAliasing
            __declspec(property(put = SetAntiAliasing, get = GetAntiAliasing)) bool AntiAliasing;

            virtual bool GetAntiAliasing() = 0;

            virtual void SetAntiAliasing(bool value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ImageResizeResults.  */
        ///图像缩放结果类
        class ImageResizeResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageResizeResults() {}
            virtual ~ImageResizeResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

            /**
            * @brief 输出图像
            */
            virtual ImageBaseData GetOutputImage() = 0;
            /**
            * @brief 检测区域
            */
            virtual RectBox GetROI() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ImageResize tool.  */
        ///图像缩放工具类
        class IMVSImageResizeModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSImageResizeModuTool() {}
            virtual ~IMVSImageResizeModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ImageResizeParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ImageResizeResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSImageResizeModuTool(IMVSImageResizeModuTool&);
            IMVSImageResizeModuTool& operator=(IMVSImageResizeModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ImageResize.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSImageResizeModu::IMVSImageResizeModuTool * __stdcall GetImageResizeToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_IMAGERESIZE_H_
