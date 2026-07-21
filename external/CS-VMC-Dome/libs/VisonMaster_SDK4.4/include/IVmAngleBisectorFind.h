/***************************************************************************************************
 * File：IVmAngleBisectorFind.h
 * Note：@~chinese 角平分线查找模块接口声明 @~english Interface for the AngleBisectorFind tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_ANGLEBISECTORFIND_H_
#define _IVM_ANGLEBISECTORFIND_H_

#include "MVD_ShapeParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSAngleBisectorFindModu
    {
    /// \addtogroup 角平分线查找
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the AngleBisectorFindParams.  */
        ///角平分线查找参数类
        class AngleBisectorFindParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit AngleBisectorFindParams() {}
            virtual ~AngleBisectorFindParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese 输入直线1(仅当次执行起效)   @~english Input Line 1(Only valid for this execution)
            virtual void SetInputLine1(Line* pInputValue, unsigned int nValueNum) = 0;

            /// @~chinese 输入直线2(仅当次执行起效)   @~english Input Line 2(Only valid for this execution)
            virtual void SetInputLine2(Line* pInputValue, unsigned int nValueNum) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the AngleBisectorFindResults.  */
        ///角平分线查找结果类
        class AngleBisectorFindResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit AngleBisectorFindResults() {}
            virtual ~AngleBisectorFindResults() {}

        public:
            /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
            /**
             * @brief 测量直线1
             */
             virtual Line GetMeasureLine1() = 0;
            /**
             * @brief 测量直线2
             */
             virtual Line GetMeasureLine2() = 0;
            /**
             * @brief 交点
             */
             virtual MVD_POINT_F GetIntersectionPt() = 0;
            /**
             * @brief 角平分线
             */
             virtual Line GetAngleBisectorLine() = 0;
            /**
             * @brief 角度标识点
             * @param nIndex [IN] 索引，范围:[0, 4]

             */
             virtual MVD_POINT_F GetAngleBisectorLine(IN int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the AngleBisectorFind tool.  */
        ///角平分线查找工具类
        class IMVSAngleBisectorFindModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSAngleBisectorFindModuTool() {}
            virtual ~IMVSAngleBisectorFindModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual AngleBisectorFindParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual AngleBisectorFindResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSAngleBisectorFindModuTool(IMVSAngleBisectorFindModuTool&);
            IMVSAngleBisectorFindModuTool& operator=(IMVSAngleBisectorFindModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for AngleBisectorFind.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSAngleBisectorFindModu::IMVSAngleBisectorFindModuTool * __stdcall GetAngleBisectorFindToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_ANGLEBISECTORFIND_H_
