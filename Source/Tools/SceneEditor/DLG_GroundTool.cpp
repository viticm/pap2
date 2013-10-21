// DLG_GroundTool.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "DLG_GroundTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLG_GroundTool dialog
namespace MFCFramework
{

CDLG_GroundTool::CDLG_GroundTool(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_GroundTool::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDLG_GroundTool)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDLG_GroundTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLG_GroundTool)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLG_GroundTool, CDialog)
	//{{AFX_MSG_MAP(CDLG_GroundTool)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CDLG_GroundTool message handlers
