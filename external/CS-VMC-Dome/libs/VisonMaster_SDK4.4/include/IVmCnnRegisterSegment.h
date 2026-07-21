/***************************************************************************************************
 * File：IVmCnnRegisterSegment.h
 * Note：@~chinese 注册分割G模块接口声明 @~english Interface for the CnnRegisterSegment tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CNNREGISTERSEGMENT_H_
#define _IVM_CNNREGISTERSEGMENT_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCnnRegisterSegmentModu
    {
    /// \addtogroup 注册分割G
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///注册分割GROI管理类
        class CnnRegisterSegmentRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoisRectangle(RectBox* pRoiValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the CnnRegisterSegmentParams.  */
        ///注册分割G参数类
        class CnnRegisterSegmentParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnRegisterSegmentParams() {}
            virtual ~CnnRegisterSegmentParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual CnnRegisterSegmentRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 最大查找个数，范围：[1,1000]   @~english Max Number to Find，range:[1,1000]
            __declspec(property(put = SetMaxObjNum, get = GetMaxObjNum)) int MaxObjNum;

            virtual int GetMaxObjNum() = 0;

            virtual void SetMaxObjNum(int value) = 0;

            /// @~chinese 最小置信度，范围：[0.01,1.0]   @~english Min Confidence，range:[0.01,1.0]
            __declspec(property(put = SetMinScore, get = GetMinScore)) double MinScore;

            virtual double GetMinScore() = 0;

            virtual void SetMinScore(double value) = 0;

            /// @~chinese 最大重叠率，范围：[0.01,1.0]   @~english Overlap Threshold，range:[0.01,1.0]
            __declspec(property(put = SetMaxOverlap, get = GetMaxOverlap)) double MaxOverlap;

            virtual double GetMaxOverlap() = 0;

            virtual void SetMaxOverlap(double value) = 0;

            /// @~chinese 面积使能   @~english Area Enable
            __declspec(property(put = SetAreaEnable, get = GetAreaEnable)) bool AreaEnable;

            virtual bool GetAreaEnable() = 0;

            virtual void SetAreaEnable(bool value) = 0;

            /// @~chinese 面积范围，范围：[1,16000000]   @~english AreaRange，range:[1,16000000]
            __declspec(property(put = SetMinArea, get = GetMinArea)) int MinArea;

            virtual int GetMinArea() = 0;

            virtual void SetMinArea(int value) = 0;

            /// @~chinese 面积范围，范围：[1,16000000]   @~english AreaRange，range:[1,16000000]
            __declspec(property(put = SetMaxArea, get = GetMaxArea)) int MaxArea;

            virtual int GetMaxArea() = 0;

            virtual void SetMaxArea(int value) = 0;

            /// @~chinese 掩膜阈值，范围：[1,100]   @~english RunParam_MaskThresh，range:[1,100]
            __declspec(property(put = SetMaskThresh, get = GetMaskThresh)) int MaskThresh;

            virtual int GetMaskThresh() = 0;

            virtual void SetMaskThresh(int value) = 0;

			/// @~chinese 个数判断   @~english Number Check
			__declspec(property(put = SetNumLimitEnable, get = GetNumLimitEnable)) bool NumLimitEnable;

			virtual bool GetNumLimitEnable() = 0;

			virtual void SetNumLimitEnable(bool value) = 0;

			/// @~chinese 个数范围，范围：[0,99999]   @~english Number Range，range:[0,99999]
			__declspec(property(put = SetNumLimitLow, get = GetNumLimitLow)) int NumLimitLow;

			virtual int GetNumLimitLow() = 0;

			virtual void SetNumLimitLow(int value) = 0;

			/// @~chinese 个数范围，范围：[0,99999]   @~english Number Range，range:[0,99999]
			__declspec(property(put = SetNumLimitHigh, get = GetNumLimitHigh)) int NumLimitHigh;

			virtual int GetNumLimitHigh() = 0;

			virtual void SetNumLimitHigh(int value) = 0;

			/// @~chinese 置信度判断   @~english Confidence Check
			__declspec(property(put = SetScoreLimitEnable, get = GetScoreLimitEnable)) bool ScoreLimitEnable;

			virtual bool GetScoreLimitEnable() = 0;

			virtual void SetScoreLimitEnable(bool value) = 0;

			/// @~chinese 置信度范围，范围：[0.0,1.0]   @~english Confidence Range，range:[0.0,1.0]
			__declspec(property(put = SetScoreLimitLow, get = GetScoreLimitLow)) double ScoreLimitLow;

			virtual double GetScoreLimitLow() = 0;

			virtual void SetScoreLimitLow(double value) = 0;

			/// @~chinese 置信度范围，范围：[0.0,1.0]   @~english Confidence Range，range:[0.0,1.0]
			__declspec(property(put = SetScoreLimitHigh, get = GetScoreLimitHigh)) double ScoreLimitHigh;

			virtual double GetScoreLimitHigh() = 0;

			virtual void SetScoreLimitHigh(double value) = 0;

			/// @~chinese 类别号判断   @~english Category No. Check
			__declspec(property(put = SetClassLimitEnable, get = GetClassLimitEnable)) bool ClassLimitEnable;

			virtual bool GetClassLimitEnable() = 0;

			virtual void SetClassLimitEnable(bool value) = 0;

			/// @~chinese 类别号范围，范围：[0,99999] @~english No. Range, Range:[0,99999]
			__declspec(property(put = SetClassLimitLow, get = GetClassLimitLow)) int ClassLimitLow;

			virtual int GetClassLimitLow() = 0;

			virtual void SetClassLimitLow(int value) = 0;

			/// @~chinese 类别号范围，范围：[0,99999] @~english No. Range, Range:[0,99999]
			__declspec(property(put = SetClassLimitHigh, get = GetClassLimitHigh)) int ClassLimitHigh;

			virtual int GetClassLimitHigh() = 0;

			virtual void SetClassLimitHigh(int value) = 0;

			/// @~chinese 类别名称判断   @~english Category Name Check
			__declspec(property(put = SetCategoryNameLimitEnable, get = GetCategoryNameLimitEnable)) bool CategoryNameLimitEnable;

			virtual bool GetCategoryNameLimitEnable() = 0;

			virtual void SetCategoryNameLimitEnable(bool value) = 0;

			/// @~chinese 类别名称   @~english Class Name
			__declspec(property(put = SetCategoryNameLimit, get = GetCategoryNameLimit)) const char* CategoryNameLimit;

			virtual const char* GetCategoryNameLimit() = 0;

			virtual void SetCategoryNameLimit(const char* value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the CnnRegisterSegmentResults.  */
        ///注册分割G结果类
        class CnnRegisterSegmentResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnRegisterSegmentResults() {}
            virtual ~CnnRegisterSegmentResults() {}

        public:
             /**
             * @brief 边缘点个数
             * @param nIndex[IN] 数据索引
             */
            virtual int GetContourNum(int nIndex) = 0;

             /**
             * @brief 多边形轮廓点
             * @param nIndex[IN] 数据索引
             */
            virtual PointF GetContourPoint(int nIndex) = 0;

            /**
            * @brief 检测区域
            * @param nIndex[IN] 数据索引
            */
            virtual RectBox GetROI(int nIndex) = 0;

             /**
             * @brief 输出渲染图
             */
             virtual ImageBaseData GetOutRenderImage() = 0;

             /**
             * @brief 输出掩膜图
             */
             virtual ImageBaseData GetClassImage() = 0;

             /**
             * @brief 目标信息矩形
             * @param nIndex[IN] 数据索引
             */
             virtual RectBox GetTargetInfoRect(int nIndex) = 0;

             /**
             * @brief 类别名称
             * @param nIndex[IN] 数据索引
             */
             virtual const char * GetClassName(int nIndex) = 0;

             /**
             * @brief 目标面积
             * @param nIndex[IN] 数据索引
             */
             virtual float GetObjArea(int nIndex) = 0;

             /**
             * @brief 目标像素值
             * @param nIndex[IN] 数据索引
             */
             virtual int GetObjPixel(int nIndex) = 0;

             /**
             * @brief 目标置信度
             * @param nIndex[IN] 数据索引
             */
             virtual float GetObjConf(int nIndex) = 0;

             /**
             * @brief 目标类别
             * @param nIndex[IN] 数据索引
             */
             virtual int GetObjClass(int nIndex) = 0;

             /**
             * @brief 目标个数
             */
             virtual int GetObjNum() = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;


        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CnnRegisterSegment tool.  */
        ///注册分割G工具类
        class IMVSCnnRegisterSegmentModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCnnRegisterSegmentModuTool() {}
            virtual ~IMVSCnnRegisterSegmentModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CnnRegisterSegmentParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CnnRegisterSegmentResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

            /**
            * @brief 导入模型
            * @param strPath [IN] 模型路径
            * @return 无，出错时抛出异常
            */
            virtual void ImportModelData(IN const char * const strPath) = 0;

        private:
            IMVSCnnRegisterSegmentModuTool(IMVSCnnRegisterSegmentModuTool&);
            IMVSCnnRegisterSegmentModuTool& operator=(IMVSCnnRegisterSegmentModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
#endif    ///< _IVM_CNNREGISTERSEGMENT_H_
