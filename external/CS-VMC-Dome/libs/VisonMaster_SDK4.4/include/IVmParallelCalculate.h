/***************************************************************************************************
 * File：IVmParallelCalculate.h
 * Note：@~chinese 平行线计算模块接口声明 @~english Interface for the ParallelCalculate tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_PARALLELCALCULATE_H_
#define _IVM_PARALLELCALCULATE_H_

#include "MVD_ShapeParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSParallelCalculateModu
    {
        /// @~chinese 方式选择   @~english Handler Mode
        typedef enum _WayToChooseEnum
        {
            /// @~chinese 过直线外一点   @~english A point outside the line
            WayToChoose_PointLine = 0x0,

            /// @~chinese 与直线相距一定距离   @~english A distance from the line
            WayToChoose_DistanceLine = 0x1,

        }WayToChooseEnum;

    /// \addtogroup 平行线计算
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the ParallelCalculateParams.  */
        ///平行线计算参数类
        class ParallelCalculateParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ParallelCalculateParams() {}
            virtual ~ParallelCalculateParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入点(仅当次执行起效)   @~english Input Point(Only valid for this execution)
            virtual void SetInputPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入直线(仅当次执行起效)   @~english Input Line(Only valid for this execution)
            virtual void SetInputLine(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 方式选择   @~english Handler Mode
            __declspec(property(put = SetWayToChoose, get = GetWayToChoose)) WayToChooseEnum WayToChoose;

            virtual WayToChooseEnum GetWayToChoose() = 0;

            virtual void SetWayToChoose(WayToChooseEnum value) = 0;

            /// @~chinese 间距，范围：[0,10000]   @~english Interval，range:[0,10000]
            __declspec(property(put = SetLineLength, get = GetLineLength)) double LineLength;

            virtual double GetLineLength() = 0;

            virtual void SetLineLength(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ParallelCalculateResults.  */
        ///平行线计算结果类
        class ParallelCalculateResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ParallelCalculateResults() {}
            virtual ~ParallelCalculateResults() {}

        public:
            /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 测量点
             */
             virtual MVD_POINT_F GetMeasurePoint() = 0;
            /**
             * @brief 测量直线
             */
             virtual Line GetMeasureLine() = 0;
            /**
             * @brief 平行线
             */
             virtual Line GetParallelLine() = 0;
            /**
             * @brief 平行线
             */
             virtual Line GetParallelLine1() = 0;
            /**
             * @brief 平行线2
             */
             virtual Line GetParallelLine2() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ParallelCalculate tool.  */
        ///平行线计算工具类
        class IMVSParallelCalculateModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSParallelCalculateModuTool() {}
            virtual ~IMVSParallelCalculateModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ParallelCalculateParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ParallelCalculateResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSParallelCalculateModuTool(IMVSParallelCalculateModuTool&);
            IMVSParallelCalculateModuTool& operator=(IMVSParallelCalculateModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ParallelCalculate.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSParallelCalculateModu::IMVSParallelCalculateModuTool * __stdcall GetParallelCalculateToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_PARALLELCALCULATE_H_
