// KGBuildListItemDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGBuildListItemDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KGBuildListItemDlg 对话框

IMPLEMENT_DYNAMIC(KGBuildListItemDlg, CDialog)
KGBuildListItemDlg::KGBuildListItemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(KGBuildListItemDlg::IDD, pParent)
{
}

KGBuildListItemDlg::~KGBuildListItemDlg()
{
}

void KGBuildListItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KGBuildListItemDlg, CDialog)
END_MESSAGE_MAP()


// KGBuildListItemDlg 消息处理程序
