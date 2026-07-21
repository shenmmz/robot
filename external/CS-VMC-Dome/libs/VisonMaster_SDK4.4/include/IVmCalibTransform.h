/***************************************************************************************************
 * File：IVmCalibTransform.h
 * Note：@~chinese 标定转换模块接口声明 @~english Interface for the CalibTransform tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CALIBTRANSFORM_H_
#define _IVM_CALIBTRANSFORM_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCalibTransformModu
    {
        /// @~chinese 坐标类型   @~english Coord Type
        typedef enum _CoordTypeEnum
        {
            /// @~chinese 图像坐标   @~english Image Coord
            CoordType_ImageCoord = 0x0,

            /// @~chinese 物理坐标   @~english Physical Coord
            CoordType_PhysicalCoord = 0x1,

        }CoordTypeEnum;

    /// \addtogroup 标定转换
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the CalibTransformParams.  */
        ///标定转换参数类
        class CalibTransformParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CalibTransformParams() {}
            virtual ~CalibTransformParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入点(仅当次执行起效)   @~english Input Point(Only valid for this execution)
            virtual void SetInputPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 图像角度(仅当次执行起效)   @~english ImagePointA(Only valid for this execution)
            virtual void SetImagePointA(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 刷新信号(仅当次执行起效)   @~english Refresh Signal(Only valid for this execution)
            virtual void SetRefreshSignal(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 标定矩阵(仅当次执行起效)   @~english Calibration Matrix(Only valid for this execution)
            virtual void SetCalibMatrix(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese (预留)标定位位姿点(仅当次执行起效)   @~english (Reserve)Input Calib Pose Point(Only valid for this execution)
            virtual void SetInputCalibPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese (预留)标定位位姿关节0角度(仅当次执行起效)   @~english (Reserve)Input Calib Pose Joint0 Angle(Only valid for this execution)
            virtual void SetCalibPoseJoint0A(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese (预留)标定位位姿关节1角度(仅当次执行起效)   @~english (Reserve)Input Calib Pose Joint1 Angle(Only valid for this execution)
            virtual void SetCalibPoseJoint1A(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese (预留)运行位位姿点(仅当次执行起效)   @~english (Reserve)Input Run Pose Point(Only valid for this execution)
            virtual void SetInputRunPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese (预留)运行位位姿关节0角度(仅当次执行起效)   @~english (Reserve)Input Run Pose Joint0 Angle(Only valid for this execution)
            virtual void SetRunPoseJoint0A(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese (预留)运行位位姿关节1角度(仅当次执行起效)   @~english (Reserve)Input Run Pose Joint1 Angle(Only valid for this execution)
            virtual void SetRunPoseJoint1A(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 坐标类型   @~english Coord Type
            __declspec(property(put = SetCoordType, get = GetCoordType)) CoordTypeEnum CoordType;

            virtual CoordTypeEnum GetCoordType() = 0;

            virtual void SetCoordType(CoordTypeEnum value) = 0;

            /// @~chinese 加载标定文件   @~english Load Calibration File
            __declspec(property(put = SetLoadCalibPath, get = GetLoadCalibPath)) const char* LoadCalibPath;

            virtual const char* GetLoadCalibPath() = 0;

            virtual void SetLoadCalibPath(const char* value) = 0;

            /// @~chinese (预留)位姿输入使能   @~english (Reserve)Pose Input Enable
            __declspec(property(put = SetPoseInfoEnable, get = GetPoseInfoEnable)) bool PoseInfoEnable;

            virtual bool GetPoseInfoEnable() = 0;

            virtual void SetPoseInfoEnable(bool value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the CalibTransformResults.  */
        ///标定转换结果类
        class CalibTransformResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CalibTransformResults() {}
            virtual ~CalibTransformResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 尺度
             */
            virtual float GetScale() = 0;
            /**
             * @brief 输出角度
             * @param nIndex [IN] 结果数量索引
             */
            ///< Property
            virtual float GetAngle(int nIndex) = 0;
            /**
             * @brief 输出坐标
             * @param nIndex [IN] 结果数量索引
             */
            /*  Note: Mechanical Coordinate of a point. */
            virtual MVD_POINT_F GetCoordinate(int nIndex) = 0;
            /**
             * @brief 单像素精度
             */
             virtual float GetPixelScale() = 0;
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
              * @brief 斜切
              */
             virtual float GetSkew() = 0;
             /**
              * @brief 宽高比
              */
             virtual float GetAspect() = 0;
             /**
             * @brief 输出标定位姿坐标(预留)
             * @param nIndex [IN] 结果数量索引
             */
             /*  Note: Mechanical Coordinate of a point. */
             virtual MVD_POINT_F GetCalibPosePoint(int nIndex) = 0;
             /**
             * @brief 标定位姿关节0角度(预留)
             */
			 virtual float GetCalibPoseJoint0A(int nIndex) = 0;
             /**
             * @brief 标定位姿关节1角度(预留)
             */
			 virtual float GetCalibPoseJoint1A(int nIndex) = 0;
             /**
             * @brief 输出运行位姿坐标(预留)
             * @param nIndex [IN] 结果数量索引
             */
             /*  Note: Mechanical Coordinate of a point. */
             virtual MVD_POINT_F GetRunPosePoint(int nIndex) = 0;
             /**
             * @brief 运行位姿关节0角度(预留)
             */
			 virtual float GetRunPoseJoint0A(int nIndex) = 0;
             /**
             * @brief 运行位姿关节1角度(预留)
             */
			 virtual float GetRunPoseJoint1A(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CalibTransform tool.  */
        ///标定转换工具类
        class IMVSCalibTransformModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCalibTransformModuTool() {}
            virtual ~IMVSCalibTransformModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CalibTransformParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CalibTransformResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSCalibTransformModuTool(IMVSCalibTransformModuTool&);
            IMVSCalibTransformModuTool& operator=(IMVSCalibTransformModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CalibTransform.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSCalibTransformModu::IMVSCalibTransformModuTool * __stdcall GetCalibTransformToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CALIBTRANSFORM_H_
