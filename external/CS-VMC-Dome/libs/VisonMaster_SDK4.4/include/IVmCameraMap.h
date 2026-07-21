/***************************************************************************************************
 * File：IVmCameraMap.h
 * Note：@~chinese 相机映射模块接口声明 @~english Interface for the CameraMap tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CAMERAMAP_H_
#define _IVM_CAMERAMAP_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCameraMapModu
    {
        /// @~chinese 自由度   @~english DOF
        typedef enum _HomoFreedomEnum
        {
            /// @~chinese 缩放、旋转、纵横比、倾斜、平移及透射   @~english Scale、Rotation、Aspect Ratio、Tilt、Translation and Transmission
            HomoFreedom_Perspective = 0x1,

            /// @~chinese 缩放、旋转、纵横比、倾斜及平移   @~english Scale、Rotation、Aspect Ratio、Tilt and Translation
            HomoFreedom_Affine = 0x2,

            /// @~chinese 缩放、旋转及平移   @~english Scale、Rotation and Translation
            HomoFreedom_Similarity = 0x3,

        }HomoFreedomEnum;

        /// @~chinese 权重函数   @~english Weighting Function
        typedef enum _WeightFunEnum
        {
            /// @~chinese 最小二乘   @~english Least Squares
            WeightFun_LeastSqure = 0x1,

            /// @~chinese Huber   @~english Huber
            WeightFun_Huber = 0x2,

            /// @~chinese Tukey   @~english Tukey
            WeightFun_Tukey = 0x3,

        }WeightFunEnum;

    /// \addtogroup 相机映射
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the CameraMapParams.  */
        ///相机映射参数类
        class CameraMapParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CameraMapParams() {}
            virtual ~CameraMapParams() {}

        public:

            /// @~chinese 目标点输入1(仅当次执行起效)   @~english TargetPointInput1(Only valid for this execution)
            virtual void SetTargetPointInput1(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象点输入1(仅当次执行起效)   @~english ObjectPointInput1(Only valid for this execution)
            virtual void SetObjectPointInput1(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标点输入2(仅当次执行起效)   @~english TargetPointInput2(Only valid for this execution)
            virtual void SetTargetPointInput2(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象点输入2(仅当次执行起效)   @~english ObjectPointInput2(Only valid for this execution)
            virtual void SetObjectPointInput2(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标点输入3(仅当次执行起效)   @~english TargetPointInput3(Only valid for this execution)
            virtual void SetTargetPointInput3(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象点输入3(仅当次执行起效)   @~english ObjectPointInput3(Only valid for this execution)
            virtual void SetObjectPointInput3(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标点输入4(仅当次执行起效)   @~english TargetPointInput4(Only valid for this execution)
            virtual void SetTargetPointInput4(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象点输入4(仅当次执行起效)   @~english ObjectPointInput4(Only valid for this execution)
            virtual void SetObjectPointInput4(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标点输入5(仅当次执行起效)   @~english TargetPointInput5(Only valid for this execution)
            virtual void SetTargetPointInput5(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象点输入5(仅当次执行起效)   @~english ObjectPointInput5(Only valid for this execution)
            virtual void SetObjectPointInput5(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标点输入6(仅当次执行起效)   @~english TargetPointInput6(Only valid for this execution)
            virtual void SetTargetPointInput6(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象点输入6(仅当次执行起效)   @~english ObjectPointInput6(Only valid for this execution)
            virtual void SetObjectPointInput6(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标点输入7(仅当次执行起效)   @~english TargetPointInput7(Only valid for this execution)
            virtual void SetTargetPointInput7(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象点输入7(仅当次执行起效)   @~english ObjectPointInput7(Only valid for this execution)
            virtual void SetObjectPointInput7(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标点输入8(仅当次执行起效)   @~english TargetPointInput8(Only valid for this execution)
            virtual void SetTargetPointInput8(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象点输入8(仅当次执行起效)   @~english ObjectPointInput8(Only valid for this execution)
            virtual void SetObjectPointInput8(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 标定文件路径   @~english Set Calibration File Path
            __declspec(property(put = SetCalibPathName, get = GetCalibPathName)) const char* CalibPathName;

            virtual const char* GetCalibPathName() = 0;

            virtual void SetCalibPathName(const char* value) = 0;

            /// @~chinese 更新文件   @~english Update File
            __declspec(property(put = SetRefreshFileEnable, get = GetRefreshFileEnable)) bool RefreshFileEnable;

            virtual bool GetRefreshFileEnable() = 0;

            virtual void SetRefreshFileEnable(bool value) = 0;

            /// @~chinese 自由度   @~english DOF
            __declspec(property(put = SetHomoFreedom, get = GetHomoFreedom)) HomoFreedomEnum HomoFreedom;

            virtual HomoFreedomEnum GetHomoFreedom() = 0;

            virtual void SetHomoFreedom(HomoFreedomEnum value) = 0;

            /// @~chinese 权重函数   @~english Weighting Function
            __declspec(property(put = SetWeightFun, get = GetWeightFun)) WeightFunEnum WeightFun;

            virtual WeightFunEnum GetWeightFun() = 0;

            virtual void SetWeightFun(WeightFunEnum value) = 0;

            /// @~chinese 权重系数，范围：[1,200]   @~english Weighting Coefficient，range:[1,200]
            __declspec(property(put = SetWeightFactor, get = GetWeightFactor)) int WeightFactor;

            virtual int GetWeightFactor() = 0;

            virtual void SetWeightFactor(int value) = 0;

            /// @~chinese 生成标定文件   @~english Create Calibration File
            virtual int DoSaveFile(IN const char * const strValue) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the CameraMapResults.  */
        ///相机映射结果类
        class CameraMapResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CameraMapResults() {}
            virtual ~CameraMapResults() {}

        public:
            /**
             * @brief X方向比例
             */
            virtual float GetRatioX() = 0;
            /**
             * @brief Y方向比例
             */
            virtual float GetRatioY() = 0;
            /**
             * @brief 相机对位的HOMO矩阵
             */
            virtual void GetTargetMatrix(float arrMatrix[9]) = 0;
            /**
             * @brief 标定状态
             */
            virtual int GetCalibStatus() = 0;
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
        /*  Note: Note: This is the class for the CameraMap tool.  */
        ///相机映射工具类
        class IMVSCameraMapModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCameraMapModuTool() {}
            virtual ~IMVSCameraMapModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CameraMapParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CameraMapResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSCameraMapModuTool(IMVSCameraMapModuTool&);
            IMVSCameraMapModuTool& operator=(IMVSCameraMapModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CameraMap.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSCameraMapModu::IMVSCameraMapModuTool * __stdcall GetCameraMapToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CAMERAMAP_H_
