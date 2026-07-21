/***************************************************************************************************
 * File：IVmCoordinateTransform.h
 * Note：@~chinese 坐标转换模块接口声明 @~english Interface for the CoordinateTransform tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_COORDINATETRANSFORM_H_
#define _IVM_COORDINATETRANSFORM_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace CoordinateTransform
    {
    /// \addtogroup 坐标转换
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the CoordinateTransformParams.  */
        ///坐标转换参数类
        class CoordinateTransformParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CoordinateTransformParams() {}
            virtual ~CoordinateTransformParams() {}

        public:

            /// @~chinese 划片中心点(仅当次执行起效)   @~english Dicing Center Point(Only valid for this execution)
            virtual void SetDicingPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 划片宽(仅当次执行起效)   @~english Dicing Width(Only valid for this execution)
            virtual void SetDicingImageWidth(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 划片高(仅当次执行起效)   @~english Dicing Height(Only valid for this execution)
            virtual void SetDicingImageHeight(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入点(仅当次执行起效)   @~english Input Point(Only valid for this execution)
            virtual void SetSrcInputPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 坐标转换使能   @~english Coordinate Conversion
            __declspec(property(put = SetCorrectEnable, get = GetCorrectEnable)) bool CorrectEnable;

            virtual bool GetCorrectEnable() = 0;

            virtual void SetCorrectEnable(bool value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the CoordinateTransformResults.  */
        ///坐标转换结果类
        class CoordinateTransformResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CoordinateTransformResults() {}
            virtual ~CoordinateTransformResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

            /**
            * @brief 输出点数量
            */
            virtual int GetDstPointNum() = 0;
            /**
            * @brief 输出点
            */
            virtual PointF GetDstPointAt(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CoordinateTransform tool.  */
        ///坐标转换工具类
        class CoordinateTransformTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CoordinateTransformTool() {}
            virtual ~CoordinateTransformTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CoordinateTransformParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CoordinateTransformResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            CoordinateTransformTool(CoordinateTransformTool&);
            CoordinateTransformTool& operator=(CoordinateTransformTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CoordinateTransform.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::CoordinateTransform::CoordinateTransformTool * __stdcall GetCoordinateTransformToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_COORDINATETRANSFORM_H_
