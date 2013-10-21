////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneLayer.cpp
//  Version     : 1.0
//  Creator     : YangLin, Wu Caizhong
//  Create Date : 2006-11-01 11:23:42
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DSceneLayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
KG3DSceneLayer::KG3DSceneLayer(LPCSTR szName, DWORD dwID):
    m_sName(szName),
    m_dwID(dwID),
    m_pParentLayer(NULL),
    m_bVisible(TRUE),
    m_bSelectable(TRUE)
{
}

KG3DSceneLayer::~KG3DSceneLayer(void)
{
	m_vecChildLayer.clear();
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneLayer::GetName(LPCTSTR *ppcszRetName)
{
    HRESULT hResutl = E_FAIL;

    KGLOG_PROCESS_ERROR(ppcszRetName);

    *ppcszRetName = m_sName.c_str();

    hResutl = S_OK;
Exit0:
    return hResutl;
}
HRESULT KG3DSceneLayer::SetName(LPCTSTR pcszName)
{
    HRESULT hResutl = E_FAIL;

    m_sName = pcszName;

    hResutl = S_OK;
//Exit0:
    return hResutl;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneLayer::GetID(DWORD *pdwRetID)
{
    HRESULT hResutl = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwRetID);

    *pdwRetID = m_dwID;

    hResutl = S_OK;
Exit0:
    return hResutl;
}

HRESULT KG3DSceneLayer::SetID(DWORD dwID)
{
    HRESULT hResutl = E_FAIL;

    m_dwID = dwID;

    hResutl = S_OK;
//Exit0:
    return hResutl;
}


////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneLayer::GetSelectable(BOOL *pbRetSelectable)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pbRetSelectable);

    *pbRetSelectable = m_bSelectable;
    
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneLayer::SetSelectable(BOOL bSelectable)
{
    HRESULT hResutl = E_FAIL;

    m_bSelectable = bSelectable;

    hResutl = S_OK;
//Exit0:
    return hResutl;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneLayer::GetVisible(BOOL *pbRetVisible)
{
    HRESULT hResutl = E_FAIL;

    KGLOG_PROCESS_ERROR(pbRetVisible);

    *pbRetVisible = m_bVisible;

    hResutl = S_OK;
Exit0:
    return hResutl;
}

HRESULT KG3DSceneLayer::SetVisible(BOOL bVisiable)
{
    HRESULT hResutl = E_FAIL;

    m_bVisible = bVisiable;

    hResutl = S_OK;
//Exit0:
    return hResutl;
}


////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneLayer::GetChildLayerCount(DWORD *pdwRetChildLayerCount)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwRetChildLayerCount);

    *pdwRetChildLayerCount = (DWORD)m_vecChildLayer.size();

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneLayer::GetChildLayer(
    DWORD dwChildLayerIndex, IEKG3DSceneLayer **ppiRetChildLayer
)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(dwChildLayerIndex < m_vecChildLayer.size());

    *ppiRetChildLayer = m_vecChildLayer[dwChildLayerIndex];

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneLayer::AddChildLayer(KG3DSceneLayer *pChildLayer)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(pChildLayer);

    pChildLayer->m_pParentLayer = this;

	m_vecChildLayer.push_back(pChildLayer);
//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneLayer::DeleteChildLayer(KG3DSceneLayer* pChildLayer)
{
    HRESULT hResult  = E_FAIL;
    KG3DSceneLayer *pLayer = NULL;
    vector<KG3DSceneLayer*>::iterator i;

    i = m_vecChildLayer.begin();
    while (i != m_vecChildLayer.end())
    {
        pLayer = *i;
        if(pLayer == pChildLayer)
        {
            pChildLayer->m_pParentLayer = NULL;
            m_vecChildLayer.erase(i);
            KG_PROCESS_SUCCESS(true);
        }
        ++i;
    }
    KGLOG_PROCESS_ERROR(false && "Can't found the child layer");
Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneLayer::GetParentLayer(KG3DSceneLayer **ppRetParentLayer)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(ppRetParentLayer);

    *ppRetParentLayer = m_pParentLayer;

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneLayer::ToggleVisible()
{
    HRESULT hResult  = E_FAIL;


//Exit1:
    hResult = S_OK;
//Exit0:
    return hResult;
}

HRESULT KG3DSceneLayer::ToggleSelectable(KG3DSceneEditorBase* pEdior)
{
    HRESULT hResult  = E_FAIL;
    
    KGLOG_PROCESS_ERROR(pEdior);

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}
