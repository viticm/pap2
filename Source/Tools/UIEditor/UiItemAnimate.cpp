////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemAnimate.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-5 13:47:57
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include <Windows.h>
#include "UiItemAnimate.h"
#include "UiEditor.h"
#include "resource.h"
#include "UiItemCommon.h"
#include "UiItemAnimateCommon.h"

////////////////////////////////////////////////////////////////////////////////

KUiItemAnimate::KUiItemAnimate()
{
    m_pItemCommon       = NULL;
    m_pItemAnimateCommon = NULL;
}

KUiItemAnimate::~KUiItemAnimate()
{ 
}

KUiWndWindow* KUiItemAnimate::Create()
{
	KUiItemAnimate* pWnd = new KUiItemAnimate;
	return pWnd;
}

int KUiItemAnimate::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult  = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(hWnd);

    strcpy(m_szName, pszName);

	m_pItemCommon = KUiItemCommon::Create();
    KG_PROCESS_ERROR(m_pItemCommon);

    nRetCode = m_pItemCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

	m_pItemAnimateCommon = KUiItemAnimateCommon::Create();
    KG_PROCESS_ERROR(m_pItemAnimateCommon);

	nRetCode = m_pItemAnimateCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

    KG_PROCESS_ERROR(m_pItemCommon);
    m_pItemCommon->Show(true);

    KG_PROCESS_ERROR(m_pItemAnimateCommon);
    m_pItemAnimateCommon->Show(false);

    nResult = true;
Exit0:
    if (!nResult) 
    {
        if (m_pItemAnimateCommon != NULL) 
        {
            m_pItemAnimateCommon->Release();
            m_pItemAnimateCommon = NULL;
        }

        if (m_pItemCommon != NULL) 
        {
            m_pItemCommon->Release();
            m_pItemCommon = NULL;
        }
    }
	return nResult;
}

void KUiItemAnimate::Release()
{
	if (m_pItemAnimateCommon)
    {
		m_pItemAnimateCommon->Release();
		m_pItemAnimateCommon = NULL;
	}

	if (m_pItemCommon) 
    {
		m_pItemCommon->Release();
		m_pItemCommon = NULL;
	}

	delete this;
}



void KUiItemAnimate::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(pIni && pszName);
    strcpy(m_szName, pszName);
    KG_PROCESS_ERROR(m_pItemAnimateCommon);
    m_pItemAnimateCommon->UpdateData(pszName, data, pIni);
    KG_PROCESS_ERROR(m_pItemCommon); 
    m_pItemCommon->UpdateData(pszName, data, pIni);

Exit0:
    return;
}

void KUiItemAnimate::Show(int bShow, int nSubWnd)
{
    KG_PROCESS_ERROR(m_pItemCommon);
    KG_PROCESS_ERROR(m_pItemAnimateCommon);

    switch(nSubWnd) 
    {
    case 0:
        m_pItemCommon->Show(true && bShow);
        m_pItemAnimateCommon->Show(false && bShow);
    	break;
    case 1:
        m_pItemCommon->Show(false && bShow);
        m_pItemAnimateCommon->Show(true && bShow);
        break;
    default:
        m_pItemCommon->Show(true && bShow);
        m_pItemAnimateCommon->Show(false && bShow);
        break;
    }

Exit0:
    return;
}

void KUiItemAnimate::FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo)
{
    KG_PROCESS_ERROR(pPageInfo);
    pPageInfo->m_PageCount = 2;
    strcpy(pPageInfo->m_szPage[0], "组件属性");
    strcpy(pPageInfo->m_szPage[1], "动画属性");
Exit0:
    return;
}

