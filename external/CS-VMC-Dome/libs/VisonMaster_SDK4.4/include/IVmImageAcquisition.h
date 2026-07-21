/***************************************************************************************************
 * File：IVmImageAcquisition.h
 * Note：@~chinese 多图采集模块接口声明 @~english Interface for the ImageAcquisition tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_IMAGEACQUISITION_H_
#define _IVM_IMAGEACQUISITION_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace ImageAcquisitionModule
    {
        /// @~chinese 图像源   @~english Image Source
        typedef enum _ImageSourceTypeEnum
        {
            /// @~chinese 本地图像   @~english Local Image
            ImageSourceType_LocalImage = 0x1,

            /// @~chinese 相机   @~english Camera
            ImageSourceType_Camera = 0x2,

        }ImageSourceTypeEnum;

    /// \addtogroup 多图采集
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the ImageAcquisitionParams.  */
        ///多图采集参数类
        class ImageAcquisitionParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageAcquisitionParams() {}
            virtual ~ImageAcquisitionParams() {}

        public:

            /// @~chinese 图像源   @~english Image Source
            __declspec(property(put = SetImageSourceType, get = GetImageSourceType)) ImageSourceTypeEnum ImageSourceType;

            virtual ImageSourceTypeEnum GetImageSourceType() = 0;

            virtual void SetImageSourceType(ImageSourceTypeEnum value) = 0;

            /// @~chinese 取图间隔，范围：[0,1000]   @~english Acquisition Rate，range:[0,1000]
            __declspec(property(put = SetInterval, get = GetInterval)) int Interval;

            virtual int GetInterval() = 0;

            virtual void SetInterval(int value) = 0;

            /// @~chinese 取图数量，范围：[3,8]   @~english Image Count，range:[3,8]
            __declspec(property(put = SetImageCount, get = GetImageCount)) int ImageCount;

            virtual int GetImageCount() = 0;

            virtual void SetImageCount(int value) = 0;

            /// @~chinese 图像路径(弃用)   @~english Image Path(Obsolete)
            __declspec(property(put = SetImageDir, get = GetImageDir)) const char* ImageDir;

            virtual const char* GetImageDir() = 0;

            virtual void SetImageDir(const char* value) = 0;

            /**
            * @brief 设置图像文件夹路径
            * @param strParam  [IN]  输出配置参数
            */
            virtual void SetImageDirPath(const char * strParam) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ImageAcquisitionResults.  */
        ///多图采集结果类
        class ImageAcquisitionResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageAcquisitionResults() {}
            virtual ~ImageAcquisitionResults() {}

        public:
            /**
             * @brief 帧号
             */
            virtual int GetFrameNum() = 0;

            /**
             * @brief 丢帧数
             */
            virtual int GetLostFrameCount() = 0;

            /**
             * @brief 丢包数
             */
            virtual int GetLostPacketCount() = 0;

            /**
             * @brief 相机获取失败次数
             */
            virtual int GetCameraFailCount() = 0;

            /**
             * @brief 获取图像数量
             */
            virtual int GetImageCount() = 0;

            /**
             * @brief 获取分布角
             */
            virtual float GetDistributionAngle(int nIndex) = 0;

            /**
             * @brief 获取照射角
             */
            virtual float GetIrradiationAngle(int nIndex) = 0;

            /**
             * @brief 获取输出图像
             */
            virtual ImageBaseData GetOutputImage(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ImageAcquisition tool.  */
        ///多图采集工具类
        class ImageAcquisitionModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageAcquisitionModuleTool() {}
            virtual ~ImageAcquisitionModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ImageAcquisitionParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ImageAcquisitionResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            ImageAcquisitionModuleTool(ImageAcquisitionModuleTool&);
            ImageAcquisitionModuleTool& operator=(ImageAcquisitionModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ImageAcquisition.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::ImageAcquisitionModule::ImageAcquisitionModuleTool * __stdcall GetImageAcquisitionToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_IMAGEACQUISITION_H_
