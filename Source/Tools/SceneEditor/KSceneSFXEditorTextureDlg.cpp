////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorTextureDlg.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-9-5 9:16:16
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSFXEditorTextureDlg.h"
#include "IEKG3DSceneSFXEditor.h"
#include "KSceneSFXEditorFrame.h"
#include "IEKG3DSFX.h"
#include "IEKG3DTexture.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KSceneSFXEditorTextureDlg, CPropertyPage)

KSceneSFXEditorTextureDlg::KSceneSFXEditorTextureDlg()
    : CPropertyPage(KSceneSFXEditorTextureDlg::IDD)
{
    m_nSelect = -1;
}

KSceneSFXEditorTextureDlg::~KSceneSFXEditorTextureDlg()
{
}

void KSceneSFXEditorTextureDlg::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_LIST_TEXTURE, m_listTexture);
    DDX_Control(pDX, IDC_EDIT_CUT_NUM, m_editCutNum);
    DDX_Control(pDX, IDC_SFX_TEX_ADD, m_buttonAdd);
    DDX_Control(pDX, IDC_SFX_TEX_DEL, m_buttonDel);
}


BEGIN_MESSAGE_MAP(KSceneSFXEditorTextureDlg, CPropertyPage)
    ON_WM_SIZE()
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TEXTURE, &KSceneSFXEditorTextureDlg::OnLvnItemchangedListTexture)
    ON_NOTIFY(NM_CLICK, IDC_LIST_TEXTURE, &KSceneSFXEditorTextureDlg::OnNMClickListTexture)
    ON_NOTIFY(LVN_ITEMACTIVATE, IDC_LIST_TEXTURE, &KSceneSFXEditorTextureDlg::OnLvnItemActivateListTexture)
    ON_EN_SETFOCUS(IDC_EDIT_CUT_NUM, &KSceneSFXEditorTextureDlg::OnEnSetfocusEditCutNum)
    ON_EN_KILLFOCUS(IDC_EDIT_CUT_NUM, &KSceneSFXEditorTextureDlg::OnEnKillfocusEditCutNum)
    ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_TEXTURE, &KSceneSFXEditorTextureDlg::OnLvnKeydownListTexture)
    ON_BN_CLICKED(IDC_SFX_TEX_ADD, OnButtonClickAdd)
    ON_BN_CLICKED(IDC_SFX_TEX_DEL, OnButtonClickDel)
END_MESSAGE_MAP()

BOOL KSceneSFXEditorTextureDlg::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    m_listTexture.InsertColumn(0, "属性名");
    m_listTexture.InsertColumn(1, "属性值");
    m_listTexture.SetColumnWidth(0, 80);
    m_listTexture.SetColumnWidth(1, 400);
    m_listTexture.SetBkColor(RGB(92, 92, 92));

    m_listTexture.SetupToolTip(200, RGB(68, 68, 68), RGB(255, 255, 255));
    m_listTexture.SetHoverTime(100);

    m_editCutNum.ShowWindow(SW_HIDE);
    m_editCutNum.SetParent(&m_listTexture);

    m_nSelect = 0;

    CreatePreViewWnd();

    return TRUE;
}

void KSceneSFXEditorTextureDlg::CreatePreViewWnd()
{
    if (!GetParent() || !GetParent()->GetParent())
        return;

    IEKG3DSceneSFXEditor* pScene = ((KGSFXModelViewSheet*)GetParent()->GetParent())->GetSfxScene();

    if (!pScene)
        return;
     
     RECT rect;
     GetClientRect(&rect);

     rect.bottom -= 50;
     rect.top = rect.bottom - 200;
     rect.left += 2;
     rect.right -= 2;

     m_wndMinView.Create(rect, this, pScene);
     PostMessage(WM_SIZE);
}

BOOL KSceneSFXEditorTextureDlg::OnSetActive()
{
    if (!::IsWindow(m_wndMinView.m_hWnd))
        CreatePreViewWnd();
    UpdateUI();
    return CPropertyPage::OnSetActive();
}

void KSceneSFXEditorTextureDlg::UpdateUI()
{
    GET_SFX_EDITOR();

    IEKG3DTexture *piTexture = NULL;
    LPCTSTR pszName = NULL;

    if (!::IsWindow(m_listTexture.m_hWnd))
        return;

    m_listTexture.RemoveAll();

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    int nCnt = (int)pSFX->GetTextureNum();
    for (int i = 0; i < nCnt; ++i)
    {
        IKG3DTexture *pTexture = NULL;

        pSFX->GetIKG3DTexture(i, &pTexture);
        piTexture = static_cast<IEKG3DTexture *>(pTexture);

        LPKGLISTITEM pListTitle = new KGLISTITEM;
        LPKGLISTITEM pListItem  = new KGLISTITEM;
        ITEMTEXT     itemText;
        _ITEMDATE    itemDate;

        itemDate.nIndex = i;
        itemDate.nType  = 0;
        itemText.szItemText.Format(TEXT("TEXTURE_%d"), i);
        pListTitle->arryItemText.Add(itemText);
        pListTitle->SetStructData(&itemDate);
        m_listTexture.InsertDepend(NULL, pListTitle);

        itemDate.nIndex = i;
        itemDate.nType  = 1;
        itemText.szItemText.Format(TEXT("ID"));
        pListItem->arryItemText.Add(itemText);
        itemText.szItemText.Format(TEXT("%d"), i);
        pListItem->arryItemText.Add(itemText);
        pListItem->SetStructData(&itemDate);
        m_listTexture.InsertDepend(pListTitle, pListItem);

        pListItem  = new KGLISTITEM;
        itemDate.nIndex = i;
        itemDate.nType  = 2;
        itemText.szItemText.Format(TEXT("文件路径"));
        pListItem->arryItemText.Add(itemText);
        if (pTexture)
        {
            piTexture->GetName(&pszName);
            itemText.szItemText = pszName;
        }
        else
            itemText.szItemText = TEXT("没有贴图");
        pListItem->arryItemText.Add(itemText);
        pListItem->SetStructData(&itemDate);
        m_listTexture.InsertDepend(pListTitle, pListItem);

        pListItem  = new KGLISTITEM;
        itemDate.nIndex = i;
        itemDate.nType  = 3;
        UINT uCut = 0;
        pSFX->GetTextureCutNum(i, &uCut);
        itemText.szItemText.Format(TEXT("切分"));
        pListItem->arryItemText.Add(itemText);
        itemText.szItemText.Format(TEXT("%d"), uCut);
        pListItem->arryItemText.Add(itemText);
        pListItem->SetStructData(&itemDate);
        m_listTexture.InsertDepend(pListTitle, pListItem);
    }

    if (!nCnt)
        m_nSelect = -1;
    else
        UpdateSelState(TRUE);

    UpdateTexturePreView();
}

void KSceneSFXEditorTextureDlg::UpdateSelState(int nFlag)
{
    LPKGLISTITEM pListTitle = m_listTexture.m_listDataTree.GetHead();
    LPKGLISTITEM pListItem  = NULL;
    _ITEMDATE itemDate;
    COLORREF color;

    KG_PROCESS_ERROR(m_nSelect != -1);
    KG_PROCESS_ERROR(pListTitle);

    pListTitle->GetStructData(&itemDate, sizeof(itemDate));

    while (pListTitle && itemDate.nIndex != m_nSelect)
    {
        pListTitle = pListTitle->pNexSiblingItem;
        pListTitle->GetStructData(&itemDate, sizeof(itemDate));
    }

    KG_PROCESS_ERROR(pListTitle);

    pListItem = pListTitle->pFirstChildItem;

    if (nFlag)
        color = RGB(240, 200, 85);
    else
        color = RGB(255, 255, 255);;
    while (pListItem)
    {
        pListItem->arryItemText[0].colorBack = color;
        pListItem->arryItemText[1].colorBack = color;
        m_listTexture.Update(pListTitle);
        pListItem = pListItem->pNexSiblingItem;
    }

Exit0:
    return;
}

void KSceneSFXEditorTextureDlg::OnButtonClickDel()
{
    GET_SFX_EDITOR();
    KSceneSFXEditorParticlePage* pPage = pFrame->GetPanelView()->GetParticlePage();
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    HRESULT hRetCode = pSFX->RemoveTexture(m_nSelect);
    int nTextNum = pSFX->GetTextureNum();
    KG_COM_PROCESS_ERROR(hRetCode);

    if (m_nSelect > nTextNum - 1)
    {
        if (m_nSelect - 1 >= 0)
        {
            m_nSelect--;
        }
        else
        {
            m_nSelect = 0;
        }
    }
    if (pPage)
        pPage->UpdateParticleKindUI();

Exit0:
    UpdateUI();

    return;
}

void KSceneSFXEditorTextureDlg::OnButtonClickAdd()
{
    GET_SFX_EDITOR();
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    HRESULT hRetCode = pSFX->AddTexture(TEXT(""));
    KG_COM_PROCESS_ERROR(hRetCode);
    UpdateUI();

Exit0:
    return;
}

void KSceneSFXEditorTextureDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    RECT rectClient;
    RECT rectCtrl;
    GetClientRect(&rectClient);

    rectCtrl.left = rectClient.left + 5;
    rectCtrl.right = rectClient.right - 5;
    rectCtrl.bottom = rectClient.bottom - 5;
    rectCtrl.top = rectCtrl.bottom - 200;

    if (::IsWindow(m_wndMinView.m_hWnd))
        m_wndMinView.MoveWindow(&rectCtrl);

    rectCtrl.bottom = rectCtrl.top - 5;
    rectCtrl.top = rectClient.top + 25;

    if (::IsWindow(m_listTexture.m_hWnd))
        m_listTexture.MoveWindow(&rectCtrl);


}

void KSceneSFXEditorTextureDlg::OnLvnItemchangedListTexture(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    *pResult = 0;
}

void KSceneSFXEditorTextureDlg::UpdateTexturePreView()
{
    GET_SFX_EDITOR();    
    
    IKG3DTexture* pTexture = NULL;
    IEKG3DTexture* piTexture = NULL;
    LPCTSTR pszName = NULL;
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    pSFX->GetIKG3DTexture(m_nSelect, &pTexture);
    if (pTexture)
    {
        piTexture = static_cast<IEKG3DTexture *>(pTexture);
        piTexture->GetName(&pszName);
        m_wndMinView.Draw(const_cast<LPSTR>(pszName));
    }
    else
    {
        m_wndMinView.Draw(const_cast<LPSTR>(TEXT("Not Resource.not")));
    }

//Exit0:
    return;
}

void KSceneSFXEditorTextureDlg::OnNMClickListTexture(NMHDR *pNMHDR, LRESULT *pResult)
{
    GET_SFX_EDITOR();
    LPKGLISTITEM pListItem = NULL;
    _ITEMDATE itemDate;
    IKG3DTexture* pTexture = NULL;
    IEKG3DTexture* piTexture = NULL;
    LPCTSTR pszName = NULL;
    int nItem = 0;
    POSITION pos = m_listTexture.GetFirstSelectedItemPosition();
    KG_PROCESS_ERROR(pos);
    nItem    = m_listTexture.GetNextSelectedItem(pos);
    KG_PROCESS_ERROR(nItem != -1);
    pListItem = (LPKGLISTITEM)m_listTexture.GetItemData(nItem);
    KG_PROCESS_ERROR(pListItem);

    pListItem->GetStructData(&itemDate, sizeof(itemDate));

    UpdateSelState(FALSE);
    m_nSelect = itemDate.nIndex;
    UpdateSelState(TRUE);

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    pSFX->GetIKG3DTexture(m_nSelect, &pTexture);
    if (pTexture)
    {
        piTexture = static_cast<IEKG3DTexture *>(pTexture);
        piTexture->GetName(&pszName);
        m_wndMinView.Draw(const_cast<LPSTR>(pszName));
    }
    else
    {
        m_wndMinView.Draw(const_cast<LPSTR>(TEXT("Not Resource.not")));
    }
Exit0:
    *pResult = 0;
}

void KSceneSFXEditorTextureDlg::OnLvnItemActivateListTexture(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    *pResult = 0;
    GET_SFX_EDITOR();

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);

    LPKGLISTITEM pListItem = (LPKGLISTITEM)m_listTexture.GetItemData(pNMIA->iItem);
    _ITEMDATE itemData;
    pListItem->GetStructData(&itemData, sizeof(itemData));

    if (itemData.nType == 2)
    {
        char szFilters[] = "Texture Files|*.TGA;*.BMP;*.JPG;*.DDS;*.PNG|"
                           "(*.TGA)|*.TGA|"
                           "(*.BMP)|*.BMP|"
                           "(*.JPG)|*.JPG|"
                           "(*.DDS)|*.DDS|"
                           "(*.PNG)|*.PNG|"
                           "All Files (*.*)|*.*||";

        CFileDialog fileDlg(
            TRUE,
            NULL,
            NULL,
            OFN_EXPLORER|OFN_FILEMUSTEXIST| OFN_HIDEREADONLY,
            szFilters,
            this
            );

        if (fileDlg.DoModal() == IDOK)
        {
            CString strTextureName = fileDlg.GetPathName();
            char szTexName[MAX_PATH];
            g_GetFilePathFromFullPath(szTexName, strTextureName.GetBuffer());
            strTextureName.ReleaseBuffer();
            if (itemData.nIndex >= 0 && itemData.nIndex < (int)pSFX->GetTextureNum())
            {
                HRESULT hr = pSFX->SetTexture(itemData.nIndex, szTexName);
                if (SUCCEEDED(hr))
                {
                    UpdateUI();
                }
            }
            //else   // 添
            //{
            //    HRESULT hr = pScene->m_lpSFX->AddTexture(strTextureName);
            //    if (SUCCEEDED(hr))
            //        UpdateUI();
            //}
        }
    }
    else if (itemData.nType == 3)
    {
        if (itemData.nIndex >= 0 && itemData.nIndex < (int)pSFX->GetTextureNum())
        {
            UINT uCut = 0;
            HRESULT hr = pSFX->GetTextureCutNum(itemData.nIndex, &uCut);
            if (SUCCEEDED(hr))
            {
                CRect rc;
                m_listTexture.GetSubItemRect(pNMIA->iItem, pNMIA->iSubItem, LVIR_BOUNDS, rc);
                m_editCutNum.MoveWindow(&rc);

                CString str;
                str.Format("%d", uCut);
                m_editCutNum.SetWindowText(str);
                m_editCutNum.SetFocus();
                m_editCutNum.ShowWindow(SW_NORMAL);
                m_editCutNum.SetSel(0, -1);

                m_nSelect = itemData.nIndex;
            }
        }
    }
}

void KSceneSFXEditorTextureDlg::OnEnSetfocusEditCutNum()
{
    // TODO: Add your control notification handler code here
}

void KSceneSFXEditorTextureDlg::OnEnKillfocusEditCutNum()
{
    m_editCutNum.ShowWindow(SW_HIDE);
    GET_SFX_EDITOR();

    CString strName;
    m_editCutNum.GetWindowText(strName);
    int nCut = ::atoi(strName.GetBuffer());
    if (nCut > 0 && m_nSelect != -1)
    {
        IEKG3DSFX *pSFX = NULL;
        pScene->GetSFX(&pSFX);
        pSFX->SetTextureCutNum(m_nSelect, (UINT)nCut);

        //
        LPKGLISTITEM pListTitle = NULL;
        LPKGLISTITEM pListItem  = NULL;
        TCHAR szTitle[128];
        sprintf(szTitle, TEXT("TEXTURE_%d"), m_nSelect);
        m_listTexture.FindItemByText(szTitle);
        pListTitle = m_listTexture.FindNextItem();
        if (pListTitle)
        {
            m_listTexture.FindItemByText(TEXT("切分"), pListTitle);
            pListItem = m_listTexture.FindNextItem();

            if (pListItem)
            {
                pListItem->arryItemText[1].szItemText.Format(TEXT("%d"), nCut);
                m_listTexture.Update(pListItem);
            }
        }
        //
        m_nSelect = -1;
    }
}

void KSceneSFXEditorTextureDlg::OnLvnKeydownListTexture(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
    //// TODO: Add your control notification handler code here
    //*pResult = 0;

    //GET_SFX_EDITOR();

    //if (pLVKeyDow->wVKey == VK_DELETE)
    //{
    //    POSITION pos = m_listTexture.GetFirstSelectedItemPosition();
    //    if (!pos)
    //        return;

    //    int nSelect = m_listTexture.GetNextSelectedItem(pos);
    //    if (nSelect >= 0 && nSelect < (int)pScene->m_lpSFX->GetTextureNum())
    //    {
    //        if (SUCCEEDED(pScene->m_lpSFX->RemoveTexture(nSelect)))
    //        {
    //            UpdateUI();
    //            KSceneSFXEditorParticlePage *pPage = pFrame->GetPanelView()->GetParticlePage();
    //            if (pPage)
    //            {
    //                pPage->UpdateParticleKindUI();
    //            }
    //        }
    //    }
    //}
}

void KSceneSFXEditorTextureDlg::OnOK()
{
    // TODO: Add your specialized code here and/or call the base class
}
