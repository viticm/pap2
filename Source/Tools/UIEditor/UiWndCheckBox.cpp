#include "Stdafx.h"
#include "UiWndCheckBox.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"

KUiWndCheckBox::KUiWndCheckBox()
{
	m_pWndImageCommon      = NULL;
	m_pWndWindowCommon     = NULL;
    m_pWndCheckBoxCommon     = NULL;  
}
KUiWndCheckBox::~KUiWndCheckBox()
{

}

KUiWndWindow* KUiWndCheckBox::Create()
{
	KUiWndCheckBox* pWnd = new KUiWndCheckBox;
	if (pWnd == NULL)
		return NULL;
	return pWnd;
}

int KUiWndCheckBox::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult  = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(hWnd);

    strcpy(m_szName, pszName);

    //window.
	m_pWndWindowCommon = KUiWndWindowCommon::Create();
    KG_PROCESS_ERROR(m_pWndWindowCommon);
    nRetCode = m_pWndWindowCommon->Init(hWnd, pRc,  pszName);
    KG_PROCESS_ERROR(nRetCode);
    
    //image.
	m_pWndImageCommon = KUiWndImageCommon::Create(m_pWndWindowCommon);
    KG_PROCESS_ERROR(m_pWndImageCommon);
	nRetCode = m_pWndImageCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

    //button.
	m_pWndCheckBoxCommon = KUiWndCheckBoxCommon::Create();
    KG_PROCESS_ERROR(m_pWndCheckBoxCommon);
	nRetCode = m_pWndCheckBoxCommon->Init(hWnd, pRc, pszName);
    KG_PROCESS_ERROR(nRetCode);

    m_pWndWindowCommon->Show(true);
    m_pWndImageCommon->Show(false);
    m_pWndCheckBoxCommon->Show(false);

    nResult = true;
Exit0:
    if (!nResult) 
    {
        if (m_pWndCheckBoxCommon != NULL) 
        {
            m_pWndCheckBoxCommon->Release();
            m_pWndCheckBoxCommon = NULL;
        }
        if (m_pWndImageCommon != NULL) 
        {
            m_pWndImageCommon->Release();
            m_pWndImageCommon = NULL;
        }

        if (m_pWndWindowCommon != NULL) 
        {
            m_pWndWindowCommon->Release();
            m_pWndWindowCommon = NULL;
        }
    }
	return nResult;
}


void KUiWndCheckBox::Release()
{
    if (m_pWndCheckBoxCommon)
    {
		m_pWndCheckBoxCommon->Release();
		m_pWndCheckBoxCommon = NULL;
	}
	if (m_pWndImageCommon)
    {
		m_pWndImageCommon->Release();
		m_pWndImageCommon = NULL;
	}

	if (m_pWndWindowCommon) 
    {
		m_pWndWindowCommon->Release();
		m_pWndWindowCommon = NULL;
	}

	delete this;
}



void KUiWndCheckBox::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(pIni && pszName);
    //strcpy(m_szName, pszName);
    KG_PROCESS_ERROR(m_pWndImageCommon);
    m_pWndImageCommon->UpdateData(pszName, data, pIni);
    KG_PROCESS_ERROR(m_pWndWindowCommon); 
    m_pWndWindowCommon->UpdateData(pszName, data, pIni);
    KG_PROCESS_ERROR(m_pWndCheckBoxCommon); 
    m_pWndCheckBoxCommon->UpdateData(pszName, data, pIni);
Exit0:
    return;
}

void KUiWndCheckBox::Show(int bShow, int nSubWnd)
{
    KG_PROCESS_ERROR(m_pWndWindowCommon);
    KG_PROCESS_ERROR(m_pWndImageCommon);
    KG_PROCESS_ERROR(m_pWndCheckBoxCommon);

    switch(nSubWnd) 
    {
    case 0:
        m_pWndWindowCommon->Show(true && bShow);
        m_pWndImageCommon->Show(false && bShow);
        m_pWndCheckBoxCommon->Show(false && bShow);
    	break;
    case 1:
        m_pWndWindowCommon->Show(false && bShow);
        m_pWndImageCommon->Show(true && bShow);
        m_pWndCheckBoxCommon->Show(false && bShow);
    	break;
    case 2:
        m_pWndWindowCommon->Show(false && bShow);
        m_pWndImageCommon->Show(false && bShow);
        m_pWndCheckBoxCommon->Show(true && bShow);
    	break;
    default:
        m_pWndWindowCommon->Show(true && bShow);
        m_pWndImageCommon->Show(false && bShow);
        m_pWndCheckBoxCommon->Show(false && bShow);
        break;
    }



Exit0:
    return;
}

void KUiWndCheckBox::FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo)
{
    KG_PROCESS_ERROR(pPageInfo);
    pPageInfo->m_PageCount = 3;
    strcpy(pPageInfo->m_szPage[0], "窗口属性");
    strcpy(pPageInfo->m_szPage[1], "图像属性");
    strcpy(pPageInfo->m_szPage[2], "选择属性");
Exit0:
    return;
}


