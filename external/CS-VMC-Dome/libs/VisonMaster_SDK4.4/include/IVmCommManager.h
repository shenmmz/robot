/***************************************************************************************************
 * File：IVmCommManager.h
 * Note：@~chinese 通信管理模块接口声明 @~english Interface for the CommManagerModule tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_COMMMANAGER_H_
#define _IVM_COMMMANAGER_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "IVmLightControl.h"

namespace VisionMasterSDK
{
    namespace CommManagerModule
    {
        /// \addtogroup 通信管理
        /// @{

        /*  Note: Note: This is the class for the CommManagerModule tool.  */
        ///通信管理工具
        class CommManagerModuleTool
        {
        protected:
            ///< Constructor and Destructor
            explicit CommManagerModuleTool() {}
            virtual ~CommManagerModuleTool() {}

        public:
           /**
            * @brief 设置字符串型数据
            * @param nDeviceId  [IN]    设备ID
            * @param strValue   [IN]    字符串
            * @param nAddressId [IN]    设备地址，只针对PLC与Modbus
            */
            virtual void SetString(IN int nDeviceId, IN const char * const strValue, IN int nAddressId = -1) = 0;

           /**
            * @brief 设置整型数据（只支持PLC和Modbus通信协议设置）
            * @param nDeviceId  [IN]    设备ID
            * @param nValue     [IN]    整型数据
            * @param nNum       [IN]    数量
            * @param nAddressId [IN]    设备地址，只针对PLC与Modbus
            */
            virtual void SetInt(IN int nDeviceId, IN const int * const nValue, IN const int nNum, IN int nAddressId = -1) = 0;

           /**
            * @brief 设置浮点型数据（只支持PLC和Modbus通信协议设置）
            * @param  nDeviceId  [IN]    设备ID
            * @param  fValue     [IN]    浮点型数据
            * @param  nNum       [IN]    数量
            * @param  nAddressId [IN]    设备地址，只针对PLC与Modbus
            */
            virtual void SetFloat(IN int nDeviceId, IN const float * const fValue, IN const int nNum, IN int nAddressId = -1) = 0;

            /**
            * @brief 设置二进制型数据（不支持PLC）
            * @param nDeviceId  [IN]    设备ID
            * @param btValue    [IN]    二进制型数据
            * @param nLen       [IN]    二进制型数据长度
            * @param nAddressId [IN]    设备地址，只针对Modbus
            */
            virtual void SetBytes(IN int nDeviceId, IN const char * const btValue, IN const size_t nLen, IN int nAddressId = -1) = 0;

            /**
            * @brief 获取读取数据
            * @param nDeviceId  [IN]      设备ID
            * @param pData      [OUT]     二进制型数据
            * @param nLen       [IN/OUT]  缓存区与实际长度（输入nLen数据长度应与实际数据长度一致）
            * @param nAddressId [IN]      设备地址，只针对PLC与Modbus
            */
            virtual void GetReadData(IN int nDeviceId, OUT char * const pData, INOUT size_t * const nLen, IN int nAddressId = -1) = 0;

            /**
            * @brief 通信设备是否处于连接状态
            * @param nDeviceId [IN] 设备ID
            * @return true: 连接，false: 未连接
            */
            virtual bool bIsDeviceConnect(IN int nDeviceId) = 0;

            /**
            * @brief 获取通信设备参数值
            * @param nDeviceId  [IN]   设备ID
            * @param strName    [IN]   参数名
            * @param nValueSize [IN]   参数值接收区大小
            * @param strValue   [OUT]  参数值
            * @param nAddressId [IN]   设备地址
            */
            virtual void GetParamValue(IN int nDeviceId, IN const char * const strName, IN const unsigned int  nValueSize, OUT char * const strValue, IN int nAddressId = -1) = 0;

            /**
            * @brief 设置通信设备参数值
            * @param nDeviceId  [IN]  设备ID
            * @param strName    [IN]  参数名
            * @param strValue   [IN]  参数值
            * @param nAddressId [IN]  设备地址
            */
            virtual void SetParamValue(IN int nDeviceId, IN const char * const strName, IN const char * const strValue, IN int nAddressId = -1) = 0;

            /**
            * @brief 设置全局光源参数
            * @param pstLightParam  [IN]  
            * @remarks 异步接口，部分异常设置不返回错误
            */
            virtual void SetGlobalLightParam(IN LightControlModule::GlobalLightParam * const pstLightParam) = 0;

        private:
            CommManagerModuleTool(CommManagerModuleTool&);
            CommManagerModuleTool& operator=(CommManagerModuleTool&);
        };
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

    /*  Note: Get tool instance for CommManagerModule.  */
    IMVS_6000_PLATFORMSDK_API VisionMasterSDK::CommManagerModule::CommManagerModuleTool * __stdcall GetCommManagerToolInstance(void);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_COMMMANAGER_H_
