/***************************************************************************************************
 * File：IVmGeometryCreate.h
 * Note：@~chinese 几何创建模块接口声明 @~english Interface for the GeometryCreate tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_GEOMETRYCREATE_H_
#define _IVM_GEOMETRYCREATE_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace GeometryCreate
    {
    /// \addtogroup 几何创建
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the GeometryCreateParams.  */
        ///几何创建参数类
        class GeometryCreateParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit GeometryCreateParams() {}
            virtual ~GeometryCreateParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入点(仅当次执行起效)   @~english Input Point(Only valid for this execution)
            virtual void SetInputPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入直线(仅当次执行起效)   @~english Input Line(Only valid for this execution)
            virtual void SetInputLine(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入圆(仅当次执行起效)   @~english Input Circle(Only valid for this execution)
            virtual void SetInputCircle(Annulus* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入Box(仅当次执行起效)   @~english Input Box (Only valid for this execution)
            virtual void SetInputBox(RectBox* pInputValue, unsigned int nValueNum) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the GeometryCreateResults.  */
        ///几何创建结果类
        class GeometryCreateResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit GeometryCreateResults() {}
            virtual ~GeometryCreateResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

            /**
             * @brief 输出点
             */
            virtual MVD_POINT_F GetOutputPoint() = 0;

            /**
             * @brief 输出直线
             */
            virtual Line GetOutputLine() = 0;

            /**
             * @brief 输出直线角度
            */
            virtual float GetOutputLineAngle() = 0;

            /**
             * @brief 输出圆
             */
            virtual Circle GetOutputCircle() = 0;

            /**
             * @brief 点个数
             */
             virtual int GetOutputPointNum() = 0;

            /**
             * @brief 输出点
             * @param nIndex [IN] 输入点索引，[0,OutputPointNum)
             */
            virtual MVD_POINT_F GetOutputPointAt(int nIndex = 0) = 0;

            /**
             * @brief 点个数
             */
            virtual int GetLineNum() = 0;

            /**
             * @brief 输出直线
             * @param nIndex [IN] 输入点索引，[0,LineNum)
             */
            virtual Line GetOutputLineAt(int nIndex = 0) = 0;

            /**
            * @brief 输出直线角度
            * @param nIndex [IN] 输入点索引，[0,LineNum)
            */
            virtual float GetOutputLineAngleAt(int nIndex = 0) = 0;

            /**
             * @brief 圆个数
             */
            virtual int GetCircleNum() = 0;

            /**
             * @brief 输出圆
             * @param nIndex [IN] 输入点索引，[0,CircleNum)
             */
            virtual Annulus GetOutputCircleAt(int nIndex = 0) = 0;

            /**
             * @brief 矩形个数
             */
            virtual int GetBoxNum() = 0;

            /**
             * @brief 输出矩形
             * @param nIndex [IN] 输入点索引，[0,BoxNum)
             */
            virtual RectBox GetOutputResultBoxAt(int nIndex = 0) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the GeometryCreate tool.  */
        ///几何创建工具类
        class GeometryCreateTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit GeometryCreateTool() {}
            virtual ~GeometryCreateTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual GeometryCreateParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual GeometryCreateResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            GeometryCreateTool(GeometryCreateTool&);
            GeometryCreateTool& operator=(GeometryCreateTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for GeometryCreate.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::GeometryCreate::GeometryCreateTool * __stdcall GetGeometryCreateToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_GEOMETRYCREATE_H_
