#include "stdafx.h"
#include "SceneEditor.h"
#include "KGValueEidtPrequestIDsDlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

// KGValueEidtPrequestIDsDlg dialog

IMPLEMENT_DYNAMIC(KGValueEidtPrequestIDsDlg, CDialog)

KGValueEidtPrequestIDsDlg::KGValueEidtPrequestIDsDlg(
	ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem,
	CWnd* pParent
) : CDialog(KGValueEidtPrequestIDsDlg::IDD, pParent)
	, m_szPreName1(_T(""))
	, m_szPreName2(_T(""))
	, m_szPreName3(_T(""))
	, m_szPreName4(_T(""))
	, m_szPreID1(_T(""))
	, m_szPreID2(_T(""))
	, m_szPreID3(_T(""))
	, m_szPreID4(_T(""))
{
	m_pTabFile   = pTabFile;
	m_pIniFile	 = pIniFile;
	m_pListItem  = pListItem;
	m_pParant	 = pParent;
}

KGValueEidtPrequestIDsDlg::~KGValueEidtPrequestIDsDlg()
{
}

void KGValueEidtPrequestIDsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NAME1, m_szPreName1);
	DDX_Text(pDX, IDC_NAME2, m_szPreName2);
	DDX_Text(pDX, IDC_NAME3, m_szPreName3);
	DDX_Text(pDX, IDC_NAME4, m_szPreName4);

	DDX_Text(pDX, IDC_ID1, m_szPreID1);
	DDX_Text(pDX, IDC_ID2, m_szPreID2);
	DDX_Text(pDX, IDC_ID3, m_szPreID3);
	DDX_Text(pDX, IDC_ID4, m_szPreID4);

	DDX_Control(pDX, IDC_AND, m_BnAnd);
	DDX_Control(pDX, IDC_OR, m_BnOr);

}

BOOL KGValueEidtPrequestIDsDlg::OnInitDialog()
{
	char szPrevQuestID[256];
	char szValue[256];
	*szPrevQuestID = '\0';
	*szValue	   = '\0';
	int i  = 0;
	int nRowsNumber = m_pTabFile->GetHeight();
	int nLogic = 0;

	CDialog::OnInitDialog();

	USER_ITEM_DATA itemData;
	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	m_pTabFile->GetString(
		itemData.nRowsIndexInFile, itemData.szParamName_2,"",
		szPrevQuestID, sizeof(szPrevQuestID)
	);
	m_szPreID1 = szPrevQuestID;

	for (i = 3; i <= nRowsNumber; i++ )
	{
		m_pTabFile->GetString(
			i, _T("QuestID"), "", szValue, sizeof(szValue)
		);
		if (m_szPreID1 == CString(szValue))
		{
			break;
		}
	}
	if(i <= nRowsNumber)
	{
		m_pTabFile->GetString(
			i, _T("MapName"), "", szValue, sizeof(szValue)
		);
		m_szPreName1 += szValue + CString("->");
		m_pTabFile->GetString(
			i, _T("TaskLinkName"), "", szValue, sizeof(szValue)
		);
		m_szPreName1 += szValue + CString("->");
		m_pTabFile->GetString(
			i, _T("QuestName"), "", szValue, sizeof(szValue)
		);
		m_szPreName1 += szValue;
	}

	m_pTabFile->GetString(
		itemData.nRowsIndexInFile, itemData.szParamName_3, "",
		szPrevQuestID, sizeof(szPrevQuestID)
	);
	m_szPreID2 = szPrevQuestID;

	for (i = 3; i <= nRowsNumber; i++ )
	{
		m_pTabFile->GetString(
			i, _T("QuestID"), "", szValue, sizeof(szValue)
		);
		if (m_szPreID2 == CString(szValue))
		{
			break;
		}
	}
	if(i <= nRowsNumber)
	{
		m_pTabFile->GetString(
			i, _T("MapName"), "", szValue, sizeof(szValue)
		);
		m_szPreName2 += szValue + CString("->");
		m_pTabFile->GetString(
			i, _T("TaskLinkName"), "", szValue, sizeof(szValue)
		);
		m_szPreName2 += szValue + CString("->");
		m_pTabFile->GetString(
			i, _T("QuestName"), "", szValue, sizeof(szValue)
		);
		m_szPreName2 += szValue;
	}

	m_pTabFile->GetString(
		itemData.nRowsIndexInFile, itemData.szParamName_4,"",
		szPrevQuestID, sizeof(szPrevQuestID)
	);
	m_szPreID3 = szPrevQuestID;

	for (i = 3; i <= nRowsNumber; i++ )
	{
		m_pTabFile->GetString(
			i, _T("QuestID"), "", szValue, sizeof(szValue)
		);
		if (m_szPreID3 == CString(szValue))
		{
			break;
		}
	}
	if(i <= nRowsNumber)
	{
		m_pTabFile->GetString(
			i, _T("MapName"), "", szValue, sizeof(szValue)
		);
		m_szPreName3 += szValue + CString("->");
		m_pTabFile->GetString(
			i, _T("TaskLinkName"), "", szValue, sizeof(szValue)
		);
		m_szPreName3 += szValue + CString("->");
		m_pTabFile->GetString(
			i, _T("QuestName"), "", szValue, sizeof(szValue)
		);
		m_szPreName3 += szValue;
	}

	m_pTabFile->GetString(
		itemData.nRowsIndexInFile, itemData.szParamName_5,"",
		szPrevQuestID, sizeof(szPrevQuestID)
	);
	m_szPreID4 = szPrevQuestID;

	for (i = 3; i <= nRowsNumber; i++ )
	{
		m_pTabFile->GetString(
			i, _T("QuestID"), "", szValue, sizeof(szValue)
		);
		if (m_szPreID4 == CString(szValue))
		{
			break;
		}
	}
	if(i <= nRowsNumber)
	{
		m_pTabFile->GetString(
			i, _T("MapName"), "", szValue, sizeof(szValue)
		);
		m_szPreName4 += szValue + CString("->");
		m_pTabFile->GetString(
			i, _T("TaskLinkName"), "", szValue, sizeof(szValue)
		);
		m_szPreName4 += szValue + CString("->");
		m_pTabFile->GetString(
			i, _T("QuestName"), "", szValue, sizeof(szValue)
		);
		m_szPreName4 += szValue;
	}

	m_pTabFile->GetInteger(
		itemData.nRowsIndexInFile, itemData.szParamName_1, 1, &nLogic
	);

	if (nLogic)
	{
		m_BnAnd.SetCheck(true);
		m_BnOr.SetCheck(false);
	}
	else
	{
		m_BnAnd.SetCheck(false);
		m_BnOr.SetCheck(true);
	}

	UpdateData(false);

	return TRUE;
}

BEGIN_MESSAGE_MAP(KGValueEidtPrequestIDsDlg, CDialog)
	ON_BN_CLICKED(IDOK, &KGValueEidtPrequestIDsDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_OR, &KGValueEidtPrequestIDsDlg::OnBnClickedOr)
	ON_BN_CLICKED(IDC_AND, &KGValueEidtPrequestIDsDlg::OnBnClickedAnd)
END_MESSAGE_MAP()

void KGValueEidtPrequestIDsDlg::OnBnClickedOk()
{
	USER_ITEM_DATA itemData;
	CString szValue;
	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	UpdateData();
	if (m_BnAnd.GetCheck())
	{
		szValue = "1";
	}
	if (m_BnOr.GetCheck())
	{
		szValue = "0";
	}

	m_pTabFile->WriteString(
		itemData.nRowsIndexInFile, itemData.szParamName_1, szValue
	);

	m_pListItem->SetStructData(&itemData);
	if (m_pParant)
		m_pParant->SendMessage(WM_USER_UPDATE_LIST_ITEM, (WPARAM)m_pListItem);

	OnOK();
}

void KGValueEidtPrequestIDsDlg::OnBnClickedOr()
{
	if (!m_BnOr.GetCheck())
	{
		m_BnOr.SetCheck(false);
		m_BnAnd.SetCheck(true);
	}
	else
	{
		m_BnOr.SetCheck(true);
		m_BnAnd.SetCheck(false);
	}
}

void KGValueEidtPrequestIDsDlg::OnBnClickedAnd()
{
	if (!m_BnAnd.GetCheck())
	{
		m_BnOr.SetCheck(true);
		m_BnAnd.SetCheck(false);
	}
	else
	{
		m_BnOr.SetCheck(false);
		m_BnAnd.SetCheck(true);
	}
}
