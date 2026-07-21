/***************************************************************************************************
 * File：IVmCnnFlaw.h
 * Note：@~chinese DL图像分割模块接口声明 @~english Interface for the CnnFlaw tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CNNFLAW_H_
#define _IVM_CNNFLAW_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

#ifndef MVD_CNNSEGMENT_LABEL_NAME_MAX_LEN
#define MVD_CNNSEGMENT_LABEL_NAME_MAX_LEN (128)
#endif

#ifndef MVD_CNNSEGMENT_SCORE_HIST_BIN_NUM
#define MVD_CNNSEGMENT_SCORE_HIST_BIN_NUM (256)
#endif

namespace VisionMasterSDK
{
    namespace IMVSCnnFlawModu
    {
        /// @~chinese 运行模式   @~english Operating Mode
        typedef enum _AlgRunModeEnum
        {
            /// @~chinese 高性能模式   @~english High Performance Mode
            AlgRunMode_HpMode = 0x0,

            /// @~chinese 极速模式   @~english Fast Mode
            AlgRunMode_FastMode = 0x1,

        }AlgRunModeEnum;

    /// \addtogroup DL图像分割
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///DL图像分割ROI管理类
        class CnnFlawRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoisRectangle(RectBox* pRoiValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the CnnFlawParams.  */
        ///DL图像分割参数类
        class CnnFlawParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnFlawParams() {}
            virtual ~CnnFlawParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual CnnFlawRoiManager* GetModuRoiManager() = 0;

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

            /// @~chinese 采样系数，范围：[1,6]   @~english Sampling Coefficient，range:[1,6]
            __declspec(property(put = SetSampleInterval, get = GetSampleInterval)) int SampleInterval;

            virtual int GetSampleInterval() = 0;

            virtual void SetSampleInterval(int value) = 0;

            /// @~chinese 运行模式   @~english Operating Mode
            __declspec(property(put = SetAlgRunMode, get = GetAlgRunMode)) AlgRunModeEnum AlgRunMode;

            virtual AlgRunModeEnum GetAlgRunMode() = 0;

            virtual void SetAlgRunMode(AlgRunModeEnum value) = 0;

            /// @~chinese 最小分数，范围：[0,255]   @~english Minimum Score，range:[0,255]
            __declspec(property(put = SetMinScore, get = GetMinScore)) int MinScore;

            virtual int GetMinScore() = 0;

            virtual void SetMinScore(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the CnnFlawResults.  */
        ///DL图像分割结果类
        class CnnFlawResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnFlawResults() {}
            virtual ~CnnFlawResults() {}

        public:
             /**
             * @brief 检测区域
             * @param nIndex[IN] 数据索引
             */
             virtual RectBox GetDetectROIs(int nIndex) = 0;
            /**
            * @brief 模块状态
            */
            virtual int GetModuStatus() = 0;
            /**
            * @brief 检测区域
            * @deprecated V4.4废弃
            */
            virtual RectBox GetROI() = 0;
            /**
            * @brief 获取类别数
            */
            virtual int GetClassNum() = 0;
            /**
            * @brief 获取单类别标签名称
            */
            virtual void GetLabelName(IN int nIndex, OUT char szLabelName[MVD_CNNSEGMENT_LABEL_NAME_MAX_LEN]) = 0;
            /**
            * @brief 获取灰度值数量
            */
            virtual int GetGrayValueNum() = 0;
            /**
            * @brief 获取灰度值数量
            */
            virtual int GetGrayValue(int nIndex) = 0;
            /**
            * @brief 获取最小分数
            */
            virtual int GetMinScore() = 0;
            /**
            * @brief 获取图像分割类别图，像素值代表其所属的类别，背景的像素值为0
            */
            virtual ImageBaseData GetClassMap() = 0;
            /**
            * @brief 显示概率图
            */
            virtual ImageBaseData GetScoreMap() = 0;
            /**
            * @brief 获取概率直方图，用于调整阈值, 采用相对值(归一化)
            */
            virtual void GetScoreHist(unsigned int pnScoreHist[MVD_CNNSEGMENT_SCORE_HIST_BIN_NUM]) = 0;
            /**
            * @brief 获取输出类别的缺陷概率图数量，需要在运行参数界面添加输出类别，没有添加时该数量为0，最大数量为8
            */
            virtual int GetOutputScoreImageNum() = 0;
            /**
            * @brief 查询缺陷概率图
            * @param  nIndex [IN] 按索引查询输出类别的缺陷概率图
            * @return 缺陷概率图
            */
            virtual ImageBaseData QueryOutputScoreImageInfo(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CnnFlaw tool.  */
        ///DL图像分割工具类
        class IMVSCnnFlawModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCnnFlawModuTool() {}
            virtual ~IMVSCnnFlawModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CnnFlawParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CnnFlawResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSCnnFlawModuTool(IMVSCnnFlawModuTool&);
            IMVSCnnFlawModuTool& operator=(IMVSCnnFlawModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CnnFlaw.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSCnnFlawModu::IMVSCnnFlawModuTool * __stdcall GetCnnFlawToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CNNFLAW_H_
