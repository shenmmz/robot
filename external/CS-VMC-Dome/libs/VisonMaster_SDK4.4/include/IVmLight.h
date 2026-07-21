/***************************************************************************************************
 * File：IVmLight.h
 * Note：@~chinese 光源模块接口声明 @~english Interface for the Light tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_LIGHT_H_
#define _IVM_LIGHT_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace LightModule
    {
        /// @~chinese 输出类型   @~english Output Type
        typedef enum _OutputTypeEnum
        {
            /// @~chinese OK时输出   @~english Output when OK
            OutputType_OK = 1,

            /// @~chinese NG时输出   @~english Output when NG
            OutputType_NG = 2,

        }OutputTypeEnum;

        /// @~chinese 光源状态1   @~english Channel 1 Light State
        typedef enum _Channel1LightStateEnum
        {
            /// @~chinese 触发后亮（常灭）   @~english LightStateOn
            Channel1LightState_LightStateOn = 1,

            /// @~chinese 触发后灭（常亮）   @~english LightStateOff
            Channel1LightState_LightStateOff = 2,

        }Channel1LightStateEnum;

        /// @~chinese 光源状态1   @~english Channel 1 Light State
        typedef enum _Channel1LightStateVBEnum
        {
            /// @~chinese 常亮   @~english LightStateOn
            Channel1LightStateVB_LightStateOn = 1,

            /// @~chinese 常灭   @~english LightStateOff
            Channel1LightStateVB_LightStateOff = 2,

        }Channel1LightStateVBEnum;

        /// @~chinese 通道1沿定义   @~english Channel 1 EdgeType
        typedef enum _Channel1EdgeTypeEnum
        {
            /// @~chinese 上升沿   @~english EdgeTypeRising
            Channel1EdgeType_EdgeTypeRising = 1,

            /// @~chinese 下降沿   @~english EdgeTypeFalling
            Channel1EdgeType_EdgeTypeFalling = 2,

        }Channel1EdgeTypeEnum;

        /// @~chinese 光源状态2   @~english Channel 2 Light State
        typedef enum _Channel2LightStateEnum
        {
            /// @~chinese 触发后亮（常灭）   @~english LightStateOn
            Channel2LightState_LightStateOn = 1,

            /// @~chinese 触发后灭（常亮）   @~english LightStateOff
            Channel2LightState_LightStateOff = 2,

        }Channel2LightStateEnum;

        /// @~chinese 光源状态2   @~english Channel 2 Light State
        typedef enum _Channel2LightStateVBEnum
        {
            /// @~chinese 常亮   @~english LightStateOn
            Channel2LightStateVB_LightStateOn = 1,

            /// @~chinese 常灭   @~english LightStateOff
            Channel2LightStateVB_LightStateOff = 2,

        }Channel2LightStateVBEnum;

        /// @~chinese 通道2沿定义   @~english Channel 2 EdgeType
        typedef enum _Channel2EdgeTypeEnum
        {
            /// @~chinese 上升沿   @~english EdgeTypeRising
            Channel2EdgeType_EdgeTypeRising = 1,

            /// @~chinese 下降沿   @~english EdgeTypeFalling
            Channel2EdgeType_EdgeTypeFalling = 2,

        }Channel2EdgeTypeEnum;

        /// @~chinese 光源状态3   @~english Channel 3 Light State
        typedef enum _Channel3LightStateEnum
        {
            /// @~chinese 触发后亮（常灭）   @~english LightStateOn
            Channel3LightState_LightStateOn = 1,

            /// @~chinese 触发后灭（常亮）   @~english LightStateOff
            Channel3LightState_LightStateOff = 2,

        }Channel3LightStateEnum;

        /// @~chinese 通道3沿定义   @~english Channel 3 EdgeType
        typedef enum _Channel3EdgeTypeEnum
        {
            /// @~chinese 上升沿   @~english EdgeTypeRising
            Channel3EdgeType_EdgeTypeRising = 1,

            /// @~chinese 下降沿   @~english EdgeTypeFalling
            Channel3EdgeType_EdgeTypeFalling = 2,

        }Channel3EdgeTypeEnum;

        /// @~chinese 光源状态4   @~english Channel 4 Light State
        typedef enum _Channel4LightStateEnum
        {
            /// @~chinese 触发后亮（常灭）   @~english LightStateOn
            Channel4LightState_LightStateOn = 1,

            /// @~chinese 触发后灭（常亮）   @~english LightStateOff
            Channel4LightState_LightStateOff = 2,

        }Channel4LightStateEnum;

        /// @~chinese 通道4沿定义   @~english Channel 4 EdgeType
        typedef enum _Channel4EdgeTypeEnum
        {
            /// @~chinese 上升沿   @~english EdgeTypeRising
            Channel4EdgeType_EdgeTypeRising = 1,

            /// @~chinese 下降沿   @~english EdgeTypeFalling
            Channel4EdgeType_EdgeTypeFalling = 2,

        }Channel4EdgeTypeEnum;

    /// \addtogroup 光源
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the LightParams.  */
        ///光源参数类
        class LightParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit LightParams() {}
            virtual ~LightParams() {}

        public:

            /// @~chinese 通道1亮度(仅当次执行起效)   @~english Channel 1 Brightness(Only valid for this execution)
            virtual void SetInputChannel1Brightness(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 通道2亮度(仅当次执行起效)   @~english Channel 2 Brightness(Only valid for this execution)
            virtual void SetInputChannel2Brightness(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 通道3亮度(仅当次执行起效)   @~english Channel 3 Brightness(Only valid for this execution)
            virtual void SetInputChannel3Brightness(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 通道4亮度(仅当次执行起效)   @~english Channel 4 Brightness(Only valid for this execution)
            virtual void SetInputChannel4Brightness(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 通道5亮度(仅当次执行起效)   @~english Channel 5 Brightness(Only valid for this execution)
            virtual void SetInputChannel5Brightness(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 通道6亮度(仅当次执行起效)   @~english Channel 6 Brightness(Only valid for this execution)
            virtual void SetInputChannel6Brightness(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入字符(仅当次执行起效)   @~english InputString(Only valid for this execution)
            virtual void SetInputString(InputStringData* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输出类型   @~english Output Type
            __declspec(property(put = SetOutputType, get = GetOutputType)) OutputTypeEnum OutputType;

            virtual OutputTypeEnum GetOutputType() = 0;

            virtual void SetOutputType(OutputTypeEnum value) = 0;

            /// @~chinese 触发时间(ms)，范围：[-1,65535]   @~english Trigger Time(ms)，range:[-1,65535]
            __declspec(property(put = SetTriggerTime, get = GetTriggerTime)) int TriggerTime;

            virtual int GetTriggerTime() = 0;

            virtual void SetTriggerTime(int value) = 0;

            /// @~chinese 通道1使能   @~english Channel 1 Enable
            __declspec(property(put = SetChannel1Enable, get = GetChannel1Enable)) bool Channel1Enable;

            virtual bool GetChannel1Enable() = 0;

            virtual void SetChannel1Enable(bool value) = 0;

            /// @~chinese 通道1亮度，光源控制器:0~255; 视觉控制器:0~100   @~english Channel 1 Brightness，LightController:0~255; VisionController:0~100
            __declspec(property(put = SetChannel1Brightness, get = GetChannel1Brightness)) int Channel1Brightness;

            virtual int GetChannel1Brightness() = 0;

            virtual void SetChannel1Brightness(int value) = 0;

            /// @~chinese 光源状态1   @~english Channel 1 Light State
            __declspec(property(put = SetChannel1LightState, get = GetChannel1LightState)) Channel1LightStateEnum Channel1LightState;

            virtual Channel1LightStateEnum GetChannel1LightState() = 0;

            virtual void SetChannel1LightState(Channel1LightStateEnum value) = 0;

            /// @~chinese 光源状态1   @~english Channel 1 Light State
            __declspec(property(put = SetChannel1LightStateVB, get = GetChannel1LightStateVB)) Channel1LightStateVBEnum Channel1LightStateVB;

            virtual Channel1LightStateVBEnum GetChannel1LightStateVB() = 0;

            virtual void SetChannel1LightStateVB(Channel1LightStateVBEnum value) = 0;

            /// @~chinese 通道1沿定义   @~english Channel 1 EdgeType
            __declspec(property(put = SetChannel1EdgeType, get = GetChannel1EdgeType)) Channel1EdgeTypeEnum Channel1EdgeType;

            virtual Channel1EdgeTypeEnum GetChannel1EdgeType() = 0;

            virtual void SetChannel1EdgeType(Channel1EdgeTypeEnum value) = 0;

            /// @~chinese 持续时间(ms)，范围：[0,30000]   @~english Duration(ms)，range:[0,30000]
            __declspec(property(put = SetChannel1DurationTime, get = GetChannel1DurationTime)) int Channel1DurationTime;

            virtual int GetChannel1DurationTime() = 0;

            virtual void SetChannel1DurationTime(int value) = 0;

            /// @~chinese 通道2使能   @~english Channel 2 Enable
            __declspec(property(put = SetChannel2Enable, get = GetChannel2Enable)) bool Channel2Enable;

            virtual bool GetChannel2Enable() = 0;

            virtual void SetChannel2Enable(bool value) = 0;

            /// @~chinese 通道2亮度，光源控制器:0~255; 视觉控制器:0~100   @~english Channel 2 Brightness，LightController:0~255; VisionController:0~100
            __declspec(property(put = SetChannel2Brightness, get = GetChannel2Brightness)) int Channel2Brightness;

            virtual int GetChannel2Brightness() = 0;

            virtual void SetChannel2Brightness(int value) = 0;

            /// @~chinese 光源状态2   @~english Channel 2 Light State
            __declspec(property(put = SetChannel2LightState, get = GetChannel2LightState)) Channel2LightStateEnum Channel2LightState;

            virtual Channel2LightStateEnum GetChannel2LightState() = 0;

            virtual void SetChannel2LightState(Channel2LightStateEnum value) = 0;

            /// @~chinese 光源状态2   @~english Channel 2 Light State
            __declspec(property(put = SetChannel2LightStateVB, get = GetChannel2LightStateVB)) Channel2LightStateVBEnum Channel2LightStateVB;

            virtual Channel2LightStateVBEnum GetChannel2LightStateVB() = 0;

            virtual void SetChannel2LightStateVB(Channel2LightStateVBEnum value) = 0;

            /// @~chinese 通道2沿定义   @~english Channel 2 EdgeType
            __declspec(property(put = SetChannel2EdgeType, get = GetChannel2EdgeType)) Channel2EdgeTypeEnum Channel2EdgeType;

            virtual Channel2EdgeTypeEnum GetChannel2EdgeType() = 0;

            virtual void SetChannel2EdgeType(Channel2EdgeTypeEnum value) = 0;

            /// @~chinese 持续时间(ms)，范围：[0,30000]   @~english Duration(ms)，range:[0,30000]
            __declspec(property(put = SetChannel2DurationTime, get = GetChannel2DurationTime)) int Channel2DurationTime;

            virtual int GetChannel2DurationTime() = 0;

            virtual void SetChannel2DurationTime(int value) = 0;

            /// @~chinese 通道3使能   @~english Channel 3 Enable
            __declspec(property(put = SetChannel3Enable, get = GetChannel3Enable)) bool Channel3Enable;

            virtual bool GetChannel3Enable() = 0;

            virtual void SetChannel3Enable(bool value) = 0;

            /// @~chinese 通道3亮度，光源控制器:0~255; 视觉控制器:0~100   @~english Channel 3 Brightness，LightController:0~255; VisionController:0~100
            __declspec(property(put = SetChannel3Brightness, get = GetChannel3Brightness)) int Channel3Brightness;

            virtual int GetChannel3Brightness() = 0;

            virtual void SetChannel3Brightness(int value) = 0;

            /// @~chinese 光源状态3   @~english Channel 3 Light State
            __declspec(property(put = SetChannel3LightState, get = GetChannel3LightState)) Channel3LightStateEnum Channel3LightState;

            virtual Channel3LightStateEnum GetChannel3LightState() = 0;

            virtual void SetChannel3LightState(Channel3LightStateEnum value) = 0;

            /// @~chinese 通道3沿定义   @~english Channel 3 EdgeType
            __declspec(property(put = SetChannel3EdgeType, get = GetChannel3EdgeType)) Channel3EdgeTypeEnum Channel3EdgeType;

            virtual Channel3EdgeTypeEnum GetChannel3EdgeType() = 0;

            virtual void SetChannel3EdgeType(Channel3EdgeTypeEnum value) = 0;

            /// @~chinese 持续时间(ms)，范围：[0,30000]   @~english Duration(ms)，range:[0,30000]
            __declspec(property(put = SetChannel3DurationTime, get = GetChannel3DurationTime)) int Channel3DurationTime;

            virtual int GetChannel3DurationTime() = 0;

            virtual void SetChannel3DurationTime(int value) = 0;

            /// @~chinese 通道4使能   @~english Channel 4 Enable
            __declspec(property(put = SetChannel4Enable, get = GetChannel4Enable)) bool Channel4Enable;

            virtual bool GetChannel4Enable() = 0;

            virtual void SetChannel4Enable(bool value) = 0;

            /// @~chinese 通道4亮度，光源控制器:0~255; 视觉控制器:0~100   @~english Channel 4 Brightness，LightController:0~255; VisionController:0~100
            __declspec(property(put = SetChannel4Brightness, get = GetChannel4Brightness)) int Channel4Brightness;

            virtual int GetChannel4Brightness() = 0;

            virtual void SetChannel4Brightness(int value) = 0;

            /// @~chinese 光源状态4   @~english Channel 4 Light State
            __declspec(property(put = SetChannel4LightState, get = GetChannel4LightState)) Channel4LightStateEnum Channel4LightState;

            virtual Channel4LightStateEnum GetChannel4LightState() = 0;

            virtual void SetChannel4LightState(Channel4LightStateEnum value) = 0;

            /// @~chinese 通道4沿定义   @~english Channel 4 EdgeType
            __declspec(property(put = SetChannel4EdgeType, get = GetChannel4EdgeType)) Channel4EdgeTypeEnum Channel4EdgeType;

            virtual Channel4EdgeTypeEnum GetChannel4EdgeType() = 0;

            virtual void SetChannel4EdgeType(Channel4EdgeTypeEnum value) = 0;

            /// @~chinese 持续时间(ms)，范围：[0,30000]   @~english Duration(ms)，range:[0,30000]
            __declspec(property(put = SetChannel4DurationTime, get = GetChannel4DurationTime)) int Channel4DurationTime;

            virtual int GetChannel4DurationTime() = 0;

            virtual void SetChannel4DurationTime(int value) = 0;

            /// @~chinese 通道5使能   @~english Channel 5 Enable
            __declspec(property(put = SetChannel5Enable, get = GetChannel5Enable)) bool Channel5Enable;

            virtual bool GetChannel5Enable() = 0;

            virtual void SetChannel5Enable(bool value) = 0;

            /// @~chinese 通道5亮度，光源控制器:0~255; 视觉控制器:0~100   @~english Channel 5 Brightness，LightController:0~255; VisionController:0~100
            __declspec(property(put = SetChannel5Brightness, get = GetChannel5Brightness)) int Channel5Brightness;

            virtual int GetChannel5Brightness() = 0;

            virtual void SetChannel5Brightness(int value) = 0;

            /// @~chinese 通道6使能   @~english Channel 6 Enable
            __declspec(property(put = SetChannel6Enable, get = GetChannel6Enable)) bool Channel6Enable;

            virtual bool GetChannel6Enable() = 0;

            virtual void SetChannel6Enable(bool value) = 0;

            /// @~chinese 通道6亮度，光源控制器:0~255; 视觉控制器:0~100   @~english Channel 6 Brightness，LightController:0~255; VisionController:0~100
            __declspec(property(put = SetChannel6Brightness, get = GetChannel6Brightness)) int Channel6Brightness;

            virtual int GetChannel6Brightness() = 0;

            virtual void SetChannel6Brightness(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/

        /// @~chinese 光源通道信息结构 @~english Structure about information of light channel
        typedef struct LightChannelInfo
        {
            int    iEnable;                                          ///< @~chinese 使能 @~english Enable
            int    iBrightness;                                      ///< @~chinese 亮度 @~english Brightness
            int    iLightState;                                      ///< @~chinese 光源状态 @~english Light state
            int    iEdgeType;                                        ///< @~chinese 沿定义 @~english Edge type
            int    iDurationTime;                                    ///< @~chinese 持续时间 @~english Duration time
        } LightChannelInfo;

        /*  Note: Interface Classes of the result of the LightResults.  */
        ///光源结果类
        class LightResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit LightResults() {}
            virtual ~LightResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

			 /**
             * @brief 通道1信息
             */
             virtual LightChannelInfo GetChannel1Info() = 0;

             /**
             * @brief 通道2信息
             */
             virtual LightChannelInfo GetChannel2Info() = 0;

             /**
             * @brief 通道3信息
             */
             virtual LightChannelInfo GetChannel3Info() = 0;

             /**
             * @brief 通道4信息
             */
             virtual LightChannelInfo GetChannel4Info() = 0;

             /**
             * @brief 通道5信息
             */
             virtual LightChannelInfo GetChannel5Info() = 0;

             /**
             * @brief 通道6信息
             */
             virtual LightChannelInfo GetChannel6Info() = 0;

        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the Light tool.  */
        ///光源工具类
        class LightModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit LightModuleTool() {}
            virtual ~LightModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual LightParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual LightResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            LightModuleTool(LightModuleTool&);
            LightModuleTool& operator=(LightModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for Light.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::LightModule::LightModuleTool * __stdcall GetLightToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_LIGHT_H_
