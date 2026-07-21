/***************************************************************************************************
 * File：IVmReadCalibFile.h
 * Note：@~chinese 标定加载模块接口声明 @~english Interface for the ReadCalibFile tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_READCALIBFILE_H_
#define _IVM_READCALIBFILE_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace ReadCalibFileModu
    {
    /// \addtogroup 标定加载
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the ReadCalibFileParams.  */
        ///标定加载参数类
        class ReadCalibFileParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ReadCalibFileParams() {}
            virtual ~ReadCalibFileParams() {}

        public:

            /// @~chinese 刷新信号(仅当次执行起效)   @~english Refresh Signal(Only valid for this execution)
            virtual void SetRefreshSignal(int* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 图像基准坐标X(仅当次执行起效)   @~english Image Reference Coordinate X(Only valid for this execution)
            virtual void SetSnapImagePointX1(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 图像基准坐标Y(仅当次执行起效)   @~english Image Reference Coordinate Y(Only valid for this execution)
            virtual void SetSnapImagePointY1(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 图像基准坐标R(仅当次执行起效)   @~english Image Reference Coordinate R(Only valid for this execution)
            virtual void SetSnapImageR1(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 拍照基准坐标X(仅当次执行起效)   @~english Photograph Reference Coordinate X(Only valid for this execution)
            virtual void SetSnapWorldPointX1(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 拍照基准坐标Y(仅当次执行起效)   @~english Photograph Reference Coordinate Y(Only valid for this execution)
            virtual void SetSnapWorldPointY1(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 拍照基准坐标R(仅当次执行起效)   @~english Photograph Reference Coordinate R(Only valid for this execution)
            virtual void SetSnapWorldR1(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 示教坐标X(仅当次执行起效)   @~english Demonstration Coordinate X(Only valid for this execution)
            virtual void SetTeachWorldPointX1(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 示教坐标Y(仅当次执行起效)   @~english Demonstration Coordinate Y(Only valid for this execution)
            virtual void SetTeachWorldPointY1(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 示教坐标R(仅当次执行起效)   @~english Demonstration Coordinate R(Only valid for this execution)
            virtual void SetTeachWorldR1(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 方案存矩阵   @~english Save Calib Data
            __declspec(property(put = SetSaveCalibDataEnable, get = GetSaveCalibDataEnable)) bool SaveCalibDataEnable;

            virtual bool GetSaveCalibDataEnable() = 0;

            virtual void SetSaveCalibDataEnable(bool value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ReadCalibFileResults.  */
        ///标定加载结果类
        class ReadCalibFileResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ReadCalibFileResults() {}
            virtual ~ReadCalibFileResults() {}

        public:
            /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

            /**
             * @brief 角度旋转一致性
             */
             virtual int GetRotDirectionState() = 0;

            /**
             * @brief 坐标系左右手一致性
             */
             virtual int GetIsRightCoorA() = 0;

            /**
             * @brief 旋转中心图像坐标
             */
             virtual Posture GetRotCenterImage() = 0;

            /**
             * @brief 旋转中心物理坐标
             */
             virtual Posture GetRotCenterWorld() = 0;
             
            /**
             * @brief 图像基准坐标
             */
             virtual Posture GetSnapImage() = 0;

            /**
             * @brief 拍照基准坐标
             */
             virtual Posture GetSnapWorld() = 0;

            /**
             * @brief 示教坐标
             */
             virtual Posture GetTeachWorld() = 0;

            /**
             * @brief 目标拍照坐标
             */
             virtual Posture GetSnapWorldCur() = 0;

            /**
             * @brief 对象拍照坐标
             */
             virtual Posture GetSnapWorldObj() = 0;

            /**
             * @brief 获取标定矩阵个数
             */
             virtual int GetCalibMatrixNum() = 0;

            /**
             * @brief 获取标定矩阵值
             */
             virtual float GetCalibMatrix(IN int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ReadCalibFile tool.  */
        ///标定加载工具类
        class ReadCalibFileModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ReadCalibFileModuTool() {}
            virtual ~ReadCalibFileModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ReadCalibFileParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ReadCalibFileResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            ReadCalibFileModuTool(ReadCalibFileModuTool&);
            ReadCalibFileModuTool& operator=(ReadCalibFileModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ReadCalibFile.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::ReadCalibFileModu::ReadCalibFileModuTool * __stdcall GetReadCalibFileToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_READCALIBFILE_H_
