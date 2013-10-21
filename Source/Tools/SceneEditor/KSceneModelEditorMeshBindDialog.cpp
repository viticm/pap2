// KSceneModelEditorMeshBindDialog.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneModelEditorMeshBindDialog.h"
#include "IEEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorMeshBindDialog dialog

KSceneModelEditorMeshBindDialog::KSceneModelEditorMeshBindDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneModelEditorMeshBindDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(KSceneModelEditorMeshBindDialog)
	//}}AFX_DATA_INIT
	m_pModel = NULL;
	m_pszSocketName = NULL;
	//m_lpModelEditor = NULL;
	m_bSelectChild = TRUE;
}


void KSceneModelEditorMeshBindDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneModelEditorMeshBindDialog)
	DDX_Control(pDX, IDC_LIST1, m_ListBoxModel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KSceneModelEditorMeshBindDialog, CDialog)
	//{{AFX_MSG_MAP(KSceneModelEditorMeshBindDialog)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorMeshBindDialog message handlers

BOOL KSceneModelEditorMeshBindDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	FillEntityListBox();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneModelEditorMeshBindDialog::OnSelchangeList1() 
{
	// TODO: Add your control notification handler code here
	
}

void KSceneModelEditorMeshBindDialog::FillEntityListBox()
{
	TCHAR strFileName[MAX_PATH];
	CString strName;
	KG_PROCESS_ERROR(m_pEditor);
	
	m_ListBoxModel.ResetContent();

	INT nCount = 0;
	m_pEditor->GetSocketCount(&nCount);
	for (INT i = 0; i < nCount; i ++)//m_vecModelsForSockets.size(); i++)
	{
		//_splitpath(m_pEditor->m_vecModelsForSockets[i]->m_scName.c_str(), NULL, NULL, strFileName, NULL);
		m_pEditor->GetSocketName(i, strName.GetBuffer(MAX_PATH + 1), MAX_PATH);
		strName.ReleaseBuffer();
		_splitpath(strName, NULL, NULL, strFileName, NULL);
		m_ListBoxModel.AddString(strName);//strFileName);
	}
	
Exit0:
	return;
}


void KSceneModelEditorMeshBindDialog::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
}

void KSceneModelEditorMeshBindDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
}

void KSceneModelEditorMeshBindDialog::OnBnClickedButtonAdd()
{
	HRESULT hr = S_OK;
	char bigBuff[800] = "";  // maximum common dialog buffer size
	char szFilter[] ="Mesh Files (*.Mesh)|*.Mesh|X Files (*.x)| *.x|SpeedTree Files (*.STree)| *.STree|体积雾 (*.lvf)| *.lvf|闪电 (*.dlf)| *.dlf||";
	if (!m_pEditor)
		return;

	CFileDialog dlg(true, NULL, NULL,
		OFN_HIDEREADONLY , szFilter);
	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	
	if (dlg.DoModal() == IDOK)
	{
		
		CString strPathName = dlg.GetPathName();
		m_pEditor->LoadModelForSockets(strPathName);
	}
	FillEntityListBox();
}

void KSceneModelEditorMeshBindDialog::OnBnClickedOk()
{
	
	int nIndex = m_ListBoxModel.GetCurSel();
	IEKG3DModel* pModel = NULL;
	KG_PROCESS_ERROR(m_pEditor);
	KG_PROCESS_ERROR(nIndex >= 0);
	//KG_PROCESS_ERROR(nIndex < (int)m_pEditor->m_vecModelsForSockets.size());

	//pModel = m_pEditor->m_vecModelsForSockets[nIndex];
	m_pEditor->GetModelForSockets(nIndex, &pModel);//by dengzhihui Refactor 2006年11月24日
	KG_PROCESS_ERROR(pModel);

	pModel->BindToSocket(m_pModel, m_pszSocketName);
Exit0:
	OnOK();
}

void KSceneModelEditorMeshBindDialog::SetSocket(IEKG3DSceneModelEditor* pEditor, IEKG3DModel* pModel, LPCTSTR pszSocketName)
{
	m_pModel = pModel;
	m_pszSocketName = pszSocketName;
	m_pEditor = pEditor;
	m_bSelectChild = TRUE;
}

void KSceneModelEditorMeshBindDialog::SetChildModel(IEKG3DSceneModelEditor* pEditor, IEKG3DModel* pModel)
{
	m_pModel = pModel;
	m_pEditor = pEditor;
	m_pszSocketName = NULL;
	m_bSelectChild = FALSE;
}
