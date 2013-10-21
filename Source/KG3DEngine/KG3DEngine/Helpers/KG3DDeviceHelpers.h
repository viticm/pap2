////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DDeviceHelpers.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-11-25 14:08:30
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DDEVICEHELPERS_H_
#define _INCLUDE_KG3DDEVICEHELPERS_H_

////////////////////////////////////////////////////////////////////////////////

HRESULT D3DXDeviceCreateVertexBufferByData(LPVOID pData, DWORD dwNumByteDataPerVertex, DWORD dwNumVertexs, DWORD dwNumBytePerVertex
										   , DWORD Usage, DWORD FVF, D3DPOOL Pool, LPDIRECT3DDEVICE9 pDevice, LPDIRECT3DVERTEXBUFFER9* ppVB, HANDLE* pSharedHandle);


HRESULT D3DXDeviceCreateIndexBufferByData(LPVOID pData, DWORD dwNumIBData, DWORD Usage, BOOL Is32Bit, D3DPOOL Pool
										  , LPDIRECT3DDEVICE9 pDevice, LPDIRECT3DINDEXBUFFER9* ppIB, HANDLE* pSharedHandle);	//根据32Bit还是16Bit，pData也应该是对应的32Bit或者16Bit数据

#endif //_INCLUDE_KG3DDEVICEHELPERS_H_
