// KDlg_ProItemBase.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_ProItemBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItemBase dialog
namespace PropertyBase
{

KDlg_ProItemBase::KDlg_ProItemBase(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_ProItemBase::IDD, pParent)
{
	//{{AFX_DATA_INIT(KDlg_ProItemBase)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void KDlg_ProItemBase::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_ProItemBase)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KDlg_ProItemBase, CDialog)
	//{{AFX_MSG_MAP(KDlg_ProItemBase)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
}
/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItemBase message handlers
