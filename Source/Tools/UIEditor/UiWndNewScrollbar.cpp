
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiWndNewScrollbar.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-11 11:43:38
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include "UiWndNewScrollbar.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"
#include "UiWndWindowCommon.h"
#include "UiWndNewScrollbarCommon.h"


KUiWndNewScrollBar::KUiWndNewScrollBar()
{
    
    m_pWndWindowCommon     = NULL;
    m_pWndNewScrollbarCommon       = NULL;

}

KUiWndNewScrollBar::~KUiWndNewScrollBar()
{ 

}

KUiWndWindow* KUiWndNewScrollBar::Create()
{
	KUiWndNewScrollBar* pWnd = new KUiWndNewScrollBar;
	if (pWnd == NULL)
		return NULL;
	return pWnd;
}

int KUiWndNewScrollBar::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult  = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(hWnd);

    strcpy(m_szName, pszName);

	m_pWndWindowCommon = KUiWndWindowCommon::Create();
    KG_PROCESS_ERROR(m_pWndWindowCommon);

    nRetCode = m_pWndWindowCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

	m_pWndNewScrollbarCommon = KUiWndNewScrollBarCommon::Create();
    KG_PROCESS_ERROR(m_pWndNewScrollbarCommon);

	nRetCode = m_pWndNewScrollbarCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

    KG_PROCESS_ERROR(m_pWndWindowCommon);
    m_pWndWindowCommon->Show(true);

    KG_PROCESS_ERROR(m_pWndNewScrollbarCommon);
    m_pWndNewScrollbarCommon->Show(false);

    nResult = true;
Exit0:
    if (!nResult) 
    {
        if (m_pWndNewScrollbarCommon != NULL) 
        {
            m_pWndNewScrollbarCommon->Release();
            m_pWndNewScrollbarCommon = NULL;
        }

        if (m_pWndWindowCommon != NULL) 
        {
            m_pWndWindowCommon->Release();
            m_pWndWindowCommon = NULL;
        }
    }
	return nResult;
}

void KUiWndNewScrollBar::Release()
{
	if (m_pWndNewScrollbarCommon)
    {
		m_pWndNewScrollbarCommon->Release();
		m_pWndNewScrollbarCommon = NULL;
	}

	if (m_pWndWindowCommon) 
    {
		m_pWndWindowCommon->Release();
		m_pWndWindowCommon = NULL;
	}

	delete this;
}



void KUiWndNewScrollBar::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(pIni && pszName);
    strcpy(m_szName, pszName);
    KG_PROCESS_ERROR(m_pWndNewScrollbarCommon);
    m_pWndNewScrollbarCommon->UpdateData(pszName, data, pIni);
    KG_PROCESS_ERROR(m_pWndWindowCommon); 
    m_pWndWindowCommon->UpdateData(pszName, data, pIni);

Exit0:
    return;
}

void KUiWndNewScrollBar::Show(int bShow, int nSubWnd)
{
    KG_PROCESS_ERROR(m_pWndWindowCommon);
    KG_PROCESS_ERROR(m_pWndNewScrollbarCommon);

    switch(nSubWnd) 
    {
    case 0:
        m_pWndWindowCommon->Show(true && bShow);
        m_pWndNewScrollbarCommon->Show(false && bShow);
    	break;
    case 1:
        m_pWndWindowCommon->Show(false && bShow);
        m_pWndNewScrollbarCommon->Show(true && bShow);
        break;
    default:
        m_pWndWindowCommon->Show(true && bShow);
        m_pWndNewScrollbarCommon->Show(false && bShow);
        break;
    }



Exit0:
    return;
}

void KUiWndNewScrollBar::FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo)
{
    KG_PROCESS_ERROR(pPageInfo);
    pPageInfo->m_PageCount = 2;
    strcpy(pPageInfo->m_szPage[0], "窗口属性");
    strcpy(pPageInfo->m_szPage[1], "滚动条属性");
Exit0:
    return;
}
