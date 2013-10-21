////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGSFXModelViewPage.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-9-1 4:03:44
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGSFXModelViewPage.h"
#include "KSceneSFXEditorFrame.h"
#include "IEKG3DModelST.h"
#include "IEKG3DSFX.h"
#include "KGSFXBingPage.h"
#include "IEKG3DMesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


extern TCHAR g_szDefWorkDirectory[256];


namespace {

    void SearchThread(void* pParam)
    {
        KGSFXModelViewPage* pPage = (KGSFXModelViewPage*)pParam;

        if (!pPage)
            return;

        pPage->m_bThreadExit = false;

        size_t i = pPage->m_ModelInfo.size();

        while (!pPage->m_bExitSearchThread)
        {
            if (pPage->m_bResearch)
            {
                pPage->m_listSearch.ResetContent();
                pPage->m_Progress.SetPos(0);
                i = 0;
                pPage->m_bResearch = false;
            }

            while (i < pPage->m_ModelInfo.size() && !pPage->m_bExitSearchThread)
            {
                if (pPage->m_bResearch)
                {
                    pPage->m_listSearch.ResetContent();
                    pPage->m_Progress.SetPos(0);
                    i = 0;
                    pPage->m_bResearch = false;
                }

                if (pPage->m_strSearch.empty())
                    i = (int)pPage->m_ModelInfo.size();

                if (i < pPage->m_ModelInfo.size())
                {
                    const ModelInfoPair& info = pPage->m_ModelInfo[i];
                    char str[128];
                    strncpy(str, (char*)pPage->m_strSearch.c_str(), sizeof(str));
                    char* ext = strrchr(str, '.');
                    
                    bool bMatch = false;

                    if (ext)
                    {
                        *ext++ = 0;

                        char* t = strrchr((char*)info.strName.c_str(), '.');

                        if (t && stricmp(++t, ext) == 0)
                        {
                            if (strstr(info.strName.c_str(), str))
                                bMatch = true;
                        }
                    }
                    else
                    {
                        if (strstr(info.strName.c_str(), str))
                            bMatch = true;
                    }

                    if (bMatch)
                    {
                        char* t = strrchr((char*)info.strName.c_str(), '\\');

                        if (t)
                            ++t;
                        else
                            t = (char*)info.strName.c_str();

                        int index = pPage->m_listSearch.AddString(t);

                        if (index != LB_ERR)
                        {
                            pPage->m_listSearch.SetItemData(index, (DWORD_PTR)info.hTreeItem);
                        }
                    }

                    pPage->m_Progress.SetPos((int)i);

                }

                ++i;
            }

            if (i == pPage->m_ModelInfo.size())
            {
                pPage->m_Progress.SetPos(0);
                ++i;
            }

            Sleep(100);
        }

        pPage->m_bThreadExit = true;
        //_endthread();
    }

}



//------------------------------------------------------------------------------
// Name : KGSFXBindBonePicture
// Desc :
//------------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(KGSFXBindBonePicture, CDialog)
KGSFXBindBonePicture::KGSFXBindBonePicture(CWnd* pDependWnd /* = NULL */)
    : CDialog(KGSFXModelViewPage::IDD, pDependWnd)
{
    m_rectCurCell.left = -100;
}

KGSFXBindBonePicture::~KGSFXBindBonePicture()
{
}

void KGSFXBindBonePicture::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(KGSFXBindBonePicture, CDialog)
    ON_WM_LBUTTONDBLCLK()
    ON_WM_TIMER()
    ON_WM_KILLFOCUS()
    ON_WM_SETFOCUS()
END_MESSAGE_MAP()

void KGSFXBindBonePicture::OnKillFocus(CWnd* pNewWnd)
{
    CDialog::OnKillFocus(pNewWnd);
    KillTimer(1);
}

void KGSFXBindBonePicture::OnSetFocus(CWnd* pOldWnd)
{
    //SetTimer(1, 10, NULL);
    CDialog::OnSetFocus(pOldWnd);
}

BOOL KGSFXBindBonePicture::OnInitDialog()
{
    int nResult =  CDialog::OnInitDialog();
    //SetTimer(1, 10, NULL);
    return nResult;
}

void KGSFXBindBonePicture::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1)
    {
        CClientDC dc(this);
        RECT rectCell;
        rectCell.left = m_rectCurCell.left + 5;
        rectCell.top  = m_rectCurCell.top  + 5;
        rectCell.right = m_rectCurCell.right - 5;
        rectCell.bottom = m_rectCurCell.bottom - 5;
        if (m_rectCurCell.left != -100)
            dc.FillRect(&rectCell, &CBrush(RGB(255, 2, 2)));
    }
    CDialog::OnTimer(nIDEvent);
}

void KGSFXBindBonePicture::OnLButtonDblClk(UINT uClass, CPoint point)
{
    // - -! , why can not do this in initdialo function ?
    //m_vecCellWnd.clear();
    //m_vecCellWnd.push_back(GetDlgItem(IDC_HEAD));
    //m_vecCellWnd.push_back(GetDlgItem(IDC_CLAVICLA_L));
    //m_vecCellWnd.push_back(GetDlgItem(IDC_CLAVICLA_R));
    //m_vecCellWnd.push_back(GetDlgItem(IDC_ARM_L));
    //m_vecCellWnd.push_back(GetDlgItem(IDC_ARM_R));
    //m_vecCellWnd.push_back(GetDlgItem(IDC_SPINE2));
    //m_vecCellWnd.push_back(GetDlgItem(IDC_SPINE1));
    //m_vecCellWnd.push_back(GetDlgItem(IDC_SPINE));
    //m_vecCellWnd.push_back(GetDlgItem(IDC_ROOT));
    //m_vecCellWnd.push_back(GetDlgItem(IDC_TWIST_L));
    //m_vecCellWnd.push_back(GetDlgItem(IDC_TWIST_R));
    //m_vecCellWnd.push_back(GetDlgItem(IDC_HAND_L));
    //m_vecCellWnd.push_back(GetDlgItem(IDC_HAND_R));
    //m_vecCellWnd.push_back(GetDlgItem(IDC_THIGH_L));
    //m_vecCellWnd.push_back(GetDlgItem(IDC_THIGH_R));
    //m_vecCellWnd.push_back(GetDlgItem(IDC_CALF_L));
    //m_vecCellWnd.push_back(GetDlgItem(IDC_CALF_R));
    //m_vecCellWnd.push_back(GetDlgItem(IDC_FOOT_L));
    //m_vecCellWnd.push_back(GetDlgItem(IDC_FOOT_R));

    //DWORD dwClickID = 0;
    //m_rectCurCell.left = -100;
    //for (size_t s = 0; s < m_vecCellWnd.size(); s++)
    //{
    //    POINT cPoint = {point.x, point.y};
    //    RECT  rectCell;
    //    m_vecCellWnd[s]->GetWindowRect(&rectCell);
    //    ScreenToClient(&rectCell);
    //    if (PtInRect(&rectCell, cPoint))
    //    {
    //        dwClickID = m_vecCellWnd[s]->GetDlgCtrlID();
    //        InvalidateRect(&m_rectCurCell);
    //        m_rectCurCell = rectCell;
    //        break;
    //    }
    //}
    //if (!dwClickID)
    //    return;

    //// get bone name
    //TCHAR szBoneName[128];
    //switch (dwClickID)
    //{
    //case IDC_HEAD :
    //    sprintf(szBoneName, TEXT("Bip01 Head"));
    //    break;
    //case IDC_CLAVICLA_L :
    //    sprintf(szBoneName, TEXT("Bip01 L Clavicle"));
    //    break;
    //case IDC_CLAVICLA_R :
    //    sprintf(szBoneName, TEXT("Bip01 R Clavicle"));
    //    break;
    //case IDC_ARM_L :
    //    sprintf(szBoneName, TEXT("Bip01 L UpperArm"));
    //    break;
    //case IDC_ARM_R :
    //    sprintf(szBoneName, TEXT("Bip01 R UpperArm"));
    //    break;
    //case IDC_SPINE2 :
    //    sprintf(szBoneName, TEXT("Bip01 Spine2"));
    //    break;
    //case IDC_SPINE1 :
    //    sprintf(szBoneName, TEXT("Bip01 Spine1"));
    //    break;
    //case IDC_SPINE :
    //    sprintf(szBoneName, TEXT("Bip01 Spine"));
    //    break;
    //case IDC_ROOT :
    //    sprintf(szBoneName, TEXT("Bip01 Pelvis"));
    //    break;
    //case IDC_TWIST_L :
    //    sprintf(szBoneName, TEXT("Bip01 L ForeTwist"));
    //    break;
    //case IDC_TWIST_R :
    //    sprintf(szBoneName, TEXT("Bip01 R ForeTwist"));
    //    break;
    //case IDC_HAND_L :
    //    sprintf(szBoneName, TEXT("Bip01 L Hand"));
    //    break;
    //case IDC_HAND_R :
    //    sprintf(szBoneName, TEXT("Bip01 R Hand"));
    //    break;
    //case IDC_THIGH_L :
    //    sprintf(szBoneName, TEXT("Bip01 L Thigh"));
    //    break;
    //case IDC_THIGH_R :
    //    sprintf(szBoneName, TEXT("Bip01 R Thigh"));
    //    break;
    //case IDC_CALF_L :
    //    sprintf(szBoneName, TEXT("Bip01 L Calf"));
    //    break;
    //case IDC_CALF_R :
    //    sprintf(szBoneName, TEXT("Bip01 R Calf"));
    //    break;
    //case IDC_FOOT_L :
    //    sprintf(szBoneName, TEXT("Bip01 L Foot"));
    //    break;
    //case IDC_FOOT_R :
    //    sprintf(szBoneName, TEXT("Bip01 R Foot"));
    //    break;
    //default :
    //    break;
    //}

    //KG3DSFXSuede* pSFXSuede  = NULL;
    //IEKG3DModel*    pBindModel = NULL;
    //IKG3DModel*   pSFXMDL    = NULL;
    //int    nCurSFXIndex      = 0;
    //CString strBoneName = TEXT("");
    //HRESULT       hResult    = E_FAIL;

    //strBoneName  = szBoneName;

    //GET_SFX_EDITOR();

    //// bind model
    //pBindModel = pScene->m_pBindModel;
    //KG_PROCESS_ERROR(pBindModel);

    //// suede
    //pSFXSuede = pScene->GetSFXSuede();
    //KG_PROCESS_ERROR(pSFXSuede);

    //// sfx
    //nCurSFXIndex = pScene->GetCurSfxBindIndex();
    //pSFXMDL = pSFXSuede->GetSFX(nCurSFXIndex);
    //if (!pSFXMDL)
    //{
    //    ::MessageBox(
    //        m_hWnd,
    //        TEXT("你还没有选择好特效呢>_<!!"),
    //        TEXT("Warning"),
    //        MB_OK | MB_ICONWARNING
    //        );
    //    KG_PROCESS_ERROR(false);
    //}

    //hResult = pScene->UnBindSuedeSFX();
    //KG_COM_PROCESS_ERROR(hResult);

    //pScene->BindSuedeSFX(strBoneName.GetBuffer());
    //KG_COM_PROCESS_ERROR(hResult);

    //::MessageBox(
    //    m_hWnd,
    //    TEXT("绑定成功>_<!!"),
    //    TEXT("Success!!"),
    //    MB_OK | MB_ICONWARNING
    //    );

    //GET_SFX_BIND_PAGE();
    //pBindPage->UpdateBondInfo(strBoneName.GetBuffer());

//Exit0:
    return;
}


//------------------------------------------------------------------------------
// Name : KGSFXModelViewPage
// Desc :
//------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(KGSFXModelViewPage, CPropertyPage)
KGSFXModelViewPage::KGSFXModelViewPage()
	: CPropertyPage(KGSFXModelViewPage::IDD)
{
    m_strStartPath = TEXT("\\data\\source\\");
    m_strSelPath  = TEXT("");
    m_strAinPath  = TEXT("");
    m_pFillThread = NULL;

    m_bExitSearchThread = false;
    m_bResearch = false;
    m_strSearch = "";
    m_bThreadExit = true;
    m_hSearch = NULL;
}

KGSFXModelViewPage::~KGSFXModelViewPage()
{

}

void KGSFXModelViewPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE, m_tree);
	DDX_Control(pDX, IDC_BUTTON_REFUSH, m_buttomReflush);
    DDX_Control(pDX, IDC_EDIT_SEARCH, m_EditSearch);
    DDX_Control(pDX, IDC_LIST_SEARCH, m_listSearch);
    DDX_Control(pDX, IDC_PROGRESS, m_Progress);
}

BEGIN_MESSAGE_MAP(KGSFXModelViewPage, CPropertyPage)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &KGSFXModelViewPage::OnTvnSelchangedTree1)
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_REFUSH, &KGSFXModelViewPage::OnBnClickedButtonRefush)
    ON_EN_CHANGE(IDC_EDIT_SEARCH, &KGSFXModelViewPage::OnEnChangeEditSearch)
    ON_LBN_SELCHANGE(IDC_LIST_SEARCH, &KGSFXModelViewPage::OnLbnSelchangeListSearch)
    ON_NOTIFY(NM_DBLCLK, IDC_TREE, &KGSFXModelViewPage::OnNMDblclkTree)
    ON_WM_CLOSE()
END_MESSAGE_MAP()

void KGSFXModelViewPage::OnDestroy()
{
    if (m_pFillThread)
        TerminateThread(m_pFillThread->m_hThread, 0);

    m_bExitSearchThread = true;
    if (m_hSearch)
        TerminateThread(m_hSearch, 0);
}

void KGSFXModelViewPage::OnPaint()
{
    CPropertyPage::OnPaint();
}

void KGSFXModelViewPage::OnSize(UINT nType, int cx, int cy)
{
    CPropertyPage::OnSize(nType, cx, cy);
    GET_SFX_EDITOR();
    RECT rectClient;
    RECT rectEdit;
    GetClientRect(&rectClient);
    rectClient.top += 25;
    rectClient.left += 2;
    //rectClient.bottom -= 5;
    rectClient.right -= 2;
    rectClient.bottom -= 250;
    if (::IsWindow(m_tree.m_hWnd))
        m_tree.MoveWindow(&rectClient);

    rectClient.top = rectClient.bottom + 2;
    rectClient.bottom = rectClient.top + 200;

    if (::IsWindow(m_listSearch))
    {
        m_listSearch.MoveWindow(&rectClient);
    }

    rectClient.top = rectClient.bottom + 2;
    rectClient.bottom = rectClient.top + 10;

    if (::IsWindow(m_Progress))
    {
        m_Progress.MoveWindow(&rectClient);
    }

    if (::IsWindow(m_EditSearch))
    {
        m_EditSearch.GetWindowRect(&rectEdit);;
        rectClient.top = rectClient.bottom + 2;
        rectClient.bottom = rectClient.top + rectEdit.bottom - rectEdit.top;
        m_EditSearch.MoveWindow(&rectClient);
    }

    //if (pScene->GetSFXSceneMode() == SFX_SCENE_MODE_EDIT)
    //{
    //    RECT rectClient;
    //    RECT rectTree;

    //    GetClientRect(&rectClient);
    //    rectClient.left += 2;
    //    rectTree = rectClient;
    //    rectTree.bottom -= 10;
    //    rectTree.top  = rectTree.bottom - 20;
    //    rectTree.right -= 2;

    //    if (::IsWindow(m_comb.m_hWnd))
    //        m_comb.MoveWindow(&rectTree);
    //    rectTree.bottom = rectTree.top;
    //    rectTree.top    = rectClient.top + 2;
    //    if (::IsWindow(m_tree.m_hWnd))
    //        m_tree.MoveWindow(&rectTree);

    //    if (::IsWindow(m_bone.m_hWnd))
    //        m_bone.ShowWindow(SW_HIDE);
    //    if (GetDlgItem(IDC_BLIST))
    //        GetDlgItem(IDC_BLIST)->ShowWindow(SW_HIDE);
    //    if (GetDlgItem(IDC_BTABL))
    //        GetDlgItem(IDC_BTABL)->ShowWindow(SW_HIDE);
    //    if (::IsWindow(m_pictureBindBone.m_hWnd))
    //        m_pictureBindBone.ShowWindow(SW_HIDE);
    //}
    //else
    //{
    //    RECT rectClient;
    //    RECT rectTree;
    //    RECT rectRadio;
    //    GetClientRect(&rectClient);
    //    rectClient.left += 2;
    //    rectTree = rectClient;
    //    rectTree.bottom = rectTree.bottom - 5;
    //    rectTree.top  = rectTree.bottom - 215;
    //    //rectTree.left += 2;
    //    rectTree.right -= 2;
    //    if (::IsWindow(m_bone.m_hWnd))
    //        m_bone.MoveWindow(&rectTree);
    //    if (::IsWindow(m_pictureBindBone.m_hWnd))
    //        m_pictureBindBone.MoveWindow(&rectTree);
    //    rectTree.bottom = rectTree.top - 2;
    //    rectTree.top    = rectTree.top - 20;
    //    rectRadio = rectTree;
    //    rectRadio.left += 10;
    //    rectRadio.right = rectRadio.left + 80;
    //    if (GetDlgItem(IDC_BLIST))
    //        GetDlgItem(IDC_BLIST)->MoveWindow(&rectRadio);
    //    rectRadio.left  = rectRadio.right + 2;
    //    rectRadio.right = rectTree.right;
    //    if (GetDlgItem(IDC_BTABL))
    //        GetDlgItem(IDC_BTABL)->MoveWindow(&rectRadio);

    //    rectTree.bottom = rectTree.top - 2;
    //    rectTree.top    = rectTree.top - 20;
    //    if (::IsWindow(m_comb.m_hWnd))
    //        m_comb.MoveWindow(&rectTree);
    //    rectTree.bottom = rectTree.top;
    //    rectTree.top    = rectClient.top + 2;
    //    if (::IsWindow(m_tree.m_hWnd))
    //        m_tree.MoveWindow(&rectTree);


    //    CButton* pRadioBList = (CButton*)GetDlgItem(IDC_BLIST);
    //    CButton* pRadioBTabl = (CButton*)GetDlgItem(IDC_BTABL);
    //    if (pRadioBList)
    //        pRadioBList->ShowWindow(SW_SHOW);
    //    if (pRadioBTabl)
    //        pRadioBTabl->ShowWindow(SW_SHOW);
    //    if (::IsWindow(m_bone.m_hWnd) && pRadioBList->GetCheck())
    //        m_bone.ShowWindow(SW_SHOW);
    //    if (::IsWindow(m_pictureBindBone.m_hWnd) && pRadioBTabl->GetCheck())
    //        m_pictureBindBone.ShowWindow(SW_SHOW);
    //}
}

BOOL KGSFXModelViewPage::OnSetActive()
{
    CPropertyPage::OnSetActive();
    return TRUE;
}

void KGSFXModelViewPage::UpdateInterface()
{
    //OnSize(0, 0, 0);
    return;
}

namespace {

    UINT _FillTree(CTreeCtrl* pTree, HTREEITEM hTreeItem, LPCTSTR szPath)
    {
        if (!pTree)
            return 0;

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
                _FillTree(pTree, pTree->InsertItem(strName.GetBuffer(), hTreeItem), strPath.GetBuffer());
            }
            else
            {
                TCHAR  szName[MAX_PATH];
                strncpy(szName, strName.GetBuffer(), sizeof(szName));
                TCHAR* pszExt = strrchr(szName, '.');
                if (!pszExt)
                    continue;
                if (!stricmp(pszExt, TEXT(".mdl")))
                    pTree->InsertItem(szName, 2, 2, hTreeItem);
                else if (!stricmp(pszExt, TEXT(".mesh")))
                    pTree->InsertItem(szName, 1, 1, hTreeItem);
                else if (!stricmp(pszExt, TEXT(".sfx")))
                    pTree->InsertItem(szName, 3, 3, hTreeItem);
                else if (!stricmp(pszExt, TEXT(".bind")))
                    pTree->InsertItem(szName, 4, 4, hTreeItem);
                else
                    continue;
            }
        }
        fileFind.Close();

        return 1;
    }

    UINT _FillTreeThread(void* pTreeHandel)
    {
        TCHAR path[MAX_PATH];
        sprintf(path, "%s%s", g_szDefWorkDirectory, TEXT("\\Data"));
        _FillTree((CTreeCtrl*)pTreeHandel, NULL, path);
        return 1;
    }
}

int GetImageIndex(const char szType[])
{
	if (!stricmp(szType, TEXT("mdl")))
		return 2;
	else if (!stricmp(szType, TEXT("mesh")))
		return 1;
	else if (!stricmp(szType, TEXT("sfx")))
		return 3;
	else if (!stricmp(szType, TEXT("bind")))
		return 4;
	else
		return 0;
}


void _FillTreeFromFileMap(
	CTreeCtrl* pTree, HTREEITEM hParent, IIniFile* pMap, const char szString[], int Image)
{
	if (!pTree || !pMap)
		return;

	char szSection[32] = TEXT("");
	char szKeyName[32] = TEXT("");
	char szKeyData[32] = TEXT("");
	char szKeyType[32] = TEXT("");

	pMap->GetString(TEXT("Corr"), szString, TEXT(""), szSection, sizeof(szSection));
	HTREEITEM hTree = pTree->InsertItem(szSection, Image, Image, hParent);

	if (!Image)
	{
		while (pMap->GetNextKey(szString, szKeyName, szKeyName))
		{
			pMap->GetString(szString, szKeyName, TEXT(""), szKeyData, sizeof(szKeyData));
			pMap->GetString(TEXT("Type"), szKeyData, TEXT(""), szKeyType, sizeof(szKeyType));
			_FillTreeFromFileMap(pTree, hTree, pMap, szKeyData, GetImageIndex(szKeyType));
		}
	}
}


int KGSFXModelViewPage::FillTreeFromFileMap()
{
	int nResult  = false;
	int nRetCode = false;

	char szSection[32] = TEXT("");
	char szKeyName[32] = TEXT("");
	char szKeyData[32] = TEXT("");
	char szKeyType[32] = TEXT("");

	TCHAR szMapFileName[MAX_PATH];
	sprintf(szMapFileName, "%s%s", g_szDefWorkDirectory, TEXT("\\sfx_editor_file_map.ini"));

	m_tree.DeleteAllItems();
	IIniFile* pMapFile = NULL;
	pMapFile = g_OpenIniFile(szMapFileName);

	KG_PROCESS_ERROR(pMapFile);

	while (pMapFile->GetNextKey(TEXT("Main"), szKeyName, szKeyName))
	{
		pMapFile->GetString(TEXT("Main"), szKeyName, TEXT(""), szKeyData, sizeof(szKeyData));
		pMapFile->GetString(TEXT("Type"), szKeyData, TEXT(""), szKeyType, sizeof(szKeyType));
		_FillTreeFromFileMap(&m_tree, NULL, pMapFile, szKeyData, GetImageIndex(szKeyType));
	}

	nResult = true;
Exit0:
	SAFE_RELEASE(pMapFile);
	return nResult;
}


BOOL KGSFXModelViewPage::OnInitDialog()
{
    int nResult = CPropertyPage::OnInitDialog();

    m_tree.SetTextColor(RGB(255, 255, 255));
    m_tree.SetLineColor(RGB(122, 122, 122));
    m_tree.SetBkColor(RGB(92,  92, 92));

    m_imageListTree.Create(16, 16, ILC_COLOR24, 5, 5);
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_FC));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_GLEAN));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_TALK));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_OFFER));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_POS));
    m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_BN));

    m_tree.SetImageList(&m_imageListTree, TVSIL_NORMAL);



	//FillTreeFromFileMap();

    //TCHAR path[MAX_PATH];
    //sprintf(path, "%s%s", g_szDefWorkDirectory, TEXT("\\Data"));
    ////FillTree(NULL, path);
    //m_pFillThread = AfxBeginThread(_FillTreeThread, (void*)&m_tree);



    //m_pictureBindBone.Create(IDD_DIALOG_BIND_TAB, this);
    //((CButton*)GetDlgItem(IDC_BLIST))->SetCheck(TRUE);
    //m_pictureBindBone.ShowWindow(SW_HIDE);


    return nResult;
}


//void _GetKeyName(IIniFile* pMapFile, const TCHAR szSection[], TCHAR* pKeyName)
//{
//	int nIndex = 0;
//	
//	TCHAR szKeyName[32] = TEXT("");
//	if (pMapFile && pKeyName)
//	{
//		while (pMapFile)
//	}
//}


int KGSFXModelViewPage::FillTree(HTREEITEM hTreeItem, LPCTSTR szPath, int* pIndex)
{
    int nResult  = false;
    int nRetCode = false;

    TCHAR szFilePath[MAX_PATH];
    sprintf(szFilePath, "%s%s", szPath, TEXT("\\*.*"));
    CFileFind fileFind;
    BOOL bWorking = fileFind.FindFile(szFilePath);

	int index = 0;
	TCHAR szKey[8];
	TCHAR szVal[8];

    while (bWorking)
    {
		itoa(index, szKey, 10);
		itoa(*pIndex, szVal, 10);

        bWorking = fileFind.FindNextFile();
        if (fileFind.IsDots())
            continue;
        CString strPath = fileFind.GetFilePath();
        CString strName = fileFind.GetFileName();
        if (fileFind.IsDirectory())
        {
			if (strName == TEXT(".svn") || strName == TEXT("maps") || strName == TEXT("Texture"))
				continue;
			/*if (pMapFile)
			{
				if (hTreeItem)
				{
					TCHAR szSec[32];
					int IndexParent = (int)m_tree.GetItemData(hTreeItem);
					itoa(IndexParent, szSec, 10);

					pMapFile->WriteString(szSec, szKey, szVal);
					pMapFile->WriteString(TEXT("Corr"), szVal, strName.GetBuffer());
					pMapFile->WriteString(TEXT("Type"), szVal, TEXT("folder"));
				}
				else
				{
					pMapFile->WriteString(TEXT("Main"), szKey, szVal);
					pMapFile->WriteString(TEXT("Corr"), szVal, strName.GetBuffer());
					pMapFile->WriteString(TEXT("Type"), szVal, TEXT("folder"));
				}
			}*/

			HTREEITEM hTree = m_tree.InsertItem(strName.GetBuffer(), hTreeItem);
			//m_tree.SetItemData(hTree, (DWORD_PTR)(*pIndex));
			//(*pIndex)++;
            FillTree(hTree, strPath.GetBuffer(), pIndex);
        }
        else
        {
			HTREEITEM hTree;
            TCHAR  szName[MAX_PATH];
            strncpy(szName, strName.GetBuffer(), sizeof(szName));
            TCHAR* pszExt = strrchr(szName, '.');
            if (!pszExt)
                continue;
            if (!stricmp(pszExt, TEXT(".mdl")))
                hTree = m_tree.InsertItem(szName, 2, 2, hTreeItem);
            else if (!stricmp(pszExt, TEXT(".mesh")))
                hTree = m_tree.InsertItem(szName, 1, 1, hTreeItem);
            else if (!stricmp(pszExt, TEXT(".sfx")))
                hTree = m_tree.InsertItem(szName, 3, 3, hTreeItem);
            else if (!stricmp(pszExt, TEXT(".bind")))
                hTree = m_tree.InsertItem(szName, 4, 4, hTreeItem);
            else
                continue;

            char fileName[MAX_PATH];
            g_GetFilePathFromFullPath(fileName, strPath.GetBuffer());
            strPath.ReleaseBuffer();
            _strlwr(fileName);

            m_ModelInfo.push_back(ModelInfoPair(fileName, hTree));


			//m_tree.SetItemData(hTree, (DWORD_PTR)(*pIndex));

			/*if (pMapFile)
			{				
				if (hTreeItem)
				{
					TCHAR szSec[32];
					int IndexParent = (int)m_tree.GetItemData(hTreeItem);
					itoa(IndexParent, szSec, 10);

					pMapFile->WriteString(szSec, szKey, szVal);
					pMapFile->WriteString(TEXT("Corr"), szVal, szName);
					pMapFile->WriteString(TEXT("Type"), szVal, ++pszExt);
				}
				else
				{
					pMapFile->WriteString(TEXT("Main"), szKey, szVal);
					pMapFile->WriteString(TEXT("Corr"), szVal, szName);
					pMapFile->WriteString(TEXT("Type"), szVal, ++pszExt);
				}
			}*/

        }

		//index++;
		//(*pIndex)++;
    }
    fileFind.Close();
    nResult = true;
//Exit0:
    return nResult;
}

CString KGSFXModelViewPage::GetSelMdlName()
{
    return m_strSelPath;
}


namespace {

    inline BOOL CheckModelName(LPCTSTR pFileName)
    {
        TCHAR szNameLw[MAX_PATH];
        _tcsncpy(szNameLw, pFileName, sizeof(szNameLw));

        _tcslwr(szNameLw);

        if (
            _tcsstr(szNameLw, TEXT(".mesh")) ||
            _tcsstr(szNameLw, TEXT(".mdl"))  ||
            _tcsstr(szNameLw, TEXT(".sfx")) 
            )
            return TRUE;
        return FALSE;
    }
}

void KGSFXModelViewPage::UpdateBindModel()
{
    GET_SFX_EDITOR();
    TCHAR szFullPath[MAX_PATH];
    g_GetFullPath(szFullPath, m_strSelPath.GetBuffer());
    if (CheckModelName(szFullPath))
        pScene->UpdateBindOrRefrenceModel(szFullPath);
}

void KGSFXModelViewPage::UpdateMdelAin()
{
    GET_SFX_EDITOR();
    TCHAR szFullPath[MAX_PATH];
    g_GetFullPath(szFullPath, m_strAinPath.GetBuffer());
    pScene->UpdateBindOrRefrenceAni(szFullPath);
}


void KGSFXModelViewPage::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    HTREEITEM hTreeItem = m_tree.GetSelectedItem();
    CString strPath = TEXT("");
    KG_PROCESS_ERROR(hTreeItem);
    while (hTreeItem)
    {
        strPath =
            TEXT("\\") + m_tree.GetItemText(hTreeItem) + strPath;
        hTreeItem = m_tree.GetParentItem(hTreeItem);
    }

    m_strSelPath = m_strStartPath + strPath;

    // update scene info
    UpdateBindModel();

    // update combox ani list
    //FillComb();

    // update bone list
    //FillBone();

    //
    InitMdlAin();


Exit0:
    *pResult = 0;
}

int KGSFXModelViewPage::FillComb()
{
    int nResult  = false;

    TCHAR  szFilePath[MAX_PATH];
    TCHAR  szFullPath[MAX_PATH];
    TCHAR* pExt = NULL;
    CFileFind fileFind;

    g_GetFullPath(szFullPath, m_strSelPath.GetBuffer());
    pExt = strrchr(szFullPath, '\\');
    KG_PROCESS_ERROR(pExt);
    *pExt = '\0';

    m_comb.ResetContent();
    sprintf(szFilePath, "%s%s", szFullPath, TEXT("\\*.*"));
    BOOL bWorking = fileFind.FindFile(szFilePath);
    while (bWorking)
    {
        bWorking = fileFind.FindNextFile();
        if (fileFind.IsDots())
            continue;
        CString strPath = fileFind.GetFilePath();
        CString strName = fileFind.GetFileName();
        if (fileFind.IsDirectory())
            continue;

        TCHAR  szName[MAX_PATH];
        strncpy(szName, strName.GetBuffer(), sizeof(szName));
        TCHAR* pszExt = strrchr(szName, '.');
        if (!pszExt)
            continue;
        if (!stricmp(pszExt, TEXT(".ani")))
            m_comb.AddString(szName);
        else
            continue;

    }
    m_comb.AddString(TEXT("浏览......"));
    fileFind.Close();

    nResult = true;
Exit0:
    return nResult;
}

void KGSFXModelViewPage::InitMdlAin()
{
    //GET_SFX_EDITOR();
    //if (m_comb.GetCount() >= 1 && pScene->m_pBindModel)
    //{
    //    m_comb.SetCurSel(0);
    //    OnCbnSelchangeComboAni();
    //    pScene->m_pBindModel->GetAnimationController(ANICTL_PRIMARY)()->PauseAnimation(TRUE);
    //}

//Exit0:
    return;
}

//LRESULT KGSFXModelViewPage::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//    if (message == WM_NCDESTROY)
//    {
//        m_bExitSearchThread = true;
//        while (!m_bThreadExit)
//        {
//            Sleep(100);
//        }
//    }
//
//    return CPropertyPage::WindowProc(message, wParam, lParam);
//}

void KGSFXModelViewPage::OnNMClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    //HTREEITEM     hTreeItem  = NULL;
    //IEKG3DModel*    pBindModel = NULL;
    //KG3DSFXSuede* pSFXSuede  = NULL;
    //IKG3DModel*   pSFXMDL    = NULL;
    //int    nCurSFXIndex      = 0;
    //CString strBoneName = TEXT("");
    //HRESULT       hResult    = E_FAIL;
    //
    //GET_SFX_EDITOR();

    //// bind model
    //pBindModel = pScene->m_pBindModel;
    //KG_PROCESS_ERROR(pBindModel);

    //// bind bone
    //hTreeItem = m_bone.GetSelectedItem();
    //KG_PROCESS_ERROR(hTreeItem);
    //strBoneName = m_bone.GetItemText(hTreeItem);
    //if (strBoneName == TEXT("原点"))
    //    strBoneName  = TEXT("");

    //// suede
    //pSFXSuede = pScene->GetSFXSuede();
    //KG_PROCESS_ERROR(pSFXSuede);

    //// sfx
    //nCurSFXIndex = pScene->GetCurSfxBindIndex();
    //pSFXMDL = pSFXSuede->GetSFX(nCurSFXIndex);
    //if (!pSFXMDL)
    //{
    //    ::MessageBox(
    //        m_hWnd,
    //        TEXT("你还没有选择好特效呢>_<!!"),
    //        TEXT("Warning"),
    //        MB_OK | MB_ICONWARNING
    //     );
    //    KG_PROCESS_ERROR(false);
    //}

    //hResult = pScene->UnBindSuedeSFX();
    //KG_COM_PROCESS_ERROR(hResult);

    //pScene->BindSuedeSFX(strBoneName.GetBuffer());
    //KG_COM_PROCESS_ERROR(hResult);

    //::MessageBox(
    //    m_hWnd,
    //    TEXT("绑定成功>_<!!"),
    //    TEXT("Success!!"),
    //    MB_OK | MB_ICONWARNING
    //);

    //GET_SFX_BIND_PAGE();
    //pBindPage->UpdateBondInfo(strBoneName.GetBuffer());

//Exit0:
    *pResult = 0;
}

//
//void KGSFXModelViewPage::FillBone()
//{
//    int nResult  = false;
//    int nRetCode = false;
//    int nCount = 0;
//    IKG3DModel*	 pModelInVector = NULL;
//
//    GET_SFX_EDITOR();
//    IEKG3DModel*	 pModel = NULL;
//    IEKG3DModelST *pMdlST = NULL;
//    //IEKG3DMeshBase *pMeshBase = NULL;
//    IEKG3DMesh *pMesh = NULL;
//
//    pScene->GetBindModel(&pModel);
//    KG_PROCESS_ERROR(pModel);
//
//    pModel->GetIEKG3DModelST(&pMdlST);
//
//    if (pMdlST)
//    {
//        m_bone.DeleteAllItems();
//        m_bone.InsertItem(TEXT("原点"), 5, 5);
//
//        nCount = pModel->GetNumModel();
//
//        for (int s = 0; s < nCount; s++)
//        {
//            IEKG3DModel* pieModel = NULL;
//			pModel->GetModel(s, &pModelInVector);
//			
//			pieModel = dynamic_cast<IEKG3DModel*>(pModelInVector);
//            pieModel->GetIEKG3DMesh(&pMesh);
//            if (!pMesh)
//                continue;
//            int nNumBone = 0;
//            pMesh->GetNumBones(&nNumBone);
//            for (int i = 0; i < nNumBone; i++)
//            {
//                LPCTSTR pszName = NULL;
//                pMesh->GetBoneInfoName(i, &pszName);
//                m_bone.InsertItem(pszName, 5, 5);
//            }
//        }
//    }
//    else
//    {
//        pModel->GetIEKG3DMesh(&pMesh);
//        if (pMesh)
//        {
//            int nNumBone = 0;
//            pMesh->GetNumBones(&nNumBone);
//            m_bone.DeleteAllItems();
//            m_bone.InsertItem(TEXT("原点"), 5, 5);
//            for (int i = 0; i < nNumBone; i++)
//            {
//                LPCTSTR pszName = NULL;
//                pMesh->GetBoneInfoName(i, &pszName);
//                m_bone.InsertItem(pszName, 5, 5);
//            }
//        }
//    }
//
//    nResult = true;
//Exit0:
//    return;
//}
void KGSFXModelViewPage::OnBnClickedBlist()
{
    int nCheck = ((CButton*)GetDlgItem(IDC_BLIST))->GetCheck();
    if (nCheck)
    {
        m_pictureBindBone.ShowWindow(SW_HIDE);
        //m_bone.ShowWindow(SW_SHOW);
    }
    else
    {
        m_pictureBindBone.ShowWindow(SW_SHOW);
      //  m_bone.ShowWindow(SW_HIDE);
    }
}

void KGSFXModelViewPage::OnBnClickedBtabl()
{
    int nCheck = ((CButton*)GetDlgItem(IDC_BTABL))->GetCheck();
    if (nCheck)
    {
        m_pictureBindBone.ShowWindow(SW_SHOW);
        //m_bone.ShowWindow(SW_HIDE);
    }
    else
    {
        m_pictureBindBone.ShowWindow(SW_HIDE);
       // m_bone.ShowWindow(SW_SHOW);
    }
}

void KGSFXModelViewPage::OnBnClickedButtonRefush()
{
	//TCHAR szMapFileName[MAX_PATH];
	TCHAR path[MAX_PATH];

	sprintf(path, "%s%s", g_szDefWorkDirectory, m_strStartPath);
	//sprintf(szMapFileName, "%s%s", g_szDefWorkDirectory, TEXT("\\sfx_editor_file_map.ini"));
	
    m_ModelInfo.clear();
	m_tree.DeleteAllItems();

    m_bExitSearchThread = true;
    while (!m_bThreadExit)
    {
        Sleep(100);
    }

	int Index = 0;
	FillTree(NULL, path, &Index);

    m_Progress.SetRange32(0, (int)m_ModelInfo.size());

    m_hSearch = (HANDLE)_beginthread(SearchThread, 0, this);
    m_bExitSearchThread = false;

}

void KGSFXModelViewPage::OnEnChangeEditSearch()
{
    CString strText;
    m_EditSearch.GetWindowText(strText);
    strText.MakeLower();
    m_strSearch = strText.GetBuffer();
    strText .ReleaseBuffer();
    m_bResearch = true;
}

void KGSFXModelViewPage::OnLbnSelchangeListSearch()
{
    int nSel = m_listSearch.GetCurSel();

    if (nSel == LB_ERR)
        return;

    HTREEITEM hTree = (HTREEITEM)m_listSearch.GetItemData(nSel);
    m_tree.Select(hTree, TVGN_CARET);
}

void KGSFXModelViewPage::OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    HTREEITEM hTreeItem = m_tree.GetSelectedItem();
    CString strPath = TEXT("");
    KG_PROCESS_ERROR(hTreeItem);
    while (hTreeItem)
    {
        strPath =
            TEXT("\\") + m_tree.GetItemText(hTreeItem) + strPath;
        hTreeItem = m_tree.GetParentItem(hTreeItem);
    }

    m_strSelPath = m_strStartPath + strPath;

    char* fileName = m_strSelPath.GetBuffer();

    if (g_IsFileExist(fileName))
    {
        char fullPath[MAX_PATH];
        g_GetFullPath(fullPath, fileName);
        AfxGetApp()->OpenDocumentFile(fullPath);
    }

    m_strSelPath.ReleaseBuffer();
    

Exit0:
    *pResult = 0;
}

void KGSFXModelViewPage::OnClose()
{
    m_bExitSearchThread = true;
    while (!m_bThreadExit)
    {
        Sleep(100);
    }
}
