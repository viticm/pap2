////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRepresentObjectBuilder.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-4-3 15:42:31
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KG3DRepresentObjectBuilder.h"
#include "KG3DRepresentObjectTable.h"
#include "KG3DClip.h"
#include "KG3DClipTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
////////////////////////////////////////////////////////////////////////////////


HRESULT KG3DRepresentObjectBuilder::Build( LPCTSTR lpFileName, DWORD dwType, DWORD_PTR wParam, DWORD_PTR lParam, KG3DRepresentObject** ppRet )
{
	KG3DRepresentObject* pObj = NULL;
	KG_PROCESS_ERROR(ppRet != NULL);
	{
		//根据FileName和Type判断创建类型
		if (NULL != lpFileName)
		{
			TypeInfo* pInfo;
			HRESULT hr = g_GetObjectTable().GetTypeInfoByFileName(&pInfo, lpFileName);
			if(SUCCEEDED(hr))
				dwType = pInfo->dwType;
		}

		HRESULT hr = New(dwType, wParam, lParam, &pObj);
		KG_COM_PROCESS_ERROR(hr);

		hr = pObj->Load(lpFileName, wParam, lParam);
		KG_COM_PROCESS_ERROR(hr);

		pObj->UpdateTransformation();
	}

	_ASSERTE(NULL != pObj);
	*ppRet = pObj;
	return S_OK;
Exit0:
	SAFE_RELEASE(pObj);
	
	return E_FAIL;
}

HRESULT KG3DRepresentObjectBuilder::Build( LPCTSTR lpFileName, DWORD dwType, DWORD_PTR wParam, DWORD_PTR lParam, IEKG3DRepresentObject** ppRet )
{
	KG3DRepresentObject* p = NULL;
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(ppRet);
	hr = Build(lpFileName, dwType, wParam, lParam, &p);
	if (SUCCEEDED(hr))
	{
		*ppRet = p;//UpCast
	}
Exit0:
	return hr;
}
HRESULT KG3DRepresentObjectBuilder::New( DWORD dwType, DWORD_PTR wParam, DWORD_PTR lParam, KG3DRepresentObject** ppRet )
{
	return g_GetObjectTable().Get1NewResource(ppRet, dwType, 0);
}