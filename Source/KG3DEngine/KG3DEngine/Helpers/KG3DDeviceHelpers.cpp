////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DDeviceHelpers.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-11-25 14:08:35
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DDeviceHelpers.h"


////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HRESULT D3DXDeviceCreateVertexBufferByData(LPVOID pData, DWORD dwNumByteDataPerVertex, DWORD dwNumVertexs, DWORD dwNumBytePerVertex , DWORD Usage
										   , DWORD FVF, D3DPOOL Pool, LPDIRECT3DDEVICE9 pDevice, LPDIRECT3DVERTEXBUFFER9* ppVB, HANDLE* pSharedHandle )
{
	LPDIRECT3DVERTEXBUFFER9 pBuffer = NULL;
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(NULL != pData && NULL != pDevice && NULL != ppVB && dwNumByteDataPerVertex <= dwNumBytePerVertex);
	{
		UINT dwVBLength = dwNumVertexs * dwNumBytePerVertex;

		hr = pDevice->CreateVertexBuffer(dwVBLength, Usage, FVF, Pool, &pBuffer, pSharedHandle);
		KG_COM_PROCESS_ERROR(hr);

		BYTE* pVertices = NULL;
		hr = pBuffer->Lock(0, dwVBLength, (LPVOID*)&pVertices, 0);
		KG_COM_PROCESS_ERROR(hr);

		_ASSERTE(sizeof(BYTE) == 1);
		BYTE* pVBEnd = pVertices + dwVBLength;

		if (dwNumByteDataPerVertex == dwNumBytePerVertex)
		{
			memcpy_s(pVertices, dwVBLength, pData, dwVBLength);
		}
		else
		{
			BYTE* pDataBytes = (BYTE*)pData;
			for (DWORD i = 0; i < dwNumVertexs; ++i)
			{
				memcpy_s(pVertices, dwNumBytePerVertex, pDataBytes, dwNumByteDataPerVertex);
				pDataBytes += dwNumByteDataPerVertex;
				pVertices += dwNumBytePerVertex;
			}
			_ASSERTE(pVertices <= pVBEnd);
			KG_USE_ARGUMENT(pVBEnd);
		}

		pBuffer->Unlock();
	}
	*ppVB = pBuffer;
	return S_OK;
Exit0:
	SAFE_RELEASE(pBuffer);
	return E_FAIL;
}

HRESULT D3DXDeviceCreateIndexBufferByData( LPVOID pData, DWORD dwNumIBData, DWORD Usage, BOOL Is32Bit, D3DPOOL Pool
										  , LPDIRECT3DDEVICE9 pDevice,  LPDIRECT3DINDEXBUFFER9* ppIB, HANDLE* pSharedHandle )
{
	_ASSERTE(sizeof(BYTE) == 1 && sizeof(DWORD) == 4 && sizeof(WORD) == 2);
	HRESULT hr = E_FAIL;
	LPDIRECT3DINDEXBUFFER9 pIB = NULL;
	KG_PROCESS_ERROR(NULL != pData && NULL != pDevice && NULL != ppIB);
	{
		D3DFORMAT format = Is32Bit ? D3DFMT_INDEX32 : D3DFMT_INDEX16;
		DWORD dwLength = Is32Bit ? (dwNumIBData * 4) : (dwNumIBData * 2);
		hr = pDevice->CreateIndexBuffer(dwLength, Usage, format, Pool, &pIB, pSharedHandle);
		KG_COM_PROCESS_ERROR(hr);

		LPVOID pBuffer = NULL;
		hr = pIB->Lock(0, dwLength, (LPVOID*)&pBuffer, 0);
		KG_COM_PROCESS_ERROR(hr);

		memcpy_s(pBuffer, dwLength, pData, dwLength);

		pIB->Unlock();
	}
	*ppIB = pIB;
	return S_OK;
Exit0:
	SAFE_RELEASE(pIB);
	return E_FAIL;
}