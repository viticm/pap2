////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DSceneCheckers.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-11-17 17:46:57
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DSCENECHECKERS_H_
#define _INCLUDE_IEKG3DSCENECHECKERS_H_

////////////////////////////////////////////////////////////////////////////////
interface IEKG3DSceneCheckers
{
	virtual HRESULT EnableCheckNormal(BOOL bEnable) = 0;
	virtual ULONG IsCheckNormalEnable() = 0;
};

#endif //_INCLUDE_IEKG3DSCENECHECKERS_H_
