/***************************************************************************************************
 * File：IVmTranslationCalib.h
 * Note：@~chinese 平移旋转标定模块接口声明 @~english Interface for the TranslationCalib tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_TRANSLATIONCALIB_H_
#define _IVM_TRANSLATIONCALIB_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace TranslationCalibModu
    {
        /// @~chinese 标定类型   @~english Calibration Type
        typedef enum _CalibTypeEnum
        {
            /// @~chinese 平移标定   @~english TransCalib
            CalibType_TransCalib = 0x0,

            /// @~chinese 平移旋转标定   @~english TransAndRotateCalib
            CalibType_TransAndRotateCalib = 0x1,

        }CalibTypeEnum;

        /// @~chinese 标定点获取   @~english Calib Points Acquisition
        typedef enum _CalibPointGetEnum
        {
            /// @~chinese 触发获取   @~english Trigger Acquisition
            CalibPointGet_TriggerAcquisition = 0x0,

            /// @~chinese 手动输入   @~english Manual Input
            CalibPointGet_ManualInput = 0x1,

        }CalibPointGetEnum;

        /// @~chinese 相机模式   @~english Camera Mode
        typedef enum _CameraModeEnum
        {
            /// @~chinese 相机静止   @~english Static Camera
            CameraMode_CameraStatic = 0x1,

            /// @~chinese 相机运动   @~english Dynamic Camera
            CameraMode_CameraMove = 0x3,

        }CameraModeEnum;

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

    /// \addtogroup 平移旋转标定
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the TranslationCalibParams.  */
        ///平移旋转标定参数类
        class TranslationCalibParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit TranslationCalibParams() {}
            virtual ~TranslationCalibParams() {}

        public:

            /// @~chinese 图像点(仅当次执行起效)   @~english Image Point(Only valid for this execution)
            virtual void SetImagePoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 物理点(仅当次执行起效)   @~english Physical Point(Only valid for this execution)
            virtual void SetPhysicalPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 图像角度(仅当次执行起效)   @~english Image Rotate Angle(Only valid for this execution)
            virtual void SetImageRotateAngle(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 物理角度(仅当次执行起效)   @~english World Rotate Angle(Only valid for this execution)
            virtual void SetWorldRotateAngle(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 外部输入字符(仅当次执行起效)   @~english External Input Character(Only valid for this execution)
            virtual void SetTrigger(InputStringData* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 外部触发字符(仅当次执行起效)   @~english External Triggered Character(Only valid for this execution)
            virtual void SetTeachFlag(InputStringData* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 标定类型   @~english Calibration Type
            __declspec(property(put = SetCalibType, get = GetCalibType)) CalibTypeEnum CalibType;

            virtual CalibTypeEnum GetCalibType() = 0;

            virtual void SetCalibType(CalibTypeEnum value) = 0;

            /// @~chinese 标定点获取   @~english Calib Points Acquisition
            __declspec(property(put = SetCalibPointGet, get = GetCalibPointGet)) CalibPointGetEnum CalibPointGet;

            virtual CalibPointGetEnum GetCalibPointGet() = 0;

            virtual void SetCalibPointGet(CalibPointGetEnum value) = 0;

            /// @~chinese 相机模式   @~english Camera Mode
            __declspec(property(put = SetCameraMode, get = GetCameraMode)) CameraModeEnum CameraMode;

            virtual CameraModeEnum GetCameraMode() = 0;

            virtual void SetCameraMode(CameraModeEnum value) = 0;

            /// @~chinese 自由度   @~english DOF
            __declspec(property(put = SetHomoFreedom, get = GetHomoFreedom)) HomoFreedomEnum HomoFreedom;

            virtual HomoFreedomEnum GetHomoFreedom() = 0;

            virtual void SetHomoFreedom(HomoFreedomEnum value) = 0;

            /// @~chinese 平移次数，范围：[4,16]   @~english Translation Times，range:[4,16]
            __declspec(property(put = SetCalibPointTotalNum, get = GetCalibPointTotalNum)) int CalibPointTotalNum;

            virtual int GetCalibPointTotalNum() = 0;

            virtual void SetCalibPointTotalNum(int value) = 0;

            /// @~chinese 旋转次数，范围：[0,16]   @~english Rotation Times，range:[0,16]
            __declspec(property(put = SetRotPointTotalNum, get = GetRotPointTotalNum)) int RotPointTotalNum;

            virtual int GetRotPointTotalNum() = 0;

            virtual void SetRotPointTotalNum(int value) = 0;

            /// @~chinese 组合标定   @~english UnionCalibEnable
            __declspec(property(put = SetUnionCalibEnable, get = GetUnionCalibEnable)) bool UnionCalibEnable;

            virtual bool GetUnionCalibEnable() = 0;

            virtual void SetUnionCalibEnable(bool value) = 0;

            /// @~chinese 示教   @~english Demonstration
            __declspec(property(put = SetTeachEnable, get = GetTeachEnable)) bool TeachEnable;

            virtual bool GetTeachEnable() = 0;

            virtual void SetTeachEnable(bool value) = 0;

            /// @~chinese 更新文件   @~english Update File
            __declspec(property(put = SetRefreshFileEnable, get = GetRefreshFileEnable)) bool RefreshFileEnable;

            virtual bool GetRefreshFileEnable() = 0;

            virtual void SetRefreshFileEnable(bool value) = 0;

            /// @~chinese 标定文件路径   @~english Set Calibration File Path
            __declspec(property(put = SetCalibPathName, get = GetCalibPathName)) const char* CalibPathName;

            virtual const char* GetCalibPathName() = 0;

            virtual void SetCalibPathName(const char* value) = 0;

            /// @~chinese 清空标定点   @~english Clear Point
            virtual int DoClearPoint() = 0;

            /// @~chinese 生成标定文件   @~english Create Calibration File
            virtual int DoSaveFile(IN const char * const strValue) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the TranslationCalibResults.  */
        ///平移旋转标定结果类
        class TranslationCalibResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit TranslationCalibResults() {}
            virtual ~TranslationCalibResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

             /**
             * @brief 标定状态
             */
             virtual int GetCalibStatus() = 0;

             /**
             * @brief 坐标系左右手一致性
             */
             virtual int GetIsRightCoorA() = 0;

             /**
             * @brief 当前标定点
             */
             virtual int GetCalibCurIndex() = 0;

             /**
             * @brief 标定点总数
             */
             virtual int GetCalibTotalNum() = 0;

             /**
             * @brief 标定图像点
             */
             virtual PointF GetCalibImagePoint(int nIndex) = 0;

             /**
             * @brief 标定物理点
             */
             virtual PointF GetCalibPhysicalPoint(int nIndex) = 0;

             /**
             * @brief 平移像素平均误差
             */
             virtual float GetTransError() = 0;

             /**
             * @brief 旋转像素平均误差
             */
             virtual float GetRotError() = 0;

             /**
             * @brief 旋转轴图像点
             */
             virtual PointF GetRotAxisPoint() = 0;

             /**
             * @brief 旋转中心物理点
             */
             virtual PointF GetRotPhysicalPoint() = 0;

             /**
             * @brief 角度旋转一致性
             */
             virtual int GetRotDirectionState() = 0;

             /**
             * @brief 平移像素最大误差
             */
             virtual float GetTransEstMax() = 0;

             /**
             * @brief 平移像素最大误差对应点数
             */
             virtual float GetTransErrMaxPtsNum() = 0;

             /**
             * @brief 平移估计真实误差
             */
             virtual float GetTransWorldError() = 0;

             /**
             * @brief 平移像素真实最大误差
             */
             virtual float GetTransErrWorldMax() = 0;

             /**
             * @brief 旋转像素最大误差
             */
             virtual float GetRotErrMax() = 0;

             /**
             * @brief 旋转像素最大误差对应点数
             */
             virtual int GetRotErrMaxPtsNum() = 0;

             /**
             * @brief 旋转真实平均误差
             */
             virtual float GetRotWorldError() = 0;

             /**
             * @brief 旋转真实最大误差
             */
             virtual float GetRotErrWorldMax() = 0;

             /**
             * @brief 尺度
             */
             virtual float GetScale() = 0;

             /**
             * @brief x偏移
             */
             virtual float GetDeltaX() = 0;

             /**
             * @brief y偏移
             */
             virtual float GetDeltaY() = 0;

             /**
             * @brief 旋转
             */
             virtual float GetRotate() = 0;

             /**
             * @brief 倾斜量
             */
             virtual float GetTilt() = 0;

             /**
             * @brief Y方向和X方向的比例
             */
             virtual float GetDirectionRatio() = 0;

             /**
             * @brief 像素精度
             */
             virtual float GetPixelPrecision() = 0;

        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the TranslationCalib tool.  */
        ///平移旋转标定工具类
        class TranslationCalibModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit TranslationCalibModuTool() {}
            virtual ~TranslationCalibModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual TranslationCalibParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual TranslationCalibResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            TranslationCalibModuTool(TranslationCalibModuTool&);
            TranslationCalibModuTool& operator=(TranslationCalibModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for TranslationCalib.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::TranslationCalibModu::TranslationCalibModuTool * __stdcall GetTranslationCalibToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_TRANSLATIONCALIB_H_
