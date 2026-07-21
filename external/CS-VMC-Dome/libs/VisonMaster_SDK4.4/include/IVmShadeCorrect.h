/***************************************************************************************************
 * File：IVmShadeCorrect.h
 * Note：@~chinese 阴影校正模块接口声明 @~english Interface for the ShadeCorrect tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_SHADECORRECT_H_
#define _IVM_SHADECORRECT_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSShadeCorrectModu
    {
        /// @~chinese 方向   @~english Direction
        typedef enum _DirectionEnum
        {
            /// @~chinese X   @~english X
            Direction_X = 0x1,

            /// @~chinese Y   @~english Y
            Direction_Y = 0x2,

            /// @~chinese XY   @~english XY
            Direction_XY = 0x3,

        }DirectionEnum;

    /// \addtogroup 阴影校正
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///阴影校正ROI管理类
        class ShadeCorrectRoiManager
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

        /*  Note: Interface Classes of the parameter of the ShadeCorrectParams.  */
        ///阴影校正参数类
        class ShadeCorrectParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ShadeCorrectParams() {}
            virtual ~ShadeCorrectParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual ShadeCorrectRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 滤波尺寸，范围：[1,50]   @~english Filter Size，range:[1,50]
            __declspec(property(put = SetRatio, get = GetRatio)) int Ratio;

            virtual int GetRatio() = 0;

            virtual void SetRatio(int value) = 0;

            /// @~chinese 增益，范围：[0,100]   @~english Gain，range:[0,100]
            __declspec(property(put = SetGain, get = GetGain)) int Gain;

            virtual int GetGain() = 0;

            virtual void SetGain(int value) = 0;

            /// @~chinese 亮度校正补偿，范围：[0,255]   @~english Compensation，range:[0,255]
            __declspec(property(put = SetOffset, get = GetOffset)) int Offset;

            virtual int GetOffset() = 0;

            virtual void SetOffset(int value) = 0;

            /// @~chinese 噪声，范围：[0,255]   @~english Noise，range:[0,255]
            __declspec(property(put = SetNoise, get = GetNoise)) int Noise;

            virtual int GetNoise() = 0;

            virtual void SetNoise(int value) = 0;

            /// @~chinese 方向   @~english Direction
            __declspec(property(put = SetDirection, get = GetDirection)) DirectionEnum Direction;

            virtual DirectionEnum GetDirection() = 0;

            virtual void SetDirection(DirectionEnum value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ShadeCorrectResults.  */
        ///阴影校正结果类
        class ShadeCorrectResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ShadeCorrectResults() {}
            virtual ~ShadeCorrectResults() {}

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
             * @brief ROI圆弧
             */
             virtual Annulus GetROIAnnulus() = 0;
            /**
             * @brief 输出图像
             */
            virtual ImageBaseData GetOutputImage() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ShadeCorrect tool.  */
        ///阴影校正工具类
        class IMVSShadeCorrectModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSShadeCorrectModuTool() {}
            virtual ~IMVSShadeCorrectModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ShadeCorrectParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ShadeCorrectResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSShadeCorrectModuTool(IMVSShadeCorrectModuTool&);
            IMVSShadeCorrectModuTool& operator=(IMVSShadeCorrectModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ShadeCorrect.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSShadeCorrectModu::IMVSShadeCorrectModuTool * __stdcall GetShadeCorrectToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_SHADECORRECT_H_
