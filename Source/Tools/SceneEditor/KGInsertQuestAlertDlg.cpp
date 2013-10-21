// KGInsertQuestAlertDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGInsertQuestAlertDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KGInsertQuestAlertDlg, CDialog)
KGInsertQuestAlertDlg::KGInsertQuestAlertDlg(
	ITabFile* pExistingFile, CString szExistingFileName, int nExistingFileRow,
	ITabFile* pInsertFile, CString szInsertFileName, int nInsertFileRow,
	IIniFile* pNamePlateFile, CWnd* pParent
): CDialog(KGInsertQuestAlertDlg::IDD, pParent)
{
	m_szExistingFileName = _T("现有文件 : ") + szExistingFileName;
	m_szInsertFileName   = _T("导入文件 : ") + szInsertFileName;
	m_nExistingFileRow   = nExistingFileRow;
	m_nInsertFileRow     = nInsertFileRow;

	m_pExistingFile      = pExistingFile;
	m_pInsertFile        = pInsertFile;
	m_pNamePlateFile     = pNamePlateFile;

}

KGInsertQuestAlertDlg::~KGInsertQuestAlertDlg()
{
}

void KGInsertQuestAlertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXISTING_LIST, m_listExisting);
	DDX_Control(pDX, IDC_INSERT_LIST, m_ListInsert);
	DDX_Text(pDX, IDC_EXISTING_FILE_NAME, m_szExistingFileName);
	DDX_Text(pDX, IDC_INSERT_FILE_NAME, m_szInsertFileName);

}

BEGIN_MESSAGE_MAP(KGInsertQuestAlertDlg, CDialog)

	ON_BN_CLICKED(ID_AUTOTURE, &KGInsertQuestAlertDlg::OnBnClickedAutoture)
	ON_BN_CLICKED(IDC_OVERLAY, &KGInsertQuestAlertDlg::OnBnClickedOverlay)
	ON_BN_CLICKED(ID_AUTOTURE_ALL, &KGInsertQuestAlertDlg::OnBnClickedAutotureAll)
	ON_BN_CLICKED(IDC_OVERLAY_ALL, &KGInsertQuestAlertDlg::OnBnClickedOverlayAll)
	ON_BN_CLICKED(IDCANCEL, &KGInsertQuestAlertDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

BOOL KGInsertQuestAlertDlg::OnInitDialog()
{
	int nResult  = false;
	int nRetCode = false;

	LPKGLISTITEM pPrimaryKeyItem = NULL;

	nRetCode = CDialog::OnInitDialog();
	KG_PROCESS_ERROR(nRetCode);

	KG_PROCESS_ERROR(m_pNamePlateFile);
	KG_PROCESS_ERROR(m_pExistingFile);
	KG_PROCESS_ERROR(m_pInsertFile);

	m_listExisting.InsertColumn(0, "物件属性名");
	m_listExisting.InsertColumn(1, "物件属性值");
	m_listExisting.SetColumnWidth(0, 212);
	m_listExisting.SetColumnWidth(1, 662);
	m_listExisting.SetBkColor(RGB(92, 92, 92));

	nRetCode = m_listExisting.BandData(&m_pExistingFile, &m_pNamePlateFile);
	KG_PROCESS_ERROR(nRetCode);

	m_listExisting.UpdateProperty(m_nExistingFileRow);
	KG_PROCESS_ERROR(nRetCode);

	m_ListInsert.InsertColumn(0, "物件属性名");
	m_ListInsert.InsertColumn(1, "物件属性值");
	m_ListInsert.SetColumnWidth(0, 212);
	m_ListInsert.SetColumnWidth(1, 662);
	m_ListInsert.SetBkColor(RGB(92, 92, 92));

	nRetCode = m_ListInsert.BandData(&m_pInsertFile, &m_pNamePlateFile);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = m_ListInsert.UpdateProperty(m_nInsertFileRow);
	KG_PROCESS_ERROR(nRetCode);

	m_listExisting.EnableEdit(false);
	m_ListInsert.EnableEdit(false);

	pPrimaryKeyItem = m_listExisting.FindItemByPropertyName(
		m_listExisting.m_listDataTree.GetHead(), _T("QuestID")
	);

	if (pPrimaryKeyItem)
	{
		pPrimaryKeyItem->arryItemText.GetAt(0).colorBack = RGB(255, 2, 2);
		pPrimaryKeyItem->arryItemText.GetAt(0).colorText = RGB(255, 255, 255);
		pPrimaryKeyItem->arryItemText.GetAt(1).colorBack = RGB(255, 2, 2);
		pPrimaryKeyItem->arryItemText.GetAt(1).colorText = RGB(255, 255, 255);
	}

	pPrimaryKeyItem = m_ListInsert.FindItemByPropertyName(
		m_ListInsert.m_listDataTree.GetHead(), _T("QuestID")
	);

	if (pPrimaryKeyItem)
	{
		pPrimaryKeyItem->arryItemText.GetAt(0).colorBack = RGB(255, 2, 2);
		pPrimaryKeyItem->arryItemText.GetAt(0).colorText = RGB(255, 255, 255);
		pPrimaryKeyItem->arryItemText.GetAt(1).colorBack = RGB(255, 2, 2);
		pPrimaryKeyItem->arryItemText.GetAt(1).colorText = RGB(255, 255, 255);
	}


	nResult = true;
Exit0:
	return nResult;
}

RETURN_VALUE KGInsertQuestAlertDlg::GetReturnValue()
{
	return m_eReturnValue;
}

void KGInsertQuestAlertDlg::OnBnClickedAutoture()
{
	m_eReturnValue = E_AUTOTUNE;
	OnOK();
}

void KGInsertQuestAlertDlg::OnBnClickedOverlay()
{
	m_eReturnValue = E_OVERLAY;
	OnOK();
}

void KGInsertQuestAlertDlg::OnBnClickedAutotureAll()
{
	m_eReturnValue = E_AUTOTUNE_ALL;
	OnOK();
}

void KGInsertQuestAlertDlg::OnBnClickedOverlayAll()
{
	m_eReturnValue = E_OVERLAY_ALL;
	OnOK();
}

void KGInsertQuestAlertDlg::OnBnClickedCancel()
{
	m_eReturnValue = E_CANCEL;
	OnCancel();
}
