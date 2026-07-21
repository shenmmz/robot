/***************************************************************************************************
 * File：IVmPyShell.h
 * Note：@~chinese Python脚本模块接口声明 @~english Interface for the PyShell tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_PYSHELL_H_
#define _IVM_PYSHELL_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace PyShellModule
    {
    /// \addtogroup Python脚本
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the PyShellParams.  */
        ///Python脚本参数类
        class PyShellParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit PyShellParams() {}
            virtual ~PyShellParams() {}

        public:
            /**
            *  @brief  设置动态输入图像数据（仅当次执行起效）
            *  @param  strParamName   [IN]参数名称
            *  @param  pstImageData   [IN]IoImage结构体指针数组
            */
            virtual void SetInputImage(const char * const strParamName, const IoImage * const pstImageData) = 0;
            /**
            *  @brief  设置动态输入Int数据（仅当次执行起效）
            *  @param  strParamName    [IN]参数名称
            *  @param  anIntVal        [IN]int型指针数组
            *  @param  nValNum         [IN]数组长度
            */
            virtual void SetInputInt(const char * const strParamName, const int * const anIntVal, const int nValNum) = 0;
            /**
            *  @brief  设置动态输入Float数据（仅当次执行起效）
            *  @param  strParamName    [IN]参数名称
            *  @param  anFloatVal      [IN]Float型指针数组
            *  @param  nValNum         [IN]数组长度
            */
            virtual void SetInputFloat(const char * const strParamName, const float * const anFloatVal, const int nValNum) = 0;
            /**
            *  @brief  设置动态输入String数据（仅当次执行起效）
            *  @param  strParamName   [IN]参数名称
            *  @param  astStrData     [IN]InputStringData结构体指针数组
            *  @param  nValNum        [IN]数组长度
            */
            virtual void SetInputString(const char * const strParamName, const InputStringData * const astStrData, const int nValNum) = 0;
            /**
            *  @brief  设置动态输入二进制数据（仅当次执行起效）
            *  @param  strParamName    [IN]参数名称
            *  @param  pBytes          [IN]BytesData结构体指针
            */
            virtual void SetInputBytes(const char * const strParamName, const BytesData * const pBytes) = 0;

            /**
            *  @brief  设置动态输入矩形框数据（仅当次执行起效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstBox        [IN] IoRectBoxArray结构体指针数组
            */
            virtual void SetInputBox(const char * const strParamName, const IoRectBoxArray * const pstBox) = 0;

            /**
            *  @brief  设置动态输入圆环数据（仅当次执行起效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstAnnulus    [IN] IoAnnulusArray结构体指针数组
            */
            virtual void SetInputAnnulus(const char * const strParamName, const IoAnnulusArray * const pstAnnulus) = 0;

            /**
            *  @brief  设置动态输入椭圆形数据（仅当次执行起效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstCircle     [IN] IoCircleArray结构体指针数组
            */
            virtual void SetInputEllipse(const char * const strParamName, const IoEllipseArray * const pstEllipse) = 0;

            /**
            *  @brief  设置动态输入线型数据（仅当次执行起效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstLine       [IN] IoLineArray结构体指针数组
            */
            virtual void SetInputLine(const char * const strParamName, const IoLineArray * const pstLine) = 0;

            /**
            *  @brief  设置动态输入点型数据（仅当次执行起效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstPointF     [IN] IoPointArray结构体指针数组
            */
            virtual void SetInputPoint(const char * const strParamName, const IoPointArray * const pstPointF) = 0;

            /**
            *  @brief  设置动态输入位置修正数据（仅当次执行起效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstFixture    [IN] IoFixtureArray结构体指针数组
            */
            virtual void SetInputFixture(const char * const strParamName, const IoFixtureArray * const pstFixture) = 0;

            /**
            *  @brief  设置动态输入矩形数据（仅当次执行起效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstRectF      [IN] IoRectFArray结构体指针数组
            */
            virtual void SetInputRect(const char * const strParamName, const IoRectFArray * const pstRectF) = 0;

            /**
            *  @brief  设置动态输入多边形数据（仅当次执行起效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstPolygon    [IN] IoPolygonArray结构体指针数组
            */
            virtual void SetInputPolygon(const char * const strParamName, const IoPolygonArray * const pstPolygon) = 0;
        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the PyShellResults.  */
        ///Python脚本结果类
        class PyShellResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit PyShellResults() {}
            virtual ~PyShellResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
             /**
             * @brief 获取整型输入
             * @param strParamName [IN] 参数名称
             * @return 整型数组，出错时抛出异常
             */
             virtual IntDataArray GetOutputInt(IN const char * const strParamName) = 0;

             /**
             * @brief 获取浮点型输入
             * @param strParamName [IN] 参数名称
             * @return 浮点型数组，出错时抛出异常
             */
             virtual FloatDataArray GetOutputFloat(IN const char * const strParamName) = 0;

             /**
             * @brief 获取字符串型输入
             * @param strParamName [IN] 参数名称
             * @return 字符串型数组，出错时抛出异常
             */
             virtual StringDataArray GetOutputString(IN const char * const strParamName) = 0;

             /**
             * @brief 获取二进制数据型输入
             * @param strParamName [IN] 参数名称
             * @return 二进制数据，出错时抛出异常
             */
             virtual ByteArrayData GetOutputByteArray(IN const char * const strParamName) = 0;

             /**
             * @brief 获取图像类型输入
             * @param strParamName [IN] 参数名称
             * @return 图像数据，出错时抛出异常
             */
             virtual IoImage GetOutputImage(IN const char * const strParamName) = 0;

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
             * @brief 获取位置修正输出数组
             * @param strParamName  [IN]  参数名称
             * @return 位置修正输出数组
             */
             virtual IoFixtureArray GetOutputFixtureArray(IN const char * const strParamName) = 0;

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
        /*  Note: Note: This is the class for the PyShell tool.  */
        ///Python脚本工具类
        class PyShellModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit PyShellModuleTool() {}
            virtual ~PyShellModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual PyShellParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual PyShellResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            PyShellModuleTool(PyShellModuleTool&);
            PyShellModuleTool& operator=(PyShellModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
#endif    ///< _IVM_PYSHELL_H_
