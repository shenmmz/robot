/***************************************************************************************************
 * File：IVmDataFilter.h
 * Note：@~chinese 筛选模块模块接口声明 @~english Interface for the DataFilter tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_DATAFILTER_H_
#define _IVM_DATAFILTER_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace DataFilterModule
    {
    /// \addtogroup 筛选模块
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the DataFilterParams.  */
        ///筛选模块参数类
        class DataFilterParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit DataFilterParams() {}
            virtual ~DataFilterParams() {}

        public:
        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the DataFilterResults.  */
        ///筛选模块结果类
        class DataFilterResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit DataFilterResults() {}
            virtual ~DataFilterResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
             /**
             * @brief 数据个数
             */
             virtual int GetDataNum() = 0;
             /**
             * @brief 获取整型输出
             * @param strParamName [IN] 参数名称
             * @return 整型数组，出错时抛出异常
             */
             virtual IntDataArray GetOutputInt(IN const char * const strParamName) = 0;

             /**
             * @brief 获取浮点型输出
             * @param strParamName [IN] 参数名称
             * @return 浮点型数组，出错时抛出异常
             */
             virtual FloatDataArray GetOutputFloat(IN const char * const strParamName) = 0;

             /**
             * @brief 获取点输出数组
             * @param strParamName  [IN]  参数名称
             * @return 点输出数组
             */
             virtual IoPointArray GetOutputPointArray(IN const char * const strParamName) = 0;

             /**
             * @brief 获取直线输出数组
             * @param strParamName  [IN]  参数名称
             * @return 直线输出数组
             */
             virtual IoLineArray GetOutputLineArray(IN const char * const strParamName) = 0;

             /**
             * @brief 获取椭圆输出数组
             * @param strParamName  [IN]  参数名称
             */
             virtual IoEllipseArray GetOutputEllipseArray(IN const char * const strParamName) = 0;

             /**
             * @brief 获取带角度矩形输出数组
             * @param strParamName  [IN]  参数名称
             * @return 带角度矩形输出数组
             */
             virtual IoRectBoxArray GetOutputBoxArray(IN const char * const strParamName) = 0;

             /**
             * @brief 获取无角度矩形（浮点型）输出数组
             * @param strParamName  [IN]  参数名称
             * @return 无角度矩形（浮点型）输出数组
             */
             virtual IoRectFArray GetOutputRectArray(IN const char * const strParamName) = 0;

             /**
             * @brief 获取圆环输出数组
             * @param strParamName  [IN]  参数名称
             * @return 圆环输出数组
             */
             virtual IoAnnulusArray GetOutputAnnulusArray(IN const char * const strParamName) = 0;

             /**
             * @brief 获取多边形输出数组
             * @param strParamName  [IN]  参数名称
             * @return 多边形输出数组
             */
             virtual IoPolygonArray GetOutputPolygonArray(IN const char * const strParamName) = 0;

        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the DataFilter tool.  */
        ///筛选模块工具类
        class DataFilterModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit DataFilterModuleTool() {}
            virtual ~DataFilterModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
              * @remarks 禁止使用参数基类的方法
             */
            virtual DataFilterParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual DataFilterResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            DataFilterModuleTool(DataFilterModuleTool&);
            DataFilterModuleTool& operator=(DataFilterModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
#endif    ///< _IVM_DATAFILTER_H_
