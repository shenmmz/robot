/***************************************************************************************************
 * File：IVmImgStitchCalib.h
 * Note：@~chinese 图像拼接模块接口声明 @~english Interface for the ImgStitchCalib tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_IMGSTITCHCALIB_H_
#define _IVM_IMGSTITCHCALIB_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSImgStitchCalibModu
    {
        /// @~chinese 输入方式   @~english Input Mode
        typedef enum _InputTypeEnum
        {
            /// @~chinese 单来源   @~english Single
            InputType_SingleSource = 0x1,

            /// @~chinese 多来源   @~english Multiple
            InputType_MultipleSource = 0x2,

        }InputTypeEnum;

        /// @~chinese 拼接类型   @~english Stitch Type
        typedef enum _StitchTypeEnum
        {
            /// @~chinese 模型拼接   @~english Model Stitch
            StitchType_ModelStitch = 0x1,

            /// @~chinese 硬拼接   @~english Hard Stitch
            StitchType_HardStitch = 0x2,

        }StitchTypeEnum;

        /// @~chinese 拼接方向   @~english Stitch Direction
        typedef enum _StitchDirectionEnum
        {
            /// @~chinese X方向   @~english X-direction
            StitchDirection_XDirection = 0x1,

            /// @~chinese Y方向   @~english Y-direction
            StitchDirection_YDirection = 0x2,

        }StitchDirectionEnum;

        /// @~chinese 融合模式   @~english Stitch Merge Type
        typedef enum _MergeTypeEnum
        {
            /// @~chinese 均值融合   @~english Merge Mean
            MergeType_MergeMean = 0x1,

            /// @~chinese 最小值融合   @~english Merge Min.
            MergeType_MergeMin = 0x2,

            /// @~chinese 最大值融合   @~english Merge Max.
            MergeType_MergeMax = 0x3,

            /// @~chinese 接尾融合   @~english Merge Uncover
            MergeType_MergeUncover = 0x4,

        }MergeTypeEnum;

    /// \addtogroup 图像拼接
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the ImgStitchCalibParams.  */
        ///图像拼接参数类
        class ImgStitchCalibParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImgStitchCalibParams() {}
            virtual ~ImgStitchCalibParams() {}

        public:

            /// @~chinese 输入图像0(仅当次执行起效)   @~english InputImage0(Only valid for this execution)
            virtual void SetInputImage0(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入方式   @~english Input Mode
            __declspec(property(put = SetInputType, get = GetInputType)) InputTypeEnum InputType;

            virtual InputTypeEnum GetInputType() = 0;

            virtual void SetInputType(InputTypeEnum value) = 0;

            /// @~chinese 拼接类型   @~english Stitch Type
            __declspec(property(put = SetStitchType, get = GetStitchType)) StitchTypeEnum StitchType;

            virtual StitchTypeEnum GetStitchType() = 0;

            virtual void SetStitchType(StitchTypeEnum value) = 0;

            /// @~chinese 拼接数目，范围：[2,100]   @~english Stitch Number，range:[2,100]
            __declspec(property(put = SetImageNum, get = GetImageNum)) int ImageNum;

            virtual int GetImageNum() = 0;

            virtual void SetImageNum(int value) = 0;

            /// @~chinese 拼接数目，范围：[2,20]   @~english Stitch Number，range:[2,20]
            __declspec(property(put = SetHardImageNum, get = GetHardImageNum)) int HardImageNum;

            virtual int GetHardImageNum() = 0;

            virtual void SetHardImageNum(int value) = 0;

            /// @~chinese 拼接方式，设置一个格式为mXn的字符串，m为行数，n为列数   @~english Stitch Method, set an "mXn" format string, m is row number, n is colomn number
            __declspec(property(put = SetImageMatrix, get = GetImageMatrix)) const char* ImageMatrix;

            virtual const char* GetImageMatrix() = 0;

            virtual void SetImageMatrix(const char* value) = 0;

            /// @~chinese 拼接方向   @~english Stitch Direction
            __declspec(property(put = SetStitchDirection, get = GetStitchDirection)) StitchDirectionEnum StitchDirection;

            virtual StitchDirectionEnum GetStitchDirection() = 0;

            virtual void SetStitchDirection(StitchDirectionEnum value) = 0;

            /// @~chinese 融合模式   @~english Stitch Merge Type
            __declspec(property(put = SetMergeType, get = GetMergeType)) MergeTypeEnum MergeType;

            virtual MergeTypeEnum GetMergeType() = 0;

            virtual void SetMergeType(MergeTypeEnum value) = 0;

            /// @~chinese 裁剪参数，范围：[0,25]   @~english Cut Ratio，range:[0,25]
            __declspec(property(put = SetCutRatio, get = GetCutRatio)) int CutRatio;

            virtual int GetCutRatio() = 0;

            virtual void SetCutRatio(int value) = 0;

            /// @~chinese 裁剪X方向，范围：[0,25]   @~english Cut Ratio X，range:[0,25]
            __declspec(property(put = SetCutRatioX, get = GetCutRatioX)) int CutRatioX;

            virtual int GetCutRatioX() = 0;

            virtual void SetCutRatioX(int value) = 0;

            /// @~chinese 裁剪Y方向，范围：[0,25]   @~english Cut Ratio Y，range:[0,25]
            __declspec(property(put = SetCutRatioY, get = GetCutRatioY)) int CutRatioY;

            virtual int GetCutRatioY() = 0;

            virtual void SetCutRatioY(int value) = 0;

            /// @~chinese 自动清空   @~english Auto Clear
            __declspec(property(put = SetAutoClear, get = GetAutoClear)) bool AutoClear;

            virtual bool GetAutoClear() = 0;

            virtual void SetAutoClear(bool value) = 0;

            /// @~chinese 自动填充   @~english Auto Fill
            __declspec(property(put = SetAutoFill, get = GetAutoFill)) bool AutoFill;

            virtual bool GetAutoFill() = 0;

            virtual void SetAutoFill(bool value) = 0;

            /// @~chinese 清空图像   @~english Clear Image
            virtual int DoClearImage() = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ImgStitchCalibResults.  */
        ///图像拼接结果类
        class ImgStitchCalibResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImgStitchCalibResults() {}
            virtual ~ImgStitchCalibResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 获取拼接结果图像
             * @return 拼接结果图像
             */
            virtual ImageBaseData GetStitchImage() = 0;
            /**
             * @brief 当前图像数量
             */
            virtual int GetCurNum() = 0;
            /**
            * @brief 图像总数量
            */
            virtual int GetTotalNum() = 0;
            /**
             * @brief 获取每张原图在拼接结果图中的位置信息
             * @param nIndex [IN] 查询点的索引值，从0开始编码
             * @return 每张原图在拼接结果图中的位置信息
             */
            virtual RectBox GetImagePosition(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ImgStitchCalib tool.  */
        ///图像拼接工具类
        class IMVSImgStitchCalibModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSImgStitchCalibModuTool() {}
            virtual ~IMVSImgStitchCalibModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ImgStitchCalibParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ImgStitchCalibResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

            /**
            * @brief 导入模型
            * @param 模型路径
            * @return 无，出错时抛出异常
            * @remarks 导入模型
            */
            virtual void ImportModelData(IN const char * const strPath) = 0;

        private:
            IMVSImgStitchCalibModuTool(IMVSImgStitchCalibModuTool&);
            IMVSImgStitchCalibModuTool& operator=(IMVSImgStitchCalibModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ImgStitchCalib.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSImgStitchCalibModu::IMVSImgStitchCalibModuTool * __stdcall GetImgStitchCalibToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_IMGSTITCHCALIB_H_
