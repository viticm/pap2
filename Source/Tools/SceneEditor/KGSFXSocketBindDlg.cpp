////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGSFXSocketBindDlg.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-9-8 15:47:06
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGSFXSocketBindDlg.h"
#include "KSceneSFXEditorFrame.h"
#include "IEKG3DModelST.h"
#include "IEKG3DModelTable.h"
#include "IEKG3DMesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KGSFXSocketBindDlg, CDialog)

KGSFXSocketBindDlg::KGSFXSocketBindDlg(CWnd* pDependWnd)
	: CDialog(KGSFXSocketBindDlg::IDD, pDependWnd)
{
}

KGSFXSocketBindDlg::~KGSFXSocketBindDlg()
{
}

void KGSFXSocketBindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE_S, m_treeSocket);
    DDX_Control(pDX, IDC_TREE_M, m_treeModels);
    DDX_Control(pDX, IDC_SOCK_UNBIND, m_buttonUnBind);
    DDX_Control(pDX, IDC_SOCK_BIND, m_buttonBind);
    DDX_Text(pDX, IDC_EDIT_INFO, m_strBindInfo);
}


BEGIN_MESSAGE_MAP(KGSFXSocketBindDlg, CDialog)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_S, &KGSFXSocketBindDlg::OnTvnSelchangedSocket)
    ON_NOTIFY(NM_DBLCLK, IDC_TREE_M, &KGSFXSocketBindDlg::OnNMClickTree)
    ON_BN_CLICKED(IDC_SOCK_UNBIND, &KGSFXSocketBindDlg::OnButtonClickUnbind)
    ON_BN_CLICKED(IDC_SOCK_BIND, &KGSFXSocketBindDlg::OnButtonClickBind)
END_MESSAGE_MAP()

void KGSFXSocketBindDlg::OnButtonClickUnbind()
{
    Unbind();
}

void KGSFXSocketBindDlg::OnButtonClickBind()
{
   Bind();
}

void KGSFXSocketBindDlg::OnTvnSelchangedSocket(NMHDR *pNMHDR, LRESULT *pResult)
{
    HTREEITEM     hTreeItem  = NULL;
    IEKG3DModel*    pBindModel = NULL;
    LPCTSTR pszName = NULL;
    HRESULT hRetCode = E_FAIL;

    CString       strSockName  = TEXT("");
    CString       strModelName = TEXT("");

    GET_SFX_EDITOR();

    pScene->GetRefrenceModel(0, &pBindModel);
    KG_PROCESS_ERROR(pBindModel);

    hTreeItem = m_treeSocket.GetSelectedItem();
    KG_PROCESS_ERROR(hTreeItem);
    strSockName = m_treeSocket.GetItemText(hTreeItem);

    hRetCode = pScene->GetModelNameBySocketName(strSockName, &pszName);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    m_strBindInfo = pszName;

    UpdateData(FALSE);

Exit0:
    return;
}

BOOL KGSFXSocketBindDlg::OnInitDialog()
{
    int nResult = CDialog::OnInitDialog();

    m_treeSocket.SetTextColor(RGB(255, 255, 255));
    m_treeSocket.SetLineColor(RGB(122, 122, 122));
    m_treeSocket.SetBkColor(RGB(92,  92, 92));

    m_treeModels.SetTextColor(RGB(255, 255, 255));
    m_treeModels.SetLineColor(RGB(122, 122, 122));
    m_treeModels.SetBkColor(RGB(92,  92, 92));

    m_imageListTree.Create(16, 16, ILC_COLOR24, 5, 5);
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_FC));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_GLEAN));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_TALK));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_OFFER));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_POS));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_BN));

    m_treeModels.SetImageList(&m_imageListTree, TVSIL_NORMAL);
    m_treeSocket.SetImageList(&m_imageListTree, TVSIL_NORMAL);

    return nResult;
}

int KGSFXSocketBindDlg::Init()
{
    int nResult = false;

    TCHAR path[MAX_PATH];
    sprintf(path, "%s%s", g_szDefWorkDirectory, TEXT("\\Data"));

    FillSocket();
    m_treeModels.DeleteAllItems();
    FillModels(NULL, path);

    return nResult;
}

void KGSFXSocketBindDlg::FillSocket()
{
    HRESULT hRetCode = E_FAIL;
    int nResult  = false;
    int nRetCode = false;
    int nCount = 0;
    IKG3DModel*	 pModelInVector = NULL;
	
    GET_SFX_EDITOR();
    IEKG3DModel*	 pModel = NULL;
    IEKG3DModelST *pMdlST = NULL;
    //IEKG3DMeshBase *pMeshBase = NULL;
    IEKG3DMesh *pMesh = NULL;

    pScene->GetRefrenceModel(0, &pModel);
    KG_PROCESS_ERROR(pModel);

    pModel->GetIEKG3DModelST(&pMdlST);

    if (pMdlST)
    {
        m_treeSocket.DeleteAllItems();
        
        nCount = pModel->GetNumModel();

        for (int s = 0; s < nCount; s++)
        {
            IEKG3DModel *pieModel = NULL;
			pModel->GetModel(s, &pModelInVector);
			
			pieModel = dynamic_cast<IEKG3DModel*>(pModelInVector);
            //KG3DMeshEx *pMesh     = (KG3DMeshEx*)(pModelInVector->GetMesh());
            pieModel->GetIEKG3DMesh(&pMesh);
            if (!pMesh)
                continue;
            DWORD dwNumSocket = 0;
            hRetCode = pMesh->GetSocketCount(&dwNumSocket);
            KGLOG_COM_PROCESS_ERROR(hRetCode);

            for (DWORD i = 0; i < dwNumSocket; i++)
            {
                LPCTSTR pszSocketName = NULL;
                hRetCode = pMesh->GetSocketName(i, &pszSocketName);
                KGLOG_COM_PROCESS_ERROR(hRetCode);

                m_treeSocket.InsertItem(pszSocketName, 5, 5);
            }
        }
    }
    else
    {
        IEKG3DMesh *pMesh = NULL;
        //IEKG3DMeshBase *pMeshBase = NULL;
        pModel->GetIEKG3DMesh(&pMesh);
        if (pMesh)
        {
            m_treeSocket.DeleteAllItems();
            DWORD dwNumSocket = 0;
            hRetCode = pMesh->GetSocketCount(&dwNumSocket);
            KGLOG_COM_PROCESS_ERROR(hRetCode);

            for (DWORD i = 0; i < dwNumSocket; i++)
            {
                LPCTSTR pszSocketName = NULL;
                hRetCode = pMesh->GetSocketName(i, &pszSocketName);
                KGLOG_COM_PROCESS_ERROR(hRetCode);

                m_treeSocket.InsertItem(pszSocketName, 5, 5);
            }

        }
    }

    nResult = true;
Exit0:
    return;
}

void KGSFXSocketBindDlg::FillModels(HTREEITEM hTreeItem, LPCTSTR szPath)
{
    int nResult  = false;
    int nRetCode = false;

    TCHAR szFilePath[MAX_PATH];
    sprintf(szFilePath, "%s%s", szPath, TEXT("\\*.*"));
    CFileFind fileFind;
    BOOL bWorking = fileFind.FindFile(szFilePath);
    while (bWorking)
    {
        bWorking = fileFind.FindNextFile();
        if (fileFind.IsDots())
            continue;
        CString strPath = fileFind.GetFilePath();
        CString strName = fileFind.GetFileName();
        if (fileFind.IsDirectory())
        {
            FillModels(m_treeModels.InsertItem(strName.GetBuffer(), hTreeItem), strPath.GetBuffer());
        }
        else
        {
            TCHAR  szName[MAX_PATH];
            strncpy(szName, strName.GetBuffer(), sizeof(szName));
            TCHAR* pszExt = strrchr(szName, '.');
            if (!pszExt)
                continue;
            if (!stricmp(pszExt, TEXT(".mdl")))
                m_treeModels.InsertItem(szName, 2, 2, hTreeItem);
            else if (!stricmp(pszExt, TEXT(".mesh")))
                m_treeModels.InsertItem(szName, 1, 1, hTreeItem);
            else if (!stricmp(pszExt, TEXT(".sfx")))
                m_treeModels.InsertItem(szName, 3, 3, hTreeItem);
            else if (!stricmp(pszExt, TEXT(".bind")))
                m_treeModels.InsertItem(szName, 4, 4, hTreeItem);
            else
                continue;
        }
    }
    fileFind.Close();
    nResult = true;
//Exit0:
    return;
}

void KGSFXSocketBindDlg::OnNMClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    Bind();
    *pResult = 0;
}

void KGSFXSocketBindDlg::Unbind()
{
    HTREEITEM     hTreeItem  = NULL;
    CString       strSockName  = TEXT("");
    HRESULT       hResult    = E_FAIL;

    GET_SFX_EDITOR();
    hTreeItem = m_treeSocket.GetSelectedItem();
    KG_PROCESS_ERROR(hTreeItem);
    strSockName = m_treeSocket.GetItemText(hTreeItem);

    hResult = pScene->UnbindItemToSocket(strSockName.GetBuffer());
    KG_COM_PROCESS_ERROR(hResult);

    ::MessageBox(
        m_hWnd,
        TEXT("取消绑定成功>_<!!"),
        TEXT("Success!!"),
        MB_OK | MB_ICONWARNING
    );

Exit0:
    return;
}

void KGSFXSocketBindDlg::Bind()
{
    HTREEITEM     hTreeItem  = NULL;
    IEKG3DModel*    pBindModel = NULL;
    CString       strSockName  = TEXT("");
    CString       strModelName = TEXT("");
    CString       strPath      = TEXT("");
    CString       strSelPath = TEXT("");
    TCHAR         szModelName[MAX_PATH];
    HRESULT       hResult    = E_FAIL;

    IEKG3DModel*    pModel = NULL;

    GET_SFX_EDITOR();

    pScene->GetRefrenceModel(0, &pBindModel);
    KG_PROCESS_ERROR(pBindModel);

    hTreeItem = m_treeSocket.GetSelectedItem();
    KG_PROCESS_ERROR(hTreeItem);
    strSockName = m_treeSocket.GetItemText(hTreeItem);

    hTreeItem = m_treeModels.GetSelectedItem();
    while (hTreeItem)
    {
        strPath =
            TEXT("\\") + m_treeModels.GetItemText(hTreeItem) + strPath;
        hTreeItem = m_treeModels.GetParentItem(hTreeItem);
    }
    strSelPath = TEXT("\\Data") + strPath;
    g_GetFullPath(szModelName, strSelPath.GetBuffer());


    hResult = pScene->BindItemToSocket(strSockName.GetBuffer(), szModelName);
    KG_COM_PROCESS_ERROR(hResult);

    ::MessageBox(
        m_hWnd,
        TEXT("绑定成功>_<!!"),
        TEXT("Success!!"),
        MB_OK | MB_ICONWARNING
    );

Exit0:
    return;
}