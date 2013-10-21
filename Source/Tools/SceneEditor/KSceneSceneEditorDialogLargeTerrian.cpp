// KSceneSceneEditorDialogLargeTerrian.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneSceneEditorDialogLargeTerrian.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorDialogLargeTerrian dialog


KSceneSceneEditorDialogLargeTerrian::KSceneSceneEditorDialogLargeTerrian(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogLargeTerrian::IDD, pParent)
{
	//{{AFX_DATA_INIT(KSceneSceneEditorDialogLargeTerrian)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void KSceneSceneEditorDialogLargeTerrian::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneSceneEditorDialogLargeTerrian)
	DDX_Control(pDX, IDC_STATIC_VIEW, m_Static_View);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogLargeTerrian, CDialog)
	//{{AFX_MSG_MAP(KSceneSceneEditorDialogLargeTerrian)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorDialogLargeTerrian message handlers

LRESULT KSceneSceneEditorDialogLargeTerrian::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::WindowProc(message, wParam, lParam);
}
