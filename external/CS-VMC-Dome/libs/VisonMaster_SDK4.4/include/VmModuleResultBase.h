/***************************************************************************************************
 *
 * 版权信息：版权所有 (c) 2020, 保留所有权利
 *
 * 文件名称： VmModuleResultBase.h
 * 摘    要: VM模块结果基础类声明
 *
 * 当前版本：4.4.0
 *
 * 日    期：2020-10-29 19:24:30
 * 备    注：新建
 ****************************************************************************************************/
#ifndef CVMMODULERESULTBASE_H__ // 2020-10-29 19:24:30
#define CVMMODULERESULTBASE_H__

#pragma warning(disable:4819)
#include <iostream>
#include <memory>
#include "iMVS-6000PlatformSDKDefine.h"

namespace VisionMasterSDK
{
/// \addtogroup 模块结果基类
/// @{
    const static unsigned int MAX_MODULE_PARAMNAME_LENGTH = 128;     ///< 模块结果名称最大长度
    const static unsigned int MAX_MODULE_RESVALUE_LENGTH = 4096;     ///< 模块结果值最大长度
    const static unsigned int MAX_POLYGON_VERTEX_NUM = 32;           ///< 多边形最大点数

    /// 整型点信息结构
    typedef struct _PointI {
        int   X;       ///< X
        int   Y;       ///< Y
    } PointI;

    /// 浮点型点信息结构
    typedef struct _PointF {
        float   X;        ///< X
        float   Y;        ///< Y
    } PointF;

    /// 位姿结构
    typedef struct _Posture {
        PointF stPoint;     ///< 点
        float  R;           ///< 角度
    } Posture;

    /// 图像像素格式
    typedef enum _MvdPixelFormat_
    {
        MVD_PIXEL_NULL = 0,                                     ///< 未知格式
        MVD_PIXEL_MONO_08 = IMVS_PF_IMG_FORMAT_MONO8,           ///< MONO8
        MVD_PIXEL_RGB_RGB24_C3 = IMVS_PF_IMG_FORMAT_RGB24,      ///< RGB24 C3
        MVD_PIXEL_RGB_RGB24_P3 = IMVS_PF_IMG_FORMAT_RGB24_P3,   ///< RGB24 P3
    }MvdPixelFormat;

    /// 图像基本信息结构
    typedef struct ImageBaseData__
    {
        void * ImageData;             ///< 图像数据
        unsigned int DataLen;         ///< 图像数据长度
        int Width;                    ///< 图像宽度
        int Height;                   ///< 图像高度
        MvdPixelFormat Pixelformat;   ///< 图像像素格式
    } ImageBaseData;

    /// 矩形信息结构
    typedef struct RectI__
    {
        PointI RectPoint;       ///< 矩形中心点
        int RectWidth;          ///< 矩形宽度
        int RectHeight;         ///< 矩形高度
    } RectI;

    /// 矩形信息结构
    typedef struct RectF__
    {
        PointF RectPoint;    ///< 矩形框中心点
        float RectWidth;     ///< 矩形宽度
        float RectHeight;    ///< 矩形高度
    } RectF;

    /// 矩形框信息结构
    typedef struct RectBox__
    {
        PointF CenterPoint;       ///< 矩形框中心点
        float BoxWidth;           ///< 矩形宽度
        float BoxHeight;          ///< 矩形高度
        float Angle;              ///< 矩形角度
    } RectBox;

    /// 直线信息结构
    typedef struct Line__
    {
        PointF StartPoint;       ///< 直线起点
        PointF EndPoint;         ///< 直线终点
        float Angle;             ///< 直线角度
    } Line;

    /// 不带角度的直线信息结构
    typedef struct Line_v2__
    {
        PointF StartPoint;     ///< 直线起点
        PointF EndPoint;       ///< 直线终点
    } Line_v2;

    /// 圆信息结构
    typedef struct Circle__
    {
        PointF CenterPoint;       ///< 圆中心点
        float Radius;             ///< 圆半径
    } Circle;

    /// 圆环信息结构
    typedef struct Annulus__
    {
        PointF CenterPoint;      ///< 中心点
        float InnerRadius;       ///< 圆弧内径
        float OuterRadius;       ///< 圆弧外径
        float StartAngle;        ///< 起始角度
        float AngleExtend;       ///< 检测范围
    } Annulus;

    /// 多边形信息结构
    typedef struct Polygon__
    {
        unsigned int VertexNum;                        ///< 多边形点数
        PointF VertexPoints[MAX_POLYGON_VERTEX_NUM];   ///< 多边形点集
    } Polygon;

    /// 缺陷修正信息结构
    typedef struct DefectFixture__
    {
        float MinWidth;            ///< 最小宽度
        float MaxWidth;            ///< 最大宽度
        float AverageWidth;        ///< 平均宽度
        float ApproximateArea;     ///< 近似面积
    } DefectFixture;

	/// 匹配点
    typedef struct MatchPoint__
    {
        float MatchOutlineX;          ///< @~chinese 匹配轮廓点X @~english Matching contour point X
        float MatchOutlineY;          ///< @~chinese 匹配轮廓点Y @~english Matching contour point Y
        float MatchOutlineScore;      ///< @~chinese 匹配轮廓点分数 @~english Matching contour point score
        int MatchOutlineIndex;        ///< @~chinese 匹配轮廓点序号 @~english Matching contour point No.
    } MatchPoint;

    /// 类别信息数据
    typedef struct ClassInfo__
    {
        char Name[MAX_MODULE_PARAMNAME_LENGTH]; ///< 类型名
        int GrayValue;                          ///< 类型值
    } ClassInfo;

	/// 类别信息图像
    typedef struct  PixelImage__
    {
        ImageBaseData stIamge;    ///< 图像
        int nClassNum;            ///< 类别数量
        ClassInfo*  pInfo;        ///< 类别信息
    } PixelImage;

    ///修正数据
    typedef struct Fixture__
    {
        PointF stInitPoint;         ///< 基准点
        float  fInitAngle;          ///< 基准角度
        float  fInitScaleX;         ///< 基准尺度X
        float  fInitScaleY;         ///< 基准尺度Y
        PointF stRunPoint;          ///< 运行点
        float  fRunAngle;           ///< 运行角度
        float  fRunScaleX;          ///< 运行尺度X
        float  fRunScaleY;          ///< 运行尺度Y
    } Fixture;

    ///轮廓点数据
    typedef struct MatchOutline__
    {
        int MatchPointNum;                 ///< 匹配点数量
        MatchPoint * MatchOutlinePoints;   ///< 匹配点
    } MatchOutline;

    /// 二进制数据
    typedef struct BaseDataInfo__
    {
        char            * pData;           ///< @~chinese 数据 @~english Image data
        size_t            nLen;            ///< @~chinese 数据长度 @~english Image data len
        unsigned int      nReserved[4];    ///< @~chinese 保留字段 @~english Reserved
    } BaseDataInfo;

    /// 字符串结果信息结构
    typedef struct StringValueInfo__
    {
        char            strValue[MAX_MODULE_RESVALUE_LENGTH];    ///< @~chinese 字符串数据 @~english String data
        unsigned int    nReserved[4];                            ///< @~chinese 保留字段 @~english Reserved
    } StringValueInfo;

    /// 输出模块结果信息结构
    typedef struct OutputResultInfo__
    {
        char            strParamName[MAX_MODULE_PARAMNAME_LENGTH];    ///< @~chinese 结果名称 @~english Result name
        int             nParamType;                                   ///< @~chinese 结果类型：1整型 2浮点型 3字符串型 @~english Result type: 1-integer, 2-float, 3-string
        int             nValueNum;                                    ///< @~chinese 模块值个数 @~english Number of module values
        int *           pIntValue;                                    ///< @~chinese 整型数据 @~english Integer data
        float *         pFloatValue;                                  ///< @~chinese 浮点型数据 @~english Float data
        StringValueInfo * pstStringValue;                             ///< @~chinese 字符串型数据 @~english String data
    } OutputResultInfo;

    /// 脚本模块输出模块结果信息结构
    typedef struct ShellOutputResultInfo__
    {
        char            strParamName[MAX_MODULE_PARAMNAME_LENGTH];    ///< @~chinese 结果名称 @~english Result name
        int             nParamType;                                   ///< @~chinese 结果类型：1整型 2浮点型 3字符串型 @~english Result type: 1-integer, 2-float, 3-string
        int             nValueNum;                                    ///< @~chinese 模块值个数 @~english Number of module values
        int *           pIntValue;                                    ///< @~chinese 整型数据 @~english Integer data
        float *         pFloatValue;                                  ///< @~chinese 浮点型数据 @~english Float data
        StringValueInfo * pstStringValue;                             ///< @~chinese 字符串型数据 @~english String data
        BaseDataInfo    * pstImgValue;                                ///< @~chinese 图像型数据 @~english Image data
        BaseDataInfo    * pstByteValue;                               ///< @~chinese 二进制数据 @~english Byte data
    } ShellOutputResultInfo;

    /// 变量计算模块输出模块结果信息结构
    typedef struct CalOutputResultInfo__
    {
        char            strParamName[MAX_MODULE_PARAMNAME_LENGTH];    ///< @~chinese 结果名称 @~english Result name
        int             nParamType;                                   ///< @~chinese 结果类型：1整型 2浮点型 3字符串型 @~english Result type: 1-integer, 2-float, 3-string
        int             nValueNum;                                    ///< @~chinese 模块值个数 @~english Number of module values
        int *           pIntValue;                                    ///< @~chinese 整型数据 @~english Integer data
        float *         pFloatValue;                                  ///< @~chinese 浮点型数据 @~english Float data
    } CalOutputResultInfo;

    /// 整型数据
    typedef struct IntResultInfo__
    {
        int   nValueStatus;    ///< 单个输出参数的状态 1：成功，其他值：失败
        int   nValueNum;       ///< 模块值个数
        int * pIntValue;       ///< 整型数据
    }IntResultInfo;

    /// 浮点型数据
    typedef struct FloatResultInfo__
    {
        int   nValueStatus;    ///< 单个输出参数的状态 1：成功，其他值：失败
        int   nValueNum;       ///< 模块值个数
        float * pFloatValue;   ///< 浮点型数据
    }FloatResultInfo;

    /// 浮点型数据
    typedef struct StringResultInfo__
    {
        int   nValueStatus;                    ///< 单个输出参数的状态 1：成功，其他值：失败
        int   nValueNum;                       ///< 模块值个数
        StringValueInfo * pstStringValue;      ///< 字符串型数据
    }StringResultInfo;

    /// 图像像素数据
    typedef struct ImageResultInfo__
    {
        int   nValueStatus;             ///< 单个输出参数的状态 1：成功，其他值：失败
        int   nValueNum;                ///< 模块值个数
        BaseDataInfo * pstImageValue;   ///< 图像像素数据
    } ImageResultInfo;

    /// 点集型数据
    typedef struct PointsetResultInfo__
    {
        int   nValueStatus;                  ///< 单个输出参数的状态 1：成功，其他值：失败
        int   nValueNum;                     ///< 模块值个数
        BaseDataInfo * pstPointsetValue;     ///< 点集型数据
    } PointsetResultInfo;

	/// 椭圆信息结构
	typedef struct Ellipse__
	{
        PointF CenterPoint;	///< 圆心点
        float MajorRadius;	///< 长半轴
        float MinorRadius;	///< 短半轴
        float Angle;	    ///< 角度
	} Ellipse;

    /** @class OutputItem
    *  @brief 模块输出项模板
    */
    class IMVS_6000_PLATFORMSDK_API OutputItem
    {

    };

	///模块结果基类
	class IMVS_6000_PLATFORMSDK_API CModuleResultBase
    {
    public:

        /**
         *  @brief  构造函数,初始化成员变量
         */
        explicit CModuleResultBase();

        /**
         *  @brief  析构函数,销毁成员变量
         */
        virtual ~CModuleResultBase();

    public:
        /**
         *  @brief  用于在运行时获取对象的真实类名称（内部使用）
         *  @return 类名
         */
        virtual const char * GetThisClassName();

        /**
        *  @brief  错误码
        *  @return 错误码
        */
        virtual unsigned int GetErrorCode();

        /**
        *  @brief  模块运行时间
        *  @return 模块运行时间
        */
        virtual float GetModuRunTime();

    protected:
        unsigned int m_nErrorCode;
        IMVS_PF_MODULE_RESULT_INFO_LIST m_stModuResInfoList;
    };
/// @}
    typedef std::shared_ptr<CModuleResultBase> CModuleResultBasePtr;
}

#endif // CVMOBJECTBASE_H__
