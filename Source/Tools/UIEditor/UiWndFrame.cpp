#include "Stdafx.h"
#include "UiWndFrame.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"


KUiWndFrame::KUiWndFrame()
{
    m_pWndWindowCommon              = NULL;
    m_pWndFrameCommon               = NULL;
    m_pWndShowAnimateOrDragDrop     = NULL;
    m_pWndFrameTabControl           = NULL;
    m_pWndFrameScript               = NULL;
}

KUiWndFrame::~KUiWndFrame()
{
}

KUiWndWindow* KUiWndFrame::Create()
{
	KUiWndFrame* pWnd = new KUiWndFrame;
	if (pWnd == NULL)
		return NULL;
	return pWnd;
}

int KUiWndFrame::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult  = false;
    int nRetCode = false;
    char szParent[128] = "";
    IIniFile *pIni = g_Ui.GetIni();
    KG_PROCESS_ERROR(pIni);

    KG_PROCESS_ERROR(hWnd);
    strcpy(m_szName, pszName);

	m_pWndWindowCommon = KUiWndWindowCommon::Create();
    KG_PROCESS_ERROR(m_pWndWindowCommon);
    nRetCode = m_pWndWindowCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

	m_pWndFrameCommon = KUiWndFrameCommon::Create();
    KG_PROCESS_ERROR(m_pWndFrameCommon);
	nRetCode = m_pWndFrameCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

    pIni->GetString(m_szName, "._Parent", "", szParent, sizeof(szParent));
    m_pWndShowAnimateOrDragDrop = KUiWndShowAnimateCommon::Create();
    KG_PROCESS_ERROR(m_pWndShowAnimateOrDragDrop);
    nRetCode = m_pWndShowAnimateOrDragDrop->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

    m_pWndFrameTabControl = KUiWndFrameTabControl::Create();
    KG_PROCESS_ERROR(m_pWndFrameTabControl);
    nRetCode = m_pWndFrameTabControl->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

    m_pWndFrameScript = KUiWndFrameScript::Create();
    KG_PROCESS_ERROR(m_pWndFrameScript);
    nRetCode = m_pWndFrameScript->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

    m_pWndWindowCommon->Show(true);
    m_pWndFrameCommon->Show(false);
    m_pWndShowAnimateOrDragDrop->Show(false);
    m_pWndFrameTabControl->Show(false);
    m_pWndFrameScript->Show(false);

    nResult = true;
Exit0:
    if (!nResult) 
    {
        if (m_pWndFrameScript != NULL) 
        {
            m_pWndFrameScript->Release();
            m_pWndFrameScript = NULL;
        }
        if (m_pWndFrameTabControl != NULL) 
        {
            m_pWndFrameTabControl->Release();
            m_pWndFrameTabControl = NULL;
        }
        if (m_pWndShowAnimateOrDragDrop != NULL) 
        {
            m_pWndShowAnimateOrDragDrop->Release();
            m_pWndShowAnimateOrDragDrop = NULL;
        }
        if (m_pWndFrameCommon != NULL) 
        {
            m_pWndFrameCommon->Release();
            m_pWndFrameCommon = NULL;
        }
        if (m_pWndWindowCommon != NULL) 
        {
            m_pWndWindowCommon->Release();
            m_pWndWindowCommon = NULL;
        }
    }
	return nResult;
}


void KUiWndFrame::Release()
{
    if (m_pWndFrameScript != NULL) 
    {
        m_pWndFrameScript->Release();
        m_pWndFrameScript = NULL;
    }
    if (m_pWndFrameTabControl) 
    {
        m_pWndFrameTabControl->Release();
        m_pWndFrameTabControl = NULL;
    }
    if (m_pWndShowAnimateOrDragDrop) 
    {
        m_pWndShowAnimateOrDragDrop->Release();
        m_pWndShowAnimateOrDragDrop = NULL;
    }
    if (m_pWndFrameCommon) 
    {
        m_pWndFrameCommon->Release();
        m_pWndFrameCommon = NULL;
    }
    if (m_pWndWindowCommon) 
    {
        m_pWndWindowCommon->Release();
        m_pWndWindowCommon = NULL;
    }

	delete this;
}



void KUiWndFrame::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(pIni && pszName);
    strcpy(m_szName, pszName);

    KG_PROCESS_ERROR(m_pWndWindowCommon);
    m_pWndWindowCommon->UpdateData(pszName, data, pIni);
    KG_PROCESS_ERROR(m_pWndFrameCommon);
    m_pWndFrameCommon->UpdateData(pszName, data, pIni);
    KG_PROCESS_ERROR(m_pWndShowAnimateOrDragDrop);
    m_pWndShowAnimateOrDragDrop->UpdateData(pszName, data, pIni);
    KG_PROCESS_ERROR(m_pWndFrameTabControl);
    m_pWndFrameTabControl->UpdateData(pszName, data, pIni);
    KG_PROCESS_ERROR(m_pWndFrameScript);
    m_pWndFrameScript->UpdateData(pszName, data, pIni);

Exit0:
    return;
}

void KUiWndFrame::Show(int bShow, int nSubWnd)
{
    KG_PROCESS_ERROR(m_pWndWindowCommon);
    KG_PROCESS_ERROR(m_pWndFrameCommon);
    KG_PROCESS_ERROR(m_pWndShowAnimateOrDragDrop);
    KG_PROCESS_ERROR(m_pWndFrameTabControl);
    KG_PROCESS_ERROR(m_pWndFrameScript);

    switch(nSubWnd) 
    {
    case 0:
        m_pWndWindowCommon->Show(true && bShow);
        m_pWndFrameCommon->Show(false && bShow);
        m_pWndShowAnimateOrDragDrop->Show(false && bShow);
        m_pWndFrameTabControl->Show(false && bShow);
        m_pWndFrameScript->Show(false && bShow);
    	break;
    case 1:
        m_pWndWindowCommon->Show(false && bShow);
        m_pWndFrameCommon->Show(true && bShow);
        m_pWndShowAnimateOrDragDrop->Show(false && bShow);
        m_pWndFrameTabControl->Show(false && bShow);
        m_pWndFrameScript->Show(false && bShow);
        break;
    case 2:
        m_pWndWindowCommon->Show(false && bShow);
        m_pWndFrameCommon->Show(false && bShow);
        m_pWndShowAnimateOrDragDrop->Show(true && bShow);
        m_pWndFrameTabControl->Show(false && bShow);
        m_pWndFrameScript->Show(false && bShow);
        break;
    case 3:
        m_pWndWindowCommon->Show(false && bShow);
        m_pWndFrameCommon->Show(false && bShow);
        m_pWndShowAnimateOrDragDrop->Show(false && bShow);
        m_pWndFrameTabControl->Show(true && bShow);
        m_pWndFrameScript->Show(false && bShow);
        break;
    case 4:
        m_pWndWindowCommon->Show(false && bShow);
        m_pWndFrameCommon->Show(false && bShow);
        m_pWndShowAnimateOrDragDrop->Show(false && bShow);
        m_pWndFrameTabControl->Show(false && bShow);
        m_pWndFrameScript->Show(true && bShow);
        break;
    default:
        m_pWndWindowCommon->Show(true && bShow);
        m_pWndFrameCommon->Show(false && bShow);
        m_pWndShowAnimateOrDragDrop->Show(false && bShow);
        m_pWndFrameTabControl->Show(false && bShow);
        m_pWndFrameScript->Show(false && bShow);
        break;
    }

Exit0:
    return;
}

void KUiWndFrame::FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo)
{
    KG_PROCESS_ERROR(pPageInfo);
    IIniFile *pIni = g_Ui.GetIni();
    char szParent[128] = "";
    KG_PROCESS_ERROR(pIni);


    pPageInfo->m_PageCount = 5;
    strcpy(pPageInfo->m_szPage[0], "窗体");
    strcpy(pPageInfo->m_szPage[1], "拖动");

    pIni->GetString(m_szName, "._Parent", "", szParent, sizeof(szParent));
    strcpy(pPageInfo->m_szPage[2], "动画");
    strcpy(pPageInfo->m_szPage[3], "Tab键");
    strcpy(pPageInfo->m_szPage[4], "脚本");

Exit0:
    return;
}
