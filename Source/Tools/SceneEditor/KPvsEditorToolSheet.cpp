// KPvsEditorToolSheet.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KPvsEditorToolSheet.h"
#include "IEKG3DSceneSFXEditor.h"
#include "KPvsEditorToolView.h"



// KPvsEditorToolSheet

IMPLEMENT_DYNAMIC(KPvsEditorToolSheet, KGPropertySheet)

KPvsEditorToolSheet::KPvsEditorToolSheet(CWnd* pParentWnd)
{

}

KPvsEditorToolSheet::~KPvsEditorToolSheet()
{
}

BEGIN_MESSAGE_MAP(KPvsEditorToolSheet, KGPropertySheet)
    ON_WM_CREATE()
    ON_WM_SIZE()
END_MESSAGE_MAP()

IEKG3DScenePvsEditor* KPvsEditorToolSheet::GetPvsScene()
{
    if (!GetParent())
        return NULL;

    KPvsEditorToolView* pToolView = static_cast<KPvsEditorToolView*>(GetParent());

    if (!pToolView)
        return NULL;

    return pToolView->GetPvsScene();

}

int KPvsEditorToolSheet::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (KGPropertySheet::OnCreate(lpCreateStruct) == -1)
        return -1;

    AddPage(&m_PlacePanel, m_PlacePanel.IDD);
    AddPage(&m_AttriPanel, m_AttriPanel.IDD);


    return 0;
}

void KPvsEditorToolSheet::OnSize(UINT nType, int cx, int cy)
{
    KGPropertySheet::OnSize(nType, cx, cy);
}
// KPvsEditorToolSheet message handlers
