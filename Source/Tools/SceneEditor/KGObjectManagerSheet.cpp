////////////////////////////////////////////////////////////////////////////////
//
//  FileName  : KGObjectManagerSheet.cpp
//  Author    : ZhaoXinyu (Chiyer)
//  CopyRight : Kingsoft Corporation All rights Reserved
//  History   : 2005.9.21 19:31 Create
//  Comment   : Sword3 Object Property Sheet
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGObjectManagerSheet.h"
//#include "KG3dsceneobjecteditor.h"
#include "KObjectEditorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KGObjectManagerSheet, CPropertyPage)

KGObjectManagerSheet::KGObjectManagerSheet()
	: CPropertyPage(KGObjectManagerSheet::IDD)
{
}

KGObjectManagerSheet::~KGObjectManagerSheet()
{
}

void KGObjectManagerSheet::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_OBJECT_MANAGE_PAGE,   m_tabObjectManageSheet);
    DDX_Control(pDX, IDC_OBJECT_PROPERTY_TREE, m_treeMeshView);
    DDX_Control(pDX, IDC_OBJECT_PROPERTY_LIST, m_listProperty);
}


BEGIN_MESSAGE_MAP(KGObjectManagerSheet, CPropertyPage)
    ON_NOTIFY(TCN_SELCHANGE,   IDC_OBJECT_MANAGE_PAGE,   OnTcnSelchangeObjectManagePage)
    ON_NOTIFY(TVN_SELCHANGED,  IDC_OBJECT_PROPERTY_TREE, OnTvnSelchangedObjectPropertyTree)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_OBJECT_PROPERTY_LIST, OnLvnItemchangedObjectPropertyList)
    ON_NOTIFY(HDN_ITEMCLICK, 0, OnHdnItemclickObjectPropertyList)
    ON_NOTIFY(LVN_COLUMNCLICK, IDC_OBJECT_PROPERTY_LIST, OnLvnColumnclickObjectPropertyList)
    ON_NOTIFY(NM_RCLICK, IDC_OBJECT_PROPERTY_TREE, OnNMRclickObjectPropertyTree)
    ON_NOTIFY(NM_DBLCLK, IDC_OBJECT_PROPERTY_LIST, OnNMDblclkObjectPropertyList)
END_MESSAGE_MAP()

void KGObjectManagerSheet::OnTcnSelchangeObjectManagePage(NMHDR *pNMHDR, LRESULT *pResult)
{
    int nSel = m_tabObjectManageSheet.GetCurSel();
    m_treeMeshView.DeleteAllItems();

    char szText[128] = {0};
    TCITEM tcItem;

    tcItem.mask       = TCIF_TEXT;
    tcItem.pszText    = szText;
    tcItem.cchTextMax = sizeof(szText);

    m_tabObjectManageSheet.GetItem(nSel, &tcItem);

    CString szPath(OBJECT_PATH);
    //szPath = szPath + tcItem.pszText + "\\";
    szPath.Delete(0, 2);
    szPath = CString(g_szDefWorkDirectory) + szPath + tcItem.pszText + "\\";

    m_szCurSheet = tcItem.pszText;

    GetRootPoint(szPath);
    GetChildPoint(m_treeRoot, szPath, GD_FIRST);

    m_treeMeshView.Expand(TVI_ROOT, TVE_EXPAND);

    *pResult = 0;
}

int KGObjectManagerSheet::GetChildPoint(HTREEITEM hParent, CString szPath, ENUM_GETDIR nType)
{
    HTREEITEM hChild;
    if (nType == GD_FIRST)
    {
        hChild = hParent;
    }
    else
    {
        hChild = m_treeMeshView.GetChildItem(hParent);
    }

    while(hChild)
    {
        CString strText = m_treeMeshView.GetItemText(hChild);
        if(strText.Right(1) != "\\")
        {
            strText += _T("\\");
        }

        CFileFind file;
        BOOL bContinue = file.FindFile(szPath + strText + "*.*");

        while(bContinue)
        {
            bContinue = file.FindNextFile();

            if (file.IsDirectory() && !file.IsDots())
            {
                m_treeMeshView.InsertItem(file.GetFileName(), hChild);
            }
            else if(file.GetFileName().Right(4) == ".ini")
            {
                CString szTemp =
                    "物件配置文件:(" + file.GetFileName() + ")没有物件名字段";
                IIniFile* pFile = g_OpenIniFile(file.GetFilePath());
                char szName[MAX_TEXT];
                pFile->GetString(
                    "caption", "szName", szTemp.GetBuffer(),
                    szName, sizeof(szName)
                );
                m_treeMeshView.InsertItem(szName, hChild);
                pFile->Release();
            }
        }

        file.Close();
        GetChildPoint(hChild, szPath + strText, GD_OTHER);
        hChild = m_treeMeshView.GetNextItem(hChild, TVGN_NEXT);
    }

    return TRUE;
}

int KGObjectManagerSheet::GetRootPoint(CString szPath)
{
    CFileFind file;
    BOOL bContinue = file.FindFile(szPath + "*");

    int temp = 0;
    while(bContinue)
    {
        bContinue = file.FindNextFile();
        if(file.IsDirectory() && !file.IsDots())
        {
            if (temp == 0)
            {
                m_treeRoot = m_treeMeshView.InsertItem(file.GetFileName());
                temp = 1;
            }
            else
            {
                m_treeMeshView.InsertItem(file.GetFileName());
            }
        }
    }

    file.Close();

    return TRUE;
}

BOOL KGObjectManagerSheet::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    CString szPath(OBJECT_PATH);
    szPath.Delete(0, 2);
    CString szPathHead = g_szDefWorkDirectory + szPath + "*";

    CFileFind file;
    BOOL bContinue = file.FindFile(szPathHead);

    int nIntex = 0;
    while(bContinue)
    {
        bContinue = file.FindNextFile();
        if(file.IsDirectory() && !file.IsDots())
        {
            m_tabObjectManageSheet.InsertItem(nIntex++, file.GetFileName());
        }
    }

    DWORD dwStyle = GetWindowLong(m_treeMeshView.m_hWnd,GWL_STYLE);
    dwStyle |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
    SetWindowLong(m_treeMeshView.m_hWnd,GWL_STYLE,dwStyle);

    char szText[128] = {0};
    TCITEM tcItem;

    tcItem.mask       = TCIF_TEXT;
    tcItem.pszText    = szText;
    tcItem.cchTextMax = sizeof(szText);

    m_tabObjectManageSheet.GetItem(0, &tcItem);

    m_szCurSheet = tcItem.pszText;

    CString szPathRoot = g_szDefWorkDirectory + szPath + tcItem.pszText + "\\";
    GetRootPoint(szPathRoot);
    GetChildPoint(m_treeRoot, szPathRoot, GD_FIRST);

    m_treeMeshView.Expand(TVI_ROOT, TVE_EXPAND);

	//m_listProperty.SetItemHeight(16);
    m_listProperty.InsertColumn(0,"物件属性名");
    m_listProperty.InsertColumn(1,"物件属性值");
    m_listProperty.SetColumnWidth(0,160);
    m_listProperty.SetColumnWidth(1,360);

    file.Close();

    return TRUE;
}

/*
int KGObjectManagerSheet::UpdateDataObjectProperty(NPC_PROPERTY* lpNpc)
{
    OBJECT_PROPERTY* lpObj = lpNpc;
    UpdateDataObjectProperty(lpObj);
    return TRUE;
}
*/

/*
int KGObjectManagerSheet::UpdateDataObjectProperty(OBJECT_PROPERTY* lpObj)
{
    m_listProperty.DeleteAllItems();

    CString szTemp;
    return TRUE;
}
*/

void KGObjectManagerSheet::OnTvnSelchangedObjectPropertyTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

    int nSel = m_tabObjectManageSheet.GetCurSel();
    char szText[128] = {0};

    TCITEM tcItem;
    tcItem.mask       = TCIF_TEXT;
    tcItem.pszText    = szText;
    tcItem.cchTextMax = sizeof(szText);

    m_tabObjectManageSheet.GetItem(nSel, &tcItem);

    HTREEITEM hItem = m_treeMeshView.GetSelectedItem();
    CString szName  = m_treeMeshView.GetItemText(hItem);

    IEKG3DScene* pScene = m_lpSceneObjectEditorDoc->GetScene();
    IEKG3DSceneObjectEditor* pEditor = NULL;
	pScene->GetSceneObjectEditor(&pEditor);
        //static_cast<KG3DSceneObjectEditor*>(pScene);
	_ASSERTE(&pEditor);

    DWORD i = 0;
    IEKG3DEngineManager *piManager = g_GetEngineIEInterface();
    if (m_szCurSheet == "NPC")
    {
        /*//by dengzhihui Refactor 2006年12月8日
        NPC_PROPERTY* lpCurNpc = NULL;
                DWORD dwNPCCount = 0;
                piManager->GetRepresentNPCPropertyCount(&dwNPCCount);
                for (i = 0; i < dwNPCCount; ++i)
                {
                    piManager->GetRepresentNPCProperty(i, &lpCurNpc);
                    ASSERT(lpCurNpc);
                    if (strcmp(szName, lpCurNpc->szName) == 0)
                    {
                        pEditor->OpenObject(lpCurNpc->szModelFile);
                        //UpdateDataObjectProperty(lpCurNpc);
                        m_nCurIndex = i;
                        break;
                    }
                }*/

    }
    else if (m_szCurSheet == "物件")
    {
        /*//by dengzhihui Refactor 2006年12月8日
        DWORD dwObjectCount = 0;
                OBJECT_PROPERTY* lpCurObj = NULL;
                piManager->GetRepresentObjectPropertyCount(&dwObjectCount);
                for (DWORD i = 0; i < dwObjectCount; ++i)
                {
                    piManager->GetRepresentObjectProperty(i, &lpCurObj);
                    ASSERT(lpCurObj);
                    if (strcmp(szName, lpCurObj->szName) == 0)
                    {
                        pEditor->OpenObject(lpCurObj->szModelFile);
                        //UpdateDataObjectProperty(lpCurObj);
                        m_nCurIndex = i;
                        break;
                    }
                }*/

    }
    /*
    while(hItem = m_treeMeshView.GetParentItem(hItem))
    {
        szName = m_treeMeshView.GetItemText(hItem) + "\\" + szName;
    }
    szName = OBJECT_PATH + CString(tcItem.pszText) + "\\" + szName;

    KG3DScene* pScene = m_lpSceneObjectEditorDoc->GetScene();
    KG3DSceneObjectEditor* pEditor = static_cast<KG3DSceneObjectEditor*>(pScene);
    pEditor->OpenObject(szName.GetBuffer());
*/
    *pResult = 0;
}

void KGObjectManagerSheet::OnLvnItemchangedObjectPropertyList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    *pResult = 0;
}

void KGObjectManagerSheet::OnHdnItemclickObjectPropertyList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
    *pResult = 0;
}

void KGObjectManagerSheet::OnLvnColumnclickObjectPropertyList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    *pResult = 0;
}

void KGObjectManagerSheet::OnNMRclickObjectPropertyTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    //AfxMessageBox("Love Love");
    *pResult = 0;
}

void KGObjectManagerSheet::OnNMDblclkObjectPropertyList(NMHDR *pNMHDR, LRESULT *pResult)
{
	/*
    POSITION pos = m_listProperty.GetFirstSelectedItemPosition();
    int nItem = m_listProperty.GetNextSelectedItem(pos);
    if (nItem != -1)
    {
        CString szSelName   = m_listProperty.GetItemText(nItem,0);
        CString szSelValue  = m_listProperty.GetItemText(nItem,1);
        DWORD dwValueType   = m_listProperty.GetItemData(nItem);

        if (m_szCurSheet == "NPC")
        {
            KGObjectPropertyEdit dlgEdit(
                szSelName, szSelValue, dwValueType, m_nCurIndex, szSelName,
                &g_cNpeTemplate,
            );
            dlgEdit.DoModal();

            UpdateDataObjectProperty(g_cNpeTemplate.m_vecObjectProperty[m_nCurIndex]);
            KG3DScene* pScene = m_lpSceneObjectEditorDoc->GetScene();
            KG3DSceneObjectEditor* pEditor =
                static_cast<KG3DSceneObjectEditor*>(pScene);
            pEditor->OpenObject(
                g_cNpeTemplate.m_vecObjectProperty[m_nCurIndex]->szModelFile
            );
        }
        else if (m_szCurSheet == "物件")
        {
            KGObjectPropertyEdit dlgEdit(
                szSelName, szSelValue, dwValueType, m_nCurIndex, szSelName,
                &g_cObjectTemplate
            );
            dlgEdit.DoModal();

            UpdateDataObjectProperty(g_cObjectTemplate.m_vecObjectProperty[m_nCurIndex]);
            KG3DScene* pScene = m_lpSceneObjectEditorDoc->GetScene();
            KG3DSceneObjectEditor* pEditor =
                static_cast<KG3DSceneObjectEditor*>(pScene);
            pEditor->OpenObject(
                g_cObjectTemplate.m_vecObjectProperty[m_nCurIndex]->szModelFile
           );
        }

    }
    *pResult = 0;
*/
}
