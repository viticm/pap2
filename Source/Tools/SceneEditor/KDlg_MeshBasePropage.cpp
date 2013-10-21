// KDlg_MeshBasePropage.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_MeshBasePropage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_MeshBasePropage property page

IMPLEMENT_DYNCREATE(KDlg_MeshBasePropage, CPropertyPage)

KDlg_MeshBasePropage::KDlg_MeshBasePropage() : CPropertyPage(KDlg_MeshBasePropage::IDD)
{
	//{{AFX_DATA_INIT(KDlg_MeshBasePropage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

KDlg_MeshBasePropage::~KDlg_MeshBasePropage()
{
}

void KDlg_MeshBasePropage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_MeshBasePropage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KDlg_MeshBasePropage, CPropertyPage)
	//{{AFX_MSG_MAP(KDlg_MeshBasePropage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KDlg_MeshBasePropage message handlers
