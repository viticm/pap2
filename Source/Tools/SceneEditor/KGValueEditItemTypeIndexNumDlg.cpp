// KGValueEditItemTypeIndexNumDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGValueEditItemTypeIndexNumDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KGValueEditItemTypeIndexNumDlg 对话框

IMPLEMENT_DYNAMIC(KGValueEditItemTypeIndexNumDlg, CDialog)
KGValueEditItemTypeIndexNumDlg::KGValueEditItemTypeIndexNumDlg(
	CWnd* pParent, int nItem)
	: CDialog(KGValueEditItemTypeIndexNumDlg::IDD, pParent)
{
}

KGValueEditItemTypeIndexNumDlg::~KGValueEditItemTypeIndexNumDlg()
{
}

void KGValueEditItemTypeIndexNumDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KGValueEditItemTypeIndexNumDlg, CDialog)
END_MESSAGE_MAP()


// KGValueEditItemTypeIndexNumDlg 消息处理程序
