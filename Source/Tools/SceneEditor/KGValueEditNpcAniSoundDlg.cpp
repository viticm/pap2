////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEditNpcAniSoundDlg.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-5-8 10:35:38
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGValueEditNpcAniSoundDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KGValueEditNpcAniSoundDlg, CDialog)

KGValueEditNpcAniSoundDlg::KGValueEditNpcAniSoundDlg(
	ITabFile* pTabFile, IIniFile* pIniFile,
	LPKGLISTITEM pListItem, CWnd* pParent
) : CDialog(KGValueEditNpcAniSoundDlg::IDD, pParent)
{
	m_pTabFile   = pTabFile;
	m_pIniFile	 = pIniFile;
	m_pListItem  = pListItem;
	m_pParant	 = pParent;
}

KGValueEditNpcAniSoundDlg::~KGValueEditNpcAniSoundDlg()
{
}

KGListCtrl_NpcAniSound::KGListCtrl_NpcAniSound()
{
	m_pMianDlg = NULL;
	m_nCurPage = 0;
	m_nCurSub  = 0;
	m_nCurItem = 0;
}
KGListCtrl_NpcAniSound::~KGListCtrl_NpcAniSound()
{

}

BEGIN_MESSAGE_MAP(KGListCtrl_NpcAniSound, KGListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
	ON_WM_CREATE()
	ON_NOTIFY(UDN_DELTAPOS, CTL_NUMBER, OnDeltaposSpin)
	ON_BN_CLICKED(CTL_BROWSE, OnBrowseClick)
END_MESSAGE_MAP()

BOOL KGValueEditNpcAniSoundDlg::OnInitDialog()
{
	int nResult  = false;
	int nRetCode = false;

	LPKGLISTITEM   pItem = NULL;
	USER_ITEM_DATA itemData;
	char szPageName[64];
	*szPageName  = '\0';
	int  nIndex  = 0;
	int  nStep_1 = 0;
	int	 nStep_2 = 0;

	CString stringParam = _T("");
	CString stringAni	= _T("");

	char szPropertyName[64];
	char szPropertyValue[1024];
	char szParam_1[64];
	char szParam_2[64];
	char szParam_3[64];

	*szPropertyValue = '\0';
	*szPropertyName  = '\0';
	*szParam_1		 = '\0';
	*szParam_2		 = '\0';
	*szParam_3		 = '\0';

	nRetCode = CDialog::OnInitDialog();
	KG_PROCESS_ERROR(nRetCode);

	KG_PROCESS_ERROR(m_pTabFile);
	KG_PROCESS_ERROR(m_pIniFile);
	KG_PROCESS_ERROR(m_pListItem);

	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	m_listCtrl.InsertColumn(0, _T("动作"));
	m_listCtrl.InsertColumn(1, _T("文件路径"));
	m_listCtrl.InsertColumn(2, _T("绑定音效"));
	m_listCtrl.InsertColumn(3, _T("播放概率"));

	m_listCtrl.SetColumnWidth(0, 50);
	m_listCtrl.SetColumnWidth(1, 190);
	m_listCtrl.SetColumnWidth(2, 190);
	m_listCtrl.SetColumnWidth(3, 80);

	m_listCtrl.SetMainDlg(this);


	for (nStep_1 = 0; nStep_1 < PAGE_SIZE; nStep_1++)
	{
		switch (nStep_1)
		{
		case 0:
			strncpy_s(szPropertyName, itemData.szParamName_1, sizeof(szPropertyName));
			break;
		case 1:
			strncpy_s(szPropertyName, itemData.szParamName_2, sizeof(szPropertyName));
			break;
		case 2:
			strncpy_s(szPropertyName, itemData.szParamName_3, sizeof(szPropertyName));
			break;
		case 3:
			strncpy_s(szPropertyName, itemData.szParamName_4, sizeof(szPropertyName));
			break;
		case 4:
			strncpy_s(szPropertyName, itemData.szParamName_5, sizeof(szPropertyName));
			break;
		case 5:
			strncpy_s(szPropertyName, itemData.szParamName_6, sizeof(szPropertyName));
			break;
		case 6:
			strncpy_s(szPropertyName, itemData.szParamName_7, sizeof(szPropertyName));
			break;
		case 7:
			strncpy_s(szPropertyName, itemData.szParamName_8, sizeof(szPropertyName));
			break;
		case 8:
			strncpy_s(szPropertyName, itemData.szParamName_9, sizeof(szPropertyName));
			break;
		default :
			KG_PROCESS_ERROR(false);
		}

		m_pIniFile->GetString(szPropertyName, _T("PARAM_1"), _T(""), szParam_1, sizeof(szParam_1));
		m_pIniFile->GetString(szPropertyName, _T("PARAM_2"), _T(""), szParam_2, sizeof(szParam_2));
		m_pIniFile->GetString(szPropertyName, _T("PARAM_3"), _T(""), szParam_3, sizeof(szParam_3));

		m_pIniFile->GetString(
			szPropertyName, _T("Name"), _T(""), szPageName, sizeof(szPageName)
		);
		m_tabCtrlPage.InsertItem(nStep_1, szPageName);

		for (nStep_2 = 0; nStep_2 < ITEM_COUNT; nStep_2++)
		{
			m_pateData[nStep_1][nStep_2][0].m_stringData.Format("%s%d", _T("动作-"), nStep_2 + 1);

			stringParam.Format("%s%d", szParam_1, nStep_2);
			m_pTabFile->GetString(
				itemData.nRowsIndexInFile, stringParam.GetBuffer(), _T(""),
				szPropertyValue, sizeof(szPropertyValue)
			);
			m_pateData[nStep_1][nStep_2][1].m_stringPate = stringParam;
			m_pateData[nStep_1][nStep_2][1].m_stringData = szPropertyValue;

			stringParam.Format("%s%d", szParam_2, nStep_2);
			m_pTabFile->GetString(
				itemData.nRowsIndexInFile, stringParam.GetBuffer(), _T(""),
				szPropertyValue, sizeof(szPropertyValue)
			);
			m_pateData[nStep_1][nStep_2][2].m_stringPate = stringParam;
			m_pateData[nStep_1][nStep_2][2].m_stringData = szPropertyValue;

			stringParam.Format("%s%d", szParam_3, nStep_2);
			m_pTabFile->GetString(
				itemData.nRowsIndexInFile, stringParam.GetBuffer(), _T(""),
				szPropertyValue, sizeof(szPropertyValue)
			);
			m_pateData[nStep_1][nStep_2][3].m_stringPate = stringParam;
			m_pateData[nStep_1][nStep_2][3].m_stringData = szPropertyValue;
		}
	}

	for (nStep_1 = 0; nStep_1 < ITEM_COUNT; nStep_1++)
	{
		pItem = new KGLISTITEM();
		for (nStep_2 = 0; nStep_2 < SUB_COUNT; nStep_2++)
		{
			ITEMTEXT itemText;
			itemText.szItemText = m_pateData[0][nStep_1][nStep_2].m_stringData;
			itemText.colorBackSel = itemText.colorBack;
			pItem->arryItemText.Add(itemText);
		}
		m_listCtrl.InsertDepend(NULL, pItem);
	}

	SendMessage(WM_SIZE);

	nResult = true;
Exit0:
	return nResult;
}

void KGValueEditNpcAniSoundDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB,  m_tabCtrlPage);
	DDX_Control(pDX, IDC_LIST, m_listCtrl);
}


BEGIN_MESSAGE_MAP(KGValueEditNpcAniSoundDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &KGValueEditNpcAniSoundDlg::OnTcnSelchangeTab)
	ON_BN_CLICKED(IDOK, &KGValueEditNpcAniSoundDlg::OnBnClickedOk)
	ON_WM_SIZE()
END_MESSAGE_MAP()

void KGValueEditNpcAniSoundDlg::OnSize(UINT nType, int cx, int cy)
{
	CRect rectClient;
	CRect rectButton;
	int   nLength = 0;

	KG_PROCESS_ERROR(m_tabCtrlPage);
	KG_PROCESS_ERROR(m_listCtrl);

	GetClientRect(&rectClient);
	rectButton = rectClient;

	nLength = rectClient.Width() / 10;

	rectClient.bottom -= 35;
	m_tabCtrlPage.MoveWindow(&rectClient);

	rectClient.left += 2;
	rectClient.right -= 2;
	rectClient.bottom -=2;
	rectClient.top += 24;
	m_listCtrl.MoveWindow(&rectClient);

	rectButton.bottom -= 4;
	rectButton.top = rectButton.bottom - 25;
	rectButton.right -= 4;
	rectButton.left = rectButton.right - 80;

	GetDlgItem(IDCANCEL)->MoveWindow(&rectButton);

	rectButton.right = rectButton.left - 4;
	rectButton.left  = rectButton.right - 80;

	GetDlgItem(IDOK)->MoveWindow(&rectButton);


	m_listCtrl.SetColumnWidth(0, nLength);
	m_listCtrl.SetColumnWidth(1, nLength * 4);
	m_listCtrl.SetColumnWidth(2, nLength * 4);
	m_listCtrl.SetColumnWidth(3, nLength);

	Invalidate();

Exit0:
	return CDialog::OnSize(nType, cx, cy);
}

void KGValueEditNpcAniSoundDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nResult  = false;
	int nRetCode = false;

	int nSelPage = m_tabCtrlPage.GetCurSel();
	int nStep_1  = 0;
	int nStep_2  = 0;

	LPKGLISTITEM   pItem = NULL;

	KG_PROCESS_ERROR(nSelPage != -1);
	m_listCtrl.m_nCurPage = nSelPage;

	m_listCtrl.RemoveAll();

	for (nStep_1 = 0; nStep_1 < ITEM_COUNT; nStep_1++)
	{
		pItem = new KGLISTITEM();
		for (nStep_2 = 0; nStep_2 < SUB_COUNT; nStep_2++)
		{
			ITEMTEXT itemText;
			itemText.szItemText = m_pateData[nSelPage][nStep_1][nStep_2].m_stringData;
			//if (!nStep_2)
			//	itemText.colorBack = RGB(200, 200, 200);
			itemText.colorBackSel = itemText.colorBack;
			pItem->arryItemText.Add(itemText);
		}
		m_listCtrl.InsertDepend(NULL, pItem);
	}

	nResult = true;
Exit0:
	*pResult = 0;
}


int  KGListCtrl_NpcAniSound::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = KGListCtrl::OnCreate(lpCreateStruct);
	KG_PROCESS_ERROR(nRetCode != -1);

	nResult = true;
Exit0:
	return nResult;
}

void KGListCtrl_NpcAniSound::OnBrowseClick()
{
	int nResult  = false;
	int nRetCode = false;

	LPKGLISTITEM pItem = NULL;
	CString stringValue = _T("");
	char szFilePath[MAX_PATH];
	*szFilePath = '\0';

	pItem = (LPKGLISTITEM)GetItemData(m_nCurItem);

	if (m_nCurSub == 1 || m_nCurSub == 2)
	{
		CFileDialog fileDlg(
			true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"动作文件|*.Ani|所有文件|*.*|",this
		);
		if (fileDlg.DoModal() == IDOK)
		{
			stringValue = fileDlg.GetPathName();
			g_GetFilePathFromFullPath(szFilePath, stringValue.GetBuffer());
			stringValue = szFilePath;
		}

		m_pMianDlg->m_pateData[m_nCurPage][m_nCurItem][m_nCurSub].m_stringData = stringValue;
		pItem->arryItemText[m_nCurSub].szItemText = stringValue;

		m_pMianDlg->Invalidate();
	}


	//nResult = true;
//Exit0:
	return;
}

void KGListCtrl_NpcAniSound::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nResult  = false;
	int nRetCode = false;

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	LPKGLISTITEM pItem = NULL;
	int	nValue = 0;

	pItem = (LPKGLISTITEM)GetItemData(m_nCurItem);

	if (m_nCurSub == 3)
	{
		nValue = atoi(
			m_pMianDlg->m_pateData[m_nCurPage][m_nCurItem][m_nCurSub].m_stringData.GetBuffer()
		);
		nValue += pNMUpDown->iDelta;

		KG_PROCESS_ERROR(nValue >= 0);

		m_pMianDlg->m_pateData[m_nCurPage][m_nCurItem][m_nCurSub].m_stringData.Format("%d", nValue);
		pItem->arryItemText[m_nCurSub].szItemText.Format("%d", nValue);

		Update(pItem);
	}

Exit0:
	*pResult = 0;
}

int  KGListCtrl_NpcAniSound::SetMainDlg(KGValueEditNpcAniSoundDlg* pMainDlg)
{
	m_pMianDlg = pMainDlg;
	return true;
}

void KGListCtrl_NpcAniSound::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nResult  = false;
	int nRetCode = false;

   	POINT point;
	CRect rectSub;
	CRect rectClient;
	int	  nItem = 0;
	int	  nSub  = 0;
	int	  nSubCount = 0;

	GetClientRect(&rectClient);
	::GetCursorPos(&point);
	ScreenToClient(&point);

	nItem = HitTest(point);
	KG_PROCESS_ERROR(nItem != -1);

	if (!m_bnBrowse)
	{
		m_bnBrowse.Create(
			"", WS_CHILD | BS_OWNERDRAW, CRect(0, 0, 16, 16), this, CTL_BROWSE
		);
		m_bnBrowse.LoadIcon(AfxGetApp()->LoadIcon(IDI_ICON_BR));
		m_bnBrowse.m_nSelFlag = false;
	}
	if (!m_spinCtrl)
	{
		m_spinCtrl.Create(WS_CHILD | UDS_SETBUDDYINT, CRect(0, 0, 16, 16), this, CTL_NUMBER);
	}
	m_bnBrowse.ShowWindow(SW_HIDE);
	m_spinCtrl.ShowWindow(SW_HIDE);

	nSubCount = CListCtrl::GetHeaderCtrl()->GetItemCount();

	for (nSub = nSubCount - 1; nSub >= 0; nSub--)
	{
		GetSubItemRect(nItem, nSub, LVIR_BOUNDS, rectSub);
		if (rectSub.PtInRect(point) && nSub)
		{
			if (rectSub.right > rectClient.right)
				rectSub.right = rectClient.right;
			rectSub.left    = rectSub.right - rectSub.Height();
			if (nSub == 1 || nSub == 2)
			{
				rectSub.top += 1;
				m_bnBrowse.MoveWindow(&rectSub);
				m_bnBrowse.ShowWindow(SW_SHOW);
			}
			else
			{
				m_spinCtrl.MoveWindow(&rectSub);
				m_spinCtrl.ShowWindow(SW_SHOW);
			}
			m_nCurItem = nItem;
			m_nCurSub  = nSub;

			break;
		}
	}

Exit0:
	*pResult = 0;
}

void KGValueEditNpcAniSoundDlg::OnBnClickedOk()
{
	USER_ITEM_DATA itemData;

	int nPage = 0;
	int nItem = 0;
	int nSubI = 0;

	KG_PROCESS_ERROR(m_pListItem);
	KG_PROCESS_ERROR(m_pTabFile);

	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	for (nPage = 0; nPage < PAGE_SIZE; nPage++)
	{
		for (nItem = 0; nItem < ITEM_COUNT; nItem++)
		{
			for (nSubI = 0; nSubI < SUB_COUNT; nSubI++)
			{
				if (!nSubI)
					continue;

				m_pTabFile->WriteString(
					itemData.nRowsIndexInFile, m_pateData[nPage][nItem][nSubI].m_stringPate,
					m_pateData[nPage][nItem][nSubI].m_stringData
				);
			}
		}
	}

Exit0:
	OnOK();
}
