/***************************************************************************************************
 * File：IVmSinglePointGrab.h
 * Note：@~chinese 单点抓取模块接口声明 @~english Interface for the SinglePointGrab tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_SINGLEPOINTGRAB_H_
#define _IVM_SINGLEPOINTGRAB_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace SinglePointGrabModu
    {
        /// @~chinese 标定类型   @~english Calibration Type
        typedef enum _CalibTypeEnum
        {
            /// @~chinese 平移标定   @~english TransCalib
            CalibType_TransCalib = 0x0,

            /// @~chinese 平移旋转标定   @~english TransAndRotateCalib
            CalibType_TransAndRotateCalib = 0x1,

        }CalibTypeEnum;

    /// \addtogroup 单点抓取
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the SinglePointGrabParams.  */
        ///单点抓取参数类
        class SinglePointGrabParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit SinglePointGrabParams() {}
            virtual ~SinglePointGrabParams() {}

        public:

            /// @~chinese 像素点(仅当次执行起效)   @~english ImagePoint(Only valid for this execution)
            virtual void SetImagePoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 图像角度(仅当次执行起效)   @~english ImagePointA(Only valid for this execution)
            virtual void SetImagePointA(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 示教物理点(仅当次执行起效)   @~english Teach Physics Points(Only valid for this execution)
            virtual void SetTeachPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 示教角度(仅当次执行起效)   @~english Teach Angle(Only valid for this execution)
            virtual void SetTeachPointA(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 示教拍照物理点(仅当次执行起效)   @~english Teach Snap Point(Only valid for this execution)
            virtual void SetTeachSnapPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 示教拍照角度(仅当次执行起效)   @~english Teach Snap Angle(Only valid for this execution)
            virtual void SetTeachSnapPointA(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 标定矩阵(仅当次执行起效)   @~english Calibration Matrix(Only valid for this execution)
            virtual void SetCalibMatrix(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 刷新信号(仅当次执行起效)   @~english Refresh Signal(Only valid for this execution)
            virtual void SetRefreshSignal(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 旋转相对角度(仅当次执行起效)   @~english Snap Point Delta Angle(Only valid for this execution)
            virtual void SetSnapPointDeltaAngle(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 标定类型   @~english Calibration Type
            __declspec(property(put = SetCalibType, get = GetCalibType)) CalibTypeEnum CalibType;

            virtual CalibTypeEnum GetCalibType() = 0;

            virtual void SetCalibType(CalibTypeEnum value) = 0;

            /// @~chinese 旋转拍照使能   @~english Snap Point Rotate Enable
            __declspec(property(put = SetSnapPointRotateEnable, get = GetSnapPointRotateEnable)) bool SnapPointRotateEnable;

            virtual bool GetSnapPointRotateEnable() = 0;

            virtual void SetSnapPointRotateEnable(bool value) = 0;

            /// @~chinese 加载标定文件   @~english Load Calibration File
            __declspec(property(put = SetFilePath, get = GetFilePath)) const char* FilePath;

            virtual const char* GetFilePath() = 0;

            virtual void SetFilePath(const char* value) = 0;

            /// @~chinese 创建基准   @~english Create Reference
            virtual int DoBasicInit() = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the SinglePointGrabResults.  */
        ///单点抓取结果类
        class SinglePointGrabResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit SinglePointGrabResults() {}
            virtual ~SinglePointGrabResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

             /**
             * @brief 获取基准点坐标
             * @return 返回基准点坐标
             */
             virtual PointF GetInitPoint() = 0;

             /**
             * @brief 获取基准点角度
             * @return 返回基准点角度
             */
             virtual float GetInitPointAngle() = 0;

             /**
             * @brief 相对坐标X
             */
             virtual float GetOffsetX() = 0;

             /**
             * @brief 相对坐标Y
             */
             virtual float GetOffsetY() = 0;

             /**
             * @brief 相对坐标R
             */
             virtual float GetOffsetR() = 0;

             /**
             * @brief 绝对坐标X
             */
             virtual float GetAbssetX() = 0;

             /**
             * @brief 绝对坐标Y
             */
             virtual float GetAbssetY() = 0;

             /**
             * @brief 绝对坐标R
             */
             virtual float GetAbssetR() = 0;

        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the SinglePointGrab tool.  */
        ///单点抓取工具类
        class SinglePointGrabModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit SinglePointGrabModuTool() {}
            virtual ~SinglePointGrabModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual SinglePointGrabParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual SinglePointGrabResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            SinglePointGrabModuTool(SinglePointGrabModuTool&);
            SinglePointGrabModuTool& operator=(SinglePointGrabModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for SinglePointGrab.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::SinglePointGrabModu::SinglePointGrabModuTool * __stdcall GetSinglePointGrabToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_SINGLEPOINTGRAB_H_
