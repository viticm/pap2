////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DSceneLayer.h
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2006-10-27 12:01:19
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DSCENELAYER_H_
#define _INCLUDE_IEKG3DSCENELAYER_H_

////////////////////////////////////////////////////////////////////////////////

interface IEKG3DSceneLayer
{
    virtual HRESULT GetName(LPCTSTR *ppcszRetName) = 0;
    virtual HRESULT SetName(LPCTSTR pcszName) = 0;

    virtual HRESULT GetID(DWORD *pdwRetID) = 0;
    virtual HRESULT SetID(DWORD dwID) = 0;

    virtual HRESULT GetSelectable(BOOL *pbRetSelectable) = 0;
    virtual HRESULT SetSelectable(BOOL bSelectable) = 0;

    virtual HRESULT GetVisible(BOOL *pbRetVisible) = 0;
    virtual HRESULT SetVisible(BOOL bVisiable) = 0;

    virtual HRESULT GetChildLayerCount(DWORD *pdwRetChildLayerCount) = 0;
    virtual HRESULT GetChildLayer(
        DWORD dwChildLayerIndex, IEKG3DSceneLayer **ppiRetChildLayer
    ) = 0;
};


#endif //_INCLUDE_IEKG3DSCENELAYER_H_
