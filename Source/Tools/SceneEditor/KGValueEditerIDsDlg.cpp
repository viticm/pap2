// KGValueEditerIDsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGValueEditerIDsDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KGValueEditerIDsDlg dialog

IMPLEMENT_DYNAMIC(KGValueEditerIDsDlg, CDialog)

KGValueEditerIDsDlg::KGValueEditerIDsDlg(
	ITabFile* pTabFile, LPKGLISTITEM pItem, CString strWndTitle, CWnd* pParent
) : CDialog(KGValueEditerIDsDlg::IDD, pParent)
{
	m_pTabFile = pTabFile;
	m_pItem = pItem;
	m_strWndTitle = strWndTitle;
	m_nSelID	  = 0;
	m_strID		  = TEXT("");
	m_pParant = pParent;
}

KGValueEditerIDsDlg::~KGValueEditerIDsDlg()
{
}

void KGValueEditerIDsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_COMBOX_IDS, m_strID);
}


BEGIN_MESSAGE_MAP(KGValueEditerIDsDlg, CDialog)
	ON_BN_CLICKED(IDOK, &KGValueEditerIDsDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL KGValueEditerIDsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CComboBox* pCombox = (CComboBox*)GetDlgItem(IDC_COMBOX_IDS);
	ITabFile* pTabFile = NULL;
	USER_ITEM_DATA itemDate;
	char szFullPath[MAX_PATH];

	m_pItem->GetStructData(&itemDate, sizeof(itemDate));
	g_GetFullPath(szFullPath, itemDate.szParamName_1);

    //
    char szDepend[128];
    *szDepend = '\0';
    if (m_pTabFile)
    {
        m_pTabFile->GetString(
            itemDate.nRowsIndexInFile, 
            itemDate.szParamName_3,
            szDepend,
            szDepend,
            sizeof(szDepend)
            );
    }
    //


    SAFE_RELEASE(pTabFile);
	pTabFile = g_OpenTabFile(szFullPath);
	if (pTabFile)
	{
		int nRows = pTabFile->GetHeight();
		char szId[24];
		for (int i = 3; i <= nRows; i++)
		{

            if (*szDepend != '\0')
            {
                char szTemp[128];
                *szTemp = '\0';
                pTabFile->GetString(i, itemDate.szParamName_3, "", szTemp, sizeof(szTemp));
                if (stricmp(szDepend, szTemp))
                    continue;
            }
			pTabFile->GetString(i, itemDate.szParamName_2, "", szId, sizeof(szId));
			if (*szId != '\0')
				pCombox->AddString(szId);
		}
	}

	USER_ITEM_DATA item;
	char szId[24];
	m_pItem->GetStructData(&item, sizeof(item));
	m_pTabFile->GetString(item.nRowsIndexInFile, itemDate.szParamName_2, "", szId, sizeof(szId));
	pCombox->SelectString(0, szId);
	SetWindowText(m_strWndTitle);
	SAFE_RELEASE(pTabFile);
	return TRUE;
}

int KGValueEditerIDsDlg::GetID()
{
	return m_nSelID;
}


// KGValueEditerIDsDlg message handlers

void KGValueEditerIDsDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	m_nSelID = atoi(m_strID.GetBuffer());
	USER_ITEM_DATA itemData;

	m_pItem->GetStructData(&itemData, sizeof(itemData));

	m_pTabFile->WriteString(
		itemData.nRowsIndexInFile, itemData.szPropertyName, m_strID.GetBuffer()
	);

	strncpy_s(
		itemData.szPropertyValue, m_strID.GetBuffer(),
		sizeof(itemData.szPropertyValue)
	);
	itemData.szPropertyValue[sizeof(itemData.szPropertyValue) - 1] ='\0';

	m_pItem->SetStructData(&itemData);

	if (m_pParant)
		m_pParant->SendMessage(WM_USER_UPDATE_LIST_ITEM, (WPARAM)m_pItem);

	OnOK();
}
