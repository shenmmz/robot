/***************************************************************************************************
 * File：IVmTargetTrack.h
 * Note：@~chinese 目标跟踪模块接口声明 @~english Interface for the TargetTrack tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_TARGETTRACK_H_
#define _IVM_TARGETTRACK_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSTargetTrackModu
    {
    /// \addtogroup 目标跟踪
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the TargetTrackParams.  */
        ///目标跟踪参数类
        class TargetTrackParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit TargetTrackParams() {}
            virtual ~TargetTrackParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入Box(仅当次执行起效)   @~english Input Box(Only valid for this execution)
            virtual void SetInputBOX(RectBox* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese X方向速度，范围：[0.01,2.00]   @~english Track Speed X，range:[0.01,2.00]
            __declspec(property(put = SetTrackSpeedX, get = GetTrackSpeedX)) double TrackSpeedX;

            virtual double GetTrackSpeedX() = 0;

            virtual void SetTrackSpeedX(double value) = 0;

            /// @~chinese Y方向速度，范围：[0.01,2.00]   @~english Track Speed Y，range:[0.01,2.00]
            __declspec(property(put = SetTrackSpeedY, get = GetTrackSpeedY)) double TrackSpeedY;

            virtual double GetTrackSpeedY() = 0;

            virtual void SetTrackSpeedY(double value) = 0;

            /// @~chinese 允许缺帧数，范围：[0,5]   @~english Max Lose Frame Num，range:[0,5]
            __declspec(property(put = SetFrameNum, get = GetFrameNum)) int FrameNum;

            virtual int GetFrameNum() = 0;

            virtual void SetFrameNum(int value) = 0;

            /// @~chinese 轨迹重叠率，范围：[1,100]   @~english track overlap，range:[1,100]
            __declspec(property(put = SetTrackOverlap, get = GetTrackOverlap)) int TrackOverlap;

            virtual int GetTrackOverlap() = 0;

            virtual void SetTrackOverlap(int value) = 0;

            /// @~chinese 计数总数判断   @~english Count Num Check
            __declspec(property(put = SetCountNumLimitEnable, get = GetCountNumLimitEnable)) bool CountNumLimitEnable;

            virtual bool GetCountNumLimitEnable() = 0;

            virtual void SetCountNumLimitEnable(bool value) = 0;

            /// @~chinese 计数总数范围，范围：[0,99999]   @~english Count Num Range，range:[0,99999]
            __declspec(property(put = SetCountNumLimitLow, get = GetCountNumLimitLow)) int CountNumLimitLow;

            virtual int GetCountNumLimitLow() = 0;

            virtual void SetCountNumLimitLow(int value) = 0;

            /// @~chinese 计数总数范围，范围：[0,99999]   @~english Count Num Range，range:[0,99999]
            __declspec(property(put = SetCountNumLimitHigh, get = GetCountNumLimitHigh)) int CountNumLimitHigh;

            virtual int GetCountNumLimitHigh() = 0;

            virtual void SetCountNumLimitHigh(int value) = 0;

            /// @~chinese 目标数判断   @~english Obj Num Check
            __declspec(property(put = SetObjNumLimitEnable, get = GetObjNumLimitEnable)) bool ObjNumLimitEnable;

            virtual bool GetObjNumLimitEnable() = 0;

            virtual void SetObjNumLimitEnable(bool value) = 0;

            /// @~chinese 目标数范围，范围：[0,99999]   @~english Obj Num Range，range:[0,99999]
            __declspec(property(put = SetObjNumLimitLow, get = GetObjNumLimitLow)) int ObjNumLimitLow;

            virtual int GetObjNumLimitLow() = 0;

            virtual void SetObjNumLimitLow(int value) = 0;

            /// @~chinese 目标数范围，范围：[0,99999]   @~english Obj Num Range，range:[0,99999]
            __declspec(property(put = SetObjNumLimitHigh, get = GetObjNumLimitHigh)) int ObjNumLimitHigh;

            virtual int GetObjNumLimitHigh() = 0;

            virtual void SetObjNumLimitHigh(int value) = 0;

            /// @~chinese 单帧计数判断   @~english Single Count Check
            __declspec(property(put = SetSingleCountLimitEnable, get = GetSingleCountLimitEnable)) bool SingleCountLimitEnable;

            virtual bool GetSingleCountLimitEnable() = 0;

            virtual void SetSingleCountLimitEnable(bool value) = 0;

            /// @~chinese 单帧计数范围，范围：[0,99999]   @~english Single Count Range，range:[0,99999]
            __declspec(property(put = SetSingleCountLimitLow, get = GetSingleCountLimitLow)) int SingleCountLimitLow;

            virtual int GetSingleCountLimitLow() = 0;

            virtual void SetSingleCountLimitLow(int value) = 0;

            /// @~chinese 单帧计数范围，范围：[0,99999]   @~english Single Count Range，range:[0,99999]
            __declspec(property(put = SetSingleCountLimitHigh, get = GetSingleCountLimitHigh)) int SingleCountLimitHigh;

            virtual int GetSingleCountLimitHigh() = 0;

            virtual void SetSingleCountLimitHigh(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        ///输出目标轮廓信息
        class ITrackObjectOutputInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit ITrackObjectOutputInfo() {}
            virtual ~ITrackObjectOutputInfo() {}

        public:
            /**
            * @brief 获取单个输出轮廓的轮廓点个数
            * @return 返回输出轮廓
            * @remarks 输出轮廓只支持矩形和多边形
            */
            virtual int GetContourPointNum() = 0;
            /**
            * @brief 获取输出轮廓点
            * @param nIndex [IN] 轮廓点索引
            * @return 返回输出轮廓
            * @remarks 输出轮廓只支持矩形和多边形
            */
            virtual PointF GetContour(int nIndex) = 0;
            /**
            * @brief 获取目标ID
            * @return 返回目标ID
            */
            virtual int GetID() = 0;
            /**
            * @brief 获取目标Id位置
            * @return 返回目标Id位置
            */
            virtual PointF GetObjIdPosition() = 0;
        };
        /*  Note: Interface Classes of the result of the TargetTrackResults.  */
        ///目标跟踪结果类
        class TargetTrackResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit TargetTrackResults() {}
            virtual ~TargetTrackResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

            /**
            * @brief 获取输出目标轮廓信息
            * @param nIndex [IN] 对应下标
            * @return 返回查找状态
            * @remarks 总数可通过GetOutputNum()获取
            */
            virtual ITrackObjectOutputInfo* GetOutputInfoAt(int nIndex) = 0;
            /**
            * @brief 获取输出轮廓的数量
            * @return 返回输出轮廓的数量
            */
            virtual int GetOutputNum() = 0;
            /**
            * @brief 获取累计检测出的目标数量
            * @return 返回累计检测出的目标数量
            * @remarks 该值为前几次运行结果中IncreasedNum的总和
            */
            virtual int GetTotalNum() = 0;
            /**
            * @brief 获取本次处理新检测出的目标数量
            * @return 返回本次处理新检测出的目标数量
            */
            virtual int GetIncreasedNum() = 0;
            /**
            * @brief 获取算法跟踪监测的矩形范围
            * @return 返回算法跟踪监测的矩形范围
            */
            virtual RectBox GetTrackRect() = 0;
            /**
            * @brief 获取算法跟踪的矩形速度X
            * @return 返回算法跟踪的矩形速度X
            */
            virtual float GetTrackSpeedX() = 0;
            /**
            * @brief 获取算法跟踪的矩形速度Y
            * @return 返回算法跟踪的矩形速度Y
            */
            virtual float GetTrackSpeedY() = 0;
            /**
            * @brief 获取算法跟踪的检测线
            * @return 返回算法跟踪的检测线
            */
            virtual Line GetTrackLine() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the TargetTrack tool.  */
        ///目标跟踪工具类
        class IMVSTargetTrackModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSTargetTrackModuTool() {}
            virtual ~IMVSTargetTrackModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual TargetTrackParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual TargetTrackResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSTargetTrackModuTool(IMVSTargetTrackModuTool&);
            IMVSTargetTrackModuTool& operator=(IMVSTargetTrackModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for TargetTrack.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSTargetTrackModu::IMVSTargetTrackModuTool * __stdcall GetTargetTrackToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_TARGETTRACK_H_
