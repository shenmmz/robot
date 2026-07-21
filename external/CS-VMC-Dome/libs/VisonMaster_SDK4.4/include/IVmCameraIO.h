/***************************************************************************************************
 * File：IVmCameraIO.h
 * Note：@~chinese 相机IO通信模块接口声明 @~english Interface for the CameraIO tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CAMERAIO_H_
#define _IVM_CAMERAIO_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace CameraIOModule
    {
        /// @~chinese 相机类型   @~english Camera Type
        typedef enum _CameraTypeEnum
        {
            /// @~chinese 普通相机   @~english CommonCamera
            CameraType_CommonCamera = 0x1,

            /// @~chinese 智能相机   @~english SmartCamera
            CameraType_SmartCamera = 2,

            /// @~chinese 线阵相机   @~english Dalsa Camera
            CameraType_DalsaCamera = 3,

        }CameraTypeEnum;

        /// @~chinese 有效电平   @~english Active Level
        typedef enum _IOEnableElectricalLevelEnum
        {
            /// @~chinese 高电平有效   @~english Active High
            IOEnableElectricalLevel_High = 0x1,

            /// @~chinese 低电平有效   @~english Active Low
            IOEnableElectricalLevel_Low = 2,

        }IOEnableElectricalLevelEnum;

        /// @~chinese 输出类型   @~english Output Type
        typedef enum _IOOutTypeEnum
        {
            /// @~chinese OK时输出   @~english Output when OK
            IOOutType_Ok = 0x1,

            /// @~chinese NG时输出   @~english Output when NG
            IOOutType_Ng = 2,

        }IOOutTypeEnum;

    /// \addtogroup 相机IO通信
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the CameraIOParams.  */
        ///相机IO通信参数类
        class CameraIOParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CameraIOParams() {}
            virtual ~CameraIOParams() {}

        public:

            /// @~chinese 相机类型   @~english Camera Type
            __declspec(property(put = SetCameraType, get = GetCameraType)) CameraTypeEnum CameraType;

            virtual CameraTypeEnum GetCameraType() = 0;

            virtual void SetCameraType(CameraTypeEnum value) = 0;

            /// @~chinese 持续时间使能   @~english DurationTimeEnable
            __declspec(property(put = SetDurationTimeEnable, get = GetDurationTimeEnable)) bool DurationTimeEnable;

            virtual bool GetDurationTimeEnable() = 0;

            virtual void SetDurationTimeEnable(bool value) = 0;

            /// @~chinese 持续时间，范围：[1,10000]   @~english Duration，range:[1,10000]
            __declspec(property(put = SetIODurationTime, get = GetIODurationTime)) int IODurationTime;

            virtual int GetIODurationTime() = 0;

            virtual void SetIODurationTime(int value) = 0;

            /// @~chinese 有效电平(弃用)   @~english Active Level(Obsolete)
            __declspec(property(put = SetIOEnableElectricalLevel, get = GetIOEnableElectricalLevel)) IOEnableElectricalLevelEnum IOEnableElectricalLevel;

            virtual IOEnableElectricalLevelEnum GetIOEnableElectricalLevel() = 0;

            virtual void SetIOEnableElectricalLevel(IOEnableElectricalLevelEnum value) = 0;

            /// @~chinese 输出类型   @~english Output Type
            __declspec(property(put = SetIOOutType, get = GetIOOutType)) IOOutTypeEnum IOOutType;

            virtual IOOutTypeEnum GetIOOutType() = 0;

            virtual void SetIOOutType(IOOutTypeEnum value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the CameraIOResults.  */
        ///相机IO通信结果类
        class CameraIOResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CameraIOResults() {}
            virtual ~CameraIOResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 通信结果
             */
            virtual const char * GetStrResult() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CameraIO tool.  */
        ///相机IO通信工具类
        class CameraIOModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CameraIOModuleTool() {}
            virtual ~CameraIOModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CameraIOParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CameraIOResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            CameraIOModuleTool(CameraIOModuleTool&);
            CameraIOModuleTool& operator=(CameraIOModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CameraIO.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::CameraIOModule::CameraIOModuleTool * __stdcall GetCameraIOToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CAMERAIO_H_
