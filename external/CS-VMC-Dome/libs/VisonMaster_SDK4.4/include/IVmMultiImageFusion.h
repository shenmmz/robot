/***************************************************************************************************
 * File：IVmMultiImageFusion.h
 * Note：@~chinese 多图融合模块接口声明 @~english Interface for the MultiImageFusion tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_MULTIIMAGEFUSION_H_
#define _IVM_MULTIIMAGEFUSION_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSMultiImageFusionModu
    {
        /// @~chinese 输出图像类型   @~english Output Image Type
        typedef enum _ResultTypeEnum
        {
            /// @~chinese 反射   @~english Reflection
            ResultType_Albedo = 0x1,

            /// @~chinese 阴影   @~english Shadow
            ResultType_Shade = 0x2,

            /// @~chinese 全部   @~english All
            ResultType_All = 0x3,

        }ResultTypeEnum;

        /// @~chinese 方向增强类型   @~english Direction Enhancement Type
        typedef enum _DirEnhanceTypeEnum
        {
            /// @~chinese 无   @~english NoMirror
            DirEnhanceType_None = 0x0,

            /// @~chinese X方向   @~english X-direction
            DirEnhanceType_XDirection = 0x1,

            /// @~chinese Y方向   @~english Y-direction
            DirEnhanceType_YDirection = 0x2,

        }DirEnhanceTypeEnum;

    /// \addtogroup 多图融合
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the MultiImageFusionParams.  */
        ///多图融合参数类
        class MultiImageFusionParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MultiImageFusionParams() {}
            virtual ~MultiImageFusionParams() {}

        public:

            /// @~chinese 图像数量（仅当次执行起效） @~english Image Quantity(Only valid for this execution)
            virtual void SetImageCount(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入图像0(仅当次执行起效)   @~english InImage0(Only valid for this execution)
            virtual void SetInImage0(ImageBaseData* pInputValue) = 0;

            /// @~chinese 分布角0(仅当次执行起效)   @~english Distribution0(Only valid for this execution)
            virtual void SetInImage0Distribution(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 照射角0(仅当次执行起效)   @~english Irradiation0(Only valid for this execution)
            virtual void SetInImage0Irradiation(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入图像1(仅当次执行起效)   @~english InImage1(Only valid for this execution)
            virtual void SetInImage1(ImageBaseData* pInputValue) = 0;

            /// @~chinese 分布角1(仅当次执行起效)   @~english Distribution1(Only valid for this execution)
            virtual void SetInImage1Distribution(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 照射角1(仅当次执行起效)   @~english Irradiation1(Only valid for this execution)
            virtual void SetInImage1Irradiation(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入图像2(仅当次执行起效)   @~english InImage2(Only valid for this execution)
            virtual void SetInImage2(ImageBaseData* pInputValue) = 0;

            /// @~chinese 分布角2(仅当次执行起效)   @~english Distribution2(Only valid for this execution)
            virtual void SetInImage2Distribution(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 照射角2(仅当次执行起效)   @~english Irradiation2(Only valid for this execution)
            virtual void SetInImage2Irradiation(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入图像3(仅当次执行起效)   @~english InImage3(Only valid for this execution)
            virtual void SetInImage3(ImageBaseData* pInputValue) = 0;

            /// @~chinese 分布角3(仅当次执行起效)   @~english Distribution3(Only valid for this execution)
            virtual void SetInImage3Distribution(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 照射角3(仅当次执行起效)   @~english Irradiation3(Only valid for this execution)
            virtual void SetInImage3Irradiation(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入图像4(仅当次执行起效)   @~english InImage4(Only valid for this execution)
            virtual void SetInImage4(ImageBaseData* pInputValue) = 0;

            /// @~chinese 分布角4(仅当次执行起效)   @~english Distribution4(Only valid for this execution)
            virtual void SetInImage4Distribution(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 照射角4(仅当次执行起效)   @~english Irradiation4(Only valid for this execution)
            virtual void SetInImage4Irradiation(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入图像5(仅当次执行起效)   @~english InImage5(Only valid for this execution)
            virtual void SetInImage5(ImageBaseData* pInputValue) = 0;

            /// @~chinese 分布角5(仅当次执行起效)   @~english Distribution5(Only valid for this execution)
            virtual void SetInImage5Distribution(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 照射角5(仅当次执行起效)   @~english Irradiation5(Only valid for this execution)
            virtual void SetInImage5Irradiation(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入图像6(仅当次执行起效)   @~english InImage6(Only valid for this execution)
            virtual void SetInImage6(ImageBaseData* pInputValue) = 0;

            /// @~chinese 分布角6(仅当次执行起效)   @~english Distribution6(Only valid for this execution)
            virtual void SetInImage6Distribution(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 照射角6(仅当次执行起效)   @~english Irradiation6(Only valid for this execution)
            virtual void SetInImage6Irradiation(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入图像7(仅当次执行起效)   @~english InImage7(Only valid for this execution)
            virtual void SetInImage7(ImageBaseData* pInputValue) = 0;

            /// @~chinese 分布角7(仅当次执行起效)   @~english Distribution7(Only valid for this execution)
            virtual void SetInImage7Distribution(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 照射角7(仅当次执行起效)   @~english Irradiation7(Only valid for this execution)
            virtual void SetInImage7Irradiation(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输出图像类型   @~english Output Image Type
            __declspec(property(put = SetResultType, get = GetResultType)) ResultTypeEnum ResultType;

            virtual ResultTypeEnum GetResultType() = 0;

            virtual void SetResultType(ResultTypeEnum value) = 0;

            /// @~chinese 滤波尺寸，范围：[1,50]   @~english Filter Size，range:[1,50]
            __declspec(property(put = SetKernelSize, get = GetKernelSize)) int KernelSize;

            virtual int GetKernelSize() = 0;

            virtual void SetKernelSize(int value) = 0;

            /// @~chinese 增强使能   @~english Enhancement Enable
            __declspec(property(put = SetEnhanceEnable, get = GetEnhanceEnable)) bool EnhanceEnable;

            virtual bool GetEnhanceEnable() = 0;

            virtual void SetEnhanceEnable(bool value) = 0;

            /// @~chinese 背景亮度，范围：[0,255]   @~english Background Brightness，range:[0,255]
            __declspec(property(put = SetBrightness, get = GetBrightness)) int Brightness;

            virtual int GetBrightness() = 0;

            virtual void SetBrightness(int value) = 0;

            /// @~chinese 对比系数，范围：[1,1000]   @~english Contrast Coefficient，range:[1,1000]
            __declspec(property(put = SetContrastCoef, get = GetContrastCoef)) int ContrastCoef;

            virtual int GetContrastCoef() = 0;

            virtual void SetContrastCoef(int value) = 0;

            /// @~chinese 光晕去除等级，范围：[0,8]   @~english Halo Removal Grade，range:[0,8]
            __declspec(property(put = SetHalationRemoveLevel, get = GetHalationRemoveLevel)) int HalationRemoveLevel;

            virtual int GetHalationRemoveLevel() = 0;

            virtual void SetHalationRemoveLevel(int value) = 0;

            /// @~chinese 方向增强类型   @~english Direction Enhancement Type
            __declspec(property(put = SetDirEnhanceType, get = GetDirEnhanceType)) DirEnhanceTypeEnum DirEnhanceType;

            virtual DirEnhanceTypeEnum GetDirEnhanceType() = 0;

            virtual void SetDirEnhanceType(DirEnhanceTypeEnum value) = 0;

            /// @~chinese 方向增强等级，范围：[1,255]   @~english Direction Enhancement Grade，range:[1,255]
            __declspec(property(put = SetDirEnhanceLevel, get = GetDirEnhanceLevel)) int DirEnhanceLevel;

            virtual int GetDirEnhanceLevel() = 0;

            virtual void SetDirEnhanceLevel(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the MultiImageFusionResults.  */
        ///多图融合结果类
        class MultiImageFusionResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MultiImageFusionResults() {}
            virtual ~MultiImageFusionResults() {}

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
             * @brief 获取图像数
             * @return 图像数
             */
            virtual int GetImageNum() = 0;
            /**
            * @brief 图像宽度
            */
            virtual int GetImageWidth() = 0;
            /**
            * @brief 图像高度
            */
            virtual int GetImageHeight() = 0;

            /**
             * @brief 获取反射图像
             * @return 反射图像
             */
            virtual ImageBaseData GetAlbedoImageCom() = 0;

            /**
             * @brief 获取阴影图像
             * @return 阴影图像
             */
            virtual ImageBaseData GetShadingImageCom() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the MultiImageFusion tool.  */
        ///多图融合工具类
        class IMVSMultiImageFusionModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSMultiImageFusionModuTool() {}
            virtual ~IMVSMultiImageFusionModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual MultiImageFusionParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual MultiImageFusionResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSMultiImageFusionModuTool(IMVSMultiImageFusionModuTool&);
            IMVSMultiImageFusionModuTool& operator=(IMVSMultiImageFusionModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for MultiImageFusion.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSMultiImageFusionModu::IMVSMultiImageFusionModuTool * __stdcall GetMultiImageFusionToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_MULTIIMAGEFUSION_H_
