#include "Stdafx.h"
#include "UiWndMinimap.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"
#include "UiColorManage.h"

KUiWndMinimap::KUiWndMinimap()
{   
	m_pWndWindowCommon = NULL;
}
KUiWndMinimap::~KUiWndMinimap()
{

}

KUiWndWindow* KUiWndMinimap::Create()
{
	KUiWndMinimap* pWnd = new KUiWndMinimap;
	if (pWnd == NULL)
		return NULL;
	return pWnd;
}

int KUiWndMinimap::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
	int nResult  = false;
	int nRetCode = false;

	KG_PROCESS_ERROR(hWnd);
	strcpy(m_szName, pszName);

	m_pWndWindowCommon = KUiWndWindowCommon::Create();
	KG_PROCESS_ERROR(m_pWndWindowCommon);
	nRetCode = m_pWndWindowCommon->Init(hWnd, pRc, pszName);
	KG_PROCESS_ERROR(nRetCode);

	m_pWndWindowCommon->Show(true);

	nResult = true;
Exit0:
	if (!nResult) 
	{
		if (m_pWndWindowCommon != NULL) 
		{
			m_pWndWindowCommon->Release();
			m_pWndWindowCommon = NULL;
		}
	}
	return nResult;
}

void KUiWndMinimap::Release()
{
	if (m_pWndWindowCommon) 
	{
		m_pWndWindowCommon->Release();
		m_pWndWindowCommon = NULL;
	}

	delete this;
}



void KUiWndMinimap::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(pIni && pszName);
	strcpy(m_szName, pszName);

	KG_PROCESS_ERROR(m_pWndWindowCommon); 
	m_pWndWindowCommon->UpdateData(pszName, data, pIni);

Exit0:
	return;
}

void KUiWndMinimap::Show(int bShow, int nSubWnd)
{
	KG_PROCESS_ERROR(m_pWndWindowCommon);

	switch(nSubWnd) 
	{
	case 0:
		m_pWndWindowCommon->Show(true && bShow);
		break;
	default:
		m_pWndWindowCommon->Show(true && bShow);
		break;
	}

Exit0:
	return;
}


void KUiWndMinimap::FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo)
{
	KG_PROCESS_ERROR(pPageInfo);
	pPageInfo->m_PageCount = 1;
	strcpy(pPageInfo->m_szPage[0], "´°¿ÚÊôĞÔ");
Exit0:
	return;
}