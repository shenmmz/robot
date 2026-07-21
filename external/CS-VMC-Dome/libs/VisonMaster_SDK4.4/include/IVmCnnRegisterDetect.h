/***************************************************************************************************
 * File：IVmCnnRegisterDetect.h
 * Note：@~chinese 注册检测模块接口声明 @~english Interface for the CnnRegisterDetect tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CNNREGISTERDETECT_H_
#define _IVM_CNNREGISTERDETECT_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCnnRegisterDetectModu
    {
        /// @~chinese 目标排序   @~english Sort Object
        typedef enum _OrderTypeEnum
        {
            /// @~chinese 按中心点X坐标升序   @~english Sort by X low to high
            OrderType_CoordX = 0x1,

            /// @~chinese 按中心点Y坐标升序   @~english Sort by Y low to high
            OrderType_CoordY = 0x2,

            /// @~chinese 按置信度降序   @~english Sort by Confidence，High to low
            OrderType_Score = 0x3,

        }OrderTypeEnum;

    /// \addtogroup 注册检测
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///注册检测ROI管理类
        class CnnRegisterDetectRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoisRectangle(RectBox* pRoiValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the CnnRegisterDetectParams.  */
        ///注册检测参数类
        class CnnRegisterDetectParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnRegisterDetectParams() {}
            virtual ~CnnRegisterDetectParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual CnnRegisterDetectRoiManager* GetModuRoiManager() = 0;

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

            /// @~chinese 最大覆盖率，范围：[0.01,1.0]   @~english Max Coverage，range:[0.01,1.0]
            __declspec(property(put = SetMaxCoverage, get = GetMaxCoverage)) double MaxCoverage;

            virtual double GetMaxCoverage() = 0;

            virtual void SetMaxCoverage(double value) = 0;

            /// @~chinese 目标排序   @~english Sort Object
            __declspec(property(put = SetOrderType, get = GetOrderType)) OrderTypeEnum OrderType;

            virtual OrderTypeEnum GetOrderType() = 0;

            virtual void SetOrderType(OrderTypeEnum value) = 0;

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

            /// @~chinese 宽度使能   @~english Width Enable
            __declspec(property(put = SetWidthEnable, get = GetWidthEnable)) bool WidthEnable;

            virtual bool GetWidthEnable() = 0;

            virtual void SetWidthEnable(bool value) = 0;

            /// @~chinese 宽度范围，范围：[1,40000]   @~english Width Range，range:[1,40000]
            __declspec(property(put = SetMinWidth, get = GetMinWidth)) int MinWidth;

            virtual int GetMinWidth() = 0;

            virtual void SetMinWidth(int value) = 0;

            /// @~chinese 宽度范围，范围：[1,40000]   @~english Width Range，range:[1,40000]
            __declspec(property(put = SetMaxWidth, get = GetMaxWidth)) int MaxWidth;

            virtual int GetMaxWidth() = 0;

            virtual void SetMaxWidth(int value) = 0;

            /// @~chinese 高度使能   @~english Height Enable
            __declspec(property(put = SetHeightEnable, get = GetHeightEnable)) bool HeightEnable;

            virtual bool GetHeightEnable() = 0;

            virtual void SetHeightEnable(bool value) = 0;

            /// @~chinese 高度范围，范围：[1,40000]   @~english Height Range，range:[1,40000]
            __declspec(property(put = SetMinHeight, get = GetMinHeight)) int MinHeight;

            virtual int GetMinHeight() = 0;

            virtual void SetMinHeight(int value) = 0;

            /// @~chinese 高度范围，范围：[1,40000]   @~english Height Range，range:[1,40000]
            __declspec(property(put = SetMaxHeight, get = GetMaxHeight)) int MaxHeight;

            virtual int GetMaxHeight() = 0;

            virtual void SetMaxHeight(int value) = 0;
			
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
        /*  Note: Interface Classes of the result of the CnnRegisterDetectResults.  */
        ///注册检测结果类
        class CnnRegisterDetectResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnRegisterDetectResults() {}
            virtual ~CnnRegisterDetectResults() {}

        public:
            /**
            * @brief 检测区域
            * @param nIndex[IN] 数据索引
            */
            virtual RectBox GetROI(int nIndex) = 0;

            /**
            * @brief 目标面积
            * @param nIndex[IN] 数据索引
            */
            virtual float GetObjArea(int nIndex) = 0;

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
        /*  Note: Note: This is the class for the CnnRegisterDetect tool.  */
        ///注册检测工具类
        class IMVSCnnRegisterDetectModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCnnRegisterDetectModuTool() {}
            virtual ~IMVSCnnRegisterDetectModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CnnRegisterDetectParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CnnRegisterDetectResults* GetResult() = 0;

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
            IMVSCnnRegisterDetectModuTool(IMVSCnnRegisterDetectModuTool&);
            IMVSCnnRegisterDetectModuTool& operator=(IMVSCnnRegisterDetectModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
#endif    ///< _IVM_CNNREGISTERDETECT_H_
