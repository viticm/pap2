////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemScene.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2007-4-4 12:46:44
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include <Windows.h>
#include "UiItemScene.h"
#include "UiEditor.h"
#include "resource.h"
#include "UiItemCommon.h"
#include "UiItemSceneCommon.h"

////////////////////////////////////////////////////////////////////////////////

KUiItemScene::KUiItemScene()
{
	m_pItemCommon       = NULL;
	m_pItemSceneCommon = NULL;
}

KUiItemScene::~KUiItemScene()
{ 
}

KUiWndWindow* KUiItemScene::Create()
{
	KUiItemScene* pWnd = new KUiItemScene;
	return pWnd;
}

int KUiItemScene::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
	int nResult  = false;
	int nRetCode = false;

	KG_PROCESS_ERROR(hWnd);

	strcpy(m_szName, pszName);

	m_pItemCommon = KUiItemCommon::Create();
	KG_PROCESS_ERROR(m_pItemCommon);

	nRetCode = m_pItemCommon->Init(hWnd, pRc, pszName);
	KG_PROCESS_ERROR(nRetCode);

	m_pItemSceneCommon = KUiItemSceneCommon::Create();
	KG_PROCESS_ERROR(m_pItemSceneCommon);

	nRetCode = m_pItemSceneCommon->Init(hWnd, pRc, pszName);
	KG_PROCESS_ERROR(nRetCode);

	KG_PROCESS_ERROR(m_pItemCommon);
	m_pItemCommon->Show(true);

	KG_PROCESS_ERROR(m_pItemSceneCommon);
	m_pItemSceneCommon->Show(false);

	nResult = true;
Exit0:
	if (!nResult) 
	{
		if (m_pItemSceneCommon != NULL) 
		{
			m_pItemSceneCommon->Release();
			m_pItemSceneCommon = NULL;
		}

		if (m_pItemCommon != NULL) 
		{
			m_pItemCommon->Release();
			m_pItemCommon = NULL;
		}
	}
	return nResult;
}

void KUiItemScene::Release()
{
	if (m_pItemSceneCommon)
	{
		m_pItemSceneCommon->Release();
		m_pItemSceneCommon = NULL;
	}

	if (m_pItemCommon) 
	{
		m_pItemCommon->Release();
		m_pItemCommon = NULL;
	}

	delete this;
}



void KUiItemScene::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(pIni && pszName);
	strcpy(m_szName, pszName);
	KG_PROCESS_ERROR(m_pItemSceneCommon);
	m_pItemSceneCommon->UpdateData(pszName, data, pIni);
	KG_PROCESS_ERROR(m_pItemCommon); 
	m_pItemCommon->UpdateData(pszName, data, pIni);

Exit0:
	return;
}

void KUiItemScene::Show(int bShow, int nSubWnd)
{
	KG_PROCESS_ERROR(m_pItemCommon);
	KG_PROCESS_ERROR(m_pItemSceneCommon);

	switch(nSubWnd) 
	{
	case 0:
		m_pItemCommon->Show(true && bShow);
		m_pItemSceneCommon->Show(false && bShow);
		break;
	case 1:
		m_pItemCommon->Show(false && bShow);
		m_pItemSceneCommon->Show(true && bShow);
		break;
	default:
		m_pItemCommon->Show(true && bShow);
		m_pItemSceneCommon->Show(false && bShow);
		break;
	}

Exit0:
	return;
}

void KUiItemScene::FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo)
{
	KG_PROCESS_ERROR(pPageInfo);
	pPageInfo->m_PageCount = 2;
	strcpy(pPageInfo->m_szPage[0], "组件属性");
	strcpy(pPageInfo->m_szPage[1], "场景属性");
Exit0:
	return;
}
