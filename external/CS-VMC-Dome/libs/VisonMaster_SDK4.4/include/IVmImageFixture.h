/***************************************************************************************************
 * File：IVmImageFixture.h
 * Note：@~chinese 图像修正模块接口声明 @~english Interface for the ImageFixture tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_IMAGEFIXTURE_H_
#define _IVM_IMAGEFIXTURE_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSImageFixtureModu
    {
    /// \addtogroup 图像修正
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the ImageFixtureParams.  */
        ///图像修正参数类
        class ImageFixtureParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageFixtureParams() {}
            virtual ~ImageFixtureParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 位置修正信息(仅当次执行起效)   @~english Position Correction Info(Only valid for this execution)
            virtual void SetPositionCorrectionInfo(Fixture* pInputValue) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ImageFixtureResults.  */
        ///图像修正结果类
        class ImageFixtureResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageFixtureResults() {}
            virtual ~ImageFixtureResults() {}

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
        /*  Note: Note: This is the class for the ImageFixture tool.  */
        ///图像修正工具类
        class IMVSImageFixtureModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSImageFixtureModuTool() {}
            virtual ~IMVSImageFixtureModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ImageFixtureParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ImageFixtureResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSImageFixtureModuTool(IMVSImageFixtureModuTool&);
            IMVSImageFixtureModuTool& operator=(IMVSImageFixtureModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ImageFixture.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSImageFixtureModu::IMVSImageFixtureModuTool * __stdcall GetImageFixtureToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_IMAGEFIXTURE_H_
