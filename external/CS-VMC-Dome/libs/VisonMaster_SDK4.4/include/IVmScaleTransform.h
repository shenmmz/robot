/***************************************************************************************************
 * File：IVmScaleTransform.h
 * Note：@~chinese 单位转换模块接口声明 @~english Interface for the ScaleTransform tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_SCALETRANSFORM_H_
#define _IVM_SCALETRANSFORM_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSScaleTransformModu
    {
    /// \addtogroup 单位转换
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the ScaleTransformParams.  */
        ///单位转换参数类
        class ScaleTransformParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ScaleTransformParams() {}
            virtual ~ScaleTransformParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 像素间距(仅当次执行起效)   @~english ImageDist(Only valid for this execution)
            virtual void SetImageDist(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 刷新信号(仅当次执行起效)   @~english Refresh Signal(Only valid for this execution)
            virtual void SetRefreshSignal(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 像素当量修正系数(仅当次执行起效)   @~english Pixel Equivalent Correct Info(Only valid for this execution)
            virtual void SetPixelEquivalentCorrectInfo(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 加载标定文件   @~english Load Calibration File
            __declspec(property(put = SetLoadCalibPath, get = GetLoadCalibPath)) const char* LoadCalibPath;

            virtual const char* GetLoadCalibPath() = 0;

            virtual void SetLoadCalibPath(const char* value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ScaleTransformResults.  */
        ///单位转换结果类
        class ScaleTransformResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ScaleTransformResults() {}
            virtual ~ScaleTransformResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 单像素精度
             */
            virtual float GetPixelScale() = 0;

            /**
             * @brief 转换结果
             */
            virtual float GetPhyDist() = 0;

            /**
             * @brief 像素距离
             */
            virtual float GetPixDist() = 0;

            /**
             * @brief 平移X
             */
            virtual float GetTranslateX() = 0;

            /**
             * @brief 平移Y
             */
            virtual float GetTranslateY() = 0;

            /**
             * @brief 旋转
             */
            virtual float GetRotate() = 0;

            /**
             * @brief 尺度
             */
            virtual float GetScale() = 0;

            /**
             * @brief 斜切
             */
            virtual float GetSkew() = 0;

            /**
             * @brief 宽高比
             */
            virtual float GetAspect() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ScaleTransform tool.  */
        ///单位转换工具类
        class IMVSScaleTransformModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSScaleTransformModuTool() {}
            virtual ~IMVSScaleTransformModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ScaleTransformParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ScaleTransformResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSScaleTransformModuTool(IMVSScaleTransformModuTool&);
            IMVSScaleTransformModuTool& operator=(IMVSScaleTransformModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ScaleTransform.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSScaleTransformModu::IMVSScaleTransformModuTool * __stdcall GetScaleTransformToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_SCALETRANSFORM_H_
