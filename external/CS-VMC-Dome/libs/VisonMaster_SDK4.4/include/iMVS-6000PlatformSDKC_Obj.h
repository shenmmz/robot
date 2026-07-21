/***************************************************************************************************
 *
 * 版权信息：版权所有 (c) 2017, 保留所有权利
 *
 * 文件名称：iMVS-6000PlatformSDKC_Obj.h
 * 摘    要: 二次开发SDK C接口头文件
 *
 * 作    者: 视觉平台组
 * 日    期：2020-01-14 16:59:32
 * 备    注：新建
 ****************************************************************************************************/
#ifndef IMVS_6000_PLATFORMSDKC_OBJ_H__ // 2020-01-14 16:59:32
#define IMVS_6000_PLATFORMSDKC_OBJ_H__

#include "iMVS-6000SDK4ServerDefine.h"
#include "iMVS-6000PlatformSDKDefine.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @fn     IMVS_SyncSelfExecuteOneModule
*  @brief  模块同步自执行:该模块利用旧的输入重新进行一次算法,将结果发给界面(不发给后继模块)
*  @param  nModuleId               [IN]    模块Id
*  @param  handle                  [IN]    句柄
*  @return 成功,返回IMVS_OK;失败,返回错误码
*/
IMVS_6000_PLATFORMSDK_API int __stdcall IMVS_SyncSelfExecuteOneModule(IN const void * const handle, IN const unsigned int nModuleId, IN bool bIsRespondToUI = true);

#ifdef __cplusplus
}
#endif 

#endif    // IMVS_6000_PLATFORMSDKC_OBJ_H__