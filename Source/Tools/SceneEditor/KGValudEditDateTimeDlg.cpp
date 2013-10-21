// KGValudEditDateTimeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGValudEditDateTimeDlg.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KGValudEditDateTimeDlg, CDialog)

KGValudEditDateTimeDlg::KGValudEditDateTimeDlg(
	ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem,
	CWnd* pParent
): CDialog(KGValudEditDateTimeDlg::IDD, pParent)
{
	m_pTabFile   = pTabFile;
	m_pIniFile	 = pIniFile;
	m_pListItem  = pListItem;
	m_pParant	 = pParent;
}

KGValudEditDateTimeDlg::~KGValudEditDateTimeDlg()
{
}

void KGValudEditDateTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER1,   m_dateCtrlStartDate);
	DDX_Control(pDX, IDC_DATETIMEPICKER2,   m_dateCtrlEndDate);
	DDX_Control(pDX, IDC_SPIN_START_TIME,   m_spinButtonStartTime);
	DDX_Control(pDX, IDC_SPIN_END_TIME,     m_spinButtonEndTime);
	DDX_Control(pDX, IDC_SPIN_G_START_TIME, m_spinButtonStartGameTime);
	DDX_Control(pDX, IDC_SPIN_G_END_TIME,   m_spinButtonEndGameTime);
	DDX_Text(pDX, IDC_EDIT_START_TIME,      m_nStartTime);
	DDX_Text(pDX, IDC_EDIT_END_TIME,        m_nEndTime);
	DDX_Text(pDX, IDC_EDIT_G_START_TIME,    m_nStartGameTime);
	DDX_Text(pDX, IDC_EDIT_G_END_TIME,      m_nEndGameTime);
	DDX_Control(pDX, IDC_CHECK,				m_checkBoxTime);
	DDX_Control(pDX, IDC_CHECK_G,			m_checkBoxGameTime);
}


BEGIN_MESSAGE_MAP(KGValudEditDateTimeDlg, CDialog)
	ON_BN_CLICKED(IDOK, &KGValudEditDateTimeDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_G, &KGValudEditDateTimeDlg::OnBnClickedCheckG)
	ON_BN_CLICKED(IDC_CHECK, &KGValudEditDateTimeDlg::OnBnClickedCheck)
END_MESSAGE_MAP()

BOOL KGValudEditDateTimeDlg::OnInitDialog()
{
	int nResult  = false;
	int nRetCode = false;

	USER_ITEM_DATA itemData;

	CString szTempString = _T("");

	CString szYear       = _T("");
	CString szMonth      = _T("");
	CString szDay        = _T("");

	CTime timeStartDate;
	CTime timeEndDate;

	char szStartDate[9];
	char szEndDate[9];

	*szStartDate = '\0';
	*szEndDate   = '\0';

	int  nYear   = 2006;
	int  nMonth  = 3;
	int  nDay    = 15;

	nRetCode = CDialog::OnInitDialog();
	KG_PROCESS_ERROR(nRetCode);

	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	m_pTabFile->GetString(
		itemData.nRowsIndexInFile, itemData.szParamName_1, _T(""),
		szStartDate, sizeof(szStartDate)
	);
	m_pTabFile->GetString(
		itemData.nRowsIndexInFile, itemData.szParamName_2, _T(""),
		szEndDate, sizeof(szEndDate)
	);

	szTempString = szStartDate;

	if (szTempString.GetLength() == 8)
	{
		szDay   = szTempString.Mid(0, 2);
		szMonth = szTempString.Mid(2, 2);
		szYear  = szTempString.Mid(4, 7);

		nDay   = atoi(szDay.GetBuffer());
		nMonth = atoi(szMonth.GetBuffer());
		nYear  = atoi(szYear.GetBuffer());
	}

	timeStartDate = CTime(nYear, nMonth, nDay, 0, 0, 0);
	m_dateCtrlStartDate.SetTime(&timeStartDate);

	szTempString = szEndDate;

	nDay   = 15;
	nMonth = 3;
	nYear  = 2006;

	if (szTempString.GetLength() == 8)
	{
		szDay   = szTempString.Mid(0, 2);
		szMonth = szTempString.Mid(2, 2);
		szYear  = szTempString.Mid(4, 7);

		nDay   = atoi(szDay.GetBuffer());
		nMonth = atoi(szMonth.GetBuffer());
		nYear  = atoi(szYear.GetBuffer());
	}

	timeEndDate = CTime(nYear, nMonth, nDay, 0, 0, 0);
	m_dateCtrlEndDate.SetTime(&timeEndDate);

	m_spinButtonStartTime.SetBuddy(GetDlgItem(IDC_EDIT_START_TIME));
	m_spinButtonEndTime.SetBuddy(GetDlgItem(IDC_EDIT_END_TIME));
	m_spinButtonStartTime.SetRange32(0, 23);
	m_spinButtonEndTime.SetRange32(0, 23);

	m_pTabFile->GetInteger(
		itemData.nRowsIndexInFile, itemData.szParamName_3, 0, &m_nStartGameTime
	);
	m_pTabFile->GetInteger(
		itemData.nRowsIndexInFile, itemData.szParamName_4, 0, &m_nEndGameTime
	);

	m_spinButtonStartGameTime.SetBuddy(GetDlgItem(IDC_EDIT_G_START_TIME));
	m_spinButtonEndGameTime.SetBuddy(GetDlgItem(IDC_EDIT_G_END_TIME));
	m_spinButtonStartGameTime.SetRange32(1, 23);
	m_spinButtonEndGameTime.SetRange32(0, 23);

	m_pTabFile->GetInteger(
		itemData.nRowsIndexInFile, itemData.szParamName_5, 0, &m_nStartTime
	);
	m_pTabFile->GetInteger(
		itemData.nRowsIndexInFile, itemData.szParamName_6, 0, &m_nEndTime
	);

	if (m_nStartTime != 0 && m_nEndTime != 0)
	{
		GetDlgItem(IDC_EDIT_G_START_TIME)->EnableWindow(false);
		GetDlgItem(IDC_EDIT_G_END_TIME)->EnableWindow(false);
		m_checkBoxTime.SetCheck(true);
		m_checkBoxGameTime.SetCheck(false);
	}
	else
	{
		GetDlgItem(IDC_EDIT_START_TIME)->EnableWindow(false);
		GetDlgItem(IDC_EDIT_END_TIME)->EnableWindow(false);
		m_checkBoxGameTime.SetCheck(true);
		m_checkBoxTime.SetCheck(false);
	}

	UpdateData(false);

	nResult = true;
Exit0:
	return nResult;
}

void KGValudEditDateTimeDlg::OnBnClickedOk()
{
	int nResult  = false;
	int nRetCode = false;

	USER_ITEM_DATA itemData;

	CString szStartDate = _T("");
	CString szEndDate   = _T("");

	CTime timeStartDate;
	CTime timeEndDate;

	nRetCode = UpdateData();
	KG_PROCESS_ERROR(nRetCode);

	m_dateCtrlStartDate.GetTime(timeStartDate);
	szStartDate = timeStartDate.Format(_T("%d%m%Y"));

	m_dateCtrlEndDate.GetTime(timeEndDate);
	szEndDate   = timeEndDate.Format(_T("%d%m%Y"));

	if (timeStartDate > timeEndDate)
	{
		::MessageBox(
			this->m_hWnd, _T("结束日期不能小于开始日期!!"), _T("出错拉!!"),
			MB_OK | MB_ICONERROR
		);
		KG_PROCESS_ERROR(false);
	}

	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	m_pTabFile->WriteString(
		itemData.nRowsIndexInFile, itemData.szParamName_1, szStartDate
	);
	m_pTabFile->WriteString(
		itemData.nRowsIndexInFile, itemData.szParamName_2, szEndDate
	);

	nRetCode = m_checkBoxGameTime.GetCheck();

	if (nRetCode)
	{
		if (m_nStartGameTime > m_nEndGameTime && timeStartDate == timeEndDate)
		{
			::MessageBox(
				this->m_hWnd, _T("结束时间不能小于开始时间!!"), _T("出错拉!!"),
				MB_OK | MB_ICONERROR
			);
			KG_PROCESS_ERROR(false);
		}
		m_pTabFile->WriteInteger(
			itemData.nRowsIndexInFile, itemData.szParamName_3, m_nStartGameTime
		);
		m_pTabFile->WriteInteger(
			itemData.nRowsIndexInFile, itemData.szParamName_4, m_nEndGameTime
		);
		m_pTabFile->WriteString(
			itemData.nRowsIndexInFile, itemData.szParamName_5, _T("")
		);
		m_pTabFile->WriteString(
			itemData.nRowsIndexInFile, itemData.szParamName_6, _T("")
		);
	}
	else
	{
		if (m_nStartTime > m_nEndTime && timeStartDate == timeEndDate)
		{
			::MessageBox(
				this->m_hWnd, _T("结束时间不能小于开始时间!!"), _T("出错拉!!"),
				MB_OK | MB_ICONERROR
			);
			KG_PROCESS_ERROR(false);
		}
		m_pTabFile->WriteInteger(
			itemData.nRowsIndexInFile, itemData.szParamName_5, m_nStartTime
		);
		m_pTabFile->WriteInteger(
			itemData.nRowsIndexInFile, itemData.szParamName_6, m_nEndTime
		);
		m_pTabFile->WriteString(
			itemData.nRowsIndexInFile, itemData.szParamName_3, _T("")
		);
		m_pTabFile->WriteString(
			itemData.nRowsIndexInFile, itemData.szParamName_4, _T("")
		);
	}

	if (m_pParant)
		m_pParant->SendMessage(WM_USER_UPDATE_LIST_ITEM, (WPARAM)m_pListItem);

//Exit1:
	OnOK();
Exit0:
	return;
}

void KGValudEditDateTimeDlg::OnBnClickedCheckG()
{
	int nRetCode = false;

	nRetCode = m_checkBoxGameTime.GetCheck();

	if (nRetCode)
	{
		GetDlgItem(IDC_EDIT_START_TIME)->EnableWindow(false);
		GetDlgItem(IDC_EDIT_END_TIME)->EnableWindow(false);
		GetDlgItem(IDC_EDIT_G_START_TIME)->EnableWindow(true);
		GetDlgItem(IDC_EDIT_G_END_TIME)->EnableWindow(true);
		m_checkBoxTime.SetCheck(false);
	}
	else
	{
		GetDlgItem(IDC_EDIT_START_TIME)->EnableWindow(true);
		GetDlgItem(IDC_EDIT_END_TIME)->EnableWindow(true);
		GetDlgItem(IDC_EDIT_G_START_TIME)->EnableWindow(false);
		GetDlgItem(IDC_EDIT_G_END_TIME)->EnableWindow(false);
		m_checkBoxTime.SetCheck(true);
	}
}

void KGValudEditDateTimeDlg::OnBnClickedCheck()
{
	int nRetCode = false;

	nRetCode = m_checkBoxTime.GetCheck();

	if (nRetCode)
	{
		GetDlgItem(IDC_EDIT_START_TIME)->EnableWindow(true);
		GetDlgItem(IDC_EDIT_END_TIME)->EnableWindow(true);
		GetDlgItem(IDC_EDIT_G_START_TIME)->EnableWindow(false);
		GetDlgItem(IDC_EDIT_G_END_TIME)->EnableWindow(false);
		m_checkBoxGameTime.SetCheck(false);
	}
	else
	{
		GetDlgItem(IDC_EDIT_START_TIME)->EnableWindow(false);
		GetDlgItem(IDC_EDIT_END_TIME)->EnableWindow(false);
		GetDlgItem(IDC_EDIT_G_START_TIME)->EnableWindow(true);
		GetDlgItem(IDC_EDIT_G_END_TIME)->EnableWindow(true);
		m_checkBoxGameTime.SetCheck(true);
	}
}
