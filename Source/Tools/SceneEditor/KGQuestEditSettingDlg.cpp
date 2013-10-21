// KGQuestEditSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGQuestEditSettingDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KGQuestEditSettingDlg dialog

IMPLEMENT_DYNAMIC(KGQuestEditSettingDlg, CDialog)

KGQuestEditSettingDlg::KGQuestEditSettingDlg(CWnd* pParent)
	: CDialog(KGQuestEditSettingDlg::IDD, pParent)
{
	m_pMainFrame = (KGObjectPropertyEditTabDlg*)pParent;
}

KGQuestEditSettingDlg::~KGQuestEditSettingDlg()
{
}

void KGQuestEditSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CHECK1, m_bnPreSerialSwitch);
	DDX_Control(pDX, IDC_CHECK2, m_bnAftRowNumSwtich);
	DDX_Control(pDX, IDC_CHECK3, m_bnMarkNameSwitch);
	DDX_Control(pDX, IDC_CHECK4, m_bnAppendWordSwitch);

	DDX_Control(pDX, IDC_COMBO1, m_comboxMark1);
	DDX_Control(pDX, IDC_COMBO2, m_comboxMark2);
	DDX_Control(pDX, IDC_COMBO3, m_comboxMark3);
	DDX_Control(pDX, IDC_COMBO4, m_comboxMark4);
}


BEGIN_MESSAGE_MAP(KGQuestEditSettingDlg, CDialog)
	ON_BN_CLICKED(IDOK, &KGQuestEditSettingDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL KGQuestEditSettingDlg::OnInitDialog()
{
	int nResult  = false;
	int nRetCode = false;

	int  nTempValue  = 0;
	int  nColsNumber = 0;
	int  nCol = 0;
	int  nPos = 0;
	CString szCurMark;
	CString szMarkTemp;

	char szSectionName[256];
	char szTempValue[256];
	*szSectionName = '\0';
	*szTempValue   = '\0';

	CDialog::OnInitDialog();

	KG_PROCESS_ERROR(m_pMainFrame->m_pTabFile);
	KG_PROCESS_ERROR(m_pMainFrame->m_pIniFile);

	nColsNumber = m_pMainFrame->m_pTabFile->GetWidth();

	m_pMainFrame->m_pIniFile->GetInteger(
		_T("Setting"), _T("PreNumber"), 0, &nTempValue
	);
	if (nTempValue)
	{
		m_bnPreSerialSwitch.SetCheck(true);
	}

	m_pMainFrame->m_pIniFile->GetInteger(
		_T("Setting"), _T("AftRowNumber"), 0, &nTempValue
	);
	if (nTempValue)
	{
		m_bnAftRowNumSwtich.SetCheck(true);
	}

	m_pMainFrame->m_pIniFile->GetInteger(
		_T("Setting"), _T("ShowName"), 0, &nTempValue
	);
	if (nTempValue)
	{
		m_bnMarkNameSwitch.SetCheck(true);
	}

	m_pMainFrame->m_pIniFile->GetInteger(
		_T("Setting"), _T("AppendWordFlag"), 0, &nTempValue
	);
	if (!nTempValue)
	{
		m_bnAppendWordSwitch.SetCheck(true);
	}

	m_comboxMark1.AddString("==空项==");
	m_comboxMark2.AddString("==空项==");
	m_comboxMark3.AddString("==空项==");
	m_comboxMark4.AddString("==空项==");

	for (nCol = 1; nCol <= nColsNumber; nCol++)
	{
		m_pMainFrame->m_pTabFile->GetString(
			1, nCol, "", szTempValue, sizeof(szTempValue)
		);
		if (CString(szTempValue) != "")
		{
			char szName[256];
			*szName = '\0';
			m_pMainFrame->m_pIniFile->GetString(
				szTempValue, _T("Name"), "", szName, sizeof(szName)
			);
			if (CString(szName) != "")
			{
				m_comboxMark1.AddString(szName);
				m_comboxMark2.AddString(szName);
				m_comboxMark3.AddString(szName);
				m_comboxMark4.AddString(szName);
			}
		}
	}

	m_pMainFrame->m_pIniFile->GetString(
		_T("Setting"),_T("After_TreeItemText"), "", szTempValue, sizeof(szTempValue)
	);
	szCurMark = szTempValue;

	szMarkTemp = szCurMark.Tokenize(_T(","), nPos);
	if (nPos != -1)
		m_pMainFrame->m_pIniFile->GetString(
			szMarkTemp.GetBuffer(), _T("Name"), "", szTempValue, sizeof(szTempValue)
		);
	else
		sprintf_s(szTempValue, "%s", _T("==空项=="));
	m_comboxMark1.SelectString(0, szTempValue);

	if (nPos != -1)
	{
		szMarkTemp = szCurMark.Tokenize(_T(","), nPos);
		m_pMainFrame->m_pIniFile->GetString(
			szMarkTemp.GetBuffer(), _T("Name"), "", szTempValue, sizeof(szTempValue)
		);
	}
	if (nPos == -1)
		sprintf_s(szTempValue, "%s", _T("==空项=="));
	m_comboxMark2.SelectString(0, szTempValue);

	if (nPos != -1)
	{
		szMarkTemp = szCurMark.Tokenize(_T(","), nPos);
		m_pMainFrame->m_pIniFile->GetString(
			szMarkTemp.GetBuffer(), _T("Name"), "", szTempValue, sizeof(szTempValue)
		);
	}
	if (nPos == -1)
		sprintf_s(szTempValue, "%s", _T("==空项=="));
	m_comboxMark3.SelectString(0, szTempValue);

	if (nPos != -1)
	{
		szMarkTemp = szCurMark.Tokenize(_T(","), nPos);
		m_pMainFrame->m_pIniFile->GetString(
			szMarkTemp.GetBuffer(), _T("Name"), "", szTempValue, sizeof(szTempValue)
		);
	}
	if (nPos == -1)
		sprintf_s(szTempValue, "%s", _T("==空项=="));
	m_comboxMark4.SelectString(0, szTempValue);

//Exit1:
	nResult = true;
Exit0:
	return nResult;
}

CString KGQuestEditSettingDlg::GetPlateName(CString szString)
{
	char szSectionName[256];
	char szKeyValue[256];
	*szSectionName = '\0';
	*szKeyValue    = '\0';
	CString szRetString = "";

	while (m_pMainFrame->m_pIniFile->GetNextSection(szSectionName, szSectionName))
	{
		m_pMainFrame->m_pIniFile->GetString(
			szSectionName, _T("Name"), "", szKeyValue, sizeof(szKeyValue)
		);
		if (CString(szKeyValue) == szString)
		{
			szRetString = szSectionName;
			break;
		}
	}

	return szRetString;
}

void KGQuestEditSettingDlg::OnBnClickedOk()
{
	int nResult  = false;
	int nRetCode = false;

	CString szAftMark = "";
	CString szString  = "";
	int     nIndex = 0;
	char    szMarkName[256];
	*szMarkName = '\0';

	UpdateData();

	KG_PROCESS_ERROR(m_pMainFrame->m_pTabFile);
	KG_PROCESS_ERROR(m_pMainFrame->m_pIniFile);

	if (m_bnPreSerialSwitch.GetCheck())
	{
		m_pMainFrame->m_pIniFile->WriteInteger(_T("Setting"), _T("PreNumber"), 1);
	}
	else
	{
		m_pMainFrame->m_pIniFile->WriteInteger(_T("Setting"), _T("PreNumber"), 0);
	}

	if (m_bnAftRowNumSwtich.GetCheck())
	{
		m_pMainFrame->m_pIniFile->WriteInteger(_T("Setting"), _T("AftRowNumber"), 1);
	}
	else
	{
		m_pMainFrame->m_pIniFile->WriteInteger(_T("Setting"), _T("AftRowNumber"), 0);
	}

	if (m_bnMarkNameSwitch.GetCheck())
	{
		m_pMainFrame->m_pIniFile->WriteInteger(_T("Setting"), _T("ShowName"), 1);
	}
	else
	{
		m_pMainFrame->m_pIniFile->WriteInteger(_T("Setting"), _T("ShowName"), 0);
	}

	if (m_bnAppendWordSwitch.GetCheck())
	{
		m_pMainFrame->m_pIniFile->WriteInteger(_T("Setting"), _T("AppendWordFlag"), 0);
	}
	else
	{
		m_pMainFrame->m_pIniFile->WriteInteger(_T("Setting"), _T("AppendWordFlag"), 1);
	}

	nIndex = m_comboxMark1.GetCurSel();
	m_comboxMark1.GetLBText(nIndex, szString);
	szAftMark += GetPlateName(szString) + _T(",");

	nIndex = m_comboxMark2.GetCurSel();
	m_comboxMark2.GetLBText(nIndex, szString);
	szAftMark += GetPlateName(szString) + _T(",");

	nIndex = m_comboxMark3.GetCurSel();
	m_comboxMark3.GetLBText(nIndex, szString);
	szAftMark += GetPlateName(szString) + _T(",");

	nIndex = m_comboxMark4.GetCurSel();
	m_comboxMark4.GetLBText(nIndex, szString);
	szAftMark += GetPlateName(szString) + _T(",");

	m_pMainFrame->m_pIniFile->WriteString(
		_T("Setting"), _T("After_TreeItemText"), szAftMark.GetBuffer()
	);

	nResult = true;
Exit0:
	OnOK();
}
