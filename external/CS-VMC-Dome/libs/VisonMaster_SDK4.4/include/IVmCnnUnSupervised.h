/***************************************************************************************************
 * File：IVmCnnUnSupervised.h
 * Note：@~chinese DL无监督分割GPU模块接口声明 @~english Interface for the CnnUnSupervised tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CNNUNSUPERVISED_H_
#define _IVM_CNNUNSUPERVISED_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCnnUnSupervisedModu
    {
    /// \addtogroup DL无监督分割GPU
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///DL无监督分割GROI管理类
        class CnnUnSupervisedRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoisRectangle(RectBox* pRoiValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the CnnUnSupervisedParams.  */
        ///DL无监督分割G参数类
        class CnnUnSupervisedParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnUnSupervisedParams() {}
            virtual ~CnnUnSupervisedParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual CnnUnSupervisedRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 模型文件路径   @~english Model File Path
            __declspec(property(put = SetLoadModelPath, get = GetLoadModelPath)) const char* LoadModelPath;

            virtual const char* GetLoadModelPath() = 0;

            virtual void SetLoadModelPath(const char* value) = 0;

            /// @~chinese 方案存模型   @~english Save Model Data
            __declspec(property(put = SetSaveModelDataEnable, get = GetSaveModelDataEnable)) bool SaveModelDataEnable;

            virtual bool GetSaveModelDataEnable() = 0;

            virtual void SetSaveModelDataEnable(bool value) = 0;

            /// @~chinese 获取模型ROI   @~english Get Roi From Model
            __declspec(property(put = SetRoiFromModelEnable, get = GetRoiFromModelEnable)) bool RoiFromModelEnable;

            virtual bool GetRoiFromModelEnable() = 0;

            virtual void SetRoiFromModelEnable(bool value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the CnnUnSupervisedResults.  */
        ///DL无监督分割GPU结果类
        class CnnUnSupervisedResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnUnSupervisedResults() {}
            virtual ~CnnUnSupervisedResults() {}

        public:
             /**
             * @brief 检测区域
             * @param nIndex[IN] 数据索引
             */
             virtual RectBox GetDetectROIs(int nIndex) = 0;

             /**
             * @brief 检测区域
             * @deprecated V4.4废弃
             */
             virtual RectBox GetROI() = 0;

             /**
             * @brief 说明模块状态
             */
             virtual int GetModuStatus() = 0;

             /**
             * @brief 概率图
             */
             virtual ImageBaseData GetProbabilityImageData() = 0;

        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CnnUnSupervised tool.  */
        ///DL无监督分割GPU工具类
        class IMVSCnnUnSupervisedModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCnnUnSupervisedModuTool() {}
            virtual ~IMVSCnnUnSupervisedModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CnnUnSupervisedParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CnnUnSupervisedResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSCnnUnSupervisedModuTool(IMVSCnnUnSupervisedModuTool&);
            IMVSCnnUnSupervisedModuTool& operator=(IMVSCnnUnSupervisedModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CnnUnSupervised.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSCnnUnSupervisedModu::IMVSCnnUnSupervisedModuTool * __stdcall GetCnnUnSupervisedToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CNNUNSUPERVISED_H_
