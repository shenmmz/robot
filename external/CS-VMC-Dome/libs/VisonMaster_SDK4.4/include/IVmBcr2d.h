/***************************************************************************************************
 * File：IVmBcr2d.h
 * Note：@~chinese 二维码识别模块接口声明 @~english Interface for the Bcr2d tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_BCR2D_H_
#define _IVM_BCR2D_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVS2dBcrModu
    {
        /// @~chinese 极性   @~english Polarity
        typedef enum _PolarityEnum
        {
            /// @~chinese 白底黑码   @~english Dark On White
            Polarity_dark = 0x0,

            /// @~chinese 黑底白码   @~english White On Dark
            Polarity_bright = 0x1,

            /// @~chinese 任意   @~english Any
            Polarity_both = 0x2,

        }PolarityEnum;

        /// @~chinese 边缘类型   @~english Edge Type
        typedef enum _EdgeTypeEnum
        {
            /// @~chinese 连续型   @~english Continuous
            EdgeType_continuous = 0x0,

            /// @~chinese 离散型   @~english Discrete
            EdgeType_discrete = 0x1,

            /// @~chinese 兼容模式   @~english Compatibility
            EdgeType_allmode = 0x2,

        }EdgeTypeEnum;

        /// @~chinese 镜像模式   @~english Mirror Mode
        typedef enum _MirrorEnum
        {
            /// @~chinese 非镜像   @~english Non-Mirrored
            Mirror_Off = 0x0,

            /// @~chinese 镜像   @~english Mirrored
            Mirror_On = 0x1,

            /// @~chinese 任意   @~english Any
            Mirror_Both = 0x2,

        }MirrorEnum;

        /// @~chinese QR畸变   @~english QR Distortion
        typedef enum _DistortionFlagEnum
        {
            /// @~chinese 非畸变   @~english No Distortion
            DistortionFlag_Off = 0x0,

            /// @~chinese 畸变   @~english Distortion
            DistortionFlag_On = 0x1,

        }DistortionFlagEnum;

        /// @~chinese 应用模式   @~english Application Mode
        typedef enum _AppModeEnum
        {
            /// @~chinese 普通模式   @~english Normal Mode
            AppMode_Normal = 0x0,

            /// @~chinese 专家模式   @~english Expert Mode
            AppMode_ProMode = 0x1,

            /// @~chinese 极速模式   @~english Fast Mode
            AppMode_FastMode = 0x2,

        }AppModeEnum;

        /// @~chinese DM码类型   @~english DM Type
        typedef enum _RectangleEnum
        {
            /// @~chinese 正方形   @~english Square
            Rectangle_Square = 0x0,

            /// @~chinese 长方形   @~english Rectangle
            Rectangle_Rectangle = 0x1,

            /// @~chinese 兼容模式   @~english Compatibility
            Rectangle_AllMode = 0x2,

        }RectangleEnum;

    /// \addtogroup 二维码识别
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///二维码识别ROI管理类
        class Bcr2dRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the Bcr2dParams.  */
        ///二维码识别参数类
        class Bcr2dParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit Bcr2dParams() {}
            virtual ~Bcr2dParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual Bcr2dRoiManager* GetModuRoiManager() = 0;

            /// @~chinese QR码   @~english QR Code
            __declspec(property(put = SetQRCode, get = GetQRCode)) bool QRCode;

            virtual bool GetQRCode() = 0;

            virtual void SetQRCode(bool value) = 0;

            /// @~chinese DataMatrix码   @~english DM Code
            __declspec(property(put = SetDMCode, get = GetDMCode)) bool DMCode;

            virtual bool GetDMCode() = 0;

            virtual void SetDMCode(bool value) = 0;

            /// @~chinese 二维码个数，范围：[1,1000]   @~english 2D Code Number，range:[1,1000]
            __declspec(property(put = SetLocCodeNum, get = GetLocCodeNum)) int LocCodeNum;

            virtual int GetLocCodeNum() = 0;

            virtual void SetLocCodeNum(int value) = 0;

            /// @~chinese 极性   @~english Polarity
            __declspec(property(put = SetPolarity, get = GetPolarity)) PolarityEnum Polarity;

            virtual PolarityEnum GetPolarity() = 0;

            virtual void SetPolarity(PolarityEnum value) = 0;

            /// @~chinese 边缘类型   @~english Edge Type
            __declspec(property(put = SetEdgeType, get = GetEdgeType)) EdgeTypeEnum EdgeType;

            virtual EdgeTypeEnum GetEdgeType() = 0;

            virtual void SetEdgeType(EdgeTypeEnum value) = 0;

            /// @~chinese 降采样倍数，范围：[1,8]   @~english Subsampling Ratio，range:[1,8]
            __declspec(property(put = SetSampleLevel, get = GetSampleLevel)) int SampleLevel;

            virtual int GetSampleLevel() = 0;

            virtual void SetSampleLevel(int value) = 0;

            /// @~chinese 码宽范围，范围：[40,1000]   @~english Code Width Range，range:[40,1000]
            __declspec(property(put = SetMinBarSize, get = GetMinBarSize)) int MinBarSize;

            virtual int GetMinBarSize() = 0;

            virtual void SetMinBarSize(int value) = 0;

            /// @~chinese 码宽范围，范围：[40,1000]   @~english Code Width Range，range:[40,1000]
            __declspec(property(put = SetMaxBarSize, get = GetMaxBarSize)) int MaxBarSize;

            virtual int GetMaxBarSize() = 0;

            virtual void SetMaxBarSize(int value) = 0;

            /// @~chinese 镜像模式   @~english Mirror Mode
            __declspec(property(put = SetMirror, get = GetMirror)) MirrorEnum Mirror;

            virtual MirrorEnum GetMirror() = 0;

            virtual void SetMirror(MirrorEnum value) = 0;

            /// @~chinese QR畸变   @~english QR Distortion
            __declspec(property(put = SetDistortionFlag, get = GetDistortionFlag)) DistortionFlagEnum DistortionFlag;

            virtual DistortionFlagEnum GetDistortionFlag() = 0;

            virtual void SetDistortionFlag(DistortionFlagEnum value) = 0;

            /// @~chinese 超时退出时间，范围：[0,5000]   @~english Timeout-Period to Exit，range:[0,5000]
            __declspec(property(put = SetWaitingTime, get = GetWaitingTime)) int WaitingTime;

            virtual int GetWaitingTime() = 0;

            virtual void SetWaitingTime(int value) = 0;

            /// @~chinese 应用模式   @~english Application Mode
            __declspec(property(put = SetAppMode, get = GetAppMode)) AppModeEnum AppMode;

            virtual AppModeEnum GetAppMode() = 0;

            virtual void SetAppMode(AppModeEnum value) = 0;

            /// @~chinese DM码类型   @~english DM Type
            __declspec(property(put = SetRectangle, get = GetRectangle)) RectangleEnum Rectangle;

            virtual RectangleEnum GetRectangle() = 0;

            virtual void SetRectangle(RectangleEnum value) = 0;

            /// @~chinese 个数判断   @~english Number Check
            __declspec(property(put = SetNumLimitEnable, get = GetNumLimitEnable)) bool NumLimitEnable;

            virtual bool GetNumLimitEnable() = 0;

            virtual void SetNumLimitEnable(bool value) = 0;

            /// @~chinese 个数范围，范围：[0,99999]   @~english Number Range，range:[0,99999]
            __declspec(property(put = SetNumLimitLow, get = GetNumLimitLow)) int NumLimitLow;

            virtual int GetNumLimitLow() = 0;

            virtual void SetNumLimitLow(int value) = 0;

            /// @~chinese 个数范围，范围：[0,99999]   @~english Number Range，range:[0,99999]
            __declspec(property(put = SetNumLimitHigh, get = GetNumLimitHigh)) int NumLimitHigh;

            virtual int GetNumLimitHigh() = 0;

            virtual void SetNumLimitHigh(int value) = 0;

            /// @~chinese 字符验证   @~english Character Verification
            __declspec(property(put = SetVerifyEnable, get = GetVerifyEnable)) bool VerifyEnable;

            virtual bool GetVerifyEnable() = 0;

            virtual void SetVerifyEnable(bool value) = 0;

            /// @~chinese 数字集   @~english Number Set
            __declspec(property(put = SetNumVerifyEnable, get = GetNumVerifyEnable)) bool NumVerifyEnable;

            virtual bool GetNumVerifyEnable() = 0;

            virtual void SetNumVerifyEnable(bool value) = 0;

            /// @~chinese 小写字母集   @~english Lowercase Set
            __declspec(property(put = SetSmallAlphabetVerify, get = GetSmallAlphabetVerify)) bool SmallAlphabetVerify;

            virtual bool GetSmallAlphabetVerify() = 0;

            virtual void SetSmallAlphabetVerify(bool value) = 0;

            /// @~chinese 大写字母集   @~english Uppercase Set
            __declspec(property(put = SetBigAlphabetVerify, get = GetBigAlphabetVerify)) bool BigAlphabetVerify;

            virtual bool GetBigAlphabetVerify() = 0;

            virtual void SetBigAlphabetVerify(bool value) = 0;

            /// @~chinese 特殊字符集   @~english Special Character Set
            __declspec(property(put = SetSpecialCharVerify, get = GetSpecialCharVerify)) bool SpecialCharVerify;

            virtual bool GetSpecialCharVerify() = 0;

            virtual void SetSpecialCharVerify(bool value) = 0;

            /// @~chinese 用户字符验证   @~english UDC Verification
            __declspec(property(put = SetUserStringVerify, get = GetUserStringVerify)) bool UserStringVerify;

            virtual bool GetUserStringVerify() = 0;

            virtual void SetUserStringVerify(bool value) = 0;

            /// @~chinese 用户字符   @~english UDC
            __declspec(property(put = SetUserString, get = GetUserString)) const char* UserString;

            virtual const char* GetUserString() = 0;

            virtual void SetUserString(const char* value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /** @enum MVD_2DCODE_TYPE
         *  @brief 二维码类型
         */
        /// 二维码类型
        typedef enum _MVD_2DCODE_TYPE__
        {
            /// @~chinese None @~english None
            MVD_2DCODE_TYPE_NONE = 0,

            /// @~chinese DM @~english DM
            MVD_2DCODE_TYPE_DM = 101,

            /// @~chinese QR @~english QR
            MVD_2DCODE_TYPE_QR = 102,
        }MVD_2DCODE_TYPE;

        ///二维码识别码信息描述类
        class I2DCodeInfo
        {
        protected:
            ///< 构造与析构函数
            explicit I2DCodeInfo() {}
            virtual ~I2DCodeInfo() {}

        public:
            /**
             * @brief 二维码位置坐标
             * @remarks 二维码的位置信息,分别为:左上角、右上角、左下角、右下角顶点坐标
             */
            virtual RectBox GetPosition() = 0;
            /**
             * @brief 二维码内容
             */
            virtual const char* GetContent() = 0;
            /**
             * @brief 二维码类型
             */
            virtual MVD_2DCODE_TYPE GetType() = 0;
            /**
             * @brief 二维码角度
             */
            virtual float GetAngle() = 0;
            /**
             * @brief 估计ppm
             */
            virtual float GetEstppm() = 0;
        };

        /*  Note: Interface Classes of the result of the Bcr2dResults.  */
        ///二维码识别结果类
        class Bcr2dResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit Bcr2dResults() {}
            virtual ~Bcr2dResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;
            /**
             * @brief 二维码个数
             */
            virtual int GetCodeNum() = 0;
            /**
             * @brief 获取二维码信息
             * @param nIndex [IN] 二维码信息索引
             * @return 返回二维码信息
             * @remarks nIndex从0开始计数
             */
            virtual I2DCodeInfo* GetCodeInfo(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the Bcr2d tool.  */
        ///二维码识别工具类
        class IMVS2dBcrModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVS2dBcrModuTool() {}
            virtual ~IMVS2dBcrModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual Bcr2dParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual Bcr2dResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVS2dBcrModuTool(IMVS2dBcrModuTool&);
            IMVS2dBcrModuTool& operator=(IMVS2dBcrModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for Bcr2d.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVS2dBcrModu::IMVS2dBcrModuTool * __stdcall GetBcr2dToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_BCR2D_H_
