/***************************************************************************************************
 * File：VmDynamicInput.h
 * Note：Interface for the dynamic output result.
 *
 * Version：4.2
 * Date：2021/11/14 11:04:12
 ***************************************************************************************************/
#ifndef _VM_DYNAMIC_IO_BASE_H_
#define _VM_DYNAMIC_IO_BASE_H_

#include "IVmDynamicIoDefine.h"
#include "iMVS-6000SDK4ServerDefine.h"

namespace VisionMasterSDK
{
    typedef struct _PointIoName {
        std::string   strX;
        std::string   strY;
    } PointIoName;

    typedef struct RectBoxIoName__
    {
        PointIoName CenterPoint;
        std::string BoxWidth;
        std::string BoxHeight;
        std::string Angle;
    } RectBoxIoName;

    // 圆环信息结构
    typedef struct AnnulusIoName__
    {
        PointIoName CenterPoint;
        std::string InnerRadius;
        std::string OuterRadius;
        std::string StartAngle;
        std::string AngleExtend;
    } AnnulusIoName;


    // 位姿结构
    typedef struct _PostureIoName
    {
        PointIoName stPoint;
        std::string  R;
    } PostureIoName;

    // Circle结构体
    typedef struct CircleIoName__
    {
        std::string strRadius;
        PointIoName stCenterPoint;
    } CircleIoName;

	// Ellipse结构体
	typedef struct EllipseIoName__
	{
		PointIoName stCenterPoint;
		std::string MajorRadius;
		std::string MinorRadius;
		std::string Angle;
	} EllipseIoName;

    // Rect结构体（不带角度）
    typedef struct RectFIoName__
    {
        PointIoName stCenterPoint;
        std::string strBoxWidth;
        std::string strBoxHeight;
    } RectFIoName;

    // Line结构体（不带角度）
    typedef struct LineIoName__
    {
        PointIoName stStartPoint;
        PointIoName stEndPoint;
    } LineIoName;

    // 位置修正结构体
    typedef struct FixtureIoName__
    {
        PointIoName stFixturedPoint;
        std::string strInitAngle;
        std::string strInitScaleX;
        std::string strInitScaleY;
        PointIoName stUnfixturedPoint;
        std::string strRunAngle;
        std::string strRunScaleX;
        std::string strRunScaleY;
    } FixtureIoName;

    // 图像基本信息结构
    typedef struct ImageBaseDataIoName__
    {
        std::string ImageData;
        std::string DataLen;
        std::string Width;
        std::string Height;
        std::string Pixelformat;
    } ImageBaseDataIoName;

    // 类别信息数据
    typedef struct ClassInfoIoName__
    {
        std::string ClassName;
        std::string GrayValue;
    } ClassInfoIoName;

    typedef struct  PixelImageIoName__
    {
        ImageBaseDataIoName ImageName;
        ClassInfoIoName  Info;
    } PixelImageIoName;

    // 多边形数据
    typedef struct  PolygonName__
    {
        std::string strVertexNumName;
        PointIoName  stVertexPoints;
    } PolygonIoName;

    /*************************** Dynamic Output Class Start ***************************/
    /*  Note: Interface Classes of the result of the CVmDynamicOutput.  */
    class CVmDynamicIo
    {
public:
        ///< Constructor and Destructor
    explicit CVmDynamicIo();
    virtual ~CVmDynamicIo();

    public:

        int InitIo(IN unsigned int nModuId, IN int nIoType);
        virtual void DeinitIo();

        std::string DeleteSpecifyChar(const char * const strInput, const char * const strFlag);
        std::string AddSpecifyChar(const std::string strInput);
        
    public:
        void DeleteDynamicIO();
        virtual int ConvertParamType(const std::string& strType, VmIoType& eType);
        

        virtual int GetCombinationOut(IN const char * const strParamName, IMVS_MODULE_IO_INFO& stOutput);
        virtual int GetCombinationOutRecursion(IN const char * const strParamName, const IMVS_MODULE_IO_INFO* pstTmp, IMVS_MODULE_IO_INFO& stOutput);
        virtual int GetPointFIoName(IN const IMVS_MODULE_IO_INFO& stIoInfo, PointIoName& stPointName);
        virtual int GetRectBoxIoName(IN const IMVS_MODULE_IO_INFO& stIoInfo, RectBoxIoName& stBoxName);
        virtual int GetAnnulusIoName(IN const IMVS_MODULE_IO_INFO& stIoInfo, AnnulusIoName& stAnnulusName);
        virtual int GetClassInfoIoName(IN const IMVS_MODULE_IO_INFO& stIoInfo, ClassInfoIoName& stClassInfoName);
        virtual int GetPostureIoName(IN const IMVS_MODULE_IO_INFO& stIoInfo, PostureIoName& stPostureName);
        virtual int GetCircleIoName(IN const IMVS_MODULE_IO_INFO& stIoInfo, CircleIoName& stCircleName);
		virtual int GetEllipseIoName(IN const IMVS_MODULE_IO_INFO& stIoInfo, EllipseIoName& stCircleName);
        virtual int GetRectFIoName(IN const IMVS_MODULE_IO_INFO& stIoInfo, RectFIoName& stRectName);
        virtual int GetLineIoName(IN const IMVS_MODULE_IO_INFO& stIoInfo, LineIoName& stLineName);
        virtual int GetFixtureIoName(IN const IMVS_MODULE_IO_INFO& stIoInfo, FixtureIoName& stFixtureName);
        virtual int GetImageV2IoName(IN const IMVS_MODULE_IO_INFO& stIoInfo, ImageBaseDataIoName& stImageName);
        virtual int GetPixelImageIoName(IN const IMVS_MODULE_IO_INFO& stIoInfo, PixelImageIoName& stPixelImageName);
        virtual int GetPolygonName(IN const IMVS_MODULE_IO_INFO& stIoInfo, PolygonIoName& stPolygonName);
        
   protected:
        IMVS_MODULE_IO_INFO_LIST*        m_pModuDynamicIoName;
        unsigned int                             m_nToolId;
    };
    /*************************** Dynamic Output Class End ***************************/
}


#endif    ///< _VM_DYNAMIC_IO_BASE_H_
