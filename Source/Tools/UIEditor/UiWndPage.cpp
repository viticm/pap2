#include "Stdafx.h"
#include "UiWndPage.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"


KUiWndPage::KUiWndPage()
{
    m_pWndImageCommon     = NULL;
	m_pWndWindowCommon     = NULL;
}


KUiWndPage::~KUiWndPage()
{
 
}

KUiWndWindow* KUiWndPage::Create()
{
	KUiWndPage* pWnd = new KUiWndPage;
	if (pWnd == NULL)
		return NULL;
	return pWnd;
}

int KUiWndPage::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult  = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(hWnd);

    strcpy(m_szName, pszName);

	m_pWndWindowCommon = KUiWndWindowCommon::Create();
    KG_PROCESS_ERROR(m_pWndWindowCommon);
    nRetCode = m_pWndWindowCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

	m_pWndImageCommon = KUiWndImageCommon::Create(m_pWndWindowCommon);
    KG_PROCESS_ERROR(m_pWndImageCommon);
    nRetCode = m_pWndImageCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

    m_pWndWindowCommon->Show(true);
    m_pWndImageCommon->Show(false);

    nResult = true;
Exit0:
    if (!nResult) 
    {
        if (m_pWndImageCommon) 
        {
            m_pWndImageCommon->Release();
            m_pWndImageCommon = NULL;
        }
         if (m_pWndWindowCommon) 
        {
            m_pWndWindowCommon->Release();
            m_pWndWindowCommon = NULL;
        }
    }
	return nResult;
}

void KUiWndPage::Release()
{
    if (m_pWndImageCommon) 
    {
        m_pWndImageCommon->Release();
        m_pWndImageCommon = NULL;
    }
	if (m_pWndWindowCommon) 
    {
		m_pWndWindowCommon->Release();
		m_pWndWindowCommon = NULL;
	}
	delete this;
}



void KUiWndPage::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(pIni && pszName);
    strcpy(m_szName, pszName);

    KG_PROCESS_ERROR(m_pWndWindowCommon); 
    m_pWndWindowCommon->UpdateData(pszName, data, pIni);
    KG_PROCESS_ERROR(m_pWndImageCommon); 
    m_pWndImageCommon->UpdateData(pszName, data, pIni);

Exit0:
    return;
}

void KUiWndPage::Show(int bShow, int nSubWnd)
{
    KG_PROCESS_ERROR(m_pWndWindowCommon);
    KG_PROCESS_ERROR(m_pWndImageCommon);

    switch(nSubWnd) 
    {
    case 0:
        m_pWndWindowCommon->Show(true && bShow);
        m_pWndImageCommon->Show(false && bShow);
    	break;
    case 1:
        m_pWndWindowCommon->Show(false && bShow);
        m_pWndImageCommon->Show(true && bShow);
        break;
    default:
        break;
        m_pWndWindowCommon->Show(true && bShow);
        m_pWndImageCommon->Show(false && bShow);
    }

Exit0:
    return;
}

void KUiWndPage::FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo)
{
    KG_PROCESS_ERROR(pPageInfo);
    pPageInfo->m_PageCount = 2;
    strcpy(pPageInfo->m_szPage[0], "¥∞ø⁄ Ù–‘");
    strcpy(pPageInfo->m_szPage[1], "±≥æ∞Õº Ù–‘");

Exit0:
    return;
}
