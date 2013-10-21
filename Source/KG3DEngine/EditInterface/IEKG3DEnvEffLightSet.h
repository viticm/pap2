////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DEnvEffLightSet.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-11-22 14:10:34
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DENVEFFLIGHTSET_H_
#define _INCLUDE_IEKG3DENVEFFLIGHTSET_H_

#include "IEKG3DEnvEffLight.h"
////////////////////////////////////////////////////////////////////////////////
typedef enum _KG3DENVEFFLIGHTSET_MDO
{
	KG3DENVEFFLIGHTSET_MDO_ALL = 0xff,
	KG3DENVEFFLIGHTSET_MDO_AMBIENT = 0x01,
}KG3DENVEFFLIGHTSET_MDO;
typedef struct _KG3DENVEFFLIGHTSET_DESCSTRUCT
{
	DWORD m_dwModificationOption;
	D3DCOLOR	m_dwSceneAmbient;
#ifdef __cplusplus
	_KG3DENVEFFLIGHTSET_DESCSTRUCT()
		:m_dwSceneAmbient(0)
	{

	}
#endif
}KG3DENVEFFLIGHTSET_DESCSTRUCT;
struct IEKG3DEnvEffLightSet 
{
	virtual HRESULT Add(IEKG3DEnvEffLight** pRetLight, INT* pIndex) = 0;
	virtual HRESULT Del(INT nIndex) = 0;
	virtual HRESULT Clear() = 0;
	virtual HRESULT Size(INT* pSize) = 0;
	virtual HRESULT Get(INT nIndex, IEKG3DEnvEffLight** pRetLight) = 0;
	virtual HRESULT RequestModification(KG3DENVEFFLIGHTSET_DESCSTRUCT* pStruct) = 0;
	virtual HRESULT GetStruct(KG3DENVEFFLIGHTSET_DESCSTRUCT* pStruct) = 0;
};
typedef struct IEKG3DEnvEffLightSet IEKG3DLightSet;

#endif //_INCLUDE_IEKG3DENVEFFLIGHTSET_H_
