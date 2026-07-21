/***************************************************************************************************
 *
 * 版权信息：版权所有 (c) 2020, 保留所有权利
 *
 * 文件名称： VmModuleParamBase.h
 * 摘    要: VM模块参数类声明
 *
 * 当前版本：4.4.0
 *
 * 日    期：2020-10-29 19:24:30
 * 备    注：新建
 ****************************************************************************************************/
#ifndef CVMMODULEPARAMBASE_H__ // 2020-10-29 19:24:30
#define CVMMODULEPARAMBASE_H__

#pragma warning(disable:4819)
#include <iostream>
#include <memory>
#include <string>
#include <algorithm>
#include <vector>
#include "VmObjectBase.h"
#include "IVmDynamicIoDefine.h"

using namespace std;

namespace VisionMasterSDK
{
/// \addtogroup 模块参数基类
/// @{
    const static unsigned int MAX_MODULE_INPUTNAME_LENGTH = 128;      ///< 模块输入名称最大长度
    const static unsigned int MAX_MODULE_INPUTVALUE_LENGTH = 4096;    ///< 模块输入值最大长度

    /// 字符串类型
    typedef struct InputStringData__
    {
        char strValue[MAX_MODULE_INPUTVALUE_LENGTH];   ///< 字符串数据
        char strReserved[32];                          ///<  保留字段
    } InputStringData;

    /// 图像像素格式
    typedef enum ImagePixelFormat_
    {
        IMAGE_PIXEL_FORMAT_NULL = 0,        ///< 未知
        IMAGE_PIXEL_FORMAT_MONO8 = 1,       ///< MONO8
        IMAGE_PIXEL_FORMAT_RGB24 = 2,       ///< RGB24 C3
        IMAGE_PIXEL_FORMAT_RGB24_P3 = 3,    ///< RGB24 P3
    }ImagePixelFormat;

    /// 图像信息名称
    typedef struct ImageName__
    {
        char DataName[MAX_MODULE_INPUTNAME_LENGTH];               ///< 数据参数名称
        char WidthName[MAX_MODULE_INPUTNAME_LENGTH];              ///< 宽度参数名称
        char HeightName[MAX_MODULE_INPUTNAME_LENGTH];             ///< 高度参数名称
        char PixelFormatName[MAX_MODULE_INPUTNAME_LENGTH];        ///< 像素格式参数名称

        ImageName__()
        {
            _snprintf_s(DataName, MAX_MODULE_INPUTNAME_LENGTH, (MAX_MODULE_INPUTNAME_LENGTH - 1), "%s", "InImage");
            _snprintf_s(WidthName, MAX_MODULE_INPUTNAME_LENGTH, (MAX_MODULE_INPUTNAME_LENGTH - 1), "%s", "InImageWidth");
            _snprintf_s(HeightName, MAX_MODULE_INPUTNAME_LENGTH, (MAX_MODULE_INPUTNAME_LENGTH - 1), "%s", "InImageHeight");
            _snprintf_s(PixelFormatName, MAX_MODULE_INPUTNAME_LENGTH, (MAX_MODULE_INPUTNAME_LENGTH - 1), "%s", "InImagePixelFormat");
        }
    } ImageName;

    /// 图像数据
    typedef struct ImageData__
    {
        void * Data;                        ///< 图像数据
        unsigned int DataLen;               ///< 图像数据长度
        int Width;                          ///< 图像宽度 
        int Height;                         ///< 图像高度
        ImagePixelFormat Pixelformat;       ///< 图像像素格式
        ImageName Names;                    ///< 图像信息名称

        ImageData__()
        {
            Data = NULL;
            DataLen = 0;
            Width = 0;
            Height = 0;
            Pixelformat = IMAGE_PIXEL_FORMAT_NULL;
        }

        ImageData__(ImageBaseData stImageData)
        {
            InitImageData(stImageData);
        }

        ImageData__(ImageBaseData stImageData, ImageName stImageName)
        {
            InitImageData(stImageData);
            Names = stImageName;
        }

    private:
        void InitImageData(ImageBaseData stImageData)
        {
            this->Data = stImageData.ImageData;
            this->DataLen = stImageData.DataLen;
            this->Width = stImageData.Width;
            this->Height = stImageData.Height;
            this->Pixelformat = IMAGE_PIXEL_FORMAT_MONO8;
            if (MVD_PIXEL_MONO_08 == stImageData.Pixelformat)
            {
                this->Pixelformat = IMAGE_PIXEL_FORMAT_MONO8;
            }
            else if (MVD_PIXEL_RGB_RGB24_C3 == stImageData.Pixelformat)
            {
                this->Pixelformat = IMAGE_PIXEL_FORMAT_RGB24;
            }
            else if (MVD_PIXEL_RGB_RGB24_P3 == stImageData.Pixelformat)
            {
                this->Pixelformat = IMAGE_PIXEL_FORMAT_RGB24_P3;
            }
        }
    } ImageData;

    /// 二进制数据
    typedef struct BytesData_
    {
        size_t    nDataLen;               ///< 数据长度
        char *    pData;                  ///< 数据
        int       nReserved[4];           ///< 保留字段
    } BytesData;

    /// ROI形状类型
    typedef enum _ROI_TYPE
    {
        ROI_TYPE_IMAGE = 1,             ///< 全图ROI
        ROI_TYPE_BOX = 2,               ///< 矩形ROI
        ROI_TYPE_ANNULUS = 3,           ///< 圆环ROI
        ROI_TYPE_POLYGON = 4,           ///< 多边形ROI
        ROI_TYPE_LINECALIPER = 5,       ///< 直线卡尺ROI
        ROI_TYPE_CIRCLECALIPER = 6,     ///< 圆形卡尺ROI
        ROI_TYPE_SECTORCALIPER = 10,    ///< 圆弧卡尺ROI
    } ROI_TYPE;

#pragma pack(push,1)
    /// 点ROI（归一化数据，即把数据映射到目标区域范围之内处理）
    typedef struct _ROI_POINT
    {
        float fX; ///< X坐标
        float fY; ///< Y坐标
    } ROI_POINT;

    /// 矩形ROI(归一化数据)
    typedef struct _ROI_BOX
    {
        unsigned char       bRoiType; ///< ROI类型
        unsigned char       bVersion; ///< 版本信息
        float               fCenterX; ///< 中心点X坐标
        float               fCenterY; ///< 中心点Y坐标
        float               fWidth;   ///< 宽度
        float               fHeight;  ///< 高度
        float               fAngle;   ///< 角度
    } ROI_BOX;

    /// 圆环ROI（归一化数据，即把数据映射到目标区域范围之内处理）
    typedef struct _ROI_ANNULUS
    {
        unsigned char bRoiType; ///< ROI类型
        unsigned char bVersion; ///< 版本信息
        ROI_POINT stCenterPoint;///< 中心点
        float   fInnerRadius;   ///< 内径
        float   fOuterRadius;   ///< 外径
        float   fStartAngle;    ///< 起始角度
        float   fEndAngle;      ///< 终止角度
    }ROI_ANNULUS;

    ///多边形ROI（归一化数据，即把数据映射到目标区域范围之内处理）,获取或设置ROI时 根据实际点数传递数据长度
    typedef struct _ROI_POLYGON
    {
        unsigned char  bRoiType;             ///< ROI类型
        unsigned char  bVersion;             ///< 版本信息
        unsigned int   nVertexNum;           ///< 点数量
        ROI_POINT      stVertexPoints[32];   ///< 点信息
    } ROI_POLYGON;

    /// 直线卡尺ROI（归一化数据，即把数据映射到目标区域范围之内处理）
    typedef struct _ROI_LINECALIPER
    {
        unsigned char bRoiType;   ///< ROI类型
        unsigned char bVersion;   ///< 版本信息
        float      fStartX;       ///< 直线起点X坐标
        float      fStartY;       ///< 直线起点Y坐标
        float      fEndX;         ///< 直线终点X坐标
        float      fEndY;         ///< 直线终点Y坐标
        float      fWidth;        ///< 宽度
        float      fHeight;       ///< 高度
        float      fAngle;        ///< 角度
        int        nNum;          ///< 数量
    } ROI_LINECALIPER;

    /// 圆形卡尺ROI（归一化数据，即把数据映射到目标区域范围之内处理）
    typedef struct _ROI_CIRCLECALIPER
    {
        unsigned char bRoiType;        ///< ROI类型
        unsigned char bVersion;        ///< 版本信息
        float      fCenterX;           ///< 中心点X坐标
        float      fCenterY;           ///< 中心点Y坐标
        float      fOutterRadius;      ///< 外径
        float      fStartAngle;        ///< 起始角度
        float      fEndAngle;          ///< 终止角度
        float      fWidth;             ///< 宽度
        float      fHeight;            ///< 高度
        int        nNum;               ///< 数量
    } ROI_CIRCLECALIPER;

    /// 圆弧卡尺ROI（归一化数据，即把数据映射到目标区域范围之内处理）
    typedef struct _ROI_SECTORCALIPER
    {
        unsigned char       bRoiType; ///< ROI类型
        unsigned char       bVersion; ///< 版本信息
        float      fStartX;           ///< 起点X坐标
        float      fStartY;           ///< 起点Y坐标
        float      fEndX;             ///< 终点X坐标
        float      fEndY;             ///< 终点Y坐标
        float      fRadius;           ///< 半径
        float      fWidth;            ///< 宽度
        float      fHeight;           ///< 高度
        int        nNum;              ///< 数量
        int        nLargeArc;         ///< 圆弧大小
        int        nClockWise;        ///< 方向
    } ROI_SECTORCALIPER;
#pragma pack(pop)

	///模块参数基类
	class IMVS_6000_PLATFORMSDK_API CModuParamsBase 
    {
    public:
        /**
         *  @brief  构造函数,初始化成员变量
         */
        explicit CModuParamsBase();

        /** 
        *  @brief  构造函数,初始化成员变量
        */
        explicit CModuParamsBase(unsigned int moduleID);

        /** 
         *  @brief  析构函数,销毁成员变量
         */
        virtual ~CModuParamsBase();

        /**
        * @brief 获取参数值
        * @param strName       [IN]  参数名
        * @param nStrValueSize [IN]  空间大小
        * @param strValue      [OUT] 参数值
        * @return 成功，返回IMVS_EC_OK；失败，返回\ref SDK错误码
        */
        virtual int GetParamValue(const char * const strName, const unsigned int  nStrValueSize, char * const strValue);

        /**
        * @brief 设置参数值
        * @param strName  [IN] 参数名
        * @param strValue [IN] 参数值
        * @return 成功，返回IMVS_EC_OK；失败，返回\ref SDK错误码
        */
        virtual int SetParamValue(const char * const strName, const char * const strValue);

        /**
        *  @brief  获取二进制数据
        *  @param  strName  [IN] 参数名
        *  @param  pBinData [IN] 获取数据写出内存
        *  @param  nMemSize [IN] 写出内存大小（数值0时获取数据实际长度）
        *  @param  nDataLen [IN] 获取数据的实际长度（大于内存大小时返回长度和错误码）
        *  @return 成功，返回IMVS_EC_OK；失败，返回\ref SDK错误码
        */
        virtual int GetBinaryData(const char * const strName, void * const pBinData, const unsigned int nMemSize, unsigned int & nDataLen);

        /**
        *  @brief  设置二进制数据
        *  @param  strName  [IN] 设置数据的名称
        *  @param  pBinData [IN] 设置数据写入内存
        *  @param  nDataLen [IN] 设置数据的实际长度
        *  @return 成功，返回IMVS_EC_OK；失败，返回\ref SDK错误码
        */
        virtual int SetBinaryData(const char * const strName, const void * const pBinData, const unsigned int nDataLen);

    public:
        /**
        * @brief 设置Int数据
        * @param strParamName [IN] 参数名
        * @param anIntVal     [IN] 整型参数
        * @param nValNum      [IN] 整型参数数量
        */
        virtual void SetInputInt(const char * const strParamName, const int * const anIntVal, const int nValNum);
        /**
        * @brief 设置Float数据
        * @param strParamName [IN] 参数名
        * @param anFloatVal   [IN] 浮点型参数
        * @param nValNum      [IN] 浮点型参数数量
        */
        virtual void SetInputFloat(const char * const strParamName, const float * const anFloatVal, const int nValNum);
        /**
        * @brief 设置String数据
        * @param strParamName [IN] 参数名
        * @param astStrData   [IN] 字符串型参数
        * @param nValNum      [IN] 字符串型参数数量
        */
        virtual void SetInputString(const char * const strParamName, const InputStringData * const astStrData, const int nValNum);
        /**
        * @brief 设置二进制数据
        * @param strParamName [IN] 参数名
        * @param pBytes       [IN] 二进制型数据
        */
        virtual void SetInputBytes(const char * const strParamName, const BytesData * const pBytes);
        /**
        * @brief 设置图像数据
        * @param pstImageData [IN] 图像数据
        */
        virtual void SetInputImage(const ImageData * const pstImageData);

        unsigned int ModuleID; /// 当前模块ID

    public:
        virtual void * GetControlInfo(void);
    };
/// @}

    /** @class ParamItem
    *  @brief 模块参数项模板
    */
    class IMVS_6000_PLATFORMSDK_API ParamItem
    {
    public:
        //// 所属模块参数
        //CModuParamsBase* RefModuParams;

        // 当前模块ID
        unsigned int ModuleID;

        // 参数名称
        string Name;

        // 参数数值
        __declspec(property(put = SetParamValue, get = GetParamValue)) string Value;
        string GetParamValue();
        void SetParamValue(string value);

    public:
        // 参数最大值
        double MaxValue;

        // 参数最小值
        double MinValue;

        //构造函数
        ParamItem();

        //初始化
        void Init(unsigned int moduleID, string paramName, double maxValue = 0, double minValue = 0);

        // 获取bool类型参数
        virtual bool GetBoolValue();

        // 设置bool类型参数
        virtual void SetBoolValue(bool value);

        // 获取int类型参数
        virtual int GetIntValue();

        // 设置int类型参数
        virtual void SetIntValue(int value);

        // 获取double类型参数
        virtual double GetDoubleValue();

        // 设置double类型参数
        virtual void SetDoubleValue(double value);

        // 获取string类型参数
        virtual char* GetStringValue(char* temp);

        // 设置string类型参数
        virtual void SetStringValue(const char* value);

        // 获取enum类型参数
        virtual int GetEnumValue();

        // 设置enum类型参数
        virtual void SetEnumValue(int value);
    };


    /** @class InputItem
    *  @brief 模块输入项模板
    */
	class IMVS_6000_PLATFORMSDK_API InputItem
    {
    public:
        // 输入类型
        typedef enum _INPUT_TYPE
        {
            INPUT_TYPE_NULL = 0,
            INPUT_TYPE_INT = 1,
            INPUT_TYPE_FLOAT = 2,
            INPUT_TYPE_STRING = 3,
            INPUT_TYPE_POINT = 4,
            INPUT_TYPE_LINE = 5,
            INPUT_TYPE_BOX = 6,
            INPUT_TYPE_ANNULUS = 7,
            INPUT_TYPE_CIRCLE = 8,
            INPUT_TYPE_FIXTURE = 9,
            INPUT_TYPE_IMAGE = 10,
        } INPUT_TYPE;

    private:
        // 所属模块参数
        CModuParamsBase* RefModuParams;

        //// 当前模块ID
        //unsigned int ModuleID;

        // 输入名称
        vector<string> Names;

        // 输入类型
        INPUT_TYPE Type;

    public:
        //构造函数
        InputItem();

        //初始化
        void Init(CModuParamsBase* moduParams, INPUT_TYPE inputType, string* inputNames, unsigned int namesNum);

        // 设置输入
        void SetInputValue(int* pInputValue, unsigned int nValueNum);

        // 设置输入
        void SetInputValue(float* pInputValue, unsigned int nValueNum);

        // 设置输入
        void SetInputValue(InputStringData* pInputValue, unsigned int nValueNum);

        // 设置输入
        void SetInputValue(PointF* pInputValue, unsigned int nValueNum);

        // 设置输入
        void SetInputValue(Line* pInputValue, unsigned int nValueNum);

        // 设置输入
        void SetInputValue(RectBox* pInputValue, unsigned int nValueNum);

        // 设置输入
        void SetInputValue(Annulus* pInputValue, unsigned int nValueNum);

        // 设置输入
        void SetInputValue(Circle* pInputValue, unsigned int nValueNum);

        // 设置输入
        void SetInputValue(Fixture* pInputValue);

        // 设置输入
        void SetInputValue(ImageBaseData* pInputValue);
    };

    /** @class RoiManager
    *  @brief ROI管理类
    */
	class IMVS_6000_PLATFORMSDK_API RoiManager
    {
    public:
        // 所属模块参数
        CModuParamsBase* RefModuParams;

        //// 当前模块ID
        //unsigned int ModuleID;

        // ROI名称
        string RoiName;

        // 支持的ROI类型
        vector<ROI_TYPE> RoiTypeAllowed;

    public:
        //构造函数
        RoiManager();

        //初始化
        void Init(CModuParamsBase* moduParams, string roiName, string roiType);

        // 设置绘制ROI全图
        void SetDrawRoi();

        // 设置绘制ROI
        void SetDrawRoi(RectBox stRoiValue, unsigned int nImgWidth, unsigned int nImgHeight);

        // 设置绘制ROI
        void SetDrawRoi(Annulus stRoiValue, unsigned int nImgWidth, unsigned int nImgHeight);

        // 设置绘制ROI
        void SetDrawRoi(Polygon& stRoiValue, unsigned int nImgWidth, unsigned int nImgHeight);

        // 设置继承ROI
        void SetInheritRoi(RectBox stRoiValue);

        // 设置继承ROI
        void SetInheritRoi(Annulus stRoiValue);

        // 设置继承屏蔽区
        void SetInheritBlind(StringData* pBlindValue, unsigned int nValueNum);

        // 设置掩膜图像
        void SetMaskImage(ImageBaseData stMaskImage);
    };

    typedef std::shared_ptr<CModuParamsBase> CModuleParamsBasePtr;
}

#endif // CVMOBJECTBASE_H__