////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGMemDateList.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-7-13 19:45:17
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KGMemDateList.h"
#include "KGPropertyListCtrl.h"
#include "KG3DFileViewDialog.h"
#include "shlwapi.h"
#include "KColorDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern TCHAR g_szDefWorkDirectory[256];

KGMemDateList::KGMemDateList()
{
     m_lpCurItem  = NULL;
     m_pDate      = NULL;
}

KGMemDateList::~KGMemDateList()
{

}

BEGIN_MESSAGE_MAP(KGMemDateList, KGListCtrl)
    ON_NOTIFY(UDN_DELTAPOS, ID_SP_SHOW, OnDeltaposSpin)
    ON_WM_LBUTTONDOWN()
    ON_COMMAND(ID_BN_SHOW, OnBnClick)
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

BOOL KGMemDateList::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    m_bnEditShow.ShowWindow(SW_HIDE);
    m_editEdit.ShowWindow(SW_HIDE);
    m_spinCtrl.ShowWindow(SW_HIDE);
    m_comboxCtrl.ShowWindow(SW_HIDE);

    return KGListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

int KGMemDateList::UpdateCtrl(int nFlag)
{
    int nResult  = false;
    int nRetCode = false;
    POSITION pos   = GetFirstSelectedItemPosition();
    int      nItem = GetNextSelectedItem(pos);

    _ItemDate itemDate;
    CRect rectBnEdit;
    CRect rectClient;
    CRect rectEditer;

    KG_PROCESS_ERROR(nItem != -1 && m_lpCurItem);

    m_lpCurItem->GetStructData(&itemDate, sizeof(itemDate));
    GetSubItemRect(nItem, 1, LVIR_BOUNDS, rectBnEdit);
    GetWindowRect(rectClient);
    ScreenToClient(rectClient);

    rectEditer		 = rectBnEdit;
    rectBnEdit.right = rectClient.right;
    rectBnEdit.left  = rectBnEdit.right - rectBnEdit.Height();

    rectEditer.left  = rectEditer.left + 1;
    rectEditer.top   = rectEditer.top  + 1;

    switch (itemDate.dwType)
    {
    case MEM_TYPE_PATH  :
    case MEM_TYPE_COLOR :
        if (nFlag)
        {
            m_bnEditShow.MoveWindow(&rectBnEdit);
            m_bnEditShow.ShowWindow(SW_SHOW);
        }
        else
            m_bnEditShow.ShowWindow(SW_HIDE);
        break;
    case MEM_TYPE_FLOAT :
    case MEM_TYPE_INT   :
        if (nFlag)
        {
            m_editEdit.MoveWindow(&rectEditer);
            m_spinCtrl.MoveWindow(&rectBnEdit);
            m_editEdit.ShowWindow(SW_SHOW);
            m_spinCtrl.ShowWindow(SW_SHOW);
        }
        else
        {
            m_editEdit.ShowWindow(SW_HIDE);
            m_spinCtrl.ShowWindow(SW_HIDE);
        }
        break;
    case MEM_TYPE_STRING :
    case MEM_TYPE_DWORD  :
        if (nFlag)
        {
            m_editEdit.MoveWindow(&rectEditer);
            m_editEdit.ShowWindow(SW_SHOW);
        }
        else
            m_editEdit.ShowWindow(SW_HIDE);

        break;
    case MEM_TYPE_BOOL :
        if (nFlag)
        {
            rectBnEdit.left   = rectEditer.left;
            rectBnEdit.bottom = rectBnEdit.top + 200;
            m_comboxCtrl.MoveWindow(&rectBnEdit);
            m_comboxCtrl.ShowWindow(SW_SHOW);
        }
        else
            m_comboxCtrl.ShowWindow(SW_HIDE);

        break;
    default :
        break;
    }

    Update(m_lpCurItem);

    nResult = true;
Exit0:
    return nResult;
}

void KGMemDateList::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    static UINT scCode = nSBCode;
    if (
        scCode != nSBCode &&
        static_cast<CWnd*>(pScrollBar) != static_cast<CWnd*>(&m_spinCtrl)
    )
    {
        if (nSBCode == 5)
            UpdateCtrl(false);
        if (nSBCode == 8)
            UpdateCtrl(true);
    }
    scCode = nSBCode;
    return KGListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void KGMemDateList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    static UINT scCode = nSBCode;
    if (
        scCode != nSBCode &&
        static_cast<CWnd*>(pScrollBar) != static_cast<CWnd*>(&m_spinCtrl)
    )
    {
        if (nSBCode == 5)
            UpdateCtrl(false);
        if (nSBCode == 8)
            UpdateCtrl(true);
    }
    scCode = nSBCode;
    return KGListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void KGMemDateList::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CClientDC dc(this);
    //CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

    LPKGLISTITEM pItem = (LPKGLISTITEM)GetItemData(lpDrawItemStruct->itemID);
    _ItemDate itemData;

    KGListCtrl::DrawItem(lpDrawItemStruct);

    KG_PROCESS_ERROR(pItem);

    pItem->GetStructData(&itemData, sizeof(itemData));

    if (itemData.dwType == MEM_TYPE_COLOR)
    {
        COLORREF colorRef;// = (COLORREF)atoi(itemData.szDate);
        DWORD    dwColor;
        int      nColorARGB[4];
        sscanf(itemData.szDate, _T("%X"), &dwColor);
        nColorARGB[0] = (dwColor & 0xFF000000)>>24;
        nColorARGB[1] = (dwColor & 0x00FF0000)>>16;
        nColorARGB[2] = (dwColor & 0x0000FF00)>>8;
        nColorARGB[3] = (dwColor & 0x000000FF);
        colorRef =
             ((COLORREF)((((nColorARGB[3])&0xff)<<24)|(((nColorARGB[2])&0xff)<<16)|(((nColorARGB[1])&0xff)<<8)|((nColorARGB[0])&0xff)));
        CRect rect;
        GetSubItemRect(
            lpDrawItemStruct->itemID, 1, LVIR_BOUNDS, rect
        );
        rect.left += 12;
        rect.top  += 2;
        rect.bottom -= 1;
        rect.right = rect.left + rect.Height();
        dc.FillRect(&rect, &CBrush(colorRef));
        dc.Draw3dRect(&rect, RGB(100, 100, 100), RGB(100, 100, 100));
    }

Exit0:
    return;
}


void KGMemDateList::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    _ItemDate useData;
    CString szEditText = _T("");

    KG_PROCESS_ERROR(m_lpCurItem);

    m_lpCurItem->GetStructData(&useData, sizeof(useData));
    float fValue = 0.0f;

    if (useData.dwType == MEM_TYPE_FLOAT)
    {
        m_editEdit.GetWindowText(szEditText);
        fValue = (float)atof(szEditText.GetBuffer()) + pNMUpDown->iDelta / 100.0f;
        //if (fValue >= useData.nMin  && fValue <= useData.nMax)
        szEditText.Format("%.2f", fValue);
        strncpy_s(
            useData.szDate, szEditText.GetBuffer(), sizeof(useData.szDate)
        );
        m_editEdit.SetWindowText(szEditText);
    }

Exit0:
    *pResult = 0;
}

int KGMemDateList::RetrievesItemData(LPKGLISTITEM pItem)
{
    _ItemDate itemData;
    KG_PROCESS_ERROR(pItem);

    KG_PROCESS_ERROR(pItem->arryItemText.GetCount() >= 2);
    pItem->GetStructData(&itemData, sizeof(itemData));

    if (itemData.dwType != MEM_TYPE_COLOR)
    {
        if (itemData.dwType == MEM_TYPE_BOOL)
        {
           char szText[128];
            if (CString(itemData.szDate) == _T("1"))
                strncpy(szText, _T("是"), sizeof(szText));
           /* else if (CString(itemData.szDate) == _T("-1"))
                strncpy(szText, _T("不关心"), sizeof(szText));*/
            else
                strncpy(szText, _T("否"), sizeof(szText));
            pItem->arryItemText.GetAt(1).szItemText = szText;

        }
        else
            pItem->arryItemText.GetAt(1).szItemText = itemData.szDate;
    }
    else
        pItem->arryItemText.GetAt(1).szItemText = _T("");

   if (!m_listDataTree.IsTitle(pItem) && m_pDate)
   {
        switch (itemData.dwType)
        {
        case MEM_TYPE_STRING :
        case MEM_TYPE_PATH   :
            {
                strncpy(
                    (char*)&m_pDate[itemData.nOffset], itemData.szDate, itemData.nSize
                );
            }
            break;
        case MEM_TYPE_FLOAT  :
            {
                *((float*)&m_pDate[itemData.nOffset])  = (float)atof(itemData.szDate);
            }
            break;
        case MEM_TYPE_INT    :
            {
                *((int*)&m_pDate[itemData.nOffset]) = atoi(itemData.szDate);
            }
            break;
        case MEM_TYPE_COLOR   :
        case MEM_TYPE_DWORD   :
            {
                sscanf(itemData.szDate, _T("%X"), (DWORD*)&m_pDate[itemData.nOffset]);
            }
            break;
        case MEM_TYPE_BOOL    :
            {
                if (CString(itemData.szDate) == "1")
                    *((INT*)&m_pDate[itemData.nOffset]) = TRUE;
                else
                    *((INT*)&m_pDate[itemData.nOffset]) = FALSE;
            }
            break;
        default :
            break;
        }
   }
Exit0:
    return true;
}

void KGMemDateList::OnBnClick()
{
    int nResult  = false;
    int nRetCode = false;

    _ItemDate itemData;

    KG_PROCESS_ERROR(m_lpCurItem);
    m_lpCurItem->GetStructData(&itemData, sizeof(itemData));

    if (itemData.dwType == MEM_TYPE_PATH)
    {
		CFileDialog fileDlg(true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, this);

		if (fileDlg.DoModal() == IDOK)
		{
			CString strPath = fileDlg.GetPathName();
			char szName[MAX_PATH];
			g_GetFilePathFromFullPath(szName, strPath.GetBuffer());
			if (strPath.Find(g_szDefWorkDirectory) == -1)
				strncpy(szName, strPath.GetBuffer(), sizeof(szName));
			strncpy(
				itemData.szDate, szName,
				sizeof(itemData.szDate)
				);
			m_lpCurItem->SetStructData(&itemData);
			Update(m_lpCurItem, true);
		}

        //KG3DFileViewDialog fileDlg(
        //    true, _T(""),  _T(""), NULL,  _T(""), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        //    NULL/*itemData.szFileFiter*/, this
        //);
        //if (fileDlg.DoModal() == IDOK)
        //{
        //    CString strPath = fileDlg.GetPathName();
        //    char szName[MAX_PATH];
        //    g_GetFilePathFromFullPath(szName, strPath.GetBuffer());
        //    if (strPath.Find(g_szDefWorkDirectory) == -1)
        //        strncpy(szName, strPath.GetBuffer(), sizeof(szName));
        //    strncpy(
        //        itemData.szDate, szName,
        //        sizeof(itemData.szDate)
        //    );
        //    m_lpCurItem->SetStructData(&itemData);
        //    Update(m_lpCurItem, true);
        //}
    }
    else if (itemData.dwType == MEM_TYPE_COLOR)
    {
        /*int nColor = atoi(itemData.szDate);
        CColorDialog colorDlg((COLORREF)nColor, 0, this);
        if (colorDlg.DoModal() == IDOK)
        {
            nColor = colorDlg.GetColor();
            itoa(nColor, itemData.szDate, 10);
            m_lpCurItem->SetStructData(&itemData);
            Update(m_lpCurItem, true);
        }*/
        int   nColorARGB[4];
        COLORREF colorRef;
        DWORD dwColor;
        sscanf(itemData.szDate, _T("%X"), &dwColor);
        nColorARGB[0] = (dwColor & 0xFF000000)>>24;
        nColorARGB[1] = (dwColor & 0x00FF0000)>>16;
        nColorARGB[2] = (dwColor & 0x0000FF00)>>8;
        nColorARGB[3] = (dwColor & 0x000000FF);
        colorRef =
            ((COLORREF)((((nColorARGB[3])&0xff)<<24)|(((nColorARGB[2])&0xff)<<16)|(((nColorARGB[1])&0xff)<<8)|((nColorARGB[0])&0xff)));
        CColorDialog colorDlg(colorRef, 0, this);
        if (colorDlg.DoModal() == IDOK)
        {
            colorRef = colorDlg.GetColor();
            nColorARGB[0] = (colorRef & 0xFF000000)>>24;
            nColorARGB[1] = (colorRef & 0x00FF0000)>>16;
            nColorARGB[2] = (colorRef & 0x0000FF00)>>8;
            nColorARGB[3] = (colorRef & 0x000000FF);

            dwColor = D3DCOLOR_ARGB(
                nColorARGB[3], nColorARGB[2], nColorARGB[1], nColorARGB[0]
            );
            sprintf(itemData.szDate, _T("%X"), dwColor);
            m_lpCurItem->SetStructData(&itemData);
            Update(m_lpCurItem, true);
        }
        /*CRect rect;
        sprintf(itemData.szDate, _T("%X"), dwColor);
        m_lpCurItem->SetStructData(&itemData);
        Update(m_lpCurItem, true);*/

    }
    else
    {

    }

    nResult = true;
Exit0:
    return;
}

int  KGMemDateList::UpdateToBandDate()
{
	_ItemDate itemData;
	if (m_lpCurItem)
	{
		_ItemDate itemData;
		m_lpCurItem->GetStructData(&itemData, sizeof(itemData));
		if (
			((itemData.dwType == MEM_TYPE_STRING) ||
			(itemData.dwType  == MEM_TYPE_FLOAT)  ||
			(itemData.dwType  == MEM_TYPE_DWORD)  ||
			(itemData.dwType  == MEM_TYPE_INT))   &&
			(m_editEdit.m_hWnd)
			)
		{
			CString strText;
			m_editEdit.GetWindowText(strText);
			strncpy(
				itemData.szDate, strText.GetBuffer(),
				sizeof(itemData.szDate)
				);
			m_lpCurItem->SetStructData(&itemData);

		}
		if (
			(itemData.dwType == MEM_TYPE_BOOL) &&
			(m_comboxCtrl.m_hWnd)
			)
		{
			int nIndex = m_comboxCtrl.GetCurSel();
			if (nIndex != -1)
			{
				strncpy(
					itemData.szDate, m_pszState[nIndex].GetBuffer(),
					sizeof(itemData.szDate)
					);
				m_lpCurItem->SetStructData(&itemData);
			}
		}
		Update(m_lpCurItem, false);
	}

	return true;
}

void KGMemDateList::OnLButtonDown(UINT nFlags, CPoint point)
{
    KGListCtrl::OnLButtonDown(nFlags, point);

    CRect rectBnEdit;
    CRect rectClient;
    CRect rectEditer;

    POSITION pos   = GetFirstSelectedItemPosition();
    int      nItem = GetNextSelectedItem(pos);

    _ItemDate itemData;

//    char szUncaseText[256];
//    char szTureText[256];
//    char szFalseText[256];

    //CScrollBar* pScrollBar = GetScrollBarCtrl(SB_BOTH);
    if (m_lpCurItem)
    {
        _ItemDate itemData;
        m_lpCurItem->GetStructData(&itemData, sizeof(itemData));
        if (
            ((itemData.dwType == MEM_TYPE_STRING) ||
            (itemData.dwType  == MEM_TYPE_FLOAT)  ||
            (itemData.dwType  == MEM_TYPE_DWORD)  ||
            (itemData.dwType  == MEM_TYPE_INT))   &&
            (m_editEdit.m_hWnd)
        )
        {
            CString strText;
            m_editEdit.GetWindowText(strText);
            strncpy(
                itemData.szDate, strText.GetBuffer(),
                sizeof(itemData.szDate)
            );
            m_lpCurItem->SetStructData(&itemData);

        }
        if (
            (itemData.dwType == MEM_TYPE_BOOL) &&
            (m_comboxCtrl.m_hWnd)
        )
        {
            int nIndex = m_comboxCtrl.GetCurSel();
            if (nIndex != -1)
            {
                strncpy(
                    itemData.szDate, m_pszState[nIndex].GetBuffer(),
                    sizeof(itemData.szDate)
                );
                m_lpCurItem->SetStructData(&itemData);
            }
        }
        

        Update(m_lpCurItem, false);
    }

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

    KG_PROCESS_ERROR(nItem != -1);

    m_lpCurItem = (LPKGLISTITEM)GetItemData(nItem);
    KG_PROCESS_ERROR(m_lpCurItem);
    m_lpCurItem->GetStructData(&itemData, sizeof(itemData));
    KG_PROCESS_SUCCESS(m_listDataTree.IsTitle(m_lpCurItem));

    GetSubItemRect(nItem, 1, LVIR_BOUNDS, rectBnEdit);
    GetWindowRect(rectClient);
    ScreenToClient(rectClient);

    rectEditer		 = rectBnEdit;
    rectBnEdit.right = rectClient.right - rectBnEdit.Height();;
    rectBnEdit.left  = rectBnEdit.right - rectBnEdit.Height();

    rectEditer.left  = rectEditer.left + 1;
    rectEditer.top   = rectEditer.top  + 1;

    switch (itemData.dwType) 
    {
    case MEM_TYPE_PATH  :
    case MEM_TYPE_COLOR :
        m_bnEditShow.MoveWindow(&rectBnEdit);
        m_bnEditShow.ShowWindow(SW_SHOW);
        break;
    case MEM_TYPE_FLOAT :
    case MEM_TYPE_INT   :
        if (itemData.dwType == MEM_TYPE_INT)
        {
            m_editEdit.ModifyStyle(0, ES_NUMBER);
            m_spinCtrl.SetBuddy(&m_editEdit);
        }
        else
        {
            //m_editEdit.ModifyStyle(0, ES_NUMBER);
            m_spinCtrl.SetBuddy(NULL);
        }
        m_editEdit.SetWindowText(itemData.szDate);
        m_editEdit.ShowWindow(SW_SHOW);
        rectEditer.right = rectBnEdit.left;
        m_editEdit.MoveWindow(&rectEditer);
        m_spinCtrl.ShowWindow(SW_SHOW);
        m_spinCtrl.MoveWindow(&rectBnEdit);
        //m_spinCtrl.SetBuddy(&m_editEdit);
        m_spinCtrl.SetRange32(INT_MIN, INT_MAX);
        m_editEdit.SetFocus();
        break;
    case MEM_TYPE_STRING :
    case MEM_TYPE_DWORD  :
        m_editEdit.ModifyStyle(ES_NUMBER, 0);
        m_editEdit.SetWindowText(itemData.szDate);
        m_editEdit.MoveWindow(&rectEditer);
        m_editEdit.ShowWindow(SW_SHOW);
        m_editEdit.SetFocus();
        break;
    case MEM_TYPE_BOOL :
        rectBnEdit.left   = rectEditer.left;
        rectBnEdit.bottom = rectBnEdit.top + 200;
        m_comboxCtrl.MoveWindow(&rectBnEdit);
        m_comboxCtrl.ShowWindow(SW_SHOW);
        while (m_comboxCtrl.GetCount() >= 1)
            m_comboxCtrl.DeleteString(0);
        m_comboxCtrl.AddString(_T("是"));
        m_pszState[0] = _T("1");
        m_comboxCtrl.AddString(_T("否"));
        m_pszState[1] = _T("0");
        //m_comboxCtrl.AddString(_T("不关心"));
        //m_pszState[2] = _T("-1");

        if (CString(itemData.szDate) == _T("1"))
            m_comboxCtrl.SelectString(0, _T("是"));
     /*   else if (CString(itemData.szDate) == _T("-1"))
            m_comboxCtrl.SelectString(0, _T("不关心"));*/
        else
            m_comboxCtrl.SelectString(0, _T("否"));

        break;
    default :
        m_editEdit.SetWindowText(_T("Debug Message : UnKnow Value Type"));
        break;
    }

Exit1:
Exit0:
    return;
}

int KGMemDateList::BandDate(BYTE* pDate, KGMEMDATEELEMENT element[])
{
    int nResult  = false;
    int nRetCode = false;

    LPKGLISTITEM pTitleItem = NULL;
    int nIndex  = 0;

    KG_PROCESS_ERROR(pDate);

    m_pDate = pDate;
    while (element[nIndex].nOffset != -1 && element[nIndex].nSize != -1)
    {
        _ItemDate itemDate;
        ITEMTEXT  itemText;
		itemText.colorBack     = RGB(255, 255, 255);
		itemText.colorBackSel  = RGB(255, 255, 255);

        if (element[nIndex].nOffset == ELEMENT_AUTO)
        {
            int nPrevIndex = nIndex - 1;
            if (nIndex)
            {
                while (element[nPrevIndex].dwType == MEM_TYPE_TITLE && nPrevIndex)
                    nPrevIndex--;
            }
            else
            {
                nPrevIndex = 0;
            }
           if (nPrevIndex)
                element[nIndex].nOffset =
                    element[nPrevIndex].nOffset + element[nPrevIndex].nSize;
            else
                element[nIndex].nOffset = 0;
        }
        strncpy(
            itemDate.szPlate, element[nIndex].strPlate,
            sizeof(itemDate.szPlate)
        );
        itemDate.nOffset = element[nIndex].nOffset;
        itemDate.nSize   = element[nIndex].nSize;
        itemDate.dwType  = element[nIndex].dwType;

        switch (itemDate.dwType)
        {
        case MEM_TYPE_TITLE :
            {
                pTitleItem            = new KGLISTITEM();
                ITEMTEXT itemText;
                itemText.colorBack     = RGB(92, 92, 92);
                itemText.colorBackSel  = RGB(92, 92, 92);
                itemText.colorFrame    = RGB(192, 192, 192);
                itemText.colorFrameSel = RGB(192, 192, 192);
                itemText.colorText	   = RGB(255, 255, 255);
                itemText.colorTextSel  = RGB(2, 255, 2);

                itemText.szItemText    = element[nIndex].strPlate;
                pTitleItem->arryItemText.Add(itemText);
                InsertDepend(NULL, pTitleItem);

                nIndex++;
                continue;
            }
            break;
        case MEM_TYPE_STRING :
        case MEM_TYPE_PATH   :
            {
                strncpy_s(
                    itemDate.szDate, sizeof(itemDate.szDate),
                    reinterpret_cast<char*>(&pDate[element[nIndex].nOffset]),
                    element[nIndex].nSize
                );
            }
            break;
        case MEM_TYPE_FLOAT   :
            {
                float value =
                    *(reinterpret_cast<float*>(&pDate[element[nIndex].nOffset]));
                sprintf(itemDate.szDate, _T("%f"), value);
            }
            break;
        case MEM_TYPE_INT     :
            {
                int value =
                    *(reinterpret_cast<int*>(&pDate[element[nIndex].nOffset]));
                sprintf(itemDate.szDate, _T("%d"), value);
            }
            break;
        case MEM_TYPE_BOOL    :
            {
                BOOL value =
                    *(reinterpret_cast<INT*>(&pDate[element[nIndex].nOffset]));
                if (value)
                    sprintf(itemDate.szDate, _T("%s"), _T("1"));
                else
                    sprintf(itemDate.szDate, _T("%s"), _T("0"));
            }
            break;
        case MEM_TYPE_COLOR    :
        case MEM_TYPE_DWORD    :
            {
                DWORD value =
                    *(reinterpret_cast<DWORD*>(&pDate[element[nIndex].nOffset]));
                sprintf(itemDate.szDate, _T("%X"), value);
            }
            break;
        default :
            break;
        }

        LPKGLISTITEM pInsertItem = new KGLISTITEM();
        itemText.szItemText      = element[nIndex].strPlate;
        pInsertItem->arryItemText.Add(itemText);
        itemText.szItemText      = itemDate.szDate;
        pInsertItem->arryItemText.Add(itemText);
        pInsertItem->SetStructData(&itemDate);

        InsertDepend(pTitleItem, pInsertItem);

        nIndex++;

    }

    nResult = true;
Exit0:
    return nResult;
}