#include "Stdafx.h"
#include "UiWndShowAnimate.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"


KUiWndShowAnimate::KUiWndShowAnimate()
{
	m_pWndWindowCommon      = NULL;
    m_pWndShowAnimateCommon = NULL;
}


KUiWndShowAnimate::~KUiWndShowAnimate()
{   
}

KUiWndWindow* KUiWndShowAnimate::Create()
{
	KUiWndShowAnimate* pWnd = new KUiWndShowAnimate;
	if (pWnd == NULL)
		return NULL;
	return pWnd;
}

int KUiWndShowAnimate::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult  = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(hWnd);
    strcpy(m_szName, pszName);

	m_pWndWindowCommon = KUiWndWindowCommon::Create();
    KG_PROCESS_ERROR(m_pWndWindowCommon);
    nRetCode = m_pWndWindowCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

    m_pWndShowAnimateCommon = KUiWndShowAnimateCommon::Create();
    KG_PROCESS_ERROR(m_pWndShowAnimateCommon);
	nRetCode = m_pWndShowAnimateCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

    m_pWndWindowCommon->Show(true);
    m_pWndShowAnimateCommon->Show(false);

    nResult = true;
Exit0:
    if (!nResult) 
    {
        if (m_pWndShowAnimateCommon != NULL) 
        {
            m_pWndShowAnimateCommon->Release();
            m_pWndShowAnimateCommon = NULL;
        }

        if (m_pWndWindowCommon != NULL) 
        {
            m_pWndWindowCommon->Release();
            m_pWndWindowCommon = NULL;
        }
    }
	return nResult;
}

void KUiWndShowAnimate::Release()
{
	if (m_pWndShowAnimateCommon)
    {
		m_pWndShowAnimateCommon->Release();
		m_pWndShowAnimateCommon = NULL;
	}
	if (m_pWndWindowCommon) 
    {
		m_pWndWindowCommon->Release();
		m_pWndWindowCommon = NULL;
	}

	delete this;
}



void KUiWndShowAnimate::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(pIni && pszName);
    strcpy(m_szName, pszName);

    KG_PROCESS_ERROR(m_pWndWindowCommon); 
    m_pWndWindowCommon->UpdateData(pszName, data, pIni);
    KG_PROCESS_ERROR(m_pWndShowAnimateCommon); 
    m_pWndShowAnimateCommon->UpdateData(pszName, data, pIni);

Exit0:
    return;
}

void KUiWndShowAnimate::Show(int bShow, int nSubWnd)
{
    KG_PROCESS_ERROR(m_pWndWindowCommon);
    KG_PROCESS_ERROR(m_pWndShowAnimateCommon);

    switch(nSubWnd) 
    {
    case 0:
        m_pWndWindowCommon->Show(true && bShow);
        m_pWndShowAnimateCommon->Show(false && bShow);
    	break;
    case 1:
        m_pWndWindowCommon->Show(false && bShow);
        m_pWndShowAnimateCommon->Show(true && bShow);
        break;
    default:
        m_pWndWindowCommon->Show(true && bShow);
        m_pWndShowAnimateCommon->Show(false && bShow);
        break;
    }
Exit0:
    return;
}

void KUiWndShowAnimate::FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo)
{
    KG_PROCESS_ERROR(pPageInfo);
    pPageInfo->m_PageCount = 2;
    strcpy(pPageInfo->m_szPage[0], "窗口属性");
    strcpy(pPageInfo->m_szPage[1], "动画属性");
Exit0:
    return;
}
