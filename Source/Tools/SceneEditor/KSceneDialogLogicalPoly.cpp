// KSceneDialogLogicalPoly.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneDialogLogicalPoly.h"


// KSceneDialogLogicalPoly dialog

IMPLEMENT_DYNAMIC(KSceneDialogLogicalPoly, CDialog)

KSceneDialogLogicalPoly::KSceneDialogLogicalPoly(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneDialogLogicalPoly::IDD, pParent)
	, m_szName(_T(""))
	, m_fHeight(0)
{
	m_lpPoly = NULL;
}



KSceneDialogLogicalPoly::~KSceneDialogLogicalPoly()
{
}



void KSceneDialogLogicalPoly::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ComBo);
	DDX_Text(pDX, IDC_EDIT1, m_szName);
	DDX_Text(pDX, IDC_EDIT7, m_fHeight);
}


BEGIN_MESSAGE_MAP(KSceneDialogLogicalPoly, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO1, &KSceneDialogLogicalPoly::OnCbnSelchangeCombo1)
	ON_EN_CHANGE(IDC_EDIT1, &KSceneDialogLogicalPoly::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT7, &KSceneDialogLogicalPoly::OnEnChangeEdit7)
END_MESSAGE_MAP()


// KSceneDialogLogicalPoly message handlers

int KSceneDialogLogicalPoly::FindLogicalSetByName(DWORD& dwModify,LPSTR pScriptName,LPSTR pName)
{
	for (size_t i=0;i<m_vecLogicalSet.size();i++)
	{
		if(_strcmpi(m_vecLogicalSet[i].szName.c_str(),pName)==0)
		{
			dwModify = m_vecLogicalSet[i].dwModifyState;
			if (m_vecLogicalSet[i].szScriptFile.find("\\scripts\\") == 0)
			{
				wsprintf(pScriptName,"%s",m_vecLogicalSet[i].szScriptFile.substr(1).c_str());
			}
			else
			{
				wsprintf(pScriptName,"%s",m_vecLogicalSet[i].szScriptFile.c_str());
			}
			return i;
		}
	}
	return -1;
}

BOOL KSceneDialogLogicalPoly::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	m_ComBo.ResetContent();
	m_ComBo.AddString("Î´ÉèÖÃ");

	for (size_t i=0;i<m_vecLogicalSet.size();i++)
	{
		int n = m_ComBo.AddString(m_vecLogicalSet[i].szName.c_str());
	}
	if(m_lpPoly)
	{
		string strName = m_lpPoly->GetLogicalSetName();
		if(strcmp(strName.c_str(),"")!=0)
		{
			int nSel = m_ComBo.FindStringExact(0,strName.c_str());
			m_ComBo.SetCurSel(nSel);
		}
		else
			m_ComBo.SetCurSel(0);
	}

	if(m_lpPoly)
	{
		m_szName.Format("%s",m_lpPoly->GetPolyName());
		m_fHeight = m_lpPoly->GetVertexHeight();
		UpdateData(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneDialogLogicalPoly::OnCbnSelchangeCombo1()
{
	int n = m_ComBo.GetCurSel();


	if(m_lpPoly)
	{
		if(n==0)
		{
			m_lpPoly->SetScriptFileName("");
			m_lpPoly->SetColor(0xFFFFFFFF);
			m_lpPoly->SetLogicalSetName("");
			m_lpPoly->SetLogicalSetID(n);
			m_lpPoly->SetModifyState(0);
		}
		else
		{
			int nIndex = n - 1;
			
			IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
			if(pManager)
			{
				TCHAR szAppName[MAX_PATH];
				TCHAR RetFileName[MAX_PATH];
				pManager->GetDefWorkDirectory(szAppName, MAX_PATH);
				pManager->GetRelativePath(szAppName,m_vecLogicalSet[nIndex].szScriptFile.c_str(),RetFileName);
				m_lpPoly->SetScriptFileName(RetFileName);
			}
			else
			{
				m_lpPoly->SetScriptFileName(m_vecLogicalSet[nIndex].szScriptFile.c_str());
			}
			
			m_lpPoly->SetColor(m_vecLogicalSet[nIndex].dwColor);
			m_lpPoly->SetLogicalSetName(m_vecLogicalSet[nIndex].szName);
			m_lpPoly->SetLogicalSetID(nIndex);
			m_lpPoly->SetModifyState(m_vecLogicalSet[nIndex].dwModifyState);
		}
	}
}

void KSceneDialogLogicalPoly::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);

	if(m_lpPoly)
	{
		m_lpPoly->SetPolyName(m_szName);
	}
}

void KSceneDialogLogicalPoly::OnEnChangeEdit7()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	if(m_lpPoly)
	{
		m_lpPoly->SetVertexHeight(m_fHeight);
	}
}

HRESULT KSceneDialogLogicalPoly::AddLogicalSet(DWORD dwColor,INT nID,LPCTSTR pName,LPCTSTR pScriptName,DWORD dwModifyState)
{
	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	TCHAR szAppDirectory[MAX_PATH] = { 0 };
	TCHAR szRetFileName[MAX_PATH] = {0};

	pManager->GetDefWorkDirectory(szAppDirectory, MAX_PATH);
	pManager->GetRelativePath(szAppDirectory,pScriptName,szRetFileName);


	LogicalSet set;
	set.dwColor = dwColor;
	set.nID = nID;
	set.szName = pName;
	set.szScriptFile = szRetFileName;
	set.dwModifyState = dwModifyState;

	m_vecLogicalSet.push_back(set);
	return S_OK;
}