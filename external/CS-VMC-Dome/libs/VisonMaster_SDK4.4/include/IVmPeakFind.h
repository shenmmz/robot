/***************************************************************************************************
 * File：IVmPeakFind.h
 * Note：@~chinese 顶点检测模块接口声明 @~english Interface for the PeakFind tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_PEAKFIND_H_
#define _IVM_PEAKFIND_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSPeakFindModu
    {
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

    /// \addtogroup 顶点检测
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///顶点检测ROI管理类
        class PeakFindRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

            /// @~chinese 掩膜图像（仅当次执行起效）   @~english Mask Image(Only valid for this execution)
            virtual void SetMaskImage(ImageBaseData stMaskImage) = 0;

        };

        /*  Note: Interface Classes of the parameter of the PeakFindParams.  */
        ///顶点检测参数类
        class PeakFindParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit PeakFindParams() {}
            virtual ~PeakFindParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual PeakFindRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 滤波尺寸，范围：[1,50]   @~english Filter Size，range:[1,50]
            __declspec(property(put = SetHalfKernelSize, get = GetHalfKernelSize)) int HalfKernelSize;

            virtual int GetHalfKernelSize() = 0;

            virtual void SetHalfKernelSize(int value) = 0;

            /// @~chinese 边缘阈值，范围：[0,255]   @~english Edge Threshold，range:[0,255]
            __declspec(property(put = SetContrastTH, get = GetContrastTH)) int ContrastTH;

            virtual int GetContrastTH() = 0;

            virtual void SetContrastTH(int value) = 0;

            /// @~chinese 边缘极性   @~english EdgePolarity
            __declspec(property(put = SetEdgePolarity, get = GetEdgePolarity)) EdgePolarityEnum EdgePolarity;

            virtual EdgePolarityEnum GetEdgePolarity() = 0;

            virtual void SetEdgePolarity(EdgePolarityEnum value) = 0;

            /// @~chinese 扫描宽度，范围：[1,100]   @~english Scan Width，range:[1,100]
            __declspec(property(put = SetScanWidth, get = GetScanWidth)) int ScanWidth;

            virtual int GetScanWidth() = 0;

            virtual void SetScanWidth(int value) = 0;

            /// @~chinese 数量判断   @~english Quantity Check
            __declspec(property(put = SetNumLimitEnable, get = GetNumLimitEnable)) bool NumLimitEnable;

            virtual bool GetNumLimitEnable() = 0;

            virtual void SetNumLimitEnable(bool value) = 0;

            /// @~chinese 数量范围，范围：[0,99999]   @~english Quantity Range，range:[0,99999]
            __declspec(property(put = SetNumLimitLow, get = GetNumLimitLow)) int NumLimitLow;

            virtual int GetNumLimitLow() = 0;

            virtual void SetNumLimitLow(int value) = 0;

            /// @~chinese 数量范围，范围：[0,99999]   @~english Quantity Range，range:[0,99999]
            __declspec(property(put = SetNumLimitHigh, get = GetNumLimitHigh)) int NumLimitHigh;

            virtual int GetNumLimitHigh() = 0;

            virtual void SetNumLimitHigh(int value) = 0;

            /// @~chinese 边缘点X判断   @~english Edge Point X Check
            __declspec(property(put = SetEdgePointXLimitEnable, get = GetEdgePointXLimitEnable)) bool EdgePointXLimitEnable;

            virtual bool GetEdgePointXLimitEnable() = 0;

            virtual void SetEdgePointXLimitEnable(bool value) = 0;

            /// @~chinese 边缘点X范围，范围：[-99999,99999]   @~english Edge Point X Range，range:[-99999,99999]
            __declspec(property(put = SetEdgePointXLimitLow, get = GetEdgePointXLimitLow)) double EdgePointXLimitLow;

            virtual double GetEdgePointXLimitLow() = 0;

            virtual void SetEdgePointXLimitLow(double value) = 0;

            /// @~chinese 边缘点X范围，范围：[-99999,99999]   @~english Edge Point X Range，range:[-99999,99999]
            __declspec(property(put = SetEdgePointXLimitHigh, get = GetEdgePointXLimitHigh)) double EdgePointXLimitHigh;

            virtual double GetEdgePointXLimitHigh() = 0;

            virtual void SetEdgePointXLimitHigh(double value) = 0;

            /// @~chinese 边缘点Y判断   @~english Edge Point Y Check
            __declspec(property(put = SetEdgePointYLimitEnable, get = GetEdgePointYLimitEnable)) bool EdgePointYLimitEnable;

            virtual bool GetEdgePointYLimitEnable() = 0;

            virtual void SetEdgePointYLimitEnable(bool value) = 0;

            /// @~chinese 边缘点Y范围，范围：[-99999,99999]   @~english Edge Point Y Range，range:[-99999,99999]
            __declspec(property(put = SetEdgePointYLimitLow, get = GetEdgePointYLimitLow)) double EdgePointYLimitLow;

            virtual double GetEdgePointYLimitLow() = 0;

            virtual void SetEdgePointYLimitLow(double value) = 0;

            /// @~chinese 边缘点Y范围，范围：[-99999,99999]   @~english Edge Point Y Range，range:[-99999,99999]
            __declspec(property(put = SetEdgePointYLimitHigh, get = GetEdgePointYLimitHigh)) double EdgePointYLimitHigh;

            virtual double GetEdgePointYLimitHigh() = 0;

            virtual void SetEdgePointYLimitHigh(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /** @enum  MVD_EDGEPOINT_STATUS
        *  @brief Find status of the edge point
        */
		/// @~chinese 边缘点状态 @~english Find status of the edge point
        typedef _MVD_EDGEPOINT_STATUS__ MVD_EDGEPOINT_STATUS;

        /** @enum  MVD_EDGEPOINT_STATUS
        *  @brief Type of the polarity of the edge point
        */
		/// @~chinese 边缘点极性 @~english Type of the polarity of the edge point
        typedef _MVD_EDGE_POLARITY__ MVD_EDGE_POLARITY;

        /*  Note: Interface Classes for describing a edge point Information.  */
		/// 边缘点信息
        class IPeakFindEdgePointInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IPeakFindEdgePointInfo() {}
            virtual ~IPeakFindEdgePointInfo() {}

        public:
             /**
             * @brief 边缘点坐标
             */
            virtual MVD_POINT_F GetEdgePoint() = 0;
            /**
             * @brief 边缘点得分
             */
            virtual float GetScore() = 0;
            /**
             * @brief 边缘点距离
             */
            virtual float GetDistance() = 0;
            /**
             * @brief 边缘点极性
             */
            virtual MVD_EDGE_POLARITY GetPolarity() = 0;
            /**
             * @brief 边缘点状态
             */
            virtual MVD_EDGEPOINT_STATUS GetStatus() = 0;
        };

        /*  Note: Interface Classes of the result of the PeakFindResults.  */
        ///顶点检测结果类
        class PeakFindResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit PeakFindResults() {}
            virtual ~PeakFindResults() {}

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
             * @brief 输出掩膜
             */
             virtual ImageBaseData GetOutputMask() = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;
            /**
             * @brief 顶点信息
             */
            virtual IPeakFindEdgePointInfo* GetPeakPointInfo() = 0;
            /**
             * @brief 边缘点点数
             */
            virtual int GetEdgePointNum() = 0;
            /**
             * @brief 获取结果中的边缘点信息
             * @param nPtIndex [IN] 边缘点索引
             * @return 返回结果边缘点序
             * @remarks nPtIndex从0开始计数。
             */
            /* Gets information of the point with an index value of 'nPtIndex'.
            The range of index value can be queried from GetEdgePointNum function. */
            virtual IPeakFindEdgePointInfo* GetEdgePointInfo(int nPtIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the PeakFind tool.  */
        ///顶点检测工具类
        class IMVSPeakFindModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSPeakFindModuTool() {}
            virtual ~IMVSPeakFindModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual PeakFindParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual PeakFindResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSPeakFindModuTool(IMVSPeakFindModuTool&);
            IMVSPeakFindModuTool& operator=(IMVSPeakFindModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for PeakFind.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSPeakFindModu::IMVSPeakFindModuTool * __stdcall GetPeakFindToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_PEAKFIND_H_
