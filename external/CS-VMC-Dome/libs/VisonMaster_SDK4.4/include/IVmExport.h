#ifndef _IVM_EXPORT_H__
#define _IVM_EXPORT_H__

#pragma warning(disable:4819)
/* note: Interface export definition of the namespace VisionMasterSDK */

namespace VisionMasterSDK
{
    #ifdef WIN32
        #if defined(IMVS6000PLATFORMSDK_EXPORTS)
            #define IMVS_6000_PLATFORMSDK_API __declspec(dllexport)
        #else
            #define IMVS_6000_PLATFORMSDK_API __declspec(dllimport)
        #endif
    #else
        #ifndef __stdcall
            #define __stdcall
        #endif
        #ifndef IMVS_6000_PLATFORMSDK_API
            #define IMVS_6000_PLATFORMSDK_API
        #endif
    #endif

    #ifndef IN
        #define IN
    #endif

    #ifndef OUT
        #define OUT
    #endif

    #ifndef INOUT
        #define INOUT
    #endif
}

/// \addtogroup 公共定义类
/// @{

//////////////////////////////////// 公共定义 //////////////////////////////////////
const static unsigned int MAX_PROCESS_NAME_LENGTH = 64;         ///< @~chinese 流程名称最大长度 @~english Maximum length of process name
const static unsigned int MAX_PROCESS_NUM = 1000;               ///< @~chinese 最大流程个数 @~english Maximum number of processes
const static unsigned int MAX_MODULE_DISPLAY_NAME_LENGTH = 256; ///< @~chinese 模块显示名称最大长度 @~english Maximum length of module display name
const static unsigned int MAX_MODULE_NAME_LENGTH = 64;          ///< @~chinese 模块名称最大长度 @~english Maximum length of module name
const static unsigned int MAX_MODULE_NUM = 2064;                ///< @~chinese 最大模块个数 @~english Maximum number of modules
const static unsigned int MAX_CORE_NUM = 64;                    ///< @~chinese 最大CPU核心个数
const static unsigned int MAX_OBJECT_IO_NAME_LENGTH = 128;      ///< @~chinese IO名称最大长度 @~english Maximum length of IO name
const static unsigned int MAX_OBJECT_IO_NUM = 1024;             ///< @~chinese IO最大个数 @~english Maximum number of IO
const static unsigned int MAX_OBJECT_NUM = 2064;                ///< @~chinese 对象的最大个数 @~english Maximum number of Objects
const static unsigned int MAX_CAMERA_SN_LEN = 64;               ///< @~chinese 相机最大序列号长度 @~english Maximum length of camera sn
const static unsigned int MAX_CAMERA_NAME_LEN = 64;             ///< @~chinese 相机最大名称长度 @~english Maximum length of camera name
const static unsigned int MAX_CAMERA_NUM = 256;                 ///< @~chinese 相机最大数量 @~english Maximum length of camera number
const static unsigned int MAX_MODULE_RUN_ERROR_INFO_NUM = 16;   ///< @~chinese 模块执行错误信息最大数量 @~english Maximum number of module run error info

/// @~chinese VM对象类型 @~english Objects type in VM
enum VMObjectType
{
    VMObjectType_Unknown = 0,       ///< @~chinese 未知类型 @~english Unknown type
    VMObjectType_Procedure = 1,     ///< @~chinese 流程对象 @~english Procedure type
    VMObjectType_Module = 2         ///< @~chinese 模块对象 @~english Module type
};

/// @~chinese 对象信息列表结构 @~english Objects information list structure
typedef struct ObjectList__
{
    VMObjectType        enType;                         ///< @~chinese 对象类型 @~english Object type
    unsigned int        nNum;                           ///< @~chinese 对象个数 @~english The number of Objects in the list
    void*               pObjects[MAX_OBJECT_NUM];       ///< @~chinese 对象信息列表 @~english Object information
    unsigned int        nReserved[4];                   ///< @~chinese 保留字段 @~english Reserved
} ObjectList;

/// @~chinese 流程信息结构 @~english Process information structure
typedef struct ProcessInfo__
{
    unsigned int nProcessID;                                    ///< @~chinese 流程ID @~english Process ID
    char         strProcessName[MAX_PROCESS_NAME_LENGTH];       ///< @~chinese 流程名称 @~english Process name
    unsigned int nReserved[4];                                  ///< @~chinese 保留字段 @~english Reserved
} ProcessInfo;

/// @~chinese 流程信息列表结构 @~english Process information list structure
typedef struct ProcessInfoList__
{
    unsigned int         nNum;                                  ///< @~chinese 流程个数 @~english The number of processes in the list
    ProcessInfo          astProcessInfo[MAX_PROCESS_NUM];       ///< @~chinese 流程信息列表 @~english Process information
    unsigned int         nReserved[4];                          ///< @~chinese 保留字段 @~english Reserved
} ProcessInfoList;

/// @~chinese 单个相机信息结构 @~english Camera information structure
typedef struct CameraInfo__
{
    char strCameraName[MAX_CAMERA_NAME_LEN];           ///< @~chinese 相机名称 @~english Camera Name
    char strCameraSN[MAX_CAMERA_SN_LEN];               ///< @~chinese 相机SN @~english Camera SN
    unsigned int nReserved[4];                         ///< @~chinese 保留字段 @~english Reserved
} CameraInfo;

/// @~chinese 相机信息列表结构 @~english Camera information list structure
typedef struct CameraInfoList__
{
    unsigned int nNum;                                          ///< @~chinese 相机数量 @~english Camera Number
    CameraInfo astCameraInfo[MAX_CAMERA_NUM];                   ///< @~chinese 相机信息 @~english Camera Info
    unsigned int nReserved[4];                                  ///< @~chinese 保留字段 @~english Reserved
} CameraInfoList;

/// @~chinese 单个模块信息结构 @~english Module information structure
typedef struct ModuInfo__
{
    unsigned int    nModuleID;                                      ///< @~chinese 模块ID @~english Module ID
    unsigned int    nProcessID;                                     ///< @~chinese 流程ID @~english Process ID
    char            strDisplayName[MAX_MODULE_DISPLAY_NAME_LENGTH]; ///< @~chinese 界面显示名称 @~english Displayed module name
    char            strModuleName[MAX_MODULE_NAME_LENGTH];          ///< @~chinese 模块名称 @~english Module name
    unsigned int    nReserved[128];                                 ///< @~chinese 保留字段 @~english Reserved
} ModuInfo;

/// @~chinese 模块信息列表结构 @~english Module information list structure
typedef struct ModuleInfoList__
{
    unsigned int           nNum;                                     ///< @~chinese 模块个数 @~english Number of modules in list
    ModuInfo               astModuleInfo[MAX_MODULE_NUM];            ///< @~chinese 模块信息列表 @~english Information details of a module
    unsigned int           nReserved[4];                             ///< @~chinese 保留字段 @~english Reserved
} ModuleInfoList;

/// @~chinese Group模块信息结构 @~english Group module information structure
typedef struct GroupModuInfo
{
    unsigned int    nModuleID;                                      ///< @~chinese 模块ID @~english Module ID
    char            strDisplayName[MAX_MODULE_DISPLAY_NAME_LENGTH]; ///< @~chinese 界面显示名称 @~english Displayed module name
    char            strModuleName[MAX_MODULE_NAME_LENGTH];          ///< @~chinese 模块名称 @~english Module name
    unsigned int    nReserved[128];                                 ///< @~chinese 保留字段 @~english Reserved
} GroupModuInfo;

/// @~chinese Group模块信息列表结构 @~english Group module information list structure
typedef struct GroupModuInfoList__
{
    unsigned int           nNum;                                     ///< @~chinese 模块个数 @~english Number of modules in list
    GroupModuInfo          astModuleInfo[MAX_MODULE_NUM];            ///< @~chinese 模块信息列表 @~english Information details of a module
    unsigned int           nReserved[4];                             ///< @~chinese 保留字段 @~english Reserved
} GroupModuInfoList;

/// 用户自定义流程运行策略信息结构
typedef struct ProcedureRunPolicy__
{
    int          nThreadNum;              ///< 指定流程其执行线程数目
    char         chCpuCore[MAX_CORE_NUM]; ///< 指定流程运行的CPU核心 CPU核数组，0-无效，1-有效。chCpuCore[0]表示cpu0、chCpuCore[1]表示cpu1, chCpuCore[2]表示cpu2...
    unsigned int nReserved[4];            ///< 保留字段
}ProcedureRunPolicy;

/// @~chinese 模块导入数据信息结构 @~english Module import data structure
typedef struct ModuleImportData__
{
    char *             pData;                 ///< @~chinese 导入文件绝对路径 @~english The imported file's path
    unsigned int       nReserved[4];          ///< @~chinese 保留字段 @~english Reserved
}ModuleImportData;

/// @~chinese 输出算法平台信息 @~english Platform information structure
typedef struct OutputPlatformInfo__
{
    unsigned int  nInfoType;                                ///< @~chinese 回调信息类型 @~english information type
    unsigned int  nDataSize;                                ///< @~chinese 回调数据分配内存 @~english Data memory size
    unsigned int  nDataLen;                                 ///< @~chinese 回调数据实际长度 @~english Data length
    void *        pData;                                    ///< @~chinese 回调数据 @~english Data
    unsigned int  nReserved[4];                             ///< @~chinese 保留字段 @~english Reserved
} OutputPlatformInfo;

/// 单个动态IO信息结构
typedef struct DynamicIOInfoItem__
{
    unsigned int     nIOType;                   ///< 动态IO类型：1-整型；2-浮点型；3-字符串型；4-图像型；5-大数据型
    char chIOName[MAX_OBJECT_IO_NAME_LENGTH];   ///< 动态IO名称
}DynamicIOInfoItem;

/// 动态IO列表信息结构
typedef struct DynamicIOInfoList__
{
    unsigned int      nIONum;                                 ///< 动态IO数量
    DynamicIOInfoItem astIOInfo[MAX_OBJECT_IO_NUM];           ///< 动态IO集合
    unsigned int      nReserved[4];                           ///< 保留字段
}DynamicIOInfoList;

/// @~chinese 模块运行错误信息 @~english Module Run Error Info
typedef struct ModuRunErrorInfo__
{
    unsigned int nModuleID;                                     ///< @~chinese 模块ID @~english Module ID
    int nErrorCode;                                             ///< @~chinese 错误码 @~english Error Code
    char strModuleName[MAX_MODULE_NAME_LENGTH];                 ///< @~chinese 模块名称 @~english Module Name
    char strDisplayName[MAX_MODULE_DISPLAY_NAME_LENGTH];        ///< @~chinese 界面显示名称 @~english Displayed Module Name
    unsigned char cReserved[16];                                ///< @~chinese 保留字段 @~english Reserved
} ModuRunErrorInfo;

/// @~chinese 模块运行错误信息列表 @~english Module Run Error Info List
typedef struct ModuRunErrorInfoList__
{
    unsigned int nNum;                                          ///< @~chinese 错误信息数量 @~english Error Info Num
    ModuRunErrorInfo stModuErrorInfo[MAX_MODULE_RUN_ERROR_INFO_NUM]; ///< @~chinese 错误信息 @~english Error Info
    unsigned char cReserved[16];                                ///< @~chinese 保留字段 @~english Reserved
} ModuRunErrorInfoList;

/// @~chinese 模块结果内存管理类型   @~english Module Result Memory Type
typedef enum _ModuResultMemoryTypeEnum
{
    /// @~chinese 内存浅拷贝   @~english Memory Shallow Copy
    Memory_Shallow_Copy = 1,

    /// @~chinese 内存深拷贝   @~english Memory Deep Copy
    Memory_Deep_Copy = 2,

}ModuResultMemoryTypeEnum;

/// @}
#endif    ///< _IVM_EXPORT_H__
