// KGValueEditTemplateIdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGValueEditTemplateIdDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KGValueEditTemplateIdDlg dialog

IMPLEMENT_DYNAMIC(KGValueEditTemplateIdDlg, CDialog)

KGValueEditTemplateIdDlg::KGValueEditTemplateIdDlg(
	ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, CWnd* pParent
) : CDialog(KGValueEditTemplateIdDlg::IDD, pParent)
{
	m_pTabFile  = pTabFile;
	m_pIniFile  = pIniFile;
	m_pListItem = pListItem;

}

KGValueEditTemplateIdDlg::~KGValueEditTemplateIdDlg()
{
}

void KGValueEditTemplateIdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBOX_IDS, m_comboxIDs);
}


BEGIN_MESSAGE_MAP(KGValueEditTemplateIdDlg, CDialog)
END_MESSAGE_MAP()

BOOL KGValueEditTemplateIdDlg::OnInitDialog()
{
	int nResult  = false;
	int nRetCode = false;

	USER_ITEM_DATA itemData;

	char szTemplateIniFile[MAX_PATH];

	char szTitleValue[64];
	char szTitleName[64];
	char szIDsName[64];
	char szIDsValue[8];

	*szTemplateIniFile	 = '\0';
	*szTitleValue		 = '\0';
	*szTitleName		 = '\0';
	*szIDsName			 = '\0';
	*szIDsValue			 = '\0';

	nRetCode = CDialog::OnInitDialog();
	KGLOG_PROCESS_ERROR(nRetCode);

	KGLOG_PROCESS_ERROR(m_pListItem);
	KGLOG_PROCESS_ERROR(m_pTabFile);
	KGLOG_PROCESS_ERROR(m_pIniFile);

	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	switch (itemData.dwValueType)
	{
	case VALUE_NPC_TEMPLATE :

		break;
	}



	nResult = true;
Exit0:
	return nResult;
}

// KGValueEditTemplateIdDlg message handlers
