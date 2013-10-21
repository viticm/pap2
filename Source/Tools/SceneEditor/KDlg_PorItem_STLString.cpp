// KDlg_ProItem_STLString.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_PorItem_STLString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_STLString dialog


KDlg_ProItem_STLString::KDlg_ProItem_STLString(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_ProItem_STLString::IDD, pParent)
{
	//{{AFX_DATA_INIT(KDlg_ProItem_STLString)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void KDlg_ProItem_STLString::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_ProItem_STLString)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KDlg_ProItem_STLString, CDialog)
	//{{AFX_MSG_MAP(KDlg_ProItem_STLString)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_STLString message handlers
