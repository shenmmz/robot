/***************************************************************************************************
 * File：IVmRotateCalib.h
 * Note：@~chinese 旋转标定模块接口声明 @~english Interface for the RotateCalib tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_ROTATECALIB_H_
#define _IVM_ROTATECALIB_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace RotateCalibModu
    {
    /// \addtogroup 旋转标定
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the RotateCalibParams.  */
        ///旋转标定参数类
        class RotateCalibParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit RotateCalibParams() {}
            virtual ~RotateCalibParams() {}

        public:

            /// @~chinese 图像点(仅当次执行起效)   @~english Image Point(Only valid for this execution)
            virtual void SetImagePoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 物理角度(仅当次执行起效)   @~english World Rotate Angle(Only valid for this execution)
            virtual void SetWorldRotateAngle(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 旋转次数(仅当次执行起效)   @~english Rotation Times(Only valid for this execution)
            virtual void SetRotNum(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 刷新信号(仅当次执行起效)   @~english Refresh Signal(Only valid for this execution)
            virtual void SetRefreshSignal(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 反向纠正   @~english RevOutputFlag
            __declspec(property(put = SetRevOutputFlag, get = GetRevOutputFlag)) bool RevOutputFlag;

            virtual bool GetRevOutputFlag() = 0;

            virtual void SetRevOutputFlag(bool value) = 0;

            /// @~chinese 加载标定文件   @~english Load Calibration File
            __declspec(property(put = SetFilePath, get = GetFilePath)) const char* FilePath;

            virtual const char* GetFilePath() = 0;

            virtual void SetFilePath(const char* value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the RotateCalibResults.  */
        ///旋转标定结果类
        class RotateCalibResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit RotateCalibResults() {}
            virtual ~RotateCalibResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

             /**
             * @brief 剩余标定次数
             */
             virtual int GetRemainCount() = 0;

             /**
             * @brief 旋转轴图像点
             */
             virtual PointF GetRotAxisPoint() = 0;

             /**
             * @brief 旋转中心物理点
             */
             virtual PointF GetRotPhysicalPoint() = 0;

             /**
             * @brief 旋转像素平均误差
             */
             virtual float GetRotError() = 0;

             /**
             * @brief 旋转真实平均误差
             */
             virtual float GetRotWorldError() = 0;

        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the RotateCalib tool.  */
        ///旋转标定工具类
        class RotateCalibModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit RotateCalibModuTool() {}
            virtual ~RotateCalibModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual RotateCalibParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual RotateCalibResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            RotateCalibModuTool(RotateCalibModuTool&);
            RotateCalibModuTool& operator=(RotateCalibModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for RotateCalib.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::RotateCalibModu::RotateCalibModuTool * __stdcall GetRotateCalibToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_ROTATECALIB_H_
