#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneEditorDialogCheckRelive.h"
#include "KSceneEditorDocLogical.h"
#include "filetype.h"
#include "IEEditor.h"

BEGIN_MESSAGE_MAP(KSceneEditorDialogCheckRelive, CDialog)
	ON_BN_CLICKED(ID_RECHECK, &KSceneEditorDialogCheckRelive::OnBnClickedButtonRecheck)
	ON_BN_CLICKED(ID_IGNORE, &KSceneEditorDialogCheckRelive::OnBnClickedButtonIgnore)
	ON_BN_CLICKED(IDC_CHECK, &KSceneEditorDialogCheckRelive::OnBnClickedButtonCheckEnable)
	ON_NOTIFY(NM_CLICK, IDC_TREE, &KSceneEditorDialogCheckRelive::OnNMClkTree)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()
KSceneEditorDialogCheckRelive* KSceneEditorDialogCheckRelive::This;
char* KSceneEditorDialogCheckRelive::s_szCurrentDir;

KSceneEditorDialogCheckRelive::KSceneEditorDialogCheckRelive(CWnd* pParentWnd) : CDialog(KSceneEditorDialogCheckRelive::IDD, pParentWnd)
{
	m_bReliveEnable = true;
	m_pSceneEditor = NULL;
	This = this;
}

void KSceneEditorDialogCheckRelive::SetCurrentDir()
{
	SAFE_DELETE(s_szCurrentDir);
	s_szCurrentDir = new char[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, s_szCurrentDir);
	char* pszFileName;
	char* szFileName = new char[MAX_PATH];
	KG_PROCESS_ERROR(szFileName);
	for (size_t i = strlen(s_szCurrentDir) - 1; i >= 0; i--)
	{
		if (s_szCurrentDir[i] == '\\')
		{
			pszFileName = s_szCurrentDir + i + 1;
			strcpy(szFileName, pszFileName);
			break;
		}
	}
	if (strcmp(szFileName, "测试") == 0)
	{
		strcat(szFileName, "1");
	}
	else if (strcmp(szFileName, "光") == 0)
	{
		strcpy(szFileName, "测试1");
	}
	else if (strcmp(szFileName, "登陆界面场景") == 0)
	{
		strcpy(szFileName, "登陆界面");
	}

	wsprintf(s_szCurrentDir, "%s\\%s%s", s_szCurrentDir, szFileName, ".Map.Logical");
Exit0:
	SAFE_DELETE(szFileName);
}

KSceneEditorDialogCheckRelive::~KSceneEditorDialogCheckRelive()
{
	SAFE_DELETE(s_szCurrentDir);
}

void KSceneEditorDialogCheckRelive::OnBnClickedButtonRecheck()
{
	OnShowWindow(true, 0);
}

void KSceneEditorDialogCheckRelive::OnBnClickedButtonCheckEnable()
{
	UpdateData(true);
	bool bEnable = ((CButton*)GetDlgItem(IDC_CHECK))->GetCheck();
	SetCheckReliveEnable(bEnable);
}

void KSceneEditorDialogCheckRelive::OnBnClickedButtonIgnore()
{
	//MessageBox("OnBnClickedButtonIgnore");
	ShowWindow(SW_HIDE);
}

void KSceneEditorDialogCheckRelive::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_Tree);
}

void KSceneEditorDialogCheckRelive::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (bShow)
	{
		m_Tree.DeleteAllItems();
	}
	bool bEnable = GetCheckReliveEnable();
	((CButton*)GetDlgItem(IDC_CHECK))->SetCheck(bEnable);
	
	LoadTree();
	UpdateData(false);
	return CDialog::OnShowWindow(bShow, nStatus);
}

BOOL KSceneEditorDialogCheckRelive::OnInitDialog()
{
	bool bRet = CDialog::OnInitDialog();

	return bRet;
}

HRESULT KSceneEditorDialogCheckRelive::LoadTree()
{
	HRESULT hResult = E_FAIL;
	//////////////////////
	ITabFile* pFile;
	char* szTabFileName = new char[MAX_PATH]; 
	std::list<KSceneEditorDocLogical::DoodadInfo*>::iterator pDoodadInfo;
	::GetCurrentDirectory(MAX_PATH, szTabFileName);
	wsprintf(szTabFileName, "%s%s", szTabFileName, "\\settings\\DoodadCheckList.tab");
	pFile = g_OpenTabFile(szTabFileName);
	std::list<int> IDlist;
	std::list<int>::iterator IDlistIterator;
	HTREEITEM hTreeItem = m_Tree.InsertItem("doodad对象树");
	HTREEITEM h;
	KG_PROCESS_ERROR(szTabFileName);

    KGLOG_PROCESS_ERROR(pFile);

	for (int i = 2; i <= pFile->GetHeight(); i++)
	{
		int temp;
		pFile->GetInteger(i, 1, 0, &temp); 
		IDlist.push_back(temp);
	}

	for (pDoodadInfo = m_lpDocLogical->m_listDoodadInfo.begin(); pDoodadInfo != m_lpDocLogical->m_listDoodadInfo.end(); ++pDoodadInfo)
	{
		IDlistIterator = find(IDlist.begin(), IDlist.end(), (*pDoodadInfo)->nTempleteID);
		if (IDlistIterator == IDlist.end())
		{
			continue;
		}
		if ((*pDoodadInfo)->dwDoodadReliveID == 0)
		{
			h = m_Tree.InsertItem((*pDoodadInfo)->szName.c_str(), hTreeItem);
			m_Tree.SetItemData(h, (DWORD_PTR)(*pDoodadInfo)->pPointer);
		}
	}
	hResult = S_OK;
Exit0:
	SAFE_DELETE(szTabFileName);
	IDlist.clear();
	SAFE_RELEASE(pFile);
	return hResult;
}

int KSceneEditorDialogCheckRelive::GetCheckReliveEnableFromFile()
{
	int nRet = false;
	int nResult = false;
	char* szCheckEnable = new char[10];
	char* szCurrentFile = s_szCurrentDir;
		
	KG_PROCESS_ERROR(szCurrentFile);
	KG_PROCESS_ERROR(szCheckEnable);

	::GetPrivateProfileString("Main", "CheckReliveEnable", "", szCheckEnable, 10, szCurrentFile);
	if (!strcmp(szCheckEnable, "true"))
	{
		This->SetCheckReliveEnable(true);
		nResult = true;
	}
	else if (!strcmp(szCheckEnable, "false"))
	{
		This->SetCheckReliveEnable(false);
		nResult = false;
	}
	else
	{
		This->SetCheckReliveEnable(true);
		SetCheckReliveEnableToFile();
		nResult = true;
	}

Exit0:
	SAFE_DELETE(szCheckEnable);
	return nResult;
}

int KSceneEditorDialogCheckRelive::SetCheckReliveEnableToFile()
{
	int nResult = false;
	int nRet = false;
	char* szCheckEnable;
	char* szCurrentFile = s_szCurrentDir;

	KG_PROCESS_ERROR(szCurrentFile);
	
	if (This->m_bReliveEnable)
	{
		szCheckEnable = "true";
	}
	else 
	{
		szCheckEnable = "false";
	}
	::WritePrivateProfileString("Main", "CheckReliveEnable", szCheckEnable, szCurrentFile);

	nResult = true;
Exit0:
	return nResult;
}

void KSceneEditorDialogCheckRelive::SetDocLogical(KSceneEditorDocLogical* lpDocLogical)
{
	m_lpDocLogical = lpDocLogical;
}

BOOL KSceneEditorDialogCheckRelive::IsLoseRelive()
{
	BOOL bResult = false;
	int nRet = 0;
	nRet = m_Tree.GetCount();
	if (nRet > 1)
	{
		bResult = true;
	}
	return bResult;
}

void KSceneEditorDialogCheckRelive::OnNMClkTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	HTREEITEM hitem = m_Tree.GetSelectedItem();
	
	if(!hitem)
		return;

	PVOID pPointer = (PVOID)m_Tree.GetItemData(hitem);
	if (!pPointer)
	{
		return;
	}
	D3DXVECTOR3 vPositionObj(0, 0, 0);
	KSceneEditorDocLogical::DoodadInfo* pDoodainfo = NULL;
	if(SUCCEEDED(m_lpDocLogical->FindDoodadInfoByEditorDoodad(&pDoodainfo, pPointer)))
    {    
		vPositionObj = pDoodainfo->vPosition;
    }

	AtlVector3 oldpos, oldlookat, delta, newpos, newlookat;

	m_pSceneEditor->GetCameraPosLookat(&oldpos, &oldlookat);
	delta.x = oldlookat.x - oldpos.x; delta.y = oldlookat.y - oldpos.y; delta.z = oldlookat.z - oldpos.z;
	newpos.x = vPositionObj.x - delta.x; newpos.y = vPositionObj.y - delta.y; newpos.z = vPositionObj.z - delta.z;
	newlookat.x = vPositionObj.x; newlookat.y = vPositionObj.y; newlookat.z = vPositionObj.z;
	D3DXVECTOR3 vz(0, 0, 0);
	if (MT_VECTOR_UNEQUAL(newlookat, vz, 0.001f))
	{
		m_pSceneEditor->SetCameraPosLookat(&newpos, &newlookat);
		m_pSceneEditor->ClearSelectedEntityList();
		m_pSceneEditor->SetSelected((IEKG3DRepresentObject*)pPointer, TRUE);
		m_pSceneEditor->UpdateSelectableEntityList(5);
	}
}

void KSceneEditorDialogCheckRelive::SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEditor)
{
	m_pSceneEditor = pSceneEditor;
}

int KSceneEditorDialogCheckRelive::GetCheckReliveEnable()
{
	return m_bReliveEnable;
}

int KSceneEditorDialogCheckRelive::SetCheckReliveEnable(bool bEnable)
{
	m_bReliveEnable = bEnable;
	return true;
}