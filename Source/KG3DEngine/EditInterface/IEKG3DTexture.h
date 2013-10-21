////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DTexture.h
//  Version     : 1.0
//  Creator     : Xia Yong
//  Create Date : 2006-11-8 15:59:18
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DTEXTURE_H_
#define _INCLUDE_IEKG3DTEXTURE_H_

////////////////////////////////////////////////////////////////////////////////
class IKG3DTexture;
interface IEKG3DTexture : public IKG3DTexture
{
    virtual HRESULT GetName(LPCTSTR *ppszName) = 0;
    virtual HRESULT GetID(DWORD *pdwID) = 0;
    virtual HRESULT CheckLoadState() = 0;
    virtual HRESULT SetTexture(LPDIRECT3DDEVICE9 pDevice, INT i) = 0;
    virtual HRESULT ForceReload() = 0;
};
#endif //_INCLUDE_IEKG3DTEXTURE_H_
