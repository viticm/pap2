#include "Stdafx.h"
#include "UiWndPageSet.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"


KUiWndPageSet::KUiWndPageSet()
{
    m_pWndWindowCommon              = NULL;
    m_pWndPageSetCommon             = NULL;
}

KUiWndPageSet::~KUiWndPageSet()
{
}

KUiWndWindow* KUiWndPageSet::Create()
{
	KUiWndPageSet* pWnd = new KUiWndPageSet;
	if (pWnd == NULL)
		return NULL;
	return pWnd;
}

int KUiWndPageSet::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult  = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(hWnd);
    strcpy(m_szName, pszName);

	m_pWndWindowCommon = KUiWndWindowCommon::Create();
    KG_PROCESS_ERROR(m_pWndWindowCommon);
    nRetCode = m_pWndWindowCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

	m_pWndPageSetCommon = KUiWndPageSetCommon::Create();
    KG_PROCESS_ERROR(m_pWndPageSetCommon);
	nRetCode = m_pWndPageSetCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

    m_pWndWindowCommon->Show(true);
    m_pWndPageSetCommon->Show(false);

    nResult = true;
Exit0:
    if (!nResult) 
    {
        if (m_pWndPageSetCommon != NULL) 
        {
            m_pWndPageSetCommon->Release();
            m_pWndPageSetCommon = NULL;
        }
        if (m_pWndWindowCommon != NULL) 
        {
            m_pWndWindowCommon->Release();
            m_pWndWindowCommon = NULL;
        }
    }
	return nResult;
}


void KUiWndPageSet::Release()
{
    if (m_pWndPageSetCommon) 
    {
        m_pWndPageSetCommon->Release();
        m_pWndPageSetCommon = NULL;
    }
    if (m_pWndWindowCommon) 
    {
        m_pWndWindowCommon->Release();
        m_pWndWindowCommon = NULL;
    }

	delete this;
}



void KUiWndPageSet::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(pIni && pszName);
    strcpy(m_szName, pszName);

    KG_PROCESS_ERROR(m_pWndWindowCommon);
    m_pWndWindowCommon->UpdateData(pszName, data, pIni);
    KG_PROCESS_ERROR(m_pWndPageSetCommon);
    m_pWndPageSetCommon->UpdateData(pszName, data, pIni);

Exit0:
    return;
}

void KUiWndPageSet::Show(int bShow, int nSubWnd)
{
    KG_PROCESS_ERROR(m_pWndWindowCommon);
    KG_PROCESS_ERROR(m_pWndPageSetCommon);
    switch(nSubWnd) 
    {
    case 0:
        m_pWndWindowCommon->Show(true && bShow);
        m_pWndPageSetCommon->Show(false && bShow);
    	break;
    case 1:
        m_pWndWindowCommon->Show(false && bShow);
        m_pWndPageSetCommon->Show(true && bShow);
        break;
    default:
        m_pWndWindowCommon->Show(true && bShow);
        m_pWndPageSetCommon->Show(false && bShow);
        break;
    }

Exit0:
    return;
}

void KUiWndPageSet::FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo)
{
    KG_PROCESS_ERROR(pPageInfo);
    pPageInfo->m_PageCount = 2;
    strcpy(pPageInfo->m_szPage[0], "窗口属性");
    strcpy(pPageInfo->m_szPage[1], "页面设置");
Exit0:
    return;
}
