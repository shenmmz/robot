/***************************************************************************************************
 * File：IVmNPointCalib.h
 * Note：@~chinese N点标定模块接口声明 @~english Interface for the NPointCalib tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_NPOINTCALIB_H_
#define _IVM_NPOINTCALIB_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSNPointCalibModu
    {
        /// @~chinese 标定点获取   @~english Calib Points Acquisition
        typedef enum _CalibPointGetEnum
        {
            /// @~chinese 触发获取   @~english Trigger Acquisition
            CalibPointGet_TriggerAcquisition = 0x0,

            /// @~chinese 手动输入   @~english Manual Input
            CalibPointGet_ManualInput = 0x1,

        }CalibPointGetEnum;

        /// @~chinese 移动优先   @~english Movement First
        typedef enum _MoveFirstTypeEnum
        {
            /// @~chinese X优先   @~english X First
            MoveFirstType_XFirst = 0x1,

            /// @~chinese Y优先   @~english Y First
            MoveFirstType_YFirst = 0x2,

        }MoveFirstTypeEnum;

        /// @~chinese 相机模式   @~english Camera Mode
        typedef enum _CameraModeEnum
        {
            /// @~chinese 相机静止上相机位   @~english Camera Stationary Up
            CameraMode_CameraStaticUp = 0x1,

            /// @~chinese 相机静止下相机位   @~english Camera Stationary Down
            CameraMode_CameraStaticDown = 0x2,

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

        /// @~chinese 权重函数   @~english Weighting Function
        typedef enum _WeightFunEnum
        {
            /// @~chinese 最小二乘   @~english Least Squares
            WeightFun_LeastSqure = 0x1,

            /// @~chinese Huber   @~english Huber
            WeightFun_Huber = 0x2,

            /// @~chinese Tukey   @~english Tukey
            WeightFun_Tukey = 0x3,

            /// @~chinese Ransac(弃用)   @~english Ransac(deprecated)
            WeightFun_Ransac = 0x4,

        }WeightFunEnum;

    /// \addtogroup N点标定
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the NPointCalibParams.  */
        ///N点标定参数类
        class NPointCalibParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit NPointCalibParams() {}
            virtual ~NPointCalibParams() {}

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
            virtual void SetTeachFlagInput(InputStringData* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 标定点获取   @~english Calib Points Acquisition
            __declspec(property(put = SetCalibPointGet, get = GetCalibPointGet)) CalibPointGetEnum CalibPointGet;

            virtual CalibPointGetEnum GetCalibPointGet() = 0;

            virtual void SetCalibPointGet(CalibPointGetEnum value) = 0;

            /// @~chinese 平移次数，范围：[4,16]   @~english Translation Times，range:[4,16]
            __declspec(property(put = SetCalibPointTotalNum, get = GetCalibPointTotalNum)) int CalibPointTotalNum;

            virtual int GetCalibPointTotalNum() = 0;

            virtual void SetCalibPointTotalNum(int value) = 0;

            /// @~chinese 旋转次数，范围：[0,16]   @~english Rotation Times，range:[0,16]
            __declspec(property(put = SetRotPointTotalNum, get = GetRotPointTotalNum)) int RotPointTotalNum;

            virtual int GetRotPointTotalNum() = 0;

            virtual void SetRotPointTotalNum(int value) = 0;

            /// @~chinese 示教   @~english Demonstration
            __declspec(property(put = SetTeachEnable, get = GetTeachEnable)) bool TeachEnable;

            virtual bool GetTeachEnable() = 0;

            virtual void SetTeachEnable(bool value) = 0;

            /// @~chinese 外部触发字符   @~english External Triggered Character
            __declspec(property(put = SetTeachFlag, get = GetTeachFlag)) const char* TeachFlag;

            virtual const char* GetTeachFlag() = 0;

            virtual void SetTeachFlag(const char* value) = 0;

            /// @~chinese 基准点X，范围：[-9999999.0,9999999.0]   @~english InitPointX，range:[-9999999.0,9999999.0]
            __declspec(property(put = SetBasePointX, get = GetBasePointX)) double BasePointX;

            virtual double GetBasePointX() = 0;

            virtual void SetBasePointX(double value) = 0;

            /// @~chinese 基准点Y，范围：[-9999999.0,9999999.0]   @~english InitPointY，range:[-9999999.0,9999999.0]
            __declspec(property(put = SetBasePointY, get = GetBasePointY)) double BasePointY;

            virtual double GetBasePointY() = 0;

            virtual void SetBasePointY(double value) = 0;

            /// @~chinese 偏移X，范围：[-99999.0,99999.0]   @~english Offset X，range:[-99999.0,99999.0]
            __declspec(property(put = SetMoveAlignX, get = GetMoveAlignX)) double MoveAlignX;

            virtual double GetMoveAlignX() = 0;

            virtual void SetMoveAlignX(double value) = 0;

            /// @~chinese 偏移Y，范围：[-99999.0,99999.0]   @~english Offset Y，range:[-99999.0,99999.0]
            __declspec(property(put = SetMoveAlignY, get = GetMoveAlignY)) double MoveAlignY;

            virtual double GetMoveAlignY() = 0;

            virtual void SetMoveAlignY(double value) = 0;

            /// @~chinese 移动优先   @~english Movement First
            __declspec(property(put = SetMoveFirstType, get = GetMoveFirstType)) MoveFirstTypeEnum MoveFirstType;

            virtual MoveFirstTypeEnum GetMoveFirstType() = 0;

            virtual void SetMoveFirstType(MoveFirstTypeEnum value) = 0;

            /// @~chinese 换向移动次数，范围：[1,16]   @~english Commutation Number，range:[1,16]
            __declspec(property(put = SetChangeDirectionMoveTime, get = GetChangeDirectionMoveTime)) int ChangeDirectionMoveTime;

            virtual int GetChangeDirectionMoveTime() = 0;

            virtual void SetChangeDirectionMoveTime(int value) = 0;

            /// @~chinese 基准角度，范围：[-180.0,180.0]   @~english InitAngle，range:[-180.0,180.0]
            __declspec(property(put = SetBaseAngle, get = GetBaseAngle)) double BaseAngle;

            virtual double GetBaseAngle() = 0;

            virtual void SetBaseAngle(double value) = 0;

            /// @~chinese 角度偏移，范围：[-180.0,180.0]   @~english Angle Offset，range:[-180.0,180.0]
            __declspec(property(put = SetMoveAngle, get = GetMoveAngle)) double MoveAngle;

            virtual double GetMoveAngle() = 0;

            virtual void SetMoveAngle(double value) = 0;

            /// @~chinese 使用相对坐标   @~english Use Relative Coordinates
            __declspec(property(put = SetUseRelativeCoordinates, get = GetUseRelativeCoordinates)) bool UseRelativeCoordinates;

            virtual bool GetUseRelativeCoordinates() = 0;

            virtual void SetUseRelativeCoordinates(bool value) = 0;

            /// @~chinese 标定原点，范围：[0,15]   @~english Calibration Origin，range:[0,15]
            __declspec(property(put = SetCalibOrigin, get = GetCalibOrigin)) int CalibOrigin;

            virtual int GetCalibOrigin() = 0;

            virtual void SetCalibOrigin(int value) = 0;

            /// @~chinese 更新文件   @~english Update File
            __declspec(property(put = SetRefreshFileEnable, get = GetRefreshFileEnable)) bool RefreshFileEnable;

            virtual bool GetRefreshFileEnable() = 0;

            virtual void SetRefreshFileEnable(bool value) = 0;

            /// @~chinese 标定文件路径   @~english Set Calibration File Path
            __declspec(property(put = SetCalibPathName, get = GetCalibPathName)) const char* CalibPathName;

            virtual const char* GetCalibPathName() = 0;

            virtual void SetCalibPathName(const char* value) = 0;

            /// @~chinese 相机模式   @~english Camera Mode
            __declspec(property(put = SetCameraMode, get = GetCameraMode)) CameraModeEnum CameraMode;

            virtual CameraModeEnum GetCameraMode() = 0;

            virtual void SetCameraMode(CameraModeEnum value) = 0;

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

            /// @~chinese 距离阈值，范围：[1,100]   @~english Distance Threshold，range:[1,100]
            __declspec(property(put = SetDistThreshold, get = GetDistThreshold)) int DistThreshold;

            virtual int GetDistThreshold() = 0;

            virtual void SetDistThreshold(int value) = 0;

            /// @~chinese 采样率，范围：[1,100]   @~english Sampling Rate，range:[1,100]
            __declspec(property(put = SetSampleRatio, get = GetSampleRatio)) int SampleRatio;

            virtual int GetSampleRatio() = 0;

            virtual void SetSampleRatio(int value) = 0;

            /// @~chinese 清空标定点   @~english Clear Point
            virtual int DoClearPoint() = 0;

            /// @~chinese 生成标定文件   @~english Create Calibration File
            virtual int DoSaveFile(IN const char * const strValue) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes for describing offset point calib process result information.  */
		/// 仅平移点标定情景下的结果信息
        class IOffsetPointCalibInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IOffsetPointCalibInfo() {}
            virtual ~IOffsetPointCalibInfo() {}

        public:
            /**
             * @brief HOMO估计状态
             */
            virtual int GetHomoEstStatus() = 0;
            /**
             * @brief 平移误差
             */
            virtual float GetOffsetError() = 0;
            /**
             * @brief HOMO矩阵(预留)
             */
            virtual void GetHomoMatrix(float homoMatrix[9]) = 0;
        };

        /*  Note: Interface Classes for describing platform calib process result information.  */
		/// 平移加旋转点标定情景下的结果信息
        class IPlatformCalibInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IPlatformCalibInfo() {}
            virtual ~IPlatformCalibInfo() {}

        public:
            /**
             * @brief N点标定中平移估计误差
             */
            virtual float GetOffsetError() = 0;
            /**
             * @brief N点标定中旋转估计误差
             */
            virtual float GetRotateError() = 0;
            /**
             * @brief 旋转中心图像坐标
             */
            virtual MVD_POINT_F GetRotCenterImageCoord() = 0;
            /**
             * @brief 旋转中心物理坐标
             */
            virtual MVD_POINT_F GetRotCenterWorldCoord() = 0;
        };

        /*  Note: Interface Classes for describing parse homo matrix process result information.  */
		/// 标定文件解析结果信息
        class ICalibMatrixToParamInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit ICalibMatrixToParamInfo() {}
            virtual ~ICalibMatrixToParamInfo() {}

        public:
            /**
             * @brief 尺度
             */
            virtual float GetScale() = 0;
            /**
             * @brief X偏移
             */
            virtual float GetOffsetX() = 0;
            /**
             * @brief Y偏移
             */
            virtual float GetOffsetY() = 0;
            /**
             * @brief 旋转角度
             */
            virtual float GetRotate() = 0;
            /**
             * @brief Y方向和X方向的比例
             */
            virtual float GetAspect() = 0;
            /**
             * @brief 倾斜量
             */
            virtual float GetSkew() = 0;
            /**
             * @brief 像素精度
             */
            virtual float GetPixelPrecision() = 0;
        };

        /*  Note: Interface Classes of the result of the NPointCalibResults.  */
        ///N点标定结果类
        class NPointCalibResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit NPointCalibResults() {}
            virtual ~NPointCalibResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 获取N点标定的结果，若旋转点个数不为0，结果为空。
             */
            /*  Note: offset calib result.
                if the count of rotate point is not zero, this result is NULL.  */
            virtual IOffsetPointCalibInfo* GetOffsetPointCalibInfo() = 0;
            /**
             * @brief 获取N点标定的结果，若旋转点个数为0，结果为空。
             */
            virtual IPlatformCalibInfo* GetPlatformCalibInfo() = 0;
            /**
             * @brief N点标定homo矩阵转参数
             */
            virtual ICalibMatrixToParamInfo* GetCalibMatrixToParamInfo() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the NPointCalib tool.  */
        ///N点标定工具类
        class IMVSNPointCalibModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSNPointCalibModuTool() {}
            virtual ~IMVSNPointCalibModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual NPointCalibParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual NPointCalibResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSNPointCalibModuTool(IMVSNPointCalibModuTool&);
            IMVSNPointCalibModuTool& operator=(IMVSNPointCalibModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for NPointCalib.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSNPointCalibModu::IMVSNPointCalibModuTool * __stdcall GetNPointCalibToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_NPOINTCALIB_H_
