// KPvsPlacePanel.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KPvsPlacePanel.h"
#include "KG3DSceneEditorDirectory.h"
#include "KSceneSceneEditorDialogOranment.h"
#include "IEKG3DSceneSFXEditor.h"
#include "KPvsEditorToolSheet.h"
#include "KSceneSceneEditorDialogOranment.h"

extern int GetItemImage(LPSTR extName);

IMPLEMENT_DYNAMIC(KPvsPlacePanel, KGLilisSlipPanel<CPropertyPage>)

KPvsPlacePanel::KPvsPlacePanel()
	: KGLilisSlipPanel<CPropertyPage>(KPvsPlacePanel::IDD),
      m_nOutputWindowID(0xffffffff),
      m_bFilterMesh(FALSE),
      m_bFilterMeshSet(FALSE),
      m_bFilterPvs(FALSE),
      m_bFilterSfx(FALSE)
{

}

KPvsPlacePanel::~KPvsPlacePanel()
{
}

void KPvsPlacePanel::DoDataExchange(CDataExchange* pDX)
{
	KGLilisSlipPanel<CPropertyPage>::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE, m_treeObj);
    DDX_Control(pDX, IDC_TREE_PORTAL, m_treePtl);
}


BEGIN_MESSAGE_MAP(KPvsPlacePanel, KGLilisSlipPanel<CPropertyPage>)
    ON_NOTIFY(NM_CLICK, IDC_TREE, &KPvsPlacePanel::OnNMClickTree)
    ON_NOTIFY(NM_CLICK, IDC_TREE_PORTAL, &KPvsPlacePanel::OnNMClickTree)
    ON_BN_CLICKED(IDC_BUTTON_RF, &KPvsPlacePanel::OnBnClickedButtonRf)
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_CHECK_MESH, &KPvsPlacePanel::OnBnClickedCheckMesh)
    ON_BN_CLICKED(IDC_CHECK_MESHSET, &KPvsPlacePanel::OnBnClickedCheckMeshset)
    ON_BN_CLICKED(IDC_CHECK_SFX, &KPvsPlacePanel::OnBnClickedCheckSfx)
    //ON_BN_CLICKED(IDC_CHECK_PVS, &KPvsPlacePanel::OnBnClickedCheckPvs)
    ON_BN_CLICKED(IDC_CHECK_CULL, &KPvsPlacePanel::OnBnClickedCheckCull)
    ON_BN_CLICKED(IDC_BUTTON_POINTLIGHT, &KPvsPlacePanel::OnBnClickedButtonPointlight)
END_MESSAGE_MAP()

void KPvsPlacePanel::OnSize(UINT nType, int cx, int cy)
{
    if (IsWindow(m_treeObj) && m_vecChileWnd.size())
    {
        RECT rect;
        RECT rect_temp;

        GetClientRect(&rect);
        
        rect_temp = rect;
        rect_temp.left += 10;
        rect_temp.right -= 10;
        rect_temp.bottom = rect_temp.top + 100;

        m_treePtl.MoveWindow(&rect_temp, TRUE);

        rect_temp.top = m_vecChileWnd[0].vWndPane.m_Rect.bottom + 80;
        rect_temp.bottom = rect_temp.top + 230;
        GetDlgItem(IDC_OBJ_VIEW)->MoveWindow(&rect_temp, TRUE);

        if (GetPvsScene() && cx != 0 && cy != 0)
            GetPvsScene()->GetScene()->OnResizeWindow(NULL);

        rect_temp.top = m_vecChileWnd[0].vWndPane.m_Rect.bottom + 315;
        rect_temp.bottom = rect.bottom - 50;
        
        m_treeObj.MoveWindow(&rect_temp, TRUE);
        RecalcPaneSize();
    }

    UpdateUI();
    
    KGLilisSlipPanel<CPropertyPage>::OnSize(nType, cx, cy);
}

void KPvsPlacePanel::OnDestroy()
{
    if (GetPvsScene())
        GetPvsScene()->GetScene()->RemoveOutputWindow(m_nOutputWindowID);   
}

IEKG3DScenePvsEditor* KPvsPlacePanel::GetPvsScene()
{
    if (!GetParent() || !GetParent()->GetParent())
        return NULL;

    KPvsEditorToolSheet* pSheet = static_cast<KPvsEditorToolSheet*>(GetParent()->GetParent());

    if (!pSheet)
        return NULL;

    return pSheet->GetPvsScene();

    //m_pPvsScene = pScene;

    //if (m_pPvsScene)
    //{
    //    m_pPvsScene->GetScene()->RemoveOutputWindow(m_nOutputWindowID);   
    //    m_pPvsScene->GetScene()->AddOutputWindow("Pvs Obj View", GetDlgItem(IDC_OBJ_VIEW)->GetSafeHwnd(), OUTPUTWND_SCENEEDITOR_OBJECTPREVEIW, 0, &m_nOutputWindowID);
    //}
}

BOOL KPvsPlacePanel::OnInitDialog()
{
    int nResult = KGLilisSlipPanel<CPropertyPage>::OnInitDialog();

    m_treeObj.SetTextColor(RGB(0, 0, 0));
    m_treeObj.SetLineColor(RGB(122 , 122, 122));
    m_treeObj.SetBkColor(RGB(255,  255, 255));

    m_treePtl.SetTextColor(RGB(0, 0, 0));
    m_treePtl.SetLineColor(RGB(122 , 122, 122));
    m_treePtl.SetBkColor(RGB(255,  255, 255));

    m_imageListTree.Create(18, 18, ILC_COLOR24, 10, 10);
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_DIR));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_MESH));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_SPEEDTREE));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_SFX));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_OBJ));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_MESH));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_MESH));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_SOUND));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_MODEL));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_MESH));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_PVS));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_PORTAL));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_PVS));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_PORTAL));

    m_treeObj.SetImageList(&m_imageListTree, TVSIL_NORMAL);
    m_treePtl.SetImageList(&m_imageListTree, TVSIL_NORMAL);

    m_treePtl.InsertItem(TEXT("BillBoard"), IMAGE_PORATL, IMAGE_PORATL, TVI_ROOT);
    m_treePtl.InsertItem(TEXT("Lock Y"), IMAGE_PORATL, IMAGE_PORATL, TVI_ROOT);
    m_treePtl.InsertItem(TEXT("Horizontal"), IMAGE_PORATL, IMAGE_PORATL, TVI_ROOT);

    CheckDlgButton(IDC_CHECK_MESH, TRUE);
    CheckDlgButton(IDC_CHECK_MESHSET, TRUE);
    CheckDlgButton(IDC_CHECK_SFX, TRUE);
    CheckDlgButton(IDC_CHECK_PVS, TRUE);

    return nResult;
}

bool KPvsPlacePanel::Filter(LPCTSTR str)
{
    if (!stricmp(str, "editor"))
        return true;

    if (!stricmp(str, "music"))
        return true;

    if (!stricmp(str, "public"))
        return true;

    if (!stricmp(str, "sound"))
        return true;

    if (!stricmp(str, "maps"))
        return true;

    if (!stricmp(str, "texture"))
        return true;

    if (!stricmp(str, ".svn"))
        return true;

    if (!stricmp(str, "."))
        return true;

    if (!stricmp(str, ".."))
        return true;

    char* ext = strrchr((char*)str, '.');

    if (!ext)
        return false;

    if (m_bFilterMesh && !stricmp(ext, ".mesh"))
        return true;

    if (m_bFilterMeshSet && !stricmp(ext, ".modelset"))
        return true;

    if (m_bFilterSfx && !stricmp(ext, ".Sfx"))
        return true;

    if (m_bFilterPvs && !stricmp(ext, ".pvs"))
        return true;

    return false;

}

void KPvsPlacePanel::SubFillTree(LPSTR Dir, LPSTR pFileType, HTREEITEM hStart)
{
    TCHAR drive[256];
    TCHAR dir[256];
    TCHAR fname[256];
    TCHAR ext[256];

    WIN32_FIND_DATA Find;
    
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();

    TCHAR szAppName[MAX_PATH];
    TCHAR szHalfPath[MAX_PATH];
    TCHAR szFullPath[MAX_PATH];
    TCHAR szSingleFile[MAX_PATH];

    wsprintf(szFullPath,"%s%s\\*.*", g_szDefWorkDirectory, Dir);
    g_pEngineManager->GetDefWorkDirectory(szAppName, MAX_PATH);
    
    HANDLE hFind = FindFirstFile(szFullPath, &Find);
    
    if (hFind != INVALID_HANDLE_VALUE)
    {
        strcpy(szSingleFile, Find.cFileName);
        _splitpath(szSingleFile, drive, dir, fname, ext );

        if (!Filter(szSingleFile))
        {
            if ((Find.dwFileAttributes != 0xFFFFFFFF)
                &&(Find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                HTREEITEM hItem = m_treeObj.InsertItem(szSingleFile, IMAGE_DIR,IMAGE_DIR, hStart);
                wsprintf(szHalfPath,"%s\\%s", Dir, szSingleFile);
                SubFillTree(szHalfPath, pFileType, hItem);
            }
            else
            {	

                int n = GetItemImage(ext);
                if (n != IMAGE_ERROR)
                {
                    TCHAR strFilePath[MAX_PATH];
                    sprintf_s(strFilePath,
                        MAX_PATH,
                        "%s%s\\%s",
                        szAppName,
                        Dir,
                        szSingleFile);
                    m_treeObj.InsertItem(szSingleFile, n, n, hStart);
                }
            }
        }

        while(FindNextFile(hFind, &Find))
        {
            strcpy(szSingleFile, Find.cFileName);
            _splitpath(szSingleFile, drive, dir, fname, ext );

            if(!Filter(szSingleFile))
            {
                if((Find.dwFileAttributes != 0xFFFFFFFF)
                    &&(Find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    HTREEITEM hItem = m_treeObj.InsertItem(szSingleFile, IMAGE_DIR,IMAGE_DIR, hStart);
                    wsprintf(szHalfPath,"%s\\%s", Dir, szSingleFile);
                    SubFillTree(szHalfPath, pFileType, hItem);
                }
                else	
                {
                    int n = GetItemImage(ext);
                    if (n != IMAGE_ERROR)
                    {
                        TCHAR strFilePath[MAX_PATH];
                        sprintf_s(strFilePath,
                            MAX_PATH,
                            "%s%s\\%s",
                            szAppName,
                            Dir,
                            szSingleFile);
                        m_treeObj.InsertItem(szSingleFile, n, n, hStart);
                    }
                }
            }
        }

        FindClose(hFind);
    }
}


void KPvsPlacePanel::FillTree()
{
    m_treeObj.DeleteAllItems();

    int nCount = g_cEditorDirectory.GetNumofDirectory();
    for(int i=0;i<nCount;i++)
    {
        TCHAR Title[256];
        TCHAR Name[256];

        g_cEditorDirectory.GetDirectory(i,Title,Name);

        size_t k = strlen(Name);
        if(Name[k-1] == '\\')
        {
            Name[k-1] = '\0';
        }
        HTREEITEM hitem = m_treeObj.InsertItem(Name,IMAGE_DIR,IMAGE_DIR, TVI_ROOT);
        SubFillTree(Name,".Mesh",hitem);
    }

}

void KPvsPlacePanel::GetItemFilePath(HTREEITEM hItem, CString& RetPath)
{
    //    TCHAR szAppDir[MAX_PATH];
    TCHAR strFileName[MAX_PATH];
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();

    KG_PROCESS_ERROR(hItem);

    strcpy(strFileName, m_treeObj.GetItemText(hItem));
    HTREEITEM hParentItem = m_treeObj.GetParentItem(hItem);
    while (hParentItem)
    {
        RetPath.Format(_T("%s\\%s"), m_treeObj.GetItemText(hParentItem), strFileName);
        _tcscpy_s(strFileName, RetPath);
        hParentItem = m_treeObj.GetParentItem(hParentItem);
    }
    //pManager->GetDefWorkDirectory(szAppDir, MAX_PATH);
    RetPath.Format(_T("%s"),strFileName);
Exit0:
    return;
}

void KPvsPlacePanel::OnNMClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    if (m_nOutputWindowID == 0xffffffff && GetPvsScene())
    {
        GetPvsScene()->GetScene()->RemoveOutputWindow(m_nOutputWindowID);   
        GetPvsScene()->GetScene()->AddOutputWindow("Pvs Obj View", GetDlgItem(IDC_OBJ_VIEW)->GetSafeHwnd(), OUTPUTWND_SCENEEDITOR_OBJECTPREVEIW, 0, &m_nOutputWindowID);
    }

    HTREEITEM selItem;
    CPoint ptMouse;

    GetCursorPos(&ptMouse);
    m_treeObj.ScreenToClient(&ptMouse);
    selItem = m_treeObj.HitTest(ptMouse);

    if (!selItem)
    {
        GetCursorPos(&ptMouse);
        m_treePtl.ScreenToClient(&ptMouse);
        selItem = m_treePtl.HitTest(ptMouse);
    }

    if (selItem && GetPvsScene())
    {
        CString strFilePath;
        GetItemFilePath(selItem, strFilePath);

        if (strFilePath == "BillBoard")
        {
            GetPvsScene()->SetPlaceProtalMode(PLACE_PTL_BILLBOARD);
            strFilePath = "Portal";
        }
        else if (strFilePath == "Lock Y")
        {
            GetPvsScene()->SetPlaceProtalMode(PLACE_PTL_LOCK_Y);
            strFilePath = "Portal";
        }
        else if (strFilePath == "Horizontal")
        {
            GetPvsScene()->SetPlaceProtalMode(PLACE_PTL_HOR);
            strFilePath = "Portal";
        }

        GetPvsScene()->SetHandObject(strFilePath.GetBuffer());
        strFilePath.ReleaseBuffer();

    }

    *pResult = 0;
}



void KPvsPlacePanel::OnBnClickedButtonRf()
{
    FillTree();
}

void KPvsPlacePanel::OnBnClickedCheckMesh()
{
    m_bFilterMesh = !IsDlgButtonChecked(IDC_CHECK_MESH);
    FillTree();
}

void KPvsPlacePanel::OnBnClickedCheckMeshset()
{
    m_bFilterMeshSet = !IsDlgButtonChecked(IDC_CHECK_MESHSET);
    FillTree();
}

void KPvsPlacePanel::OnBnClickedCheckSfx()
{
    m_bFilterSfx = !IsDlgButtonChecked(IDC_CHECK_SFX);
    FillTree();
}

void KPvsPlacePanel::OnBnClickedCheckPvs()
{
    m_bFilterPvs = !IsDlgButtonChecked(IDC_CHECK_PVS);
    FillTree();
}

void KPvsPlacePanel::OnBnClickedCheckCull()
{
    if (GetPvsScene())
    {
        GetPvsScene()->EnableCull(IsDlgButtonChecked(IDC_CHECK_CULL));
    }
}

void KPvsPlacePanel::OnBnClickedButtonPointlight()
{
    if (GetPvsScene())
        GetPvsScene()->SetHandObject(PLIGHT_FILE_PATH);
}
