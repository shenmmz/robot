/***************************************************************************************************
* File:  MVD_ErrorDefine.h
* Note:  ch: 算法包SDK层相关的状态码定义。
         En: SDK-related status code definition in namespace VisionDesigner.
*
* Version:  V3.2.0.3
****************************************************************************************************/


#ifndef _MVD_ERROR_DEFINE_H_
#define _MVD_ERROR_DEFINE_H_

#include "MVD_ErrorDefine_Exp.h"
#include "MVD_ErrorDefine_Algo.h"
#include "MVD_ErrorDefine_Cam.h"


// ch:正确码定义 | en:Definition of correct code
const static int MVD_OK                      = 0x00000000;    ///< 成功，无错误 | en:Successed, no error
const static int MVD_E_MIN                   = 0x80100000;    ///< 算法包SDK的错误码最小值 | en:Minimum value of the SDK-releated error code.
const static int MVD_E_MAX                   = 0x8010FFFF;    ///< 算法包SDK的错误码最大值 | en:Maximum value of the SDK-releated error code.

//算法包通用错误码定义:范围0x80100000-0x801000FF | en:Common error 
const static int MVD_E_HANDLE                = 0x80100000;    ///< 错误或无效的句柄 | en:Error or invalid handle
const static int MVD_E_SUPPORT               = 0x80100001;    ///< 不支持的功能 | en:Not supported function
const static int MVD_E_OVER_ABILITY          = 0x80100002;    ///< 超出限制的能力范围 | en: Beyond the limits of capacity
const static int MVD_E_CALLORDER             = 0x80100003;    ///< 函数调用顺序错误 | en:Function calling order error
const static int MVD_E_PRECONDITION          = 0x80100004;    ///< 前置条件有误 | en:Precondition error
const static int MVD_E_PARAMETER_RANGE       = 0x80100005;    ///< 参数超出范围限制 | en:Parameter out of range limit
const static int MVD_E_PARAMETER_ILLEGAL     = 0x80100006;    ///< 参数非法 | en:Illegal parameter. Such as a null pointer
const static int MVD_E_PARAMETER_MATCH       = 0x80100007;    ///< 参数互相不匹配 | en:Parameters do not match each other
const static int MVD_E_PARAMETER_GENERIC     = 0x80100008;    ///< 通用参数错误 | en:General parameter error
const static int MVD_E_RESOURCE              = 0x80100009;    ///< 资源申请失败 | en:Applying resource failed
const static int MVD_E_BUFOVER               = 0x8010000A;    ///< 缓存已满 | en:Out of memory
const static int MVD_E_NOOUTBUF              = 0x8010000B;    ///< 没有可输出的缓存 | en:No Avaliable Buffer
const static int MVD_E_NOENOUGH_BUF          = 0x8010000C;    ///< 传入的内存空间不足 | en:Insufficient memory prepared
const static int MVD_E_FILE_PATH             = 0x8010000D;    ///< 不支持的文件路径 | en: Unsupported file path
const static int MVD_E_FILE_FORMAT           = 0x8010000E;    ///< 不支持的文件格式 | en: Unsupported file format
const static int MVD_E_FILE_CORRUPTED        = 0x8010000F;    ///< 文件损坏 | en: Corrupted file
const static int MVD_E_FILE_GENERIC          = 0x80100010;    ///< 通用文件错误 | en: General file error
const static int MVD_E_NODATA                = 0x80100011;    ///< 无数据 | en:No data
const static int MVD_E_ABNORMAL_IMAGE        = 0x80100012;    ///< 异常图像 | en:Abnormal image
const static int MVD_E_VERSION               = 0x80100013;    ///< 版本不匹配 | en:Version mismatches
const static int MVD_E_LOAD_LIBRARY          = 0x80100014;    ///< 动态导入DLL失败 | en:Load library failed
const static int MVD_E_RUNTIME               = 0x80100015;    ///< 运行环境错误 | en: Wrong with running environment
const static int MVD_E_NO_AVAILABLE_DEVICE   = 0x80100016;    ///< 没有可用设备，不存在可用的显卡或者显卡驱动未更新| en: No avaliable device
const static int MVD_E_MODEL_TYPE            = 0x80100017;    ///< 模型类型错误 | en: Wrong with model type 
const static int MVD_E_MODEL_VERSION         = 0x80100018;    ///< 模型版本错误 | en:Wrong with model version 
const static int MVD_E_MODEL_ANALYSIS        = 0x80100019;    ///< 模型解析错误 | en:Wrong with model analysis
const static int MVD_E_OUTOF_MEMORY          = 0x8010001A;    ///< 申请CPU内存失败 | en:Fail to allocate CPU memory
const static int MVD_E_OUTOF_GPUMEMORY       = 0x8010001B;    ///< 申请GPU内存失败 | en:Fail to allocate GPU memory
const static int MVD_E_ROI_TYPE_ERROR        = 0x8010001C;    ///< ROI类型不支持     | en:Unsupported ROI shape type
const static int MVD_E_ROI_ANGLE_ERROR       = 0x8010001D;    ///< 不支持带角度的ROI | en:Unsupported ROI with angle
const static int MVD_E_PARAM_KEYNAME         = 0x8010001E;    ///< 运行参数名有误 | en:Run param name error
const static int MVD_E_MVDALGO_DEV_ENV       = 0x8010001F;    ///< 环境变量配置不正确 | en:The environment variable is misconfigured
const static int MVD_E_GET_LOCK_KEY          = 0x80100020;    ///< 获取加密狗Key失败 | en: Failed to get key from lock
const static int MVD_E_DATA_ENCRYPTION       = 0x80100021;    ///< 数据加密失败 | en: Failed to exceed data encryption
const static int MVD_E_DATA_DECRYPTION       = 0x80100022;    ///< 数据解密失败 | en: Failed to exceed data decryption
const static int MVD_E_DATA_PARSE            = 0x80100023;    ///< 数据解析失败 | en: Failed to parse data
const static int MVD_E_GPU_DEVICE            = 0x80100024;    ///< GPU设备操作错误 | en: Wrong with GPU device
const static int MVD_E_ROI_RANGE             = 0x80100025;    ///< ROI超出范围 | en: ROI out of range limit
const static int MVD_E_UNKNOW                = 0x801000FF;    ///< 未知的错误 | en:Unknown error

// 算法处理流程相关:范围0x80100200-0x801002FF
const static int MVD_ALGORITHM_E_ALGORITHM   = 0x80100201;    ///< ch: 算法版本错误 | en:Unknown error on algorithms
const static int MVD_ALGORITHM_E_CONFIG      = 0x80100202;    ///< ch: 算法配置错误 | en: Wrong configuration
const static int MVD_ALGORITHM_E_UNKNOW      = 0x80100203;    ///< ch: 算法未知错误 | en:Unknown error on algorithm SDK

// 字符缺陷检测算法相关
const static int MVD_ALGORITHM_E_MKINSP_ROUGH_MATCH_FAILED          = 0x80100204;    ///< ch: 字符缺陷检测粗定位失败
const static int MVD_ALGORITHM_E_MKINSP_MODEL_VERSION_NOT_SUPPORT   = 0x80100205;    ///< ch: 字符缺陷检测模板版本号不支持

const static int MVD_E_RENDER_IMG_FORMAT            = 0x80100301;    ///< ch: 图像像素格式不正确或者不支持 | en: Image pixel format is not supported or incorrect
const static int MVD_E_RENDER_IMG_TYPE              = 0x80100302;    ///< ch: 图像类型不支持或不正确 | en: Image type is not supported or incorrect
const static int MVD_E_RENDER_IMG_SIZE              = 0x80100303;    ///< ch: 图像宽高不正确或者超出范围| en: Image width or height is incorrect or unsupported
const static int MVD_E_RENDER_IMG_ENCOD             = 0x80100304;    ///< ch: 图像编解码失败 | en: Image encoding and decoding failed
const static int MVD_E_RENDER_CANVAS_ZOOM           = 0x80100305;    ///< ch: 画布缩放超出限制 | en: Canvas zoom is out of scope
const static int MVD_E_RENDER_CANVAS_PARAM          = 0x80100306;    ///< ch: 画布大小范围非法 | en: Canvas size range is illegal
const static int MVD_E_RENDER_GENERIC               = 0x80100307;    ///< ch: 渲染引擎通用错误 | en: Rendering engine generic error
const static int MVD_E_RENDER_ILLEGAL_SHAPE         = 0x80100308;    ///< ch: 渲染引擎绘制范围有误的图形 | en: Graphics with the wrong rendering engine scope
const static int MVD_E_RENDER_MEMORY                = 0x80100309;    ///< ch: 渲染引擎画布拷贝错误 | en: Render engine canvas copy error
const static int MVD_E_RENDER_HANDLD_NOTFIND        = 0x8010030A;    ///< ch: 图像或图形句柄不存在或无效 | en: Render Shape or Img Handle not Find
#endif //_MVD_ERROR_DEFINE_H_

