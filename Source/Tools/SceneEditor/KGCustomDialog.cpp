////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGCustomDialog.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-4-13 20:18:02
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KGCustomDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KGCustomDialog, CDialog)

KGCustomDialog::KGCustomDialog(
	IIniFile* pIniFile, TCHAR pcszSectionName[], CWnd* pParent
) : CDialog(KGCustomDialog::IDD, pParent)
{
	m_pIniFile = pIniFile;

	*m_szSectionName = '\0';
	if (pcszSectionName)
		_tcsncpy_s(m_szSectionName, pcszSectionName, sizeof(m_szSectionName));
}

KGCustomDialog::~KGCustomDialog(void)
{
}

void KGCustomDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(KGCustomDialog, CDialog)
	ON_BN_CLICKED(IDOK, &KGCustomDialog::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL KGCustomDialog::OnInitDialog()
{
	int nResult  = false;
	int nRetCode = false;

	TCHAR szParam_X[64];
	TCHAR szCaption[64];
	*szCaption = '\0';
	*szParam_X = '\0';

	INT	pnCoordinate[4];
	INT nIndex = 0;
	ZeroMemory(pnCoordinate, sizeof(pnCoordinate));

	nRetCode = CDialog::OnInitDialog();
	KG_PROCESS_ERROR(nRetCode);

	KG_PROCESS_ERROR(*m_szSectionName != '\0');
	KG_PROCESS_ERROR(m_pIniFile);

	nRetCode = m_pIniFile->GetMultiInteger(
		m_szSectionName, _T("Coordinate"), pnCoordinate, sizeof(pnCoordinate)
	);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = m_pIniFile->GetString(
		m_szSectionName, _T("Name"), _T("Caption"), szCaption, sizeof(szCaption)
	);

	SetWindowText(szCaption);
	MoveWindow(
		pnCoordinate[0],  pnCoordinate[1],
		pnCoordinate[0] + pnCoordinate[2],
		pnCoordinate[1] + pnCoordinate[3]
	);
	GetDlgItem(IDCANCEL)->MoveWindow(
		pnCoordinate[0] + pnCoordinate[2] - 95,
		pnCoordinate[1] + pnCoordinate[3] - 50,
		80, 22
	);
	GetDlgItem(IDOK)->MoveWindow(
		pnCoordinate[0] + pnCoordinate[2] - 180,
		pnCoordinate[1] + pnCoordinate[3] - 50,
		80, 22
	);

	for (nIndex = 0; nIndex < CTRL_NUM; nIndex++)
	{
		TCHAR szDataType[64];
		TCHAR szTitle[64];
		*szDataType = '\0';
		*szTitle    = '\0';

		ZeroMemory(pnCoordinate, sizeof(pnCoordinate));
		_stprintf_s(szParam_X, _T("%s%d"), _T("PARAM_"), nIndex + 1);

		nRetCode = nRetCode = m_pIniFile->GetString(
			m_szSectionName, szParam_X, _T(""), szCaption, sizeof(szCaption)
		);
		if (!nRetCode)
			break;
		m_pIniFile->GetMultiInteger(
			szCaption, _T("Coordinate"), pnCoordinate, sizeof(pnCoordinate)
		);
		m_pIniFile->GetString(
			szCaption, _T("Name"), _T(""), szTitle, sizeof(szTitle)
		);
		m_pIniFile->GetString(
			szCaption, _T("Type"), _T(""), szDataType, sizeof(szDataType)
		);
		m_ctrlArray[nIndex].Init(
			szCaption, szTitle, GetType(szDataType), pnCoordinate, this, nIndex
		);
	}

	nResult = true;
Exit0:
	if (!nResult)
	{
		MessageBox(_T("Initialize Error!!\t\t"), _T("Error"), MB_OK | MB_ICONERROR);
		EndDialog(false);
	}
	return nResult;;
}

BASE_DATA_TYPE KGCustomDialog::GetType(const TCHAR szType[])
{
	BASE_DATA_TYPE eCtrlType = TYPE_UNKNOW;

	if (!_tcscmp(szType, _T("MULI_STRING")))
		eCtrlType = TYPE_MULTI_STRING;
	else if (!_tcscmp(szType, _T("STRING")))
		eCtrlType = TYPE_SINGL_STRING;
	else if (!_tcscmp(szType, _T("INT")))
		eCtrlType = TYPE_INTEGER;
	else
		eCtrlType = TYPE_UNKNOW;

	return eCtrlType;
}

void KGCustomDialog::OnBnClickedOk()
{
	//OnOK();
}

BOOL KGCustomDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	else
	{
		return CDialog::PreTranslateMessage(pMsg);
	}
}

CustomCtrl::CustomCtrl()
{
	m_pWndCtrl   = NULL;
	m_szPropertyName = _T("");
	m_szCtrlName = _T("");
	m_szValue    = _T("");
	m_eCtrlType  = TYPE_INTEGER;
}

int CustomCtrl::Init(
	CString	m_szPropertyName, CString szCtrlName, BASE_DATA_TYPE eCtrlType,
	const INT pnCoor[], CWnd* pParent, int nID
)
{
	CString szCaption = _T("");
	RECT rectWnd;
	SIZE sizeName;

	m_szCtrlName = szCtrlName;
	m_eCtrlType  = eCtrlType;
	szCaption    = m_szCtrlName + _T(":");

	ZeroMemory(&rectWnd, sizeof(rectWnd));
	m_spinCtrl.Create(WS_CHILD | UDS_SETBUDDYINT | WS_VISIBLE, rectWnd, pParent, nID + 200);

	switch (m_eCtrlType)
	{
	case TYPE_SINGL_STRING :
		ZeroMemory(&sizeName, sizeof(sizeName));
		::GetTextExtentPoint(
			pParent->GetDC()->m_hDC, szCaption.GetBuffer(), szCaption.GetLength(),
			&sizeName
		);
		if (sizeName.cx > pnCoor[2] / 2)
			sizeName.cx = pnCoor[2] / 2;
		SetRect(&rectWnd, pnCoor[0], pnCoor[1] + 2, pnCoor[0] + sizeName.cx, pnCoor[1] + 20);
		m_staticCaption.Create(
			szCaption.GetBuffer(), WS_CHILD | WS_VISIBLE, rectWnd, pParent, nID + 100
		);
		m_staticCaption.SetFont(pParent->GetFont());
		m_pWndCtrl = new CEdit();
		SetRect(&rectWnd, pnCoor[0] + sizeName.cx, pnCoor[1],  pnCoor[0] + pnCoor[2], pnCoor[1] + 20);
		((CEdit*)m_pWndCtrl)->Create(
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_WANTRETURN,
			rectWnd, pParent, nID
		);
		m_pWndCtrl->SetFont(pParent->GetFont());
		break;
	case TYPE_MULTI_STRING :
		ZeroMemory(&sizeName, sizeof(sizeName));
		SetRect(&rectWnd, pnCoor[0], pnCoor[1], pnCoor[0] + pnCoor[2], pnCoor[1] + pnCoor[3]);
		m_staticCaption.Create(
			szCaption.GetBuffer(), WS_CHILD | WS_VISIBLE, rectWnd, pParent, nID + 100
		);
		m_staticCaption.SetFont(pParent->GetFont());
		m_pWndCtrl = new CEdit();
		SetRect(&rectWnd, pnCoor[0], pnCoor[1] + 18, pnCoor[0] + pnCoor[2], pnCoor[1] + pnCoor[3]);
		((CEdit*)m_pWndCtrl)->Create(
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOHSCROLL | ES_WANTRETURN,
			rectWnd, pParent, nID
		);
		m_pWndCtrl->SetFont(pParent->GetFont());
		break;
	case TYPE_INTEGER :
		ZeroMemory(&sizeName, sizeof(sizeName));
		::GetTextExtentPoint(
			pParent->GetDC()->m_hDC, szCaption.GetBuffer(), szCaption.GetLength(),
			&sizeName
		);
		if (sizeName.cx > pnCoor[2] / 2)
			sizeName.cx = pnCoor[2] / 2;
		SetRect(&rectWnd, pnCoor[0], pnCoor[1] + 2, pnCoor[0] + sizeName.cx, pnCoor[1] + 20);
		m_staticCaption.Create(
			szCaption.GetBuffer(), WS_CHILD | WS_VISIBLE, rectWnd, pParent, nID + 100
		);
		m_staticCaption.SetFont(pParent->GetFont());
		m_pWndCtrl = new CEdit();
		SetRect(&rectWnd, pnCoor[0] + sizeName.cx, pnCoor[1],  pnCoor[0] + pnCoor[2] - 20, pnCoor[1] + 20);
		((CEdit*)m_pWndCtrl)->Create(
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_WANTRETURN | ES_NUMBER,
			rectWnd, pParent, nID
		);
		m_pWndCtrl->SetFont(pParent->GetFont());
		m_spinCtrl.SetBuddy(m_pWndCtrl);
		SetRect(&rectWnd, pnCoor[0] + pnCoor[2] - 20, pnCoor[1], pnCoor[0] + pnCoor[2], pnCoor[1] + 20);
		m_spinCtrl.MoveWindow(&rectWnd);
		m_spinCtrl.SetRange32(0, 100);
		break;
	}
	return true;
}