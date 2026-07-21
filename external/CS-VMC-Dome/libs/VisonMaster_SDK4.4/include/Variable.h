/***************************************************************************************************
* File：Variable.h
* Note：Header file for the Variable tool.
*
* Version：4.4
* Date: 2023-11-4 14:43:42
***************************************************************************************************/
#ifndef CVARIABLE_H_
#define CVARIABLE_H_

#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmDynamicVar.h"

#define    MAX_VAR_NUM    2048        ///变量最大数量
#define    COMM_NAME_MAX_LENGTH    256    
#define    COMM_TYPE_MAX_LENGTH    16    

namespace VisionMasterSDK
{
    namespace VariableModule
    {
	/// \addtogroup 变量类
	/// @{
        typedef struct VarInfo
        {
            /// @~chinese 通信初始化 @~english communicate initial
            bool bCommEnable;

            /// @~chinese 变量类型 string, int, float @~english variable type
            char strValueType[COMM_TYPE_MAX_LENGTH];

            /// @~chinese 变量长度 @~english variable length
            unsigned int  nDataLen;

            /// @~chinese 变量值 @~english variable value
            char *pDataValue;

            /// @~chinese 变量名称 @~english variable name
            char strValueName[COMM_NAME_MAX_LENGTH];

            /// @~chinese 变量注释信息 @~english variable remark info
            char strRemark[COMM_NAME_MAX_LENGTH];

            /// @~chinese 保留 @~english remark
            int nReserved[4];

        }VarInfo;

        /// @~chinese 变量模块信息列表结构 @~english variable information list structure
        typedef struct VarInfoList
        {
            /// @~chinese 个数 @~english  count
            int nCount;

            /// @~chinese variable list @~english list
            VarInfo stValue[MAX_VAR_NUM];

            /// @~chinese 保留 @~english remark
            int nReserved[4];

        }VarInfoList;

        class CVariable
        {
        public:
            /** @fn    CVariable();
            *  @brief  构造函数,初始化成员变量
            *  @param  无
            *  @return 无
            */
            explicit CVariable();

            /** @fn    CVariable();
            *  @brief  构造函数,初始化成员变量
            *  @param  nModuleId 模块ID
            *  @return 无
            */
            CVariable(IN unsigned int nModuleId);

            /** @fn    CVariable();
            *  @brief  构造函数,初始化成员变量
            *  @param  strModuName 模块名称
            *  @return 无
            */
            CVariable(IN const char * strModuName);

            /** @fn    ~CVariable();
            *  @brief  析构函数,销毁成员变量
            *  @param  无
            *  @return 无
            */
            virtual ~CVariable();

        public:
            /**
            * @brief 设置变量
			* @param  strParam [IN] 参数名称
            * @param  strVal [IN] char型指针,变量值
            */
            virtual void SetVariable(IN const char * const strParam, IN const char * const strVal);

            /**
            * @brief 获取变量
			* @param  strParam [IN] 参数名称
            * @param  strVal [OUT] 变量值
            */
            virtual void GetVariable(IN const char * const strParam, OUT char strVal[8192]);

            /**
            * @brief 获取变量的名称和值
            * @return 变量信息列表
            */
            virtual VarInfoList* GetAllVar(void);

            /**
            *  @brief  设置Int数据
            *  @param  strParamName [IN] 参数名称
            *  @param  anIntVal     [IN] int型指针数组
            *  @param  nValNum      [IN] 数组长度
            */
            virtual void SetVarInt(const char * const strParamName, const int * const anIntVal, const int nValNum);

            /**
            *  @brief  设置Float数据
            *  @param  strParamName [IN] 参数名称
            *  @param  anFloatVal   [IN] float型指针数组
            *  @param  nValNum      [IN] 数组长度
            */
            virtual void SetVarFloat(const char * const strParamName, const float * const anFloatVal, const int nValNum);

            /**
            *  @brief  设置String数据
            *  @param  strParamName [IN] 参数名称
            *  @param  astStrData   [IN] InputStringData结构体指针数组
            *  @param  nValNum      [IN] 数组长度
            */
            virtual void SetVarString(const char * const strParamName, const InputStringData * const astStrData, const int nValNum);

            /**
            *  @brief  设置动态二进制数据（仅当前方案有效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pBytes  [IN] 二进制数据结构体
            */
            virtual void SetVarByte(const char * const strParamName, const ByteArrayData * const pBytes);

            /**
            *  @brief  设置动态输入图像数据（仅当前方案有效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstImageData  [IN] IoImage结构体指针数组
            */
            virtual void SetVarImage(const char * const strParamName, const IoImage * const pstImageData);

            /**
            *  @brief  设置动态输入矩形框数据（仅当前方案有效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstBox        [IN] IoRectBoxArray结构体指针数组
            */
            virtual void SetVarBox(const char * const strParamName, const IoRectBoxArray * const pstBox);

            /**
            *  @brief  设置动态输入圆环ROI数据（仅当前方案有效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstAnnulus    [IN] IoAnnulusArray结构体指针数组
            */
            virtual void SetVarAnnulus(const char * const strParamName, const IoAnnulusArray * const pstAnnulus);

            /**
            *  @brief  设置动态输入椭圆形数据（仅当前方案有效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstCircle     [IN] IoCircleArray结构体指针数组
            */
            virtual void SetVarEllipse(const char * const strParamName, const IoEllipseArray * const pstEllipse);


            /**
            *  @brief  设置动态输入线型数据（仅当前方案有效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstLine       [IN] IoLineArray结构体指针数组
            */
            virtual void SetVarLine(const char * const strParamName, const IoLineArray * const pstLine);

            /**
            *  @brief  设置动态输入点型数据（仅当前方案有效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstPointF     [IN] IoPointArray结构体指针数组
            */
            virtual void SetVarPoint(const char * const strParamName, const IoPointArray * const pstPointF);

            /**
            *  @brief  设置动态输入位置修正数据（仅当前方案有效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstFixture    [IN] IoFixtureArray结构体指针数组
            */
            virtual void SetVarFixture(const char * const strParamName, const IoFixtureArray * const pstFixture);

            /**
            *  @brief  设置动态输入矩形数据（仅当前方案有效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstRectF      [IN] IoRectFArray结构体指针数组
            */
            virtual void SetVarRect(const char * const strParamName, const IoRectFArray * const pstRectF);

            /**
            *  @brief  设置动态输入多边形数据（仅当前方案有效）
            *  @param  strParamName  [IN] 参数名称
            *  @param  pstPolygon    [IN] IoPolygonArray结构体指针数组
            */
            virtual void SetVarPolygon(const char * const strParamName, const IoPolygonArray * const pstPolygon);

            /**
            * @brief 获取整型输出
            * @param strParamName  [IN]  参数名称
            * @return 整型数组,出错时抛出异常
            */
            virtual IntDataArray GetVarInt(IN const char * const strParamName);

            /**
            * @brief 获取浮点型输出
            * @param strParamName  [IN]  参数名称
            * @return 浮点型数组,出错时抛出异常
            */
            virtual FloatDataArray GetVarFloat(IN const char * const strParamName);

            /**
            * @brief 获取字符串型输出
            * @param strParamName  [IN]  参数名称
            * @return 字符串型数组,出错时抛出异常
            */
            virtual StringDataArray GetVarString(IN const char * const strParamName);

            /**
            * @brief 获取图像像素数据输出
            * @param strParamName  [IN]  参数名称
            * @return 图像像素数据,出错时抛出异常
            */
            virtual IoImage GetVarImage(IN const char * const strParamName);

            /**
            * @brief 获取二进制数据型输出
            * @param strParamName  [IN]  参数名称
            * @return 二进制数据,出错时抛出异常
            */
            virtual ByteArrayData GetVarByteArray(IN const char * const strParamName);

            /**
            * @brief 获取点输出数组
            * @param strParamName  [IN]  参数名称
            * @return 点输出数组,出错时抛出异常
            */
            virtual IoPointArray GetVarPointArray(IN const char * const strParamName);

            /**
            * @brief 获取直线输出数组
            * @param strParamName  [IN]  参数名称
            * @return 直线输出数组,出错时抛出异常
            */
            virtual IoLineArray GetVarLineArray(IN const char * const strParamName);

            /**
            * @brief 获取椭圆输出数组
            * @param strParamName  [IN]  参数名称
            * @return 椭圆输出数组,出错时抛出异常
            */
            virtual IoEllipseArray GetVarEllipseArray(IN const char * const strParamName);

            /**
            * @brief 获取带角度矩形输出数组
            * @param strParamName  [IN]  参数名称
            * @return 带角度矩形输出数组,出错时抛出异常
            */
            virtual IoRectBoxArray GetVarBoxArray(IN const char * const strParamName);

            /**
            * @brief 获取无角度矩形（浮点型）输出数组
            * @param strParamName  [IN]  参数名称
            * @return 无角度矩形(浮点型)输出数组,出错时抛出异常
            */
            virtual IoRectFArray GetVarRectArray(IN const char * const strParamName);

            /**
            * @brief 获取位置修正输出数组
            * @param strParamName  [IN]  参数名称
            * @return 位置修正输出数组,出错时抛出异常
            */
            virtual IoFixtureArray GetVarFixtureArray(IN const char * const strParamName);

            /**
            * @brief 获取圆环输出数组
            * @param strParamName  [IN]  参数名称
            * @return 圆环输出数组,出错时抛出异常
            */
            virtual IoAnnulusArray GetVarAnnulusArray(IN const char * const strParamName);

            /**
            * @brief 获取多边形输出数组
            * @param strParamName  [IN]  参数名称
            * @return 多边形输出数组,出错时抛出异常
            */
            virtual IoPolygonArray GetVarPolygonArray(IN const char * const strParamName);

        private:
            VarInfoList* m_pstVarList;
        protected:
            CVmDynamicVar m_oDynamicVar;
            unsigned int mnModuleId;
        };
	/// @}
    }
}

#endif    ///< CVARIABLE_H_
