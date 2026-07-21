/***************************************************************************************************
 * File：IVmDivideImage.h
 * Note：@~chinese 划片拆分模块接口声明 @~english Interface for the DivideImage tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_DIVIDEIMAGE_H_
#define _IVM_DIVIDEIMAGE_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSDivideImageModu
    {
    /// \addtogroup 划片拆分
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///划片拆分ROI管理类
        class DivideImageRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the DivideImageParams.  */
        ///划片拆分参数类
        class DivideImageParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit DivideImageParams() {}
            virtual ~DivideImageParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual DivideImageRoiManager* GetModuRoiManager() = 0;

            /// @~chinese X方向划片数，范围：[1,50]   @~english Divisions in X Direction，range:[1,50]
            __declspec(property(put = SetNumX, get = GetNumX)) int NumX;

            virtual int GetNumX() = 0;

            virtual void SetNumX(int value) = 0;

            /// @~chinese Y方向划片数，范围：[1,50]   @~english Divisions in Y Direction，range:[1,50]
            __declspec(property(put = SetNumY, get = GetNumY)) int NumY;

            virtual int GetNumY() = 0;

            virtual void SetNumY(int value) = 0;

            /// @~chinese X方向重叠率，范围：[0,100]   @~english X-Direction Overlap Rate，range:[0,100]
            __declspec(property(put = SetOverlaprateX, get = GetOverlaprateX)) int OverlaprateX;

            virtual int GetOverlaprateX() = 0;

            virtual void SetOverlaprateX(int value) = 0;

            /// @~chinese Y方向重叠率，范围：[0,100]   @~english Y-Direction Overlap Rate，range:[0,100]
            __declspec(property(put = SetOverlaprateY, get = GetOverlaprateY)) int OverlaprateY;

            virtual int GetOverlaprateY() = 0;

            virtual void SetOverlaprateY(int value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the DivideImageResults.  */
        ///划片拆分结果类
        class DivideImageResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit DivideImageResults() {}
            virtual ~DivideImageResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;

            /**
            * @brief 划片个数
            */
            virtual int GetDivideCount() = 0;
            /**
            * @brief 检测区域
            */
            virtual RectBox GetROI() = 0;
            /**
            * @brief 划片BOX
            */
            virtual RectBox GetDivideRectAt(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the DivideImage tool.  */
        ///划片拆分工具类
        class IMVSDivideImageModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSDivideImageModuTool() {}
            virtual ~IMVSDivideImageModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual DivideImageParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual DivideImageResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSDivideImageModuTool(IMVSDivideImageModuTool&);
            IMVSDivideImageModuTool& operator=(IMVSDivideImageModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for DivideImage.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSDivideImageModu::IMVSDivideImageModuTool * __stdcall GetDivideImageToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_DIVIDEIMAGE_H_
