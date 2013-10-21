////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGInIFileEditDlg.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-7-13 19:45:17
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KGInIFileEditDlg.h"
#include "KGPropertyListCtrl.h"
#include "KG3DFileViewDialog.h"
#include "shlwapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern TCHAR g_szDefWorkDirectory[256];

KGIniListCtrl::KGIniListCtrl()
{
    m_pIniDateType = NULL;
    m_lpCurItem    = NULL;
    m_nCurIndex    = 0;
}

KGIniListCtrl::~KGIniListCtrl()
{
    SAFE_RELEASE(m_pIniDateType);
    for (int i = 0; i < (int)m_vecIniProperty.size(); i++)
        SAFE_DELETE(m_vecIniProperty[i]);
    m_vecIniProperty.clear();
}

BEGIN_MESSAGE_MAP(KGIniListCtrl, KGListCtrl)
    ON_NOTIFY(UDN_DELTAPOS, ID_SP_SHOW, OnDeltaposSpin)
    ON_WM_LBUTTONDOWN()
    ON_COMMAND(ID_BN_SHOW, OnBnClick)
END_MESSAGE_MAP()

void KGIniListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    KGListCtrl::OnLButtonDown(nFlags, point);

    CRect rectBnEdit;
    CRect rectClient;
    CRect rectEditer;

    POSITION pos   = GetFirstSelectedItemPosition();
    int      nItem = GetNextSelectedItem(pos);

    USER_ITEM_DATA itemData;

    char szUncaseText[256];
    char szTureText[256];
    char szFalseText[256];

    //HRGN rgn;

    KG_PROCESS_ERROR(nItem != -1);
    if (m_lpCurItem)
    {
        USER_ITEM_DATA itemData;
        m_lpCurItem->GetStructData(&itemData, sizeof(itemData));
        if (
            ((itemData.dwValueType == VALUE_STRING) ||
             (itemData.dwValueType == VALUE_FLOAT)  ||
             (itemData.dwValueType == VALUE_INT))   &&
            (m_editEdit.m_hWnd)
        )
        {
            CString strText;
            m_editEdit.GetWindowText(strText);
            strncpy(
                itemData.szPropertyValue, strText.GetBuffer(),
                sizeof(itemData.szPropertyValue)
            );
            m_lpCurItem->SetStructData(&itemData);

        }
        if (
            (itemData.dwValueType == VALUE_BOOL) &&
            (m_comboxCtrl.m_hWnd)
        )
        {
            int nIndex = m_comboxCtrl.GetCurSel();
            if (nIndex != -1)
            {
                strncpy(
                    itemData.szPropertyValue, m_pszState[nIndex].GetBuffer(),
                    sizeof(itemData.szPropertyValue)
                );
                m_lpCurItem->SetStructData(&itemData);
            }
        }
        Update(m_lpCurItem, false);
    }
    m_lpCurItem = (LPKGLISTITEM)GetItemData(nItem);
    KG_PROCESS_ERROR(m_lpCurItem);
    m_lpCurItem->GetStructData(&itemData, sizeof(itemData));
    KG_PROCESS_SUCCESS(m_listDataTree.IsTitle(m_lpCurItem));

    if (!m_bnEditShow.m_hWnd)
    {
        m_bnEditShow.Create(_T("?"), WS_CHILD, rectBnEdit, this, ID_BN_SHOW);
    }
    if (!m_editEdit.m_hWnd)
    {
        m_editEdit.Create(ES_AUTOHSCROLL, rectEditer, this, ID_ED_SHOW);
        m_editEdit.SetFont(GetFont());
    }
    if (!m_spinCtrl.m_hWnd)
    {
        m_spinCtrl.Create(WS_CHILD | UDS_SETBUDDYINT, rectBnEdit, this, ID_SP_SHOW);
    }
    if (!m_comboxCtrl.m_hWnd)
    {
        m_comboxCtrl.Create(WS_CHILD | CBS_DROPDOWNLIST, rectBnEdit, this, ID_CB_SHOW);
        m_comboxCtrl.SetFont(GetFont());
    }

    m_editEdit.ShowWindow(SW_HIDE);
    m_bnEditShow.ShowWindow(SW_HIDE);
    m_spinCtrl.ShowWindow(SW_HIDE);
    m_comboxCtrl.ShowWindow(SW_HIDE);

    GetSubItemRect(nItem, 1, LVIR_BOUNDS, rectBnEdit);
    GetWindowRect(rectClient);
    ScreenToClient(rectClient);

    rectEditer		 = rectBnEdit;
    rectBnEdit.right = rectClient.right;
    rectBnEdit.left  = rectBnEdit.right - rectBnEdit.Height();

    rectEditer.left  = rectEditer.left + 1;
    rectEditer.top   = rectEditer.top  + 1;

    switch (itemData.dwValueType)
    {
    case VALUE_FILE  :
    case VALUE_COLOR :
        m_bnEditShow.ShowWindow(SW_SHOW);
        m_bnEditShow.MoveWindow(&rectBnEdit);
        break;
    case VALUE_FLOAT :
    case VALUE_INT   :
        if (itemData.dwValueType == VALUE_INT)
        {
            m_editEdit.ModifyStyle(0, ES_NUMBER);
            m_spinCtrl.SetBuddy(&m_editEdit);
        }
        else
        {
            //m_editEdit.ModifyStyle(0, ES_NUMBER);
            m_spinCtrl.SetBuddy(NULL);
        }
        m_editEdit.SetWindowText(itemData.szPropertyValue);
        m_editEdit.ShowWindow(SW_SHOW);
        rectEditer.right = rectBnEdit.left;
        m_editEdit.MoveWindow(&rectEditer);
        m_spinCtrl.ShowWindow(SW_SHOW);
        m_spinCtrl.MoveWindow(&rectBnEdit);
        //m_spinCtrl.SetBuddy(&m_editEdit);
        m_spinCtrl.SetRange32(itemData.nMin, itemData.nMax);
        m_editEdit.SetFocus();
        break;
    case VALUE_STRING :
        m_editEdit.ModifyStyle(ES_NUMBER, 0);
        m_editEdit.SetWindowText(itemData.szPropertyValue);
        m_editEdit.ShowWindow(SW_SHOW);
        m_editEdit.MoveWindow(&rectEditer);
        m_editEdit.SetFocus();
        break;
    case VALUE_BOOL :
        m_comboxCtrl.ShowWindow(SW_SHOW);
        rectBnEdit.left   = rectEditer.left;
        rectBnEdit.bottom = rectBnEdit.top + 200;
        m_comboxCtrl.MoveWindow(&rectBnEdit);
        while (m_comboxCtrl.GetCount() >= 1)
            m_comboxCtrl.DeleteString(0);
        m_pIniDateType->GetString(
            itemData.szPropertyName, "True", _T("是"), szTureText,
            sizeof(szTureText)
        );
        if (CString(szTureText) != "")
        {
            m_comboxCtrl.AddString(szTureText);
            m_pszState[0] = _T("1");
        }
        m_pIniDateType->GetString(
            itemData.szPropertyName, "False", _T("否"), szFalseText,
            sizeof(szFalseText)
        );
        if (CString(szFalseText) != "")
        {
            m_comboxCtrl.AddString(szFalseText);
            m_pszState[1] = _T("0");
        }
        m_pIniDateType->GetString(
            itemData.szPropertyName, "Uncase", _T(""), szUncaseText,
            sizeof(szUncaseText)
        );
        if (CString(szUncaseText) != "")
        {
            m_comboxCtrl.AddString(szUncaseText);
            m_pszState[2] = _T("-1");
        }

        if (CString(itemData.szPropertyValue) == _T("1"))
            m_comboxCtrl.SelectString(0, szTureText);
        else if (CString(itemData.szPropertyValue) == _T("-1"))
            m_comboxCtrl.SelectString(0, szUncaseText);
        else
            m_comboxCtrl.SelectString(0, szFalseText);

        break;
    default :
        m_editEdit.SetWindowText(_T("Debug Message : UnKnow Value Type"));
        break;
    }

Exit1:
Exit0:
    return;
}

void KGIniListCtrl::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    USER_ITEM_DATA useData;
    CString szEditText = _T("");

    KG_PROCESS_ERROR(m_lpCurItem);

    m_lpCurItem->GetStructData(&useData, sizeof(useData));
    float fValue = 0.0f;

    if (useData.dwValueType == VALUE_FLOAT)
    {
        m_editEdit.GetWindowText(szEditText);
        fValue = (float)atof(szEditText.GetBuffer()) + pNMUpDown->iDelta / 100.0f;
        if (fValue >= useData.nMin  && fValue <= useData.nMax)
            szEditText.Format("%.2f", fValue);
        strncpy_s(
            useData.szPropertyValue, szEditText.GetBuffer(), sizeof(useData.szPropertyValue)
        );
        m_editEdit.SetWindowText(szEditText);
    }

    //m_lpCurItem->SetStructData(&useData);

Exit0:
    *pResult = 0;
}

void KGIniListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

    LPKGLISTITEM pItem = (LPKGLISTITEM)GetItemData(lpDrawItemStruct->itemID);
    USER_ITEM_DATA itemData;

    KGListCtrl::DrawItem(lpDrawItemStruct);

    KG_PROCESS_ERROR(pItem);

    pItem->GetStructData(&itemData, sizeof(itemData));

    if (itemData.dwValueType == VALUE_COLOR)
    {
        COLORREF colorRef = (COLORREF)atoi(itemData.szPropertyValue);
        CRect rect;
        GetSubItemRect(
            lpDrawItemStruct->itemID, 1, LVIR_BOUNDS, rect
        );
        rect.left += 12;
        rect.top  += 2;
        rect.bottom -= 1;
        rect.right = rect.left + rect.Height();
        pDC->FillRect(&rect, &CBrush(colorRef));
        pDC->Draw3dRect(&rect, RGB(100, 100, 100), RGB(100, 100, 100));
    }

Exit0:
    return;
}

int KGIniListCtrl::RetrievesItemData(LPKGLISTITEM pItem)
{
    USER_ITEM_DATA itemData;
    KG_PROCESS_ERROR(pItem);

    KG_PROCESS_ERROR(pItem->arryItemText.GetCount() >= 2);
    pItem->GetStructData(&itemData, sizeof(itemData));

    if (itemData.dwValueType != VALUE_COLOR)
    {
        if (itemData.dwValueType == VALUE_BOOL && m_pIniDateType)
        {
            char szText[128];
            if (CString(itemData.szPropertyValue) == _T("1"))
                m_pIniDateType->GetString(
                    itemData.szPropertyName, _T("True"), _T("是"),
                    szText, sizeof(szText)
                );
            else if (CString(itemData.szPropertyValue) == _T("-1"))
                m_pIniDateType->GetString(
                    itemData.szPropertyName, _T("Uncase"), _T(""),
                    szText, sizeof(szText)
                );
            else
                m_pIniDateType->GetString(
                    itemData.szPropertyName, _T("False"), _T("否"),
                    szText, sizeof(szText)
                );
            pItem->arryItemText.GetAt(1).szItemText = szText;

        }
        else
            pItem->arryItemText.GetAt(1).szItemText = itemData.szPropertyValue;
    }
    else
        pItem->arryItemText.GetAt(1).szItemText = _T("");

    if (!m_listDataTree.IsTitle(pItem))
        m_vecIniProperty[m_nCurIndex]->pIniFile->WriteString(
            itemData.szParamName_1, itemData.szPropertyName, itemData.szPropertyValue
        );

Exit0:
    return true;
}

int KGIniListCtrl::BandDate(vector<CString> vecStrIniProperty, CString strIniDateType)
{
    int nResult  = false;
    int nRetCode = false;

    m_strIniDateType = strIniDateType;
    m_pIniDateType   = g_OpenIniFile(strIniDateType);
    KG_PROCESS_ERROR(m_pIniDateType);

    for (int i = 0; i < (int)vecStrIniProperty.size(); i++)
    {
        IIniFile* pIniFile = NULL;
        if (::PathFileExists(vecStrIniProperty[i]))
            pIniFile = g_OpenIniFile(vecStrIniProperty[i], false, true);
        else
            Build(&pIniFile);
        if (pIniFile)
        {
            _InIInfo* pIniInfo = new _InIInfo;
            pIniInfo->strPath  = vecStrIniProperty[i];
            pIniInfo->pIniFile = pIniFile;
            m_vecIniProperty.push_back(pIniInfo);
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

void KGIniListCtrl::OnBnClick()
{
    int nResult  = false;
    int nRetCode = false;

    USER_ITEM_DATA itemData;

    KG_PROCESS_ERROR(m_lpCurItem);
    m_lpCurItem->GetStructData(&itemData, sizeof(itemData));

    if (itemData.dwValueType == VALUE_FILE)
    {
        KG3DFileViewDialog fileDlg(
            true, _T(""),  _T(""), NULL,  _T(""), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
            itemData.szFileFiter, this
        );
        if (fileDlg.DoModal() == IDOK)
        {
            CString strPath = fileDlg.GetPathName();
            char szName[MAX_PATH];
            g_GetFilePathFromFullPath(szName, strPath.GetBuffer());
            if (strPath.Find(g_szDefWorkDirectory) == -1)
                strncpy(szName, strPath.GetBuffer(), sizeof(szName));
            strncpy(
                itemData.szPropertyValue, szName,
                sizeof(itemData.szPropertyValue)
            );
            m_lpCurItem->SetStructData(&itemData);
            Update(m_lpCurItem, true);
        }
    }
    else if (itemData.dwValueType == VALUE_COLOR)
    {
        int nColor = atoi(itemData.szPropertyValue);
        CColorDialog colorDlg((COLORREF)nColor, 0, this);
        if (colorDlg.DoModal() == IDOK)
        {
            nColor = colorDlg.GetColor();
            itoa(nColor, itemData.szPropertyValue, 10);
            m_lpCurItem->SetStructData(&itemData);
            Update(m_lpCurItem, true);
        }

    }
    else
    {

    }

    nResult = true;
Exit0:
    return;
}

int KGIniListCtrl::UpdateProperty(int nIndex)
{
    int nResult  = false;
    int nRetCode = false;

    nRetCode = DeleteAllItems();
    KG_PROCESS_ERROR(nRetCode);

    KG_PROCESS_ERROR(nIndex < (int)m_vecIniProperty.size());

    char szSection[128];
    char szKeyName[128];
    char szKeyDate[512];

    *szSection = '\0';
    *szKeyName = '\0';

    m_nCurIndex = nIndex;
    while (m_vecIniProperty[nIndex]->pIniFile->GetNextSection(szSection, szSection))
    {
        LPKGLISTITEM pTitleItem = new KGLISTITEM;
        ITEMTEXT	 itemText;

        itemText.colorBack     = RGB(92, 92, 92);
        itemText.colorBackSel  = RGB(92, 92, 92);
        itemText.colorFrame    = RGB(192, 192, 192);
        itemText.colorFrameSel = RGB(192, 192, 192);
        itemText.colorText	   = RGB(255, 255, 255);
        itemText.colorTextSel  = RGB(2, 255, 2);

        itemText.szItemText   = szSection ;
        pTitleItem->arryItemText.Add(itemText);
        InsertDepend(NULL, pTitleItem);

        *szKeyName = '\0';
        while (
            m_vecIniProperty[nIndex]->pIniFile->GetNextKey(
                szSection, szKeyName, szKeyName
            )
        )
        {
            LPKGLISTITEM    pInsertItem = new KGLISTITEM;
            ITEMTEXT	    itemText;
            USER_ITEM_DATA  itemData;
            char            szDateType[128];
            char            szKeyPlate[128];

            strncpy(itemData.szParamName_1, szSection, sizeof(itemData.szParamName_1));
            m_vecIniProperty[nIndex]->pIniFile->GetString(
                szSection, szKeyName, _T(""), szKeyDate, sizeof(szKeyDate)
            );

            m_pIniDateType->GetString(
                szKeyName, _T("Name"), _T(""), szKeyPlate, sizeof(szKeyPlate)
            );
            if (*szKeyPlate == '\0')
                strncpy(szKeyPlate, szKeyName, sizeof(szKeyPlate));
            itemText.szItemText = szKeyPlate;
            pInsertItem->arryItemText.Add(itemText);
            itemText.szItemText = szKeyDate;
            pInsertItem->arryItemText.Add(itemText);

            strncpy(itemData.szPropertyName,  szKeyName, sizeof(itemData.szPropertyName));
            strncpy(itemData.szPropertyValue, szKeyDate, sizeof(itemData.szPropertyValue));

            m_pIniDateType->GetString(
                szKeyName, _T("Type"), _T(""), szDateType, sizeof(szDateType)
            );
            itemData.dwValueType = KGPropertyListCtrl::GetKeyType(szDateType);
            if (itemData.dwValueType == VALUE_UNKNOW)
                itemData.dwValueType = VALUE_STRING;

            m_pIniDateType->GetInteger(szKeyName, _T("Min"),    INT_MIN, &itemData.nMin);
            m_pIniDateType->GetInteger(szKeyName, _T("Max"),    INT_MAX, &itemData.nMax);
            m_pIniDateType->GetInteger(szKeyName, _T("Length"), INT_MIN, &itemData.nLength);
            m_pIniDateType->GetInteger(szKeyName, _T("Need"), 0, &itemData.nNeedFlag);
            m_pIniDateType->GetInteger(szKeyName, _T("Edit"), 1, &itemData.nEditFlag);
            m_pIniDateType->GetString(
                szKeyName, _T("Tip"), _T(""), pInsertItem->szTipText,
                sizeof(pInsertItem->szTipText)
            );
            m_pIniDateType->GetString(
                szKeyName,_T("FILE_FIETR"), _T("所有文件|*.*|"), itemData.szFileFiter,
                sizeof(itemData.szFileFiter)
            );
            pInsertItem->SetStructData(&itemData);
            InsertDepend(pTitleItem, pInsertItem);
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

int KGIniListCtrl::Save()
{
    for (int i = 0; i < (int)m_vecIniProperty.size(); i++)
    {
        if (m_vecIniProperty[i]->pIniFile)
            m_vecIniProperty[i]->pIniFile->Save(m_vecIniProperty[i]->strPath.GetBuffer());
    }
    return true;
}

int KGIniListCtrl::Build(IIniFile** ppIniFile)
{
    int nResult  = false;
    int nRetCode = false;

    char szSection[128];
    char szKeyName[128];
    char szKeyDate[512];

    *szSection = '\0';
    *szKeyName = '\0';

    KG_PROCESS_ERROR(m_pIniDateType);
    KG_PROCESS_ERROR(ppIniFile);

    (*ppIniFile) = g_CreateIniFile();

    while (m_pIniDateType->GetNextSection(szSection, szSection))
    {
        *szKeyName = '\0';
        nRetCode = m_pIniDateType->GetString(
            szSection, _T("Name"), _T(""), szKeyDate, sizeof(szKeyDate)
        );
        while (m_pIniDateType->GetNextKey(szSection, szKeyName, szKeyName) && !nRetCode)
        {
            if (m_pIniDateType->GetString(
                    szSection, szKeyName, _T(""), szKeyDate, sizeof(szKeyDate)
                )
            )
            {
                char szDefDate[MAX_PATH];
                m_pIniDateType->GetString(
                    szKeyDate, _T("Def"), _T(""), szDefDate, sizeof(szDefDate)
                );
                (*ppIniFile)->WriteString(szSection, szKeyDate, szDefDate);
            }
        }
    }


    nResult = true;
Exit0:
    return nResult;
}

IMPLEMENT_DYNAMIC(KGInIFileEditDlg, CDialog)

KGInIFileEditDlg::KGInIFileEditDlg(
    vector<CString> vecStrIniProperty, CString strIniDateType, CWnd* pParent
) : CDialog(KGInIFileEditDlg::IDD, pParent)
{
    m_vecStrIniProperty = vecStrIniProperty;
    m_strIniDateType    = strIniDateType;
}

KGInIFileEditDlg::~KGInIFileEditDlg()
{
    m_vecStrIniProperty.clear();
}

void KGInIFileEditDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST, m_list);
    DDX_Control(pDX, IDC_TREE, m_tree);
}

BEGIN_MESSAGE_MAP(KGInIFileEditDlg, CDialog)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnTvnSelchangedTree)
    ON_BN_CLICKED(IDOK, &KGInIFileEditDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL KGInIFileEditDlg::OnInitDialog()
{
    int nResult  = false;
    int nRetCode = false;

    DWORD dwStyle;

    nRetCode = CDialog::OnInitDialog();
    KG_PROCESS_ERROR(nRetCode);

    m_imageList.Create(15, 15, ILC_COLOR24, 2, 2);
    m_imageList.Add(AfxGetApp()->LoadIcon(IDC_GLEAN));

    m_list.InsertColumn(0, "属性名");
    m_list.InsertColumn(1, "属性值");
    m_list.SetColumnWidth(0, 150);
    m_list.SetColumnWidth(1, 700);
    m_list.SetBkColor(RGB(92, 92, 92));

    m_list.SetupToolTip(200, RGB(68, 68, 68), RGB(255, 255, 255));
    m_list.SetHoverTime(100);

    m_tree.SetImageList(&m_imageList, TVSIL_NORMAL);
    m_tree.SetBkColor(RGB( 92,  92, 92));
    m_tree.SetTextColor(RGB(255, 255, 255));
    m_tree.SetLineColor(RGB(255, 255, 255));

    dwStyle = GetWindowLong(m_tree.m_hWnd, GWL_STYLE);
    dwStyle |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
    SetWindowLong(m_tree.m_hWnd, GWL_STYLE, dwStyle);

    m_list.BandDate(m_vecStrIniProperty, m_strIniDateType);
    //m_list.UpdateProperty(0);

    for (int i = 0; i < (int)m_list.m_vecIniProperty.size(); i ++)
    {
        CString strTitle = m_list.m_vecIniProperty[i]->strPath;
        CString strName  = PathFindFileName(strTitle.GetBuffer());
        CString strPlate;
        HTREEITEM hItem = NULL;
        int nPos = 0;
        strPlate = strName.Tokenize(_T("."), nPos);
        hItem = m_tree.InsertItem(strPlate);
        if (hItem)
            m_tree.SetItemData(hItem, static_cast<DWORD_PTR>(i));
    }

    m_tree.SelectItem(m_tree.GetRootItem());

    nResult = true;
Exit0:
    return nResult;
}

void KGInIFileEditDlg::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    int  nResult  = false;
    int  nRetCode = false;

    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    HTREEITEM hItem = m_tree.GetSelectedItem();
    KG_PROCESS_ERROR(hItem);

    m_list.UpdateProperty(static_cast<int>(m_tree.GetItemData(hItem)));

    nResult = true;
Exit0:
    return;
}

void KGInIFileEditDlg::OnBnClickedOk()
{
    m_list.Save();
    return OnOK();
}