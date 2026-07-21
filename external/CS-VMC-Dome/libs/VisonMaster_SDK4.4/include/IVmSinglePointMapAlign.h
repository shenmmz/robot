/***************************************************************************************************
 * File：IVmSinglePointMapAlign.h
 * Note：@~chinese 单点映射对位模块接口声明 @~english Interface for the SinglePointMapAlign tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_SINGLEPOINTMAPALIGN_H_
#define _IVM_SINGLEPOINTMAPALIGN_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace SinglePointMapAlignModu
    {
    /// \addtogroup 单点映射对位
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the SinglePointMapAlignParams.  */
        ///单点映射对位参数类
        class SinglePointMapAlignParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit SinglePointMapAlignParams() {}
            virtual ~SinglePointMapAlignParams() {}

        public:

            /// @~chinese 对象像素点(仅当次执行起效)   @~english ObjImagePoint(Only valid for this execution)
            virtual void SetObjImagePoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 对象图像角度(仅当次执行起效)   @~english Object Image Point Angle(Only valid for this execution)
            virtual void SetObjImagePointA(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标像素点(仅当次执行起效)   @~english TarImagePoint(Only valid for this execution)
            virtual void SetTarImagePoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标像素直线起点(仅当次执行起效)   @~english TarImageLineS(Only valid for this execution)
            virtual void SetTarImageLineS(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 目标像素直线终点(仅当次执行起效)   @~english TarImageLineE(Only valid for this execution)
            virtual void SetTarImageLineE(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 示教物理点(仅当次执行起效)   @~english Teach Physics Points(Only valid for this execution)
            virtual void SetTeachPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 示教角度(仅当次执行起效)   @~english Teach Angle(Only valid for this execution)
            virtual void SetTeachPointA(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese N点标定矩阵(仅当次执行起效)   @~english NPoint Calib Matrix(Only valid for this execution)
            virtual void SetNPointCalibMatrix(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 映射标定矩阵(仅当次执行起效)   @~english Map Calib Matrix(Only valid for this execution)
            virtual void SetMapCalibMatrix(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 刷新信号(仅当次执行起效)   @~english Refresh Signal(Only valid for this execution)
            virtual void SetRefreshSignal(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese N点标定文件   @~english N Calibration File
            __declspec(property(put = SetFilePath, get = GetFilePath)) const char* FilePath;

            virtual const char* GetFilePath() = 0;

            virtual void SetFilePath(const char* value) = 0;

            /// @~chinese 映射标定文件   @~english Load MapCalibration File
            __declspec(property(put = SetFilePathMap, get = GetFilePathMap)) const char* FilePathMap;

            virtual const char* GetFilePathMap() = 0;

            virtual void SetFilePathMap(const char* value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the SinglePointMapAlignResults.  */
        ///单点映射对位结果类
        class SinglePointMapAlignResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit SinglePointMapAlignResults() {}
            virtual ~SinglePointMapAlignResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

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
        /*  Note: Note: This is the class for the SinglePointMapAlign tool.  */
        ///单点映射对位工具类
        class SinglePointMapAlignModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit SinglePointMapAlignModuTool() {}
            virtual ~SinglePointMapAlignModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual SinglePointMapAlignParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual SinglePointMapAlignResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            SinglePointMapAlignModuTool(SinglePointMapAlignModuTool&);
            SinglePointMapAlignModuTool& operator=(SinglePointMapAlignModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for SinglePointMapAlign.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::SinglePointMapAlignModu::SinglePointMapAlignModuTool * __stdcall GetSinglePointMapAlignToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_SINGLEPOINTMAPALIGN_H_
