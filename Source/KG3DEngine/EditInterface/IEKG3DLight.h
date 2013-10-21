////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DLight.h
//  Version     : 1.0
//  Creator     : Xia Yong
//  Create Date : 2006-12-6 10:54:55
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DLIGHT_H_
#define _INCLUDE_IEKG3DLIGHT_H_
#include "IEKG3DEnvEffLight.h"
////////////////////////////////////////////////////////////////////////////////
/*// Last modify by Chen Tianhong: 2007-11-29 9:35:02//所有功能被移动到了IEKG3DEnvEffLight，后者占据前者的名字
interface IEKG3DLight
{
    virtual HRESULT GetLight(D3DLIGHT9 **ppLight) = 0;
    virtual HRESULT GetLightEnableState(BOOL *pEnable) = 0;
    virtual HRESULT EnableLight(BOOL bEnable) = 0;
    virtual HRESULT InitDefault() = 0;
    virtual DWORD*  GetSceneAmbientPtr() = 0;
    virtual HRESULT GetSceneAmbient(DWORD *pdwSceneAmbient) = 0;
	virtual HRESULT SetSceneAmbient(DWORD dwSceneAmbient) = 0;
    virtual HRESULT LoadFromFile(LPCTSTR pszFileName) = 0;
	virtual void SetDiffuse(DWORD dwColor) = 0;
	virtual void SetAmbient(DWORD dwColor) = 0;

    virtual D3DCOLORVALUE* GetSkyLight() = 0;
    virtual D3DCOLORVALUE* GetComLight() = 0;

};
*/
#endif //_INCLUDE_IEKG3DLIGHT_H_
