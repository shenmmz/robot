/***************************************************************************************************
 * File：IVmBcr.h
 * Note：@~chinese 条码识别模块接口声明 @~english Interface for the Bcr tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_BCR_H_
#define _IVM_BCR_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSBcrModu
    {
    /// \addtogroup 条码识别
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///条码识别ROI管理类
        class BcrRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the BcrParams.  */
        ///条码识别参数类
        class BcrParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit BcrParams() {}
            virtual ~BcrParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual BcrRoiManager* GetModuRoiManager() = 0;

            /// @~chinese CODE39码   @~english CODE 39
            __declspec(property(put = SetCODE39, get = GetCODE39)) bool CODE39;

            virtual bool GetCODE39() = 0;

            virtual void SetCODE39(bool value) = 0;

            /// @~chinese CODE128码   @~english CODE 128
            __declspec(property(put = SetCODE128, get = GetCODE128)) bool CODE128;

            virtual bool GetCODE128() = 0;

            virtual void SetCODE128(bool value) = 0;

            /// @~chinese 库得巴码   @~english Codabar
            __declspec(property(put = SetCODABAR, get = GetCODABAR)) bool CODABAR;

            virtual bool GetCODABAR() = 0;

            virtual void SetCODABAR(bool value) = 0;

            /// @~chinese EAN码   @~english EAN
            __declspec(property(put = SetEAN, get = GetEAN)) bool EAN;

            virtual bool GetEAN() = 0;

            virtual void SetEAN(bool value) = 0;

            /// @~chinese 交替25码   @~english ITF25
            __declspec(property(put = SetITF25, get = GetITF25)) bool ITF25;

            virtual bool GetITF25() = 0;

            virtual void SetITF25(bool value) = 0;

            /// @~chinese CODE93码   @~english CODE 93
            __declspec(property(put = SetCODE93, get = GetCODE93)) bool CODE93;

            virtual bool GetCODE93() = 0;

            virtual void SetCODE93(bool value) = 0;

            /// @~chinese 条码个数，范围：[1,200]   @~english Bar Code Number，range:[1,200]
            __declspec(property(put = SetBarNum, get = GetBarNum)) int BarNum;

            virtual int GetBarNum() = 0;

            virtual void SetBarNum(int value) = 0;

            /// @~chinese 降采样系数，范围：[1,8]   @~english Decimation Coefficient，range:[1,8]
            __declspec(property(put = SetPreSampleLevel, get = GetPreSampleLevel)) int PreSampleLevel;

            virtual int GetPreSampleLevel() = 0;

            virtual void SetPreSampleLevel(int value) = 0;

            /// @~chinese 检测窗口大小，范围：[4,5]   @~english Detection Window Size，range:[4,5]
            __declspec(property(put = SetLocWinSize, get = GetLocWinSize)) int LocWinSize;

            virtual int GetLocWinSize() = 0;

            virtual void SetLocWinSize(int value) = 0;

            /// @~chinese 静区宽度，范围：[0,200]   @~english Quiet Zone Width，range:[0,200]
            __declspec(property(put = SetSegQuietW, get = GetSegQuietW)) int SegQuietW;

            virtual int GetSegQuietW() = 0;

            virtual void SetSegQuietW(int value) = 0;

            /// @~chinese 去伪过滤尺寸，范围：[0,4000]   @~english DfkSize，range:[0,4000]
            __declspec(property(put = SetDfkMinSize, get = GetDfkMinSize)) int DfkMinSize;

            virtual int GetDfkMinSize() = 0;

            virtual void SetDfkMinSize(int value) = 0;

            /// @~chinese 去伪过滤尺寸，范围：[0,4000]   @~english DfkSize，range:[0,4000]
            __declspec(property(put = SetDfkMaxSize, get = GetDfkMaxSize)) int DfkMaxSize;

            virtual int GetDfkMaxSize() = 0;

            virtual void SetDfkMaxSize(int value) = 0;

            /// @~chinese 超时退出时间，范围：[0,5000]   @~english Timeout-Period to Exit，range:[0,5000]
            __declspec(property(put = SetWaitingTime, get = GetWaitingTime)) int WaitingTime;

            virtual int GetWaitingTime() = 0;

            virtual void SetWaitingTime(int value) = 0;

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
        /** @enum MVD_1DCODE_TYPE
        *  @brief  1D Code Type - enum type
        */
        ///条码类型
        typedef _MVD_1DCODE_TYPE__ MVD_1DCODE_TYPE;

        /*  Note: Interface Classes for describing 1D Code detail information.  */
        class I1DCodeInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit I1DCodeInfo() {}
            virtual ~I1DCodeInfo() {}

        public:
            /**
            * @brief 条码的位置,[0,3600],顺时针
            */
            virtual RectBox GetPosition(void) = 0;
            /**
             * @brief 条码内容
             */
            virtual const char* GetContent() = 0;
            /**
             * @brief 条码类型
             */
            virtual MVD_1DCODE_TYPE GetType() = 0;
            /**
             * @brief 条码角度
             */
            virtual float GetAngle() = 0;
            /**
            * @brief 估计ppm
            */
            virtual float GetEstppm() = 0;
        };

        /*  Note: Interface Classes of the result of the BcrResults.  */
        ///条码识别结果类
        class BcrResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit BcrResults() {}
            virtual ~BcrResults() {}

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
             * @brief 条码个数
             */
            virtual int GetCodeNum() = 0;
            /**
             * @brief 条码区域个数(当前版本尚未支持,输出为0)
             */
            virtual int GetRegionNum() = 0;
            /**
             * @brief 获取条码信息
             * @param nIndex [IN] 条码信息索引
             * @return 返回条码信息
             * @remarks nIndex从0开始计数
             */
            virtual I1DCodeInfo* GetCodeInfo(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the Bcr tool.  */
        ///条码识别工具类
        class IMVSBcrModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSBcrModuTool() {}
            virtual ~IMVSBcrModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual BcrParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual BcrResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSBcrModuTool(IMVSBcrModuTool&);
            IMVSBcrModuTool& operator=(IMVSBcrModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for Bcr.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSBcrModu::IMVSBcrModuTool * __stdcall GetBcrToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_BCR_H_
