////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEditNpcTemplate.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-5-16 11:59:43
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGValueEditNpcTemplate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KGValueEditNpcTemplate, CDialog)

KGValueEditNpcTemplate::KGValueEditNpcTemplate(
	ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, CWnd* pParent
) : CDialog(KGValueEditNpcTemplate::IDD, pParent)
{
	m_pTabFile   = pTabFile;
	m_pIniFile	 = pIniFile;
	m_pListItem  = pListItem;
	m_pParant	 = pParent;
}

KGValueEditNpcTemplate::~KGValueEditNpcTemplate()
{
}

void KGValueEditNpcTemplate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE,	m_treeCtrl);
	DDX_Control(pDX, IDC_LIST,  m_listCtrl);

}

BOOL KGValueEditNpcTemplate::OnInitDialog()
{
	int nResult  = false;
	int nRetCode = false;

	USER_ITEM_DATA itemData;
	HTREEITEM  hTreeItem = NULL;

	char szTypeString[64];
	char szFullPath[MAX_PATH];
	char szFilePath[MAX_PATH];

	*szFullPath   = '\0';
	*szFilePath   = '\0';
	*szTypeString = '\0';

	int	nDefId = 0;


	DWORD dwStyle = 0x0;

	nRetCode = CDialog::OnInitDialog();
	KG_PROCESS_ERROR(nRetCode);

	KG_PROCESS_ERROR(m_pListItem);
	KG_PROCESS_ERROR(m_pTabFile);
	KG_PROCESS_ERROR(m_pIniFile);

	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	m_imageListTree.Create(16, 16, ILC_COLOR24, 4, 4);
	m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_FC));
	m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_FO));
	m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_TALK));

	m_treeCtrl.SetImageList(&m_imageListTree, TVSIL_NORMAL);
	m_treeCtrl.SetTextColor(RGB(255, 255, 255));
	m_treeCtrl.SetLineColor(RGB(122, 122, 122));
	m_treeCtrl.SetBkColor(RGB(92,  92, 92));

	dwStyle = GetWindowLong(m_treeCtrl.m_hWnd, GWL_STYLE);
	dwStyle |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
	SetWindowLong(m_treeCtrl.m_hWnd, GWL_STYLE, dwStyle);

	m_listCtrl.InsertColumn(0, _T("属性名"));
	m_listCtrl.InsertColumn(1, _T("属性值"));
	m_listCtrl.SetColumnWidth(0, 150);
	m_listCtrl.SetColumnWidth(1, 500);
	m_listCtrl.SetBkColor(RGB(92, 92, 92));

	nRetCode = m_pIniFile->GetString(
		itemData.szPropertyName, _T("Type"), _T(""), szTypeString, sizeof(szTypeString)
	);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = m_pIniFile->GetString(
		szTypeString, _T("TAB"), _T(""), szFilePath, sizeof(szFilePath)
	);
	KG_PROCESS_ERROR(nRetCode);

	g_GetFullPath(szFullPath, szFilePath);
	m_subEditer.m_pGameLogicTabFile = g_OpenTabFile(szFullPath);
	KG_PROCESS_ERROR(m_subEditer.m_pGameLogicTabFile);

	nRetCode = m_pIniFile->GetString(
		szTypeString, _T("INI"), _T(""), szFilePath, sizeof(szFilePath)
	);
	KG_PROCESS_ERROR(nRetCode);

	g_GetFullPath(szFullPath, szFilePath);
	m_subEditer.m_pGameLogicIniFile = g_OpenIniFile(szFullPath);
	KG_PROCESS_ERROR(m_subEditer.m_pGameLogicIniFile);

	nRetCode = m_pIniFile->GetString(
		szTypeString, _T("CLS"), _T(""), szFilePath, sizeof(szFilePath)
	);
	KG_PROCESS_ERROR(nRetCode);

	g_GetFullPath(szFullPath, szFilePath);
	m_subEditer.m_pClassifyFile = g_OpenIniFile(szFullPath);
	KG_PROCESS_ERROR(m_subEditer.m_pClassifyFile);

	m_treeCtrl.Update(&m_subEditer);

	m_listCtrl.BandData(&(m_subEditer.m_pGameLogicTabFile), &(m_subEditer.m_pGameLogicIniFile));
	m_listCtrl.EnableEdit(false);

	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, itemData.szPropertyName, 0, &nDefId);

	hTreeItem = m_treeCtrl.GetItemByPrimaryKey(m_treeCtrl.GetRootItem(), nDefId);
	if (hTreeItem)
		m_treeCtrl.SelectItem(hTreeItem);

	SetWindowText(_T("NPC 模版表"));

	nResult = true;
Exit0:
	return nResult;
}

BEGIN_MESSAGE_MAP(KGValueEditNpcTemplate, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &KGValueEditNpcTemplate::OnTvnSelchangedTree)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, &KGValueEditNpcTemplate::OnBnClickedOk)
END_MESSAGE_MAP()

HBRUSH KGValueEditNpcTemplate::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (pWnd == GetDlgItem(IDC_EDIT))
		return ::CreateSolidBrush(RGB(255, 255, 255));
	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}
void KGValueEditNpcTemplate::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nResult  = false;
	int nRetCode = false;

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();

	int	  nItemData  = 0;

	KG_PROCESS_ERROR(m_subEditer.m_pGameLogicTabFile);
	KG_PROCESS_ERROR(m_pListItem);
	KG_PROCESS_ERROR(hSelItem);

	nItemData = (int)m_treeCtrl.GetItemData(hSelItem);
	m_listCtrl.UpdateProperty(_T("ID"), nItemData);

	UpdateData(false);
	m_treeCtrl.Invalidate();

	nResult = true;
Exit0:
	*pResult = 0;
}


void KGValueEditNpcTemplate::OnBnClickedOk()
{
	UpdateData();

	HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();
	USER_ITEM_DATA itemData;
	int nItemData = 0;

	KG_PROCESS_ERROR(m_pTabFile);
	KG_PROCESS_ERROR(m_pListItem);
	KG_PROCESS_ERROR(hSelItem);

	m_pListItem->GetStructData(&itemData, sizeof(itemData));
	nItemData = (int)m_treeCtrl.GetItemData(hSelItem);

	m_pTabFile->WriteInteger(
		itemData.nRowsIndexInFile, itemData.szPropertyName, nItemData
	);

	if (m_pParant)
		m_pParant->SendMessage(WM_USER_UPDATE_LIST_ITEM, (WPARAM)m_pListItem);

Exit0:
	OnOK();
}


