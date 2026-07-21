/***************************************************************************************************
 * File：IVmImageSharpness.h
 * Note：@~chinese 清晰度评估模块接口声明 @~english Interface for the ImageSharpness tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_IMAGESHARPNESS_H_
#define _IVM_IMAGESHARPNESS_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSImageSharpnessModu
    {
        /// @~chinese 评价模式   @~english Assess Mode
        typedef enum _SharpnessModeEnum
        {
            /// @~chinese 自相关   @~english Autocorrelation
            SharpnessMode_AutoCoreeMode = 0x1,

            /// @~chinese 梯度平方   @~english Gradient Square
            SharpnessMode_SquaredGrad = 0x2,

        }SharpnessModeEnum;

    /// \addtogroup 清晰度评估
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///清晰度评估ROI管理类
        class ImageSharpnessRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
            virtual void SetRoiAnnulus(Annulus stRoiValue) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

            /// @~chinese 掩膜图像（仅当次执行起效）   @~english Mask Image(Only valid for this execution)
            virtual void SetMaskImage(ImageBaseData stMaskImage) = 0;

        };

        /*  Note: Interface Classes of the parameter of the ImageSharpnessParams.  */
        ///清晰度评估参数类
        class ImageSharpnessParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageSharpnessParams() {}
            virtual ~ImageSharpnessParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual ImageSharpnessRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 评价模式   @~english Assess Mode
            __declspec(property(put = SetSharpnessMode, get = GetSharpnessMode)) SharpnessModeEnum SharpnessMode;

            virtual SharpnessModeEnum GetSharpnessMode() = 0;

            virtual void SetSharpnessMode(SharpnessModeEnum value) = 0;

            /// @~chinese 噪声等级，范围：[0,32]   @~english Noise Level，range:[0,32]
            __declspec(property(put = SetNoiseLevel, get = GetNoiseLevel)) int NoiseLevel;

            virtual int GetNoiseLevel() = 0;

            virtual void SetNoiseLevel(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ImageSharpnessResults.  */
        ///清晰度评估结果类
        class ImageSharpnessResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageSharpnessResults() {}
            virtual ~ImageSharpnessResults() {}

        public:
             /**
             * @brief 屏蔽区字符串
             */
             virtual StringData* GetBlindPolygonString(unsigned int& nValueNum) = 0;

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
             * @brief ROI圆弧
             */
             virtual Annulus GetROIAnnulus() = 0;
            /**
             * @brief 图像清晰度分数 
             */
            virtual float GetSharpness() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ImageSharpness tool.  */
        ///清晰度评估工具类
        class IMVSImageSharpnessModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSImageSharpnessModuTool() {}
            virtual ~IMVSImageSharpnessModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ImageSharpnessParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ImageSharpnessResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSImageSharpnessModuTool(IMVSImageSharpnessModuTool&);
            IMVSImageSharpnessModuTool& operator=(IMVSImageSharpnessModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ImageSharpness.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSImageSharpnessModu::IMVSImageSharpnessModuTool * __stdcall GetImageSharpnessToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_IMAGESHARPNESS_H_
