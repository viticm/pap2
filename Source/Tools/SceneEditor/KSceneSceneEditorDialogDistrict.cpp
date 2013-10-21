// KSceneSceneEditorDialogDistrict.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneSceneEditorDialogDistrict.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorDialogDistrict dialog


KSceneSceneEditorDialogDistrict::KSceneSceneEditorDialogDistrict(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogDistrict::IDD, pParent)
{
	//{{AFX_DATA_INIT(KSceneSceneEditorDialogDistrict)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void KSceneSceneEditorDialogDistrict::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneSceneEditorDialogDistrict)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogDistrict, CDialog)
	//{{AFX_MSG_MAP(KSceneSceneEditorDialogDistrict)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorDialogDistrict message handlers
