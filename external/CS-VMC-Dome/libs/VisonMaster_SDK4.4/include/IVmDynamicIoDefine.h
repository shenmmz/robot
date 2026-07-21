/***************************************************************************************************
 * File：IVmDynamicIoDefine.h
 * Note：Interface for the dynamic io.
 *
 ***************************************************************************************************/
#ifndef _IVM_DYNAMIC_IODEFINE_H_
#define _IVM_DYNAMIC_IODEFINE_H_

#pragma warning(disable:4819)
//#include "IVmExport.h"
#include "VmModuleResultBase.h"
//#include "VmModuleParamBase.h"
namespace VisionMasterSDK
{
    //IO类型
    typedef enum VmIoType__ {
        IO_TYPE_NOTSUPPORT = 0,     // 不支持
        IO_TYPE_INT = 1,            // 整形
        IO_TYPE_FLOAT = 2,          // 浮点型
        IO_TYPE_STRING = 3,         // 字符串型
        IO_TYPE_IMAGE = 4,          // 图像
        IO_TYPE_POINTSET = 5,       // ** 目前不对外开放 **
        IO_TYPE_BYTE = 6,           // 二进制
        IO_TYPE_POINT_F = 7,        // 点
        IO_TYPE_POINT_I = 8,        // 点
        IO_TYPE_LINE = 9,           // 线
        IO_TYPE_CIRCLE = 10,        // 圆
        IO_TYPE_RECT_F = 11,        // 不带角度矩形
        IO_TYPE_RECT_I = 12,        // 不带角度矩形
        IO_TYPE_ROIBOX = 13,        // 带角度矩形
        IO_TYPE_FIXTURE = 14,       // 位置修正
        IO_TYPE_ANNULUS = 16,       // 圆环(ROIANNULUS)
        IO_TYPE_CONTOURPOINTS = 17, // ** 目前不对外开放 **
        IO_TYPE_CLASSINFO = 18,     // 类别信息
        IO_TYPE_PIXELIMAGE = 19,    // 带有类别信息的图像
        IO_TYPE_POSTURE = 20,       // 位姿
        IO_TYPE_POLYGON = 21,       // 多边形
		IO_TYPE_ELLIPSE = 22,       // 椭圆
        IO_TYPE_MAX                 // 暂不支持
    }VmIoType;

    //IO基类
    typedef struct VmIoBase__
    {
        VmIoType eType;//io类型
        virtual ~VmIoBase__(){}
        virtual void DeleteMemory(){}

    }VmIoBase;
    
    ///< 字符串
    typedef struct StringData_ 
    {
        char strValue[MAX_MODULE_RESVALUE_LENGTH]; // CH: 字符串数据 | EN: String
        char strReserved[32];                      // CH: 保留字段 | EN: Reserved
    } StringData;

    

    ///< 整型数组
    typedef struct IntDataArray_ : VmIoBase
    {
        int    nValueNum;               // CH: 整型数据数量 | EN: Number of int value
        int *  pIntVal;                 // CH: 整型数据 | EN: Int data
        int    nReserved[4];            // CH: 保留字段 | EN: Reserved

        IntDataArray_()
        {
            eType = IO_TYPE_INT;
            nValueNum = 0;
            pIntVal = NULL;
            memset(nReserved, 0, sizeof(nReserved));
        }

        virtual void DeleteMemory()
        {//不需要实现
        }
    } IntDataArray;

    ///< 浮点型数组
    typedef struct FloatDataArray_ : VmIoBase
    {
        int     nValueNum;               // CH: 浮点型数据数量 | EN: Number of int value
        float * pFloatVal;               // CH: 浮点型数据 | EN: Int data
        int     nReserved[4];            // CH: 保留字段 | EN: Reserved

        FloatDataArray_()
        {
            eType = IO_TYPE_FLOAT;
            nValueNum = 0;
            pFloatVal = NULL;
            memset(nReserved, 0, sizeof(nReserved));
        }

        virtual void DeleteMemory()
        {//不需要实现
        }
    } FloatDataArray;

    ///< 字符串型数组
    typedef struct StringDataArray_ : VmIoBase
    {
        int     nValueNum;               // CH: 字符串型数据数量 | EN: Number of int value
        StringData * astStringVal;       // CH: 字符串型数据 | EN: Int data
        int     nReserved[4];            // CH: 保留字段 | EN: Reserved

        StringDataArray_()
        {
            eType = IO_TYPE_STRING;
            nValueNum = 0;
            astStringVal = NULL;
            memset(nReserved, 0, sizeof(nReserved));
        }

        virtual void DeleteMemory()
        {
            if (NULL != astStringVal)
            {
                delete [] astStringVal;
                astStringVal = NULL;
            }
        }
    } StringDataArray;

    ///< 二进制数据
    typedef struct ByteArrayData_ : VmIoBase
    {
        size_t    nDataLen;               // CH: 数据长度 | EN: Data length
        char *    pData;                  // CH: 数据 | EN: Data
        int       nReserved[4];           // CH: 保留字段 | EN: Reserved

        ByteArrayData_()
        {
            eType = IO_TYPE_BYTE;
            nDataLen = 0;
            pData = NULL;
            memset(nReserved, 0, sizeof(nReserved));
        }

        virtual void DeleteMemory()
        {//不需要实现
        }
    } ByteArrayData;

    ///< 图像类型
    typedef struct IoImage__ : VmIoBase
    {
		int nDataEmpty;                // CH: 数据是否获取 0 没有数据  1 有数据 | EN: DataReceived 
        ImageBaseData stImage;           // CH: 
        int     nReserved[4];            // CH: 保留字段 | EN: Reserved

        IoImage__()
        {
			nDataEmpty = 0; //初始没有数据，有数据之后需置为1
            eType = IO_TYPE_IMAGE;
            memset(&stImage, 0, sizeof(stImage));
            memset(nReserved, 0, sizeof(nReserved));
        }

        virtual void DeleteMemory()
        {//不需要实现

        }
        
    } IoImage;

    //点结果数组
    typedef struct IoPointArray__ : VmIoBase
    {
        int    nValueNum;               // CH: 整型数据数量 | EN: Number of int value
        PointF *  pPointVal;                 // CH: 点指针 | EN:
        int    nReserved[4];            // CH: 保留字段 | EN: Reserved

        IoPointArray__()
        {
            eType = IO_TYPE_POINT_F;
            nValueNum = 0;
            pPointVal = NULL;
            memset(nReserved, 0, sizeof(nReserved));
        }

        virtual void DeleteMemory()
        {
            if (NULL != pPointVal)
            {
                delete[] pPointVal;
            }
        }
    }IoPointArray;

    //直线结果数组
    typedef struct IoLineArray__ : VmIoBase
    {
        int    nValueNum;               // CH: 整型数据数量 | EN: Number of int value
        Line_v2 *  pLineVal;                 // CH: 
        int    nReserved[4];            // CH: 保留字段 | EN: Reserved

        IoLineArray__()
        {
            eType = IO_TYPE_LINE;
            nValueNum = 0;
            pLineVal = NULL;
            memset(nReserved, 0, sizeof(nReserved));
        }

        virtual void DeleteMemory()
        {
            if (NULL != pLineVal)
            {
                delete[] pLineVal;
            }
        }
    }IoLineArray;

    //圆结果数组
    typedef struct IoCircleArray__ : VmIoBase
    {
        int    nValueNum;               // CH: 整型数据数量 | EN: Number of int value
        Circle *  pCircleVal;                 // CH: 
        int    nReserved[4];            // CH: 保留字段 | EN: Reserved

        IoCircleArray__()
        {
            eType = IO_TYPE_LINE;
            nValueNum = 0;
            pCircleVal = NULL;
            memset(nReserved, 0, sizeof(nReserved));
        }

        virtual void DeleteMemory()
        {
            if (NULL != pCircleVal)
            {
                delete[] pCircleVal;
            }
        }
    }IoCircleArray;

	//椭圆结果数组
	typedef struct IoEllipseArray__ : VmIoBase
	{
		int    nValueNum;               // CH: 整型数据数量 | EN: Number of int value
		Ellipse *  pEllipseVal;                 // CH: 
		int    nReserved[4];            // CH: 保留字段 | EN: Reserved

		IoEllipseArray__()
		{
			eType = IO_TYPE_ELLIPSE;
			nValueNum = 0;
			pEllipseVal = NULL;
			memset(nReserved, 0, sizeof(nReserved));
		}

		virtual void DeleteMemory()
		{
			if (NULL != pEllipseVal)
			{
				delete[] pEllipseVal;
			}
		}
	}IoEllipseArray;

    //带角度矩形结果数组
    typedef struct IoRectBoxArray__ : VmIoBase
    {
        int    nValueNum;               // CH: 整型数据数量 | EN: Number of int value
        RectBox *  pBoxVal;                 // CH: 带角度矩形 | EN:
        int    nReserved[4];            // CH: 保留字段 | EN: Reserved

        IoRectBoxArray__()
        {
            eType = IO_TYPE_ROIBOX;
            nValueNum = 0;
            pBoxVal = NULL;
            memset(nReserved, 0, sizeof(nReserved));
        }

        virtual void DeleteMemory()
        {
            if (NULL != pBoxVal)
            {
                delete[] pBoxVal;
            }
            pBoxVal = NULL;
        }
    }IoRectBoxArray;

    //无角度矩形结果数组
    typedef struct IoRectFArray__ : VmIoBase
    {
        int    nValueNum;               // CH: 整型数据数量 | EN: Number of int value
        RectF *  pRectVal;                 // CH: 
        int    nReserved[4];            // CH: 保留字段 | EN: Reserved

        IoRectFArray__()
        {
            eType = IO_TYPE_RECT_F;
            nValueNum = 0;
            pRectVal = NULL;
            memset(nReserved, 0, sizeof(nReserved));
        }

        virtual void DeleteMemory()
        {
            if (NULL != pRectVal)
            {
                delete[] pRectVal;
            }
        }
    }IoRectFArray;

    //位置修正结果数组
    typedef struct IoFixtureArray__ : VmIoBase
    {
        int    nValueNum;               // CH: 整型数据数量 | EN: Number of int value
        Fixture *  pFixtureVal;                 // CH: 
        int    nReserved[4];            // CH: 保留字段 | EN: Reserved

        IoFixtureArray__()
        {
            eType = IO_TYPE_FIXTURE;
            nValueNum = 0;
            pFixtureVal = NULL;
            memset(nReserved, 0, sizeof(nReserved));
        }

        virtual void DeleteMemory()
        {
            if (NULL != pFixtureVal)
            {
                delete[] pFixtureVal;
            }
        }
    }IoFixtureArray;


    //圆环结果数组
    typedef struct IoAnnulusArray__ : VmIoBase
    {
        int    nValueNum;               // CH: 整型数据数量 | EN: Number of int value
        Annulus *  pAnnulusVal;                 // CH: 
        int    nReserved[4];            // CH: 保留字段 | EN: Reserved

        IoAnnulusArray__()
        {
            eType = IO_TYPE_ANNULUS;
            nValueNum = 0;
            pAnnulusVal = NULL;
            memset(nReserved, 0, sizeof(nReserved));
        }

        virtual void DeleteMemory()
        {
            if (NULL != pAnnulusVal)
            {
                delete[] pAnnulusVal;
            }
        }
    }IoAnnulusArray;

    //类别信息结果数组
    typedef struct IoClassInfoArray__ : VmIoBase
    {
        int    nValueNum;               // CH: 整型数据数量 | EN: Number of int value
        ClassInfo *  pClassInfoVal;                 // CH: 
        int    nReserved[4];            // CH: 保留字段 | EN: Reserved

        IoClassInfoArray__()
        {
            eType = IO_TYPE_CLASSINFO;
            nValueNum = 0;
            pClassInfoVal = NULL;
            memset(nReserved, 0, sizeof(nReserved));
        }

        virtual void DeleteMemory()
        {
            if (NULL != pClassInfoVal)
            {
                delete[] pClassInfoVal;
            }
        }
    }IoClassInfoArray;


    //带类别信息图像数组
    typedef struct IoPixelImage__ : VmIoBase
    {
		int nDataEmpty;                // CH: 数据是否获取 0 没有数据  1 有数据 | EN: DataReceived 
        PixelImage  stImageWithClass;  // CH: 
        int    nReserved[4];            // CH: 保留字段 | EN: Reserved

        IoPixelImage__()
        {
			nDataEmpty = 0;    //初始没有数据，有数据之后需置为1
            eType = IO_TYPE_PIXELIMAGE;
            memset(&stImageWithClass, 0, sizeof(stImageWithClass));
			stImageWithClass.pInfo = NULL;
            memset(nReserved, 0, sizeof(nReserved));
        }

        virtual void DeleteMemory()
        {
			if (NULL != stImageWithClass.pInfo)
			{
				delete[] stImageWithClass.pInfo;
			}
        }
    }IoPixelImage;

    //位姿数组
    typedef struct IoPostureArray__ : VmIoBase
    {
        int    nValueNum;               // CH: 整型数据数量 | EN: Number of int value
        Posture *  pPosture;                 // CH: 
        int    nReserved[4];            // CH: 保留字段 | EN: Reserved

        IoPostureArray__()
        {
            eType = IO_TYPE_POSTURE;
            nValueNum = 0;
            pPosture = NULL;
            memset(nReserved, 0, sizeof(nReserved));
        }

        virtual void DeleteMemory()
        {
            if (NULL != pPosture)
            {
                delete[] pPosture;
            }
        }
    }IoPostureArray;

	//多边形数组
	typedef struct IoPolygonArray__ : VmIoBase
	{
		int    nValueNum;               // CH: 整型数据数量 | EN: Number of int value
		Polygon *  pPolygon;                 // CH: 
		int    nReserved[4];            // CH: 保留字段 | EN: Reserved

		IoPolygonArray__()
		{
            eType = IO_TYPE_POLYGON;
			nValueNum = 0;
			pPolygon = NULL;
			memset(nReserved, 0, sizeof(nReserved));
		}

		virtual void DeleteMemory()
		{
			if (NULL != pPolygon)
			{
				delete[] pPolygon;
			}
		}
	}IoPolygonArray;

    //IO名称和类型
    typedef struct IoNameInfo__
    {
        char strIoName[MAX_MODULE_PARAMNAME_LENGTH];  //IO名称
        VmIoType nType;                               //IO类型
        char reserve[8];

        IoNameInfo__()
        {
            memset(&strIoName, 0, sizeof(strIoName));
            nType = IO_TYPE_NOTSUPPORT;
            memset(reserve, 0, sizeof(reserve));
        }

    }IoNameInfo;

    //
    typedef struct IoNameInfoArr__
    {
        int nNameNum;    // CH: 数量 | EN: Number
        IoNameInfo*  pstIo;// CH: 
        int    nReserved[4];            // CH: 保留字段 | EN: Reserved

        IoNameInfoArr__()
        {
            nNameNum = 0;
            pstIo = NULL;
            memset(nReserved, 0, sizeof(nReserved));
        }

    }IoNameInfoArray;

}


#endif    ///< _IVM_GROUP_H_
