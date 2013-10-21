#include "Stdafx.h"
#include "UiWndWebPage.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"
#include "UiWndWindowCommon.h"
#include "UiWndWebPageCommon.h"


KUiWndWebPage::KUiWndWebPage()
: m_pWndWindowCommon(NULL)
, m_pWndWebPageCommon(NULL)
{
}


KUiWndWebPage::~KUiWndWebPage()
{
}

KUiWndWindow* KUiWndWebPage::Create()
{
	return new KUiWndWebPage;
}

int KUiWndWebPage::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult  = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(hWnd);

    strcpy(m_szName, pszName);

	m_pWndWindowCommon = KUiWndWindowCommon::Create();
    KG_PROCESS_ERROR(m_pWndWindowCommon);

    nRetCode = m_pWndWindowCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

	m_pWndWebPageCommon = KUiWndWebPageCommon::Create();
    KG_PROCESS_ERROR(m_pWndWebPageCommon);

    nRetCode = m_pWndWebPageCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

    m_pWndWindowCommon->Show(true);
    m_pWndWebPageCommon->Show(false);

    nResult = true;
Exit0:
    if (!nResult) 
    {
        if (m_pWndWebPageCommon) 
        {
            m_pWndWebPageCommon->Release();
            m_pWndWebPageCommon = NULL;
        }
		if (m_pWndWindowCommon) 
        {
            m_pWndWindowCommon->Release();
            m_pWndWindowCommon = NULL;
        }
    }
	return nResult;
}

void KUiWndWebPage::Release()
{
	if (m_pWndWebPageCommon) 
	{
		m_pWndWebPageCommon->Release();
		m_pWndWebPageCommon = NULL;
	}
	if (m_pWndWindowCommon) 
    {
		m_pWndWindowCommon->Release();
		m_pWndWindowCommon = NULL;
	}
	delete this;
}

void KUiWndWebPage::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(pIni && pszName);

    strcpy(m_szName, pszName);

    if (m_pWndWindowCommon)
		m_pWndWindowCommon->UpdateData(pszName, data, pIni);

    if (m_pWndWebPageCommon)
		m_pWndWebPageCommon->UpdateData(pszName, data, pIni);

Exit0:
    return;
}

void KUiWndWebPage::Show(int bShow, int nSubWnd)
{
    KG_PROCESS_ERROR(m_pWndWindowCommon);
    KG_PROCESS_ERROR(m_pWndWebPageCommon);

    switch (nSubWnd) 
    {
    case 0:
        m_pWndWindowCommon->Show(true && bShow);
        m_pWndWebPageCommon->Show(false && bShow);
    	break;
    case 1:
        m_pWndWindowCommon->Show(false && bShow);
        m_pWndWebPageCommon->Show(true && bShow);
        break;
    default:
        break;
        m_pWndWindowCommon->Show(true && bShow);
        m_pWndWebPageCommon->Show(false && bShow);
    }

Exit0:
    return;
}

void KUiWndWebPage::FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo)
{
    KG_PROCESS_ERROR(pPageInfo);

    pPageInfo->m_PageCount = 2;
    strcpy(pPageInfo->m_szPage[0], "´°¿ÚÊôĞÔ");
    strcpy(pPageInfo->m_szPage[1], "ÍøÒ³ÊôĞÔ");

Exit0:
    return;
}
