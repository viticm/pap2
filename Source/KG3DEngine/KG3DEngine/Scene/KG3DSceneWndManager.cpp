#include "StdAfx.h"
#include ".\kg3dscenewndmanager.h"
#include "KG3DGraphicsTool.h"

//---Add by FengBo 2007-9-5--------------
#include "KG3DPostRenderEffectBloom.h"
#include "KG3DPostRenderEffectShockWave.h"
//-----------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KG3DSceneWndManager::KG3DSceneWndManager(KG3DScene* pScene)
{
	m_nIDCount = 0;
	m_lpCurOutputWnd =  NULL;
	m_lpPrimaryWindow = NULL;
	m_lpParentScene = pScene;
}

KG3DSceneWndManager::~KG3DSceneWndManager(void)
{
	list<KG3DSceneOutputWnd*>::iterator i=m_listOutputWindows.begin();
	while(i!=m_listOutputWindows.end())
	{
		KG3DSceneOutputWnd* pWnd = *i;
		SAFE_DELETE(pWnd);
		i++;
	}
	m_listOutputWindows.clear();
}

int KG3DSceneWndManager::AddOutputWindow(TCHAR szName[],HWND hWnd, DWORD dwType,DWORD_PTR dwOption)
{
	KG3DSceneOutputWnd* pWnd = new KG3DSceneOutputWnd( this );
	
	pWnd->m_hWnd = hWnd;
	if(pWnd->m_hWnd)
	{
		::GetClientRect(hWnd,&pWnd->m_Rect);
	}
	pWnd->SetType(dwType);
	pWnd->SetOption(dwOption);///Option要在Type后面设，因为要根据Type决定Option究竟是个什么东西

	pWnd->m_Viewport.X = 0;
	pWnd->m_Viewport.Y = 0;
	pWnd->m_Viewport.Width  = pWnd->m_Rect.right - pWnd->m_Rect.left;
	pWnd->m_Viewport.Height = pWnd->m_Rect.bottom - pWnd->m_Rect.top;
	pWnd->m_Viewport.MinZ = 0.0F;
	pWnd->m_Viewport.MaxZ = 1.0F;
	pWnd->m_bNeedRender = TRUE;
	pWnd->m_bEnableOutput = TRUE;

	wsprintf(pWnd->m_szName,szName);
	pWnd->m_nID = m_nIDCount++;
	m_listOutputWindows.push_back(pWnd);

	if(!m_lpCurOutputWnd)
	{
		m_lpCurOutputWnd = pWnd;
	}
	if(!m_lpPrimaryWindow)
	{
		m_lpPrimaryWindow = pWnd;
	}
	
	return pWnd->m_nID;
}

HRESULT KG3DSceneWndManager::RemoveOutputWindow(int nID)
{
	list<KG3DSceneOutputWnd*>::iterator i=m_listOutputWindows.begin();
	while(i!=m_listOutputWindows.end())
	{
		KG3DSceneOutputWnd* pWnd = *i;
		if(pWnd->m_nID==nID)
		{
			m_listOutputWindows.erase(i);
			KG3DCamera* pCam = g_cGraphicsTool.GetCamera();
			//释放下面这些乱乱的东西
			if (pCam == &pWnd->GetCamera())
			{
				g_cGraphicsTool.SetCamera(NULL);//释放掉Camera
			}
			/*
			if (pWnd == m_lpSceneEditorMainWnd)
						{
							m_lpSceneEditorMainWnd = NULL;
						}*/
			
			if (pWnd == m_lpPrimaryWindow)
			{
				m_lpPrimaryWindow = NULL;
			}
			if (pWnd == m_lpCurOutputWnd)
			{
				m_lpCurOutputWnd = NULL;
			}
			SAFE_DELETE(pWnd);
			return S_OK;
		}
		i++;
	}
	return E_FAIL;
}

int KG3DSceneWndManager::FindOutputWindow(HWND hWnd)
{
	list<KG3DSceneOutputWnd*>::iterator i=m_listOutputWindows.begin();
	while(i!=m_listOutputWindows.end())
	{
		KG3DSceneOutputWnd* pWnd = *i;
		if(pWnd->m_hWnd == hWnd)
		{
			return pWnd->m_nID;
		}
		i++;
	}
	return -1;
}

int KG3DSceneWndManager::FindOutputWindow(TCHAR szName[])
{
	list<KG3DSceneOutputWnd*>::iterator i=m_listOutputWindows.begin();
	while(i!=m_listOutputWindows.end())
	{
		KG3DSceneOutputWnd* pWnd = *i;
		if(_strcmpi(pWnd->m_szName,szName)==0)
		{
			return pWnd->m_nID;
		}
		i++;
	}
	return -1;
}


HRESULT KG3DSceneWndManager::OnResizeWindow(PRECT const pRect)
{
	list<KG3DSceneOutputWnd*>::iterator i=m_listOutputWindows.begin();
	while(i!=m_listOutputWindows.end())
	{
		KG3DSceneOutputWnd* pWnd = *i;
		pWnd->OnResizeWindow(pRect);
		i++;
	}
	return S_OK;
}

//HRESULT KG3DSceneWndManager::GetPickRay(D3DXVECTOR3 * RayOrig,D3DXVECTOR3* RayDir )
//{
//	if(!m_lpCurOutputWnd)
//		return E_FAIL;
//	return m_lpCurOutputWnd->GetPickRay(RayOrig,RayDir);
//}

HRESULT KG3DSceneWndManager::FindFar_Near_Point(D3DXVECTOR3 &Near,D3DXVECTOR3 &Far)
{
	if(!m_lpCurOutputWnd)
		return E_FAIL;

	return m_lpCurOutputWnd->GetNear_Far_Point(Near,Far);
}

IEKG3DSceneOutputWnd* KG3DSceneWndManager::GetForceWindow()
{
    list<KG3DSceneOutputWnd*>::iterator i=m_listOutputWindows.begin();

    while (i != m_listOutputWindows.end())
    {
        if ((*i)->m_hWnd == ::GetFocus())
            return (*i);
        i++;
    }

    return NULL;
}

HRESULT KG3DSceneWndManager::FrameMove()
{
    /*
    list<KG3DSceneOutputWnd*>::iterator i=m_listOutputWindows.begin();
    while(i!=m_listOutputWindows.end())
    {   
        (*i)->GetCamera().ClearOffset();
        i++;
    } */
    return S_OK;
}

IEKG3DSceneOutputWnd* KG3DSceneWndManager::GetOutputWindow(int nID)
{
	list<KG3DSceneOutputWnd*>::iterator i=m_listOutputWindows.begin();
	while(i!=m_listOutputWindows.end())
	{
		KG3DSceneOutputWnd* pWnd = *i;
		if(pWnd->m_nID == nID)
			return pWnd;
		i++;
	}
	return NULL;
}

HRESULT KG3DSceneWndManager::SetPrimaryWindow(int nID)
{
	KG3DSceneOutputWnd* pWnd = static_cast<KG3DSceneOutputWnd *>(GetOutputWindow(nID));
	if(pWnd)
	{
		m_lpPrimaryWindow = pWnd;
	}
	return S_OK;
}

HRESULT KG3DSceneWndManager::SetCurrentOutputWindow(int nID)
{
    KG3DSceneOutputWnd* pWnd = static_cast<KG3DSceneOutputWnd *>(GetOutputWindow(nID));
	if(pWnd)
	{
		m_lpCurOutputWnd = pWnd;
	}
	return S_OK;
}
HRESULT KG3DSceneWndManager::GetPrimaryWindow(IEKG3DSceneOutputWnd **ppiRetPrimaryWindow)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(ppiRetPrimaryWindow);

    *ppiRetPrimaryWindow = m_lpPrimaryWindow;

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}
HRESULT KG3DSceneWndManager:: GetCurOutputWnd(IEKG3DSceneOutputWnd **ppiRetCurOutputWnd)
{
    HRESULT hResult  = E_FAIL;

    KG_PROCESS_ERROR(NULL != ppiRetCurOutputWnd && NULL != m_lpCurOutputWnd);

    *ppiRetCurOutputWnd = m_lpCurOutputWnd;

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}