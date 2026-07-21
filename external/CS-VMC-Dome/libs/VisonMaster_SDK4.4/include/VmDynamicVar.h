/***************************************************************************************************
* File：VmDynamicVar.h
* Note：Interface for the dynamic Var result.
*
* Version：4.2
* Date：2021/11/14 11:04:12
***************************************************************************************************/
#ifndef _VM_DYNAMICVAR_H_
#define _VM_DYNAMICVAR_H_

#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "IVmDynamicIoDefine.h"
#include "iMVS-6000SDK4ServerDefine.h"
#include "VmDynamicIo.h"

namespace VisionMasterSDK
{
    /*************************** Dynamic Var Struct Start ***************************/
    const static unsigned int MAX_VAR_STRING_LENGTH = 256;          //变量名称最大长度
    const static unsigned int MAX_VAR_TYPE_LENGTH = 16;          //变量名称最大长度
    const static unsigned int MAX_VAR_GUID_LENGTH = 36;

    const static string VAR_INT = "int";
    const static string VAR_FLOAT = "float";
    const static string VAR_STRING = "string";
    const static string VAR_BYTE = "byte";
    const static string VAR_IMAGE = "IMAGE";
    const static string VAR_ROIBOX = "ROIBOX";
    const static string VAR_ROIANNULUS = "ROIANNULUS";
    const static string VAR_ROIPOLYGON = "ROIPOLYGON";
    const static string VAR_POINT = "POINT";
    const static string VAR_LINE = "LINE";
    const static string VAR_FIXTURE = "FIXTURE";
    const static string VAR_Rect = "Rect";
    const static string VAR_ELLIPSE = "ELLIPSE";
    const static string VAR_POINTSET = "pointset";

    enum VAR_MODIFYACTION
    {
        VAR_NAME = 0,
        VAR_TYPE,
        VAR_REMARK,
        VAR_VALUE,
        VAR_LINKMODULE,
        VAR_LINKMODULEBYPARAM,
        VAR_COMMENABLE,
        VAR_INDEX,
        VAR_SETTOP,
        VAR_VALUE_CPLUS
    };

    //io类型 6种，基本固定
    enum VAR_IO_VALUE_TYPE
    {
        IO_VALUE_TYPE_INT = 0,           //int
        IO_VALUE_TYPE_FLOAT,             //float
        IO_VALUE_TYPE_STRING,            //string
        IO_VALUE_TYPE_BYTE,              //byte数组
        IO_VALUE_TYPE_IMAGE,             //图像数据
        IO_VALUE_TYPE_POINTSET,          //点集数据类型
        IO_VALUE_TYPE_UNKNOW
    };

    typedef struct VAR_ByteDataArray
    {
        char*           pData;
        unsigned int    nDataLen;
        unsigned int    nDataSize;
        char            strReserved[64];    // CH: 保留字段 | EN: Reserved
        unsigned int    nRelease;
        unsigned int    nReserved[3];        // CH: 保留字段 | EN: Reserved
    }VAR_ByteDataArray_;

    //子IO数据，需要根据类型来区分
    typedef struct IMVS_VAR_SUB_VALUE
    {
        char chszValueName[MAX_VAR_STRING_LENGTH];                   //子IO名称
        VAR_IO_VALUE_TYPE emValueType;                       //变量类型  string, int, float,二进制数据
        int nValueNum;
        int* pIntValue;
        float* pFloatValue;
        VAR_ByteDataArray* pByteValue;
        int nRelease;
        int nReserved[3];                                        //备注
    }IMVS_VAR_SUB_VALUE;

    typedef struct IMVS_MODIFY_VAR
    {
        VAR_MODIFYACTION enumAction;                        //执行动作
        char chszVarName[MAX_VAR_STRING_LENGTH];            //变量名称
        char chszSubVarName[MAX_VAR_STRING_LENGTH];         //变量子名称
        char chszChangeValue[MAX_VAR_STRING_LENGTH];        //新的数据
        unsigned int nValueNum;
        void*   pData;
    }IMVS_MODIFY_VAR_;

    typedef struct IMVS_VARIABLE_VALUE
    {
        unsigned int nValueNum;
        int* pIntValue;
        float* pFloatValue;
        VAR_ByteDataArray* pByteValue;
        IMVS_VAR_SUB_VALUE* pIoValue;
        int nReserved[4];           // CH: 保留字段 | EN: Reserved
    }IMVS_VARIABLE_VALUE_;

    typedef struct IMVS_VARIABLE
    {
        int nIndex;                                              //索引标识
        char chszGuid[MAX_VAR_GUID_LENGTH];                      //Guid唯一标识
        bool bCommEnable;                                        //通信使能初始化
        char chszValueName[MAX_VAR_STRING_LENGTH];               //变量名称
        char chszValueType[MAX_VAR_TYPE_LENGTH];                 //变量类型  string, int, float,IMAGE,Cicle,ROIBox,Rect等
        char chszValue[MAX_VAR_STRING_LENGTH];                   //变量值
        char chszRemark[MAX_VAR_STRING_LENGTH];                  //注释信息
        char chsGroupInfo[MAX_VAR_STRING_LENGTH];                //分组信息
        int nLinkCount;                                          //关联模块个数
        void* pLinkModuleList;                                   //关联模块具体信息
        bool bCombination;                                       //是否是组合类型
        IMVS_VARIABLE_VALUE stVarbleValue;
        int nReserved[4];                                        //备注
    }IMVS_VARIABLE_;

    /*************************** End Dynamic Var Struct Start ***************************/

    /*************************** Dynamic Var Class Start ***************************/
    /*  Note: Interface Classes of the result of the CVmDynamicVar.  */
    class CVmDynamicVar : public CVmDynamicIo
    {
    public:
        ///< Constructor and Destructor
        explicit CVmDynamicVar();
        virtual ~CVmDynamicVar();

    public:

        int Init(IN unsigned int nModuId);
        virtual void Deinit();

#pragma region 设置数据

        /** @fn    SetVarInt();
        *  @brief  设置Int数据
        *  @param  无
        *  @return 无
        */
        virtual void SetVarInt(const char * const strParamName, const int * const anIntVal, const int nValNum);
        /** @fn    SetVarFloat();
        *  @brief  设置Float数据
        *  @param  无
        *  @return 无
        */
        virtual void SetVarFloat(const char * const strParamName, const float * const anFloatVal, const int nValNum);
        /** @fn    SetVarString();
        *  @brief  设置String数据
        *  @param  无
        *  @return 无
        */
        virtual void SetVarString(const char * const strParamName, const InputStringData * const astStrData, const int nValNum);
        /** @fn    SetVarBytes();
        *  @brief  设置16进制数据
        *  @param  无
        *  @return 无
        */
        virtual void SetVarBytes(const char * const strParamName, const ByteArrayData * const pBytes);
        /** @fn    SetVarImage();
        *  @brief  设置图像数据
        *  @param  无
        *  @return 无
        */
        virtual void SetVarImage(const char * const strParamName, const IoImage * const pstImageData);
        /** @fn    SetVarRoiBox();
        *  @brief  设置矩形框数据
        *  @param  无
        *  @return 无
        */
        virtual void SetVarRoiBox(const char * const strParamName, const IoRectBoxArray * const pstRoiBox);
        /** @fn    SetVarRoiAnnulus();
        *  @brief  设置圆环ROI数据
        *  @param  无
        *  @return 无
        */
        virtual void SetVarRoiAnnulus(const char * const strParamName, const IoAnnulusArray * const pstRoiAnnulus);
        /** @fn    SetVarCircle();
        *  @brief  设置圆形数据
        *  @param  无
        *  @return 无
        */
        virtual void SetVarEllipse(const char * const strParamName, const IoEllipseArray * const pstEllipse);
        /** @fn    SetVarLine();
        *  @brief  设置线型数据
        *  @param  无
        *  @return 无
        */
        virtual void SetVarLine(const char * const strParamName, const IoLineArray * const pstLine);

        /** @fn    SetVarPointF();
        *  @brief  设置点型数据
        *  @param  无
        *  @return 无
        */
        virtual void SetVarPointF(const char * const strParamName, const IoPointArray * const pstPointF);

        /** @fn    SetVarFixture();
        *  @brief  设置位置修正数据
        *  @param  无
        *  @return 无
        */
        virtual void SetVarFixture(const char * const strParamName, const IoFixtureArray * const pstFixture);
        /** @fn    SetVarPointset();
        *  @brief  设置点集数据，只支持数组长度为1
        *  @param  无
        *  @return 无
        */
        virtual void SetVarPointset(const char * const strParamName, const MatchOutline * const pstMatchOutline);
        /** @fn    SetVarRectF();
        *  @brief  设置矩形数据
        *  @param  无
        *  @return 无
        */
        virtual void SetVarRectF(const char * const strParamName, const IoRectFArray * const pstRectF);

        /** @fn    SetVarPolygon();
        *  @brief  设置多边形数据
        *  @param  无
        *  @return 无
        */
        virtual void SetVarPolygon(const char * const strParamName, const IoPolygonArray * const pstPolygon);


#pragma endregion

#pragma region 获取数据

        /**
        * @brief 获取整型结果
        * @param 参数名称
        * @param 整型数组
        * @param 整型数值数量
        * @return 无，出错时抛出异常
        * @par 注解
        */
        virtual IntDataArray GetVarInt(IN const char * const strParamName);

        /**
        * @brief 获取浮点型结果
        * @param 参数名称
        * @param 浮点型数组
        * @param 浮点型数值数量
        * @return 无，出错时抛出异常
        * @par 注解
        */
        virtual FloatDataArray GetVarFloat(IN const char * const strParamName);

        /**
        * @brief 获取字符串型结果
        * @param 参数名称
        * @param 字符串型数组
        * @param 字符串型数值数量
        * @return 无，出错时抛出异常
        * @par 注解
        */
        virtual StringDataArray GetVarString(IN const char * const strParamName);

        /**
        * @brief 获取二进制数据型结果
        * @param 参数名称
        * @return 二进制数据
        * @par 注解
        */
        virtual ByteArrayData GetVarByteArray(IN const char * const strParamName);

        //图像结果（单个图像）
        virtual IoImage GetVarImage(IN const char * const strParamName);

        //获取点输出数组
        virtual IoPointArray GetVarPointArray(IN const char * const strParamName);

        //获取直线输出数组
        virtual IoLineArray GetVarLineArray(IN const char * const strParamName);

        //获取椭圆输出数组
        virtual IoEllipseArray GetVarEllipseArray(IN const char * const strParamName);

        //获取带角度矩形输出数组
        virtual IoRectBoxArray GetVarRectBoxArray(IN const char * const strParamName);

        //获取无角度矩形(浮点型)输出数组
        virtual IoRectFArray GetVarRectFArray(IN const char * const strParamName);

        //获取位置修正输出数组
        virtual IoFixtureArray GetVarFixtureArray(IN const char * const strParamName);

        //获取圆环输出数组
        virtual IoAnnulusArray GetVarAnnulusArray(IN const char * const strParamName);

        //获取类别信息输出数组
        //virtual IoClassInfoArray GetVarClassInfoArray(IN const char * const strParamName);

        //获取带类别信息的图(界面可根据类别信息将灰度图显示成彩图)
        //virtual IoPixelImage GetVarPixelImage(IN const char * const strParamName);

        //获取位姿数组 -- 暂不支持
        //virtual IoPostureArray GetVarIoPostureArray(IN const char * const strParamName);

        //获取多边形输出数组
        virtual IoPolygonArray GetVarPolygonArray(IN const char * const strParamName);

        //所有所有输出名称信息，只获取最外层输出，如Group添加IO时界面上展示的名称（组合里面的Filter名称不单独提供）
        //CONTOURPOINTS、POINTSET、小写的image,这三种类型不支持
        //virtual IoNameInfoArray GetAllVarNameInfo();

        //根据名称信息获取输出对象基类指针
        virtual VmIoBase* GetVarObject(IN const IoNameInfo& stNameInfo);
#pragma endregion

        //判断基本类型输出参数名是否存在(int、float、string、byte)
        int VarFilterNameExist(IN const char * const strParamName, IN const char * const strParamType);

    private:

        void DeleteVarCache(const std::string& strName);
        void AddVarCache(const std::string& strName, VmIoBase* pObj);
        void DeleteAllVarCache();
        VmIoBase* FindVarCache(const std::string& strName);
        void ClearIoNameArray();
        bool IsFilterNameInfo(IMVS_MODULE_IO_INFO& stIo);
        //设置变量数据
        void SetVarValue(IN const char * const chParamName, IN const void* pData, IN const int nDataLen, IN const char* chzVarType);
        //获取变量数据
        void GetVarValue(IN const char * const chParamName, OUT char* pData, IN int nDataLen);
        //校验名称
        void CheckName(IN const char* const chParamName, std::string& strName);
        //释放内存
        void ReleaseIOValue(IN IMVS_VAR_SUB_VALUE* pSunIOValue, IN const int nNum);
    private:
        IMVS_PF_MODULE_RESULT_INFO_LIST* m_pModuleResultCopy;
        IoNameInfoArray                  m_stVarNameArr;
        std::map<std::string, VmIoBase*> m_mapVarCache;//用户获取输出时的缓存，key:name
    };
    /*************************** Dynamic Var Class End ***************************/
}
#endif