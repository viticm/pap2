// KSceneObjectEditorPropertyPageBlock.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneObjectEditorPropertyPageBlock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorPropertyPageBlock property page

IMPLEMENT_DYNCREATE(KSceneObjectEditorPropertyPageBlock, CPropertyPage)

KSceneObjectEditorPropertyPageBlock::KSceneObjectEditorPropertyPageBlock() : CPropertyPage(KSceneObjectEditorPropertyPageBlock::IDD)
{
	//{{AFX_DATA_INIT(KSceneObjectEditorPropertyPageBlock)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

KSceneObjectEditorPropertyPageBlock::~KSceneObjectEditorPropertyPageBlock()
{
}

void KSceneObjectEditorPropertyPageBlock::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneObjectEditorPropertyPageBlock)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KSceneObjectEditorPropertyPageBlock, CPropertyPage)
	//{{AFX_MSG_MAP(KSceneObjectEditorPropertyPageBlock)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorPropertyPageBlock message handlers
