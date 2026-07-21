/***************************************************************************************************
* File：IVmLightControl.h
* Note：CH: 光源控制模块接口声明 | En: Interface for the LightControl tool.
*
***************************************************************************************************/
#ifndef _IVM_LIGHTCONTROL_H_
#define _IVM_LIGHTCONTROL_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"

namespace VisionMasterSDK
{
    namespace LightControlModule
    {
        /// 设备类型
        /// V4.3之前版本原有控制器管理使用TYPE_开头部分
        /// V4.4版本新增通信管理-控制器管理使用TYPE_IN_COMM_开头部分
        typedef enum DeviceTypeEnum_
        {
            TYPE_NONE = 0x00000000,                      ///< 无
            TYPE_LIGHT_CONTROLLER = 0x00001004,          ///< 光源控制器
            TYPE_VB2200 = 0x00440101,                    ///< VB2200系列视觉控制器
            TYPE_VB2230 = 0x00440102,                    ///< VB2230系列视觉控制器
            TYPE_VC4000 = 0x00880204,                    ///< VC4000系列视觉控制器
            TYPE_VC3000_LIGHT = 0x00000204,              ///< VC3000系列视觉控制器光源模块
            TYPE_VC3000_IO = 0x00880000,                 ///< VC3000系列视觉控制器IO模块
            TYPE_VC3000_GPIO = 0x00380000,               ///< VC3000系列视觉控制器GPIO模块
            TYPE_VC2000 = 0x01880204,                    ///< VC2000系列视觉控制器

            TYPE_IN_COMM_LIGHT_4C = 101,                 ///< 4C系列视觉控制器
            TYPE_IN_COMM_LIGHT_6C = 102,                 ///< 6C系列视觉控制器
            TYPE_IN_COMM_VB2200 = 103,                   ///< VB2200系列视觉控制器
            TYPE_IN_COMM_VB2230 = 104,                   ///< VB2230系列视觉控制器
            TYPE_IN_COMM_VC2000 = 105,                   ///< VC2000系列视觉控制器
            TYPE_IN_COMM_VC3000_IO = 106,                ///< VC3000系列视觉控制器IO模块
            TYPE_IN_COMM_VC3000_LIGHT = 107,             ///< VC3000系列视觉控制器光源模块
            TYPE_IN_COMM_GPIO = 108,                     ///< VC3000系列视觉控制器GPIO模块
            TYPE_IN_COMM_VC3000H = 109,                  ///< VC3000H系列视觉控制器
            TYPE_IN_COMM_VC4000 = 110,                   ///< VC4000系列视觉控制器

        }DeviceTypeEnum;

        /// 沿定义,用户表示输入信号的触发
        typedef enum TriggerEdgeEnum_
        {
            MV_IO_EDGE_UNKNOW = 0x00,        ///< 未知
            MV_IO_EDGE_RISING = 0x01,        ///< 上升沿
            MV_IO_EDGE_FALLING = 0x02,        ///< 下降沿
        } TriggerEdgeEnum;

        /// 光源常亮常灭
        typedef enum LightStateEnum_
        {
            MV_IO_LIGHTSTATE_ON = 0x0001,     ///< 触发后常亮
            MV_IO_LIGHTSTATE_OFF = 0x0002,     ///< 触发后常灭
        } LightStateEnum;

    /// \addtogroup 控制器管理
    /// @{
        /// 光源控制信息结构
        typedef struct LightChannelInfo_
        {
            int  nChannelIndex;    ///< 通道序号
            bool bChannelEnable;   ///< 通道使能
            int  nLightBrightness; ///< 通道亮度
            int  nLightState;      ///< 光源状态。常灭：1，常亮：2
            int  nTriggerEdge;     ///< 沿定义（硬触发）
            int  nDurationTime;    ///< 持续时间（硬触发）
        } LightChannelInfo;

        /// 光源通道信息
        typedef struct LightConfigInfo_
        {
            LightChannelInfo stChannel1;     ///< 通道1
            LightChannelInfo stChannel2;     ///< 通道2
            LightChannelInfo stChannel3;     ///< 通道3
            LightChannelInfo stChannel4;     ///< 通道4
            LightChannelInfo stChannel5;     ///< 通道5
            LightChannelInfo stChannel6;     ///< 通道6
        } LightConfigInfo;

        /// 光源通道参数信息
        typedef struct GlobalLightParam_
        {
            int nDeviceIndex;                  ///< 设备序号
            int nDeviceType;                   ///< 设备类型 
            char strReserved[16];              ///< 保留字段
            int nTriggerTime;                  ///< 触发时间
            LightConfigInfo stLightConfig;     ///< 光源通道信息
            int nReserved[4];                  ///< 保留字段
        } GlobalLightParam;

        /*  Note: Note: This is the class for the LightControl tool.  */
		///光源控制工具
        class LightControlModuleTool
        {
        protected:
            ///< Constructor and Destructor
            explicit LightControlModuleTool() {}
            virtual ~LightControlModuleTool() {}

        public:
            /**
            * @brief 设置全局光源参数信息
            * @remarks 异步接口，部分异常设置不返回错误
            */
            virtual void SetGlobalLightParam(IN GlobalLightParam * const pstLightParam) = 0;

        private:
            LightControlModuleTool(LightControlModuleTool&);
            LightControlModuleTool& operator=(LightControlModuleTool&);
        };
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

    /*  Note: Get tool instance for LightControl.  */
    IMVS_6000_PLATFORMSDK_API VisionMasterSDK::LightControlModule::LightControlModuleTool * __stdcall GetLightControlToolInstance(void);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_LIGHTCONTROL_H_