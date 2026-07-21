/***************************************************************************************************
 * File：IVmImageCorrectManual.h
 * Note：@~chinese 图像矫正模块接口声明 @~english Interface for the ImageCorrectManual tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_IMAGECORRECTMANUAL_H_
#define _IVM_IMAGECORRECTMANUAL_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSImageCorrectManualModu
    {
    /// \addtogroup 图像矫正
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the ImageCorrectManualParams.  */
        ///图像矫正参数类
        class ImageCorrectManualParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageCorrectManualParams() {}
            virtual ~ImageCorrectManualParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 曲张量，范围：[-25600,25500]   @~english Expansion Amount，range:[-25600,25500]
            __declspec(property(put = SetWarpPara, get = GetWarpPara)) int WarpPara;

            virtual int GetWarpPara() = 0;

            virtual void SetWarpPara(int value) = 0;

            /// @~chinese 缩放量，范围：[-25600,25500]   @~english Scaling Amount，range:[-25600,25500]
            __declspec(property(put = SetZoomPara, get = GetZoomPara)) int ZoomPara;

            virtual int GetZoomPara() = 0;

            virtual void SetZoomPara(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ImageCorrectManualResults.  */
        ///图像矫正结果类
        class ImageCorrectManualResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageCorrectManualResults() {}
            virtual ~ImageCorrectManualResults() {}

        public:
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
        /*  Note: Note: This is the class for the ImageCorrectManual tool.  */
        ///图像矫正工具类
        class IMVSImageCorrectManualModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSImageCorrectManualModuTool() {}
            virtual ~IMVSImageCorrectManualModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ImageCorrectManualParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ImageCorrectManualResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSImageCorrectManualModuTool(IMVSImageCorrectManualModuTool&);
            IMVSImageCorrectManualModuTool& operator=(IMVSImageCorrectManualModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ImageCorrectManual.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSImageCorrectManualModu::IMVSImageCorrectManualModuTool * __stdcall GetImageCorrectManualToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_IMAGECORRECTMANUAL_H_
