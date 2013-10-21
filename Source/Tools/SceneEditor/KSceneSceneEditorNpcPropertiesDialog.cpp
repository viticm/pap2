// KSceneSceneEditorNpcPropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneSceneEditorNpcPropertiesDialog.h"
#include ".\kscenesceneeditornpcpropertiesdialog.h"
//#include "SceneSkinEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorNpcPropertiesDialog dialog


KSceneSceneEditorNpcPropertiesDialog::KSceneSceneEditorNpcPropertiesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorNpcPropertiesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(KSceneSceneEditorNpcPropertiesDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	//m_lpSceneEditor = NULL;
}


void KSceneSceneEditorNpcPropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneSceneEditorNpcPropertiesDialog)
	DDX_Control(pDX, IDC_EDIT1, m_Edit_ScriptFile);
	DDX_Control(pDX, IDC_STATIC_TIP, m_Static_Tip);
	DDX_Control(pDX, IDC_LIST_SCRIPT, m_List_Script);
	DDX_Control(pDX, IDC_EDIT_NAME, m_Edit_Name);
	DDX_Control(pDX, IDC_EDIT_LEVEL, m_Edit_Level);
	DDX_Control(pDX, IDC_EDIT_KIND, m_Edit_Kind);
	DDX_Control(pDX, IDC_COMBO_TEMPLATEID, m_ComboBox_TemplateID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorNpcPropertiesDialog, CDialog)
	//{{AFX_MSG_MAP(KSceneSceneEditorNpcPropertiesDialog)
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_SETFILE, OnBnClickedButtonSetfile)
	ON_EN_CHANGE(IDC_EDIT_NAME, OnEnChangeEditName)
	ON_EN_CHANGE(IDC_EDIT_KIND, OnEnChangeEditKind)
	ON_EN_CHANGE(IDC_EDIT_LEVEL, OnEnChangeEditLevel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorNpcPropertiesDialog message handlers

void KSceneSceneEditorNpcPropertiesDialog::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

int KSceneSceneEditorNpcPropertiesDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here


	return 0;
}

BOOL KSceneSceneEditorNpcPropertiesDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	FillComBo();

	/*KRepNPC* pRepNpc = GetFirstSelectedNPC();
	if(pRepNpc)
	{
		if(pRepNpc->m_pNpcTemplate)
		{
			m_ComboBox_TemplateID.SetCurSel(pRepNpc->m_pNpcTemplate->m_dwTemplateID - 1);
		}
		TCHAR Name[256];
		if(SUCCEEDED(pRepNpc->GetLogicalName(Name)))
		{
			m_Edit_Name.SetWindowText(Name);
		}

		int nKind = 0;
		if(SUCCEEDED(pRepNpc->GetLogicalKind(&nKind)))
		{
			wsprintf(Name,"%d",nKind);
			m_Edit_Kind.SetWindowText(Name);
		}

		int nLevel = 0;
		if(SUCCEEDED(pRepNpc->GetLogicalLevel(&nLevel)))
		{
			wsprintf(Name,"%d",nLevel);
			m_Edit_Level.SetWindowText(Name);
		}

		m_Edit_ScriptFile.SetWindowText(pRepNpc->m_szLogicalScriptFile.c_str());
	}*/
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSceneEditorNpcPropertiesDialog::FillComBo()
{
	{
		for(int i=100;i>=0;i--)
		m_ComboBox_TemplateID.DeleteString(i);
	}

	//size_t nMaxID = g_pSO3World->m_Settings.m_NpcTemplateList.GetMaxTemplateID();
	//for (int nIndex = 1; nIndex <= (int)nMaxID; nIndex++)
	//{
	//	KNpcTemplate* pNpcTemplate = 
	//		g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(nIndex);
	//	if (pNpcTemplate != NULL && pNpcTemplate->m_dwTemplateID != 0)
	//	{
	//		TCHAR Name[256];
	//		wsprintf(Name,"%d %s",nIndex,pNpcTemplate->m_szName);
	//		int hItem = m_ComboBox_TemplateID.AddString(Name);
	//		m_ComboBox_TemplateID.SetItemData(hItem, (DWORD_PTR)pNpcTemplate);
	//	}
	//}

}

//KRepNPC*  KSceneSceneEditorNpcPropertiesDialog::GetFirstSelectedNPC()
//{
//	if(!m_lpSceneEditor)
//		return NULL;
//	if(m_lpSceneEditor->m_SelectedList.size()==0)
//		return NULL;
//	list<KSceneEntity>::iterator i = m_lpSceneEditor->m_SelectedList.begin();
//	KSceneEntity  Entity = *i;
//	if(Entity.Type == SCENEENTITY_OBJECT)
//	{
//		LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
//		if(pObject->m_dwType==OBJECTTYPE_NPC)
//		{
//			KRepNPC* pNPC = (KRepNPC*) Entity.Pointer;
//			return pNPC;
//		}
//		else
//		{
//			return NULL;
//		}
//	}
//	else
//	{
//		return NULL;
//	}
//}

void KSceneSceneEditorNpcPropertiesDialog::OnBnClickedButtonSetfile()
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpSceneEditor)
		return ;

	KRepNPC* pRepNpc = GetFirstSelectedNPC();
	if(!pRepNpc)
		return ;

	CFileDialog dlg(true, "脚本", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "脚本文件(*.Lua)|*.Lua||");
	if(dlg.DoModal() != IDOK)
	{
		return;
	}

	TCHAR str[256];
	wsprintf(str,"%s",dlg.GetPathName());
	pRepNpc->SetLogicalScript(str);

	m_Edit_ScriptFile.SetWindowText(str);*/
}

void KSceneSceneEditorNpcPropertiesDialog::OnEnChangeEditName()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	/*if(!m_lpSceneEditor)
		return ;

	KRepNPC* pRepNpc = GetFirstSelectedNPC();
	if(!pRepNpc)
		return ;

	TCHAR str[256];
	m_Edit_Name.GetWindowText(str,256);

	pRepNpc->SetLogicalName(str);*/
}

void KSceneSceneEditorNpcPropertiesDialog::OnEnChangeEditKind()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	/*if(!m_lpSceneEditor)
		return ;

	KRepNPC* pRepNpc = GetFirstSelectedNPC();
	if(!pRepNpc)
		return ;

	TCHAR str[256];
	m_Edit_Kind.GetWindowText(str,256);

	int nKind = 0;
	sscanf(str,"%d",&nKind);*/

	//pRepNpc->set
}

void KSceneSceneEditorNpcPropertiesDialog::OnEnChangeEditLevel()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	/*if(!m_lpSceneEditor)
		return ;

	KRepNPC* pRepNpc = GetFirstSelectedNPC();
	if(!pRepNpc)
		return ;

	TCHAR str[256];
	m_Edit_Level.GetWindowText(str,256);

	int nLevel = 0;
	sscanf(str,"%d",&nLevel);

	pRepNpc->SetLogicalLevel(nLevel);*/
}
