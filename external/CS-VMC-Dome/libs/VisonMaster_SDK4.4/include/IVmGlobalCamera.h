/***************************************************************************************************
* File：IVmGlobalCamera.h
* Note：CH: 全局相机模块接口声明 | En: Interface for the GlobalCameraModule tool.
*
***************************************************************************************************/
#ifndef _IVM_GLOBALCAMERA_H_
#define _IVM_GLOBALCAMERA_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"

namespace VisionMasterSDK
{
    namespace GlobalCameraModule
    {
        /// @~chinese 选择器 @~english Selector
        typedef enum _GammaSelectorEnum
        {
            /// @~chinese USER @~english USER
            GammaSelector_USER = 1,

            /// @~chinese SRGB @~english SRGB
            GammaSelector_SRGB = 2,

        }GammaSelectorEnum;

        /// @~chinese 电平类型 @~english ElectricalLevel Type
        typedef enum _ElectricalLevelEnum
        {
            /// @~chinese 高电平有效 @~english Active High
            High = 1,

            /// @~chinese 低电平有效 @~english Active Low
            Low = 2,

        }ElectricalLevelEnum;

        /// @~chinese 相机类型 @~english Camera Type
        typedef enum _CameraMoldEnum
        {
            /// @~chinese 普通相机 @~english Common Camera
            CommonCamera = 0x1,

            /// @~chinese 智能相机 @~english Smart Camera
            SmartCamera = 0x2,

            /// @~chinese 线阵相机 @~english Dalsa Camera
            DalsaCamera = 0x3,

        }CameraMoldEnum;

    /// \addtogroup 全局相机
    /// @{  
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the GlobalCameraParams.  */
        ///全局相机参数
        class GlobalCameraParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit GlobalCameraParams() {}
            virtual ~GlobalCameraParams() {}

        public:

            /// @~chinese 图像宽度  @~english Image Width
            __declspec(property(put = SetWidth, get = GetWidth)) int Width;

            virtual int GetWidth() = 0;

            virtual void SetWidth(int value) = 0;

            /// @~chinese 图像高度  @~english Image Height
            __declspec(property(put = SetHeight, get = GetHeight)) int Height;

            virtual int GetHeight() = 0;

            virtual void SetHeight(int value) = 0;

            /// @~chinese 曝光时间  @~english Exposure Time
            __declspec(property(put = SetExposureTime, get = GetExposureTime)) double ExposureTime;

            virtual double GetExposureTime() = 0;

            virtual void SetExposureTime(double value) = 0;

            /// @~chinese 像素格式  @~english Pixel Format
            __declspec(property(put = SetPixelFormat, get = GetPixelFormat)) int PixelFormat;

            virtual int GetPixelFormat() = 0;

            virtual void SetPixelFormat(int value) = 0;

            /// @~chinese 触发源(value范围从0开始，可参考GlobalCameraModuleAlgorithmTab.xml中的TriggerSource参数) | @~english Trigger Source(The value range starts from 0. Please refer to the TriggerSource parameter in GlobalCameraModuleAlgorithmTab.xml)
            __declspec(property(put = SetTriggerSource, get = GetTriggerSource)) int TriggerSource;

            virtual int GetTriggerSource() = 0;

            virtual void SetTriggerSource(int value) = 0;

            /// @~chinese 触发延迟（us）  @~english Trigger Delay(us)
            __declspec(property(put = SetTriggerDelay, get = GetTriggerDelay)) double TriggerDelay;

            virtual double GetTriggerDelay() = 0;

            virtual void SetTriggerDelay(double value) = 0;
			
			/// @~chinese 条件触发数量   @~english Conditional Trigger Number
            __declspec(property(put = SetConditionalTriggerNumber, get = GetConditionalTriggerNumber)) double ConditionalTriggerNumber;

            virtual double GetConditionalTriggerNumber() = 0;

            virtual void SetConditionalTriggerNumber(double value) = 0;

            /// @~chinese 条件触发时延   @~english Conditional Trigger TimeOut
            __declspec(property(put = SetConditionalTriggerTimeOut, get = GetConditionalTriggerTimeOut)) double ConditionalTriggerTimeOut;

            virtual double GetConditionalTriggerTimeOut() = 0;

            virtual void SetConditionalTriggerTimeOut(double value) = 0;

            /// @~chinese 增益（dB）  @~english Gain(dB)
            __declspec(property(put = SetGain, get = GetGain)) double Gain;

            virtual double GetGain() = 0;

            virtual void SetGain(double value) = 0;

            /// @~chinese Gamma使能  @~english Gamma Enable
            __declspec(property(put = SetGammaEnable, get = GetGammaEnable)) bool GammaEnable;

            virtual bool GetGammaEnable() = 0;

            virtual void SetGammaEnable(bool value) = 0;

            /// @~chinese 选择器  @~english Selector
            __declspec(property(put = SetGammaSelector, get = GetGammaSelector)) GammaSelectorEnum GammaSelector;

            virtual GammaSelectorEnum GetGammaSelector() = 0;

            virtual void SetGammaSelector(GammaSelectorEnum value) = 0;

            /// @~chinese 相机类型  @~english Camera Type
            __declspec(property(put = SetCameraMold, get = GetCameraMold)) CameraMoldEnum CameraMold;

            virtual CameraMoldEnum GetCameraMold() = 0;

            virtual void SetCameraMold(CameraMoldEnum value) = 0;

            /// @~chinese 电平类型  @~english ElectricalLevel Type
            __declspec(property(put = SetElectricalLevel, get = GetElectricalLevel)) ElectricalLevelEnum ElectricalLevel;

            virtual ElectricalLevelEnum GetElectricalLevel() = 0;

            virtual void SetElectricalLevel(ElectricalLevelEnum value) = 0;

            /**
            * @brief 获取相机信息列表
            * @return 相机信息列表
            */
            virtual CameraInfoList * GetCameraInfoList(void) = 0;

            /**
            * @brief 获取选择相机SN
            * @return 选择相机SN
            */
            virtual const char * GetChosenCameraSN() = 0;

            /**
            * @brief 设置选择相机SN
            * @param value [IN] 选择相机SN
            */
            virtual void SetChosenCameraSN(const char * value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the GlobalCameraResults.  */
        class GlobalCameraResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit GlobalCameraResults() {}
            virtual ~GlobalCameraResults() {}

        public:

        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the GlobalCameraModule tool.  */
        ///全局相机工具
        class GlobalCameraModuleTool
        {
        protected:
            ///< Constructor and Destructor
            explicit GlobalCameraModuleTool() {}
            virtual ~GlobalCameraModuleTool() {}

        public:
            /**
            * @brief 获取参数对象
            * @return 失败，返回NULL
            * @remarks 通过参数对象可设置获取模块参数
            */
            virtual GlobalCameraParams* GetParamObj() = 0;

            /**
            * @brief 获取结果对象
            * @return 失败，返回NULL
            */
            virtual GlobalCameraResults* GetResult() = 0;

            /**
            * @brief 判断相机是否处于连接状态
            */
            virtual bool bIsCameraConnect(void) = 0;

            /**
            * @brief 保存参数到用户集1
            */
            virtual void SaveParamToUser1(void) = 0;


            /**
            * @brief 获取出图触发流程使能状态
            */
            virtual bool GetTriggerProcessEnable() = 0;

            /**
            * @brief 设置出图是否触发流程运行
            * @remarks true表示触发流程，false表示不触发流程
            */
            virtual void SetTriggerProcessEnable(bool value) = 0;

        private:
            GlobalCameraModuleTool(GlobalCameraModuleTool&);
            GlobalCameraModuleTool& operator=(GlobalCameraModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @} 
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

    /*  Note: Get tool instance for GlobalCameraModule.  */
    IMVS_6000_PLATFORMSDK_API VisionMasterSDK::GlobalCameraModule::GlobalCameraModuleTool * __stdcall GetGlobalCameraToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_GLOBALCAMERA_H_
