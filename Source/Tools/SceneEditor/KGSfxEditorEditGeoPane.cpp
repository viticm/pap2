// KGSfxEditorEditGeoPane.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGSfxEditorEditGeoPane.h"
#include ".\kscenesfxeditorframe.h"


// KGSfxEditorEditGeoPane dialog

IMPLEMENT_DYNAMIC(KGSfxEditorEditGeoPane, KGLilisSlipPanel<CPropertyPage>)

KGSfxEditorEditGeoPane::KGSfxEditorEditGeoPane()
	: KGLilisSlipPanel<CPropertyPage>(KGSfxEditorEditGeoPane::IDD)
{

}

KGSfxEditorEditGeoPane::~KGSfxEditorEditGeoPane()
{
}

void KGSfxEditorEditGeoPane::DoDataExchange(CDataExchange* pDX)
{
	KGLilisSlipPanel<CPropertyPage>::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KGSfxEditorEditGeoPane, KGLilisSlipPanel<CPropertyPage>)
    ON_COMMAND_RANGE(IDC_GEO_CHECK1, IDC_GEO_CHECK1 + 10, &KGSfxEditorEditGeoPane::OnGeoCommand)
END_MESSAGE_MAP()


void KGSfxEditorEditGeoPane::OnGeoCommand(UINT uId)
{
    GET_SFX_EDITOR();

    CButton* pSelCheck = (CButton*)GetDlgItem(uId);
    if (!pSelCheck->GetCheck())
    {
        pScene->SetEditState(SCENESTATE_SELECT);
        return;
    }

    for (INT i = (INT)IDC_GEO_CHECK1; i <= (INT)IDC_GEO_CHECK4; i++)
    {
        CButton* pCheck = (CButton*)GetDlgItem(i);
        if (pCheck != pSelCheck)
            pCheck->SetCheck(FALSE);
    }

    pScene->SetEditState(SCENESTATE_SFX_GEO);

    switch (uId)
    {
    case IDC_GEO_CHECK1 :
        pScene->SetGeoType(SFX_GEO_TYPE_LAUNCHER, SFX_GEO__LAUNCHER_SUB_TYPE_RECT);
        break;
    case IDC_GEO_CHECK2 :
        pScene->SetGeoType(SFX_GEO_TYPE_LAUNCHER, SFX_GEO__LAUNCHER_SUB_TYPE_SHER);
        break;
    case IDC_GEO_CHECK3 :
        pScene->SetGeoType(SFX_GEO_TYPE_LAUNCHER, SFX_GEO__LAUNCHER_SUB_TYPE_CIRC);
        break;
    case IDC_GEO_CHECK4 :
        pScene->SetGeoType(SFX_GEO_TYPE_LAUNCHER, SFX_GEO__LAUNCHER_SUB_TYPE_CYLR);
        break;
    default :
        ASSERT(false);
        break;
    }

    pScene->SetGeoStep(SFX_GEO_STEP_P);

}

int KGSfxEditorEditGeoPane::OnSetActive()
{
    return UpdateUI();
}

int KGSfxEditorEditGeoPane::UpdateUI()
{
    GET_SFX_EDITOR_RET(false);

    if (pScene->GetEditState() != SCENESTATE_SFX_GEO)
        for (INT i = (INT)IDC_GEO_CHECK1; i <= (INT)IDC_GEO_CHECK4; i++)
            ((CButton*)GetDlgItem(i))->SetCheck(FALSE);

    return true;
}

BOOL KGSfxEditorEditGeoPane::OnInitDialog()
{
    int nResult  = KGLilisSlipPanel<CPropertyPage>::OnInitDialog();

    return nResult;
}

// KGSfxEditorEditGeoPane message handlers
