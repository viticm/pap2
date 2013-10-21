////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemTreeLeaf.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-5 16:02:09
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include <Windows.h>
#include "UiItemTreeLeaf.h"
#include "UiEditor.h"
#include "resource.h"
#include "UiItemCommon.h"
#include "UiItemImageCommon.h"
#include "UiItemHandleCommon.h"
#include "UiItemTreeLeafCommon.h"

////////////////////////////////////////////////////////////////////////////////

KUiItemTreeLeaf::KUiItemTreeLeaf()
{
    m_pItemCommon       = NULL;
    m_pItemHandleCommon = NULL;
    m_pItemTreeLeafCommon = NULL;
}

KUiItemTreeLeaf::~KUiItemTreeLeaf()
{ 
}

KUiWndWindow* KUiItemTreeLeaf::Create()
{
	KUiItemTreeLeaf* pWnd = new KUiItemTreeLeaf;
	return pWnd;
}

int KUiItemTreeLeaf::Init(HWND hWnd, RECT* pRc, const char *pszName)
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

	m_pItemTreeLeafCommon = KUiItemTreeLeafCommon::Create();
    KG_PROCESS_ERROR(m_pItemTreeLeafCommon);

	nRetCode = m_pItemTreeLeafCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

    KG_PROCESS_ERROR(m_pItemCommon);
    m_pItemCommon->Show(true);

    KG_PROCESS_ERROR(m_pItemHandleCommon);
    m_pItemHandleCommon->Show(false);

    KG_PROCESS_ERROR(m_pItemTreeLeafCommon);
    m_pItemTreeLeafCommon->Show(false);

    nResult = true;
Exit0:
    if (!nResult) 
    {
        if (m_pItemTreeLeafCommon) 
        {
            m_pItemTreeLeafCommon->Release();
            m_pItemTreeLeafCommon = NULL;
        }
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
    }
	return nResult;
}

void KUiItemTreeLeaf::Release()
{
	if (m_pItemTreeLeafCommon)
    {
		m_pItemTreeLeafCommon->Release();
		m_pItemTreeLeafCommon = NULL;
	}
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



void KUiItemTreeLeaf::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(pIni && pszName);
    strcpy(m_szName, pszName);
    KG_PROCESS_ERROR(m_pItemTreeLeafCommon);
    m_pItemTreeLeafCommon->UpdateData(pszName, data, pIni);
    KG_PROCESS_ERROR(m_pItemHandleCommon);
    m_pItemHandleCommon->UpdateData(pszName, data, pIni);
    KG_PROCESS_ERROR(m_pItemCommon); 
    m_pItemCommon->UpdateData(pszName, data, pIni);

Exit0:
    return;
}

void KUiItemTreeLeaf::Show(int bShow, int nSubWnd)
{
    KG_PROCESS_ERROR(m_pItemCommon);
    KG_PROCESS_ERROR(m_pItemHandleCommon);
    KG_PROCESS_ERROR(m_pItemTreeLeafCommon);

    switch(nSubWnd) 
    {
    case 0:
        m_pItemCommon->Show(true && bShow);
        m_pItemHandleCommon->Show(false && bShow);
        m_pItemTreeLeafCommon->Show(false && bShow);
    	break;
    case 1:
        m_pItemCommon->Show(false && bShow);
        m_pItemHandleCommon->Show(true && bShow);
        m_pItemTreeLeafCommon->Show(false && bShow);
        break;
    case 2:
        m_pItemCommon->Show(false && bShow);
        m_pItemHandleCommon->Show(false && bShow);
        m_pItemTreeLeafCommon->Show(true && bShow);
        break;
    default:
        m_pItemCommon->Show(true && bShow);
        m_pItemHandleCommon->Show(false && bShow);
        m_pItemTreeLeafCommon->Show(false && bShow);
        break;
    }

Exit0:
    return;
}

void KUiItemTreeLeaf::FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo)
{
    KG_PROCESS_ERROR(pPageInfo);
    pPageInfo->m_PageCount = 3;
    strcpy(pPageInfo->m_szPage[0], "组件属性");
    strcpy(pPageInfo->m_szPage[1], "容器属性");
    strcpy(pPageInfo->m_szPage[2], "树叶属性");
Exit0:
    return;
}