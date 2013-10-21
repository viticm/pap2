////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEngineInterfaceGetter.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-11-18 10:29:49
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DENGINEINTERFACEGETTER_H_
#define _INCLUDE_KG3DENGINEINTERFACEGETTER_H_

////////////////////////////////////////////////////////////////////////////////
class KG3DEngineManager;

HRESULT g_GetEngineIEInterface(KG3DEngineManager& Em, DWORD dwType1, DWORD dwType2, DWORD_PTR wParam, DWORD_PTR lParam, LPVOID* ppInterface);
HRESULT g_GetEngineInterface(KG3DEngineManager& Em, DWORD dwType, DWORD_PTR p1, DWORD_PTR p2, LPVOID* ppInterface);

#endif //_INCLUDE_KG3DENGINEINTERFACEGETTER_H_
