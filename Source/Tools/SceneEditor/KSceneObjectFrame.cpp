// KSceneObjectFrame.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneObjectFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectFrame

IMPLEMENT_DYNCREATE(KSceneObjectFrame, CFormView)

KSceneObjectFrame::KSceneObjectFrame()
	: CFormView(KSceneObjectFrame::IDD)
{
	//{{AFX_DATA_INIT(KSceneObjectFrame)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

KSceneObjectFrame::~KSceneObjectFrame()
{
}

void KSceneObjectFrame::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneObjectFrame)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KSceneObjectFrame, CFormView)
	//{{AFX_MSG_MAP(KSceneObjectFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectFrame diagnostics

#ifdef _DEBUG
void KSceneObjectFrame::AssertValid() const
{
	CFormView::AssertValid();
}

void KSceneObjectFrame::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectFrame message handlers
