/***************************************************************************************************
 * File：IVmRegionCopy.h
 * Note：@~chinese 拷贝填充模块接口声明 @~english Interface for the RegionCopy tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_REGIONCOPY_H_
#define _IVM_REGIONCOPY_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSRegionCopyModu
    {
        /// @~chinese 处理类型   @~english Process Type
        typedef enum _RegionTypeEnum
        {
            /// @~chinese 拷贝   @~english Copy
            RegionType_Copy = 0x1,

            /// @~chinese 填充   @~english Fill
            RegionType_Fill = 0x2,

        }RegionTypeEnum;

    /// \addtogroup 拷贝填充
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///拷贝填充ROI管理类
        class RegionCopyRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 圆环ROI（仅当次执行起效）   @~english Annulus ROI(Only valid for this execution)
            virtual void SetRoiAnnulus(Annulus stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the RegionCopyParams.  */
        ///拷贝填充参数类
        class RegionCopyParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit RegionCopyParams() {}
            virtual ~RegionCopyParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual RegionCopyRoiManager* GetModuRoiManager() = 0;

            /// @~chinese 处理类型   @~english Process Type
            __declspec(property(put = SetRegionType, get = GetRegionType)) RegionTypeEnum RegionType;

            virtual RegionTypeEnum GetRegionType() = 0;

            virtual void SetRegionType(RegionTypeEnum value) = 0;

            /// @~chinese 区域内填充值，范围：[0,255]   @~english Inside Fill Value，range:[0,255]
            __declspec(property(put = SetFillVal, get = GetFillVal)) int FillVal;

            virtual int GetFillVal() = 0;

            virtual void SetFillVal(int value) = 0;

            /// @~chinese 区域外填充值，范围：[0,255]   @~english Outside Fill Value，range:[0,255]
            __declspec(property(put = SetExtFillVal, get = GetExtFillVal)) int ExtFillVal;

            virtual int GetExtFillVal() = 0;

            virtual void SetExtFillVal(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the RegionCopyResults.  */
        ///拷贝填充结果类
        class RegionCopyResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit RegionCopyResults() {}
            virtual ~RegionCopyResults() {}

        public:
             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;

             /**
             * @brief 输出掩膜
             */
             virtual ImageBaseData GetOutputMask() = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
             /**
             * @brief ROI圆弧
             */
             virtual Annulus GetROIAnnulus() = 0;
            /**
             * @brief 输出图像
             */
            virtual ImageBaseData GetOutputImage() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the RegionCopy tool.  */
        ///拷贝填充工具类
        class IMVSRegionCopyModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSRegionCopyModuTool() {}
            virtual ~IMVSRegionCopyModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual RegionCopyParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual RegionCopyResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSRegionCopyModuTool(IMVSRegionCopyModuTool&);
            IMVSRegionCopyModuTool& operator=(IMVSRegionCopyModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for RegionCopy.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSRegionCopyModu::IMVSRegionCopyModuTool * __stdcall GetRegionCopyToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_REGIONCOPY_H_
