// KEditorSettingPageMusic.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KEditorSettingPageMusic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingPageMusic property page

IMPLEMENT_DYNCREATE(KEditorSettingPageMusic, CPropertyPage)

KEditorSettingPageMusic::KEditorSettingPageMusic() : CPropertyPage(KEditorSettingPageMusic::IDD)
{
	//{{AFX_DATA_INIT(KEditorSettingPageMusic)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

KEditorSettingPageMusic::~KEditorSettingPageMusic()
{
}

void KEditorSettingPageMusic::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KEditorSettingPageMusic)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KEditorSettingPageMusic, CPropertyPage)
	//{{AFX_MSG_MAP(KEditorSettingPageMusic)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingPageMusic message handlers
