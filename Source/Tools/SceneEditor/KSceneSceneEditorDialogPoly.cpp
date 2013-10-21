// KSceneSceneEditorDialogPoly.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorDialogPoly.h"
#include "IEEditor.h"
#include "SceneEditorDoc.h"
#include "KSceneEditorDocLogical.h"
#include "KSceneDialogLogicalPoly.h"
#include "CS3LogicForm.h"
#include "comutil.h"

extern DWORD TransToEngineFormat(long lLogicInfo);
// KSceneSceneEditorDialogPoly dialog

IMPLEMENT_DYNAMIC(KSceneSceneEditorDialogPoly, CDialog)

KSceneSceneEditorDialogPoly::KSceneSceneEditorDialogPoly(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogPoly::IDD, pParent)
	, m_fHeight(0)
	, m_nVergeNum(4)
	, m_fRadii(1000)
	, m_fDegree(45)
{
	m_lpDoc = NULL;
	m_lpEditor = NULL;
}

KSceneSceneEditorDialogPoly::~KSceneSceneEditorDialogPoly()
{
}

void KSceneSceneEditorDialogPoly::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_fHeight);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Text(pDX, IDC_EDIT_VERGENUM, m_nVergeNum);
	DDX_Text(pDX, IDC_EDIT_RADII, m_fRadii);
	DDX_Text(pDX, IDC_EDIT_DEGREE, m_fDegree);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogPoly, CDialog)
	ON_BN_CLICKED(IDC_NEWPOLY, &KSceneSceneEditorDialogPoly::OnBnClickedNewpoly)
	ON_BN_CLICKED(IDC_END, &KSceneSceneEditorDialogPoly::OnBnClickedEnd)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, &KSceneSceneEditorDialogPoly::OnEnChangeEditHeight)
	ON_BN_CLICKED(IDC_SHOWPOLY, &KSceneSceneEditorDialogPoly::OnBnClickedShowpoly)
	ON_BN_CLICKED(IDC_BUTTON1, &KSceneSceneEditorDialogPoly::OnBnClickedButton1)
	ON_LBN_DBLCLK(IDC_LIST1, &KSceneSceneEditorDialogPoly::OnLbnDblclkList1)
	ON_LBN_SELCHANGE(IDC_LIST1, &KSceneSceneEditorDialogPoly::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON_SET, &KSceneSceneEditorDialogPoly::OnBnClickedButtonSet)
	ON_BN_CLICKED(IDC_BUILD_POLY, &KSceneSceneEditorDialogPoly::OnBnClickedBuildPoly)
	ON_EN_CHANGE(IDC_EDIT_VERGENUM, &KSceneSceneEditorDialogPoly::OnEnChangeEditVergenum)
	ON_EN_CHANGE(IDC_EDIT_DEGREE, &KSceneSceneEditorDialogPoly::OnEnChangeEditDegree)
	ON_EN_CHANGE(IDC_EDIT_RADII, &KSceneSceneEditorDialogPoly::OnEnChangeEditRadii)
END_MESSAGE_MAP()


// KSceneSceneEditorDialogPoly message handlers

void KSceneSceneEditorDialogPoly::OnBnClickedNewpoly()
{
	if(!m_lpEditor)
		return;
	m_lpEditor->BeginAddPolyVertex();
}

void KSceneSceneEditorDialogPoly::OnBnClickedEnd()
{
	if(!m_lpEditor)
		return;
	m_lpEditor->EndAddPolyVertex(0);
}

void KSceneSceneEditorDialogPoly::OnEnChangeEditHeight()
{
	UpdateData(TRUE);
	if(!m_lpEditor)
		return;
	m_lpEditor->SetPolyHeight(m_fHeight);
}

BOOL KSceneSceneEditorDialogPoly::OnInitDialog()
{
	CDialog::OnInitDialog();
	BOOL bVal = TRUE;
	// TODO:  Add extra initialization here
	if(m_lpEditor)
	{
		m_fHeight = m_lpEditor->GetPolyHeight();
		bVal = m_lpEditor->IsRenderPoly();
	}
	((CButton*)(GetDlgItem(IDC_SHOWPOLY)))->SetCheck(bVal);
	UpdateData(FALSE);

	return TRUE;  
	// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSceneEditorDialogPoly::OnBnClickedShowpoly()
{
	BOOL bcheck = ((CButton*)(GetDlgItem(IDC_SHOWPOLY)))->GetCheck();
	if(m_lpEditor)
	{
		m_lpEditor->EnableRenderPoly(bcheck);
	}
}

void KSceneSceneEditorDialogPoly::OnBnClickedButton1()
{
	FillList();
}

HRESULT GetLogicalformItem(DWORD& dwModify,LPSTR pName,LPSTR pScriptName,int nIndex,KSceneEditorDoc* pDoc)
{
	int nCount = pDoc->m_lpLogicalform->get_Count();

	if( nIndex<0 || nIndex>=nCount )
		return E_FAIL;

	long lLogicInfo = 0;
	_bstr_t cbstr = "";
	_bstr_t cbstr2 = "";
	BSTR bstr = cbstr.Detach();
	BSTR bstr2 = cbstr2.Detach();

	pDoc->m_lpLogicalform->GetItem(nIndex,&bstr2, &lLogicInfo, &bstr);

	cbstr.Attach(bstr);
	cbstr2.Attach(bstr2);

	wsprintf(pName,"%S",bstr2);
	wsprintf(pScriptName,"%S",bstr);

	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	TCHAR szAppDirectory[MAX_PATH] = { 0 };
	TCHAR szRetFileName[MAX_PATH] = { 0 };

	pManager->GetDefWorkDirectory(szAppDirectory, MAX_PATH);
	pManager->GetRelativePath(szAppDirectory,pScriptName,pScriptName);

	dwModify = TransToEngineFormat(lLogicInfo);
	return S_OK;
}

void KSceneSceneEditorDialogPoly::FillList()
{
	m_List.ResetContent();

	KSceneDialogLogicalPoly dlg;
	{
		int nCount = m_lpDoc->m_lpLogicalform->get_Count();
		long lLogicInfo = 0;
		for (int i = 0; i < nCount; i ++)
		{
			_bstr_t cbstr = "";
			_bstr_t cbstr2 = "";
			BSTR bstr = cbstr.Detach();
			BSTR bstr2 = cbstr2.Detach();

			m_lpDoc->m_lpLogicalform->GetItem(i,&bstr2, &lLogicInfo, &bstr);

			cbstr.Attach(bstr);
			cbstr2.Attach(bstr2);

			TCHAR Name[MAX_PATH];
			wsprintf(Name,"%S",bstr2);
			TCHAR ScriptName[MAX_PATH];
			wsprintf(ScriptName,"%S",bstr);

			DWORD dwModify = TransToEngineFormat(lLogicInfo);

			dlg.AddLogicalSet(dwModify,i,Name,ScriptName,dwModify);
		}
	}


	int N = 0;
	list<IEKG3DRepresentPoly*>::iterator i = m_lpDoc->m_lpLogicalData->m_listLogicalPoly.begin();
	list<IEKG3DRepresentPoly*>::iterator iend = m_lpDoc->m_lpLogicalData->m_listLogicalPoly.end();
	while (i!=iend)
	{
		IEKG3DRepresentPoly* pPoly = *i;
		
		TCHAR szFileName[MAX_PATH];
		TCHAR szLogicalSetName[MAX_PATH];
		int   nLogicalSetIndex = 0;
		DWORD dwModifyState = 0;

		nLogicalSetIndex = pPoly->GetLogicalSetID();
		dwModifyState    = pPoly->GetModifyState();
		wsprintf(szFileName,"%s",pPoly->GetScriptFileName());
		wsprintf(szLogicalSetName,"%s",pPoly->GetLogicalSetName());

		if(_strcmpi(szLogicalSetName,"")==0)
		{
			if(SUCCEEDED(GetLogicalformItem(dwModifyState,szLogicalSetName,szFileName,nLogicalSetIndex,m_lpDoc)))
			{
				pPoly->SetModifyState(dwModifyState);
				pPoly->SetLogicalSetName(szLogicalSetName);
				pPoly->SetScriptFileName(szFileName);
			}
		}
		else
		{
			nLogicalSetIndex = dlg.FindLogicalSetByName(dwModifyState,szFileName,szLogicalSetName);
			if(nLogicalSetIndex!=-1)
			{
				pPoly->SetModifyState(dwModifyState);
				pPoly->SetScriptFileName(szFileName);
			}
		}

		TCHAR szName[MAX_PATH];

		
		wsprintf(szName,"%s -- %s",pPoly->GetPolyName(),pPoly->GetLogicalSetName());

		int nIndex = m_List.AddString(szName);
		m_List.SetItemData(nIndex,N);

		i++;
		N++;
	}

}

void KSceneSceneEditorDialogPoly::OnLbnDblclkList1()
{
	int nIndex = m_List.GetCurSel();
	int nPolyIndex = (int)m_List.GetItemData(nIndex);
	IEKG3DRepresentPoly* pPoly = m_lpDoc->m_lpLogicalData->FindPolyByIndex(nPolyIndex);
	
	m_lpEditor->ClearSelectedEntityList();

	if(pPoly)
	{
		m_lpEditor->SetSelected(SCENEENTITY_OBJECT,pPoly->GetObjectPointer(), TRUE);

		PostMessage(WM_COMMAND,ID_ZOOMTO_OBJ, 0);
	}
}

void KSceneSceneEditorDialogPoly::OnLbnSelchangeList1()
{
	// TODO: Add your control notification handler code here
}


//HRESULT GetLogicalformItem(LPSTR pName,LPSTR pScriptName,int nIndex,KSceneEditorDoc* pDoc)
//{
//	long lLogicInfo = 0;
//	_bstr_t cbstr = "";
//	_bstr_t cbstr2 = "";
//	BSTR bstr = cbstr.Detach();
//	BSTR bstr2 = cbstr2.Detach();
//
//	pDoc->m_lpLogicalform->GetItem(nIndex,&bstr2, &lLogicInfo, &bstr);
//
//	cbstr.Attach(bstr);
//	cbstr2.Attach(bstr2);
//
//	wsprintf(pName,"%S",bstr2);
//	wsprintf(pScriptName,"%S",bstr);
//
//	return S_OK;
//}

void KSceneSceneEditorDialogPoly::OnBnClickedButtonSet()
{
	int nIndex = m_List.GetCurSel();
	int nPolyIndex = (int)m_List.GetItemData(nIndex);
	IEKG3DRepresentPoly* pPoly = m_lpDoc->m_lpLogicalData->FindPolyByIndex(nPolyIndex);

	if(pPoly)
	{
		KSceneDialogLogicalPoly dlg;

		int nCount = m_lpDoc->m_lpLogicalform->get_Count();
		long lLogicInfo = 0;
		for (int i = 0; i < nCount; i ++)
		{
			_bstr_t cbstr = "";
			_bstr_t cbstr2 = "";
			BSTR bstr = cbstr.Detach();
			BSTR bstr2 = cbstr2.Detach();

			m_lpDoc->m_lpLogicalform->GetItem(i,&bstr2, &lLogicInfo, &bstr);

			cbstr.Attach(bstr);
			cbstr2.Attach(bstr2);

			TCHAR Name[MAX_PATH];
			wsprintf(Name,"%S",bstr2);
			TCHAR ScriptName[MAX_PATH];
			wsprintf(ScriptName,"%S",bstr);

			DWORD dwModify = TransToEngineFormat(lLogicInfo);

			dlg.AddLogicalSet(dwModify,i,Name,ScriptName,dwModify);
		}

		dlg.SetPoly(pPoly);
		dlg.DoModal();
	}
}
void KSceneSceneEditorDialogPoly::OnEnChangeEditVergenum()
{
	UpdateData();
	if(m_nVergeNum < 3)
	{	
		m_nVergeNum = 3;
		UpdateData(FALSE);
	}
}

void KSceneSceneEditorDialogPoly::OnEnChangeEditRadii()
{
	UpdateData();
	if(m_fRadii < 100)
	{
		m_fRadii = 100;
		UpdateData(FALSE);
	}
}

void KSceneSceneEditorDialogPoly::OnEnChangeEditDegree()
{
	UpdateData();
}

void KSceneSceneEditorDialogPoly::OnBnClickedBuildPoly()
{
	if(!m_lpEditor)
		return;
	m_lpEditor->BuildPolyAndSetInHand(m_nVergeNum,m_fRadii,m_fHeight,m_fDegree);
}

