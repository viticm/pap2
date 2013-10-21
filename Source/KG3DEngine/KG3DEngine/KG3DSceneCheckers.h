////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneCheckers.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-11-17 17:50:28
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSCENECHECKERS_H_
#define _INCLUDE_KG3DSCENECHECKERS_H_

////////////////////////////////////////////////////////////////////////////////
#include "KG3DCommonObject.h"
#include "IEKG3DSceneCheckers.h"

struct KG3DNormalChekerShaderArg 
{
	DWORD dwType;
	LPCTSTR strPath;//±ØÐëÊÇstatic×Ö·ûÖ¸Õë
};

struct KG3DSceneCheckers : public KG3DCommonObjectSimple, public IEKG3DSceneCheckers
{
	STDMETHOD_(ULONG, GetType)() = 0;	
	STDMETHOD_(LPVOID, ToAnotherInterface)(DWORD_PTR Param) = 0;

	virtual HRESULT EnableCheckNormal(BOOL bEnable) = 0;
	virtual ULONG IsCheckNormalEnable() = 0;
	virtual HRESULT InitNormalCheckerShaders(KG3DNormalChekerShaderArg Args[], size_t ArgsCount) = 0;
};
KG3DSceneCheckers& g_GetSceneCheckers();

#endif //_INCLUDE_KG3DSCENECHECKERS_H_
