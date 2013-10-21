// KSceneSettingDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSettingDialog.h"
#include ".\kscenesettingdialog.h"
#include "IEKG3DEnvironment.h"
#include "IEKG3DCollocator.h"

#include "KSceneSettingPageBase.h"
#include "KSceneSettingPageGeneral.h"
#include "KSceneSettingPageGround.h"
#include "KSceneSettingPageLighting.h"
#include "KSceneSettingPageAmbient.h"
#include "KSceneSettingPageFog.h"
#include "KSceneSettingPageSkyBox.h"
#include "KSceneSettingPageClouds.h"
#include "KSceneSettingPageLensFlare.h"
#include "KSceneSettingPageFarMount.h"
#include "KSceneSettingPageDW.h"
#include "KSceneSettingPageEnvironment.h"
#include "KSceneSettingPageRegionSplit.h"
#include "KSceneSettingPageWind.h"

//#include "KGHelperClasses.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

// KSceneSettingDialog dialog

IMPLEMENT_DYNAMIC(KSceneSettingDialog, CDialog)
KSceneSettingDialog::KSceneSettingDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSettingDialog::IDD, pParent)
{
	//m_lpSceneEditor = NULL;
	m_bIsMyselfModal = false;
}

KSceneSettingDialog::~KSceneSettingDialog()
{
	KG_CUSTOM_HELPERS::TContainerDelete(m_vecPages);
}

void KSceneSettingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KSceneSettingDialog, CDialog)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_SS_BN_APPLY, OnBnClickedApply)
	ON_BN_CLICKED(IDC_SS_BN_CANCEL, OnBnClickedDiscard)
	ON_WM_SHOWWINDOW()
    ON_BN_CLICKED(IDC_RADIO_SETTINGLOW, &KSceneSettingDialog::OnBnClickedRadioSettinglow)
    ON_BN_CLICKED(IDC_RADIO_SETTINGADVANCED, &KSceneSettingDialog::OnBnClickedRadioSettingadvanced)
END_MESSAGE_MAP()


// KSceneSettingDialog message handlers


#define ADD_PAGE(classname)	NULL;{\
classname *p = new classname;\
if(p)\
{\
	m_vecPages.push_back(p);\
	m_PropertySheet.AddPage(p);\
}\
}NULL


BOOL KSceneSettingDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	{	
		ADD_PAGE(KSceneSettingPageEnvironment);
		ADD_PAGE(KSceneSettingPageFog);
		ADD_PAGE(KSceneSettingPageSkyBox);
		ADD_PAGE(KSceneSettingPageClouds);
		ADD_PAGE(KSceneSettingPageLensFlare);
		ADD_PAGE(KSceneSettingPageFarMount);
		ADD_PAGE(KSceneSettingPageLighting);
		ADD_PAGE(KSceneSettingPageDW);
		ADD_PAGE(KSceneSettingPageRegionSplit);
		ADD_PAGE(KSceneSettingPageWind);

	}



	//显示非摸态的属性表
	m_PropertySheet.Create(this, WS_CHILD | WS_VISIBLE, 0);

	// 当对话框搜索下一个Tab项时，WS_EX_CONTROLPARENT标记避免了死循环发生的可能性
	//他可以让对话框搜索到属性表中的子窗体控件，就象对话框窗体上的其他普通控件一样。
	m_PropertySheet.ModifyStyleEx (0, WS_EX_CONTROLPARENT | WS_EX_TRANSPARENT);

	//允许用户TAB键切换到属性表
	m_PropertySheet.ModifyStyle( 0, WS_TABSTOP );

	//显示属性表				
	m_PropertySheet.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	
	//调整属性表大小
	CWnd* pButton = GetDlgItem(IDOK);
	if (pButton)
	{
		CRect ButtonRect, PropertySheetRect, DialogRect;
		pButton->GetWindowRect(&ButtonRect);
		m_PropertySheet.GetWindowRect(&PropertySheetRect);
		GetClientRect(&DialogRect);
		int nRestHeight = DialogRect.Height() - PropertySheetRect.Height() - ButtonRect.Height();
		GetWindowRect(&DialogRect);
		int nNewHeight = DialogRect.Height() - nRestHeight + 10;
		SetWindowPos(NULL, 0, 0, DialogRect.Width(), nNewHeight, SWP_NOZORDER | SWP_NOMOVE);
		static int nIDArray[] = {
			IDC_BN_RESTORE_DEFAULT
			, IDOK
			, IDCANCEL
			, IDC_SS_BN_APPLY
			, IDC_SS_BN_CANCEL
            , IDC_RADIO_SETTINGLOW
            , IDC_RADIO_SETTINGADVANCED
		};
		for (int i = 0; i < _countof(nIDArray); i++)
		{
			pButton = GetDlgItem(nIDArray[i]);
			if (pButton)
			{
				pButton->GetWindowRect(&ButtonRect);
				ScreenToClient(&ButtonRect);
				pButton->SetWindowPos(NULL, ButtonRect.left, ButtonRect.top - nRestHeight + 5, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
		}

        {
            IEKG3DEnvironment* pEnvironment = KSH::GetEngineInterfaceEx<IEKG3DEnvironment>();
            if (pEnvironment)
            {
                DWORD dwType = pEnvironment->GetSettingType();
                CButton *pButton = NULL;
                pButton = (CButton*)GetDlgItem(nIDArray[dwType + 5]);
                pButton->SetCheck(TRUE);
            }
        }
	}
    
	///<CTH>// Last modify by Chen Tianhong: 2007-6-6 17:16:42
	///改成非模态对话框，不用Timmer了
	//m_nTimer = _kg_to_smaller_size_cast<INT>(SetTimer(1, 100, NULL));///设定计时器，ID为1，时间间隔为100，不使用TimerProc而使用OnTimer（NULL），返回新的系统分派的Timer的ID
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSettingDialog::OnBnClickedOk()
{
	this->OnBnClickedApply();
    OnOK();
}

/*
void KSceneSettingDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	/ *
	if (g_pEngineManager)
		{
			g_pEngineManager->FrameMove();
			g_pEngineManager->Render();
		}* /
	
	CDialog::OnTimer(nIDEvent);
}
*/

/*
void KSceneSettingDialog::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	/ *
	if (m_nTimer != -1)
		{
			KillTimer(m_nTimer);
			m_nTimer = -1;
		}* /
	
	CDialog::PostNcDestroy();
}*/


void KSceneSettingDialog::OnCancel()	//为了保证OnKillActive的执行
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_bIsMyselfModal)
	{
		m_bIsMyselfModal = false;
		CDialog::OnCancel();
	}
	else
	{
		CPropertyPage* pPage = this->m_PropertySheet.GetActivePage();
		if (pPage)
		{
			pPage->OnKillActive();
		}
		this->ShowWindow(SW_HIDE);
	}
	
}

void KSceneSettingDialog::OnOK()	//为了保证OnKillActive的执行
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_bIsMyselfModal)
	{
		m_bIsMyselfModal = false;
		CDialog::OnOK();
	}
	else
	{
		CPropertyPage* pPage = this->m_PropertySheet.GetActivePage();
		if (pPage)
		{
			pPage->OnKillActive();
		}
		this->ShowWindow(SW_HIDE);
	}
}

INT_PTR KSceneSettingDialog::DoModal()
{
	m_bIsMyselfModal =true;
	return CDialog::DoModal();
}
void KSceneSettingDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	if(bShow)
	{
		INT nCount = m_PropertySheet.GetPageCount();
		if (nCount > 0)
		{	
			CPropertyPage* pFirstPage = m_PropertySheet.GetActivePage();;
			if (pFirstPage && pFirstPage->GetSafeHwnd())
			{
				pFirstPage->OnSetActive();
			}
		}
	}
}

void KSceneSettingDialog::OnBnClickedApply()
{
	KSceneSettingPageBase* p = dynamic_cast<KSceneSettingPageBase*>(m_PropertySheet.GetActivePage());
	if (p)
	{
		p->SynFromInterfaceToInner();
		p->SynFromInnerToInterface();
	}
}

void KSceneSettingDialog::OnBnClickedDiscard()
{
	KSceneSettingPageBase* p = dynamic_cast<KSceneSettingPageBase*>(m_PropertySheet.GetActivePage());
	if (p)
	{
		p->SynFromInnerToInterface();
	}
}

void KSceneSettingDialog::OnBnClickedRadioSettinglow()
{
    HRESULT hRetCode = E_FAIL;
    DWORD dwType = 0;
    IEKG3DEnvironment* pEnvironment = KSH::GetEngineInterfaceEx<IEKG3DEnvironment>();
    KG_PROCESS_ERROR(pEnvironment);

    dwType = pEnvironment->GetSettingType();
    if (dwType != SETTING_LOW)
    {
        IEKG3DSceneSceneEditor* pSceneEditor = KSH::GetEngineInterfaceEx<IEKG3DSceneSceneEditor>();
        KG_PROCESS_ERROR(pSceneEditor);

        pSceneEditor->UpdateSettings(SETTING_LOW);

        {
            KSceneSettingPageBase* pPage = dynamic_cast<KSceneSettingPageBase*>(m_PropertySheet.GetActivePage());
            if (pPage)
            {
                pPage->OnSetActive();
            }
        }
    }

Exit0:
    return;
}

void KSceneSettingDialog::OnBnClickedRadioSettingadvanced()
{
    HRESULT hRetCode = E_FAIL;
    DWORD dwType = 1;
    IEKG3DEnvironment* pEnvironment = KSH::GetEngineInterfaceEx<IEKG3DEnvironment>();
    KG_PROCESS_ERROR(pEnvironment);

    dwType = pEnvironment->GetSettingType();
    if (dwType != SETTING_ADVANCED)
    {
        IEKG3DSceneSceneEditor* pSceneEditor = KSH::GetEngineInterfaceEx<IEKG3DSceneSceneEditor>();
        KG_PROCESS_ERROR(pSceneEditor);

        pSceneEditor->UpdateSettings(SETTING_ADVANCED);

        {
            KSceneSettingPageBase* pPage = dynamic_cast<KSceneSettingPageBase*>(m_PropertySheet.GetActivePage());
            if (pPage)
            {
                pPage->OnSetActive();
            }
        }
    }

Exit0:
    return;
}