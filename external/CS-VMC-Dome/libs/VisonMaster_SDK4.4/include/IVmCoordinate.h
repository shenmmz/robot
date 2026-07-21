/***************************************************************************************************
 * File：IVmCoordinate.h
 * Note：@~chinese 坐标系模块接口声明 @~english Interface for the Coordinate tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_COORDINATE_H_
#define _IVM_COORDINATE_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace CoordinateModule
    {
        /// @~chinese 坐标系创建   @~english Create Coordinate
        typedef enum _CordinateCreateEnum
        {
            /// @~chinese 绘制   @~english Draw
            CordinateCreate_Draw = 0x0,

            /// @~chinese 订阅   @~english Subscribe
            CordinateCreate_Subscribe = 0x1,

        }CordinateCreateEnum;

        /// @~chinese 输入方式   @~english Input Mode
        typedef enum _InputWayEnum
        {
            /// @~chinese 按点   @~english By Point
            InputWay_ByPoint = 0x0,

            /// @~chinese 按坐标   @~english By Coordinate
            InputWay_ByCoordinate = 0x1,

            /// @~chinese 按矩阵   @~english By Matrix
            InputWay_ByMatrix = 0x2,

        }InputWayEnum;

    /// \addtogroup 坐标系
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the CoordinateParams.  */
        ///坐标系参数类
        class CoordinateParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CoordinateParams() {}
            virtual ~CoordinateParams() {}

        public:

            /// @~chinese 标定矩阵(仅当次执行起效)   @~english Calibration Matrix(Only valid for this execution)
            virtual void SetCalibMatrix(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入点(仅当次执行起效)   @~english Input Point(Only valid for this execution)
            virtual void SetSrcInputPoint(PointF* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入角度(仅当次执行起效)   @~english Input Angle(Only valid for this execution)
            virtual void SetSrcAngle(float* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 坐标系创建   @~english Create Coordinate
            __declspec(property(put = SetCordinateCreate, get = GetCordinateCreate)) CordinateCreateEnum CordinateCreate;

            virtual CordinateCreateEnum GetCordinateCreate() = 0;

            virtual void SetCordinateCreate(CordinateCreateEnum value) = 0;

            /// @~chinese 输入方式   @~english Input Mode
            __declspec(property(put = SetInputWay, get = GetInputWay)) InputWayEnum InputWay;

            virtual InputWayEnum GetInputWay() = 0;

            virtual void SetInputWay(InputWayEnum value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the CoordinateResults.  */
        ///坐标系结果类
        class CoordinateResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CoordinateResults() {}
            virtual ~CoordinateResults() {}

        public:
             /**
             * @brief 坐标系矩阵
             * @param nIndex[IN] 数据索引
             */
            virtual float GetCorCalibMatrix(int nIndex) = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;


        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the Coordinate tool.  */
        ///坐标系工具类
        class CoordinateModuleTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CoordinateModuleTool() {}
            virtual ~CoordinateModuleTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CoordinateParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CoordinateResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            CoordinateModuleTool(CoordinateModuleTool&);
            CoordinateModuleTool& operator=(CoordinateModuleTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
#endif    ///< _IVM_COORDINATE_H_
