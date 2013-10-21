////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemBox.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-3-1 15:17:15
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include <Windows.h>
#include "UiItemBox.h"
#include "UiEditor.h"
#include "resource.h"
#include "UiItemCommon.h"
#include "UiItemBoxCommon.h"

////////////////////////////////////////////////////////////////////////////////

KUiItemBox::KUiItemBox()
{
	m_pItemCommon       = NULL;
	m_pItemBoxCommon = NULL;
}

KUiItemBox::~KUiItemBox()
{ 
}

KUiWndWindow* KUiItemBox::Create()
{
	KUiItemBox* pWnd = new KUiItemBox;
	return pWnd;
}

int KUiItemBox::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
	int nResult  = false;
	int nRetCode = false;

	KG_PROCESS_ERROR(hWnd);

	strcpy(m_szName, pszName);

	m_pItemCommon = KUiItemCommon::Create();
	KG_PROCESS_ERROR(m_pItemCommon);

	nRetCode = m_pItemCommon->Init(hWnd, pRc, pszName);
	KG_PROCESS_ERROR(nRetCode);

	m_pItemBoxCommon = KUiItemBoxCommon::Create();
	KG_PROCESS_ERROR(m_pItemBoxCommon);

	nRetCode = m_pItemBoxCommon->Init(hWnd, pRc, pszName);
	KG_PROCESS_ERROR(nRetCode);

	KG_PROCESS_ERROR(m_pItemCommon);
	m_pItemCommon->Show(true);

	KG_PROCESS_ERROR(m_pItemBoxCommon);
	m_pItemBoxCommon->Show(false);

	nResult = true;
Exit0:
	if (!nResult) 
	{
		if (m_pItemBoxCommon != NULL) 
		{
			m_pItemBoxCommon->Release();
			m_pItemBoxCommon = NULL;
		}

		if (m_pItemCommon != NULL) 
		{
			m_pItemCommon->Release();
			m_pItemCommon = NULL;
		}
	}
	return nResult;
}

void KUiItemBox::Release()
{
	if (m_pItemBoxCommon)
	{
		m_pItemBoxCommon->Release();
		m_pItemBoxCommon = NULL;
	}

	if (m_pItemCommon) 
	{
		m_pItemCommon->Release();
		m_pItemCommon = NULL;
	}

	delete this;
}



void KUiItemBox::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(pIni && pszName);
	strcpy(m_szName, pszName);
	KG_PROCESS_ERROR(m_pItemBoxCommon);
	m_pItemBoxCommon->UpdateData(pszName, data, pIni);
	KG_PROCESS_ERROR(m_pItemCommon); 
	m_pItemCommon->UpdateData(pszName, data, pIni);

Exit0:
	return;
}

void KUiItemBox::Show(int bShow, int nSubWnd)
{
	KG_PROCESS_ERROR(m_pItemCommon);
	KG_PROCESS_ERROR(m_pItemBoxCommon);

	switch(nSubWnd) 
	{
	case 0:
		m_pItemCommon->Show(true && bShow);
		m_pItemBoxCommon->Show(false && bShow);
		break;
	case 1:
		m_pItemCommon->Show(false && bShow);
		m_pItemBoxCommon->Show(true && bShow);
		break;
	default:
		m_pItemCommon->Show(true && bShow);
		m_pItemBoxCommon->Show(false && bShow);
		break;
	}

Exit0:
	return;
}

void KUiItemBox::FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo)
{
	KG_PROCESS_ERROR(pPageInfo);
	pPageInfo->m_PageCount = 2;
	strcpy(pPageInfo->m_szPage[0], "组件属性");
	strcpy(pPageInfo->m_szPage[1], "盒子属性");
Exit0:
	return;
}

