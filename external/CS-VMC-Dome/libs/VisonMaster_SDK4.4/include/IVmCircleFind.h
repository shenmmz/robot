/***************************************************************************************************
 * File：IVmCircleFind.h
 * Note：@~chinese 圆查找模块接口声明 @~english Interface for the CircleFind tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CIRCLEFIND_H_
#define _IVM_CIRCLEFIND_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCircleFindModu
    {
        /// @~chinese 边缘类型   @~english Edge Type
        typedef enum _FindModeEnum
        {
            /// @~chinese 最强   @~english Strongest Edge
            FindMode_Best = 0x1,

            /// @~chinese 最后一条   @~english Last Edge
            FindMode_Largest = 0x2,

            /// @~chinese 第一条   @~english First Edge
            FindMode_SMALLEST = 0x3,

        }FindModeEnum;

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

        /// @~chinese 初始拟合   @~english Initial Fit
        typedef enum _InitTypeEnum
        {
            /// @~chinese 全局   @~english Global
            InitType_ALS = 0x1,

            /// @~chinese 局部   @~english Local
            InitType_LLS = 0x2,

        }InitTypeEnum;

        /// @~chinese 拟合方式   @~english Fit Mode
        typedef enum _FitFunEnum
        {
            /// @~chinese 最小二乘   @~english Least Squares
            FitFun_LS = 0x1,

            /// @~chinese huber   @~english huber
            FitFun_Huber = 0x2,

            /// @~chinese tukey   @~english tukey
            FitFun_Tukey = 0x3,

        }FitFunEnum;

    /// \addtogroup 圆查找
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///圆查找ROI管理类
        class CircleFindRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
            virtual void SetRoiAnnulus(Annulus stRoiValue) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the CircleFindParams.  */
        ///圆查找参数类
        class CircleFindParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CircleFindParams() {}
            virtual ~CircleFindParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual CircleFindRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 扇环半径，范围：[1,10000]   @~english Sector Radius，range:[1,10000]
            __declspec(property(put = SetMinRadius, get = GetMinRadius)) int MinRadius;

            virtual int GetMinRadius() = 0;

            virtual void SetMinRadius(int value) = 0;

            /// @~chinese 扇环半径，范围：[1,10000]   @~english Sector Radius，range:[1,10000]
            __declspec(property(put = SetMaxRadius, get = GetMaxRadius)) int MaxRadius;

            virtual int GetMaxRadius() = 0;

            virtual void SetMaxRadius(int value) = 0;

            /// @~chinese 边缘类型   @~english Edge Type
            __declspec(property(put = SetFindMode, get = GetFindMode)) FindModeEnum FindMode;

            virtual FindModeEnum GetFindMode() = 0;

            virtual void SetFindMode(FindModeEnum value) = 0;

            /// @~chinese 边缘极性   @~english EdgePolarity
            __declspec(property(put = SetEdgePolarity, get = GetEdgePolarity)) EdgePolarityEnum EdgePolarity;

            virtual EdgePolarityEnum GetEdgePolarity() = 0;

            virtual void SetEdgePolarity(EdgePolarityEnum value) = 0;

            /// @~chinese 边缘阈值，范围：[0,255]   @~english Edge Threshold，range:[0,255]
            __declspec(property(put = SetEdgeThresh, get = GetEdgeThresh)) int EdgeThresh;

            virtual int GetEdgeThresh() = 0;

            virtual void SetEdgeThresh(int value) = 0;

            /// @~chinese 滤波尺寸，范围：[1,50]   @~english Filter Size，range:[1,50]
            __declspec(property(put = SetEdgeWidth, get = GetEdgeWidth)) int EdgeWidth;

            virtual int GetEdgeWidth() = 0;

            virtual void SetEdgeWidth(int value) = 0;

            /// @~chinese 卡尺数量，范围：[3,1000]   @~english Caliper Number，range:[3,1000]
            __declspec(property(put = SetRadNum, get = GetRadNum)) int RadNum;

            virtual int GetRadNum() = 0;

            virtual void SetRadNum(int value) = 0;

            /// @~chinese 剔除点数，范围：[0,997]   @~english Reject Number，range:[0,997]
            __declspec(property(put = SetRejectNum, get = GetRejectNum)) int RejectNum;

            virtual int GetRejectNum() = 0;

            virtual void SetRejectNum(int value) = 0;

            /// @~chinese 初定位   @~english Initial Locating
            __declspec(property(put = SetCoarseDetectFlag, get = GetCoarseDetectFlag)) bool CoarseDetectFlag;

            virtual bool GetCoarseDetectFlag() = 0;

            virtual void SetCoarseDetectFlag(bool value) = 0;

            /// @~chinese 下采样系数，范围：[1,8]   @~english Subsampling Coefficient，range:[1,8]
            __declspec(property(put = SetCCDSampleScale, get = GetCCDSampleScale)) int CCDSampleScale;

            virtual int GetCCDSampleScale() = 0;

            virtual void SetCCDSampleScale(int value) = 0;

            /// @~chinese 圆定位敏感度，范围：[1,1000]   @~english Locating Sensitivity，range:[1,1000]
            __declspec(property(put = SetCCDCircleThresh, get = GetCCDCircleThresh)) int CCDCircleThresh;

            virtual int GetCCDCircleThresh() = 0;

            virtual void SetCCDCircleThresh(int value) = 0;

            /// @~chinese 剔除距离，范围：[1,1000]   @~english Distance to Remove，range:[1,1000]
            __declspec(property(put = SetRejectDist, get = GetRejectDist)) int RejectDist;

            virtual int GetRejectDist() = 0;

            virtual void SetRejectDist(int value) = 0;

            /// @~chinese 投影宽度，范围：[1,500]   @~english Projection Width，range:[1,500]
            __declspec(property(put = SetProLength, get = GetProLength)) int ProLength;

            virtual int GetProLength() = 0;

            virtual void SetProLength(int value) = 0;

            /// @~chinese 初始拟合   @~english Initial Fit
            __declspec(property(put = SetInitType, get = GetInitType)) InitTypeEnum InitType;

            virtual InitTypeEnum GetInitType() = 0;

            virtual void SetInitType(InitTypeEnum value) = 0;

            /// @~chinese 拟合方式   @~english Fit Mode
            __declspec(property(put = SetFitFun, get = GetFitFun)) FitFunEnum FitFun;

            virtual FitFunEnum GetFitFun() = 0;

            virtual void SetFitFun(FitFunEnum value) = 0;

            /// @~chinese 半径判断   @~english Radius Check
            __declspec(property(put = SetRadiusLimitEnable, get = GetRadiusLimitEnable)) bool RadiusLimitEnable;

            virtual bool GetRadiusLimitEnable() = 0;

            virtual void SetRadiusLimitEnable(bool value) = 0;

            /// @~chinese 半径判断，范围：[0,99999]   @~english Radius Check，range:[0,99999]
            __declspec(property(put = SetRadiusLimitLow, get = GetRadiusLimitLow)) double RadiusLimitLow;

            virtual double GetRadiusLimitLow() = 0;

            virtual void SetRadiusLimitLow(double value) = 0;

            /// @~chinese 半径判断，范围：[0,99999]   @~english Radius Check，range:[0,99999]
            __declspec(property(put = SetRadiusLimitHigh, get = GetRadiusLimitHigh)) double RadiusLimitHigh;

            virtual double GetRadiusLimitHigh() = 0;

            virtual void SetRadiusLimitHigh(double value) = 0;

            /// @~chinese 中心X判断   @~english Center X Check
            __declspec(property(put = SetCenterXLimitEnable, get = GetCenterXLimitEnable)) bool CenterXLimitEnable;

            virtual bool GetCenterXLimitEnable() = 0;

            virtual void SetCenterXLimitEnable(bool value) = 0;

            /// @~chinese 中心X判断，范围：[-99999,99999]   @~english Center X Check，range:[-99999,99999]
            __declspec(property(put = SetCenterXLimitLow, get = GetCenterXLimitLow)) double CenterXLimitLow;

            virtual double GetCenterXLimitLow() = 0;

            virtual void SetCenterXLimitLow(double value) = 0;

            /// @~chinese 中心X判断，范围：[-99999,99999]   @~english Center X Check，range:[-99999,99999]
            __declspec(property(put = SetCenterXLimitHigh, get = GetCenterXLimitHigh)) double CenterXLimitHigh;

            virtual double GetCenterXLimitHigh() = 0;

            virtual void SetCenterXLimitHigh(double value) = 0;

            /// @~chinese 中心Y判断   @~english Center Y Check
            __declspec(property(put = SetCenterYLimitEnable, get = GetCenterYLimitEnable)) bool CenterYLimitEnable;

            virtual bool GetCenterYLimitEnable() = 0;

            virtual void SetCenterYLimitEnable(bool value) = 0;

            /// @~chinese 中心Y范围，范围：[-99999,99999]   @~english Center Y Range，range:[-99999,99999]
            __declspec(property(put = SetCenterYLimitLow, get = GetCenterYLimitLow)) double CenterYLimitLow;

            virtual double GetCenterYLimitLow() = 0;

            virtual void SetCenterYLimitLow(double value) = 0;

            /// @~chinese 中心Y范围，范围：[-99999,99999]   @~english Center Y Range，range:[-99999,99999]
            __declspec(property(put = SetCenterYLimitHigh, get = GetCenterYLimitHigh)) double CenterYLimitHigh;

            virtual double GetCenterYLimitHigh() = 0;

            virtual void SetCenterYLimitHigh(double value) = 0;

            /// @~chinese 拟合误差判断   @~english Fit Error Check
            __declspec(property(put = SetFitErrorLimitEnable, get = GetFitErrorLimitEnable)) bool FitErrorLimitEnable;

            virtual bool GetFitErrorLimitEnable() = 0;

            virtual void SetFitErrorLimitEnable(bool value) = 0;

            /// @~chinese 拟合误差范围，范围：[0,100.0]   @~english Fit Error Range，range:[0,100.0]
            __declspec(property(put = SetFitErrorLimitLow, get = GetFitErrorLimitLow)) double FitErrorLimitLow;

            virtual double GetFitErrorLimitLow() = 0;

            virtual void SetFitErrorLimitLow(double value) = 0;

            /// @~chinese 拟合误差范围，范围：[0,100.0]   @~english Fit Error Range，range:[0,100.0]
            __declspec(property(put = SetFitErrorLimitHigh, get = GetFitErrorLimitHigh)) double FitErrorLimitHigh;

            virtual double GetFitErrorLimitHigh() = 0;

            virtual void SetFitErrorLimitHigh(double value) = 0;

            /// @~chinese 匹配点数判断   @~english Fit Points Check
            __declspec(property(put = SetFitPointsLimitEnable, get = GetFitPointsLimitEnable)) bool FitPointsLimitEnable;

            virtual bool GetFitPointsLimitEnable() = 0;

            virtual void SetFitPointsLimitEnable(bool value) = 0;

            /// @~chinese 拟合点数范围，范围：[3,99999]   @~english Fit Points Range，range:[3,99999]
            __declspec(property(put = SetFitPointsLimitLow, get = GetFitPointsLimitLow)) int FitPointsLimitLow;

            virtual int GetFitPointsLimitLow() = 0;

            virtual void SetFitPointsLimitLow(int value) = 0;

            /// @~chinese 拟合点数范围，范围：[3,99999]   @~english Fit Points Range，range:[3,99999]
            __declspec(property(put = SetFitPointsLimitHigh, get = GetFitPointsLimitHigh)) int FitPointsLimitHigh;

            virtual int GetFitPointsLimitHigh() = 0;

            virtual void SetFitPointsLimitHigh(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /** @enum  MVD_EDGEPOINT_STATUS
         *  @brief Find status of the edge point
         */
		/// @~chinese 拟合点状态 @~english status of the edge point
        typedef _MVD_EDGEPOINT_STATUS__ MVD_EDGEPOINT_STATUS;

        /** @enum  MVD_EDGEPOINT_STATUS
         *  @brief Type of the polarity of the edge point
         */
		/// @~chinese 边缘极性 @~english edge polarity
        typedef _MVD_EDGE_POLARITY__ MVD_EDGE_POLARITY;

        /*  Note: Interface Classes for describing edge points Information.  */
		/// 圆查找轮廓点信息类
        class ICircleFindEdgePointInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit ICircleFindEdgePointInfo() {}
            virtual ~ICircleFindEdgePointInfo() {}

        public:
            /**
             * @brief 边缘点坐标
             */
            virtual MVD_POINT_F GetEdgePoint() = 0;
            /**
             * @brief 边缘点状态
             */
            virtual MVD_EDGEPOINT_STATUS GetStatus() = 0;
        };

        /*  Note: Interface Classes of the result of the IMVSCircleFindModuTool.  */
        class CircleFindResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CircleFindResults() {}
            virtual ~CircleFindResults() {}

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
             * @brief 卡尺检测区
             */
             virtual RectBox GetDetectCaliperBox(int nIndex) = 0;
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;
             /**
             * @brief ROI圆弧
             */
             virtual Annulus GetROIAnnulus() = 0;
            /**
             * @brief 0:查找圆失败；1:查找圆成功并输出
             * @return 查找状态
             */
            virtual int GetStatus() = 0;
            /**
             * @brief 拟合圆心
             */
            virtual MVD_POINT_F GetCircleCenter() = 0;
            /**
             * @brief 拟合圆半径
             */
            virtual float GetCircleRadius() = 0;
            /**
             * @brief 拟合误差
             */
            virtual float GetCircleness() = 0;
            /**
             * @brief 边缘点点数
             */
            virtual int GetPointNum() = 0;
            /**
             * @brief 获取结果中的单个边缘点信息
             * @param nPtIndex [OUT] 边缘点信息
             * @return 返回结果边缘点序
             * @remarks nPtIndex从0开始计数
             */
            /* Gets information of the point with an index value of 'nPtIndex'. The range of index value can be queried from GetPointNum function. */
            virtual ICircleFindEdgePointInfo* GetEdgePointInfo(int nPtIndex) = 0;
            /**
             * @brief 获取拟合圆弧
             * @return 返回拟合圆弧的位置
             */
            virtual Annulus GetFitArc() = 0;
            /**
            * @brief 获取输出圆环
            * @return 返回输出圆环
            */
            virtual Annulus GetOutputCircleAnnulus() = 0;
            /**
             * @brief 卡尺数
             */
            virtual int GetCaliperNum() = 0;
            /**
             * @brief 获取结果中的单个卡尺位置
             * @param nCaliperIndex [IN] 卡尺索引
             * @return 返回卡尺信息
             * @remarks nPtIndex从0开始计数
             */
            virtual RectBox GetCaliperBox(int nCaliperIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CircleFind tool.  */
        ///圆查找工具类
        class IMVSCircleFindModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCircleFindModuTool() {}
            virtual ~IMVSCircleFindModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CircleFindParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CircleFindResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSCircleFindModuTool(IMVSCircleFindModuTool&);
            IMVSCircleFindModuTool& operator=(IMVSCircleFindModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CircleFind.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSCircleFindModu::IMVSCircleFindModuTool * __stdcall GetCircleFindToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CIRCLEFIND_H_
