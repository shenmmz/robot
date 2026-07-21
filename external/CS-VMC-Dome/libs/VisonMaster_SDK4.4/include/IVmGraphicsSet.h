/***************************************************************************************************
 * File：IVmGraphicsSet.h
 * Note：@~chinese 图形收集模块接口声明 @~english Interface for the GraphicsSet tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_GRAPHICSSET_H_
#define _IVM_GRAPHICSSET_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"
#include "IVmDynamicIoDefine.h"

namespace VisionMasterSDK
{
    namespace GraphicsSetModule
    {
        /// 文本信息
        typedef struct TextInfo
        {
            /// @~chinese 文本位置 @~english Text Position
            PointF stPosition;

            /// @~chinese 字符串数据 @~english String
            char strValue[MAX_MODULE_RESVALUE_LENGTH];

            /// @~chinese 保留字段 @~english Reserved
            char strReserved[32];

        } TextInfo;

    /// \addtogroup 图形收集
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the GraphicsSetParams.  */
        ///图形收集参数类
        class GraphicsSetParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit GraphicsSetParams() {}
            virtual ~GraphicsSetParams() {}

        public:

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the GraphicsSetResults.  */
        ///图形收集结果类
        class GraphicsSetResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit GraphicsSetResults() {}
            virtual ~GraphicsSetResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

            /**
            * @brief 获取点输出
            * @param strParamName [IN] 参数名称
            * @return 无，出错时抛出异常
            */
            virtual IoPointArray GetOutputPointArray(IN const char * const strParamName) = 0;
            /**
            * @brief 获取直线输出
            * @param strParamName [IN] 参数名称
            * @return 无，出错时抛出异常
            */
            virtual IoLineArray GetOutputLineArray(IN const char * const strParamName) = 0;
            /**
            * @brief 获取矩形输出
            * @param strParamName [IN] 参数名称
            * @return 无，出错时抛出异常
            */
            virtual IoRectBoxArray GetOutputBoxArray(IN const char * const strParamName) = 0;
            /**
            * @brief 获取圆输出
            * @param strParamName [IN] 参数名称
            * @return 无，出错时抛出异常
            */
            virtual IoAnnulusArray GetOutputAnnulusArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取椭圆输出
            * @param strParamName [IN] 参数名称
            * @return 无，出错时抛出异常
            */
            virtual IoEllipseArray GetOutputEllipseArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取文本输出
            * @param strParamName [IN] 参数名称
            * @return 无，出错时抛出异常
            */
            virtual StringDataArray GetOutputString(IN const char * const strParamName) = 0;

            /**
            * @brief 获取输出状态
            * @param strParamName [IN] 参数名称
            * @return 无，出错时抛出异常
            */
            virtual IntDataArray GetOutputInt(IN const char * const strParamName) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the GraphicsSet tool.  */
        ///图形收集工具类
        class GraphicsSetModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit GraphicsSetModuleTool() {}
            virtual ~GraphicsSetModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual GraphicsSetParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual GraphicsSetResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            GraphicsSetModuleTool(GraphicsSetModuleTool&);
            GraphicsSetModuleTool& operator=(GraphicsSetModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for GraphicsSet.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::GraphicsSetModule::GraphicsSetModuleTool * __stdcall GetGraphicsSetToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_GRAPHICSSET_H_
