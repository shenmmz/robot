/***************************************************************************************************
 * File：IVmColorRecognition.h
 * Note：@~chinese 颜色识别模块接口声明 @~english Interface for the ColorRecognition tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_ColorRecognition_H_
#define _IVM_ColorRecognition_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSColorRecognitionModu
    {
        /// @~chinese KNN距离   @~english KNN Distance
        typedef enum _KnnDistanceEnum
        {
            /// @~chinese 欧式距离   @~english Euclidean Distance
            KnnDistance_EuclideanDistance = 0x1,

            /// @~chinese 曼哈顿距离   @~english Manhattan Distance
            KnnDistance_ManhattanDistance = 0x2,

            /// @~chinese 相交距离   @~english Intersect Distance
            KnnDistance_IntersectDistance = 0x3,

            /// @~chinese 偏移距离   @~english Earthmovers Distance
            KnnDistance_EarthmoversDistance = 0x4,

        }KnnDistanceEnum;

    /// \addtogroup 颜色识别
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///颜色识别ROI管理类
        class ColorRecognitionRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

            /// @~chinese 多边形屏蔽区字符串（仅当次执行起效）   @~english Polygon Blind String(Only valid for this execution)
            virtual void SetBlindPolygonString(StringData* pBlindValue, unsigned int nValueNum) = 0;

        };

        /*  Note: Interface Classes of the parameter of the ColorRecognitionParams.  */
        ///颜色识别参数类
        class ColorRecognitionParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ColorRecognitionParams() {}
            virtual ~ColorRecognitionParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual ColorRecognitionRoiManager* GetModuRoiManager() = 0;

            /// @~chinese K值，范围：[1,100]   @~english K Value，range:[1,100]
            __declspec(property(put = SetKnnK, get = GetKnnK)) int KnnK;

            virtual int GetKnnK() = 0;

            virtual void SetKnnK(int value) = 0;

            /// @~chinese KNN距离   @~english KNN Distance
            __declspec(property(put = SetKnnDistance, get = GetKnnDistance)) KnnDistanceEnum KnnDistance;

            virtual KnnDistanceEnum GetKnnDistance() = 0;

            virtual void SetKnnDistance(KnnDistanceEnum value) = 0;

            /// @~chinese 识别类别   @~english Color Type
            __declspec(property(put = SetColorType, get = GetColorType)) bool ColorType;

            virtual bool GetColorType() = 0;

            virtual void SetColorType(bool value) = 0;

            /// @~chinese 最佳匹配名称   @~english Best Matched Name
            __declspec(property(put = SetTopTypeName, get = GetTopTypeName)) const char* TopTypeName;

            virtual const char* GetTopTypeName() = 0;

            virtual void SetTopTypeName(const char* value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
/// \addtogroup 宏定义
/// @{

        /// 直方图BIN数目上限
        const int MVD_COLOR_MAX_HIST_BIN_NUM = 256;

        /// 直方图I通道BIN数目上限
        const int MVD_COLOR_MAX_HIST_BIN_NUM_I = 8;
/// @}	
        
        /*  Note: 颜色识别结果特征数据类.  */
        ///颜色识别结果特征数据类
        class IColorRecognitionFeatureData
        {
        protected:
            ///< 构造与析构函数
            explicit IColorRecognitionFeatureData(){}
            virtual ~IColorRecognitionFeatureData(){}
        public:
            /**
             * @brief H通道bin数目
             */
            virtual unsigned int GetHChannelBinNum() = 0;
            /**
             * @brief H通道数据
             * @param pfHData [OUT] H通道数据
             */
            virtual void GetHChannelData(float pfHData[MVD_COLOR_MAX_HIST_BIN_NUM]) = 0;
            /**
             * @brief S通道bin数目
             */
            virtual unsigned int GetSChannelBinNum() = 0;
            /**
             * @brief S通道数据
             * @param pfSData [OUT] S通道数据
             */
            virtual void GetSChannelData(float pfSData[MVD_COLOR_MAX_HIST_BIN_NUM]) = 0;
            /**
             * @brief I通道bin数目
             */
            virtual unsigned int GetIChannelBinNum() = 0;
            /**
             * @brief I通道数据
             * @param pfIData [OUT] I通道数据
             */
            virtual void GetIChannelData(float pfIData[MVD_COLOR_MAX_HIST_BIN_NUM_I]) = 0;
        };

        /*  Note: 颜色识别信息描述类.  */
        ///颜色识别信息描述类
        class IColorRecognitionInfo
        {
        protected:
            ///< 构造与析构函数
            explicit IColorRecognitionInfo(){}
            virtual ~IColorRecognitionInfo(){}

        public:
            /**
             * @brief 识别名字
             */
            virtual const char* GetLabelName() = 0;
            /**
             * @brief 识别分数
             */
            virtual float GetScore() = 0;
        };

        /*  Note: Interface Classes of the result of the ColorRecognitionResults.  */
        ///颜色识别结果类
        class ColorRecognitionResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit ColorRecognitionResults() {}
            virtual ~ColorRecognitionResults() {}

        public:
             /**
             * @brief 最佳分数
             */
             virtual float GetTopScore() = 0;

             /**
             * @brief 最佳匹配名称
             */
             virtual const char * GetTopTypeName() = 0;

             /**
             * @brief 屏蔽区字符串
             */
             virtual StringData* GetBlindPolygonString(unsigned int& nValueNum) = 0;

             /**
             * @brief 屏蔽区多边形点集
             */
             virtual PointF GetBlindPolygonPoints(int nIndex) = 0;

             /**
             * @brief 屏蔽区多边形点数
             */
             virtual int GetBlindPolygonPointNum(int nIndex) = 0;

             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;
            /**
             * @brief 识别置信度
             */
            virtual float GetConfidence() = 0;
            /**
             * @brief 总识别信息
             */
            virtual IColorRecognitionInfo* GetTotalRecognitionInfo() = 0;
            /**
             * @brief 识别到的类别数量
             */
            virtual unsigned int GetNumOfClasses() = 0;
            /**
             * @brief 各类别信息
             * @param nIndex [IN] 类别索引,[0, ClassNum)
             */
            virtual IColorRecognitionInfo* GetRecognitionInfoAt(unsigned int nIndex) = 0;
            /**
             * @brief 样本直方图数据
             */
            virtual IColorRecognitionFeatureData* GetSampleFeatureData() = 0;
            /**
             * @brief 模型直方图数据(模型中最匹配的标签类的样本特征信息)
             */
            virtual IColorRecognitionFeatureData* GetModelFeatureData() = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the ColorRecognition tool.  */
        ///颜色识别工具类
        class IMVSColorRecognitionModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSColorRecognitionModuTool() {}
            virtual ~IMVSColorRecognitionModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual ColorRecognitionParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual ColorRecognitionResults* GetResult() = 0;

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
            IMVSColorRecognitionModuTool(IMVSColorRecognitionModuTool&);
            IMVSColorRecognitionModuTool& operator=(IMVSColorRecognitionModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for ColorRecognition.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSColorRecognitionModu::IMVSColorRecognitionModuTool * __stdcall GetColorRecognitionToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_ColorRecognition_H_
