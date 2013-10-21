////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemImage.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-5 9:16:41
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include <Windows.h>
#include "UiItemImage.h"
#include "UiEditor.h"
#include "resource.h"
#include "UiItemCommon.h"
#include "UiItemImageCommon.h"

////////////////////////////////////////////////////////////////////////////////

KUiItemImage::KUiItemImage()
{
    m_pItemCommon       = NULL;
    m_pItemImageCommon = NULL;
}

KUiItemImage::~KUiItemImage()
{ 
}

KUiWndWindow* KUiItemImage::Create()
{
	KUiItemImage* pWnd = new KUiItemImage;
	return pWnd;
}

int KUiItemImage::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult  = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(hWnd);

    strcpy(m_szName, pszName);

	m_pItemCommon = KUiItemCommon::Create();
    KG_PROCESS_ERROR(m_pItemCommon);

    nRetCode = m_pItemCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

	m_pItemImageCommon = KUiItemImageCommon::Create();
    KG_PROCESS_ERROR(m_pItemImageCommon);

	nRetCode = m_pItemImageCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

    KG_PROCESS_ERROR(m_pItemCommon);
    m_pItemCommon->Show(true);

    KG_PROCESS_ERROR(m_pItemImageCommon);
    m_pItemImageCommon->Show(false);

    nResult = true;
Exit0:
    if (!nResult) 
    {
        if (m_pItemImageCommon != NULL) 
        {
            m_pItemImageCommon->Release();
            m_pItemImageCommon = NULL;
        }

        if (m_pItemCommon != NULL) 
        {
            m_pItemCommon->Release();
            m_pItemCommon = NULL;
        }
    }
	return nResult;
}

void KUiItemImage::Release()
{
	if (m_pItemImageCommon)
    {
		m_pItemImageCommon->Release();
		m_pItemImageCommon = NULL;
	}

	if (m_pItemCommon) 
    {
		m_pItemCommon->Release();
		m_pItemCommon = NULL;
	}

	delete this;
}



void KUiItemImage::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(pIni && pszName);
    strcpy(m_szName, pszName);
    KG_PROCESS_ERROR(m_pItemImageCommon);
    m_pItemImageCommon->UpdateData(pszName, data, pIni);
    KG_PROCESS_ERROR(m_pItemCommon); 
    m_pItemCommon->UpdateData(pszName, data, pIni);

Exit0:
    return;
}

void KUiItemImage::Show(int bShow, int nSubWnd)
{
    KG_PROCESS_ERROR(m_pItemCommon);
    KG_PROCESS_ERROR(m_pItemImageCommon);

    switch(nSubWnd) 
    {
    case 0:
        m_pItemCommon->Show(true && bShow);
        m_pItemImageCommon->Show(false && bShow);
    	break;
    case 1:
        m_pItemCommon->Show(false && bShow);
        m_pItemImageCommon->Show(true && bShow);
        break;
    default:
        m_pItemCommon->Show(true && bShow);
        m_pItemImageCommon->Show(false && bShow);
        break;
    }

Exit0:
    return;
}

void KUiItemImage::FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo)
{
    KG_PROCESS_ERROR(pPageInfo);
    pPageInfo->m_PageCount = 2;
    strcpy(pPageInfo->m_szPage[0], "组件属性");
    strcpy(pPageInfo->m_szPage[1], "图像属性");
Exit0:
    return;
}


