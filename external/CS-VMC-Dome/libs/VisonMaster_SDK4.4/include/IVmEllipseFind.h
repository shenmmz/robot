/***************************************************************************************************
 * File：IVmEllipseFind.h
 * Note：@~chinese 椭圆查找模块接口声明 @~english Interface for the EllipseFind tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_ELLIPSEFIND_H_
#define _IVM_ELLIPSEFIND_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSEllipseFindModu
    {
        /// @~chinese 边缘类型   @~english Edge Type
        typedef enum _EllipseFindModeEnum
        {
            /// @~chinese 最强   @~english Strongest Edge
            EllipseFindMode_Best = 0x1,

            /// @~chinese 最后一条   @~english Last Edge
            EllipseFindMode_Largest = 0x2,

            /// @~chinese 第一条   @~english First Edge
            EllipseFindMode_SMALLEST = 0x3,

        }EllipseFindModeEnum;

        /// @~chinese 边缘极性   @~english EdgePolarity
        typedef enum _EdgePolarityEnum
        {
            /// @~chinese 从黑到白   @~english Dark to Light
            EdgePolarity_BlackToWhite = 0x1,

            /// @~chinese 从白到黑   @~english Light to Dark
            EdgePolarity_WhiteToBlack = 0x2,

            /// @~chinese 任意极性   @~english Both
            EdgePolarity_Both = 0x3,

        }EdgePolarityEnum;

    /// \addtogroup 椭圆查找
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///椭圆查找ROI管理类
        class EllipseFindRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the EllipseFindParams.  */
        ///椭圆查找参数类
        class EllipseFindParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit EllipseFindParams() {}
            virtual ~EllipseFindParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual EllipseFindRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 缩放范围，范围：[0.1,2]   @~english Zoom Range，range:[0.1,2]
            __declspec(property(put = SetScaleInside, get = GetScaleInside)) double ScaleInside;

            virtual double GetScaleInside() = 0;

            virtual void SetScaleInside(double value) = 0;

            /// @~chinese 缩放范围，范围：[0.1,2]   @~english Zoom Range，range:[0.1,2]
            __declspec(property(put = SetScaleOutside, get = GetScaleOutside)) double ScaleOutside;

            virtual double GetScaleOutside() = 0;

            virtual void SetScaleOutside(double value) = 0;

            /// @~chinese 边缘类型   @~english Edge Type
            __declspec(property(put = SetEllipseFindMode, get = GetEllipseFindMode)) EllipseFindModeEnum EllipseFindMode;

            virtual EllipseFindModeEnum GetEllipseFindMode() = 0;

            virtual void SetEllipseFindMode(EllipseFindModeEnum value) = 0;

            /// @~chinese 边缘极性   @~english EdgePolarity
            __declspec(property(put = SetEdgePolarity, get = GetEdgePolarity)) EdgePolarityEnum EdgePolarity;

            virtual EdgePolarityEnum GetEdgePolarity() = 0;

            virtual void SetEdgePolarity(EdgePolarityEnum value) = 0;

            /// @~chinese 边缘阈值，范围：[1,255]   @~english Edge Threshold，range:[1,255]
            __declspec(property(put = SetEdgeThresh, get = GetEdgeThresh)) int EdgeThresh;

            virtual int GetEdgeThresh() = 0;

            virtual void SetEdgeThresh(int value) = 0;

            /// @~chinese 卡尺数量，范围：[6,1000]   @~english Caliper Number，range:[6,1000]
            __declspec(property(put = SetRaysNum, get = GetRaysNum)) int RaysNum;

            virtual int GetRaysNum() = 0;

            virtual void SetRaysNum(int value) = 0;

            /// @~chinese 滤波尺寸，范围：[1,50]   @~english Filter Size，range:[1,50]
            __declspec(property(put = SetEdgeWidth, get = GetEdgeWidth)) int EdgeWidth;

            virtual int GetEdgeWidth() = 0;

            virtual void SetEdgeWidth(int value) = 0;

            /// @~chinese 投影宽度，范围：[1,100]   @~english Projection Width，range:[1,100]
            __declspec(property(put = SetRegionWidth, get = GetRegionWidth)) int RegionWidth;

            virtual int GetRegionWidth() = 0;

            virtual void SetRegionWidth(int value) = 0;

            /// @~chinese 误差容忍度，范围：[1,200]   @~english ErrorTolerance，range:[1,200]
            __declspec(property(put = SetFitErrorTolerance, get = GetFitErrorTolerance)) int FitErrorTolerance;

            virtual int GetFitErrorTolerance() = 0;

            virtual void SetFitErrorTolerance(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the EllipseFindResults.  */
        ///椭圆查找结果类
        class EllipseFindResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit EllipseFindResults() {}
            virtual ~EllipseFindResults() {}

        public:
             /**
             * @brief 屏蔽区字符串
             */
             virtual StringData* GetBlindPolygonString(unsigned int& nValueNum) = 0;

             /**
             * @brief 屏蔽区多边形点集
             */
             virtual PointF GetBlindPolygonPoints(int nIndex) = 0;

             /**
             * @brief 屏蔽区多边形点数
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
             * @brief 椭圆轮廓点
             */
			 virtual PointF GetEllipseCoutourPoint(int nIndex) = 0;
			 /**
             * @brief 轮廓点状态
			 */
			 virtual int GetEdgePointStatus(int nIndex) = 0;
             /**
             * @brief 边缘点个数
             */
             virtual int GetEdgePointNum() = 0;
			 /**
             * @brief 输出椭圆
			 */
			 virtual Ellipse GetOutputEllipse() = 0;
			 /**
             * @brief 椭圆环
			 */
			 virtual Ellipse GetOutputEllipseRing(int nIndex) = 0;
			 /**
             * @brief 椭圆环数量
			 */
			 virtual int GetOutputEllipseRingNum() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the EllipseFind tool.  */
        ///椭圆查找工具类
        class IMVSEllipseFindModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSEllipseFindModuTool() {}
            virtual ~IMVSEllipseFindModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual EllipseFindParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual EllipseFindResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSEllipseFindModuTool(IMVSEllipseFindModuTool&);
            IMVSEllipseFindModuTool& operator=(IMVSEllipseFindModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for EllipseFind.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSEllipseFindModu::IMVSEllipseFindModuTool * __stdcall GetEllipseFindToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_ELLIPSEFIND_H_
