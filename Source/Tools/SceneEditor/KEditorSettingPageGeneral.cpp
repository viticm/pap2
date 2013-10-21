// KEditorSettingPageGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KEditorSettingPageGeneral.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingPageGeneral property page

IMPLEMENT_DYNCREATE(KEditorSettingPageGeneral, CPropertyPage)

KEditorSettingPageGeneral::KEditorSettingPageGeneral() : CPropertyPage(KEditorSettingPageGeneral::IDD)
{
	//{{AFX_DATA_INIT(KEditorSettingPageGeneral)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

KEditorSettingPageGeneral::~KEditorSettingPageGeneral()
{
}

void KEditorSettingPageGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KEditorSettingPageGeneral)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KEditorSettingPageGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(KEditorSettingPageGeneral)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingPageGeneral message handlers
