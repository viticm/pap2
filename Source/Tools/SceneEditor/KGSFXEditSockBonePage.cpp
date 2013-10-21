////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGSFXEditSockBonePage.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-9-29 14:35:21
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KGSFXEditSockBonePage.h"
#include "SceneEditor.h"
#include "KGSFXModelViewPage.h"
#include "KSceneSFXEditorFrame.h"
#include "IEKG3DModelST.h"
#include "IEKG3DSFX.h"
#include "KGSFXBingPage.h"
#include "IEKG3DSceneEntity.h"
//#include "KG3DSceneEditorBase.h"
#include "IEKG3DMesh.h"
#include "IEKG3DMeshBone.h"
#include "KSceneHelperFuncs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KGSFXEditSockBonePage, CPropertyPage)
KGSFXEditSockBonePage::KGSFXEditSockBonePage()
    : CPropertyPage(KGSFXEditSockBonePage::IDD)
{
    bModifyInit = false;
}

KGSFXEditSockBonePage::~KGSFXEditSockBonePage()
{
}

void KGSFXEditSockBonePage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SOCK,  m_sock);
    DDX_Control(pDX, IDC_BONE,  m_bone);
    DDX_Control(pDX, IDC_MODEL, m_comb);
}


BEGIN_MESSAGE_MAP(KGSFXEditSockBonePage, CPropertyPage)
    ON_CBN_SELCHANGE(IDC_MODEL, &KGSFXEditSockBonePage::OnCbnSelchangeModel)
    ON_NOTIFY(TVN_SELCHANGED, IDC_BONE, &KGSFXEditSockBonePage::OnTvnSelchangedBone)
    ON_NOTIFY(TVN_SELCHANGED, IDC_SOCK, &KGSFXEditSockBonePage::OnTvnSelchangedSock)
    ON_BN_CLICKED(IDC_LINK, &KGSFXEditSockBonePage::OnBnClickedLink)
    ON_NOTIFY(NM_DBLCLK, IDC_SOCK, &KGSFXEditSockBonePage::OnNMDblclkSock)
    ON_NOTIFY(NM_DBLCLK, IDC_BONE, &KGSFXEditSockBonePage::OnNMDblclkBone)
    ON_WM_SIZE()
    ON_CBN_SETFOCUS(IDC_MODEL, &KGSFXEditSockBonePage::OnCbnSetfocusModel)
END_MESSAGE_MAP()

BOOL KGSFXEditSockBonePage::OnInitDialog()
{
    int nResult = CPropertyPage::OnInitDialog();

    m_sock.SetTextColor(RGB(255, 255, 255));
    m_sock.SetLineColor(RGB(122, 122, 122));
    m_sock.SetBkColor(RGB(92,  92, 92));

    m_bone.SetTextColor(RGB(255, 255, 255));
    m_bone.SetLineColor(RGB(122, 122, 122));
    m_bone.SetBkColor(RGB(92,  92, 92));

    m_imageListTree.Create(16, 16, ILC_COLOR24, 5, 5);
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_FC));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_GLEAN));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_TALK));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_OFFER));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_POS));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_BN));

    m_sock.SetImageList(&m_imageListTree, TVSIL_NORMAL);
    m_bone.SetImageList(&m_imageListTree, TVSIL_NORMAL);

    return nResult;
}

BOOL KGSFXEditSockBonePage::OnSetActive()
{
    GET_SFX_EDITOR_RET(false);

    if (pScene->IsModifyMode())
    {
        if (!bModifyInit)
        {
            FillComb();
            m_comb.SetCurSel(0);
            OnCbnSelchangeModel();
            bModifyInit = true;
        }
        GetDlgItem(IDC_LINK)->EnableWindow(false);
    }
    else
    {
        FillSock();
        FillBone();
    }

    return CPropertyPage::OnSetActive();
}

BOOL KGSFXEditSockBonePage::OnKillActive()
{
    GET_SFX_EDITOR_RET(true);

    if (pScene->IsModifyMode())
    {
        
    }
    else
    {
        ((CButton*)GetDlgItem(IDC_LINK))->SetCheck(false);
        pScene->UpdateSceneMode(SFX_SCENE_MODE_EDIT);
    }
    return TRUE;
}

void KGSFXEditSockBonePage::FillSock()
{
    HRESULT hRetCode = E_FAIL;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
    int nResult  = false;
    int nRetCode = false;

    IEKG3DSceneEntity *piEntity = NULL;
    IEKG3DModelST* pSelMdlST = NULL;
    IEKG3DModel*   pSelModel = NULL;
    IKG3DModel*   pModel = NULL;
    int nCount = 0;
    IEKG3DMesh *pMesh = NULL;
    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;

    m_sock.DeleteAllItems();

    GET_SFX_EDITOR();
    pScene->GetSceneEditBase(&piSceneEditorBase);
	GETSELECT(piSceneEditorBase);


	IEKG3DSceneSelectionTool* pSelTool = NULL;
	pScene->GetSelectionToolInterface(&pSelTool);
	KG_PROCESS_ERROR(NULL != pSelTool);

	//KG_PROCESS_SUCCESS(!(pSelector->GetSelectedEntitySize()));//大小为0不是出错
	KG_PROCESS_SUCCESS(0 == pSelTool->GetSelectionCount());//大小为0不是出错

	//pSelector->GetSelectedEntity(0, &piEntity);

	IEKG3DSceneSelectionToolAccessProxy* pProxy = KSF::GetSelectionProxy(piSceneEditorBase);
	KG_PROCESS_ERROR(NULL != pProxy);
	piEntity = pProxy->Reset();
	KG_PROCESS_ERROR(NULL != piEntity);

	_ASSERTE(piEntity);
    hRetCode = piEntity->GetType(&EntityType);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(EntityType == SCENEENTITY_MODEL);

    hRetCode = piEntity->GetPointer((VOID **)&pSelModel);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(pSelModel);

    pSelModel->GetIEKG3DModelST(&pSelMdlST);
    if (pSelMdlST)
    {
        nCount = pSelModel->GetNumModel();

        for (int s = 0; s < nCount; s++)
        {
            IEKG3DModel* pieModel = NULL;
			pSelModel->GetModel(s, &pModel);
			if (!pModel)
				continue;

			pieModel = dynamic_cast<IEKG3DModel*>(pModel);
			if (!pieModel)
				continue;

            pieModel->GetIEKG3DMesh(&pMesh);
            
            if (!pMesh)
                continue;
            DWORD dwNumSocket = 0;
            hRetCode = pMesh->GetSocketCount(&dwNumSocket);
            KGLOG_COM_PROCESS_ERROR(hRetCode);
            for (DWORD i = 0; i < dwNumSocket; i++)
            {
                LPCTSTR pszSocketName = NULL;
                pMesh->GetSocketName(i, &pszSocketName);
                m_sock.InsertItem(pszSocketName, 5, 5);
            }

        }
    }
    else
    {
        pSelModel->GetIEKG3DMesh(&pMesh);
        KGLOG_PROCESS_SUCCESS(!pMesh);

        DWORD dwNumSocket = 0;
        hRetCode = pMesh->GetSocketCount(&dwNumSocket);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        for (DWORD i = 0; i < dwNumSocket; i++)
        {
            LPCTSTR pszSocketName = NULL;
            pMesh->GetSocketName(i, &pszSocketName);
            m_sock.InsertItem(pszSocketName, 5, 5);
        }
    }
Exit1:
    nResult = true;
Exit0:
    return;
}

void KGSFXEditSockBonePage::FillBone()
{
    HRESULT hRetCode = E_FAIL;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
    int nResult  = false;
    int nRetCode = false;

    IEKG3DSceneEntity *piEntity = NULL;
    IEKG3DModelST* pSelMdlST = NULL;
    IEKG3DModel*   pSelModel = NULL;
    IKG3DModel*   pModel = NULL;
    int nCount = 0;
    //IEKG3DMeshBase *pMeshBase = NULL;
    IEKG3DMesh *pMesh = NULL;
    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;

    m_bone.DeleteAllItems();

    GET_SFX_EDITOR();
    pScene->GetSceneEditBase(&piSceneEditorBase);

	GETSELECT(piSceneEditorBase);
	/*KG_PROCESS_ERROR(pSelector->GetSelectedEntitySize());
	pSelector->GetSelectedEntity(0, &piEntity);*/
	piEntity = KSF::GetFirstSelectedEntity(pScene);
	KG_PROCESS_ERROR(NULL != piEntity);

    hRetCode = piEntity->GetType(&EntityType);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(EntityType == SCENEENTITY_MODEL);

    hRetCode = piEntity->GetPointer((VOID **)&pSelModel);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(pSelModel);

    pSelModel->GetIEKG3DModelST(&pSelMdlST);

    if (pSelMdlST)
    {
        m_bone.InsertItem(TEXT("原点"), 5, 5);

        nCount = pSelModel->GetNumModel();

        for (int s = 0; s < nCount; s++)
        {
			IEKG3DModel* pieModel = NULL;
			pSelModel->GetModel(s, &pModel);
			pieModel = dynamic_cast<IEKG3DModel*>(pModel);

            if (!pieModel)
                continue;

            //KG3DMeshEx *pMesh     = (KG3DMeshEx*)(pModel->GetMesh());
            pieModel->GetIEKG3DMesh(&pMesh);
            
            if (!pMesh)
                continue;
            int		nNumBone = 0;
            pMesh->GetNumBones(&nNumBone);
            for (int i = 0; i < nNumBone; i++)
            {
                LPCTSTR pszName = NULL;
                pMesh->GetBoneInfoName(i, &pszName);
                m_bone.InsertItem(pszName, 5, 5);
            }
        }
    }
    else
    {
        pSelModel->GetIEKG3DMesh(&pMesh);
        KGLOG_PROCESS_SUCCESS(!pMesh);

        int nNumBone = 0;
        pMesh->GetNumBones(&nNumBone);
        m_bone.InsertItem(TEXT("原点"), 5, 5);
        for (int i = 0; i < nNumBone; i++)
        {
            LPCTSTR pszName = NULL;
            pMesh->GetBoneInfoName(i, &pszName);
            m_bone.InsertItem(pszName, 5, 5);
        }

    }
Exit1:
    nResult = true;
Exit0:
    return;
}

void KGSFXEditSockBonePage::FillComb()
{
    HRESULT hRetCode = E_FAIL;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
    int nResult  = false;
    int nRetCode = false;

    IEKG3DSceneEntity *piEntity = NULL;
    IEKG3DModel* pSelModel = NULL;
    IEKG3DModel *piCurBeBindModel = NULL;
    LPCTSTR pszName = NULL;
    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;

    m_comb.ResetContent();

    GET_SFX_EDITOR();

    hRetCode = pScene->GetCurBeBindModel(&piCurBeBindModel);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = pScene->GetSceneEditBase(&piSceneEditorBase);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

	GETSCENE(pScene);
    hRetCode = p3DScene->GetFirstRenderEntity(&piEntity);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    while (piEntity)
    {

        hRetCode = piEntity->GetType(&EntityType);
        KGLOG_COM_PROCESS_ERROR(hRetCode);
        if (EntityType == SCENEENTITY_MODEL)
        {
            IEKG3DModel* pModel = NULL;
            hRetCode = piEntity->GetPointer((VOID **)&pModel);
            KGLOG_COM_PROCESS_ERROR(hRetCode);

            hRetCode = pModel->GetName(&pszName);
            KGLOG_COM_PROCESS_ERROR(hRetCode);

            char* pName = strrchr(const_cast<char*>(pszName), '\\');

            if (pName && pModel != piCurBeBindModel)
                m_comb.AddString(++pName);
        }

        hRetCode = p3DScene->GetNextRenderEntity(&piEntity);
        KGLOG_COM_PROCESS_ERROR(hRetCode);
    }


    hRetCode = p3DScene->GetFirstRenderEntity(&piEntity);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    KG_PROCESS_ERROR(piEntity);

    hRetCode = piEntity->GetType(&EntityType);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(EntityType == SCENEENTITY_MODEL);

    hRetCode = piEntity->GetPointer((VOID **)&pSelModel);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    if (pSelModel)
    {
        hRetCode = pSelModel->GetName(&pszName);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        char* pName = strrchr(const_cast<char*>(pszName), '\\');
        if (pName)
            m_comb.SelectString(0, ++pName);
    }

    nResult = true;
Exit0:
    return;
}

void KGSFXEditSockBonePage::OnCbnSelchangeModel()
{
    HRESULT hRetCode = E_FAIL;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
    IEKG3DSceneEntity *piEntity = NULL;
    int nResult  = false;
    int nRetCode = false;
    IEKG3DModel* pModel = NULL;
    LPCTSTR pszName = NULL;
    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;

    GET_SFX_EDITOR();

    ///if (pScene->IsModifyMode())
   //     return;

    char szName[128];
    int nSelIndex = m_comb.GetCurSel();
    KG_PROCESS_ERROR(nSelIndex != -1);

    m_comb.GetLBText(nSelIndex, szName);

    hRetCode = pScene->GetSceneEditBase(&piSceneEditorBase);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

	GETSCENE(pScene);
    hRetCode = p3DScene->GetFirstRenderEntity(&piEntity);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

	IEKG3DSceneSelectionTool* pTool = NULL;
	HRESULT hr = pScene->GetSelectionToolInterface(&pTool);
	if (FAILED(hr))
	{
		return;
	}

	GETSELECT(piSceneEditorBase);
    while (piEntity)
    {
        hRetCode = piEntity->GetType(&EntityType);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        if (EntityType == SCENEENTITY_MODEL)
        {

            hRetCode = piEntity->GetPointer((VOID **)&pModel);
            KGLOG_COM_PROCESS_ERROR(hRetCode);

            hRetCode = pModel->GetName(&pszName);
            KGLOG_COM_PROCESS_ERROR(hRetCode);

            char* pName = strrchr(const_cast<char *>(pszName), '\\');
            if (pName && !stricmp(++pName, szName))
            {
                pScene->SetCurBindModel(pModel);
				//pSelector->ClearSelectedEntity();
				pTool->ClearSelection();
				
				//pSelector->AddSelectedEntity(EntityType, pModel);
				pTool->SetSelected(EntityType, pModel, TRUE);
                
				FillSock();
                FillBone();
                break;
            }
        }

		GETSCENE(pScene);
        hRetCode = p3DScene->GetNextRenderEntity(&piEntity);
        KGLOG_COM_PROCESS_ERROR(hRetCode);
    }

    nResult = true;
Exit0:
    return;
}

void KGSFXEditSockBonePage::OnTvnSelchangedBone(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    HTREEITEM    hTreeItem  = NULL;
    CString      strBoneName = TEXT("");
    IEKG3DModel *piCurBindModel = NULL;
    GET_SFX_EDITOR();

    hTreeItem = m_bone.GetSelectedItem();
    KG_PROCESS_ERROR(hTreeItem);
    strBoneName = m_bone.GetItemText(hTreeItem);
    if (strBoneName == TEXT("原点"))
    {
        strBoneName = TEXT("");
        pScene->SetBindTargerInfo(0, strBoneName);
    }
    else
    {
        pScene->SetBindTargerInfo(1, strBoneName);
    }
    pScene->GetCurBindModel(&piCurBindModel);
    if (piCurBindModel)
    {
        IEKG3DMeshBone* piMeshBone = NULL;
        piCurBindModel->GetIEKG3DMeshBone(&piMeshBone);

        if (piMeshBone)
            piMeshBone->SetSelectedBone(strBoneName);
    }

    m_sock.SelectItem(NULL);

Exit0:
    *pResult = 0;
}

void KGSFXEditSockBonePage::OnTvnSelchangedSock(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    HTREEITEM    hTreeItem  = NULL;
    CString      strSockName = TEXT("");

    GET_SFX_EDITOR();

    hTreeItem = m_sock.GetSelectedItem();
    KG_PROCESS_ERROR(hTreeItem);
    strSockName = m_sock.GetItemText(hTreeItem);

    pScene->SetBindTargerInfo(2, strSockName);
    m_bone.SelectItem(NULL);

Exit0:
    *pResult = 0;
}

void KGSFXEditSockBonePage::OnSize(UINT nType, int cx, int cy)
{
    CPropertyPage::OnSize(nType, cx, cy);
    RECT rectClient;
    RECT rectTree;
    GetClientRect(&rectClient);

    rectTree = rectClient;
    rectTree.top   += 52;
    rectTree.left  += 2;
    rectTree.right -= 2;
    rectTree.bottom = rectClient.bottom / 2;

    if (::IsWindow(m_bone.m_hWnd))
        m_bone.MoveWindow(&rectTree);

    rectTree.top = rectTree.bottom + 2;
    rectTree.bottom = rectClient.bottom;

    if (::IsWindow(m_sock.m_hWnd))
        m_sock.MoveWindow(&rectTree);
}

void KGSFXEditSockBonePage::OnBnClickedLink()
{
    GET_SFX_EDITOR();
    if (((CButton*)GetDlgItem(IDC_LINK))->GetCheck())
    {
        pScene->UpdateSceneMode(SFX_SCENE_MODE_BIND);
    }
    else
    {
        pScene->UpdateSceneMode(SFX_SCENE_MODE_EDIT);
    }
    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
    pScene->GetSceneEditBase(&piSceneEditorBase);

	//piSceneEditorBase->m_listSelectedEntity.clear();
	GETSELECT(piSceneEditorBase);
	//pSelector->ClearSelectedEntity();

	IEKG3DSceneSelectionTool* pTool = NULL;
	pScene->GetSelectionToolInterface(&pTool);
	if(NULL == pTool)
		return;

	pTool->ClearSelection();

    FillBone();
    FillSock();
}

void KGSFXEditSockBonePage::OnNMDblclkBone(NMHDR *pNMHDR, LRESULT *pResult)
{
    IEKG3DModel *pModel = NULL;
    IEKG3DMeshBone* piBoneMesh = NULL;
    DWORD dwSFXSceneMode = 0;
    GET_SFX_EDITOR();

    pScene->GetSFXSceneMode(&dwSFXSceneMode);
    pScene->GetCurBindModel(&pModel);

    if (dwSFXSceneMode == SFX_SCENE_MODE_BIND && pModel)
    {
        LPCTSTR pcszBindTargeInfoName = NULL;
        pScene->GetBindTargerInfoName(&pcszBindTargeInfoName);

        pModel->GetIEKG3DMeshBone(&piBoneMesh);

        if (piBoneMesh)
            piBoneMesh->SetSelectedBone(pcszBindTargeInfoName);

        if (!stricmp(pcszBindTargeInfoName, TEXT("")))
            pScene->BindSelMdlToTargetMdl(pcszBindTargeInfoName, 1);
        else
            pScene->BindSelMdlToTargetMdl(pcszBindTargeInfoName, 0);

    }
    *pResult = 0;
}

void KGSFXEditSockBonePage::OnNMDblclkSock(NMHDR *pNMHDR, LRESULT *pResult)
{

    IEKG3DModel *pModel = NULL;
    IEKG3DMeshBone* piBoneMesh = NULL;
    DWORD dwSFXSceneMode = 0;
    GET_SFX_EDITOR();

    pScene->GetSFXSceneMode(&dwSFXSceneMode);
    pScene->GetCurBindModel(&pModel);

    if (dwSFXSceneMode == SFX_SCENE_MODE_BIND && pModel)
    {
        LPCTSTR pcszBindTargeInfoName = NULL;
        pScene->GetBindTargerInfoName(&pcszBindTargeInfoName);

        pModel->GetIEKG3DMeshBone(&piBoneMesh);

        if (piBoneMesh)
            piBoneMesh->SetSelectedBone(pcszBindTargeInfoName);
        pScene->BindSelMdlToTargetMdl(pcszBindTargeInfoName, 2);
    }
    *pResult = 0;
}



void KGSFXEditSockBonePage::OnCbnSetfocusModel()
{
    GET_SFX_EDITOR();
    if (!pScene->IsModifyMode())
        FillComb();
    return;
}
