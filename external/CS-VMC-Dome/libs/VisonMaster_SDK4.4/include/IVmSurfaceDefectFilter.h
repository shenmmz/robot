/***************************************************************************************************
 * File：IVmSurfaceDefectFilter.h
 * Note：@~chinese 表面缺陷滤波模块接口声明 @~english Interface for the SurfaceDefectFilter tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_SURFACEDEFECTFILTER_H_
#define _IVM_SURFACEDEFECTFILTER_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSSurfaceDefectFilterModu
    {
        /// @~chinese 滤波模式   @~english FilterMode
        typedef enum _FilterModeEnum
        {
            /// @~chinese 高精度   @~english High Precision
            FilterMode_Precision = 0x1,

            /// @~chinese 快速   @~english Fast
            FilterMode_Fast = 0x2,

        }FilterModeEnum;

    /// \addtogroup 表面缺陷滤波
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///表面缺陷滤波ROI管理类
        class SurfaceDefectFilterRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
            virtual void SetRoiAnnulus(Annulus stRoiValue) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

            /// @~chinese 掩膜图像（仅当次执行起效）   @~english Mask Image(Only valid for this execution)
            virtual void SetMaskImage(ImageBaseData stMaskImage) = 0;

        };

        /*  Note: Interface Classes of the parameter of the SurfaceDefectFilterParams.  */
        ///表面缺陷滤波参数类
        class SurfaceDefectFilterParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit SurfaceDefectFilterParams() {}
            virtual ~SurfaceDefectFilterParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual SurfaceDefectFilterRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 滤波模式   @~english FilterMode
            __declspec(property(put = SetFilterMode, get = GetFilterMode)) FilterModeEnum FilterMode;

            virtual FilterModeEnum GetFilterMode() = 0;

            virtual void SetFilterMode(FilterModeEnum value) = 0;

            /// @~chinese 滤波核宽度，范围：[3,51]   @~english Filter Kernel Width，range:[3,51]
            __declspec(property(put = SetKerWidth, get = GetKerWidth)) int KerWidth;

            virtual int GetKerWidth() = 0;

            virtual void SetKerWidth(int value) = 0;

            /// @~chinese 滤波核高度，范围：[3,51]   @~english Filter Kernel Height，range:[3,51]
            __declspec(property(put = SetKerHeight, get = GetKerHeight)) int KerHeight;

            virtual int GetKerHeight() = 0;

            virtual void SetKerHeight(int value) = 0;

            /// @~chinese 核的数量，范围：[1,180]   @~english Ker Num，range:[1,180]
            __declspec(property(put = SetKerNum, get = GetKerNum)) int KerNum;

            virtual int GetKerNum() = 0;

            virtual void SetKerNum(int value) = 0;

            /// @~chinese 标准差，范围：[10,500]   @~english LumStd，range:[10,500]
            __declspec(property(put = SetSigma, get = GetSigma)) int Sigma;

            virtual int GetSigma() = 0;

            virtual void SetSigma(int value) = 0;

            /// @~chinese 偏移，范围：[0,100000]   @~english Offset，range:[0,100000]
            __declspec(property(put = SetOffset, get = GetOffset)) int Offset;

            virtual int GetOffset() = 0;

            virtual void SetOffset(int value) = 0;

            /// @~chinese 0度权重，范围：[-10.0,10.0]   @~english 0 Angle Weight，range:[-10.0,10.0]
            __declspec(property(put = SetWeight0, get = GetWeight0)) double Weight0;

            virtual double GetWeight0() = 0;

            virtual void SetWeight0(double value) = 0;

            /// @~chinese 30度权重，范围：[-10.0,10.0]   @~english 30 Angle Weight，range:[-10.0,10.0]
            __declspec(property(put = SetWeight1, get = GetWeight1)) double Weight1;

            virtual double GetWeight1() = 0;

            virtual void SetWeight1(double value) = 0;

            /// @~chinese 60度权重，范围：[-10.0,10.0]   @~english 60 Angle Weight，range:[-10.0,10.0]
            __declspec(property(put = SetWeight2, get = GetWeight2)) double Weight2;

            virtual double GetWeight2() = 0;

            virtual void SetWeight2(double value) = 0;

            /// @~chinese 90度权重，范围：[-10.0,10.0]   @~english 90 Angle Weight，range:[-10.0,10.0]
            __declspec(property(put = SetWeight3, get = GetWeight3)) double Weight3;

            virtual double GetWeight3() = 0;

            virtual void SetWeight3(double value) = 0;

            /// @~chinese 120度权重，范围：[-10.0,10.0]   @~english 120 Angle Weight，range:[-10.0,10.0]
            __declspec(property(put = SetWeight4, get = GetWeight4)) double Weight4;

            virtual double GetWeight4() = 0;

            virtual void SetWeight4(double value) = 0;

            /// @~chinese 150度权重，范围：[-10.0,10.0]   @~english 150 Angle Weight，range:[-10.0,10.0]
            __declspec(property(put = SetWeight5, get = GetWeight5)) double Weight5;

            virtual double GetWeight5() = 0;

            virtual void SetWeight5(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the SurfaceDefectFilterResults.  */
        ///表面缺陷滤波结果类
        class SurfaceDefectFilterResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit SurfaceDefectFilterResults() {}
            virtual ~SurfaceDefectFilterResults() {}

        public:
             /**
             * @brief 屏蔽区字符串
             */
             virtual StringData* GetBlindPolygonString(unsigned int& nValueNum) = 0;

             /**
             * @brief 多边形点集
             * @param nIndex[IN] 数据索引
             */
             virtual PointF GetBlindPolygonPoints(int nIndex) = 0;

             /**
             * @brief 多边形点数
             * @param nIndex[IN] 数据索引
             */
             virtual int GetBlindPolygonPointNum(int nIndex) = 0;

             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

             /**
             * @brief 输出图像
             */
             virtual ImageBaseData GetOutputImage() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the SurfaceDefectFilter tool.  */
        ///表面缺陷滤波工具类
        class IMVSSurfaceDefectFilterModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSSurfaceDefectFilterModuTool() {}
            virtual ~IMVSSurfaceDefectFilterModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual SurfaceDefectFilterParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual SurfaceDefectFilterResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSSurfaceDefectFilterModuTool(IMVSSurfaceDefectFilterModuTool&);
            IMVSSurfaceDefectFilterModuTool& operator=(IMVSSurfaceDefectFilterModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for SurfaceDefectFilter.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSSurfaceDefectFilterModu::IMVSSurfaceDefectFilterModuTool * __stdcall GetSurfaceDefectFilterToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_SURFACEDEFECTFILTER_H_
