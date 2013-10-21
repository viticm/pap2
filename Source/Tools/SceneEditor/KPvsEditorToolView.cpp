// KPvsEditorToolView.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KPvsEditorToolView.h"
#include "KPvsEditorToolSheet.h"
#include "KPvsEditorDoc.h"
#include "IEKG3DSceneSFXEditor.h"



// KPvsEditorToolView

IMPLEMENT_DYNCREATE(KPvsEditorToolView, CFormView)

KPvsEditorToolView::KPvsEditorToolView()
	: CFormView(KPvsEditorToolView::IDD),
      m_pSheet(NULL),
      m_pPvsScene(NULL)
{

}

KPvsEditorToolView::~KPvsEditorToolView()
{
}

void KPvsEditorToolView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(KPvsEditorToolView, CFormView)
    ON_WM_SIZE()
END_MESSAGE_MAP()


// KPvsEditorToolView diagnostics

#ifdef _DEBUG
void KPvsEditorToolView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void KPvsEditorToolView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

IEKG3DScenePvsEditor* KPvsEditorToolView::GetPvsScene()
{
    KPvsEditorDoc* doc = static_cast<KPvsEditorDoc*>(GetDocument());

    if (!doc)
        return NULL;

    return doc->GetPvsScene();
}

void KPvsEditorToolView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();

    ASSERT(!m_pSheet);

    m_pSheet = new KPvsEditorToolSheet(this);
    
    RECT rect;
    GetClientRect(&rect);

    m_pSheet->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rect, this, 123);
    m_pSheet->SetWindowPos(NULL, 0, 0, 800, 480, SWP_NOACTIVATE);

    KPvsEditorDoc* doc = static_cast<KPvsEditorDoc*>(GetDocument());

    if (doc)
        m_pPvsScene = doc->GetPvsScene();

}

void KPvsEditorToolView::OnSize(UINT nType, int cx, int cy)
{
    RECT rect;
    
    GetClientRect(&rect);
    rect.bottom += 10;
    rect.left += 2;

    if (m_pSheet && ::IsWindow(m_pSheet->m_hWnd))
        m_pSheet->MoveWindow(&rect);

    CFormView::OnSize(nType, cx, cy);
}