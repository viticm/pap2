////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemText.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-4 16:21:15
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include <Windows.h>
#include "UiItemText.h"
#include "UiEditor.h"
#include "resource.h"
#include "UiItemCommon.h"
#include "UiItemTextCommon.h"

////////////////////////////////////////////////////////////////////////////////

KUiItemText::KUiItemText()
{
    m_pItemCommon       = NULL;
    m_pItemTextCommon = NULL;
}

KUiItemText::~KUiItemText()
{ 
}

KUiWndWindow* KUiItemText::Create()
{
	KUiItemText* pWnd = new KUiItemText;
	return pWnd;
}

int KUiItemText::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult  = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(hWnd);

    strcpy(m_szName, pszName);

	m_pItemCommon = KUiItemCommon::Create();
    KG_PROCESS_ERROR(m_pItemCommon);

    nRetCode = m_pItemCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

	m_pItemTextCommon = KUiItemTextCommon::Create();
    KG_PROCESS_ERROR(m_pItemTextCommon);

	nRetCode = m_pItemTextCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

    KG_PROCESS_ERROR(m_pItemCommon);
    m_pItemCommon->Show(true);

    KG_PROCESS_ERROR(m_pItemTextCommon);
    m_pItemTextCommon->Show(false);

    nResult = true;
Exit0:
    if (!nResult) 
    {
        if (m_pItemTextCommon != NULL) 
        {
            m_pItemTextCommon->Release();
            m_pItemTextCommon = NULL;
        }

        if (m_pItemCommon != NULL) 
        {
            m_pItemCommon->Release();
            m_pItemCommon = NULL;
        }
    }
	return nResult;
}

void KUiItemText::Release()
{
	if (m_pItemTextCommon)
    {
		m_pItemTextCommon->Release();
		m_pItemTextCommon = NULL;
	}

	if (m_pItemCommon) 
    {
		m_pItemCommon->Release();
		m_pItemCommon = NULL;
	}

	delete this;
}



void KUiItemText::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(pIni && pszName);
    strcpy(m_szName, pszName);
    KG_PROCESS_ERROR(m_pItemTextCommon);
    m_pItemTextCommon->UpdateData(pszName, data, pIni);
    KG_PROCESS_ERROR(m_pItemCommon); 
    m_pItemCommon->UpdateData(pszName, data, pIni);

Exit0:
    return;
}

void KUiItemText::Show(int bShow, int nSubWnd)
{
    KG_PROCESS_ERROR(m_pItemCommon);
    KG_PROCESS_ERROR(m_pItemTextCommon);

    switch(nSubWnd) 
    {
    case 0:
        m_pItemCommon->Show(true && bShow);
        m_pItemTextCommon->Show(false && bShow);
    	break;
    case 1:
        m_pItemCommon->Show(false && bShow);
        m_pItemTextCommon->Show(true && bShow);
        break;
    default:
        m_pItemCommon->Show(true && bShow);
        m_pItemTextCommon->Show(false && bShow);
        break;
    }

Exit0:
    return;
}

void KUiItemText::FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo)
{
    KG_PROCESS_ERROR(pPageInfo);
    pPageInfo->m_PageCount = 2;
    strcpy(pPageInfo->m_szPage[0], "组件属性");
    strcpy(pPageInfo->m_szPage[1], "文本属性");
Exit0:
    return;
}
