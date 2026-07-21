/***************************************************************************************************
 * File：IVmMatrixCircleFind.h
 * Note：@~chinese 阵列圆查找模块接口声明 @~english Interface for the MatrixCircleFind tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_MATRIXCIRCLEFIND_H_
#define _IVM_MATRIXCIRCLEFIND_H_

#include "MVD_AlgoParamDefine.h"
using namespace VisionDesigner;
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSMatrixCircleFindModu
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

    /// \addtogroup 阵列圆查找
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///阵列圆查找ROI管理类
        class MatrixCircleFindRoiManager
        {
        public:

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the MatrixCircleFindParams.  */
        ///阵列圆查找参数类
        class MatrixCircleFindParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MatrixCircleFindParams() {}
            virtual ~MatrixCircleFindParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual MatrixCircleFindRoiManager* GetModuRoiManager() = 0;

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

            /// @~chinese 卡尺宽度，范围：[1,500]   @~english CaliperWidth，range:[1,500]
            __declspec(property(put = SetRadWidth, get = GetRadWidth)) double RadWidth;

            virtual double GetRadWidth() = 0;

            virtual void SetRadWidth(double value) = 0;

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

            /// @~chinese 输出轮廓点   @~english Output Contour Point
            __declspec(property(put = SetOutputCoutourPoint, get = GetOutputCoutourPoint)) bool OutputCoutourPoint;

            virtual bool GetOutputCoutourPoint() = 0;

            virtual void SetOutputCoutourPoint(bool value) = 0;

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

            /// @~chinese 圆个数判断   @~english Find Circle Num Check
            __declspec(property(put = SetOutputCircleNumEnable, get = GetOutputCircleNumEnable)) bool OutputCircleNumEnable;

            virtual bool GetOutputCircleNumEnable() = 0;

            virtual void SetOutputCircleNumEnable(bool value) = 0;

            /// @~chinese 圆个数判断，范围：[1,25]   @~english Find Circle Num Check，range:[1,25]
            __declspec(property(put = SetOutputCircleNumLimitLow, get = GetOutputCircleNumLimitLow)) int OutputCircleNumLimitLow;

            virtual int GetOutputCircleNumLimitLow() = 0;

            virtual void SetOutputCircleNumLimitLow(int value) = 0;

            /// @~chinese 圆个数判断，范围：[1,25]   @~english Find Circle Num Check，range:[1,25]
            __declspec(property(put = SetOutputCircleNumLimitHigh, get = GetOutputCircleNumLimitHigh)) int OutputCircleNumLimitHigh;

            virtual int GetOutputCircleNumLimitHigh() = 0;

            virtual void SetOutputCircleNumLimitHigh(int value) = 0;

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
		/// 阵列圆查找轮廓点信息类
        class IMatrixCircleFindEdgePointInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IMatrixCircleFindEdgePointInfo() {}
            virtual ~IMatrixCircleFindEdgePointInfo() {}

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

        /*  Note: Interface Classes for describing edge points Information.  */
		/// 阵列圆查找结果信息类
        class IMatrixCircleFindResultInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit IMatrixCircleFindResultInfo() {}
            virtual ~IMatrixCircleFindResultInfo() {}

        public:
            /**
             * @brief 0:查找圆失败；1:查找圆成功并输出
            */
            virtual int GetOutputCircleAnnulusStatus() = 0;
            /**
            * @brief 获取输出圆环
            * @return 返回输出圆环
            */
            virtual Annulus GetOutputCircleAnnulus() = 0;
            /**
             * @brief 拟合圆率
            */
            virtual float GetCircleness() = 0;
            /**
             * @brief 卡尺数
            */
            virtual int GetCaliperNum() = 0;
            /**
            * @brief 获取结果中的单个卡尺位置
            * @param nCaliperIndex [IN] 卡尺索引，范围[0,CaliperNum)
            * @return 返回卡尺信息
            */
            virtual RectBox GetCaliperBox(int nCaliperIndex) = 0;
            /**
            * @brief 获取结果中的单个卡尺检测区位置
            * @param nCaliperIndex [IN] 卡尺索引，范围[0,卡尺个数)
            * @return 返回卡尺检测区信息
            */
            virtual RectBox GetDetectCaliperBox(int nDetectCaliperIndex) = 0;
        };
        
        
        /*  Note: Interface Classes of the result of the MatrixCircleFindResults.  */
        ///阵列圆查找结果类
        class MatrixCircleFindResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit MatrixCircleFindResults() {}
            virtual ~MatrixCircleFindResults() {}

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
             virtual RectBox GetROI(int nIndex) = 0;

             /**
              * @brief ROI圆弧
             */
             virtual Annulus GetROIAnnulus(int nIndex) = 0;

             /**
              * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

             /**
              * @brief 边缘点点数
             */
             virtual int GetPointNum(int nIndex) = 0;
             /**
             * @brief 获取结果中的单个边缘点信息
             * @param nIndex [IN] 边缘点信息
             * @return 返回结果边缘点序
             * @remarks nPtIndex从0开始计数。
             */
             virtual IMatrixCircleFindEdgePointInfo* GetEdgePointInfo(int nPtIndex) = 0;

             /**
              * @brief 输出圆环个数
             */
             virtual int GetMatrixCircleFindNum() = 0;

             /**
             * @brief 获取结果中的单个输出圆环信息
             * @param nPtIndex [IN] 边缘点信息
             * @return 返回单个输出圆环信息
             * @remarks nPtIndex从0开始计数。
             */
             virtual IMatrixCircleFindResultInfo* GetMatrixCircleFindInfo(int nPtIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the MatrixCircleFind tool.  */
        ///阵列圆查找工具类
        class IMVSMatrixCircleFindModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSMatrixCircleFindModuTool() {}
            virtual ~IMVSMatrixCircleFindModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual MatrixCircleFindParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual MatrixCircleFindResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSMatrixCircleFindModuTool(IMVSMatrixCircleFindModuTool&);
            IMVSMatrixCircleFindModuTool& operator=(IMVSMatrixCircleFindModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for MatrixCircleFind.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSMatrixCircleFindModu::IMVSMatrixCircleFindModuTool * __stdcall GetMatrixCircleFindToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_MATRIXCIRCLEFIND_H_
