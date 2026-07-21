/***************************************************************************************************
 * File：IVmGroup.h
 * Note：Interface for the group tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_GROUP_H_
#define _IVM_GROUP_H_

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"
#include "IVmDynamicIoDefine.h"
#include "Variable.h"
using namespace VisionMasterSDK::VariableModule;
namespace VisionMasterSDK
{
    namespace IMVSGroup
    {
        /// @~chinese 数据类型 @~english Data Type
        typedef enum DataTypeEnum
        {
            /// @~chinese int @~english int
            DataType_Int = 0x1,

            /// @~chinese float @~english float
            DataType_Float = 0x2,

            /// @~chinese string @~english string
            DataType_String = 0x3,

        }DataTypeEnum;

        /// @~chinese 比较运算 @~english Compare Operate
        typedef enum CmpTypeEnum
        {
            /// >
            CmpType_GT = 0x1,

            /// <
            CmpType_LT = 0x2,

            /// =
            CmpType_EQ = 0x3,

            /// !=
            CmpType_NE = 0x4,

            /// >=
            CmpType_GE = 0x5,

            /// <=
            CmpType_LE = 0x6,

        }CmpTypeEnum;

    /// \addtogroup Group
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the parameter of the IMVSGroupParams.  */
        /// Group参数类
        class IMVSGroupParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSGroupParams() {}
            virtual ~IMVSGroupParams() {}

        public:

            /// @~chinese 循环使能 @~english Loop Enable
            __declspec(property(put = SetEnableLoop, get = GetEnableLoop)) bool EnableLoop;

            virtual bool GetEnableLoop() = 0;

            virtual void SetEnableLoop(bool value) = 0;

            /// @~chinese 循环间隔（ms） @~english Cycle Time(ms)
            __declspec(property(put = SetLoopTimeGap, get = GetLoopTimeGap)) int LoopTimeGap;

            virtual int GetLoopTimeGap() = 0;

            virtual void SetLoopTimeGap(int value) = 0;

            /// @~chinese 中断循环 @~english Break Loop
            __declspec(property(put = SetEnableBreak, get = GetEnableBreak)) bool EnableBreak;

            virtual bool GetEnableBreak() = 0;

            virtual void SetEnableBreak(bool value) = 0;

            /// @~chinese 数据类型 @~english Data Type
            __declspec(property(put = SetDataType, get = GetDataType)) DataTypeEnum DataType;

            virtual DataTypeEnum GetDataType() = 0;

            virtual void SetDataType(DataTypeEnum value) = 0;

            /// @~chinese 比较运算 @~english Compare Operate
            __declspec(property(put = SetCmpType, get = GetCmpType)) CmpTypeEnum CmpType;

            virtual CmpTypeEnum GetCmpType() = 0;

            virtual void SetCmpType(CmpTypeEnum value) = 0;

            /**
            *  @brief  设置Int数据（仅当次执行起效）
            *  @param  strParamName [IN] 参数名称
            *  @param  anIntVal     [IN] int型指针数组
            *  @param  nValNum      [IN] 数组长度
            */
            virtual void SetInputInt(const char * const strParamName, const int * const anIntVal, const int nValNum) = 0;
            /**
            *  @brief  设置Float数据（仅当次执行起效）
            *  @param  strParamName [IN] 参数名称
            *  @param  anFloatVal   [IN] float型指针数组
            *  @param  nValNum      [IN] 数组长度
            */
            virtual void SetInputFloat(const char * const strParamName, const float * const anFloatVal, const int nValNum) = 0;
            /**
            *  @brief  设置String数据（仅当次执行起效）
            *  @param  strParamName [IN] 参数名称
            *  @param  astStrData   [IN] InputStringData结构体指针数组
            *  @param  nValNum      [IN] 数组长度
            */
            virtual void SetInputString(const char * const strParamName, const InputStringData * const astStrData, const int nValNum) = 0;
            /**
            *  @brief  设置动态输入图像数据（仅当次执行起效）
            *  @param  strParamName  [IN]    参数名称
            *  @param  pstImageData  [OUT]   IoImage结构体指针数组
            */
            virtual void SetInputImageV2(const char * const strParamName, const IoImage * const pstImageData) = 0;

            /**
            *  @brief  设置动态输入矩形框数据（仅当次执行起效）
            *  @param  strParamName  [IN]   参数名称
            *  @param  pstBox        [IN]  IoRectBoxArray结构体指针数组
            */
            virtual void SetInputBox(const char * const strParamName, const IoRectBoxArray * const pstBox) = 0;

            /**
            *  @brief  设置动态输入圆环ROI数据（仅当次执行起效）
            *  @param  strParamName  [IN]   参数名称
            *  @param  pstAnnulus    [IN]  IoAnnulusArray结构体指针数组
            */
            virtual void SetInputAnnulus(const char * const strParamName, const IoAnnulusArray * const pstAnnulus) = 0;

            /**
            *  @brief  设置动态输入圆形数据（仅当次执行起效）
            *  @param  strParamName  [IN]   参数名称
            *  @param  pstCircle     [IN]  IoCircleArray结构体指针数组
            */
            virtual void SetInputCircle(const char * const strParamName, const IoCircleArray * const pstCircle) = 0;

            /**
            *  @brief  设置动态输入椭圆形数据（仅当次执行起效）
            *  @param  strParamName   [IN] 参数名称
            *  @param  IoEllipseArray [IN] 结构体指针数组
            */
			virtual void SetInputEllipse(const char * const strParamName, const IoEllipseArray * const pstEllipse) = 0;

            /**
            *  @brief  设置动态输入线型数据（仅当次执行起效）
            *  @param  strParamName  [IN]    参数名称
            *  @param  pstLine       [IN]   IoLineArray结构体指针数组
            */
            virtual void SetInputLine(const char * const strParamName, const IoLineArray * const pstLine) = 0;

            /**
            *  @brief  设置动态输入点型数据（仅当次执行起效）
            *  @param  strParamName  [IN]    参数名称
            *  @param  pstPointF     [IN]   IoPointArray结构体指针数组
            */
            virtual void SetInputPoint(const char * const strParamName, const IoPointArray * const pstPointF) = 0;

            /**
            *  @brief  设置动态输入位置修正数据（仅当次执行起效）
            *  @param  strParamName  [IN]   参数名称
            *  @param  pstFixture    [IN]  IoFixtureArray结构体指针数组
            */
            virtual void SetInputFixture(const char * const strParamName, const IoFixtureArray * const pstFixture) = 0;

            /**
            *  @brief  设置动态输入矩形数据（仅当次执行起效）
            *  @param  strParamName  [IN]    参数名称
            *  @param  pstRectF      [IN]   IoRectFArray结构体指针数组
            */
            virtual void SetInputRect(const char * const strParamName, const IoRectFArray * const pstRectF) = 0;

            /**
            *  @brief  设置动态输入类别信息数据（仅当次执行起效）
            *  @param  strParamName  [IN]    参数名称
            *  @param  pstClassInfo  [IN]   IoClassInfoArray结构体指针数组
            */
            virtual void SetInputClassInfo(const char * const strParamName, const IoClassInfoArray * const pstClassInfo) = 0;

            /**
            *  @brief  设置动态输入像素图像数据(仅当次执行起效)
            *  @param  strParamName   [IN]   参数名称
            *  @param  pstPixelImage  [IN]  IoPixelImage结构体指针数组
            */
            virtual void SetInputPixelImage(const char * const strParamName, const IoPixelImage * const pstPixelImage) = 0;
            /**
            *  @brief  设置动态输入多边形数据(仅当次执行起效)
            *  @param  strParamName  [IN]   参数名称
            *  @param  pstPolygon    [IN]  IoPolygonArray结构体指针数组
            */
            virtual void SetInputPolygon(const char * const strParamName, const IoPolygonArray * const pstPolygon) = 0;
            /**
            *  @brief  设置动态输入位姿数据（仅当次执行起效）
            *  @param  strParamName  [IN]   参数名称
            *  @param  pstPosture    [IN]  IoPostureArray结构体指针数组
            */
            virtual void SetInputPosture(const char * const strParamName, const IoPostureArray * const pstPosture) = 0;
            /**
            *  @brief  获取所有输入名称信息
            */
			virtual IoNameInfoArray GetAllInputNameInfo() = 0;
        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        /*  Note: Interface Classes of the result of the IMVSGroupResults.  */
        /// Group结果类
        class IMVSGroupResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSGroupResults() {}
            virtual ~IMVSGroupResults() {}

        public:
            /**
            * @brief 获取模块状态
            * @return 模块状态
            */
            virtual int GetModuStatus() = 0;

            /**
            * @brief 获取整型输出
            * @param strParamName  [IN]  参数名称
            * @return 整型数组,出错时抛出异常
            */
            virtual IntDataArray GetOutputInt(IN const char * const strParamName) = 0;

            /**
            * @brief 获取浮点型输出
            * @param strParamName  [IN]  参数名称
            * @return 浮点型数组,出错时抛出异常
            */
            virtual FloatDataArray GetOutputFloat(IN const char * const strParamName) = 0;

            /**
            * @brief 获取字符串型输出
            * @param strParamName  [IN]  参数名称
            * @return 字符串型数组,出错时抛出异常
            */
            virtual StringDataArray GetOutputString(IN const char * const strParamName) = 0;

            /**
            * @brief 获取图像像素数据输出
            * @param strParamName  [IN]  参数名称
            * @return 图像像素数据,出错时抛出异常
            */
            virtual BaseDataInfo GetOutputImage(IN const char * const strParamName) = 0;

            /**
            * @brief 获取点集型输出
            * @param strParamName  [IN]  参数名称
            * @return 点集,出错时抛出异常
            */
            virtual MatchOutline GetOutputPointset(IN const char * const strParamName) = 0;

            /**
            * @brief 获取二进制数据型输出
            * @param strParamName  [IN]  参数名称
            * @return 二进制数据,出错时抛出异常
            */
            virtual ByteArrayData GetOutputByteArray(IN const char * const strParamName) = 0;
            /**
            * @brief 获取图像结果（整个图像）
            * @param strParamName  [IN]  参数名称
            * @return 图像结果,出错时抛出异常
            */
            virtual IoImage GetOutputImageV2(IN const char * const strParamName) = 0;

            /**
            * @brief 获取点输出数组
            * @param strParamName  [IN]  参数名称
            * @return 点输出数组,出错时抛出异常
            */
            virtual IoPointArray GetOutputPointArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取直线输出数组
            * @param strParamName  [IN]  参数名称
            * @return 直线输出数组,出错时抛出异常
            */
            virtual IoLineArray GetOutputLineArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取圆输出数组
            * @param strParamName  [IN]  参数名称
            * @return 圆输出数组,出错时抛出异常
            */
            virtual IoCircleArray GetOutputCircleArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取椭圆输出数组
            * @param strParamName  [IN]  参数名称
            * @return 椭圆输出数组,出错时抛出异常
            */
			virtual IoEllipseArray GetOutputEllipseArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取带角度矩形输出数组
            * @param strParamName  [IN]  参数名称
            * @return 带角度矩形输出数组,出错时抛出异常
            */
            virtual IoRectBoxArray GetOutputBoxArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取无角度矩形（浮点型）输出数组
            * @param strParamName  [IN]  参数名称
            * @return 无角度矩形(浮点型)输出数组,出错时抛出异常
            */
            virtual IoRectFArray GetOutputRectArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取位置修正输出数组
            * @param strParamName  [IN]  参数名称
            * @return 位置修正输出数组,出错时抛出异常
            */
            virtual IoFixtureArray GetOutputFixtureArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取圆环输出数组
            * @param strParamName  [IN]  参数名称
            * @return 圆环输出数组,出错时抛出异常
            */
            virtual IoAnnulusArray GetOutputAnnulusArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取类别信息输出数组
            * @param strParamName  [IN]  参数名称
            * @return 类别信息输出数组,出错时抛出异常
            */
            virtual IoClassInfoArray GetOutputClassInfoArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取带类别信息的图（界面可根据类别信息将灰度图显示成彩图）
            * @param strParamName  [IN]  参数名称
            * @return 带类别信息的图,出错时抛出异常
            */
            virtual IoPixelImage GetOutputPixelImage(IN const char * const strParamName) = 0;

            /**
            * @brief 获取位姿数组
            * @param strParamName  [IN]  参数名称
            * @return 位姿数组,出错时抛出异常
            */
            virtual IoPostureArray GetOutputPostureArray(IN const char * const strParamName) = 0; 
            
            /**
            * @brief 获取多边形输出数组
            * @param strParamName  [IN]  参数名称
            * @return 多边形输出数组,出错时抛出异常
            */
            virtual IoPolygonArray GetOutputPolygonArray(IN const char * const strParamName) = 0;

            /**
            * @brief 获取所有输出名称信息（CONTOURPOINTS、POINTSET、小写的image，这三种类型不支持）
            * @return 所有输出名称信息,出错时抛出异常
            */
            virtual IoNameInfoArray GetAllOutputNameInfo() = 0;

            /**
            * @brief 根据名称信息获取输出对象基类指针
            * @param stNameInfo [IN] 输出名称信息
            * @return 输出对象基类指针,出错时抛出异常
            */
            virtual VmIoBase* GetOutputObject(IN const IoNameInfo& stNameInfo) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the IMVSGroup tool.  */
        /// Group工具类
        class IMVSGroupTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSGroupTool() {}
            virtual ~IMVSGroupTool() {}

        public:
            /**
            * @brief 参数对象
            * @return 失败，返回NULL
            * @remarks 通过参数对象可设置获取模块参数
            */
            virtual IMVSGroupParams* GetParamObj() = 0;

            /**
            * @brief 查找结果
            * @return 未执行或执行失败，返回NULL
            */
            virtual IMVSGroupResults* GetResult() = 0;

            /**
            * @brief 获取Group内模块运行错误信息
            * @return 错误信息，数量上限16个
            */
            virtual ModuRunErrorInfoList* GetModuErrorInfoList(void) = 0;

            /**
            * @brief 保存Group至文件
            * @param strGroPath   [IN] 文件路径
            * @param strPassword  [IN] 文件密码
            * @return 无，出错时抛出异常
            * @remarks 不支持导出方案中Group相关的连接订阅信息
            * @deprecated 弃用,推荐(SaveAsGroup(const char * const strGroPath, const char * const strPassword)
            */
            virtual void ExportGroup(IN const char * const strGroPath, IN const char * const strPassword = NULL) = 0;

            /**
            * @brief 保存Group至文件
            * @param strGroPath   [IN] 文件路径
            * @param strPassword  [IN] 文件密码
            * @return 无，出错时抛出异常
            * @remarks 不支持导出方案中Group相关的连接订阅信息
            */
            virtual void SaveAsGroup(IN const char * const strGroPath, IN const char * const strPassword = NULL) = 0;

            /**
            * @brief 自执行一次
            * @param bIsRespondToUI [IN] 接口执行期间是否尝试响应界面操作，在UI线程中设置为true可避免阻塞界面，在非UI线程中设置为false可减小耗时波动
            * @return 无，出错时抛出异常
            * @remarks 自执行一次（只支持独立Group）
            */
            virtual void Run(IN bool bIsRespondToUI = true) = 0;

            /**
            * @brief 获取所有模块信息列表
            * @return 所有模块信息列表
            */
            virtual GroupModuInfoList * GetAllModuleList(void) = 0;

            /**
            * @brief 禁用Group
            */
            virtual void DisableGroup(void) = 0;

            /**
            * @brief 启用Group
            */
            virtual void EnableGroup(void) = 0;

            /**
             * @brief 获取当前模块ID(预留，内部使用)
             * @return 当前模块ID
             */
            virtual unsigned int GetCurrentModuId(void) = 0;

            /**
            * @brief 获取局部变量
            * @return 局部变量工具指针
            */
            virtual CVariable* GetLocalVariable() = 0;

        public:
            /**
            * @brief 根据名称获取模块对象
            * @param strKey  [IN]  模块显示名称
            * @return 模块对象指针
            */
            virtual void * operator[] (IN const char * const strKey) = 0;
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}

/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

    /*  Note: Load the independent Group.
        brief: EN: Password parameter is reserved. | CH: 密码参数预留，暂不支持 */
    /* CH: 弃用,推荐(LoadIndependentGroup(const char * const strGroPath, IN const char * const strPassword)) | EN: Obsolete,recommend(LoadIndependentGroup(const char * const strGroPath, IN const char * const strPassword)) */
    IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSGroup::IMVSGroupTool * __stdcall LoadGroup(IN const char * const strGroPath, IN const char * const strPassword = NULL);
    /*  Note: Load the independent Group.
    brief: EN: Password parameter is reserved. | CH: 密码参数预留，暂不支持
    remarks: CH: 该接口加载的Group对象无法配合控件使用，如需配合控件，建议使用独立Group控件接口CVmSingleModuleSetControlInterface::LoadIndependentGroup */
    IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSGroup::IMVSGroupTool * __stdcall LoadIndependentGroup(IN const char * const strGroPath, IN const char * const strPassword = NULL);
    /*  Note: Get the independent Group by control. */
    IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSGroup::IMVSGroupTool * __stdcall GetGroupByCtrl(IN void * pCtrlInfo);
    /*  Note: Destroy tool instance for group.  */
    IMVS_6000_PLATFORMSDK_API void __stdcall DestroyGroupInstance(IN VisionMasterSDK::IMVSGroup::IMVSGroupTool* pToolInstance);

#ifdef __cplusplus
}
#endif 

#endif    ///< _IVM_GROUP_H_
