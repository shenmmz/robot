/***************************************************************************************************
 * File：IVmCnnFastFlaw.h
 * Note：@~chinese DL快速图像分割模块接口声明 @~english Interface for the CnnFastFlaw tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CNNFASTFLAW_H_
#define _IVM_CNNFASTFLAW_H_

#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCnnFastFlawModu
    {
    /// \addtogroup DL快速图像分割
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///DL快速图像分割ROI管理类
        class CnnFastFlawRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoisRectangle(RectBox* pRoiValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the CnnFastFlawParams.  */
        ///DL快速图像分割参数类
        class CnnFastFlawParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnFastFlawParams() {}
            virtual ~CnnFastFlawParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual CnnFastFlawRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 模型文件路径   @~english Model File Path
            __declspec(property(put = SetLoadModelPath, get = GetLoadModelPath)) const char* LoadModelPath;

            virtual const char* GetLoadModelPath() = 0;

            virtual void SetLoadModelPath(const char* value) = 0;

            /// @~chinese 方案存模型   @~english Save Model Data
            __declspec(property(put = SetSaveModelDataEnable, get = GetSaveModelDataEnable)) bool SaveModelDataEnable;

            virtual bool GetSaveModelDataEnable() = 0;

            virtual void SetSaveModelDataEnable(bool value) = 0;

            /// @~chinese 按ROI裁图   @~english Cut Via Roi
            __declspec(property(put = SetCutViaROIEnable, get = GetCutViaROIEnable)) bool CutViaROIEnable;

            virtual bool GetCutViaROIEnable() = 0;

            virtual void SetCutViaROIEnable(bool value) = 0;

            /// @~chinese 批处理使能(仅GPU模块生效)   @~english Batch Processing Enable(Takes effect only in GPU modules)
            __declspec(property(put = SetBatchProcessEnable, get = GetBatchProcessEnable)) bool BatchProcessEnable;

            virtual bool GetBatchProcessEnable() = 0;

            virtual void SetBatchProcessEnable(bool value) = 0;

            /// @~chinese 批处理等级，范围：[1,32](仅GPU模块生效)   @~english Batch Processing Level，range:[1,32](Takes effect only in GPU modules)
            __declspec(property(put = SetBatchProcessingLevel, get = GetBatchProcessingLevel)) int BatchProcessingLevel;

            virtual int GetBatchProcessingLevel() = 0;

            virtual void SetBatchProcessingLevel(int value) = 0;

            /// @~chinese 小目标模式   @~english Small Object Mode
            __declspec(property(put = SetSODEnable, get = GetSODEnable)) bool SODEnable;

            virtual bool GetSODEnable() = 0;

            virtual void SetSODEnable(bool value) = 0;

            /// @~chinese 横向滑窗数，范围：[1,16]   @~english Moving Window Number (Horizontally)，range:[1,16]
            __declspec(property(put = SetXSlidingWinNumOfSOD, get = GetXSlidingWinNumOfSOD)) int XSlidingWinNumOfSOD;

            virtual int GetXSlidingWinNumOfSOD() = 0;

            virtual void SetXSlidingWinNumOfSOD(int value) = 0;

            /// @~chinese 纵向滑窗数，范围：[1,16]   @~english Moving Window Number (Vertically)，range:[1,16]
            __declspec(property(put = SetYSlidingWinNumOfSOD, get = GetYSlidingWinNumOfSOD)) int YSlidingWinNumOfSOD;

            virtual int GetYSlidingWinNumOfSOD() = 0;

            virtual void SetYSlidingWinNumOfSOD(int value) = 0;

            /// @~chinese 滑窗重叠率，范围：[0,0.6]   @~english Moving Window Overlap Rate，range:[0,0.6]
            __declspec(property(put = SetSlidingWinOverlap, get = GetSlidingWinOverlap)) double SlidingWinOverlap;

            virtual double GetSlidingWinOverlap() = 0;

            virtual void SetSlidingWinOverlap(double value) = 0;

            /// @~chinese 最小分数，范围：[0,255]   @~english Minimum Score，range:[0,255]
            __declspec(property(put = SetMinScore, get = GetMinScore)) int MinScore;

            virtual int GetMinScore() = 0;

            virtual void SetMinScore(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the CnnFastFlawResults.  */
        ///DL快速图像分割结果类
        class CnnFastFlawResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnFastFlawResults() {}
            virtual ~CnnFastFlawResults() {}

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
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

			 /**
			 * @brief 最小分数
			 */
			 virtual int GetMinScore() = 0;

			 /**
			 * @brief 灰度值
			 */
			 virtual int GetGrayValue(int nIndex) = 0;


			 /**
			 * @brief 类别名称
			 */
			 virtual string GetClassName(int nIndex) = 0;

			 /**
			 * @brief 类别图
			 */
			 virtual ImageBaseData GetClassImageData() = 0;

        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CnnFastFlaw tool.  */
        ///DL快速图像分割工具类
        class IMVSCnnFastFlawModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCnnFastFlawModuTool() {}
            virtual ~IMVSCnnFastFlawModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CnnFastFlawParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CnnFastFlawResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSCnnFastFlawModuTool(IMVSCnnFastFlawModuTool&);
            IMVSCnnFastFlawModuTool& operator=(IMVSCnnFastFlawModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CnnFastFlaw.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSCnnFastFlawModu::IMVSCnnFastFlawModuTool * __stdcall GetCnnFastFlawToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CNNFASTFLAW_H_
