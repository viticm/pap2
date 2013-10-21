
/**
* @file     SystemInfo.h
* @brief    获取系统信息类
*
* @details  本类获取系统各种信息
* @author   xiaojian <xiaojian@kingsoft.com>
* @date     2008-09-09
* @version	1.0.0.1
*/
#include "stdafx.h"

#pragma once

class CSystemInfo
{
public:
    /** @brief	构造函数 */
    CSystemInfo(void);
    /** @brief	析构函数 */
    ~CSystemInfo(void);

public:
    /**
    * @brief		    获取操作系统版本信息
    *
    * @details
    * 
    * @param[out]       返回操作系统版本字符串
    * @return[S_OK]     成功
    * @return[E_FAILE]  失败
    * @remarks	
    */
    HRESULT GetSystemVersion(CString& strVersion);

    /**
    * @brief		    获取系统内存大小
    *
    * @details
    * 
    * @param[out]       返回内存大小字符串
    * @return[S_OK]     成功
    * @return[E_FAILE]  失败
    * @remarks	        单位kb
    */
    HRESULT GetSystemMemInfo(CString& strMem);

    /**
    * @brief		    获取系统CPU信息
    *
    * @details
    * 
    * @param[out]       返回CPU信息字符串
    * @return[S_OK]     成功
    * @return[E_FAILE]  失败
    * @remarks	        多CPU时,各CPU信息以 '&' 符号分隔
    */
    HRESULT GetSystemCpuInfo(CString& strCpu);

	/*
	* @brief			获取系统显卡信息
	* 
	* @details
	* @param[out]		返回显卡信息
	* @return[S_OK]		成功
	* @return[E_FAILE]	失败
	* @remarks			多显卡时，各显卡信息以 '&' 符号分隔
	*/
	HRESULT GetDisplayInfo(CString& strDisplay);

	/*
	* @brief			获取硬件唯一ID
	*
	* @details
	* @param[out]		硬件id
	* @return[S_OK]		成功
	* @return[E_FAILE]	失败
	* @remarks			MAC地址
	*/
	HRESULT GetHardDiskId(CString& strHardId);

	/*
	* @brief			md5加密
	*
	* @details			直接在需要加密字符串后追加密钥字符串,然后计算md5值
	* @param[in]		需要加密字符串
	* @param[out]		加密后字符串
	* @return[S_OK]		成功
	* @return[E_FAILE]	失败
	* @remarks			加密密钥 : 92A73EB6-FA72-40f9-AFD2-BD0376F40FD0
	*/
	HRESULT EncryptString(const CString strSrc, CString& strOut);

	int GetCpuCoreNum();

private:
	const CString m_strKey;
	int m_nCpuCoreNum;
};
