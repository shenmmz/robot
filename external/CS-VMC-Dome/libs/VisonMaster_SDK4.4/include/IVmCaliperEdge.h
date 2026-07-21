/***************************************************************************************************
 * File：IVmCaliperEdge.h
 * Note：@~chinese 边缘查找模块接口声明 @~english Interface for the CaliperEdge tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CALIPEREDGE_H_
#define _IVM_CALIPEREDGE_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCaliperEdgeModu
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

        /// @~chinese 边缘选择方式   @~english Edge Choose Mode
        typedef enum _FindModeEnum
        {
            /// @~chinese 按强度   @~english By Strength
            FindMode_Best = 0x1,

            /// @~chinese 从前往后   @~english Front to Back
            FindMode_First = 0x2,

            /// @~chinese 从后往前   @~english Back to Front
            FindMode_Last = 0x3,

            /// @~chinese 所有   @~english All
            FindMode_All = 0x4,

        }FindModeEnum;

        /// @~chinese 排序方式   @~english Sort Type
        typedef enum _SortTypeEnum
        {
            /// @~chinese 分数降序   @~english Score Descend
            SortType_ScoreDescend = 0x0,

            /// @~chinese 分数升序   @~english Score Ascend
            SortType_ScoreAscend = 0x1,

            /// @~chinese 方向正向   @~english Orient Forward
            SortType_OrientForward = 0x2,

            /// @~chinese 方向逆向   @~english Orient Backward
            SortType_OrientBackward = 0x3,

        }SortTypeEnum;

        /// @~chinese 查找方向   @~english Search Direction
        typedef enum _FindOrientEnum
        {
            /// @~chinese 从上到下   @~english Top to Bottom
            FindOrient_UpToDown = 0x1,

            /// @~chinese 从左到右   @~english Left to Right
            FindOrient_LeftToRight = 0x2,

        }FindOrientEnum;

    /// \addtogroup 边缘查找
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///边缘查找ROI管理类
        class CaliperEdgeRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the CaliperEdgeParams.  */
        ///边缘查找参数类
        class CaliperEdgeParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CaliperEdgeParams() {}
            virtual ~CaliperEdgeParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual CaliperEdgeRoiManager* GetModuRoiManager() = 0;

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

            /// @~chinese 边缘选择方式   @~english Edge Choose Mode
            __declspec(property(put = SetFindMode, get = GetFindMode)) FindModeEnum FindMode;

            virtual FindModeEnum GetFindMode() = 0;

            virtual void SetFindMode(FindModeEnum value) = 0;

            /// @~chinese 最大结果数，范围：[1,1000]   @~english Max Result Number，range:[1,1000]
            __declspec(property(put = SetMaximum, get = GetMaximum)) int Maximum;

            virtual int GetMaximum() = 0;

            virtual void SetMaximum(int value) = 0;

            /// @~chinese 排序方式   @~english Sort Type
            __declspec(property(put = SetSortType, get = GetSortType)) SortTypeEnum SortType;

            virtual SortTypeEnum GetSortType() = 0;

            virtual void SetSortType(SortTypeEnum value) = 0;

            /// @~chinese 查找方向   @~english Search Direction
            __declspec(property(put = SetFindOrient, get = GetFindOrient)) FindOrientEnum FindOrient;

            virtual FindOrientEnum GetFindOrient() = 0;

            virtual void SetFindOrient(FindOrientEnum value) = 0;

            /// @~chinese 最小边缘分数，范围：[0,1.0]   @~english Min Edge Score，range:[0,1.0]
            __declspec(property(put = SetMinScore, get = GetMinScore)) double MinScore;

            virtual double GetMinScore() = 0;

            virtual void SetMinScore(double value) = 0;

            /// @~chinese 数量判断   @~english Quantity Check
            __declspec(property(put = SetNumLimitEnable, get = GetNumLimitEnable)) bool NumLimitEnable;

            virtual bool GetNumLimitEnable() = 0;

            virtual void SetNumLimitEnable(bool value) = 0;

            /// @~chinese 数量判断，范围：[0,99999]   @~english Quantity Check，range:[0,99999]
            __declspec(property(put = SetNumLimitLow, get = GetNumLimitLow)) int NumLimitLow;

            virtual int GetNumLimitLow() = 0;

            virtual void SetNumLimitLow(int value) = 0;

            /// @~chinese 数量判断，范围：[0,99999]   @~english Quantity Check，range:[0,99999]
            __declspec(property(put = SetNumLimitHigh, get = GetNumLimitHigh)) int NumLimitHigh;

            virtual int GetNumLimitHigh() = 0;

            virtual void SetNumLimitHigh(int value) = 0;

            /// @~chinese 边缘点X判断   @~english Edge Point X Check
            __declspec(property(put = SetEdgePointXLimitEnable, get = GetEdgePointXLimitEnable)) bool EdgePointXLimitEnable;

            virtual bool GetEdgePointXLimitEnable() = 0;

            virtual void SetEdgePointXLimitEnable(bool value) = 0;

            /// @~chinese 边缘点X判断，范围：[0,99999]   @~english Edge Point X Check，range:[0,99999]
            __declspec(property(put = SetEdgePointXLimitLow, get = GetEdgePointXLimitLow)) double EdgePointXLimitLow;

            virtual double GetEdgePointXLimitLow() = 0;

            virtual void SetEdgePointXLimitLow(double value) = 0;

            /// @~chinese 边缘点X判断，范围：[0,99999]   @~english Edge Point X Check，range:[0,99999]
            __declspec(property(put = SetEdgePointXLimitHigh, get = GetEdgePointXLimitHigh)) double EdgePointXLimitHigh;

            virtual double GetEdgePointXLimitHigh() = 0;

            virtual void SetEdgePointXLimitHigh(double value) = 0;

            /// @~chinese 边缘点Y判断   @~english Edge Point Y Check
            __declspec(property(put = SetEdgePointYLimitEnable, get = GetEdgePointYLimitEnable)) bool EdgePointYLimitEnable;

            virtual bool GetEdgePointYLimitEnable() = 0;

            virtual void SetEdgePointYLimitEnable(bool value) = 0;

            /// @~chinese 边缘点Y判断，范围：[0,99999]   @~english Edge Point Y Check，range:[0,99999]
            __declspec(property(put = SetEdgePointYLimitLow, get = GetEdgePointYLimitLow)) double EdgePointYLimitLow;

            virtual double GetEdgePointYLimitLow() = 0;

            virtual void SetEdgePointYLimitLow(double value) = 0;

            /// @~chinese 边缘点Y判断，范围：[0,99999]   @~english Edge Point Y Check，range:[0,99999]
            __declspec(property(put = SetEdgePointYLimitHigh, get = GetEdgePointYLimitHigh)) double EdgePointYLimitHigh;

            virtual double GetEdgePointYLimitHigh() = 0;

            virtual void SetEdgePointYLimitHigh(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /** @enum  MVD_EDGEPOINT_STATUS
         *  @brief Type of the polarity of the edge point
         */
		/// @~chinese 边缘极性 @~english edge polarity
        typedef _MVD_EDGE_POLARITY__ MVD_EDGE_POLARITY;

        /*  Note: Interface Classes for describing single edge Information.  */
        /// 边缘信息
        class IEdgeFindSingleEdgeInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IEdgeFindSingleEdgeInfo() {}
            virtual ~IEdgeFindSingleEdgeInfo() {}

        public:
            /**
             * @brief 边缘点
             */
            virtual MVD_POINT_F GetEdgePoint() = 0;
            /**
             * @brief 边缘点偏移量
             */
            virtual float GetBias() = 0;
            /**
             * @brief 边缘点得分
             */
            virtual float GetScore() = 0;
            /**
             * @brief 边缘极性
             */
            virtual MVD_EDGE_POLARITY GetPolarity() = 0;
            /**
             * @brief 直线
             */
            virtual Line GetLineInfo() = 0;
        };
        /*  Note: Interface Classes of the result of the CaliperEdgeResults.  */
        ///边缘查找结果类
        class CaliperEdgeResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CaliperEdgeResults() {}
            virtual ~CaliperEdgeResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;
            /**
             * @brief 边缘信息
             */
            virtual int GetEdgeNum() = 0;
            /**
             * @brief 边缘点数
             */
            virtual IEdgeFindSingleEdgeInfo* GetSingleEdgeInfo( int nIndex ) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CaliperEdge tool.  */
        ///边缘查找工具类
        class IMVSCaliperEdgeModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCaliperEdgeModuTool() {}
            virtual ~IMVSCaliperEdgeModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CaliperEdgeParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CaliperEdgeResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSCaliperEdgeModuTool(IMVSCaliperEdgeModuTool&);
            IMVSCaliperEdgeModuTool& operator=(IMVSCaliperEdgeModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CaliperEdge.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSCaliperEdgeModu::IMVSCaliperEdgeModuTool * __stdcall GetCaliperEdgeToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CALIPEREDGE_H_
