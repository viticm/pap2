// KPvsEditorFrame.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KPvsEditorFrame.h"
#include "KPvsEditorToolView.h"
#include "KPvsEditorView.h"
#include "KPvsEditorDoc.h"
#include "KPvsEditorAboutBox.h"


IMPLEMENT_DYNCREATE(KGPvsViewSplitterWnd, CSplitterWnd)

KGPvsViewSplitterWnd::KGPvsViewSplitterWnd()
    : m_bTracked(false)
{
    //m_cxSplitter    = 1;
    //m_cySplitter    = 1;
    //m_cxBorderShare = 1;
    //m_cyBorderShare = 1;
    //m_cxSplitterGap = 1;
    //m_cySplitterGap = 1;
    //m_cxBorder      = 2;
    //m_cyBorder      = 2;
    //m_bHasHScroll   = FALSE;
    //m_bHasVScroll   = FALSE;
}

KGPvsViewSplitterWnd::~KGPvsViewSplitterWnd()
{
}


BEGIN_MESSAGE_MAP(KGPvsViewSplitterWnd, CSplitterWnd)
    ON_WM_SIZE()
END_MESSAGE_MAP()

void KGPvsViewSplitterWnd::OnSize(UINT nType, int cx, int cy)
{
    CSplitterWnd::OnSize(nType, cx, cy);

    if (GetDlgItem(IdFromRowCol(0, 0)) && GetDlgItem(IdFromRowCol(0, 1)))
    {
        CRect Rect;
        GetClientRect(&Rect);
        SetColumnInfo(0, max(1, Rect.Width() - 270), 30);
        SetColumnInfo(1, 270, 30);
        RecalcLayout();
    }

}

IMPLEMENT_DYNCREATE(KPvsEditorFrame, MFCFramework::CChildFrame)

KPvsEditorFrame::KPvsEditorFrame()
{

}

KPvsEditorFrame::~KPvsEditorFrame()
{

}


BEGIN_MESSAGE_MAP(KPvsEditorFrame, MFCFramework::CChildFrame)
    ON_COMMAND(ID_DELOBJ, &KPvsEditorFrame::OnDeleteSelectedObject)
    ON_COMMAND(ID_ACTION_ADD, &KPvsEditorFrame::OnZoomInObject)
    ON_COMMAND(ID_ACTION_DEC, &KPvsEditorFrame::OnZoomOutObject)
    ON_COMMAND(ID_APP_ABOUT, &KPvsEditorFrame::OnAbout)
END_MESSAGE_MAP()

void KPvsEditorFrame::OnAbout()
{
    KPvsEditorAboutBox dlg;
    dlg.DoModal();
}

void KPvsEditorFrame::OnDeleteSelectedObject()
{
    if (!GetActiveView())
        return;

    KPvsEditorDoc* doc = (KPvsEditorDoc*)GetActiveView()->GetDocument();
    if (doc->GetPvsScene())
        doc->GetPvsScene()->DeleteSelectedObject();
}

void KPvsEditorFrame::OnZoomInObject()
{
    if (!GetActiveView())
        return;

    KPvsEditorDoc* doc = (KPvsEditorDoc*)GetActiveView()->GetDocument();
    if (doc->GetPvsScene())
        doc->GetPvsScene()->ScalHandObj(-1.f);

}

void KPvsEditorFrame::OnZoomOutObject()
{
    if (!GetActiveView())
        return;

    KPvsEditorDoc* doc = (KPvsEditorDoc*)GetActiveView()->GetDocument();
    if (doc->GetPvsScene())
        doc->GetPvsScene()->ScalHandObj(1.f);

}



BOOL KPvsEditorFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
    m_wndSplitter.CreateStatic(this, 1, 2);
    m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(KPvsEditorView), CSize(800, 10), pContext);
    m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(KPvsEditorToolView),CSize(100, 10), pContext);

    CRect Rect;
    GetClientRect(&Rect);

    m_wndSplitter.SetColumnInfo(0, Rect.Width() - 250, 30);
    m_wndSplitter.SetColumnInfo(1, 250, 30);
    m_wndSplitter.RecalcLayout();

    return TRUE;

}

// KPvsEditorFrame message handlers
