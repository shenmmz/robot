/***************************************************************************************************
 * File：IVmLineFind.h
 * Note：@~chinese 直线查找模块接口声明 @~english Interface for the LineFind tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_LINEFIND_H_
#define _IVM_LINEFIND_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSLineFindModu
    {
        /// @~chinese 边缘类型   @~english Edge Type
        typedef enum _FindModeEnum
        {
            /// @~chinese 最强   @~english Strongest Edge
            FindMode_Best = 0x1,

            /// @~chinese 第一条   @~english First Edge
            FindMode_First = 0x2,

            /// @~chinese 最后一条   @~english Last Edge
            FindMode_Last = 0x3,

            /// @~chinese 接近中线   @~english Close To Mid
            FindMode_Mid = 0x4,

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

    /// \addtogroup 直线查找
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///直线查找ROI管理类
        class LineFindRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the LineFindParams.  */
        ///直线查找参数类
        class LineFindParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit LineFindParams() {}
            virtual ~LineFindParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual LineFindRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 边缘类型   @~english Edge Type
            __declspec(property(put = SetFindMode, get = GetFindMode)) FindModeEnum FindMode;

            virtual FindModeEnum GetFindMode() = 0;

            virtual void SetFindMode(FindModeEnum value) = 0;

            /// @~chinese 边缘极性   @~english EdgePolarity
            __declspec(property(put = SetEdgePolarity, get = GetEdgePolarity)) EdgePolarityEnum EdgePolarity;

            virtual EdgePolarityEnum GetEdgePolarity() = 0;

            virtual void SetEdgePolarity(EdgePolarityEnum value) = 0;

            /// @~chinese 边缘阈值，范围：[1,255]   @~english Edge Threshold，range:[1,255]
            __declspec(property(put = SetEdgeStrength, get = GetEdgeStrength)) int EdgeStrength;

            virtual int GetEdgeStrength() = 0;

            virtual void SetEdgeStrength(int value) = 0;

            /// @~chinese 滤波尺寸，范围：[1,50]   @~english Filter Size，range:[1,50]
            __declspec(property(put = SetKernelSize, get = GetKernelSize)) int KernelSize;

            virtual int GetKernelSize() = 0;

            virtual void SetKernelSize(int value) = 0;

            /// @~chinese 卡尺数量，范围：[2,1000]   @~english Caliper Number，range:[2,1000]
            __declspec(property(put = SetRayNum, get = GetRayNum)) int RayNum;

            virtual int GetRayNum() = 0;

            virtual void SetRayNum(int value) = 0;

            /// @~chinese 直线查找反向   @~english RevertFindOrient
            __declspec(property(put = SetRevertFindOrient, get = GetRevertFindOrient)) bool RevertFindOrient;

            virtual bool GetRevertFindOrient() = 0;

            virtual void SetRevertFindOrient(bool value) = 0;

            /// @~chinese 角度归一化   @~english AngleNormalization
            __declspec(property(put = SetAngleNormalization, get = GetAngleNormalization)) bool AngleNormalization;

            virtual bool GetAngleNormalization() = 0;

            virtual void SetAngleNormalization(bool value) = 0;

            /// @~chinese 角度使能   @~english Line Angle Enable
            __declspec(property(put = SetLineAngleEnable, get = GetLineAngleEnable)) bool LineAngleEnable;

            virtual bool GetLineAngleEnable() = 0;

            virtual void SetLineAngleEnable(bool value) = 0;

            /// @~chinese 期望直线角度，范围：[-180,180]   @~english Expect Angle，range:[-180,180]
            __declspec(property(put = SetExpectAngle, get = GetExpectAngle)) double ExpectAngle;

            virtual double GetExpectAngle() = 0;

            virtual void SetExpectAngle(double value) = 0;

            /// @~chinese 角度旋转容忍，范围：[0,180]   @~english Rotate Tolerance，range:[0,180]
            __declspec(property(put = SetRotateTolerance, get = GetRotateTolerance)) double RotateTolerance;

            virtual double GetRotateTolerance() = 0;

            virtual void SetRotateTolerance(double value) = 0;

            /// @~chinese 剔除点数，范围：[0,998]   @~english Reject Number，range:[0,998]
            __declspec(property(put = SetRejectNum, get = GetRejectNum)) int RejectNum;

            virtual int GetRejectNum() = 0;

            virtual void SetRejectNum(int value) = 0;

            /// @~chinese 剔除距离，范围：[1,1000]   @~english Distance to Remove，range:[1,1000]
            __declspec(property(put = SetRejectDist, get = GetRejectDist)) int RejectDist;

            virtual int GetRejectDist() = 0;

            virtual void SetRejectDist(int value) = 0;

            /// @~chinese 投影宽度，范围：[1,100]   @~english Projection Width，range:[1,100]
            __declspec(property(put = SetRegionWidth, get = GetRegionWidth)) int RegionWidth;

            virtual int GetRegionWidth() = 0;

            virtual void SetRegionWidth(int value) = 0;

            /// @~chinese 初始拟合   @~english Initial Fit
            __declspec(property(put = SetInitType, get = GetInitType)) InitTypeEnum InitType;

            virtual InitTypeEnum GetInitType() = 0;

            virtual void SetInitType(InitTypeEnum value) = 0;

            /// @~chinese 拟合方式   @~english Fit Mode
            __declspec(property(put = SetFitFun, get = GetFitFun)) FitFunEnum FitFun;

            virtual FitFunEnum GetFitFun() = 0;

            virtual void SetFitFun(FitFunEnum value) = 0;

            /// @~chinese 匹配点数判断   @~english Fit Points Check
            __declspec(property(put = SetFitPointsLimitEnable, get = GetFitPointsLimitEnable)) bool FitPointsLimitEnable;

            virtual bool GetFitPointsLimitEnable() = 0;

            virtual void SetFitPointsLimitEnable(bool value) = 0;

            /// @~chinese 拟合点数范围，范围：[2,99999]   @~english Fit Points Range，range:[2,99999]
            __declspec(property(put = SetFitPointsLimitLow, get = GetFitPointsLimitLow)) int FitPointsLimitLow;

            virtual int GetFitPointsLimitLow() = 0;

            virtual void SetFitPointsLimitLow(int value) = 0;

            /// @~chinese 拟合点数范围，范围：[2,99999]   @~english Fit Points Range，range:[2,99999]
            __declspec(property(put = SetFitPointsLimitHigh, get = GetFitPointsLimitHigh)) int FitPointsLimitHigh;

            virtual int GetFitPointsLimitHigh() = 0;

            virtual void SetFitPointsLimitHigh(int value) = 0;

            /// @~chinese 角度判断   @~english Angle Check
            __declspec(property(put = SetAngleLimitEnable, get = GetAngleLimitEnable)) bool AngleLimitEnable;

            virtual bool GetAngleLimitEnable() = 0;

            virtual void SetAngleLimitEnable(bool value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngleLimitLow, get = GetAngleLimitLow)) double AngleLimitLow;

            virtual double GetAngleLimitLow() = 0;

            virtual void SetAngleLimitLow(double value) = 0;

            /// @~chinese 角度范围，范围：[-180,180]   @~english Angle Range，range:[-180,180]
            __declspec(property(put = SetAngleLimitHigh, get = GetAngleLimitHigh)) double AngleLimitHigh;

            virtual double GetAngleLimitHigh() = 0;

            virtual void SetAngleLimitHigh(double value) = 0;

            /// @~chinese 拟合误差判断   @~english Fit Error Check
            __declspec(property(put = SetScoreLimitEnable, get = GetScoreLimitEnable)) bool ScoreLimitEnable;

            virtual bool GetScoreLimitEnable() = 0;

            virtual void SetScoreLimitEnable(bool value) = 0;

            /// @~chinese 拟合误差范围，范围：[0,9999]   @~english Fit Error Range，range:[0,9999]
            __declspec(property(put = SetScoreLimitLow, get = GetScoreLimitLow)) double ScoreLimitLow;

            virtual double GetScoreLimitLow() = 0;

            virtual void SetScoreLimitLow(double value) = 0;

            /// @~chinese 拟合误差范围，范围：[0,9999]   @~english Fit Error Range，range:[0,9999]
            __declspec(property(put = SetScoreLimitHigh, get = GetScoreLimitHigh)) double ScoreLimitHigh;

            virtual double GetScoreLimitHigh() = 0;

            virtual void SetScoreLimitHigh(double value) = 0;

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
        
        /*  Note: Interface Classes for describing a edge point Information.  */
		/// 直线查找轮廓点信息类
        class ILineFindEdgePointInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit ILineFindEdgePointInfo() {}
            virtual ~ILineFindEdgePointInfo() {}

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

        /*  Note: Interface Classes of the result of the LineFindResults.  */
        ///直线查找结果类
        class LineFindResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit LineFindResults() {}
            virtual ~LineFindResults() {}

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
             * @brief 直线起点
             */
            virtual MVD_POINT_F GetLineStartPoint() = 0;
            /**
             * @brief 直线终点
             */
            virtual MVD_POINT_F GetLineEndPoint() = 0;
            /**
            * @brief 直线中点
            */
            virtual MVD_POINT_F GetMidpoint() = 0;
            /**
             * @brief 直线角度
             */
            virtual float GetLineAngle() = 0;
            /**
             * @brief  拟合误差
             */
            virtual float GetLineStraightness() = 0;
            /**
             * @brief 0:查找失败；1:查找成功
             * @return 查找状态
             */
            virtual int GetStatus() = 0;
            /**
             * @brief 边缘点点数
             */
            virtual int GetEdgePointNum() = 0;
            /**
             * @brief 获取结果边缘点序
             * @param nPtIndex [OUT] 边缘点序
             * @return 返回结果边缘点序
             * @remarks nPtIndex从0开始计数。
             */
            virtual ILineFindEdgePointInfo* GetEdgePointInfo(int nPtIndex) = 0;
            /**
             * @brief 卡尺数
             */
            virtual int GetCaliperNum() = 0;
            /**
             * @brief 获取结果中的单个卡尺位置
             * @param nCaliperIndex [IN] 卡尺索引
             * @return 返回卡尺信息
             * @remarks nPtIndex从0开始计数。
             */
            virtual RectBox GetCaliperBox(int nCaliperIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the LineFind tool.  */
        ///直线查找工具类
        class IMVSLineFindModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSLineFindModuTool() {}
            virtual ~IMVSLineFindModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual LineFindParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual LineFindResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSLineFindModuTool(IMVSLineFindModuTool&);
            IMVSLineFindModuTool& operator=(IMVSLineFindModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for LineFind.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSLineFindModu::IMVSLineFindModuTool * __stdcall GetLineFindToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_LINEFIND_H_
