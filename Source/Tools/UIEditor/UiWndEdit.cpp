#include "Stdafx.h"
#include "UiWndEdit.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"


KUiWndEdit::KUiWndEdit()
{
    m_pWndWindowCommon     = NULL;
    m_pWndEditCommon       = NULL;
}

KUiWndEdit::~KUiWndEdit()
{

}

KUiWndWindow* KUiWndEdit::Create()
{
	KUiWndEdit* pWnd = new KUiWndEdit;
	if (pWnd == NULL)
		return NULL;
	return pWnd;
}

int KUiWndEdit::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult  = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(hWnd);

    strcpy(m_szName, pszName);

	m_pWndWindowCommon = KUiWndWindowCommon::Create();
    KG_PROCESS_ERROR(m_pWndWindowCommon);

    nRetCode = m_pWndWindowCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

	m_pWndEditCommon = KUiWndEditCommon::Create();
    KG_PROCESS_ERROR(m_pWndEditCommon);

	nRetCode = m_pWndEditCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

    KG_PROCESS_ERROR(m_pWndWindowCommon);
    m_pWndWindowCommon->Show(true);

    KG_PROCESS_ERROR(m_pWndEditCommon);
    m_pWndEditCommon->Show(false);

    nResult = true;
Exit0:
    if (!nResult) 
    {
        if (m_pWndEditCommon != NULL) 
        {
            m_pWndEditCommon->Release();
            m_pWndEditCommon = NULL;
        }

        if (m_pWndWindowCommon != NULL) 
        {
            m_pWndWindowCommon->Release();
            m_pWndWindowCommon = NULL;
        }
    }
	return nResult;
}

void KUiWndEdit::Release()
{
	if (m_pWndEditCommon)
    {
		m_pWndEditCommon->Release();
		m_pWndEditCommon = NULL;
	}

	if (m_pWndWindowCommon) 
    {
		m_pWndWindowCommon->Release();
		m_pWndWindowCommon = NULL;
	}

	delete this;
}

void KUiWndEdit::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(pIni && pszName);
    strcpy(m_szName, pszName);
    KG_PROCESS_ERROR(m_pWndEditCommon);
    m_pWndEditCommon->UpdateData(pszName, data, pIni);
    KG_PROCESS_ERROR(m_pWndWindowCommon); 
    m_pWndWindowCommon->UpdateData(pszName, data, pIni);

Exit0:
    return;
}

void KUiWndEdit::Show(int bShow, int nSubWnd)
{
    KG_PROCESS_ERROR(m_pWndWindowCommon);
    KG_PROCESS_ERROR(m_pWndEditCommon);
    switch(nSubWnd) 
    {
    case 0:
        m_pWndWindowCommon->Show(true && bShow);
        m_pWndEditCommon->Show(false && bShow);
    	break;
    case 1:
        m_pWndWindowCommon->Show(false && bShow);
        m_pWndEditCommon->Show(true && bShow);
        break;
    default:
        m_pWndWindowCommon->Show(true && bShow);
        m_pWndEditCommon->Show(false && bShow);
        break;
    }
Exit0:
    return;
}

void KUiWndEdit::FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo)
{
    KG_PROCESS_ERROR(pPageInfo);
    pPageInfo->m_PageCount = 2;
    strcpy(pPageInfo->m_szPage[0], "´°¿ÚÊôĞÔ");
    strcpy(pPageInfo->m_szPage[1], "±à¼­¿òÊôĞÔ");
Exit0:
    return;
}
