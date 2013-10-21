// KGNewObjectOption.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGNewObjectOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KGNewObjectOption dialog

IMPLEMENT_DYNAMIC(KGNewObjectOption, CDialog)

KGNewObjectOption::KGNewObjectOption(ITabFile* pTabFile, CWnd* pParent /*=NULL*/)
	: CDialog(KGNewObjectOption::IDD, pParent)
{
	m_pTabFile = pTabFile;
	m_nNewRepresentIdFlag = 1;
	m_nOldRepresentIdFlag = 0;
	m_nOldRepresentId	  = 0;
}

KGNewObjectOption::~KGNewObjectOption()
{
}

CString KGNewObjectOption::GetObjectName()
{
	return m_stringObjectName;
}

int KGNewObjectOption::IsNewRepresentId()
{
	return m_nNewRepresentIdFlag;
}

int KGNewObjectOption::GetSelRepresentId()
{
	return m_nOldRepresentId;
}

void KGNewObjectOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO,   m_comboxRepresentIdList);
	DDX_Check(pDX, IDC_CHECK_NEW, m_nNewRepresentIdFlag);
	DDX_Check(pDX, IDC_CHECK_OLD, m_nOldRepresentIdFlag);
	DDX_Text(pDX, IDC_EDIT_NAME,  m_stringObjectName);
}

BEGIN_MESSAGE_MAP(KGNewObjectOption, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_NEW, &KGNewObjectOption::OnBnClickedCheckNew)
	ON_BN_CLICKED(IDC_CHECK_OLD, &KGNewObjectOption::OnBnClickedCheckOld)
END_MESSAGE_MAP()

BOOL KGNewObjectOption::OnInitDialog()
{
	int  nResult  = FALSE;
	int  nRetCode = FALSE;

	int  nRowsCount = 0;
	int  nCurRow    = 0;

	char szRepresentId[64];
	*szRepresentId = '\0';

	KGLOG_PROCESS_ERROR(m_pTabFile);

	nRetCode = CDialog::OnInitDialog();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRowsCount = m_pTabFile->GetHeight();

	for (nCurRow = 3; nCurRow <= nRowsCount; nCurRow++)
	{
		nRetCode = m_pTabFile->GetString(
			nCurRow, _T("RepresentID"), _T(""), szRepresentId, sizeof(szRepresentId)
		);
		//KGLOG_PROCESS_ERROR(nRetCode);
		m_comboxRepresentIdList.AddString(szRepresentId);
	}

	nResult = TRUE;
Exit0:
	return nResult;

}

void KGNewObjectOption::OnBnClickedOk()
{
	CString szRepresentId = _T("");

	UpdateData(TRUE);

	m_comboxRepresentIdList.GetWindowText(szRepresentId);
	m_nOldRepresentId = atoi(szRepresentId.GetBuffer());

	m_stringObjectName.TrimRight();
	m_stringObjectName.TrimLeft();

	if (m_stringObjectName == _T(""))
		MessageBox(_T("名字不能为空!!\t\t"), _T("Error!!"), MB_OK | MB_ICONERROR);
	else if (m_nOldRepresentIdFlag && szRepresentId == _T(""))
		MessageBox(_T("请选择RepresentID!!\t\t"), _T("Error!!"), MB_OK | MB_ICONERROR);
	else
		OnOK();
}

void KGNewObjectOption::OnBnClickedCheckNew()
{
	UpdateData(TRUE);

	if (m_nNewRepresentIdFlag)
	{
		m_nOldRepresentIdFlag = 0;
	}
	else
	{
		m_nOldRepresentIdFlag = 1;
	}

	UpdateData(FALSE);
}

void KGNewObjectOption::OnBnClickedCheckOld()
{
	UpdateData(TRUE);

	if (m_nOldRepresentIdFlag)
	{
		m_nNewRepresentIdFlag = 0;
	}
	else
	{
		m_nNewRepresentIdFlag = 1;
	}

	UpdateData(FALSE);
}
