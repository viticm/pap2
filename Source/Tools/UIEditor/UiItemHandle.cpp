////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemHandle.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-30 16:31:39
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include <Windows.h>
#include "UiItemHandle.h"
#include "UiEditor.h"
#include "resource.h"


////////////////////////////////////////////////////////////////////////////////

KUiItemHandle::KUiItemHandle()
{
    m_pItemCommon       = NULL;
    m_pItemHandleCommon = NULL;
}

KUiItemHandle::~KUiItemHandle()
{ 
}

KUiWndWindow* KUiItemHandle::Create()
{
	KUiItemHandle* pWnd = new KUiItemHandle;
	return pWnd;
}

int KUiItemHandle::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult  = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(hWnd);

    strcpy(m_szName, pszName);

	m_pItemCommon = KUiItemCommon::Create();
    KG_PROCESS_ERROR(m_pItemCommon);

    nRetCode = m_pItemCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

	m_pItemHandleCommon = KUiItemHandleCommon::Create();
    KG_PROCESS_ERROR(m_pItemHandleCommon);

	nRetCode = m_pItemHandleCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

    KG_PROCESS_ERROR(m_pItemCommon);
    m_pItemCommon->Show(true);

    KG_PROCESS_ERROR(m_pItemHandleCommon);
    m_pItemHandleCommon->Show(false);

    nResult = true;
Exit0:
    if (!nResult) 
    {
        if (m_pItemHandleCommon != NULL) 
        {
            m_pItemHandleCommon->Release();
            m_pItemHandleCommon = NULL;
        }

        if (m_pItemCommon != NULL) 
        {
            m_pItemCommon->Release();
            m_pItemCommon = NULL;
        }
    }
	return nResult;
}

void KUiItemHandle::Release()
{
	if (m_pItemHandleCommon)
    {
		m_pItemHandleCommon->Release();
		m_pItemHandleCommon = NULL;
	}

	if (m_pItemCommon) 
    {
		m_pItemCommon->Release();
		m_pItemCommon = NULL;
	}

	delete this;
}



void KUiItemHandle::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(pIni && pszName);
    strcpy(m_szName, pszName);
    KG_PROCESS_ERROR(m_pItemHandleCommon);
    m_pItemHandleCommon->UpdateData(pszName, data, pIni);
    KG_PROCESS_ERROR(m_pItemCommon); 
    m_pItemCommon->UpdateData(pszName, data, pIni);

Exit0:
    return;
}

void KUiItemHandle::Show(int bShow, int nSubWnd)
{
    KG_PROCESS_ERROR(m_pItemCommon);
    KG_PROCESS_ERROR(m_pItemHandleCommon);

    switch(nSubWnd) 
    {
    case 0:
        m_pItemCommon->Show(true && bShow);
        m_pItemHandleCommon->Show(false && bShow);
    	break;
    case 1:
        m_pItemCommon->Show(false && bShow);
        m_pItemHandleCommon->Show(true && bShow);
        break;
    default:
        m_pItemCommon->Show(true && bShow);
        m_pItemHandleCommon->Show(false && bShow);
        break;
    }



Exit0:
    return;
}

void KUiItemHandle::FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo)
{
    KG_PROCESS_ERROR(pPageInfo);
    pPageInfo->m_PageCount = 2;
    strcpy(pPageInfo->m_szPage[0], "组件属性");
    strcpy(pPageInfo->m_szPage[1], "容器属性");
Exit0:
    return;
}
