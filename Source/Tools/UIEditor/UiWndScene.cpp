#include "Stdafx.h"
#include "UiWndScene.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"
#include "UiWndWindowCommon.h"
#include "UiWndSceneCommon.h"


KUiWndScene::KUiWndScene()
: m_pWndWindowCommon(NULL)
, m_pWndSceneCommon(NULL)
{
}


KUiWndScene::~KUiWndScene()
{
}

KUiWndWindow* KUiWndScene::Create()
{
	return new KUiWndScene;
}

int KUiWndScene::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult  = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(hWnd);

    strcpy(m_szName, pszName);

	m_pWndWindowCommon = KUiWndWindowCommon::Create();
    KG_PROCESS_ERROR(m_pWndWindowCommon);

    nRetCode = m_pWndWindowCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

	m_pWndSceneCommon = KUiWndSceneCommon::Create();
    KG_PROCESS_ERROR(m_pWndSceneCommon);

    nRetCode = m_pWndSceneCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

    m_pWndWindowCommon->Show(true);
    m_pWndSceneCommon->Show(false);

    nResult = true;
Exit0:
    if (!nResult) 
    {
        if (m_pWndSceneCommon) 
        {
            m_pWndSceneCommon->Release();
            m_pWndSceneCommon = NULL;
        }
		if (m_pWndWindowCommon) 
        {
            m_pWndWindowCommon->Release();
            m_pWndWindowCommon = NULL;
        }
    }
	return nResult;
}

void KUiWndScene::Release()
{
	if (m_pWndSceneCommon) 
	{
		m_pWndSceneCommon->Release();
		m_pWndSceneCommon = NULL;
	}
	if (m_pWndWindowCommon) 
    {
		m_pWndWindowCommon->Release();
		m_pWndWindowCommon = NULL;
	}
	delete this;
}

void KUiWndScene::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(pIni && pszName);

    strcpy(m_szName, pszName);

    if (m_pWndWindowCommon)
		m_pWndWindowCommon->UpdateData(pszName, data, pIni);

    if (m_pWndSceneCommon)
		m_pWndSceneCommon->UpdateData(pszName, data, pIni);

Exit0:
    return;
}

void KUiWndScene::Show(int bShow, int nSubWnd)
{
    KG_PROCESS_ERROR(m_pWndWindowCommon);
    KG_PROCESS_ERROR(m_pWndSceneCommon);

    switch (nSubWnd) 
    {
    case 0:
        m_pWndWindowCommon->Show(true && bShow);
        m_pWndSceneCommon->Show(false && bShow);
    	break;
    case 1:
        m_pWndWindowCommon->Show(false && bShow);
        m_pWndSceneCommon->Show(true && bShow);
        break;
    default:
        break;
        m_pWndWindowCommon->Show(true && bShow);
        m_pWndSceneCommon->Show(false && bShow);
    }

Exit0:
    return;
}

void KUiWndScene::FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo)
{
    KG_PROCESS_ERROR(pPageInfo);

    pPageInfo->m_PageCount = 2;
    strcpy(pPageInfo->m_szPage[0], "窗口属性");
    strcpy(pPageInfo->m_szPage[1], "场景属性");

Exit0:
    return;
}
