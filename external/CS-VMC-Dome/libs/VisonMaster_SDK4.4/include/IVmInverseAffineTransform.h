/***************************************************************************************************
 * File：IVmInverseAffineTransform.h
 * Note：@~chinese 逆仿射变换模块接口声明 @~english Interface for the InverseAffineTransform tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_INVERSEAFFINETRANSFORM_H_
#define _IVM_INVERSEAFFINETRANSFORM_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSInverseAffineTransformModu
    {
    /// \addtogroup 逆仿射变换
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///逆仿射变换ROI管理类
        class InverseAffineTransformRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the InverseAffineTransformParams.  */
        ///逆仿射变换参数类
        class InverseAffineTransformParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit InverseAffineTransformParams() {}
            virtual ~InverseAffineTransformParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入底图(仅当次执行起效)   @~english Input Back Image(Only valid for this execution)
            virtual void SetInputImageBack(ImageBaseData* pInputValue) = 0;

            /// @~chinese 图像宽度(仅当次执行起效)   @~english Image Width(Only valid for this execution)
            virtual void SetImagewidth(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 图像高度(仅当次执行起效)   @~english Image Height(Only valid for this execution)
            virtual void SetImageheight(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual InverseAffineTransformRoiManager* GetModuRoiManager() = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the InverseAffineTransformResults.  */
        ///逆仿射变换结果类
        class InverseAffineTransformResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit InverseAffineTransformResults() {}
            virtual ~InverseAffineTransformResults() {}

        public:
             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

			 /**
			 * @brief 获取输出图像
			 */
			 virtual ImageBaseData GetOutputImage() = 0;

        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the InverseAffineTransform tool.  */
        ///逆仿射变换工具类
        class IMVSInverseAffineTransformModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSInverseAffineTransformModuTool() {}
            virtual ~IMVSInverseAffineTransformModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual InverseAffineTransformParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual InverseAffineTransformResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSInverseAffineTransformModuTool(IMVSInverseAffineTransformModuTool&);
            IMVSInverseAffineTransformModuTool& operator=(IMVSInverseAffineTransformModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for InverseAffineTransform.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSInverseAffineTransformModu::IMVSInverseAffineTransformModuTool * __stdcall GetInverseAffineTransformToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_INVERSEAFFINETRANSFORM_H_
