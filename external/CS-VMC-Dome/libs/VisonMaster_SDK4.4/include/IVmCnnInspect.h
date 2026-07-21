/***************************************************************************************************
 * File：IVmCnnInspect.h
 * Note：@~chinese DL异常检测模块接口声明 @~english Interface for the CnnInspect tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CNNINSPECT_H_
#define _IVM_CNNINSPECT_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCnnInspectModu
    {
    /// \addtogroup DL异常检测
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///DL异常检测ROI管理类
        class CnnInspectRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the CnnInspectParams.  */
        ///DL异常检测参数类
        class CnnInspectParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnInspectParams() {}
            virtual ~CnnInspectParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual CnnInspectRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 模型文件路径   @~english Model File Path
            __declspec(property(put = SetLoadModelPath, get = GetLoadModelPath)) const char* LoadModelPath;

            virtual const char* GetLoadModelPath() = 0;

            virtual void SetLoadModelPath(const char* value) = 0;

            /// @~chinese 方案存模型   @~english Save Model Data
            __declspec(property(put = SetSaveModelDataEnable, get = GetSaveModelDataEnable)) bool SaveModelDataEnable;

            virtual bool GetSaveModelDataEnable() = 0;

            virtual void SetSaveModelDataEnable(bool value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the CnnInspectResults.  */
        ///DL异常检测结果类
        class CnnInspectResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnInspectResults() {}
            virtual ~CnnInspectResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;
            /**
             * @brief 获取残差图
             */
            virtual ImageBaseData GetOutResidualImage() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CnnInspect tool.  */
        ///DL异常检测工具类
        class IMVSCnnInspectModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCnnInspectModuTool() {}
            virtual ~IMVSCnnInspectModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CnnInspectParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CnnInspectResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSCnnInspectModuTool(IMVSCnnInspectModuTool&);
            IMVSCnnInspectModuTool& operator=(IMVSCnnInspectModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CnnInspect.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSCnnInspectModu::IMVSCnnInspectModuTool * __stdcall GetCnnInspectToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CNNINSPECT_H_
