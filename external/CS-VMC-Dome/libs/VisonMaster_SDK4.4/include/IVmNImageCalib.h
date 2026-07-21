/***************************************************************************************************
 * File：IVmNImageCalib.h
 * Note：@~chinese N图像标定模块接口声明 @~english Interface for the NImageCalib tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_NIMAGECALIB_H_
#define _IVM_NIMAGECALIB_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSNImageCalibModu
    {
        /// @~chinese 移动优先   @~english Movement First
        typedef enum _MoveFirstTypeEnum
        {
            /// @~chinese X优先   @~english X First
            MoveFirstType_XFirst = 0x1,

            /// @~chinese Y优先   @~english Y First
            MoveFirstType_YFirst = 0x2,

        }MoveFirstTypeEnum;

        /// @~chinese 标定板类型   @~english Calibration Board Type
        typedef enum _CalibBoardTypeEnum
        {
            /// @~chinese 海康I型标定板   @~english Hikvision I Calibration Board
            CalibBoardType_TypeHKI = 0x7,

            /// @~chinese 海康II型标定板   @~english Hikvision II Calibration Board
            CalibBoardType_TypeHKII = 0x6,

            /// @~chinese 海康扩展I型标定板   @~english Hikvision_IV2 Calibration Board
            CalibBoardType_TypeHKIV2 = 0x9,

            /// @~chinese 海康扩展II型标定板   @~english Hikvision_IIV2 Calibration Board
            CalibBoardType_TypeHKIIV2 = 0x8,

        }CalibBoardTypeEnum;

        /// @~chinese 中值滤波状态   @~english Median Filtering State
        typedef enum _FilterStatusEnum
        {
            /// @~chinese 执行滤波   @~english Execution Filtering
            FilterStatus_FilterStateTure = 0x1,

            /// @~chinese 无滤波   @~english No Filter
            FilterStatus_FilterStateFalse = 0x2,

        }FilterStatusEnum;

        /// @~chinese 亚像素窗口   @~english Sub-pixel window
        typedef enum _SubpixelWinAutoEnum
        {
            /// @~chinese 自适应   @~english Self-Adaption
            SubpixelWinAuto_SubpixelWinSizeAuto = 0x1,

            /// @~chinese 设置值   @~english Set
            SubpixelWinAuto_SubpixelWinSizeUser = 0x2,

        }SubpixelWinAutoEnum;

    /// \addtogroup N图像标定
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the NImageCalibParams.  */
        ///N图像标定参数类
        class NImageCalibParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit NImageCalibParams() {}
            virtual ~NImageCalibParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 图像点(仅当次执行起效)   @~english Image Point(Only valid for this execution)
            virtual void SetImagePoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 物理点(仅当次执行起效)   @~english Physical Point(Only valid for this execution)
            virtual void SetPhysicalPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 外部输入字符(仅当次执行起效)   @~english External Input Character(Only valid for this execution)
            virtual void SetPhyPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 旋转角度(仅当次执行起效)   @~english Rotation Angle(Only valid for this execution)
            virtual void SetRotateAngle(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 平移次数，范围：[0,15]   @~english Translation Times，range:[0,15]
            __declspec(property(put = SetCalibPointTotalNum, get = GetCalibPointTotalNum)) int CalibPointTotalNum;

            virtual int GetCalibPointTotalNum() = 0;

            virtual void SetCalibPointTotalNum(int value) = 0;

            /// @~chinese 旋转次数，范围：[0,9]   @~english Rotation Times，range:[0,9]
            __declspec(property(put = SetRotPointTotalNum, get = GetRotPointTotalNum)) int RotPointTotalNum;

            virtual int GetRotPointTotalNum() = 0;

            virtual void SetRotPointTotalNum(int value) = 0;

            /// @~chinese 基准点X，范围：[-9999999.0,9999999.0]   @~english InitPointX，range:[-9999999.0,9999999.0]
            __declspec(property(put = SetBasePointX, get = GetBasePointX)) double BasePointX;

            virtual double GetBasePointX() = 0;

            virtual void SetBasePointX(double value) = 0;

            /// @~chinese 基准点Y，范围：[-9999999.0,9999999.0]   @~english InitPointY，range:[-9999999.0,9999999.0]
            __declspec(property(put = SetBasePointY, get = GetBasePointY)) double BasePointY;

            virtual double GetBasePointY() = 0;

            virtual void SetBasePointY(double value) = 0;

            /// @~chinese 偏移X，范围：[-99999.0,99999.0]   @~english Offset X，range:[-99999.0,99999.0]
            __declspec(property(put = SetMoveAlignX, get = GetMoveAlignX)) double MoveAlignX;

            virtual double GetMoveAlignX() = 0;

            virtual void SetMoveAlignX(double value) = 0;

            /// @~chinese 偏移Y，范围：[-99999.0,99999.0]   @~english Offset Y，range:[-99999.0,99999.0]
            __declspec(property(put = SetMoveAlignY, get = GetMoveAlignY)) double MoveAlignY;

            virtual double GetMoveAlignY() = 0;

            virtual void SetMoveAlignY(double value) = 0;

            /// @~chinese 移动优先   @~english Movement First
            __declspec(property(put = SetMoveFirstType, get = GetMoveFirstType)) MoveFirstTypeEnum MoveFirstType;

            virtual MoveFirstTypeEnum GetMoveFirstType() = 0;

            virtual void SetMoveFirstType(MoveFirstTypeEnum value) = 0;

            /// @~chinese 换向移动次数，范围：[1,16]   @~english Commutation Number，range:[1,16]
            __declspec(property(put = SetChangeDirectionMoveTime, get = GetChangeDirectionMoveTime)) int ChangeDirectionMoveTime;

            virtual int GetChangeDirectionMoveTime() = 0;

            virtual void SetChangeDirectionMoveTime(int value) = 0;

            /// @~chinese 基准角度，范围：[-180.0,180.0]   @~english InitAngle，range:[-180.0,180.0]
            __declspec(property(put = SetBaseAngle, get = GetBaseAngle)) double BaseAngle;

            virtual double GetBaseAngle() = 0;

            virtual void SetBaseAngle(double value) = 0;

            /// @~chinese 角度偏移，范围：[-180.0,180.0]   @~english Angle Offset，range:[-180.0,180.0]
            __declspec(property(put = SetMoveAngle, get = GetMoveAngle)) double MoveAngle;

            virtual double GetMoveAngle() = 0;

            virtual void SetMoveAngle(double value) = 0;

            /// @~chinese 矩阵修正   @~english Homo Fix
            __declspec(property(put = SetHomoFixEnable, get = GetHomoFixEnable)) bool HomoFixEnable;

            virtual bool GetHomoFixEnable() = 0;

            virtual void SetHomoFixEnable(bool value) = 0;

            /// @~chinese 更新文件   @~english Update File
            __declspec(property(put = SetRefreshFileEnable, get = GetRefreshFileEnable)) bool RefreshFileEnable;

            virtual bool GetRefreshFileEnable() = 0;

            virtual void SetRefreshFileEnable(bool value) = 0;

            /// @~chinese 标定文件路径   @~english Set Calibration File Path
            __declspec(property(put = SetCalibPathName, get = GetCalibPathName)) const char* CalibPathName;

            virtual const char* GetCalibPathName() = 0;

            virtual void SetCalibPathName(const char* value) = 0;

            /// @~chinese 相机移动   @~english Camera Movement
            __declspec(property(put = SetCareraMove, get = GetCareraMove)) bool CareraMove;

            virtual bool GetCareraMove() = 0;

            virtual void SetCareraMove(bool value) = 0;

            /// @~chinese 标定板类型   @~english Calibration Board Type
            __declspec(property(put = SetCalibBoardType, get = GetCalibBoardType)) CalibBoardTypeEnum CalibBoardType;

            virtual CalibBoardTypeEnum GetCalibBoardType() = 0;

            virtual void SetCalibBoardType(CalibBoardTypeEnum value) = 0;

            /// @~chinese 灰度对比度，范围：[1,255](弃用)   @~english Grayscale Contrast，range:[1,255](Obsolete)
            __declspec(property(put = SetGrayContrast, get = GetGrayContrast)) int GrayContrast;

            virtual int GetGrayContrast() = 0;

            virtual void SetGrayContrast(int value) = 0;

            /// @~chinese 中值滤波状态   @~english Median Filtering State
            __declspec(property(put = SetFilterStatus, get = GetFilterStatus)) FilterStatusEnum FilterStatus;

            virtual FilterStatusEnum GetFilterStatus() = 0;

            virtual void SetFilterStatus(FilterStatusEnum value) = 0;

            /// @~chinese 亚像素窗口(弃用)   @~english Sub-pixel window(Obsolete)
            __declspec(property(put = SetSubpixelWinAuto, get = GetSubpixelWinAuto)) SubpixelWinAutoEnum SubpixelWinAuto;

            virtual SubpixelWinAutoEnum GetSubpixelWinAuto() = 0;

            virtual void SetSubpixelWinAuto(SubpixelWinAutoEnum value) = 0;

            /// @~chinese 设置窗口大小，范围：[3,150](弃用)   @~english Set window size，range:[3,150](Obsolete)
            __declspec(property(put = SetSubPixelWindowSize, get = GetSubPixelWindowSize)) int SubPixelWindowSize;

            virtual int GetSubPixelWindowSize() = 0;

            virtual void SetSubPixelWindowSize(int value) = 0;

            /// @~chinese 生成标定文件   @~english Create Calibration File
            virtual int DoSaveFile(IN const char * const strValue) = 0;

            /// @~chinese 清除图像   @~english Clear Image
            virtual int DoClearImage() = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
		/// 机构平移运动误差类
        class INImageCalibMechanismTransErrorInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit INImageCalibMechanismTransErrorInfo() {}
            virtual ~INImageCalibMechanismTransErrorInfo() {}
        public:
            /**
             * @brief 平移过程中图像移动量与机构移动量之间的偏差
             */
            virtual MVD_POINT_F GetTransPoint() = 0;
            /**
             * @brief 平移过程中的图像尺度变化
             */
            virtual float GetScale() = 0;
            /**
             * @brief 平移过程中的图像之间旋转变化量
             */
            virtual float GetRotate() = 0;
            
            /**
             * @brief 平移估计时的像素误差
             */
            virtual float GetTransError() = 0;
            /**
             * @brief 平移估计时的真实误差，单位mm
             */
            virtual float GetTransWorldError() = 0;
            /**
             * @brief 旋转估计像素误差
             */
            virtual float GetRotError() = 0;
            /**
             * @brief 旋转估计真实误差，单位mm
             */
            virtual float GetRotWorldError() = 0;
            /**
             * @brief 机构旋转运动误差,是输入的旋转角度与计算的机构旋转角度之差
             */
            virtual float GetRotAngleError() = 0;
        };

        /*  Note: Interface Classes of the result of the NImageCalibResults.  */
        ///N图像标定结果类
        class NImageCalibResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit NImageCalibResults() {}
            virtual ~NImageCalibResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 每张图像的标定状态
             */
            virtual int GetCalibStatus(int nImageIndex) = 0;
            /**
             * @brief 轴心在旋转图像中的图像坐标
             */
            virtual MVD_POINT_F GetRotImageCenter() = 0;
            /**
             * @brief 机构平移运动误差
             */
            virtual INImageCalibMechanismTransErrorInfo* GetTransMotionErrorInfo() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the NImageCalib tool.  */
        ///N图像标定工具类
        class IMVSNImageCalibModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSNImageCalibModuTool() {}
            virtual ~IMVSNImageCalibModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual NImageCalibParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual NImageCalibResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSNImageCalibModuTool(IMVSNImageCalibModuTool&);
            IMVSNImageCalibModuTool& operator=(IMVSNImageCalibModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for NImageCalib.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSNImageCalibModu::IMVSNImageCalibModuTool * __stdcall GetNImageCalibToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_NIMAGECALIB_H_
