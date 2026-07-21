/***************************************************************************************************
 *
 * 版权信息：版权所有 (c) 2017, 保留所有权利
 *
 * 文件名称：iMVS-6000PlatformSDKC_IN.h
 * 摘    要: 二次开发SDK C接口头文件
 *
 * 作    者: 视觉平台组
 * 日    期：2019-05-18 16:46:42
 * 备    注：新建
 ****************************************************************************************************/
#ifndef IMVS_6000_PLATFORMSDKC_IN_H__ // 2019-05-18 16:46:42
#define IMVS_6000_PLATFORMSDKC_IN_H__

#include "iMVS-6000SDK4ServerDefine.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @fn     IMVS_QueryModuleHistoryResult
*  @brief  查询模块历史结果
*  @param  handle                  [IN]     操作句柄
*  @param  nModuleID               [IN]     模块ID
*  @param  nIndex                  [IN]     索引
*  @param  nResultNum              [IN]     结果数量（最大40条）
*  @param  pstQueryModuResInfoList [OUT]    查询模块历史结果信息列表
*  @return 成功,返回IMVS_EC_OK;失败,返回错误码
*/
IMVS_6000_PLATFORMSDK_API int __stdcall IMVS_QueryModuleHistoryResult(
	IN const void * const handle,
	IN const unsigned int nModuleID,
	IN const int nIndex,
	IN const unsigned int nRequestNum,
	OUT IMVS_QUERY_MODULE_RES_INFO_LIST *& pstQueryModuResInfoList,
	IN const bool bResBuffFirst = true,
	IN const bool bNeedImage = true,
	IN const bool bUseResStructMap = true,
	IN const bool bQueryByRunCount = false,
	IN const unsigned int nRunCount = 0);

#ifdef __cplusplus
}
#endif 

#endif    // IMVS_6000_PLATFORMSDKC_IN_H__
