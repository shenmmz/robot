/***************************************************************************************************
 * File：IVmRotateCalculate.h
 * Note：@~chinese 旋转计算模块接口声明 @~english Interface for the RotateCalculate tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_ROTATECALCULATE_H_
#define _IVM_ROTATECALCULATE_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSRotateCalculateModu
    {
    /// \addtogroup 旋转计算
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the RotateCalculateParams.  */
        ///旋转计算参数类
        class RotateCalculateParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit RotateCalculateParams() {}
            virtual ~RotateCalculateParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入点(仅当次执行起效)   @~english Input Point(Only valid for this execution)
            virtual void SetInputPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入直线(仅当次执行起效)   @~english Input Line(Only valid for this execution)
            virtual void SetInputLine(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 旋转中心(仅当次执行起效)   @~english Rotate Center(Only valid for this execution)
            virtual void SetRotateCenter(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 角度(仅当次执行起效)   @~english Angle(Only valid for this execution)
            virtual void SetAngle(float* pInputValue, unsigned int nValueNum) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the RotateCalculateResults.  */
        ///旋转计算结果类
        class RotateCalculateResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit RotateCalculateResults() {}
            virtual ~RotateCalculateResults() {}

        public:
            /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 旋转中心点
             */
             virtual PointF GetInOutRotateCenterPt() = 0;
            /**
             * @brief 点数量
             */
             virtual int GetPointNum() = 0;
            /**
             * @brief 输入输出点
             * @param nIndex [IN] 索引
             */
             virtual PointF GetInputOutPoint(IN int nIndex) = 0;
            /**
             * @brief 输出点
             * @param nIndex [IN] 索引
             */
             virtual PointF GetOutPoint(IN int nIndex) = 0;
            /**
             * @brief 输入输出线
             */
             virtual Line GetInputOutLine() = 0;
            /**
             * @brief 输出线
             */
             virtual Line GetOutLine() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the RotateCalculate tool.  */
        ///旋转计算工具类
        class IMVSRotateCalculateModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSRotateCalculateModuTool() {}
            virtual ~IMVSRotateCalculateModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual RotateCalculateParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual RotateCalculateResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSRotateCalculateModuTool(IMVSRotateCalculateModuTool&);
            IMVSRotateCalculateModuTool& operator=(IMVSRotateCalculateModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for RotateCalculate.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSRotateCalculateModu::IMVSRotateCalculateModuTool * __stdcall GetRotateCalculateToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_ROTATECALCULATE_H_
