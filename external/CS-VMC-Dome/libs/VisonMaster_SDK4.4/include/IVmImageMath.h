/***************************************************************************************************
 * File：IVmImageMath.h
 * Note：@~chinese 图像运算模块接口声明 @~english Interface for the ImageMath tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_IMAGEMATH_H_
#define _IVM_IMAGEMATH_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSImageMathModu
    {
        /// @~chinese 运算类型   @~english Calc Type
        typedef enum _ArithmeticTypeEnum
        {
            /// @~chinese 图像加   @~english Addition
            ArithmeticType_Add = 0x1,

            /// @~chinese 图像减   @~english Substraction
            ArithmeticType_Sub = 0x2,

            /// @~chinese 图像绝对差   @~english Absolute Difference
            ArithmeticType_AbsDiff = 0x3,

            /// @~chinese 两者最大值   @~english Max Value
            ArithmeticType_Max = 0x4,

            /// @~chinese 两者最小值   @~english Min Value
            ArithmeticType_Min = 0x5,

            /// @~chinese 两者均值   @~english Mean Value
            ArithmeticType_Average = 0x6,

            /// @~chinese 图像与   @~english AND
            ArithmeticType_And = 0x7,

            /// @~chinese 图像或   @~english OR
            ArithmeticType_Or = 0x8,

            /// @~chinese 图像异或   @~english XOR
            ArithmeticType_Xor = 0x9,

            /// @~chinese k1*I1+k2*I2+C   @~english RunParam_(k1*I1+k2*I2+c)
            ArithmeticType_BiImgLinear = 0xA,

            /// @~chinese k1*I1+C   @~english RunParam_(k1*I1+c)
            ArithmeticType_SingleImgLinear = 0xB,

            /// @~chinese I1+C   @~english RunParam_I1+C
            ArithmeticType_ADDC = 0xC,

            /// @~chinese I1-C   @~english RunParam_I1-C
            ArithmeticType_SUBC = 0xD,

            /// @~chinese C-I1   @~english RunParam_C-I1
            ArithmeticType_CSUB = 0xE,

            /// @~chinese 两幅图像乘   @~english RunParam_Image1_multiply_Image2
            ArithmeticType_MUL = 0xF,

            /// @~chinese 两幅图像除   @~english RunParam_Image1_divide_Image2
            ArithmeticType_DIV = 0x10,

            /// @~chinese I1/C   @~english RunParam_I1/C
            ArithmeticType_DIVC = 0x11,

            /// @~chinese C/I1   @~english RunParam_C/I1
            ArithmeticType_CDIV = 0x12,

            /// @~chinese I1&&C   @~english RunParam_Image1 and C
            ArithmeticType_ANDC = 0x13,

            /// @~chinese I1||C   @~english RunParam_Image1 or C
            ArithmeticType_ORC = 0x14,

            /// @~chinese I1^C   @~english RunParam_Image1 XOR C
            ArithmeticType_XORC = 0x15,

            /// @~chinese 图像和常数最大值   @~english RunParam_Image and constant maxima
            ArithmeticType_MAXC = 0x16,

            /// @~chinese 图像和常数最小值   @~english RunParam_Image and constant minima
            ArithmeticType_MINC = 0x17,

            /// @~chinese 图像和常数均值   @~english RunParam_Image and constant mean
            ArithmeticType_AVGC = 0x18,

            /// @~chinese |I1-C|   @~english RunParam_Image1 subtract C absolute
            ArithmeticType_ABS_DIFFC = 0x19,

            /// @~chinese 两幅图像与非   @~english RunParam_Two images and non
            ArithmeticType_ANDNOT = 0x1A,

            /// @~chinese 两幅图像或非   @~english RunParam_Two images or not
            ArithmeticType_ORNOT = 0x1B,

            /// @~chinese !(I1&&C)   @~english RunParam_not Image1 and C
            ArithmeticType_ANDNOTC = 0x1C,

            /// @~chinese !(I1||C)   @~english RunParam_not Image1 or C
            ArithmeticType_ORNOTC = 0x1D,

            /// @~chinese (I1-(I2 + C))>=0?255:0   @~english RunParam_SUB_GREATER_EQ_BIN
            ArithmeticType_SUB_GREATER_EQ_BIN = 0x1E,

            /// @~chinese (I1-(I2 + C))>=0?0:255   @~english RunParam_SUB_LESS_EQ_BIN
            ArithmeticType_SUB_LESS_EQ_BIN = 0x1F,

        }ArithmeticTypeEnum;

    /// \addtogroup 图像运算
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///图像运算ROI管理类
        class ImageMathRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the ImageMathParams.  */
        ///图像运算参数类
        class ImageMathParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageMathParams() {}
            virtual ~ImageMathParams() {}

        public:

            /// @~chinese 输入图像1(仅当次执行起效)   @~english InputImage1(Only valid for this execution)
            virtual void SetInputImage1(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入图像2(仅当次执行起效)   @~english InputImage2(Only valid for this execution)
            virtual void SetInputImage2(ImageBaseData* pInputValue) = 0;

            /// @~chinese 图像权重1，范围：[0,255]   @~english Image K1，range:[0,255]
            __declspec(property(put = SetImageWeight1, get = GetImageWeight1)) double ImageWeight1;

            virtual double GetImageWeight1() = 0;

            virtual void SetImageWeight1(double value) = 0;

            /// @~chinese 图像补偿1，范围：[-255,255]   @~english Image Offset 1，range:[-255,255]
            __declspec(property(put = SetImageOffset1, get = GetImageOffset1)) double ImageOffset1;

            virtual double GetImageOffset1() = 0;

            virtual void SetImageOffset1(double value) = 0;

            /// @~chinese 图像权重2，范围：[0,255]   @~english Image K2，range:[0,255]
            __declspec(property(put = SetImageWeight2, get = GetImageWeight2)) double ImageWeight2;

            virtual double GetImageWeight2() = 0;

            virtual void SetImageWeight2(double value) = 0;

            /// @~chinese 图像补偿2，范围：[-255,255]   @~english Image Offset 2，range:[-255,255]
            __declspec(property(put = SetImageOffset2, get = GetImageOffset2)) double ImageOffset2;

            virtual double GetImageOffset2() = 0;

            virtual void SetImageOffset2(double value) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual ImageMathRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 运算类型   @~english Calc Type
            __declspec(property(put = SetArithmeticType, get = GetArithmeticType)) ArithmeticTypeEnum ArithmeticType;

            virtual ArithmeticTypeEnum GetArithmeticType() = 0;

            virtual void SetArithmeticType(ArithmeticTypeEnum value) = 0;

            /// @~chinese 运算权重1，范围：[0,255]   @~english ImageK 1，range:[0,255]
            __declspec(property(put = SetImageK1, get = GetImageK1)) double ImageK1;

            virtual double GetImageK1() = 0;

            virtual void SetImageK1(double value) = 0;

            /// @~chinese 运算权重2，范围：[0,255]   @~english ImageK 2，range:[0,255]
            __declspec(property(put = SetImageK2, get = GetImageK2)) double ImageK2;

            virtual double GetImageK2() = 0;

            virtual void SetImageK2(double value) = 0;

            /// @~chinese 运算补偿，范围：[-255,255]   @~english Image Offset，range:[-255,255]
            __declspec(property(put = SetImageC, get = GetImageC)) double ImageC;

            virtual double GetImageC() = 0;

            virtual void SetImageC(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ImageMathResults.  */
        ///图像运算结果类
        class ImageMathResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageMathResults() {}
            virtual ~ImageMathResults() {}

        public:
             /**
             * @brief ROI圆弧
             */
             virtual Annulus GetROIAnnulus() = 0;

             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 输出图像
             */
            virtual ImageBaseData GetOutputImage() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ImageMath tool.  */
        ///图像运算工具类
        class IMVSImageMathModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSImageMathModuTool() {}
            virtual ~IMVSImageMathModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ImageMathParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ImageMathResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSImageMathModuTool(IMVSImageMathModuTool&);
            IMVSImageMathModuTool& operator=(IMVSImageMathModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ImageMath.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSImageMathModu::IMVSImageMathModuTool * __stdcall GetImageMathToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_IMAGEMATH_H_
