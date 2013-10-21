#include "StdAfx.h"
#include "KG3DOutputWindowTable.h"
#include "KG3DGraphicsTool.h"
#define MAX_NUM_OF_OUTPUTWINDSTABLE 3

HRESULT KG3DOutputWindowTable::Init()
{
	m_WindowTable.clear();
	return S_OK;
}

HRESULT KG3DOutputWindowTable::UnInit()
{
	std::map<HWND,KG3DSceneOutputWnd*>::iterator i = m_WindowTable.begin();
	while(i != m_WindowTable.end())
	{
		KG3DSceneOutputWnd* pWnd = (*i).second;
		m_WindowTable.erase(i);
		KG3DCamera* pCam = g_cGraphicsTool.GetCamera();
		if (pCam == &pWnd->GetCamera())
		{
			g_cGraphicsTool.SetCamera(NULL);// Õ∑≈µÙCamera
		}
		SAFE_DELETE(pWnd);
		i++;
	}
	return S_OK;
}


HRESULT KG3DOutputWindowTable::Assign(KG3DSceneOutputWnd* pOutputWnd,HWND hWnd)
{
	
	std::map<HWND,KG3DSceneOutputWnd*>::iterator it = m_WindowTable.find(hWnd);
	if (it == m_WindowTable.end())
	{
		pOutputWnd = NULL;
	}
	else
	{
		pOutputWnd = (*it).second;
		m_WindowTable.erase(it);
	}

	return S_OK;
}

HRESULT KG3DOutputWindowTable::Recycle(KG3DSceneOutputWnd* pOutputWnd)
{
	if (m_WindowTable.size() > MAX_NUM_OF_OUTPUTWINDSTABLE)
	{
		std::map<HWND,KG3DSceneOutputWnd*>::iterator it = m_WindowTable.begin();
		int nDel = MAX_NUM_OF_OUTPUTWINDSTABLE;
		while(it != m_WindowTable.end() && nDel > 0)
		{
			KG3DSceneOutputWnd* pWnd = (*it).second;
			m_WindowTable.erase(it);
			KG3DCamera* pCam = g_cGraphicsTool.GetCamera();
			if (pCam == &pWnd->GetCamera())
			{
				g_cGraphicsTool.SetCamera(NULL);// Õ∑≈µÙCamera
			}
			SAFE_DELETE(pWnd);
			it++;
			nDel--;
		}
	}
	HWND hWnd = pOutputWnd->m_hWnd;
	std::map<HWND,KG3DSceneOutputWnd*>::iterator it = m_WindowTable.find(hWnd);
	if (it != m_WindowTable.end())
	{
		KG3DSceneOutputWnd* pWnd = (*it).second;
		m_WindowTable.erase(it);
		KG3DCamera* pCam = g_cGraphicsTool.GetCamera();
		if (pCam == &pWnd->GetCamera())
		{
			g_cGraphicsTool.SetCamera(NULL);// Õ∑≈µÙCamera
		}
		SAFE_DELETE(pWnd);
	}
	m_WindowTable.insert(std::map<HWND,KG3DSceneOutputWnd*>::value_type(hWnd,pOutputWnd));
	return S_OK;
}