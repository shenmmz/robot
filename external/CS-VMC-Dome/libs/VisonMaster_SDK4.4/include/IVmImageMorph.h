/***************************************************************************************************
 * File：IVmImageMorph.h
 * Note：@~chinese 形态学处理模块接口声明 @~english Interface for the ImageMorph tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_IMAGEMORPH_H_
#define _IVM_IMAGEMORPH_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSImageMorphModu
    {
        /// @~chinese 形态学类型   @~english Morphology Type
        typedef enum _MorphTypeEnum
        {
            /// @~chinese 膨胀   @~english Dilation
            MorphType_Dilate = 0x1,

            /// @~chinese 腐蚀   @~english Erosion
            MorphType_Erode = 0x2,

            /// @~chinese 开   @~english Opening
            MorphType_Open = 0x3,

            /// @~chinese 闭   @~english Closing
            MorphType_Close = 0x4,

        }MorphTypeEnum;

        /// @~chinese 形态学形状   @~english Structuring Element
        typedef enum _MorphShapeEnum
        {
            /// @~chinese 矩形   @~english Box
            MorphShape_Rectange = 0x0,

            /// @~chinese 椭圆   @~english Ellipse
            MorphShape_Ellipse = 0x1,

            /// @~chinese 十字   @~english Cross
            MorphShape_Crosss = 0x2,

        }MorphShapeEnum;

    /// \addtogroup 形态学处理
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///形态学处理ROI管理类
        class ImageMorphRoiManager
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

        /*  Note: Interface Classes of the parameter of the ImageMorphParams.  */
        ///形态学处理参数类
        class ImageMorphParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageMorphParams() {}
            virtual ~ImageMorphParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual ImageMorphRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 形态学类型   @~english Morphology Type
            __declspec(property(put = SetMorphType, get = GetMorphType)) MorphTypeEnum MorphType;

            virtual MorphTypeEnum GetMorphType() = 0;

            virtual void SetMorphType(MorphTypeEnum value) = 0;

            /// @~chinese 形态学形状   @~english Structuring Element
            __declspec(property(put = SetMorphShape, get = GetMorphShape)) MorphShapeEnum MorphShape;

            virtual MorphShapeEnum GetMorphShape() = 0;

            virtual void SetMorphShape(MorphShapeEnum value) = 0;

            /// @~chinese 迭代次数，范围：[0,10]   @~english Iteration Times，range:[0,10]
            __declspec(property(put = SetMorphIterNum, get = GetMorphIterNum)) int MorphIterNum;

            virtual int GetMorphIterNum() = 0;

            virtual void SetMorphIterNum(int value) = 0;

            /// @~chinese 核宽度，范围：[1,51]   @~english Element Width，range:[1,51]
            __declspec(property(put = SetKernelWidth, get = GetKernelWidth)) int KernelWidth;

            virtual int GetKernelWidth() = 0;

            virtual void SetKernelWidth(int value) = 0;

            /// @~chinese 核高度，范围：[1,51]   @~english Element Height，range:[1,51]
            __declspec(property(put = SetKernelHeight, get = GetKernelHeight)) int KernelHeight;

            virtual int GetKernelHeight() = 0;

            virtual void SetKernelHeight(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ImageMorphResults.  */
        ///形态学处理结果类
        class ImageMorphResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageMorphResults() {}
            virtual ~ImageMorphResults() {}

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
             * @brief 输出图像
             */
            virtual ImageBaseData GetOutputImage() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ImageMorph tool.  */
        ///形态学处理工具类
        class IMVSImageMorphModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSImageMorphModuTool() {}
            virtual ~IMVSImageMorphModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ImageMorphParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ImageMorphResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSImageMorphModuTool(IMVSImageMorphModuTool&);
            IMVSImageMorphModuTool& operator=(IMVSImageMorphModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ImageMorph.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSImageMorphModu::IMVSImageMorphModuTool * __stdcall GetImageMorphToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_IMAGEMORPH_H_
