// Game.cpp : Implementation of CGame

#include "stdafx.h"
#include "Game.h"


// CGame
STDMETHODIMP CGame::GetSystemVersion(BSTR* bstrVersion)
{
    // TODO: Add your implementation code here
    CString strVerion;

    HRESULT hResult = m_SystemInfo.GetSystemVersion(strVerion);

    if (SUCCEEDED(hResult))
    {
        (*bstrVersion) = strVerion.AllocSysString();
    }

    return hResult;
}

STDMETHODIMP CGame::GetSystemMemInfo(BSTR* bstrMem)
{
    // TODO: Add your implementation code here
    CString strMemInfo;

    HRESULT hResult = m_SystemInfo.GetSystemMemInfo(strMemInfo);

    if (SUCCEEDED(hResult))
    {
        (*bstrMem) = strMemInfo.AllocSysString();
    }

    return hResult;
}

STDMETHODIMP CGame::GetSystemCpuInfo(BSTR* bstrCpu)
{
    // TODO: Add your implementation code here
    CString strCpuInfo;

    HRESULT hResult = m_SystemInfo.GetSystemCpuInfo(strCpuInfo);

    if (SUCCEEDED(hResult))
    {
        (*bstrCpu) = strCpuInfo.AllocSysString();
    }

    return hResult;
}

STDMETHODIMP CGame::GetDisplayInfo(BSTR* bstrDisplay)
{
	// TODO: Add your implementation code here
	CString strDisplayInfo;

	HRESULT hResult = m_SystemInfo.GetDisplayInfo(strDisplayInfo);

	if (SUCCEEDED(hResult))
	{
		(*bstrDisplay) = strDisplayInfo.AllocSysString();
	}

	return hResult;
}

STDMETHODIMP CGame::GetHardDiskId(BSTR* bstrHardId)
{
	// TODO: Add your implementation code here
	CString strHardDiskId;

	HRESULT hResult = m_SystemInfo.GetHardDiskId(strHardDiskId);

	if (SUCCEEDED(hResult))
	{
		(*bstrHardId) = strHardDiskId.AllocSysString();
	}

	return hResult;
}

STDMETHODIMP CGame::EncryptString(BSTR bstrSrc, BSTR* bstrDest)
{
	// TODO: Add your implementation code here
	CString strSrc(bstrSrc);
	CString strOut;

	HRESULT hResult = m_SystemInfo.EncryptString(strSrc, strOut);

	if (SUCCEEDED(hResult))
	{
		(*bstrDest) = strOut.AllocSysString();
	}

	return hResult;
}
