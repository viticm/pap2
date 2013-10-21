////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneLayer.h
//  Version     : 1.0
//  Creator     : YangLin, Wu Caizhong
//  Create Date : 2006-11-01 11:19:33
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSCENELAYER_H_
#define _INCLUDE_KG3DSCENELAYER_H_

#include "IEKG3DSceneLayer.h"

class KG3DSceneEditorBase;

////////////////////////////////////////////////////////////////////////////////
class KG3DSceneLayer : public IEKG3DSceneLayer
{
public:    // for interface
    virtual HRESULT GetName(LPCTSTR *ppcszRetName);
    virtual HRESULT SetName(LPCTSTR pcszName);

    virtual HRESULT GetID(DWORD *pdwRetID);
    virtual HRESULT SetID(DWORD dwID);

    virtual HRESULT GetSelectable(BOOL *pbRetSelectable);
    virtual HRESULT SetSelectable(BOOL bSelectable);

    virtual HRESULT GetVisible(BOOL *pbRetVisible);
    virtual HRESULT SetVisible(BOOL bVisiable);

    virtual HRESULT GetChildLayerCount(DWORD *pdwRetChildLayerCount);
    virtual HRESULT GetChildLayer(
        DWORD dwChildLayerIndex, IEKG3DSceneLayer **ppiRetChildLayer
    );


public: // for engine internal use
    KG3DSceneLayer(LPCSTR szName, DWORD dwID);
    virtual ~KG3DSceneLayer(void);

    HRESULT GetParentLayer(KG3DSceneLayer **ppRetParentLayer);

    HRESULT AddChildLayer(KG3DSceneLayer *pChildLayer);
	HRESULT DeleteChildLayer(KG3DSceneLayer *pChildLayer);

    HRESULT ToggleVisible();
    HRESULT ToggleSelectable(KG3DSceneEditorBase* pEdior);


    string  m_sName;
    DWORD   m_dwID;
    BOOL    m_bSelectable;
    BOOL    m_bVisible;
    KG3DSceneLayer *m_pParentLayer;
    vector<KG3DSceneLayer *>m_vecChildLayer;
};

#endif //_INCLUDE_KG3DSCENELAYER_H_
