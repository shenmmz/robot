/***************************************************************************************************
 * File：IVmImageSource.h
 * Note：@~chinese 图像源模块接口声明 @~english Interface for the ImageSource tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_IMAGESOURCE_H_
#define _IVM_IMAGESOURCE_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace ImageSourceModule
    {
        /// @~chinese 图像源   @~english Image Source
        typedef enum _ImageSourceTypeEnum
        {
            /// @~chinese 本地图像   @~english Local Image
            ImageSourceType_LocalImage = 0x1,

            /// @~chinese 相机   @~english Camera
            ImageSourceType_Camera = 0x2,

            /// @~chinese SDK   @~english SDK
            ImageSourceType_SDK = 0x3,

        }ImageSourceTypeEnum;

        /// @~chinese 像素格式   @~english Pixel Format
        typedef enum _PixelFormatEnum
        {
            /// @~chinese MONO8   @~english MONO8
            PixelFormat_MONO8 = 0x1,

            /// @~chinese RGB24   @~english RGB24
            PixelFormat_RGB24 = 0x2,

        }PixelFormatEnum;

    /// \addtogroup 图像源
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the ImageSourceParams.  */
        ///图像源参数类
        class ImageSourceParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageSourceParams() {}
            virtual ~ImageSourceParams() {}

        public:

            /// @~chinese 触发字符(仅当次执行起效)   @~english TriggerString(Only valid for this execution)
            virtual void SetTriggerString(InputStringData* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 过滤字符(仅当次执行起效)   @~english TriggerFilter(Only valid for this execution)
            virtual void SetTriggerFilter(InputStringData* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 曝光时间(仅当次执行起效)   @~english ExposureTimeInput(Only valid for this execution)
            virtual void SetExposureTimeInput(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 增益时间(仅当次执行起效)   @~english GainInput(Only valid for this execution)
            virtual void SetGainInput(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 触发清空变量(仅当次执行起效)   @~english Clear Trigger Value(Only valid for this execution)
            virtual void SetClearTriggerValue(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 图像源   @~english Image Source
            __declspec(property(put = SetImageSourceType, get = GetImageSourceType)) ImageSourceTypeEnum ImageSourceType;

            virtual ImageSourceTypeEnum GetImageSourceType() = 0;

            virtual void SetImageSourceType(ImageSourceTypeEnum value) = 0;

            /// @~chinese 像素格式   @~english Pixel Format
            __declspec(property(put = SetPixelFormat, get = GetPixelFormat)) PixelFormatEnum PixelFormat;

            virtual PixelFormatEnum GetPixelFormat() = 0;

            virtual void SetPixelFormat(PixelFormatEnum value) = 0;

            /// @~chinese 取图间隔，范围：[0,1000]   @~english Acquisition Rate，range:[0,1000]
            __declspec(property(put = SetInterval, get = GetInterval)) int Interval;

            virtual int GetInterval() = 0;

            virtual void SetInterval(int value) = 0;

            /// @~chinese 方案存图   @~english Save Image
            __declspec(property(put = SetSolSaveImageData, get = GetSolSaveImageData)) bool SolSaveImageData;

            virtual bool GetSolSaveImageData() = 0;

            virtual void SetSolSaveImageData(bool value) = 0;

            /// @~chinese 显示图像名称   @~english Show Image Name
            __declspec(property(put = SetShowImageName, get = GetShowImageName)) bool ShowImageName;

            virtual bool GetShowImageName() = 0;

            virtual void SetShowImageName(bool value) = 0;

            /// @~chinese SN初始值，范围：[0,100000]   @~english Initial SN，range:[0,100000]
            __declspec(property(put = SetInitialSN, get = GetInitialSN)) int InitialSN;

            virtual int GetInitialSN() = 0;

            virtual void SetInitialSN(int value) = 0;

            /// @~chinese 图片缓存，范围：[0,5]   @~english PathCache，range:[0,5]
            __declspec(property(put = SetPathCache, get = GetPathCache)) int PathCache;

            virtual int GetPathCache() = 0;

            virtual void SetPathCache(int value) = 0;

            /// @~chinese 输出Mono8   @~english Output Mono8
            __declspec(property(put = SetOutMono8, get = GetOutMono8)) bool OutMono8;

            virtual bool GetOutMono8() = 0;

            virtual void SetOutMono8(bool value) = 0;

            /// @~chinese 拼接使能   @~english Stitch Enable
            __declspec(property(put = SetStitchEnable, get = GetStitchEnable)) bool StitchEnable;

            virtual bool GetStitchEnable() = 0;

            virtual void SetStitchEnable(bool value) = 0;

            /// @~chinese 起始高度，范围：[0,20000]   @~english Start Height，range:[0,20000]
            __declspec(property(put = SetStitchStartHeight, get = GetStitchStartHeight)) int StitchStartHeight;

            virtual int GetStitchStartHeight() = 0;

            virtual void SetStitchStartHeight(int value) = 0;

            /// @~chinese 拼接高度，范围：[0,20000]   @~english Stitch Height，range:[0,20000]
            __declspec(property(put = SetStitchHeight, get = GetStitchHeight)) int StitchHeight;

            virtual int GetStitchHeight() = 0;

            virtual void SetStitchHeight(int value) = 0;

            /// @~chinese 触发清空   @~english Trigger Clear
            __declspec(property(put = SetClearTrigger, get = GetClearTrigger)) bool ClearTrigger;

            virtual bool GetClearTrigger() = 0;

            virtual void SetClearTrigger(bool value) = 0;

            /// @~chinese 自动切换   @~english Auto Switch
            __declspec(property(put = SetAutoPlay, get = GetAutoPlay)) bool AutoPlay;

            virtual bool GetAutoPlay() = 0;

            virtual void SetAutoPlay(bool value) = 0;

            /// @~chinese 最后一张停止   @~english The last one stops
            __declspec(property(put = SetAutoStop, get = GetAutoStop)) bool AutoStop;

            virtual bool GetAutoStop() = 0;

            virtual void SetAutoStop(bool value) = 0;

            /// @~chinese 字符触发过滤   @~english Character Trigger Filter
            __declspec(property(put = SetTriggerStringEnable, get = GetTriggerStringEnable)) bool TriggerStringEnable;

            virtual bool GetTriggerStringEnable() = 0;

            virtual void SetTriggerStringEnable(bool value) = 0;

            /// @~chinese 订阅文件夹   @~english Subscribe Folder
            __declspec(property(put = SetIsSubscribeFolderMode, get = GetIsSubscribeFolderMode)) bool IsSubscribeFolderMode;

            virtual bool GetIsSubscribeFolderMode() = 0;

            virtual void SetIsSubscribeFolderMode(bool value) = 0;

            /// @~chinese 图像路径   @~english Image Path
            __declspec(property(put = SetSubscribeFolderPath, get = GetSubscribeFolderPath)) const char* SubscribeFolderPath;

            virtual const char* GetSubscribeFolderPath() = 0;

            virtual void SetSubscribeFolderPath(const char* value) = 0;

            /// @~chinese 更新缩略图   @~english Refresh Image
            __declspec(property(put = SetRefreshImage, get = GetRefreshImage)) bool RefreshImage;

            virtual bool GetRefreshImage() = 0;

            virtual void SetRefreshImage(bool value) = 0;

            /// @~chinese 触发流程执行   @~english Trigger Process Run
            __declspec(property(put = SetTriggerProcessRun, get = GetTriggerProcessRun)) bool TriggerProcessRun;

            virtual bool GetTriggerProcessRun() = 0;

            virtual void SetTriggerProcessRun(bool value) = 0;

            /**
            * @brief 通过相机名称设置相机ID
            * @param strCameraName [IN] 相机名称
            * @return 无，出错时抛出异常
            */
            virtual void SetCameraName(IN char* strCameraName) = 0;

            /**
            * @brief 获取绑定相机名称
            * @param strCameraName [IN] 相机名称
            * @return 无，出错时抛出异常
            */
            virtual void GetCameraName(OUT char* strCameraName) = 0;
        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ImageSourceResults.  */
        ///图像源结果类
        class ImageSourceResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageSourceResults() {}
            virtual ~ImageSourceResults() {}

        public:
            /**
             * @brief 图像数据
             */
            virtual ImageBaseData GetImageData() = 0;
            /**
             * @brief Mono8图像数据
             */
            virtual ImageBaseData GetImageDataMono8() = 0;
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
             * @brief 获取相机失败次数
             */
            virtual int GetCameraFailCount() = 0;
            /**
             * @brief 当前本地图像路径
             */
            virtual const char * GetCurImagePath() = 0;
            /**
             * @brief 当前本地图像名称
             */
            virtual const char * GetCurImageName() = 0;
            /**
            * @brief 获取SNCode
            */
            virtual int GetSNCode() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ImageSource tool.  */
        ///图像源工具类
        class ImageSourceModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ImageSourceModuleTool() {}
            virtual ~ImageSourceModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ImageSourceParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ImageSourceResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

            /**
             * @brief 设置图像路径
             * @param strPath [IN] 图像路径
             * @return 无，出错时抛出异常
             * @remarks 设置图像路径（图像源需选择SDK模式）
             */
            virtual void SetImagePath(IN const char * const strPath) = 0;

            /**
             * @brief 设置图像数据
             * @param pstImgData [IN] 图像数据
             * @param bIsWaitCopy [IN] 是否等待图像拷贝完成,如不等待,则在当次执行期间,避免释放该图像数据
             * @return 无，出错时抛出异常
             * @remarks 设置图像数据（图像源需选择SDK模式）(仅当次执行起效)
             */
            virtual void SetImageData(IN const ImageBaseData * const pstImgData, IN bool bIsWaitCopy = true) = 0;

            /**
            * @brief 点对点设置图像数据
            * @param pstImgData [IN] 图像数据
            * @param bIsWaitCopy [IN] 是否等待图像拷贝完成,如不等待,则在当次执行期间,避免释放该图像数据
            * @return 无，出错时抛出异常
            * @remarks 点对点设置图像数据（图像源需选择SDK模式）(仅当次执行起效)
            */
            virtual void SetImageDataP2P(IN const ImageBaseData * const pstImgData, IN bool bIsWaitCopy = true) = 0;

            /**
            * @brief 通过路径添加图像
            * @param strPath [IN] 图像路径
            * @return 无，出错时抛出异常
            */
            virtual void AddInputImageByPath(IN const char * const strPath) = 0;

            /**
            * @brief 通过路径删除图像
            * @param strPath [IN] 图像路径
            * @return 无，出错时抛出异常
            */
            virtual void DeleteInputImageByPath(IN const char * const strPath) = 0;

            /**
            * @brief 清空所有输入图像
            * @return 无，出错时抛出异常
            */
            virtual void ClearAllInputImage() = 0;

            

        private:
            ImageSourceModuleTool(ImageSourceModuleTool&);
            ImageSourceModuleTool& operator=(ImageSourceModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ImageSource.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::ImageSourceModule::ImageSourceModuleTool * __stdcall GetImageSourceToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_IMAGESOURCE_H_
