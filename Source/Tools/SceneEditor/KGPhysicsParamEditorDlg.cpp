// KGPhysicsParamEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGPhysicsParamEditorDlg.h"
#include "IEEditor.h"

// KGPhysicsParamEditorDlg dialog

IMPLEMENT_DYNAMIC(KGPhysicsParamEditorDlg, CDialog)

KGPhysicsParamEditorDlg::KGPhysicsParamEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(KGPhysicsParamEditorDlg::IDD, pParent)
{
}

KGPhysicsParamEditorDlg::~KGPhysicsParamEditorDlg()
{
}

void KGPhysicsParamEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE, m_Tree);

    DDX_Text(pDX, IDC_EDIT_RADIUS, m_ParamCur.fRadius);
    DDV_MinMaxFloat(pDX, m_ParamCur.fRadius, 0.0001f, 10000.0f);
    DDX_Text(pDX, IDC_EDIT_DENSITY, m_ParamCur.fDensity);
    DDX_Text(pDX, IDC_EDIT_ITERATION_COUNT, m_ParamCur.nSolverIterationCount);
    DDX_Text(pDX, IDC_EDIT_lINEARDAMPING, m_ParamCur.fLinearDamping);
    DDX_Text(pDX, IDC_EDIT_ANGULARDAMPING, m_ParamCur.fAngularDamping);
    DDX_Text(pDX, IDC_SphericalJointTwistLimitLow, m_ParamCur.SphericalJoint.TwistLimitLow.fValue);
    DDX_Text(pDX, IDC_SphericalJointTwistLimitLowHardness, m_ParamCur.SphericalJoint.TwistLimitLow.fHardness);
    DDX_Text(pDX, IDC_SphericalJointTwistLimitLowRestitution, m_ParamCur.SphericalJoint.TwistLimitLow.fRestitution);
    DDX_Text(pDX, IDC_SphericalJointTwistLimitHigh, m_ParamCur.SphericalJoint.TwistLimitHigh.fValue);
    DDX_Text(pDX, IDC_SphericalJointTwistLimitHighHardness, m_ParamCur.SphericalJoint.TwistLimitHigh.fHardness);
    DDX_Text(pDX, IDC_SphericalJointTwistLimitHighRestitution, m_ParamCur.SphericalJoint.TwistLimitHigh.fRestitution);
    DDX_Text(pDX, IDC_SphericalJointSwingLimit, m_ParamCur.SphericalJoint.SwingLimit.fValue);
    DDX_Text(pDX, IDC_SphericalJointSwingLimitHardness, m_ParamCur.SphericalJoint.SwingLimit.fHardness);
    DDX_Text(pDX, IDC_SphericalJointSwingLimitRestitution, m_ParamCur.SphericalJoint.SwingLimit.fRestitution);
    DDX_Text(pDX, IDC_SphericalJoinTwistSpringSpring, m_ParamCur.SphericalJoint.TwistSpring.fSpring);
    DDX_Text(pDX, IDC_SphericalJoinTwistSpringDamper, m_ParamCur.SphericalJoint.TwistSpring.fDamper);
    DDX_Text(pDX, IDC_SphericalJoinSwingSpringSpring, m_ParamCur.SphericalJoint.SwingSpring.fSpring);
    DDX_Text(pDX, IDC_SphericalJoinSwingSpringDamper, m_ParamCur.SphericalJoint.SwingSpring.fDamper);
    DDX_Text(pDX, IDC_SphericalJoinJointSpringSpring, m_ParamCur.SphericalJoint.JointSpring.fSpring);
    DDX_Text(pDX, IDC_SphericalJoinJointSpringDamper, m_ParamCur.SphericalJoint.JointSpring.fDamper);
    DDX_Check(pDX, IDC_SphericalJointEnableTwistLimit, m_ParamCur.SphericalJoint.bEnableTwistLimit);
    DDX_Check(pDX, IDC_SphericalJointEnableSwingLimit, m_ParamCur.SphericalJoint.bEnableSwingLimit);
    DDX_Check(pDX, IDC_SphericalJointEnableTwistSpring, m_ParamCur.SphericalJoint.bEnableTwistSpring);
    DDX_Check(pDX, IDC_SphericalJointEnableSwingSpring, m_ParamCur.SphericalJoint.bEnableSwingSpring);
    DDX_Check(pDX, IDC_SphericalJointEnableJointSpring, m_ParamCur.SphericalJoint.bEnableJointSpring);
}


BEGIN_MESSAGE_MAP(KGPhysicsParamEditorDlg, CDialog)
    ON_BN_CLICKED(IDC_APPLY, &KGPhysicsParamEditorDlg::OnBnClickedApply)
    ON_BN_CLICKED(IDOK, &KGPhysicsParamEditorDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &KGPhysicsParamEditorDlg::OnBnClickedCancel)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &KGPhysicsParamEditorDlg::OnTvnSelchangedTree)
END_MESSAGE_MAP()

BOOL KGPhysicsParamEditorDlg::OnInitDialog()
{
    int nResult  = false;
    int nRetCode = false;
    IIniFile* pIniFile = NULL;
    KG3DPhysiscParam Param;
    TCHAR szSection[128];
    HTREEITEM hItem = NULL;

    nRetCode = CDialog::OnInitDialog();
    KGLOG_PROCESS_ERROR(nRetCode);

    pIniFile = g_OpenIniFile(PHYSISC_PRARM_FILE_NAME);
    KGLOG_PROCESS_ERROR(pIniFile);

    nRetCode = m_Tree.DeleteAllItems();
    KGLOG_PROCESS_ERROR(nRetCode);

    hItem = m_Tree.InsertItem("ÈáÌåÀàÐÍ");
    KGLOG_PROCESS_ERROR(hItem);
    
    szSection[0] = '\0';
    while (pIniFile->GetNextSection(szSection, szSection))
    {
        TCHAR Temp[128];
        TCHAR Name[128];
        int id = atoi(szSection);
        if ((id <= 0) && (strcmp(szSection, "00") != 0))
            continue;

        pIniFile->GetString(szSection, "name", "", Temp, sizeof(Temp));
        sprintf(Name, "%s - %s", szSection, Temp);

        nRetCode = pIniFile->GetFloat(szSection, "fRadius", 0.0f, &Param.fRadius);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "fDensity", 0.0f, &Param.fDensity);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetInteger(szSection, "nSolverIterationCount", 0, &Param.nSolverIterationCount);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "fLinearDamping", 0.0f, &Param.fLinearDamping);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "fAngularDamping", 0.0f, &Param.fAngularDamping);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = pIniFile->GetBool(szSection, "SphericalJoint.bEnableTwistLimit", &Param.SphericalJoint.bEnableTwistLimit);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.TwistLimitLow.fValue", 0.0f, &Param.SphericalJoint.TwistLimitLow.fValue);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.TwistLimitLow.fHardness", 0.0f, &Param.SphericalJoint.TwistLimitLow.fHardness);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.TwistLimitLow.fRestitution", 0.0f, &Param.SphericalJoint.TwistLimitLow.fRestitution);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.TwistLimitHigh.fValue", 0.0f, &Param.SphericalJoint.TwistLimitHigh.fValue);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.TwistLimitHigh.fHardness", 0.0f, &Param.SphericalJoint.TwistLimitHigh.fHardness);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.TwistLimitHigh.fRestitution", 0.0f, &Param.SphericalJoint.TwistLimitHigh.fRestitution);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = pIniFile->GetBool(szSection, "SphericalJoint.bEnableSwingLimit", &Param.SphericalJoint.bEnableSwingLimit);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.SwingLimit.fValue", 0.0f, &Param.SphericalJoint.SwingLimit.fValue);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.SwingLimit.fHardness", 0.0f, &Param.SphericalJoint.SwingLimit.fHardness);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.SwingLimit.fRestitution", 0.0f, &Param.SphericalJoint.SwingLimit.fRestitution);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = pIniFile->GetBool(szSection, "SphericalJoint.bEnableTwistSpring", &Param.SphericalJoint.bEnableTwistSpring);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.TwistSpring.fSpring", 0.0f, &Param.SphericalJoint.TwistSpring.fSpring);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.TwistSpring.fDamper", 0.0f, &Param.SphericalJoint.TwistSpring.fDamper);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = pIniFile->GetBool(szSection, "SphericalJoint.bEnableSwingSpring", &Param.SphericalJoint.bEnableSwingSpring);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.SwingSpring.fSpring", 0.0f, &Param.SphericalJoint.SwingSpring.fSpring);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.SwingSpring.fDamper", 0.0f, &Param.SphericalJoint.SwingSpring.fDamper);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = pIniFile->GetBool(szSection, "SphericalJoint.bEnableJointSpring", &Param.SphericalJoint.bEnableJointSpring);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.JointSpring.fSpring", 0.0f, &Param.SphericalJoint.JointSpring.fSpring);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.JointSpring.fDamper", 0.0f, &Param.SphericalJoint.JointSpring.fDamper);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = m_Tree.SetItemData(m_Tree.InsertItem(Name, hItem), (DWORD)id);
        KGLOG_PROCESS_ERROR(nRetCode);
            
        m_mapParams[id] = Param;
    }

    nRetCode = m_Tree.SetItemData(hItem, (DWORD)0);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_Tree.Expand(hItem, TVE_EXPAND);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_Tree.SelectItem(hItem);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    KG_COM_RELEASE(pIniFile);
    return nResult;
}

void KGPhysicsParamEditorDlg::_UpdateUi()
{
    int id = (int)m_Tree.GetItemData(m_Tree.GetSelectedItem());
    
    BOOL bEnable = false;
    if (id > 0)
        bEnable = true;

    CWnd* pWnd = GetWindow(GW_CHILD);

    while (pWnd)
    {
        TCHAR szName[128];
        GetClassName(pWnd->m_hWnd, szName, sizeof(szName));
        LONG s = GetWindowLong(pWnd->m_hWnd, GWL_STYLE);

        if (!stricmp(szName, "Edit") || (!stricmp(szName, "Button") && (s & BS_CHECKBOX) == BS_CHECKBOX))
            pWnd->EnableWindow(bEnable);
        pWnd = pWnd->GetWindow(GW_HWNDNEXT);
    }

}

void KGPhysicsParamEditorDlg::_UpdateToMap(HTREEITEM hItem)
{
    if (!hItem)
        return;

    UpdateData();

    int id = (int)m_Tree.GetItemData(hItem);

    if (id && m_mapParams.find(id) == m_mapParams.end())
        return;

    m_mapParams[id] = m_ParamCur;

}

void KGPhysicsParamEditorDlg::_UpdateToUi(HTREEITEM hItem)
{
    if (!hItem)
        return;

    int id = (int)m_Tree.GetItemData(hItem);

    if (m_mapParams.find(id) == m_mapParams.end())
        return;
    
    m_ParamCur = m_mapParams[id];

    UpdateData(false);
}

void KGPhysicsParamEditorDlg::OnBnClickedApply()
{
    HRESULT hrResult = E_FAIL;
    IIniFile* pIniFile = NULL;
    map<int, KG3DPhysiscParam>::iterator it;
    HTREEITEM hItem = m_Tree.GetSelectedItem();

    _UpdateToMap(hItem);

    pIniFile = g_OpenIniFile(PHYSISC_PRARM_FILE_NAME);
    KGLOG_PROCESS_ERROR(pIniFile);

    for (it = m_mapParams.begin(); it != m_mapParams.end(); ++it)
    {
        TCHAR szSection[128];
        int id = it->first;
        KG3DPhysiscParam Param = it->second;

        if (id == 0)
            continue;

        sprintf(szSection, "%02d", id);

        pIniFile->WriteFloat(szSection, "fRadius", Param.fRadius);
        pIniFile->WriteFloat(szSection, "fDensity", Param.fDensity);
        pIniFile->WriteInteger(szSection, "nSolverIterationCount", Param.nSolverIterationCount);
        pIniFile->WriteFloat(szSection, "fLinearDamping", Param.fLinearDamping);
        pIniFile->WriteFloat(szSection, "fAngularDamping", Param.fAngularDamping);

        pIniFile->WriteInteger(szSection, "SphericalJoint.bEnableTwistLimit", Param.SphericalJoint.bEnableTwistLimit);
        pIniFile->WriteFloat(szSection, "SphericalJoint.TwistLimitLow.fValue", Param.SphericalJoint.TwistLimitLow.fValue);
        pIniFile->WriteFloat(szSection, "SphericalJoint.TwistLimitLow.fHardness", Param.SphericalJoint.TwistLimitLow.fHardness);
        pIniFile->WriteFloat(szSection, "SphericalJoint.TwistLimitLow.fRestitution",  Param.SphericalJoint.TwistLimitLow.fRestitution);
        pIniFile->WriteFloat(szSection, "SphericalJoint.TwistLimitHigh.fValue", Param.SphericalJoint.TwistLimitHigh.fValue);
        pIniFile->WriteFloat(szSection, "SphericalJoint.TwistLimitHigh.fHardness", Param.SphericalJoint.TwistLimitHigh.fHardness);
        pIniFile->WriteFloat(szSection, "SphericalJoint.TwistLimitHigh.fRestitution", Param.SphericalJoint.TwistLimitHigh.fRestitution);

        pIniFile->WriteInteger(szSection, "SphericalJoint.bEnableSwingLimit", Param.SphericalJoint.bEnableSwingLimit);
        pIniFile->WriteFloat(szSection, "SphericalJoint.SwingLimit.fValue", Param.SphericalJoint.SwingLimit.fValue);
        pIniFile->WriteFloat(szSection, "SphericalJoint.SwingLimit.fHardness",  Param.SphericalJoint.SwingLimit.fHardness);
        pIniFile->WriteFloat(szSection, "SphericalJoint.SwingLimit.fRestitution", Param.SphericalJoint.SwingLimit.fRestitution);

        pIniFile->WriteInteger(szSection, "SphericalJoint.bEnableTwistSpring", Param.SphericalJoint.bEnableTwistSpring);
        pIniFile->WriteFloat(szSection, "SphericalJoint.TwistSpring.fSpring", Param.SphericalJoint.TwistSpring.fSpring);
        pIniFile->WriteFloat(szSection, "SphericalJoint.TwistSpring.fDamper", Param.SphericalJoint.TwistSpring.fDamper);

        pIniFile->WriteInteger(szSection, "SphericalJoint.bEnableSwingSpring", Param.SphericalJoint.bEnableSwingSpring);    
        pIniFile->WriteFloat(szSection, "SphericalJoint.SwingSpring.fSpring", Param.SphericalJoint.SwingSpring.fSpring);
        pIniFile->WriteFloat(szSection, "SphericalJoint.SwingSpring.fDamper", Param.SphericalJoint.SwingSpring.fDamper);

        pIniFile->WriteInteger(szSection, "SphericalJoint.bEnableJointSpring", Param.SphericalJoint.bEnableJointSpring);
        pIniFile->WriteFloat(szSection, "SphericalJoint.JointSpring.fSpring", Param.SphericalJoint.JointSpring.fSpring);
        pIniFile->WriteFloat(szSection, "SphericalJoint.JointSpring.fDamper", Param.SphericalJoint.JointSpring.fDamper);
    }

    pIniFile->Save(PHYSISC_PRARM_FILE_NAME);

    if (m_lpSceneEditor)
        m_lpSceneEditor->RefreshPhysisc();

    hrResult = S_OK;
Exit0:
    KG_COM_RELEASE(pIniFile);
}

void KGPhysicsParamEditorDlg::OnBnClickedOk()
{
    OnBnClickedApply();
    OnOK();
    delete this;
}


void KGPhysicsParamEditorDlg::OnBnClickedCancel()
{
    OnCancel();
    delete this;
}

void KGPhysicsParamEditorDlg::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    *pResult = 0;

    if (pNMTreeView->itemOld.hItem)
        _UpdateToMap(pNMTreeView->itemOld.hItem);

    if (pNMTreeView->itemNew.hItem)
        _UpdateToUi(pNMTreeView->itemNew.hItem);

    _UpdateUi();

}
