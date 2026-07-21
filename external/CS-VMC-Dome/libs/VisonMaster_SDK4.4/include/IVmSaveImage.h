/***************************************************************************************************
 * File：IVmSaveImage.h
 * Note：@~chinese 输出图像模块接口声明 @~english Interface for the SaveImage tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_SAVEIMAGE_H_
#define _IVM_SAVEIMAGE_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace SaveImage
    {
        /// @~chinese 保存条件   @~english Save Condition
        typedef enum _ImageSaveConditionEnum
        {
            /// @~chinese 全部保存   @~english Save All
            ImageSaveCondition_ALL = 0x0,

            /// @~chinese OK时保存   @~english Save OK
            ImageSaveCondition_OK = 0x1,

            /// @~chinese NG时保存   @~english Save NG
            ImageSaveCondition_NG = 0x2,

            /// @~chinese 不保存   @~english Not Save
            ImageSaveCondition_NONE = 0x3,

        }ImageSaveConditionEnum;

        /// @~chinese 存储方式   @~english Save Mode
        typedef enum _ImageSaveTypeEnum
        {
            /// @~chinese 覆盖存储   @~english Overwrite
            ImageSaveType_OverWrite = 0x0,

            /// @~chinese 停止存储   @~english Stop Saving
            ImageSaveType_NoSave = 0x1,

        }ImageSaveTypeEnum;

        /// @~chinese 存储单位   @~english Storage Unit
        typedef enum _DiskUnitEnum
        {
            /// @~chinese MB   @~english MB
            DiskUnit_MByte = 0x0,

            /// @~chinese GB   @~english GB
            DiskUnit_GByte = 0x1,

        }DiskUnitEnum;

        /// @~chinese 保存格式   @~english Picture Format
        typedef enum _ImageTypeEnum
        {
            /// @~chinese BMP   @~english BMP
            ImageType_BMP = 0x0,

            /// @~chinese JPEG   @~english JPEG
            ImageType_JPEG = 0x1,

            /// @~chinese PNG   @~english PNG
            ImageType_PNG = 0x2,

        }ImageTypeEnum;

        /// @~chinese 编码格式   @~english Coding Format
        typedef enum _CodingFormatEnum
        {
            /// @~chinese UTF8   @~english UTF8
            CodingFormat_UTF8 = 0x0,

            /// @~chinese UNICODE   @~english UNICODE
            CodingFormat_UNICODE = 0x1,

        }CodingFormatEnum;

        /// @~chinese 像素格式   @~english Pixel Format
        typedef enum _PixelFormatEnum
        {
            /// @~chinese RGB24   @~english RGB24
            PixelFormat_RGB24 = 0x0,

            /// @~chinese MONO8   @~english MONO8
            PixelFormat_MONO8 = 0x1,

        }PixelFormatEnum;

        /// @~chinese 图形倍率类型   @~english Graph Rate Type
        typedef enum _GraphRateTypeEnum
        {
            /// @~chinese 原图尺寸   @~english Match Image
            GraphRateType_MatchImage = 0x0,

            /// @~chinese 界面尺寸   @~english Match View
            GraphRateType_MatchView = 0x1,

            /// @~chinese 自定义倍率   @~english Custom Rate
            GraphRateType_CustomRate = 0x2,

        }GraphRateTypeEnum;

        /// @~chinese 线型   @~english LineType
        typedef enum _LineTypeEnum
        {
            /// @~chinese 4连通线   @~english 4-connected Line
            LineType_FourConnectedLine = 0x0,

            /// @~chinese 8连通线   @~english 8-connected Line
            LineType_EightConnectedLine = 0x1,

            /// @~chinese 抗锯齿线   @~english Antialiased Line
            LineType_AntialiasedLine = 0x2,

        }LineTypeEnum;

    /// \addtogroup 输出图像
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the SaveImageParams.  */
        ///输出图像参数类
        class SaveImageParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit SaveImageParams() {}
            virtual ~SaveImageParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输出使能   @~english Output Enable
            __declspec(property(put = SetOutputEnable, get = GetOutputEnable)) bool OutputEnable;

            virtual bool GetOutputEnable() = 0;

            virtual void SetOutputEnable(bool value) = 0;

            /// @~chinese 存图使能   @~english Save Image Enable
            __declspec(property(put = SetSaveImageEnable, get = GetSaveImageEnable)) bool SaveImageEnable;

            virtual bool GetSaveImageEnable() = 0;

            virtual void SetSaveImageEnable(bool value) = 0;

            /// @~chinese 触发保存   @~english Save Trigger
            __declspec(property(put = SetImageSaveTrigger, get = GetImageSaveTrigger)) bool ImageSaveTrigger;

            virtual bool GetImageSaveTrigger() = 0;

            virtual void SetImageSaveTrigger(bool value) = 0;

            /// @~chinese 保存条件   @~english Save Condition
            __declspec(property(put = SetImageSaveCondition, get = GetImageSaveCondition)) ImageSaveConditionEnum ImageSaveCondition;

            virtual ImageSaveConditionEnum GetImageSaveCondition() = 0;

            virtual void SetImageSaveCondition(ImageSaveConditionEnum value) = 0;

            /// @~chinese 调试保存   @~english Debug Save
            __declspec(property(put = SetDebugInfoSave, get = GetDebugInfoSave)) bool DebugInfoSave;

            virtual bool GetDebugInfoSave() = 0;

            virtual void SetDebugInfoSave(bool value) = 0;

            /// @~chinese 生成目录   @~english Generate Directory
            __declspec(property(put = SetGenerateDir, get = GetGenerateDir)) bool GenerateDir;

            virtual bool GetGenerateDir() = 0;

            virtual void SetGenerateDir(bool value) = 0;

            /// @~chinese 同步存储(弃用)   @~english Synchronous Storage(Obsolete)
            __declspec(property(put = SetSynchronousStorage, get = GetSynchronousStorage)) bool SynchronousStorage;

            virtual bool GetSynchronousStorage() = 0;

            virtual void SetSynchronousStorage(bool value) = 0;

            /// @~chinese 存图间隔，范围：[1,10]   @~english Storage Interval，range:[1,10]
            __declspec(property(put = SetStorageInterval, get = GetStorageInterval)) int StorageInterval;

            virtual int GetStorageInterval() = 0;

            virtual void SetStorageInterval(int value) = 0;

            /// @~chinese 保存渲染图   @~english Save Render Image
            __declspec(property(put = SetRenderImgEnable, get = GetRenderImgEnable)) bool RenderImgEnable;

            virtual bool GetRenderImgEnable() = 0;

            virtual void SetRenderImgEnable(bool value) = 0;

            /// @~chinese 渲染图路径 @~english Render Image Path
            __declspec(property(put = SetRenderImgPath, get = GetRenderImgPath)) const char* RenderImgPath;

            virtual const char* GetRenderImgPath() = 0;

            virtual void SetRenderImgPath(const char* value) = 0;

            /// @~chinese 渲染OK路径 @~english Rendering OK Path
            __declspec(property(put = SetRenderOKPath, get = GetRenderOKPath)) const char* RenderOKPath;

            virtual const char* GetRenderOKPath() = 0;

            virtual void SetRenderOKPath(const char* value) = 0;

            /// @~chinese 渲染NG路径 @~english Rendering NG Path
            __declspec(property(put = SetRenderNGPath, get = GetRenderNGPath)) const char* RenderNGPath;

            virtual const char* GetRenderNGPath() = 0;

            virtual void SetRenderNGPath(const char* value) = 0;

            /// @~chinese 渲染图缓存，范围：[1,20] @~english Render Image Cache, Range:[1,20]
            __declspec(property(put = SetRenderImgCache, get = GetRenderImgCache)) int RenderImgCache;

            virtual int GetRenderImgCache() = 0;

            virtual void SetRenderImgCache(int value) = 0;

            /// @~chinese 保存原图   @~english Save Original Image
            __declspec(property(put = SetOriginImgEnable, get = GetOriginImgEnable)) bool OriginImgEnable;

            virtual bool GetOriginImgEnable() = 0;

            virtual void SetOriginImgEnable(bool value) = 0;

            /// @~chinese 原图路径 @~english Original Image Path
            __declspec(property(put = SetOriginImgPath, get = GetOriginImgPath)) const char* OriginImgPath;

            virtual const char* GetOriginImgPath() = 0;

            virtual void SetOriginImgPath(const char* value) = 0;

            /// @~chinese 原图OK路径 @~english OK Path of Original Image
            __declspec(property(put = SetOriginOKPath, get = GetOriginOKPath)) const char* OriginOKPath;

            virtual const char* GetOriginOKPath() = 0;

            virtual void SetOriginOKPath(const char* value) = 0;

            /// @~chinese 原图NG路径 @~english NG Path of Original Image
            __declspec(property(put = SetOriginNGPath, get = GetOriginNGPath)) const char* OriginNGPath;

            virtual const char* GetOriginNGPath() = 0;

            virtual void SetOriginNGPath(const char* value) = 0;

            /// @~chinese 原图缓存，范围：[1,20] @~english Original Image Cache, Range:[1,20]
            __declspec(property(put = SetOriginImgCache, get = GetOriginImgCache)) int OriginImgCache;

            virtual int GetOriginImgCache() = 0;

            virtual void SetOriginImgCache(int value) = 0;

            /// @~chinese 存储方式   @~english Save Mode
            __declspec(property(put = SetImageSaveType, get = GetImageSaveType)) ImageSaveTypeEnum ImageSaveType;

            virtual ImageSaveTypeEnum GetImageSaveType() = 0;

            virtual void SetImageSaveType(ImageSaveTypeEnum value) = 0;

            /// @~chinese 磁盘剩余空间，范围：[1,1024]   @~english Disk Freespace，range:[1,1024]
            __declspec(property(put = SetDiskFreespace, get = GetDiskFreespace)) int DiskFreespace;

            virtual int GetDiskFreespace() = 0;

            virtual void SetDiskFreespace(int value) = 0;

            /// @~chinese 存储单位   @~english Storage Unit
            __declspec(property(put = SetDiskUnit, get = GetDiskUnit)) DiskUnitEnum DiskUnit;

            virtual DiskUnitEnum GetDiskUnit() = 0;

            virtual void SetDiskUnit(DiskUnitEnum value) = 0;

            /// @~chinese 最大保存天数，范围：[1,500]   @~english Max Save Days，range:[1,500]
            __declspec(property(put = SetImageMemoryDay, get = GetImageMemoryDay)) int ImageMemoryDay;

            virtual int GetImageMemoryDay() = 0;

            virtual void SetImageMemoryDay(int value) = 0;

            /// @~chinese 保存格式   @~english Picture Format
            __declspec(property(put = SetImageType, get = GetImageType)) ImageTypeEnum ImageType;

            virtual ImageTypeEnum GetImageType() = 0;

            virtual void SetImageType(ImageTypeEnum value) = 0;

            /// @~chinese 图片压缩质量，范围：[1,100]   @~english Compression Quality，range:[1,100]
            __declspec(property(put = SetImageCompressionRation, get = GetImageCompressionRation)) int ImageCompressionRation;

            virtual int GetImageCompressionRation() = 0;

            virtual void SetImageCompressionRation(int value) = 0;

            /// @~chinese 启用FTP   @~english Enable FTP
            __declspec(property(put = SetFTPEnable, get = GetFTPEnable)) bool FTPEnable;

            virtual bool GetFTPEnable() = 0;

            virtual void SetFTPEnable(bool value) = 0;

            /// @~chinese 连接   @~english Connect
            __declspec(property(put = SetConnectEnable, get = GetConnectEnable)) bool ConnectEnable;

            virtual bool GetConnectEnable() = 0;

            virtual void SetConnectEnable(bool value) = 0;

            /// @~chinese 服务器IP @~english Server IP
            __declspec(property(put = SetServerIP, get = GetServerIP)) const char* ServerIP;

            virtual const char* GetServerIP() = 0;

            virtual void SetServerIP(const char* value) = 0;

            /// @~chinese 服务器端口，范围：[1,65535] @~english Server Port, Range:[1,65535]
            __declspec(property(put = SetServerPort, get = GetServerPort)) int ServerPort;

            virtual int GetServerPort() = 0;

            virtual void SetServerPort(int value) = 0;

            /// @~chinese 用户名   @~english User Name
            __declspec(property(put = SetUserName, get = GetUserName)) const char* UserName;

            virtual const char* GetUserName() = 0;

            virtual void SetUserName(const char* value) = 0;

            /// @~chinese 密码   @~english Password
            __declspec(property(put = SetPassword, get = GetPassword)) const char* Password;

            virtual const char* GetPassword() = 0;

            virtual void SetPassword(const char* value) = 0;

            /// @~chinese 编码格式   @~english Coding Format
            __declspec(property(put = SetCodingFormat, get = GetCodingFormat)) CodingFormatEnum CodingFormat;

            virtual CodingFormatEnum GetCodingFormat() = 0;

            virtual void SetCodingFormat(CodingFormatEnum value) = 0;

            /// @~chinese FTP路径   @~english FTP Path
            __declspec(property(put = SetFTPPath, get = GetFTPPath)) const char* FTPPath;

            virtual const char* GetFTPPath() = 0;

            virtual void SetFTPPath(const char* value) = 0;

            /// @~chinese 像素格式   @~english Pixel Format
            __declspec(property(put = SetPixelFormat, get = GetPixelFormat)) PixelFormatEnum PixelFormat;

            virtual PixelFormatEnum GetPixelFormat() = 0;

            virtual void SetPixelFormat(PixelFormatEnum value) = 0;

            /// @~chinese 图形倍率类型   @~english Graph Rate Type
            __declspec(property(put = SetGraphRateType, get = GetGraphRateType)) GraphRateTypeEnum GraphRateType;

            virtual GraphRateTypeEnum GetGraphRateType() = 0;

            virtual void SetGraphRateType(GraphRateTypeEnum value) = 0;

            /// @~chinese 线宽倍率，范围：[1,16]   @~english Line Width Rate，range:[1,16]
            __declspec(property(put = SetLineWidthRate, get = GetLineWidthRate)) int LineWidthRate;

            virtual int GetLineWidthRate() = 0;

            virtual void SetLineWidthRate(int value) = 0;

            /// @~chinese 字宽倍率，范围：[1,16]   @~english Font Size Rate，range:[1,16]
            __declspec(property(put = SetFontSizeRate, get = GetFontSizeRate)) int FontSizeRate;

            virtual int GetFontSizeRate() = 0;

            virtual void SetFontSizeRate(int value) = 0;

            /// @~chinese 线型   @~english LineType
            __declspec(property(put = SetLineType, get = GetLineType)) LineTypeEnum LineType;

            virtual LineTypeEnum GetLineType() = 0;

            virtual void SetLineType(LineTypeEnum value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the SaveImageResults.  */
        ///输出图像结果类
        class SaveImageResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit SaveImageResults() {}
            virtual ~SaveImageResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 原图像保存路径
             */
            virtual const char * GetOriginImageSavePath() = 0;

            /**
             * @brief 原图像名称
             */
            virtual const char * GetOriginImageName() = 0;

            /**
             * @brief 渲染图像路径
             */
            virtual const char * GetRenderImageSavePath() = 0;

            /**
             * @brief 渲染图像名称
             */
            virtual const char * GetRenderImageName() = 0;

            /**
             * @brief 输出图像
             */
            virtual ImageBaseData GetOutputImage() = 0;

             /**
             * @brief 大文件
             */
             virtual const char * GetBigFile() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the SaveImage tool.  */
        ///输出图像工具类
        class SaveImageTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit SaveImageTool() {}
            virtual ~SaveImageTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual SaveImageParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual SaveImageResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            SaveImageTool(SaveImageTool&);
            SaveImageTool& operator=(SaveImageTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for SaveImage.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::SaveImage::SaveImageTool * __stdcall GetSaveImageToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_SAVEIMAGE_H_
