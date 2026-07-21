/***************************************************************************************************
 * File：IVmImageBuffer.h
 * Note：@~chinese 缓存图像模块接口声明 @~english Interface for the ImageBuffer tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_IMAGEBUFFER_H_
#define _IVM_IMAGEBUFFER_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace ImageBufferModule
    {
    /// \addtogroup 缓存图像
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the ImageBufferParams.  */
        ///缓存图像参数类
        class ImageBufferParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageBufferParams() {}
            virtual ~ImageBufferParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 刷新标志(仅当次执行起效)   @~english Refresh Flag(Only valid for this execution)
            virtual void SetRefreshFlag(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 缓存数量(仅当次执行起效)   @~english Cache Quantity(Only valid for this execution)
            virtual void SetBufferCount(int* pInputValue, unsigned int nValueNum) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ImageBufferResults.  */
        ///缓存图像结果类
        class ImageBufferResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageBufferResults() {}
            virtual ~ImageBufferResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 输出图像0
             */
            virtual ImageBaseData GetOutImage0() = 0;

            /**
             * @brief 输出图像1
             */
            virtual ImageBaseData GetOutImage1() = 0;

            /**
             * @brief 输出图像2
             */
            virtual ImageBaseData GetOutImage2() = 0;

            /**
             * @brief 输出图像3
             */
            virtual ImageBaseData GetOutImage3() = 0;

            /**
             * @brief 输出图像4
             */
            virtual ImageBaseData GetOutImage4() = 0;

            /**
             * @brief 输出图像5
             */
            virtual ImageBaseData GetOutImage5() = 0;

            /**
             * @brief 输出图像6
             */
            virtual ImageBaseData GetOutImage6() = 0;

            /**
             * @brief 输出图像7
             */
            virtual ImageBaseData GetOutImage7() = 0;

            /**
             * @brief 输出图像8
             */
            virtual ImageBaseData GetOutImage8() = 0;

            /**
             * @brief 输出图像9
             */
            virtual ImageBaseData GetOutImage9() = 0;

            /**
             * @brief 输出图像10
             */
            virtual ImageBaseData GetOutImage10() = 0;

            /**
             * @brief 输出图像11
             */
            virtual ImageBaseData GetOutImage11() = 0;

            /**
             * @brief 输出图像12
             */
            virtual ImageBaseData GetOutImage12() = 0;

            /**
             * @brief 输出图像13
             */
            virtual ImageBaseData GetOutImage13() = 0;

            /**
             * @brief 输出图像14
             */
            virtual ImageBaseData GetOutImage14() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ImageBuffer tool.  */
        ///缓存图像工具类
        class ImageBufferModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageBufferModuleTool() {}
            virtual ~ImageBufferModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ImageBufferParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ImageBufferResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            ImageBufferModuleTool(ImageBufferModuleTool&);
            ImageBufferModuleTool& operator=(ImageBufferModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ImageBuffer.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::ImageBufferModule::ImageBufferModuleTool * __stdcall GetImageBufferToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_IMAGEBUFFER_H_
