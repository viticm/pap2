// KSceneDataFlowEditorDefineDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneDataFlowEditorDefineDialog.h"
#include "IEEditor.h"

// KSceneDataFlowEditorDefineDialog dialog

IMPLEMENT_DYNAMIC(KSceneDataFlowEditorDefineDialog, CDialog)

KSceneDataFlowEditorDefineDialog::KSceneDataFlowEditorDefineDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneDataFlowEditorDefineDialog::IDD, pParent)
	, m_szDefineName(_T(""))
	, m_dwNumVariable(0)
	, m_szVariableName(_T(""))
{
	m_lpEditor = NULL;
	m_uCurrentSelectedDefine = 0;
	m_nCurrentSelectedVariable = 0;
}

KSceneDataFlowEditorDefineDialog::~KSceneDataFlowEditorDefineDialog()
{
	m_uCurrentSelectedDefine = 0;
	m_nCurrentSelectedVariable = 0;
}

void KSceneDataFlowEditorDefineDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DEFINE, m_ListDefine);
	DDX_Text(pDX, IDC_EDIT_DEFINENAME, m_szDefineName);
	DDX_Text(pDX, IDC_EDIT_NUMVARIABLE, m_dwNumVariable);
	DDX_Control(pDX, IDC_LIST_VARIABLE, m_ListVariable);
	DDX_Text(pDX, IDC_EDIT_VARIABLENAME, m_szVariableName);
	DDX_Control(pDX, IDC_COMBO_VARIABLETYPE, m_ComboVariableType);
}




BEGIN_MESSAGE_MAP(KSceneDataFlowEditorDefineDialog, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DEFINE, &KSceneDataFlowEditorDefineDialog::OnLvnItemchangedListDefine)
	ON_BN_CLICKED(IDC_BUTTON_ADDDEFINE, &KSceneDataFlowEditorDefineDialog::OnBnClickedButtonAdddefine)
	ON_BN_CLICKED(IDC_BUTTON_DELETEDEFINE, &KSceneDataFlowEditorDefineDialog::OnBnClickedButtonDeletedefine)
	ON_EN_CHANGE(IDC_EDIT_DEFINENAME, &KSceneDataFlowEditorDefineDialog::OnEnChangeEditDefinename)
	ON_EN_CHANGE(IDC_EDIT_NUMVARIABLE, &KSceneDataFlowEditorDefineDialog::OnEnChangeEditNumvariable)
	ON_LBN_SELCHANGE(IDC_LIST_VARIABLE, &KSceneDataFlowEditorDefineDialog::OnLbnSelchangeListVariable)
	ON_EN_CHANGE(IDC_EDIT_VARIABLENAME, &KSceneDataFlowEditorDefineDialog::OnEnChangeEditVariablename)
	ON_CBN_SELCHANGE(IDC_COMBO_VARIABLETYPE, &KSceneDataFlowEditorDefineDialog::OnCbnSelchangeComboVariabletype)
	ON_BN_CLICKED(IDC_BUTTON_SETNUMVARIABLE, &KSceneDataFlowEditorDefineDialog::OnBnClickedButtonSetnumvariable)
END_MESSAGE_MAP()


// KSceneDataFlowEditorDefineDialog message handlers

void KSceneDataFlowEditorDefineDialog::OnLvnItemchangedListDefine(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	//m_uCurrentSelectedDefine = 0;
	if(!m_lpEditor)
		return;

	POSITION ps = m_ListDefine.GetFirstSelectedItemPosition();
	while (ps)
	{
		int nIndex = m_ListDefine.GetNextSelectedItem(ps);
		if(nIndex>=0)
		{
			m_uCurrentSelectedDefine = (UINT)m_ListDefine.GetItemData(nIndex);
		}
	}

	if(m_uCurrentSelectedDefine)
	{
		TCHAR Name[MAX_PATH];
		if(SUCCEEDED(m_lpEditor->GetDataDefineName(Name,m_uCurrentSelectedDefine)))
		{

			DWORD dwNum = 0;
			m_lpEditor->GetNumVariableofDataDefine(&dwNum,m_uCurrentSelectedDefine);

			m_szDefineName.Format("%s",Name);
			m_dwNumVariable = dwNum;
			UpdateData(FALSE);
		}

		FillListVariable();

	}
	*pResult = 0;
}

void KSceneDataFlowEditorDefineDialog::OnBnClickedButtonAdddefine()
{
	KGLOG_PROCESS_ERROR(m_lpEditor);

	UINT uHandle = 0;
	m_lpEditor->NewOneDataDefine(&uHandle);

	FillListDefine();
Exit0:
	;
}

void KSceneDataFlowEditorDefineDialog::OnBnClickedButtonDeletedefine()
{
	// TODO: Add your control notification handler code here
}

void KSceneDataFlowEditorDefineDialog::OnEnChangeEditDefinename()
{
	UpdateData(TRUE);
	if(!m_lpEditor)
		return;
	if (m_uCurrentSelectedDefine)
	{
		m_lpEditor->SetDataDefineName(m_szDefineName,m_uCurrentSelectedDefine);
	}
	FillListDefine();
}

void KSceneDataFlowEditorDefineDialog::OnEnChangeEditNumvariable()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void KSceneDataFlowEditorDefineDialog::OnLbnSelchangeListVariable()
{
	if(!m_lpEditor)
		return;

	if(m_uCurrentSelectedDefine)
	{
		int nIndex = m_ListVariable.GetCurSel();

		if(nIndex>=0 )
		{
			m_nCurrentSelectedVariable = nIndex;
			KG3DDATAFLOWVARIABLETYPE eType;
			TCHAR Name[MAX_PATH];
			if(SUCCEEDED(m_lpEditor->GetVariableofDataDefine(m_nCurrentSelectedVariable,&eType,Name,m_uCurrentSelectedDefine)))
			{
				m_szVariableName.Format("%s",Name);

				m_ComboVariableType.SetCurSel(eType);
				UpdateData(FALSE);
			}
		}
	}
}

void KSceneDataFlowEditorDefineDialog::OnEnChangeEditVariablename()
{
	if(!m_lpEditor)
		return;

	UpdateData(TRUE);

	if(m_uCurrentSelectedDefine)
	{		
		if(m_nCurrentSelectedVariable>=0)
		{
			KG3DDATAFLOWVARIABLETYPE eType;
			TCHAR Name[MAX_PATH];
			if(SUCCEEDED(m_lpEditor->GetVariableofDataDefine(m_nCurrentSelectedVariable,&eType,Name,m_uCurrentSelectedDefine)))
			{
				m_lpEditor->SetVariableofDataDefine(m_nCurrentSelectedVariable,eType,m_szVariableName,m_uCurrentSelectedDefine);
				FillListVariable();
			}
		}
	}
}

void KSceneDataFlowEditorDefineDialog::OnCbnSelchangeComboVariabletype()
{
	if(!m_lpEditor)
		return;

	UpdateData(TRUE);

	if(m_uCurrentSelectedDefine)
	{		
		if(m_nCurrentSelectedVariable>=0)
		{
			KG3DDATAFLOWVARIABLETYPE eType;
			TCHAR Name[MAX_PATH];
			if(SUCCEEDED(m_lpEditor->GetVariableofDataDefine(m_nCurrentSelectedVariable,&eType,Name,m_uCurrentSelectedDefine)))
			{
				eType = (KG3DDATAFLOWVARIABLETYPE)m_ComboVariableType.GetCurSel();
				m_lpEditor->SetVariableofDataDefine(m_nCurrentSelectedVariable,eType,m_szVariableName,m_uCurrentSelectedDefine);
				
				FillListVariable();
			}
		}
	}
}

HRESULT KSceneDataFlowEditorDefineDialog::FillListDefine()
{
	vector<UINT>vecDefine;
	m_ListDefine.DeleteAllItems();
	KGLOG_PROCESS_ERROR(m_lpEditor);

	m_lpEditor->GetAllDataDefine(&vecDefine);
	for (size_t i=0;i<vecDefine.size();i++)
	{
		UINT uHandle = vecDefine[i];

		TCHAR name[MAX_PATH];

		m_lpEditor->GetDataDefineName(name,uHandle);

		int nIndex = m_ListDefine.InsertItem(0,name);
		m_ListDefine.SetItemData(nIndex,uHandle);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KSceneDataFlowEditorDefineDialog::FillListVariable()
{
	if(!m_lpEditor)
		return E_FAIL;
	{
		m_ListVariable.SetCurSel(-1);
		int nCount = m_ListVariable.GetCount();
		for (int i=0;i<nCount;i++)
		{
			m_ListVariable.DeleteString(0);
		}
	}

	if(m_uCurrentSelectedDefine)
	{
		DWORD dwNum = 0;

		if(FAILED(m_lpEditor->GetNumVariableofDataDefine(&dwNum,m_uCurrentSelectedDefine)))
			return E_FAIL;

		for (DWORD i=0;i<dwNum;i++)
		{
			KG3DDATAFLOWVARIABLETYPE eType;
			TCHAR Name[MAX_PATH];
			TCHAR TotalName[MAX_PATH];
			TCHAR typeName[MAX_PATH];

			m_lpEditor->GetVariableofDataDefine(i,&eType,Name,m_uCurrentSelectedDefine);
			m_lpEditor->GetVariableType(&eType,typeName,(DWORD)eType);

			wsprintf(TotalName,"%s : %s",Name,typeName);
			m_ListVariable.AddString(TotalName);
		}
	}
	return S_OK;
}

HRESULT KSceneDataFlowEditorDefineDialog::FillComboVariable()
{
	if(!m_lpEditor)
		return E_FAIL;

	{
		int nCount = m_ComboVariableType.GetCount();
		for (int i=0;i<nCount;i++)
		{
			m_ComboVariableType.DeleteString(0);
		}
	}

	DWORD dwNumType = 0;
	m_lpEditor->GetNumVariableType(&dwNumType);
	for (DWORD i=0;i<dwNumType;i++)
	{
		KG3DDATAFLOWVARIABLETYPE eType;
		TCHAR Name[MAX_PATH];

		if (SUCCEEDED(m_lpEditor->GetVariableType(&eType,Name,i)))
		{
			int nIndex = m_ComboVariableType.AddString(Name);
			m_ComboVariableType.SetItemData(nIndex,eType);
		}
		;
	}

	return S_OK;
}

BOOL KSceneDataFlowEditorDefineDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_ListDefine.InsertColumn(0, _T("Name"),	LVCFMT_CENTER, 200);

	m_ListDefine.SetExtendedStyle(
		m_ListDefine.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
		);

	FillComboVariable();

	FillListDefine();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneDataFlowEditorDefineDialog::OnBnClickedButtonSetnumvariable()
{
	if(!m_lpEditor)
		return ;
	if (m_uCurrentSelectedDefine)
	{
		UpdateData(TRUE);

		m_lpEditor->SetNumVariableofDataDefine(m_dwNumVariable,m_uCurrentSelectedDefine);

		FillListVariable();
	}
}
