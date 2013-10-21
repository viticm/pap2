// KGSfxEditModifyPane.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGSfxEditModifyPane.h"
#include "KSceneSFXEditorFrame.h"

// KGSfxEditModifyPane dialog

IMPLEMENT_DYNAMIC(KGSfxEditModifyPane, KGLilisSlipPanel<CPropertyPage>)

KGSfxEditModifyPane::KGSfxEditModifyPane()
	: KGLilisSlipPanel<CPropertyPage>(KGSfxEditModifyPane::IDD)
{

}

KGSfxEditModifyPane::~KGSfxEditModifyPane()
{
}

void KGSfxEditModifyPane::DoDataExchange(CDataExchange* pDX)
{
	KGLilisSlipPanel<CPropertyPage>::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE, m_Tree);
}


BEGIN_MESSAGE_MAP(KGSfxEditModifyPane, KGLilisSlipPanel<CPropertyPage>)
END_MESSAGE_MAP()

int KGSfxEditModifyPane::OnSetActive()
{
    GET_SFX_EDITOR_RET(false);
    IEKG3DSFX* pSfx = NULL;
    pScene->GetSFX(&pSfx);
    ASSERT(pSfx);

    m_Tree.BindSfx(pSfx);

    return TRUE;
}


// KGSfxEditModifyPane message handlers
