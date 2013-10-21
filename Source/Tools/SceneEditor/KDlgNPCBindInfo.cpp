// KDlgNPCBindInfo.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KDlgNPCBindInfo.h"
#include "kg3dobjecteditor.h"
//#include "KG3DSceneObjectEditor.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

// KDlgNPCBindInfo dialog
static TCHAR g_strFaceFileSuffix[] = "_face.mesh";
static TCHAR* g_strPartIDs[] = { "MainModelFile",
											"S_Face_MeshFile",
											"S_Hat_MeshFile",
											"S_LH_MeshFile",
											"S_LP_MeshFile",
											"S_RH_MeshFile",
											"S_RP_MeshFile",
											"S_Spine2_MeshFile"};

IMPLEMENT_DYNAMIC(KDlgNPCBindInfo, CDialog)

KDlgNPCBindInfo::KDlgNPCBindInfo(LPVOID pScene,
								 ITabFile* pTabFile,
								 IIniFile* pIniFile,
								 LPKGLISTITEM pListItem,
								 CWnd* pParent /*=NULL*/)
	: CDialog(KDlgNPCBindInfo::IDD, pParent)
{
	m_pScene = static_cast<IEKG3DSceneObjectEditor*>(pScene);//TODO:by dengzhihui Refactor 2006年11月15日
	m_pTabFile = pTabFile;
	m_pIniFile = pIniFile;
	m_pListItem = pListItem;
}

KDlgNPCBindInfo::~KDlgNPCBindInfo()
{
}

void KDlgNPCBindInfo::BindInfo::UpdateData(CDataExchange* pDx)
{
	for(int i = 0; i < COUNT; i++)
	{
		DDX_Text(pDx, IDC_EDIT_FACE + i, strNames[i]);
	}
}

void KDlgNPCBindInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ITEM, m_ctlItemListTree);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_ctlType);
	m_BindInfo.UpdateData(pDX);
}


BEGIN_MESSAGE_MAP(KDlgNPCBindInfo, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SETWEAPON, &KDlgNPCBindInfo::OnBnClickedButtonSetweapon)
	ON_BN_CLICKED(IDC_BUTTON_SETITEM, &KDlgNPCBindInfo::OnBnClickedButtonSetitem)
	ON_BN_CLICKED(IDC_RADIO_LEFTHAND, &KDlgNPCBindInfo::OnBnClickedRadioLefthand)
	ON_BN_CLICKED(IDC_RADIO_RIGHTHAND, &KDlgNPCBindInfo::OnBnClickedRadioRighthand)
	ON_BN_CLICKED(IDC_RADIO_LEFTPELVIS, &KDlgNPCBindInfo::OnBnClickedRadioLeftpelvis)
	ON_BN_CLICKED(IDC_RADIO_RIGHTPELVIS, &KDlgNPCBindInfo::OnBnClickedRadio1Rightpelvis)
	ON_BN_CLICKED(IDC_RADIO_SPINE0, &KDlgNPCBindInfo::OnBnClickedRadioSpine0)
	ON_BN_CLICKED(IDC_RADIO_SPINE1, &KDlgNPCBindInfo::OnBnClickedRadioSpine1)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &KDlgNPCBindInfo::OnCbnSelchangeComboType)
	ON_BN_CLICKED(IDC_BUTTON_OK, &KDlgNPCBindInfo::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &KDlgNPCBindInfo::OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &KDlgNPCBindInfo::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_OPENFACE, &KDlgNPCBindInfo::OnBnClickedButtonOpenface)
	ON_NOTIFY(NM_CLICK, IDC_TREE_ITEM, &KDlgNPCBindInfo::OnNMClickTreeItem)
END_MESSAGE_MAP()


// KDlgNPCBindInfo message handlers

void KDlgNPCBindInfo::OnBnClickedButtonSetweapon()
{
	/*TCHAR strDir[MAX_PATH];
	BROWSEINFO BrowseInfo;
	memset(&BrowseInfo, 0, sizeof(BROWSEINFO));
	BrowseInfo.hwndOwner = GetSafeHwnd();
	LPCITEMIDLIST pItem = SHBrowseForFolder(&BrowseInfo);
	UpdateData(TRUE);
	if (pItem)
	{
		if (SHGetPathFromIDList(pItem, strDir))
		{
			m_strBaseDir = strDir;
		}
		else
		{
			m_strBaseDir = "";
		}
		UpdateData(FALSE);
	}*/
}

void KDlgNPCBindInfo::OnBnClickedButtonSetitem()
{

}

BOOL KDlgNPCBindInfo::OnInitDialog()
{
	TCHAR strTemp[MAX_PATH];
	TCHAR strTemp1[MAX_PATH];

	CDialog::OnInitDialog();

	m_Item.SetIncludeSubDir(TRUE);
	m_Weapon.SetIncludeSubDir(TRUE);
	m_Item.AddSuffix(".mesh");
	m_Weapon.AddSuffix(".mesh");
	m_Weapon.AddDirFilter("texture");
	m_Item.AddDirFilter("texture");
	m_Item.AddDirFilter("cvs");
	m_Weapon.AddDirFilter("cvs");

	m_Weapon.SetDirectory(g_strWeaponDirectory);
	m_Item.SetDirectory(g_strItemDirectory);

	//
	USER_ITEM_DATA itemData;
	m_pListItem->GetStructData(&itemData, sizeof(USER_ITEM_DATA));

	m_pTabFile->GetString(itemData.nRowsIndexInFile,
		TEXT(g_strPartIDs[MAIN_MODEL]),
		TEXT(""),
		strTemp1,
		MAX_PATH);

	_strlwr_s(strTemp1, MAX_PATH);

	sprintf(strTemp, "%s%s", g_szDefWorkDirectory, strTemp1);
	m_BindInfo.strNames[BindInfo::FACE] = strTemp;
	m_BindInfo.strNames[BindInfo::FACE].Replace(".mdl", g_strFaceFileSuffix);

	m_pTabFile->GetString(itemData.nRowsIndexInFile,
		TEXT(g_strPartIDs[FACE]),
		TEXT(""),
		strTemp1,
		MAX_PATH);
	if (strTemp1[0] != '\0')
	{
		sprintf(strTemp, "%s%s", g_szDefWorkDirectory, strTemp1);
		m_BindInfo.strNames[BindInfo::FACE] = strTemp;
	}

	m_pTabFile->GetString(itemData.nRowsIndexInFile,
		TEXT(g_strPartIDs[LEFT_HAND]),
		TEXT(""),
		strTemp1,
		MAX_PATH);

	if (strTemp1[0] != '\0')
	{
		sprintf(strTemp, "%s%s", g_szDefWorkDirectory, strTemp1);
		m_BindInfo.strNames[BindInfo::LEFTHAND] = strTemp;
	}

	m_pTabFile->GetString(itemData.nRowsIndexInFile,
		TEXT(g_strPartIDs[LEFT_PELVIS]),
		TEXT(""),
		strTemp1,
		MAX_PATH);
	if (strTemp1[0] != '\0')
	{
		sprintf(strTemp, "%s%s", g_szDefWorkDirectory, strTemp1);
		m_BindInfo.strNames[BindInfo::LEFTPELVIS] = strTemp;
	}

	m_pTabFile->GetString(itemData.nRowsIndexInFile,
		TEXT(g_strPartIDs[RIGHT_HAND]),
		TEXT(""),
		strTemp1,
		MAX_PATH);
	if (strTemp1[0] != '\0')
	{
		sprintf(strTemp, "%s%s", g_szDefWorkDirectory, strTemp1);
		m_BindInfo.strNames[BindInfo::RIGHTHAND] = strTemp;
	}

	m_pTabFile->GetString(itemData.nRowsIndexInFile,
		TEXT(g_strPartIDs[RIGHT_PELVIS]),
		TEXT(""),
		strTemp1,
		MAX_PATH);
	if (strTemp1[0] != '\0')
	{
		sprintf(strTemp, "%s%s", g_szDefWorkDirectory, strTemp1);
		m_BindInfo.strNames[BindInfo::RIGHTPELVIS] = strTemp;
	}

	m_pTabFile->GetString(itemData.nRowsIndexInFile,
		TEXT(g_strPartIDs[SPINE]),
		TEXT(""),
		strTemp1,
		MAX_PATH);
	if (strTemp1[0] != '\0')
	{
		sprintf(strTemp, "%s%s", g_szDefWorkDirectory, strTemp1);
		m_BindInfo.strNames[BindInfo::SPINE0] = strTemp;
	}

	UpdateData(FALSE);
	BindItemByBindInfo();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KDlgNPCBindInfo::OnBnClickedRadioLefthand()
{
	int nState = static_cast<CButton*>(GetDlgItem(IDC_RADIO_LEFTHAND))->GetCheck();
	switch (nState)
	{
	case BST_CHECKED:
		{
			GetDlgItem(IDC_COMBO_TYPE)->EnableWindow(TRUE);
		}
		break;
	case BST_UNCHECKED:
		{
			GetDlgItem(IDC_COMBO_TYPE)->EnableWindow(FALSE);
		}
		break;
	case BST_INDETERMINATE:
		break;
	}
}

void KDlgNPCBindInfo::OnBnClickedRadioRighthand()
{
	int nState = static_cast<CButton*>(GetDlgItem(IDC_RADIO_RIGHTHAND))->GetCheck();
	switch (nState)
	{
	case BST_CHECKED:
		{
			GetDlgItem(IDC_COMBO_TYPE)->EnableWindow(TRUE);
		}
		break;
	case BST_UNCHECKED:
		{
			GetDlgItem(IDC_COMBO_TYPE)->EnableWindow(FALSE);
		}
		break;
	case BST_INDETERMINATE:
		break;
	}
}

void KDlgNPCBindInfo::OnBnClickedRadioLeftpelvis()
{
	int nState = static_cast<CButton*>(GetDlgItem(IDC_RADIO_LEFTPELVIS))->GetCheck();
	switch (nState)
	{
	case BST_CHECKED:
		{
			GetDlgItem(IDC_COMBO_TYPE)->EnableWindow(TRUE);
		}
		break;
	case BST_UNCHECKED:
		{
			GetDlgItem(IDC_COMBO_TYPE)->EnableWindow(FALSE);
		}
		break;
	case BST_INDETERMINATE:
		break;
	}
}

void KDlgNPCBindInfo::OnBnClickedRadio1Rightpelvis()
{
	int nState = static_cast<CButton*>(GetDlgItem(IDC_RADIO_RIGHTPELVIS))->GetCheck();
	switch (nState)
	{
	case BST_CHECKED:
		{
			GetDlgItem(IDC_COMBO_TYPE)->EnableWindow(TRUE);
		}
		break;
	case BST_UNCHECKED:
		{
			GetDlgItem(IDC_COMBO_TYPE)->EnableWindow(FALSE);
		}
		break;
	case BST_INDETERMINATE:
		break;
	}
}

void KDlgNPCBindInfo::OnBnClickedRadioSpine0()
{
	int nState = static_cast<CButton*>(GetDlgItem(IDC_RADIO_SPINE0))->GetCheck();
	switch (nState)
	{
	case BST_CHECKED:
		{
			GetDlgItem(IDC_COMBO_TYPE)->EnableWindow(TRUE);
		}
		break;
	case BST_UNCHECKED:
		{
			GetDlgItem(IDC_COMBO_TYPE)->EnableWindow(FALSE);
		}
		break;
	case BST_INDETERMINATE:
		break;
	}
}

void KDlgNPCBindInfo::OnBnClickedRadioSpine1()
{
	int nState = static_cast<CButton*>(GetDlgItem(IDC_RADIO_SPINE1))->GetCheck();
	switch (nState)
	{
	case BST_CHECKED:
		{
			GetDlgItem(IDC_COMBO_TYPE)->EnableWindow(TRUE);
		}
		break;
	case BST_UNCHECKED:
		{
			GetDlgItem(IDC_COMBO_TYPE)->EnableWindow(FALSE);
		}
		break;
	case BST_INDETERMINATE:
		break;
	}
}

void KDlgNPCBindInfo::OnCbnSelchangeComboType()
{
	int nSel = m_ctlType.GetCurSel();
	switch(nSel)
	{
	case ITEM_TYPE_WEAPON:
		{
			m_Item.DetachWindow(&m_ctlItemListTree);
			m_Weapon.AttachWindow(&m_ctlItemListTree);
		}
		break;
	case ITEM_TYPE_OTHER:
		{
			m_Weapon.DetachWindow(&m_ctlItemListTree);
			m_Item.AttachWindow(&m_ctlItemListTree);
		}
		break;
	}
	m_Weapon.UpdateUI();
	m_Item.UpdateUI();
}

void KDlgNPCBindInfo::OnBnClickedButtonOk()
{
	TCHAR strTemp[MAX_PATH];
	TCHAR strLowAppDir[MAX_PATH];

	TCHAR* pRelativePath = NULL;


	strcpy_s(strLowAppDir, MAX_PATH, g_szDefWorkDirectory);
	_strlwr_s(strLowAppDir, MAX_PATH);
	size_t uStride = strlen(strLowAppDir);

	USER_ITEM_DATA itemData;
	m_pListItem->GetStructData(&itemData, sizeof(USER_ITEM_DATA));

	strcpy(strTemp, m_BindInfo.strNames[BindInfo::FACE]);
	_strlwr_s(strTemp, MAX_PATH);
	pRelativePath = strstr(strTemp, strLowAppDir);
	if (!pRelativePath)
	{
		MessageBox(strTemp, "Invalidate file name.");
	}
	else
	{
		pRelativePath += uStride;
		m_pTabFile->WriteString(itemData.nRowsIndexInFile,
			TEXT(g_strPartIDs[FACE]),
			pRelativePath);
	}

	//strcpy(strTemp, m_BindInfo.strNames[BindInfo::]);
	//m_pTabFile->WriteString(itemData.nRowsIndexInFile,
	//	TEXT(g_strPartIDs[HAT]),
	//	strTemp);

	strcpy(strTemp, m_BindInfo.strNames[BindInfo::LEFTHAND]);
	_strlwr_s(strTemp, MAX_PATH);
	pRelativePath = strstr(strTemp, strLowAppDir);
	if (!pRelativePath)
	{
		MessageBox(strTemp, "Invalidate file name.");
	}
	else
	{
		pRelativePath += uStride;
		m_pTabFile->WriteString(itemData.nRowsIndexInFile,
			TEXT(g_strPartIDs[LEFT_HAND]),
			pRelativePath);
	}

	strcpy(strTemp, m_BindInfo.strNames[BindInfo::LEFTPELVIS]);
	_strlwr_s(strTemp, MAX_PATH);
	pRelativePath = strstr(strTemp, strLowAppDir);
	if (!pRelativePath)
	{
		MessageBox(strTemp, "Invalidate file name.");
	}
	else
	{
		pRelativePath += uStride;
		m_pTabFile->WriteString(itemData.nRowsIndexInFile,
			TEXT(g_strPartIDs[LEFT_PELVIS]),
			pRelativePath);
	}

	strcpy(strTemp, m_BindInfo.strNames[BindInfo::RIGHTHAND]);
	_strlwr_s(strTemp, MAX_PATH);
	pRelativePath = strstr(strTemp, strLowAppDir);
	if (!pRelativePath)
	{
		MessageBox(strTemp, "Invalidate file name.");
	}
	else
	{
		pRelativePath += uStride;
		m_pTabFile->WriteString(itemData.nRowsIndexInFile,
			TEXT(g_strPartIDs[RIGHT_HAND]),
			pRelativePath);
	}

	strcpy(strTemp, m_BindInfo.strNames[BindInfo::RIGHTPELVIS]);
	_strlwr_s(strTemp, MAX_PATH);
	pRelativePath = strstr(strTemp, strLowAppDir);
	if (!pRelativePath)
	{
		MessageBox(strTemp, "Invalidate file name.");
	}
	else
	{
		pRelativePath += uStride;
		m_pTabFile->WriteString(itemData.nRowsIndexInFile,
			TEXT(g_strPartIDs[RIGHT_PELVIS]),
			pRelativePath);
	}

	strcpy(strTemp, m_BindInfo.strNames[BindInfo::SPINE0]);
	_strlwr_s(strTemp, MAX_PATH);
	pRelativePath = strstr(strTemp, strLowAppDir);
	if (!pRelativePath)
	{
		MessageBox(strTemp, "Invalidate file name.");
	}
	else
	{
		pRelativePath += uStride;
		m_pTabFile->WriteString(itemData.nRowsIndexInFile,
			TEXT(g_strPartIDs[SPINE]),
			pRelativePath);
	}
}

void KDlgNPCBindInfo::OnBnClickedButtonCancel()
{
	OnCancel();
}

void KDlgNPCBindInfo::OnCancel()
{
	DestroyWindow();
}


void KDlgNPCBindInfo::OnBnClickedButtonClear()
{
	KG_PROCESS_ERROR(m_pScene);

	for (int i = 0; i < 6; i++)
	{
		if (static_cast<CButton*>(GetDlgItem(IDC_RADIO_LEFTHAND + i))->GetCheck() == BST_CHECKED)
		{
			m_BindInfo.strNames[i + 1] = "";
			UpdateData(FALSE);
			m_pScene->UnBind(i + 1);
			break;
		}
	}
Exit0:
	return;
}

void KDlgNPCBindInfo::OnBnClickedButtonOpenface()
{
	TCHAR strFacePath[MAX_PATH];
	TCHAR strFaceDriver[MAX_PATH];
	TCHAR strOpenInitialPath[MAX_PATH];

	static TCHAR strFilter[] = "Mesh文件(*.mesh)|*.mesh;All Files (*.*)|*.*||";

	CFileDialog dlgFaceFile(TRUE,
		".mesh",
		0,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		strFilter,
		this);

	if (m_BindInfo.strNames[BindInfo::FACE].GetLength())
	{
		_splitpath_s(m_BindInfo.strNames[BindInfo::FACE],
			strFaceDriver,
			MAX_PATH,
			strFacePath,
			MAX_PATH,
			NULL,
			0,
			NULL,
			0);
		sprintf_s(strOpenInitialPath,
			MAX_PATH,
			"%s%s",
			strFacePath,
			strFaceDriver);
	}
	else
	{
		sprintf_s(strOpenInitialPath, MAX_PATH, "%sdata\\source\\", g_szDefWorkDirectory);
	}

	dlgFaceFile.m_ofn.lpstrInitialDir = strOpenInitialPath;

	if (dlgFaceFile.DoModal() == IDOK)
	{
		m_BindInfo.strNames[BindInfo::FACE] = dlgFaceFile.GetPathName();
		UpdateData(FALSE);
		m_pScene->Bind(BindInfo::FACE, m_BindInfo.strNames[BindInfo::FACE], TRUE);
	}
}

void KDlgNPCBindInfo::OnNMClickTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (pNMHDR->hwndFrom == m_ctlItemListTree.GetSafeHwnd())
	{
		CPoint Point;
		GetCursorPos(&Point);
		m_ctlItemListTree.ScreenToClient(&Point);
		HTREEITEM hItem = m_ctlItemListTree.HitTest(Point);
		if (!hItem)
			return;
		m_ctlItemListTree.SelectItem(hItem);

		DWORD_PTR pData = m_ctlItemListTree.GetItemData(hItem);
		KG_PROCESS_ERROR(pData);

		LPCSTR pDir = reinterpret_cast<LPCSTR>(pData);
		KG_PROCESS_ERROR(pDir);

		for (int i = 0; i < BindInfo::COUNT; i++)
		{
			if (static_cast<CButton*>(GetDlgItem(IDC_RADIO_LEFTHAND + i))->GetCheck() == BST_CHECKED)
			{
				m_BindInfo.strNames[i + 1] = pDir;
				UpdateData(FALSE);
				m_pScene->Bind(i + 1, pDir, TRUE);
				break;
			}
		}
	}
	*pResult = 0;
Exit0:
	return;
}

void KDlgNPCBindInfo::BindItemByBindInfo()
{
	KG_PROCESS_ERROR(m_pScene);

	for (int i = 0; i < BindInfo::COUNT; i++)
	{
		m_pScene->Bind(i, m_BindInfo.strNames[i], TRUE);
	}
Exit0:
	return;
}