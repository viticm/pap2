// MasterListDialogBar.cpp : implementation file
//

#include "stdafx.h"
#include "RecordeOperator.h"
#include "MasterListDialogBar.h"
#include "RecordeOperatorDoc.h"


// MasterListDialogBar dialog

IMPLEMENT_DYNAMIC(MasterListDialogBar, CDialogBar)

MasterListDialogBar::MasterListDialogBar()
{

}

MasterListDialogBar::~MasterListDialogBar()
{
}

void MasterListDialogBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MASTER_TREE, m_treeMasters);
	DDX_Control(pDX, IDC_MASTER_LISTCONTROL, m_listCtrl);
}

LONG MasterListDialogBar::OnInitDialog ( UINT wParam, LONG lParam)
{
	BOOL bRet = HandleInitDialog(wParam, lParam);
	if (!UpdateData(FALSE))
	{

	}
	return bRet;
}

//CSize MasterListDialogBar::CalcDynamicLayout(int nLength, DWORD nMode)
//{
//	CRect rcFrame;
//	GetDockingFrame() ->GetClientRect(&rcFrame);
//	return CSize(400, 5 * rcFrame.Height() / 6);
//}


BEGIN_MESSAGE_MAP(MasterListDialogBar, CDialogBar)
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog )  
END_MESSAGE_MAP()


// MasterListDialogBar message handlers
