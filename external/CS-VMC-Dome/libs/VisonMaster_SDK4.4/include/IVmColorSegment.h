/***************************************************************************************************
 * File：IVmColorSegment.h
 * Note：@~chinese 颜色分割模块接口声明 @~english Interface for the ColorSegment tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_COLORSEGMENT_H_
#define _IVM_COLORSEGMENT_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSColorSegmentModu
    {
    /// \addtogroup 颜色分割
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///颜色分割ROI管理类
        class ColorSegmentRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
            virtual void SetRoiAnnulus(Annulus stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the ColorSegmentParams.  */
        ///颜色分割参数类
        class ColorSegmentParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ColorSegmentParams() {}
            virtual ~ColorSegmentParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual ColorSegmentRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 全局孔洞填充面积，范围：[0,10000000]   @~english Global Hole Area，range:[0,10000000]
            __declspec(property(put = SetHoleArea, get = GetHoleArea)) int HoleArea;

            virtual int GetHoleArea() = 0;

            virtual void SetHoleArea(int value) = 0;

            /// @~chinese 全局杂斑筛除面积，范围：[0,10000000]   @~english Global Clutter Area，range:[0,10000000]
            __declspec(property(put = SetClutterArea, get = GetClutterArea)) int ClutterArea;

            virtual int GetClutterArea() = 0;

            virtual void SetClutterArea(int value) = 0;

            /// @~chinese 颜色面积判断   @~english Color Total Area Check
            __declspec(property(put = SetAreaLimitEnable, get = GetAreaLimitEnable)) bool AreaLimitEnable;

            virtual bool GetAreaLimitEnable() = 0;

            virtual void SetAreaLimitEnable(bool value) = 0;

            /// @~chinese 面积范围，范围：[0,9999999]   @~english Area Range，range:[0,9999999]
            __declspec(property(put = SetAreaLimitLow, get = GetAreaLimitLow)) double AreaLimitLow;

            virtual double GetAreaLimitLow() = 0;

            virtual void SetAreaLimitLow(double value) = 0;

            /// @~chinese 面积范围，范围：[0,9999999]   @~english Area Range，range:[0,9999999]
            __declspec(property(put = SetAreaLimitHigh, get = GetAreaLimitHigh)) double AreaLimitHigh;

            virtual double GetAreaLimitHigh() = 0;

            virtual void SetAreaLimitHigh(double value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the ColorSegmentResults.  */
        ///颜色分割结果类
        class ColorSegmentResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ColorSegmentResults() {}
            virtual ~ColorSegmentResults() {}

        public:
             /**
             * @brief ROI圆弧
             */
             virtual Annulus GetROIAnnulus() = 0;

             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;

             /**
             * @brief 输出渲染图
             */
             virtual ImageBaseData GetOutRenderImage() = 0;

             /**
             * @brief 输出图像
             */
             virtual ImageBaseData GetOutputImage() = 0;

             /**
             * @brief 灰度值
             * @param nIndex[IN] 数据索引
             */
             virtual int GetGrayValue(int nIndex) = 0;

             /**
             * @brief 类别名称
             * @param nIndex[IN] 数据索引
             */
             virtual const char * GetClassName(int nIndex) = 0;

             /**
             * @brief 总面积
             */
             virtual float GetTotalArea() = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;


        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ColorSegment tool.  */
        ///颜色分割工具类
        class IMVSColorSegmentModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSColorSegmentModuTool() {}
            virtual ~IMVSColorSegmentModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ColorSegmentParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ColorSegmentResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSColorSegmentModuTool(IMVSColorSegmentModuTool&);
            IMVSColorSegmentModuTool& operator=(IMVSColorSegmentModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
#endif    ///< _IVM_COLORSEGMENT_H_
