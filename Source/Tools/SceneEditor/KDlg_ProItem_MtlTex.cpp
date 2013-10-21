// KDlg_ProItem_MtlTex.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_ProItem_MtlTex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_MtlTex dialog
namespace PropertyBase
{

KDlg_ProItem_MtlTex::KDlg_ProItem_MtlTex(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_ProItem_MtlTex::IDD, pParent)
{
	//{{AFX_DATA_INIT(KDlg_ProItem_MtlTex)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void KDlg_ProItem_MtlTex::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_ProItem_MtlTex)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KDlg_ProItem_MtlTex, CDialog)
	//{{AFX_MSG_MAP(KDlg_ProItem_MtlTex)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

}
/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_MtlTex message handlers
