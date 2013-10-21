////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEditNpcSocketDlg.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-5-12 13:14:46
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGValueEditNpcSocketDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KGValueEditNpcSocketDlg, CDialog)

KGValueEditNpcSocketDlg::KGValueEditNpcSocketDlg(
	ITabFile* pTabFile, IIniFile* pIniFile,
	LPKGLISTITEM pListItem, CWnd* pParent
) : CDialog(KGValueEditNpcSocketDlg::IDD, pParent)
{
	m_pTabFile   = pTabFile;
	m_pIniFile	 = pIniFile;
	m_pListItem  = pListItem;
	m_pParant	 = pParent;
}

KGValueEditNpcSocketDlg::~KGValueEditNpcSocketDlg()
{
	m_mapProperty.clear();
}


KGListCtrl_NpcSocket::KGListCtrl_NpcSocket()
{
	m_pMianDlg = NULL;
}
KGListCtrl_NpcSocket::~KGListCtrl_NpcSocket()
{

}

int KGListCtrl_NpcSocket::SetMainDlg(KGValueEditNpcSocketDlg* pMainDlg)
{
	m_pMianDlg = pMainDlg;
	return true;
}

BEGIN_MESSAGE_MAP(KGListCtrl_NpcSocket, KGListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
	ON_BN_CLICKED(CTL_BROWSE, OnBrowseClick)
END_MESSAGE_MAP()

void KGListCtrl_NpcSocket::OnBrowseClick()
{
	int nResult  = false;
	int nRetCode = false;

	LPKGLISTITEM pItem = NULL;
	CString stringValue = _T("");
	char szFilePath[MAX_PATH];
	*szFilePath = '\0';

	pItem = (LPKGLISTITEM)GetItemData(m_nCurItem);


	CFileDialog fileDlg(
		true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"模型文件|*.mesh|材质文件|*.mtl|所有文件|*.*|",this
	);
	if (fileDlg.DoModal() == IDOK)
	{
		stringValue = fileDlg.GetPathName();
		g_GetFilePathFromFullPath(szFilePath, stringValue.GetBuffer());
		stringValue = szFilePath;
	}

	m_pMianDlg->m_mapProperty[pItem->arryItemText[0].szItemText].m_stringData = stringValue;
	pItem->arryItemText[1].szItemText = stringValue;;

	Update(pItem);

//Exit0:
	return;
}

void KGListCtrl_NpcSocket::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nResult  = false;
	int nRetCode = false;

	LPKGLISTITEM pItem = NULL;
   	POINT point;
	CRect rectClient;
	CRect rectSub;
	int	  nItem = 0;
	int	  nSub  = 0;
	int	  nSubCount = 0;

	GetClientRect(&rectClient);
	::GetCursorPos(&point);
	ScreenToClient(&point);

	nItem = HitTest(point);
	KG_PROCESS_ERROR(nItem != -1);

	pItem = (LPKGLISTITEM)GetItemData(nItem);

	if (!m_bnBrowse)
	{
		m_bnBrowse.Create(
			"", WS_CHILD | BS_OWNERDRAW, CRect(0, 0, 16, 16), this, CTL_BROWSE
		);
		m_bnBrowse.LoadIcon(AfxGetApp()->LoadIcon(IDI_ICON_BR));
		m_bnBrowse.m_nSelFlag = false;
	}
	m_bnBrowse.ShowWindow(SW_HIDE);

	nSubCount = CListCtrl::GetHeaderCtrl()->GetItemCount();
	for (nSub = nSubCount - 1; nSub >= 0; nSub--)
	{
		GetSubItemRect(nItem, nSub, LVIR_BOUNDS, rectSub);
		if (rectSub.PtInRect(point) && nSub && pItem->nTitleLever)
		{
			if (rectSub.right > rectClient.right)
				rectSub.right = rectClient.right;
			rectSub.left    = rectSub.right - rectSub.Height();

			rectSub.top += 1;
			m_bnBrowse.MoveWindow(&rectSub);
			m_bnBrowse.ShowWindow(SW_SHOW);

			m_nCurItem = nItem;

			break;
		}
	}

Exit0:
	*pResult = 0;
}

void KGValueEditNpcSocketDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_listCtrl);
}

void KGValueEditNpcSocketDlg::OnBnClickedOk()
{
	map<CString, Property_Pate_Data>::iterator i = m_mapProperty.begin();
	USER_ITEM_DATA itemData;

	KG_PROCESS_ERROR(m_pListItem);
	KG_PROCESS_ERROR(m_pTabFile);

	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	while (i != m_mapProperty.end())
	{
		m_pTabFile->WriteString(
			itemData.nRowsIndexInFile, i->second.m_stringPate, i->second.m_stringData
		);
		i++;
	}

Exit0:
	return OnOK();
}

BOOL KGValueEditNpcSocketDlg::OnInitDialog()
{
	int nResult  = false;
	int nRetCode = false;

	USER_ITEM_DATA itemData;
	LPKGLISTITEM pItem = NULL;

	char szKeyValue_1[1024];
	char szKeyValue_2[1024];
	char szKeyName_1[64];
	char szKeyName_2[64];

	*szKeyValue_1 = '\0';
	*szKeyValue_2 = '\0';
	*szKeyName_1  = '\0';
	*szKeyName_2  = '\0';

	nRetCode = CDialog::OnInitDialog();
	KG_PROCESS_ERROR(nRetCode);

	KG_PROCESS_ERROR(m_pListItem);
	KG_PROCESS_ERROR(m_pTabFile);
	KG_PROCESS_ERROR(m_pIniFile);

	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	m_listCtrl.InsertColumn(0, _T("插槽名"));
	m_listCtrl.InsertColumn(1, _T("文件路径"));

	m_listCtrl.SetColumnWidth(0, 150);
	m_listCtrl.SetColumnWidth(1, 400);

	m_listCtrl.SetMainDlg(this);

	while (m_pIniFile->GetNextKey(itemData.szPropertyName, szKeyName_1, szKeyName_1))
	{
		ITEMTEXT itemText;

		if (!strstr(szKeyName_1, _T("PARAM")))
			continue;

		pItem = new KGLISTITEM();

		m_pIniFile->GetString(
			itemData.szPropertyName, szKeyName_1, _T(""), szKeyValue_1, sizeof(szKeyValue_1)
		);
		m_pIniFile->GetString(
			szKeyValue_1, _T("Name"), _T(""), szKeyName_2, sizeof(szKeyName_2)
		);

		itemText.szItemText = szKeyName_2;
		itemText.colorExpandRectBack     = RGB(255, 255, 255);
		itemText.colorExpandRectBackSel  = RGB(255, 255, 255);
		itemText.colorExpandRectFrame    = RGB(168, 168, 168);
		itemText.colorExpandRectFrameSel = RGB(255, 2, 2);
		itemText.colorBackSel = itemText.colorBack;
		pItem->arryItemText.Add(itemText);

		m_listCtrl.InsertDepend(NULL, pItem);

		char szKeyValue[1024];
		char szKeyName[64];
		*szKeyValue = '\0';
		*szKeyName  = '\0';

		while (m_pIniFile->GetNextKey(szKeyValue_1, szKeyName, szKeyName))
		{
			char szValue[1024];
			*szValue = '\0';

			ITEMTEXT itemText;

			if (!strstr(szKeyName, _T("PARAM")))
				continue;


			LPKGLISTITEM pItemSub = new KGLISTITEM();

			m_pIniFile->GetString(
				szKeyValue_1, szKeyName, _T(""), szKeyValue, sizeof(szKeyValue)
			);
			m_pIniFile->GetString(
				szKeyValue, _T("Name"), _T(""), szValue, sizeof(szValue)
			);

			itemText.szItemText = szValue;
			itemText.colorBackSel = itemText.colorBack;
			pItemSub->arryItemText.Add(itemText);

			m_pTabFile->GetString(
				itemData.nRowsIndexInFile, szKeyValue, _T(""),szValue, sizeof(szValue)
			);
			itemText.szItemText = szValue;
			itemText.colorBackSel = itemText.colorBack;
			pItemSub->arryItemText.Add(itemText);

			m_mapProperty[pItemSub->arryItemText[0].szItemText].m_stringPate = szKeyValue;
			m_mapProperty[pItemSub->arryItemText[0].szItemText].m_stringData = szValue;
			m_listCtrl.InsertDepend(pItem, pItemSub);
		}
	}



	nResult = true;
Exit0:
	return nResult;
}

BEGIN_MESSAGE_MAP(KGValueEditNpcSocketDlg, CDialog)
	ON_BN_CLICKED(IDOK, &KGValueEditNpcSocketDlg::OnBnClickedOk)
END_MESSAGE_MAP()
