// KGSceneEditorPvsPane.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGSceneEditorPvsPane.h"
#include "KSceneHelperFuncs.h"
#include "IEEditor.h"


IMPLEMENT_DYNAMIC(KGSceneEditorPvsPane, CDialog)

KGSceneEditorPvsPane::KGSceneEditorPvsPane(CWnd* pParent /*=NULL*/)
	: CDialog(KGSceneEditorPvsPane::IDD, pParent)
    , m_pEditor(NULL)
    , m_pPvs(NULL)
    , m_bEditPortal(FALSE)
    //, m_pSelectFilterSave(NULL)
{

}

KGSceneEditorPvsPane::~KGSceneEditorPvsPane()
{
}

void KGSceneEditorPvsPane::UpdateUI()
{
    if (!m_pEditor)
        return;

    ULONG nSelNum = 0;
    //m_pEditor->GetSelectedEntityCount(&nSelNum);
	nSelNum = KSF::GetSelectionCount(m_pEditor);

    if (nSelNum != 1)
        return;

    IEKG3DSceneEntity* pEntity = NULL;
    //m_pEditor->GetSelectedEntity(0, &pEntity);
	pEntity = KSF::GetFirstSelectedEntity(m_pEditor);

    if (!pEntity)
        return;
    
    SCENEENTITYTYPE dwType;
    pEntity->GetType(&dwType);

    IEKG3DRepresentObject* pObj = NULL;
    
    if (dwType != SCENEENTITY_OBJECT)
        return;

    pEntity->GetPointer((void**)&pObj);

    IEKG3DRepresentPVS* pPvs = NULL;

    //if (pObj->GetObjType() != REPRESENT_TYPE_PVS)
	if(pObj->GetType() != REPRESENTOBJECT_PVS)
        return;

    m_pPvs = dynamic_cast<IEKG3DRepresentPVS*>(pObj);

    ASSERT(m_pPvs);

    UpdataPortalList();

    CString strTitle;
    TCHAR strName[128];
    pObj->GetName(strName, sizeof(strName));
    strTitle.Format(TEXT("µ±Ç° Pvs Object : %s"), strName);
    SetDlgItemText(IDC_STATIC_PVS_NAME, strTitle);

}

void KGSceneEditorPvsPane::UpdataPortalList()
{
    m_PortalListCtrl.ResetContent();

    for (int i = 0; i < m_pPvs->GetPortalCount(); ++i)
    {
        IKG3DPvsPortal* pPortal = m_pPvs->GetPortal(static_cast<DWORD>(i));
        m_PortalListCtrl.AddString(pPortal->GetName());
    }
}

void KGSceneEditorPvsPane::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_CHECK_EDIT_PORTAL, m_bEditPortal);
    DDX_Control(pDX, IDC_LIST_PORTAL, m_PortalListCtrl);
}


BEGIN_MESSAGE_MAP(KGSceneEditorPvsPane, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_CONVER, &KGSceneEditorPvsPane::OnBnClickedButtonConver)
    ON_BN_CLICKED(IDC_BUTTON_ADD_PORTAL, &KGSceneEditorPvsPane::OnBnClickedButtonAddPortal)
    //ON_BN_CLICKED(IDC_CHECK_EDIT_PORTAL, &KGSceneEditorPvsPane::OnBnClickedCheckEditPortal)
    ON_LBN_SELCHANGE(IDC_LIST_PORTAL, &KGSceneEditorPvsPane::OnLbnSelchangeListPortal)
    ON_BN_CLICKED(IDC_BUTTON_DEL_PORTAL, &KGSceneEditorPvsPane::OnBnClickedButtonDelPortal)
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()


void KGSceneEditorPvsPane::OnBnClickedButtonConver()
{
    if  (!m_pEditor)
        return;

    if (FAILED(m_pEditor->ConvertCurrentSelectedObjectToPVS(&m_pPvs)))
    {
        MessageBox(TEXT("Convert Faild!"));
        return;
    }

}

void KGSceneEditorPvsPane::OnBnClickedButtonAddPortal()
{
    if (!m_pEditor || !m_pPvs)
        return;

    D3DXVECTOR3 vPos = D3DXVECTOR3(0.f, 500.f,  0.f);
    D3DXVECTOR3 vDir = D3DXVECTOR3(0.f, 0.f, -1.f);

    if (FAILED(m_pEditor->AddPvsPortal(m_pPvs, vPos, vDir ,1000.f, 1000.f)))
    {
        MessageBox(TEXT("Add Portal Faild!"));
        return;
    }

    UpdataPortalList();
}


//void KGSceneEditorPvsPane::OnBnClickedCheckEditPortal()
//{
//    UpdateData();
//
//    if (m_bEditPortal)
//    {
//        m_pSelectFilterSave = m_pEditor->GetSelFilter();
//
//        m_PvsSelectFilter.Attach(m_pPvs);
//        m_pEditor->SetSelFilter(&m_PvsSelectFilter);
//    }
//    else
//    {
//        m_pEditor->SetSelFilter(m_pSelectFilterSave);
//    }
//}

void KGSceneEditorPvsPane::OnLbnSelchangeListPortal()
{
    if (!m_pEditor || !m_pPvs || !m_bEditPortal)
        return;

    int nSel = m_PortalListCtrl.GetCurSel();

    if (nSel == LB_ERR)
        return;

    IKG3DPvsPortal* pSelPoratl = m_pPvs->GetPortal(static_cast<DWORD>(nSel));
    
    if (!pSelPoratl)
        return;

    m_pEditor->ClearSelectedEntityList();
    m_pEditor->SetSelected(SCENEENTITY_PVS_PORTAL, pSelPoratl, TRUE);
    SetDlgItemText(IDC_EDIT_NAME, pSelPoratl->GetName());
}

void KGSceneEditorPvsPane::OnBnClickedButtonDelPortal()
{
    if (!m_pEditor || !m_pPvs)
        return;

    int nSel = m_PortalListCtrl.GetCurSel();
    
    if (nSel == LB_ERR)
        return;

    m_pEditor->DeleteSelectedEntity();

    m_pPvs->DeletePortal(static_cast<DWORD>(nSel));
    UpdataPortalList();
    SetDlgItemText(IDC_EDIT_NAME, TEXT(""));
}

void KGSceneEditorPvsPane::OnOK()
{
}

BOOL KGSceneEditorPvsPane::PreTranslateMessage(MSG* pMsg)
{
    BOOL ret = CDialog::PreTranslateMessage(pMsg);

    if (pMsg->message != WM_KEYDOWN || pMsg->hwnd != GetDlgItem(IDC_EDIT_NAME)->GetSafeHwnd())
        return ret;

    if (pMsg->wParam != VK_RETURN)
        return ret;

    if (!m_pEditor || !m_pPvs || !m_bEditPortal)
        return ret;

    int nSel = m_PortalListCtrl.GetCurSel();

    if (nSel == LB_ERR)
        return ret;

    IKG3DPvsPortal* pSelPoratl = m_pPvs->GetPortal(static_cast<DWORD>(nSel));

    if (!pSelPoratl)
        return ret;

    UpdateData();
    TCHAR szPortalName[128];
    GetDlgItemText(IDC_EDIT_NAME, szPortalName, sizeof(szPortalName));
    pSelPoratl->SetName(szPortalName);
    UpdataPortalList();

    return ret;
}
