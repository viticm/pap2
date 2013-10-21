////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemShadow.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-5 14:33:29
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include <Windows.h>
#include "UiItemShadow.h"
#include "UiEditor.h"
#include "resource.h"
#include "UiItemCommon.h"
#include "UiItemImageCommon.h"
#include "UiItemShadowCommon.h"

////////////////////////////////////////////////////////////////////////////////

KUiItemShadow::KUiItemShadow()
{
    m_pItemCommon       = NULL;
    m_pItemShadowCommon = NULL;
}

KUiItemShadow::~KUiItemShadow()
{ 
}

KUiWndWindow* KUiItemShadow::Create()
{
	KUiItemShadow* pWnd = new KUiItemShadow;
	return pWnd;
}

int KUiItemShadow::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult  = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(hWnd);

    strcpy(m_szName, pszName);

	m_pItemCommon = KUiItemCommon::Create();
    KG_PROCESS_ERROR(m_pItemCommon);

    nRetCode = m_pItemCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

	m_pItemShadowCommon = KUiItemShadowCommon::Create();
    KG_PROCESS_ERROR(m_pItemShadowCommon);

	nRetCode = m_pItemShadowCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

    KG_PROCESS_ERROR(m_pItemCommon);
    m_pItemCommon->Show(true);

    KG_PROCESS_ERROR(m_pItemShadowCommon);
    m_pItemShadowCommon->Show(false);

    nResult = true;
Exit0:
    if (!nResult) 
    {
        if (m_pItemShadowCommon != NULL) 
        {
            m_pItemShadowCommon->Release();
            m_pItemShadowCommon = NULL;
        }

        if (m_pItemCommon != NULL) 
        {
            m_pItemCommon->Release();
            m_pItemCommon = NULL;
        }
    }
	return nResult;
}

void KUiItemShadow::Release()
{
	if (m_pItemShadowCommon)
    {
		m_pItemShadowCommon->Release();
		m_pItemShadowCommon = NULL;
	}

	if (m_pItemCommon) 
    {
		m_pItemCommon->Release();
		m_pItemCommon = NULL;
	}

	delete this;
}



void KUiItemShadow::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(pIni && pszName);
    strcpy(m_szName, pszName);
    KG_PROCESS_ERROR(m_pItemShadowCommon);
    m_pItemShadowCommon->UpdateData(pszName, data, pIni);
    KG_PROCESS_ERROR(m_pItemCommon); 
    m_pItemCommon->UpdateData(pszName, data, pIni);

Exit0:
    return;
}

void KUiItemShadow::Show(int bShow, int nSubWnd)
{
    KG_PROCESS_ERROR(m_pItemCommon);
    KG_PROCESS_ERROR(m_pItemShadowCommon);

    switch(nSubWnd) 
    {
    case 0:
        m_pItemCommon->Show(true && bShow);
        m_pItemShadowCommon->Show(false && bShow);
    	break;
    case 1:
        m_pItemCommon->Show(false && bShow);
        m_pItemShadowCommon->Show(true && bShow);
        break;
    default:
        m_pItemCommon->Show(true && bShow);
        m_pItemShadowCommon->Show(false && bShow);
        break;
    }

Exit0:
    return;
}

void KUiItemShadow::FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo)
{
    KG_PROCESS_ERROR(pPageInfo);
    pPageInfo->m_PageCount = 2;
    strcpy(pPageInfo->m_szPage[0], "组件属性");
    strcpy(pPageInfo->m_szPage[1], "阴影属性");
Exit0:
    return;
}