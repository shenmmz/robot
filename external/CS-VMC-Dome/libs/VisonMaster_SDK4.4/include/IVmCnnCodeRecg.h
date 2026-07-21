/***************************************************************************************************
 * File：IVmCnnCodeRecg.h
 * Note：@~chinese DL读码模块接口声明 @~english Interface for the CnnCodeRecg tool.
 *
 ***************************************************************************************************/
#ifndef _IVM_CNNCODERECG_H_
#define _IVM_CNNCODERECG_H_

#include "MVD_AlgoParamDefine.h"

using namespace VisionDesigner;

#pragma warning(disable:4819)
#include "IVmExport.h"
#include "VmModuleResultBase.h"
#include "VmModuleParamBase.h"
#include "VmToolBase.h"

namespace VisionMasterSDK
{
    namespace IMVSCnnCodeRecgModu
    {
        /// @~chinese 极性   @~english Polarity
        typedef enum _CodePolarityEnum
        {
            /// @~chinese 白底黑码   @~english Dark On White
            CodePolarity_dark = 0x0,

            /// @~chinese 黑底白码   @~english White On Dark
            CodePolarity_bright = 0x1,

            /// @~chinese 任意   @~english Any
            CodePolarity_both = 0x2,

        }CodePolarityEnum;

        /// @~chinese 边缘类型   @~english Edge Type
        typedef enum _DiscreteFlagEnum
        {
            /// @~chinese 连续型   @~english Continuous
            DiscreteFlag_continuous = 0x0,

            /// @~chinese 离散型   @~english Discrete
            DiscreteFlag_discrete = 0x1,

            /// @~chinese 兼容模式   @~english Compatibility
            DiscreteFlag_allmode = 0x2,

        }DiscreteFlagEnum;

        /// @~chinese 镜像模式   @~english Mirror Mode
        typedef enum _MirrorModeEnum
        {
            /// @~chinese 非镜像   @~english Non-Mirrored
            MirrorMode_Off = 0x0,

            /// @~chinese 镜像   @~english Mirrored
            MirrorMode_On = 0x1,

            /// @~chinese 任意   @~english Any
            MirrorMode_Both = 0x2,

        }MirrorModeEnum;

        /// @~chinese QR畸变   @~english QR Distortion
        typedef enum _DistortionFlagEnum
        {
            /// @~chinese 非畸变   @~english No Distortion
            DistortionFlag_Off = 0x0,

            /// @~chinese 畸变   @~english Distortion
            DistortionFlag_On = 0x1,

        }DistortionFlagEnum;

        /// @~chinese DM码类型   @~english DM Type
        typedef enum _RectangleEnum
        {
            /// @~chinese 正方形   @~english Square
            Rectangle_Square = 0x0,

            /// @~chinese 长方形   @~english Rectangle
            Rectangle_Rectangle = 0x1,

            /// @~chinese 兼容模式   @~english Compatibility
            Rectangle_AllMode = 0x2,

        }RectangleEnum;

        /// @~chinese ISO标准   @~english ISO Standard
        typedef enum _StandardTypeEnum
        {
            /// @~chinese ISO15415   @~english ISO15415
            StandardType_ISO15415 = 0x1,

            /// @~chinese ISO29158   @~english ISO29158
            StandardType_ISO29158 = 0x2,

        }StandardTypeEnum;

        /// @~chinese 镜像模式   @~english Mirror Mode
        typedef enum _MirrorEnum
        {
            /// @~chinese 非镜像   @~english Non-Mirrored
            Mirror_Off = 0x0,

            /// @~chinese 镜像   @~english Mirrored
            Mirror_On = 0x1,

            /// @~chinese 任意   @~english Any
            Mirror_Both = 0x2,

        }MirrorEnum;

        /// @~chinese 评级处理类型   @~english Classification Type
        typedef enum _ClassificationTypeEnum
        {
            /// @~chinese 处理类型1   @~english process type 1
            ClassificationType_process_type_one = 0x1,

            /// @~chinese 处理类型2   @~english process type 2
            ClassificationType_process_type_two = 0x2,

        }ClassificationTypeEnum;

        /// @~chinese 评级模式   @~english Verify Mode Type
        typedef enum _VerifyLabelTypeEnum
        {
            /// @~chinese ISO模式   @~english ISO Mode
            VerifyLabelType_ISO_Mode = 0x1,

            /// @~chinese HIK模式   @~english HIK Mode
            VerifyLabelType_HIK_Mode = 0x2,

        }VerifyLabelTypeEnum;

        /// @~chinese 极性   @~english Polarity
        typedef enum _PolarityEnum
        {
            /// @~chinese 白底黑码   @~english Dark On White
            Polarity_dark = 0x0,

            /// @~chinese 黑底白码   @~english White On Dark
            Polarity_bright = 0x1,

            /// @~chinese 任意   @~english Any
            Polarity_both = 0x2,

        }PolarityEnum;

        /// @~chinese 边缘类型   @~english Edge Type
        typedef enum _EdgeFeatureEnum
        {
            /// @~chinese 连续型   @~english Continuous
            EdgeFeature_continuous = 0x0,

            /// @~chinese 离散型   @~english Discrete
            EdgeFeature_discrete = 0x1,

        }EdgeFeatureEnum;

    /// \addtogroup DL读码
    /// @{
        /*************************** Params Class Start ***************************/
        /*  Note: Interface Classes of the RoiManager.  */
        ///DL读码ROI管理类
        class CnnCodeRecgRoiManager
        {
        public:

            /// @~chinese 矩形ROI（仅当次执行起效）   @~english Rectangle ROI(Only valid for this execution)
            virtual void SetRoiRectangle(RectBox stRoiValue) = 0;

        };

        /*  Note: Interface Classes of the parameter of the CnnCodeRecgParams.  */
        ///DL读码参数类
        class CnnCodeRecgParams : public CModuParamsBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnCodeRecgParams() {}
            virtual ~CnnCodeRecgParams() {}

        public:

            /// @~chinese 输入图像(仅当次执行起效)   @~english InputImage(Only valid for this execution)
            virtual void SetInputImage(ImageBaseData* pInputValue) = 0;

            /// @~chinese ROI管理器   @~english ROI Manager
            virtual CnnCodeRecgRoiManager* GetModuRoiManager() = 0;

            /// @~chinese CODE39码 @~english CODE 39
            __declspec(property(put = SetCODE39, get = GetCODE39)) bool CODE39;

            virtual bool GetCODE39() = 0;

            virtual void SetCODE39(bool value) = 0;

            /// @~chinese CODE128码  @~english CODE 128
            __declspec(property(put = SetCODE128, get = GetCODE128)) bool CODE128;

            virtual bool GetCODE128() = 0;

            virtual void SetCODE128(bool value) = 0;

            /// @~chinese 库得巴码 @~english  Codabar
            __declspec(property(put = SetCODABAR, get = GetCODABAR)) bool CODABAR;

            virtual bool GetCODABAR() = 0;

            virtual void SetCODABAR(bool value) = 0;

            /// @~chinese EAN8码 @~english EAN8
            __declspec(property(put = SetEAN8, get = GetEAN8)) bool EAN8;

            virtual bool GetEAN8() = 0;

            virtual void SetEAN8(bool value) = 0;

            /// @~chinese EAN13码 @~english EAN13
            __declspec(property(put = SetEAN13, get = GetEAN13)) bool EAN13;

            virtual bool GetEAN13() = 0;

            virtual void SetEAN13(bool value) = 0;

            /// @~chinese UPCA码 @~english UPCA
            __declspec(property(put = SetUPCA, get = GetUPCA)) bool UPCA;

            virtual bool GetUPCA() = 0;

            virtual void SetUPCA(bool value) = 0;

            /// @~chinese UPCE码 @~english UPCE
            __declspec(property(put = SetUPCE, get = GetUPCE)) bool UPCE;

            virtual bool GetUPCE() = 0;

            virtual void SetUPCE(bool value) = 0;

            /// @~chinese 交替25码 @~english ITF25
            __declspec(property(put = SetITF25, get = GetITF25)) bool ITF25;

            virtual bool GetITF25() = 0;

            virtual void SetITF25(bool value) = 0;

            /// @~chinese CODE93码 @~english CODE 93
            __declspec(property(put = SetCODE93, get = GetCODE93)) bool CODE93;

            virtual bool GetCODE93() = 0;

            virtual void SetCODE93(bool value) = 0;

            /// @~chinese MATRIX25码 @~english MATRIX25
            __declspec(property(put = SetMATRIX25, get = GetMATRIX25)) bool MATRIX25;

            virtual bool GetMATRIX25() = 0;

            virtual void SetMATRIX25(bool value) = 0;

            /// @~chinese MSI码 @~english MSI
            __declspec(property(put = SetMSI, get = GetMSI)) bool MSI;

            virtual bool GetMSI() = 0;

            virtual void SetMSI(bool value) = 0;

            /// @~chinese CNPOST码 @~english CHINAPOST
            __declspec(property(put = SetCHINAPOST, get = GetCHINAPOST)) bool CHINAPOST;

            virtual bool GetCHINAPOST() = 0;

            virtual void SetCHINAPOST(bool value) = 0;

            /// @~chinese CODE11码 @~english CODE11
            __declspec(property(put = SetCODE11, get = GetCODE11)) bool CODE11;

            virtual bool GetCODE11() = 0;

            virtual void SetCODE11(bool value) = 0;

            /// @~chinese IND25码 @~english INDUSTRIAL25
            __declspec(property(put = SetINDUSTRIAL25, get = GetINDUSTRIAL25)) bool INDUSTRIAL25;

            virtual bool GetINDUSTRIAL25() = 0;

            virtual void SetINDUSTRIAL25(bool value) = 0;

            /// @~chinese ITF14码 @~english ITF14
            __declspec(property(put = SetITF14, get = GetITF14)) bool ITF14;

            virtual bool GetITF14() = 0;

            virtual void SetITF14(bool value) = 0;

            /// @~chinese 条码个数 @~english Bar Code Number
            __declspec(property(put = SetBarCodeNum, get = GetBarCodeNum)) int BarCodeNum;

            virtual int GetBarCodeNum() = 0;

            virtual void SetBarCodeNum(int value) = 0;

            /// @~chinese QR码 @~english QR Code
            __declspec(property(put = SetQRCode, get = GetQRCode)) bool QRCode;

            virtual bool GetQRCode() = 0;

            virtual void SetQRCode(bool value) = 0;

            /// @~chinese DataMatrix码 @~english DM Code
            __declspec(property(put = SetDMCode, get = GetDMCode)) bool DMCode;

            virtual bool GetDMCode() = 0;

            virtual void SetDMCode(bool value) = 0;

            /// @~chinese 二维码个数 @~english 2D Code Number
            __declspec(property(put = SetLocCodeNum, get = GetLocCodeNum)) int LocCodeNum;

            virtual int GetLocCodeNum() = 0;

            virtual void SetLocCodeNum(int value) = 0;

            /// @~chinese 极性(弃用)   @~english Polarity(Obsolete)
            __declspec(property(put = SetCodePolarity, get = GetCodePolarity)) CodePolarityEnum CodePolarity;

            virtual CodePolarityEnum GetCodePolarity() = 0;

            virtual void SetCodePolarity(CodePolarityEnum value) = 0;

            /// @~chinese 边缘类型(弃用)   @~english Edge Type(Obsolete)
            __declspec(property(put = SetDiscreteFlag, get = GetDiscreteFlag)) DiscreteFlagEnum DiscreteFlag;

            virtual DiscreteFlagEnum GetDiscreteFlag() = 0;

            virtual void SetDiscreteFlag(DiscreteFlagEnum value) = 0;

            /// @~chinese 降采样倍数，范围：[1,6](弃用)   @~english Subsampling Ratio，range:[1,6](Obsolete)
            __declspec(property(put = SetSampleLevel, get = GetSampleLevel)) int SampleLevel;

            virtual int GetSampleLevel() = 0;

            virtual void SetSampleLevel(int value) = 0;

            /// @~chinese 镜像模式(弃用)   @~english Mirror Mode(Obsolete)
            __declspec(property(put = SetMirrorMode, get = GetMirrorMode)) MirrorModeEnum MirrorMode;

            virtual MirrorModeEnum GetMirrorMode() = 0;

            virtual void SetMirrorMode(MirrorModeEnum value) = 0;

            /// @~chinese QR畸变(弃用)   @~english QR Distortion(Obsolete)
            __declspec(property(put = SetDistortionFlag, get = GetDistortionFlag)) DistortionFlagEnum DistortionFlag;

            virtual DistortionFlagEnum GetDistortionFlag() = 0;

            virtual void SetDistortionFlag(DistortionFlagEnum value) = 0;

            /// @~chinese DM码类型(弃用)   @~english DM Type(Obsolete)
            __declspec(property(put = SetRectangle, get = GetRectangle)) RectangleEnum Rectangle;

            virtual RectangleEnum GetRectangle() = 0;

            virtual void SetRectangle(RectangleEnum value) = 0;

            /// @~chinese PDF417码 @~english PDF417 Code
            __declspec(property(put = SetPDF417Code, get = GetPDF417Code)) bool PDF417Code;

            virtual bool GetPDF417Code() = 0;

            virtual void SetPDF417Code(bool value) = 0;

            /// @~chinese 堆叠码个数，范围：[1,200]   @~english LocSD Code Number，range:[1,200]
            __declspec(property(put = SetLocSDCodeNum, get = GetLocSDCodeNum)) int LocSDCodeNum;

            virtual int GetLocSDCodeNum() = 0;

            virtual void SetLocSDCodeNum(int value) = 0;

            /// @~chinese 超时退出时间，范围：[0,5000]   @~english Timeout-Period to Exit，range:[0,5000]
            __declspec(property(put = SetWaitingTime, get = GetWaitingTime)) int WaitingTime;

            virtual int GetWaitingTime() = 0;

            virtual void SetWaitingTime(int value) = 0;

            /// @~chinese 高性能模式   @~english High Performance Mode
            __declspec(property(put = SetPerfMode, get = GetPerfMode)) bool PerfMode;

            virtual bool GetPerfMode() = 0;

            virtual void SetPerfMode(bool value) = 0;

            /// @~chinese 码行列号   @~english Arrange Flag
            __declspec(property(put = SetArrangeFlag, get = GetArrangeFlag)) bool ArrangeFlag;

            virtual bool GetArrangeFlag() = 0;

            virtual void SetArrangeFlag(bool value) = 0;

            /// @~chinese 行数量，范围：[1,200]   @~english Row Num，range:[1,200]
            __declspec(property(put = SetArrangeRowNum, get = GetArrangeRowNum)) int ArrangeRowNum;

            virtual int GetArrangeRowNum() = 0;

            virtual void SetArrangeRowNum(int value) = 0;

            /// @~chinese 列数量，范围：[1,200]   @~english Column Num，range:[1,200]
            __declspec(property(put = SetArrangeColumnNum, get = GetArrangeColumnNum)) int ArrangeColumnNum;

            virtual int GetArrangeColumnNum() = 0;

            virtual void SetArrangeColumnNum(int value) = 0;

            /// @~chinese 码等级   @~english Code Grade
            __declspec(property(put = SetGradeFlag, get = GetGradeFlag)) bool GradeFlag;

            virtual bool GetGradeFlag() = 0;

            virtual void SetGradeFlag(bool value) = 0;

            /// @~chinese ISO标准   @~english ISO Standard
            __declspec(property(put = SetStandardType, get = GetStandardType)) StandardTypeEnum StandardType;

            virtual StandardTypeEnum GetStandardType() = 0;

            virtual void SetStandardType(StandardTypeEnum value) = 0;

            /// @~chinese 镜像模式(弃用)   @~english Mirror Mode(Obsolete)
            __declspec(property(put = SetMirror, get = GetMirror)) MirrorEnum Mirror;

            virtual MirrorEnum GetMirror() = 0;

            virtual void SetMirror(MirrorEnum value) = 0;

            /// @~chinese 评级处理类型   @~english Classification Type
            __declspec(property(put = SetClassificationType, get = GetClassificationType)) ClassificationTypeEnum ClassificationType;

            virtual ClassificationTypeEnum GetClassificationType() = 0;

            virtual void SetClassificationType(ClassificationTypeEnum value) = 0;

            /// @~chinese 孔径尺寸，范围：[10,100](弃用)   @~english Aperture Size，range:[10,100](Obsolete)
            __declspec(property(put = SetAperture, get = GetAperture)) int Aperture;

            virtual int GetAperture() = 0;

            virtual void SetAperture(int value) = 0;

            /// @~chinese 标准孔径，范围：[0.01,100](弃用)   @~english Standard Aperture，range:[0.01,100](Obsolete)
            __declspec(property(put = SetStdAperture, get = GetStdAperture)) double StdAperture;

            virtual double GetStdAperture() = 0;

            virtual void SetStdAperture(double value) = 0;

            /// @~chinese 成像放大率，范围：[1,10000](弃用)   @~english System magnification，range:[1,10000](Obsolete)
            __declspec(property(put = SetSystemMag, get = GetSystemMag)) double SystemMag;

            virtual double GetSystemMag() = 0;

            virtual void SetSystemMag(double value) = 0;

            /// @~chinese 评级模式   @~english Verify Mode Type
            __declspec(property(put = SetVerifyLabelType, get = GetVerifyLabelType)) VerifyLabelTypeEnum VerifyLabelType;

            virtual VerifyLabelTypeEnum GetVerifyLabelType() = 0;

            virtual void SetVerifyLabelType(VerifyLabelTypeEnum value) = 0;

            /// @~chinese 极性   @~english Polarity
            __declspec(property(put = SetPolarity, get = GetPolarity)) PolarityEnum Polarity;

            virtual PolarityEnum GetPolarity() = 0;

            virtual void SetPolarity(PolarityEnum value) = 0;

            /// @~chinese 二维码行数，范围：[10,57]   @~english 2D Code Row，range:[10,57]
            __declspec(property(put = SetSymbolRows, get = GetSymbolRows)) int SymbolRows;

            virtual int GetSymbolRows() = 0;

            virtual void SetSymbolRows(int value) = 0;

            /// @~chinese 二维码列数，范围：[10,57]   @~english 2D Code Column，range:[10,57]
            __declspec(property(put = SetSymbolCols, get = GetSymbolCols)) int SymbolCols;

            virtual int GetSymbolCols() = 0;

            virtual void SetSymbolCols(int value) = 0;

            /// @~chinese 边缘类型(弃用)   @~english Edge Type(Obsolete)
            __declspec(property(put = SetEdgeFeature, get = GetEdgeFeature)) EdgeFeatureEnum EdgeFeature;

            virtual EdgeFeatureEnum GetEdgeFeature() = 0;

            virtual void SetEdgeFeature(EdgeFeatureEnum value) = 0;

            /// @~chinese 译码评分   @~english Decoding Score
            __declspec(property(put = SetDecodeFlag, get = GetDecodeFlag)) bool DecodeFlag;

            virtual bool GetDecodeFlag() = 0;

            virtual void SetDecodeFlag(bool value) = 0;

            /// @~chinese 边缘确定度   @~english Edge Certainty
            __declspec(property(put = SetEdgeDeterminationFlag, get = GetEdgeDeterminationFlag)) bool EdgeDeterminationFlag;

            virtual bool GetEdgeDeterminationFlag() = 0;

            virtual void SetEdgeDeterminationFlag(bool value) = 0;

            /// @~chinese 符号对比度   @~english SymbolContrast
            __declspec(property(put = SetSymbolContrastFlag, get = GetSymbolContrastFlag)) bool SymbolContrastFlag;

            virtual bool GetSymbolContrastFlag() = 0;

            virtual void SetSymbolContrastFlag(bool value) = 0;

            /// @~chinese 最小反射率   @~english Minimum Reflectivity
            __declspec(property(put = SetMinReflectanceFlag, get = GetMinReflectanceFlag)) bool MinReflectanceFlag;

            virtual bool GetMinReflectanceFlag() = 0;

            virtual void SetMinReflectanceFlag(bool value) = 0;

            /// @~chinese 边缘对比度   @~english Edge Contrast
            __declspec(property(put = SetMinEdgeContrastFlag, get = GetMinEdgeContrastFlag)) bool MinEdgeContrastFlag;

            virtual bool GetMinEdgeContrastFlag() = 0;

            virtual void SetMinEdgeContrastFlag(bool value) = 0;

            /// @~chinese 模块均匀性   @~english Module Uniformity
            __declspec(property(put = SetModulationFlag, get = GetModulationFlag)) bool ModulationFlag;

            virtual bool GetModulationFlag() = 0;

            virtual void SetModulationFlag(bool value) = 0;

            /// @~chinese 可译码性   @~english Decodability
            __declspec(property(put = SetDecodabilityFlag, get = GetDecodabilityFlag)) bool DecodabilityFlag;

            virtual bool GetDecodabilityFlag() = 0;

            virtual void SetDecodabilityFlag(bool value) = 0;

            /// @~chinese 缺陷度   @~english Defects
            __declspec(property(put = SetDefectsFlag, get = GetDefectsFlag)) bool DefectsFlag;

            virtual bool GetDefectsFlag() = 0;

            virtual void SetDefectsFlag(bool value) = 0;

            /// @~chinese 静区(弃用)   @~english Quiet Zone(Obsolete)
            __declspec(property(put = SetQuietZoneFlag, get = GetQuietZoneFlag)) bool QuietZoneFlag;

            virtual bool GetQuietZoneFlag() = 0;

            virtual void SetQuietZoneFlag(bool value) = 0;

            /// @~chinese 可译码性A，范围：[0,100]   @~english Decodability A，range:[0,100]
            __declspec(property(put = SetDecodabilityThrA, get = GetDecodabilityThrA)) int DecodabilityThrA;

            virtual int GetDecodabilityThrA() = 0;

            virtual void SetDecodabilityThrA(int value) = 0;

            /// @~chinese 可译码性B，范围：[0,100]   @~english Decodability B，range:[0,100]
            __declspec(property(put = SetDecodabilityThrB, get = GetDecodabilityThrB)) int DecodabilityThrB;

            virtual int GetDecodabilityThrB() = 0;

            virtual void SetDecodabilityThrB(int value) = 0;

            /// @~chinese 可译码性C，范围：[0,100]   @~english Decodability C，range:[0,100]
            __declspec(property(put = SetDecodabilityThrC, get = GetDecodabilityThrC)) int DecodabilityThrC;

            virtual int GetDecodabilityThrC() = 0;

            virtual void SetDecodabilityThrC(int value) = 0;

            /// @~chinese 可译码性D，范围：[0,100]   @~english Decodability D，range:[0,100]
            __declspec(property(put = SetDecodabilityThrD, get = GetDecodabilityThrD)) int DecodabilityThrD;

            virtual int GetDecodabilityThrD() = 0;

            virtual void SetDecodabilityThrD(int value) = 0;

            /// @~chinese 缺陷度A，范围：[0,100]   @~english Defect Degree A，range:[0,100]
            __declspec(property(put = SetDefectsThrA, get = GetDefectsThrA)) int DefectsThrA;

            virtual int GetDefectsThrA() = 0;

            virtual void SetDefectsThrA(int value) = 0;

            /// @~chinese 缺陷度B，范围：[0,100]   @~english Defect Degree B，range:[0,100]
            __declspec(property(put = SetDefectsThrB, get = GetDefectsThrB)) int DefectsThrB;

            virtual int GetDefectsThrB() = 0;

            virtual void SetDefectsThrB(int value) = 0;

            /// @~chinese 缺陷度C，范围：[0,100]   @~english Defect Degree C，range:[0,100]
            __declspec(property(put = SetDefectsThrC, get = GetDefectsThrC)) int DefectsThrC;

            virtual int GetDefectsThrC() = 0;

            virtual void SetDefectsThrC(int value) = 0;

            /// @~chinese 缺陷度D，范围：[0,100]   @~english Defect Degree D，range:[0,100]
            __declspec(property(put = SetDefectsThrD, get = GetDefectsThrD)) int DefectsThrD;

            virtual int GetDefectsThrD() = 0;

            virtual void SetDefectsThrD(int value) = 0;

            /// @~chinese 最小反射率A，范围：[0,100]   @~english Minimum Reflectivity A，range:[0,100]
            __declspec(property(put = SetMinReflectanceThrA, get = GetMinReflectanceThrA)) int MinReflectanceThrA;

            virtual int GetMinReflectanceThrA() = 0;

            virtual void SetMinReflectanceThrA(int value) = 0;

            /// @~chinese 边缘对比度A，范围：[0,100]   @~english Edge Contrast A，range:[0,100]
            __declspec(property(put = SetMinEdgeContrastThrA, get = GetMinEdgeContrastThrA)) int MinEdgeContrastThrA;

            virtual int GetMinEdgeContrastThrA() = 0;

            virtual void SetMinEdgeContrastThrA(int value) = 0;

            /// @~chinese 模块均匀性A，范围：[0,100]   @~english Module Uniformity A，range:[0,100]
            __declspec(property(put = SetModulationThrA, get = GetModulationThrA)) int ModulationThrA;

            virtual int GetModulationThrA() = 0;

            virtual void SetModulationThrA(int value) = 0;

            /// @~chinese 模块均匀性B，范围：[0,100]   @~english Module Uniformity B，range:[0,100]
            __declspec(property(put = SetModulationThrB, get = GetModulationThrB)) int ModulationThrB;

            virtual int GetModulationThrB() = 0;

            virtual void SetModulationThrB(int value) = 0;

            /// @~chinese 模块均匀性C，范围：[0,100]   @~english Module Uniformity C，range:[0,100]
            __declspec(property(put = SetModulationThrC, get = GetModulationThrC)) int ModulationThrC;

            virtual int GetModulationThrC() = 0;

            virtual void SetModulationThrC(int value) = 0;

            /// @~chinese 模块均匀性D，范围：[0,100]   @~english Module Uniformity D，range:[0,100]
            __declspec(property(put = SetModulationThrD, get = GetModulationThrD)) int ModulationThrD;

            virtual int GetModulationThrD() = 0;

            virtual void SetModulationThrD(int value) = 0;

            /// @~chinese 对比度A，范围：[0,100]   @~english Contrast A，range:[0,100]
            __declspec(property(put = SetSymbolContrastThrA, get = GetSymbolContrastThrA)) int SymbolContrastThrA;

            virtual int GetSymbolContrastThrA() = 0;

            virtual void SetSymbolContrastThrA(int value) = 0;

            /// @~chinese 对比度B，范围：[0,100]   @~english Contrast B，range:[0,100]
            __declspec(property(put = SetSymbolContrastThrB, get = GetSymbolContrastThrB)) int SymbolContrastThrB;

            virtual int GetSymbolContrastThrB() = 0;

            virtual void SetSymbolContrastThrB(int value) = 0;

            /// @~chinese 对比度C，范围：[0,100]   @~english Contrast C，range:[0,100]
            __declspec(property(put = SetSymbolContrastThrC, get = GetSymbolContrastThrC)) int SymbolContrastThrC;

            virtual int GetSymbolContrastThrC() = 0;

            virtual void SetSymbolContrastThrC(int value) = 0;

            /// @~chinese 对比度D，范围：[0,100]   @~english Contrast D，range:[0,100]
            __declspec(property(put = SetSymbolContrastThrD, get = GetSymbolContrastThrD)) int SymbolContrastThrD;

            virtual int GetSymbolContrastThrD() = 0;

            virtual void SetSymbolContrastThrD(int value) = 0;

            /// @~chinese 孔径设置(弃用)   @~english Aperture Settings(Obsolete)
            __declspec(property(put = SetApertureEnable, get = GetApertureEnable)) bool ApertureEnable;

            virtual bool GetApertureEnable() = 0;

            virtual void SetApertureEnable(bool value) = 0;

            /// @~chinese 孔径尺寸，范围：[1,100](弃用)   @~english Aperture Size，range:[1,100](Obsolete)
            __declspec(property(put = SetApertureIn, get = GetApertureIn)) int ApertureIn;

            virtual int GetApertureIn() = 0;

            virtual void SetApertureIn(int value) = 0;

            /// @~chinese 个数判断   @~english Number Check
            __declspec(property(put = SetNumLimitEnable, get = GetNumLimitEnable)) bool NumLimitEnable;

            virtual bool GetNumLimitEnable() = 0;

            virtual void SetNumLimitEnable(bool value) = 0;

            /// @~chinese 个数范围，范围：[0,99999]   @~english Number Range，range:[0,99999]
            __declspec(property(put = SetNumLimitLow, get = GetNumLimitLow)) int NumLimitLow;

            virtual int GetNumLimitLow() = 0;

            virtual void SetNumLimitLow(int value) = 0;

            /// @~chinese 个数范围，范围：[0,99999]   @~english Number Range，range:[0,99999]
            __declspec(property(put = SetNumLimitHigh, get = GetNumLimitHigh)) int NumLimitHigh;

            virtual int GetNumLimitHigh() = 0;

            virtual void SetNumLimitHigh(int value) = 0;

            /// @~chinese 字符验证   @~english Character Verification
            __declspec(property(put = SetVerifyEnable, get = GetVerifyEnable)) bool VerifyEnable;

            virtual bool GetVerifyEnable() = 0;

            virtual void SetVerifyEnable(bool value) = 0;

            /// @~chinese 数字集   @~english Number Set
            __declspec(property(put = SetNumVerifyEnable, get = GetNumVerifyEnable)) bool NumVerifyEnable;

            virtual bool GetNumVerifyEnable() = 0;

            virtual void SetNumVerifyEnable(bool value) = 0;

            /// @~chinese 小写字母集   @~english Lowercase Set
            __declspec(property(put = SetSmallAlphabetVerify, get = GetSmallAlphabetVerify)) bool SmallAlphabetVerify;

            virtual bool GetSmallAlphabetVerify() = 0;

            virtual void SetSmallAlphabetVerify(bool value) = 0;

            /// @~chinese 大写字母集   @~english Uppercase Set
            __declspec(property(put = SetBigAlphabetVerify, get = GetBigAlphabetVerify)) bool BigAlphabetVerify;

            virtual bool GetBigAlphabetVerify() = 0;

            virtual void SetBigAlphabetVerify(bool value) = 0;

            /// @~chinese 特殊字符集   @~english Special Character Set
            __declspec(property(put = SetSpecialCharVerify, get = GetSpecialCharVerify)) bool SpecialCharVerify;

            virtual bool GetSpecialCharVerify() = 0;

            virtual void SetSpecialCharVerify(bool value) = 0;

            /// @~chinese 用户字符验证   @~english UDC Verification
            __declspec(property(put = SetUserStringVerify, get = GetUserStringVerify)) bool UserStringVerify;

            virtual bool GetUserStringVerify() = 0;

            virtual void SetUserStringVerify(bool value) = 0;

            /// @~chinese 用户字符   @~english UDC
            __declspec(property(put = SetUserString, get = GetUserString)) const char* UserString;

            virtual const char* GetUserString() = 0;

            virtual void SetUserString(const char* value) = 0;

        };
        /*************************** Params Class End ***************************/

        /*************************** Results Class Start ***************************/
        ///Note: 码信息描述类
        class ICodeInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit ICodeInfo() {}
            virtual ~ICodeInfo() {}

        public:
            /**
             * @brief 码状态
             */
            virtual int GetCodeStatus() = 0;
            /**
            * @brief 码角点信息
            * @remarks 码的角点信息,分别为:左上角、右上角、左下角、右下角顶点坐标
            * @deprecated V4.4废弃
            */
            virtual PointF GetCornerPoint(int nIndex) = 0;
            /**
            * @brief 码角点信息
            * @remarks 码的角点信息,分别为:左上角、右上角、左下角、右下角顶点坐标
            */
            virtual PointF GetCodeCornerPoint(int nIndex) = 0;
            /**
            * @brief 码的矩形框
            */
            virtual RectBox GetRect() = 0;
            /**
             * @brief 码内容
             */
            virtual const char* GetContent() = 0;
            /**
             * @brief 码类型
             */
            virtual int GetType() = 0;
            /**
             * @brief 码角度
             */
            virtual float GetAngle() = 0;
            /**
             * @brief 估计ppm
             */
            virtual float GetEstppm() = 0;
            /**
            * @brief 码行号
            */
            virtual int GetCodeRowNum() = 0;
            /**
            * @brief 码列号
            */
            virtual int GetCodeColumnNum() = 0;
        };

        ///码等级信息描述类
        class ICodeGradeInfo
        {
        protected:
            ///< Constructor and Destructor
            explicit ICodeGradeInfo() {}
            virtual ~ICodeGradeInfo() {}

        public:
             /**
             * @brief 总质量等级
             */
             virtual int GetOverQuality() = 0;
             /**
             * @brief 译码评分
             */
             virtual int GetDecode() = 0;
             /**
             * @brief 符号对比度
             */
             virtual int GetSymbolContrast() = 0;
             /**
             * @brief 模块均匀性
             */
             virtual int GetModulation() = 0;
             /**
             * @brief 符号对比度分数
             */
             virtual float GetScScore() = 0;
             /**
             * @brief 模块均匀性分数
             */
             virtual float GetModScore() = 0;
             /**
             * @brief 固定程度
             */
             virtual int GetFixedPatternDamage() = 0;
             /**
             * @brief 轴向不均匀性
             */
             virtual int GetAxial() = 0;
             /**
             * @brief 网格不均匀性
             */
             virtual int GetGrid() = 0;
             /**
             * @brief 未使用纠错
             */
             virtual int GetUnusedErrorCorrection() = 0;
             /**
             * @brief 水平打印伸缩
             */
             virtual int GetPrintGrowthHor() = 0;
             /**
             * @brief 垂直打印伸缩
             */
             virtual int GetPrintGrowthVer() = 0;
             /**
             * @brief 反射率余量
             */
             virtual int GetRmGrade() = 0;
             /**
             * @brief 固定程度分数
             */
             virtual float GetFpdScore() = 0;
             /**
             * @brief 码轴规整性分数
             */
             virtual float GetAnScore() = 0;
             /**
             * @brief 网格均匀性分数
             */
             virtual float GetGnScore() = 0;
             /**
             * @brief 未使用纠错分数
             */
             virtual float GetUecScore() = 0;
             /**
             * @brief 水平打印伸缩分数
             */
             virtual float GetPghScore() = 0;
             /**
             * @brief 垂直打印伸缩分数
             */
             virtual float GetPgvScore() = 0;
             /**
             * @brief 反射率余量分数
             */
             virtual float GetRmScore() = 0;
             /**
             * @brief 边缘确定度
             */
             virtual int GetEdgeDetermination() = 0;
             /**
             * @brief 最小反射率
             */
             virtual int GetMinReflectance() = 0;
             /**
             * @brief 最小边缘对比度
             */
             virtual int GetMinEdgeContrast() = 0;
             /**
             * @brief 可译码性
             */
             virtual int GetDecodability() = 0;
             /**
             * @brief 缺陷度
             */
             virtual int GetDefects() = 0;
             /**
             * @brief 静区
             */
             virtual int GetQuietZone() = 0;
             /**
             * @brief 边缘确定度分数
             */
             virtual float GetEdgeScore() = 0;
             /**
             * @brief 最小反射率分数
             */
             virtual float GetMinrScore() = 0;
             /**
             * @brief 最小边缘对比度分数
             */
             virtual float GetMineScore() = 0;
             /**
             * @brief 可译码性分数
             */
             virtual float GetDcdScore() = 0;
             /**
             * @brief 缺陷分数
             */
             virtual float GetDefScore() = 0;
             /**
             * @brief 静区分数
             */
             virtual float GetQzScore() = 0;

        };

        /*  Note: Interface Classes of the result of the CnnCodeRecgResults.  */
        ///DL读码结果类
        class CnnCodeRecgResults : public CModuleResultBase
        {
        protected:
            ///< Constructor and Destructor
            explicit CnnCodeRecgResults() {}
            virtual ~CnnCodeRecgResults() {}

        public:
             /**
             * @brief 模块状态
             */
             virtual int GetModuStatus() = 0;
             /**
             * @brief 检测区域
             */
             virtual RectBox GetROI() = 0;
            /**
             * @brief 码个数
             */
            virtual int GetCodeNum() = 0;

            /**
             * @brief 获取码信息
             * @param nIndex [IN] 码信息索引
             * @return 返回码信息
             * @remarks  nIndex从0开始计数
             */
            virtual ICodeInfo* GetCodeInfo(int nIndex) = 0;

            /**
             * @brief 码等级数量
             */
            virtual int GetCodeGradeNum() = 0;

            /**
             * @brief 获取码等级信息
             * @param nIndex [IN] 码等级信息索引
             * @return 返回码等级信息
             * @remarks  nIndex从0开始计数
             */
            virtual ICodeGradeInfo* GetCodeGradeInfo(int nIndex) = 0;
        };
        /*************************** Results Class End ***************************/

        /*************************** Tool Class Start ***************************/
        /*  Note: Note: This is the class for the CnnCodeRecg tool.  */
        ///DL读码工具类
        class IMVSCnnCodeRecgModuTool : public CVmToolBase
        {
        protected:
            ///< Constructor and Destructor
            explicit IMVSCnnCodeRecgModuTool() {}
            virtual ~IMVSCnnCodeRecgModuTool() {}

        public:
            /**
             * @brief 参数对象
             * @return 失败，返回NULL
             * @remarks 通过参数对象可设置获取模块参数
             */
            virtual CnnCodeRecgParams* GetParamObj() = 0;

            /**
             * @brief 查找结果
             * @return 未执行或执行失败，返回NULL
             */
            virtual CnnCodeRecgResults* GetResult() = 0;

            /**
             * @brief 模块自执行
             * @return 无，出错时抛出异常
             * @remarks 模块自执行一次
             */
            virtual void Run() = 0;

        private:
            IMVSCnnCodeRecgModuTool(IMVSCnnCodeRecgModuTool&);
            IMVSCnnCodeRecgModuTool& operator=(IMVSCnnCodeRecgModuTool&);
        };
        /*************************** Tool Class End ***************************/
    /// @}
    }
}
/*  Note:Interfaces to export.  */
#ifdef __cplusplus
extern "C" {
#endif

/*  Note: Get tool instance for CnnCodeRecg.  */
IMVS_6000_PLATFORMSDK_API VisionMasterSDK::IMVSCnnCodeRecgModu::IMVSCnnCodeRecgModuTool * __stdcall GetCnnCodeRecgToolInstance(IN const char * const strName);

#ifdef __cplusplus
}
#endif

#endif    ///< _IVM_CNNCODERECG_H_
