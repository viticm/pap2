////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEdit4PairParamDlg.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-4-25 22:38:27
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGValueEdit4PairParamDlg.h"
#include "KGLog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KGValueEdit4PairParamDlg, CDialog)

KGValueEdit4PairParamDlg::KGValueEdit4PairParamDlg(
	ITabFile* pTabFile, IIniFile* pIniFile,
	LPKGLISTITEM pListItem, CWnd* pParent
) : CDialog(KGValueEdit4PairParamDlg::IDD, pParent)
{
	m_pTabFile   = pTabFile;
	m_pIniFile	 = pIniFile;
	m_pListItem  = pListItem;
	m_pParant	 = pParent;
}

KGValueEdit4PairParamDlg::~KGValueEdit4PairParamDlg()
{
}

BOOL KGValueEdit4PairParamDlg::OnInitDialog()
{
	int nResult  = false;
	int nRetCode = false;

	USER_ITEM_DATA itemData;
	char szValue[64];
	nRetCode = CDialog::OnInitDialog();
	KGLOG_PROCESS_ERROR(nRetCode);

	KGLOG_PROCESS_ERROR(m_pTabFile);
	KGLOG_PROCESS_ERROR(m_pIniFile);
	KGLOG_PROCESS_ERROR(m_pListItem);

	m_pListItem->GetStructData(& itemData, sizeof(itemData));

	m_pTabFile->GetString(
		itemData.nRowsIndexInFile, itemData.szParamName_1, _T(""), szValue, sizeof(szValue)
	);
	m_stringParam_1 = szValue;
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, itemData.szParamName_2, 0, &m_nParam_1);

	m_pTabFile->GetString(
		itemData.nRowsIndexInFile, itemData.szParamName_3, _T(""), szValue, sizeof(szValue)
	);
	m_stringParam_2 = szValue;
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, itemData.szParamName_4, 0, &m_nParam_2);

	m_pTabFile->GetString(
		itemData.nRowsIndexInFile, itemData.szParamName_5, _T(""), szValue, sizeof(szValue)
	);
	m_stringParam_3 = szValue;
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, itemData.szParamName_6, 0, &m_nParam_3);

	m_pTabFile->GetString(
		itemData.nRowsIndexInFile, itemData.szParamName_7, _T(""), szValue, sizeof(szValue)
	);
	m_stringParam_4 = szValue;
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, itemData.szParamName_8, 0, &m_nParam_4);

	m_spinCtrl_1.SetBuddy(GetDlgItem(IDC_EDIT_1_2));
	m_spinCtrl_2.SetBuddy(GetDlgItem(IDC_EDIT_2_2));
	m_spinCtrl_3.SetBuddy(GetDlgItem(IDC_EDIT_3_2));
	m_spinCtrl_4.SetBuddy(GetDlgItem(IDC_EDIT_4_2));

	m_spinCtrl_1.SetRange32(itemData.nMin, itemData.nMax);
	m_spinCtrl_2.SetRange32(itemData.nMin, itemData.nMax);
	m_spinCtrl_3.SetRange32(itemData.nMin, itemData.nMax);
	m_spinCtrl_4.SetRange32(itemData.nMin, itemData.nMax);

	m_pIniFile->GetString(
		itemData.szPropertyName, _T("Name"),  _T(""), szValue, sizeof(szValue)
	);
	SetWindowText(szValue);
	UpdateData(FALSE);

	nResult = true;
Exit0:
	return nResult;
}

void KGValueEdit4PairParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SPIN_1,   m_spinCtrl_1);
	DDX_Control(pDX, IDC_SPIN_2,   m_spinCtrl_2);
	DDX_Control(pDX, IDC_SPIN_3,   m_spinCtrl_3);
	DDX_Control(pDX, IDC_SPIN_4,   m_spinCtrl_4);

	DDX_Control(pDX, IDC_BUTTON_1, m_bnParam_1);
	DDX_Control(pDX, IDC_BUTTON_2, m_bnParam_2);
	DDX_Control(pDX, IDC_BUTTON_3, m_bnParam_3);
	DDX_Control(pDX, IDC_BUTTON_4, m_bnParam_4);

	DDX_Text(pDX, IDC_EDIT_1_1,	   m_stringParam_1);
	DDX_Text(pDX, IDC_EDIT_2_1,	   m_stringParam_2);
	DDX_Text(pDX, IDC_EDIT_3_1,	   m_stringParam_3);
	DDX_Text(pDX, IDC_EDIT_4_1,	   m_stringParam_4);

	DDX_Text(pDX, IDC_EDIT_1_2,	   m_nParam_1);
	DDX_Text(pDX, IDC_EDIT_2_2,	   m_nParam_2);
	DDX_Text(pDX, IDC_EDIT_3_2,	   m_nParam_3);
	DDX_Text(pDX, IDC_EDIT_4_2,	   m_nParam_4);
}


BEGIN_MESSAGE_MAP(KGValueEdit4PairParamDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_1, &KGValueEdit4PairParamDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_2, &KGValueEdit4PairParamDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON_3, &KGValueEdit4PairParamDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON_4, &KGValueEdit4PairParamDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDOK, &KGValueEdit4PairParamDlg::OnBnClickedOk)
END_MESSAGE_MAP()


int KGValueEdit4PairParamDlg::UpdateParam(CString* pStringParam)
{
	int nResult  = false;
	int nRetCode = false;

	USER_ITEM_DATA itemData;

	KGLOG_PROCESS_ERROR(pStringParam);
	KGLOG_PROCESS_ERROR(m_pTabFile);
	KGLOG_PROCESS_ERROR(m_pIniFile);
	KGLOG_PROCESS_ERROR(m_pListItem);

	m_pListItem->GetStructData(& itemData, sizeof(itemData));

	{
		CFileDialog dlg(
			TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			itemData.szFileFiter, this
			/*_T("模型文件|*.mesh|材质文件|*.mtl|动画文件|*.ani|所有文件|*.*|"), this*/
		);
		if (dlg.DoModal() == IDOK)
		{
			char szRetPath[MAX_PATH];
			*pStringParam = dlg.GetPathName();
			nRetCode = g_GetFilePathFromFullPath(&szRetPath[0], pStringParam->GetBuffer());
			if (!nRetCode)
			{
				::MessageBox(
					this->m_hWnd, _T("只能选择在工作目录中的文件!!\t\t"), _T("Error!!"),
					MB_OK | MB_ICONERROR
				);
				KGLOG_PROCESS_ERROR(false);
			}
			*pStringParam = szRetPath;
		}
	}

	UpdateData(FALSE);

	nResult = true;
Exit0:
	return nResult;
}

void KGValueEdit4PairParamDlg::OnBnClickedButton1()
{
	UpdateParam(&m_stringParam_1);
}

void KGValueEdit4PairParamDlg::OnBnClickedButton2()
{
	UpdateParam(&m_stringParam_2);
}

void KGValueEdit4PairParamDlg::OnBnClickedButton3()
{
	UpdateParam(&m_stringParam_3);
}

void KGValueEdit4PairParamDlg::OnBnClickedButton4()
{
	UpdateParam(&m_stringParam_4);
}

void KGValueEdit4PairParamDlg::OnBnClickedOk()
{
	int nResult  = false;
	int nRetCode = false;

	USER_ITEM_DATA itemData;

	KGLOG_PROCESS_ERROR(m_pTabFile);
	KGLOG_PROCESS_ERROR(m_pIniFile);
	KGLOG_PROCESS_ERROR(m_pListItem);

	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	UpdateData(TRUE);

	m_pTabFile->WriteString(
		itemData.nRowsIndexInFile, itemData.szParamName_1, m_stringParam_1.GetBuffer()
	);
	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, itemData.szParamName_2, m_nParam_1);

	m_pTabFile->WriteString(
		itemData.nRowsIndexInFile, itemData.szParamName_3, m_stringParam_2.GetBuffer()
	);
	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, itemData.szParamName_4, m_nParam_2);

	m_pTabFile->WriteString(
		itemData.nRowsIndexInFile, itemData.szParamName_5, m_stringParam_3.GetBuffer()
	);
	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, itemData.szParamName_6, m_nParam_3);

	m_pTabFile->WriteString(
		itemData.nRowsIndexInFile, itemData.szParamName_7, m_stringParam_4.GetBuffer()
	);
	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, itemData.szParamName_8, m_nParam_4);

	if (m_pParant)
		m_pParant->SendMessage(WM_USER_UPDATE_LIST_ITEM, (WPARAM)m_pListItem);

Exit0:
	OnOK();
}
