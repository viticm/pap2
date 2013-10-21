#include "Stdafx.h"
#include"UiFontManage.h"
#include "UiColorManage.h"
#include "resource.h"



namespace UIFONTMANAGE
{

    static KUiFontManage FontManager;

    KUiFontManage::KUiFontManage()
    {   
        m_nCurrentItemNum = 0;
        m_hWnd = NULL;
        m_szFontFileName[0] = 0;
        m_hListWnd = NULL;
        m_szFont[0] = 0;
        m_pFontIni = NULL;
        m_bSel = FALSE;
        m_bManage = FALSE;

    }

    KUiFontManage::~KUiFontManage()
    {
        if (m_pFontIni) 
        {
            m_pFontIni->Clear();
            m_pFontIni->Release();
            m_pFontIni = NULL;
        }
	    if (m_hWnd)
	    {
		    ::DestroyWindow(m_hWnd);
		    m_hWnd = NULL;
	    }
    }

    int KUiFontManage::Init(HWND hWnd)
    {
        int nResult  = false;
        int nRetCode = false;

        KG_PROCESS_ERROR(hWnd);
        KG_PROCESS_ERROR(m_szFontFileName[0]);
        m_hWnd = hWnd;

        RECT rcWindow;
        ::GetWindowRect(m_hWnd, &rcWindow);

        ::SetWindowPos(m_hWnd, HWND_TOP, 400, 200,
		    rcWindow.right -rcWindow.left, rcWindow.bottom - rcWindow.top,
		    SWP_SHOWWINDOW);

	    m_hListWnd = ::GetDlgItem(hWnd, IDC_WND_FONT_MANAGE_FONT_LIST);
        KG_PROCESS_ERROR(m_hListWnd);
	    //设置文件列表框的属性与分栏信息
	    ListView_SetExtendedListViewStyle(m_hListWnd,
		    ListView_GetExtendedListViewStyle(m_hListWnd) |	LVS_EX_FULLROWSELECT);
	    RECT	rc;
	    ::GetClientRect(m_hListWnd, &rc);
	    LVCOLUMN	lvc;
	    char		szHeaderText[128];
	    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
	    lvc.cx = (rc.right - rc.left) / 3;
	    lvc.pszText = szHeaderText;
	    lvc.cchTextMax = 128;
        strcpy(szHeaderText, "字体名:");
	    ListView_InsertColumn(m_hListWnd, 0, &lvc);

        lvc.cx = (rc.right - rc.left)  / 3;
	    lvc.mask |= LVCF_SUBITEM;
	    lvc.iSubItem = 1;
	    strcpy(szHeaderText, "字体颜色");
	    ListView_InsertColumn(m_hListWnd, 1, &lvc);

        lvc.cx = (rc.right - rc.left)  / 3;
	    lvc.mask |= LVCF_SUBITEM;
	    lvc.iSubItem = 1;
	    strcpy(szHeaderText, "字边颜色");
	    ListView_InsertColumn(m_hListWnd, 2, &lvc);  

		lvc.cx = (rc.right - rc.left)  / 3;
		lvc.mask |= LVCF_SUBITEM;
		lvc.iSubItem = 1;
		strcpy(szHeaderText, "阴影颜色");
		ListView_InsertColumn(m_hListWnd, 3, &lvc);  

        //读入Font配置文件
        if (m_pFontIni == NULL) 
        {
            m_pFontIni = g_OpenIniFile(m_szFontFileName);
        }
        KG_PROCESS_ERROR(m_pFontIni);

        int nFontNum = 0;
        char szName[8] = "";
        char szKey[32] = "";
        int i = 0;
        nFontNum = m_pFontIni->GetSectionCount();

        for (i = 0; i < nFontNum; i++) 
        {
            sprintf(szName, "%d", i);
            m_pFontIni->GetString(szName, "Name", "", szKey, sizeof(szKey));

            LV_ITEM			lvi;
		    memset(&lvi, 0, sizeof(LV_ITEM));
		    lvi.mask = LVIF_TEXT;
		    lvi.pszText		= (char*)szKey;
		    lvi.cchTextMax  = 128;
		    lvi.iItem       = ListView_GetItemCount(m_hListWnd);
		    lvi.iItem		= ListView_InsertItem(m_hListWnd, &lvi);

            m_pFontIni->GetString(szName, "Color", "", szKey, sizeof(szKey));
            ListView_SetItemText(m_hListWnd, lvi.iItem, 1, szKey);
            m_pFontIni->GetString(szName, "BorderColor", "", szKey, sizeof(szKey));
            ListView_SetItemText(m_hListWnd, lvi.iItem, 2, szKey);
			m_pFontIni->GetString(szName, "ProjectionColor", "", szKey, sizeof(szKey));
			ListView_SetItemText(m_hListWnd, lvi.iItem, 3, szKey);
			
        }

        nResult = true;
    Exit0:

	    return nResult;
    }

    BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        int nResult = FALSE;

        switch(message) 
        {
        case WM_INITDIALOG:
            FontManager.Init(hWnd);
        	break;
        case WM_CLOSE:
            FontManager.Release();
            EndDialog(hWnd, FALSE);
            nResult = TRUE;
            break;
        case WM_DRAWITEM:
            FontManager.Draw((LPDRAWITEMSTRUCT)lParam);
            break;
        case WM_NOTIFY:
            {
		        LPNMHDR pNotify = (LPNMHDR)lParam;
		        if (pNotify->hwndFrom == FontManager.GetListHandle())
			        FontManager.OnListNotify(pNotify);
            }
            break;
        case WM_COMMAND:
	        {
		        int nNotify = HIWORD(wParam);
		        switch(nNotify)
		        {
		        case EN_CHANGE:
                    FontManager.OnUpdateEditText(LOWORD(wParam));
			        break;
		        case 0:
                    FontManager.OnButtonClick(LOWORD(wParam));
			        break;
		        }
	        }
            break;
        default:
            break;
        }

	    return nResult;
    }

    void KUiFontManage::Draw(LPDRAWITEMSTRUCT lpDrawItem)
    {
        KG_PROCESS_ERROR(lpDrawItem->CtlType == ODT_LISTVIEW && lpDrawItem->hwndItem == m_hListWnd);
        KG_PROCESS_ERROR(m_pFontIni);

        HDC hDc = lpDrawItem->hDC;
        COLORREF bkColor, FontColor, FontBColor, FontPColor;
        char szName[8];
		szName[0] = '\0';
        char szValue[128];
		szValue[0] = '\0';
        char szColorValue[128];
		szColorValue[0] = '\0';
        char szBoderColorValue[128];
		szBoderColorValue[0] = '\0';
		char szProjectionValue[128];
		szProjectionValue[0] = '\0';

        sprintf(szName, "%d", lpDrawItem->itemID);
        m_pFontIni->GetString(szName, "Name", "", szValue, 32);
        m_pFontIni->GetString(szName, "Color", "", szColorValue, 32);
        m_pFontIni->GetString(szName, "BorderColor", "", szBoderColorValue, 32);
		m_pFontIni->GetString(szName, "ProjectionColor", "", szProjectionValue, 32);
        FontColor = UICOLORMANAGE::GetColorValueByName(szColorValue);
        FontBColor = UICOLORMANAGE::GetColorValueByName(szBoderColorValue);
		FontPColor = UICOLORMANAGE::GetColorValueByName(szProjectionValue);

        char *pClip = NULL;
        pClip = strchr(szValue, 0);
        int i = 0;
        for (i = pClip - szValue; i < sizeof(szValue); i++) 
        {
            szValue[i] = ' ';
        }
        szValue[127] = 0;

        pClip = strchr(szColorValue, 0);
        for (i = pClip - szColorValue; i < sizeof(szColorValue); i++) 
        {
            szColorValue[i] = ' ';
        }
        szColorValue[127] = 0;

        pClip = strchr(szBoderColorValue, 0);
        for (i = pClip - szBoderColorValue; i < sizeof(szBoderColorValue); i++) 
        {
            szBoderColorValue[i] = ' ';
        }
        szBoderColorValue[127] = 0;

		pClip = strchr(szProjectionValue, 0);
		for (i = pClip - szProjectionValue; i < sizeof(szProjectionValue); i++) 
		{
			szProjectionValue[i] = ' ';
		}
		szProjectionValue[127] = 0;


        SetBkMode(hDc, OPAQUE);
        if (lpDrawItem->itemState == ODS_SELECTED) 
        {
            bkColor = SetBkColor(hDc, RGB( 0, 0, 0));
            SetTextColor(hDc, RGB(255,0, 0));
        }
        else
        {
            SetTextColor(hDc, RGB(0,0, 0));
        }
        
        RECT rc;
        ListView_GetSubItemRect(m_hListWnd, lpDrawItem->itemID, 0, LVIR_BOUNDS, &rc);
        DrawText(hDc, szValue, sizeof(szValue), &rc, NULL);

        COLORREF bkBkcolor =SetBkColor(hDc, FontColor); 
        ListView_GetSubItemRect(m_hListWnd, lpDrawItem->itemID, 1, LVIR_BOUNDS, &rc);
        DrawText(hDc, szColorValue, sizeof(szColorValue), &rc, NULL);

        SetBkColor(hDc, FontBColor); 
        ListView_GetSubItemRect(m_hListWnd, lpDrawItem->itemID, 2, LVIR_BOUNDS, &rc);
        DrawText(hDc, szBoderColorValue, sizeof(szBoderColorValue), &rc, NULL);

		SetBkColor(hDc, FontPColor); 
		ListView_GetSubItemRect(m_hListWnd, lpDrawItem->itemID, 3, LVIR_BOUNDS, &rc);
		DrawText(hDc, szProjectionValue, sizeof(szProjectionValue), &rc, NULL);

        SetBkColor(hDc, bkBkcolor);
        
        if (lpDrawItem->itemState == ODS_SELECTED) 
        {
            SetBkColor(hDc, bkColor);
        }
        SetBkMode(hDc, TRANSPARENT);

    Exit0:
        return;
    }

    void KUiFontManage::OnUpdateEditText(int nId)
    {

    }


    void KUiFontManage::OnListNotify(LPNMHDR pNotify)
    {
	    switch(pNotify->code)
	    {
	    case LVN_ITEMACTIVATE:
	    case NM_CLICK:
		    {
			    LPNMITEMACTIVATE pActive = (LPNMITEMACTIVATE)pNotify;
			    if (pActive->iItem >= 0)
			    {
                    m_nCurrentItemNum = pActive->iItem;

                    char szName[8] = "";
                    char szValue[32] = "";
                    int  nValue = 0;
                    sprintf(szName, "%d", pActive->iItem);
                    if (m_pFontIni == NULL) 
                        break;
                    m_pFontIni->GetString(szName, "Name", "", szValue, sizeof(szValue));
                    ::SetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_NAME, szValue);
                    m_pFontIni->GetString(szName, "Color", "", szValue, sizeof(szValue));
                    ::SetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_COLOR, szValue);
                    m_pFontIni->GetString(szName, "BorderColor", "", szValue, sizeof(szValue));
                    ::SetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_BCOLOR, szValue);
					m_pFontIni->GetString(szName, "ProjectionColor", "", szValue, sizeof(szValue));
					::SetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_PCOLOR, szValue);
                    m_pFontIni->GetInteger(szName, "Size", 0, &nValue);
                    ::SetDlgItemInt(m_hWnd, IDC_WND_FONT_MANAGE_FONT_SIZE, nValue, false);
                    m_pFontIni->GetInteger(szName, "BorderSize", 0, &nValue);
                    ::SetDlgItemInt(m_hWnd, IDC_WND_FONT_MANAGE_FONT_BSIZE, nValue, false);
					m_pFontIni->GetInteger(szName, "ProjectionSize", 0, &nValue);
					::SetDlgItemInt(m_hWnd, IDC_WND_FONT_MANAGE_FONT_PSIZE, nValue, false);
                    m_pFontIni->GetInteger(szName, "FontID", 0, &nValue);
                    ::SetDlgItemInt(m_hWnd, IDC_WND_FONT_MANAGE_FONT_TYPE, nValue, false);


                    DRAWITEMSTRUCT stDrawItem;
                    stDrawItem.hDC = GetDC(m_hListWnd);
                    stDrawItem.itemID = pActive->iItem;
                    stDrawItem.itemState = ODS_SELECTED;
                    stDrawItem.CtlType = ODT_LISTVIEW;
                    stDrawItem.hwndItem = m_hListWnd;
                    SendMessage(GetParent(m_hListWnd), WM_DRAWITEM, (WPARAM)m_hListWnd, (LPARAM)&stDrawItem);
                  
			    }
		    }
		    break;
        case NM_RCLICK:
            {
			    LPNMITEMACTIVATE pActive = (LPNMITEMACTIVATE)pNotify;
			    if (pActive->iItem >= 0)
			    {
                    m_nCurrentItemNum = pActive->iItem;

                    char szName[8] = "";
                    char szValue[32] = "";
                    int  nValue = 0;
                    sprintf(szName, "%d", pActive->iItem);
                    if (m_pFontIni == NULL) 
                        break;
					m_pFontIni->GetString(szName, "Name", "", szValue, sizeof(szValue));
					::SetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_NAME, szValue);
					m_pFontIni->GetString(szName, "Color", "", szValue, sizeof(szValue));
					::SetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_COLOR, szValue);
					m_pFontIni->GetString(szName, "BorderColor", "", szValue, sizeof(szValue));
					::SetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_BCOLOR, szValue);
					m_pFontIni->GetString(szName, "ProjectionColor", "", szValue, sizeof(szValue));
					::SetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_PCOLOR, szValue);
					m_pFontIni->GetInteger(szName, "Size", 0, &nValue);
					::SetDlgItemInt(m_hWnd, IDC_WND_FONT_MANAGE_FONT_SIZE, nValue, false);
					m_pFontIni->GetInteger(szName, "BorderSize", 0, &nValue);
					::SetDlgItemInt(m_hWnd, IDC_WND_FONT_MANAGE_FONT_BSIZE, nValue, false);
					m_pFontIni->GetInteger(szName, "ProjectionSize", 0, &nValue);
					::SetDlgItemInt(m_hWnd, IDC_WND_FONT_MANAGE_FONT_PSIZE, nValue, false);
					m_pFontIni->GetInteger(szName, "FontID", 0, &nValue);
					::SetDlgItemInt(m_hWnd, IDC_WND_FONT_MANAGE_FONT_TYPE, nValue, false);


                    DRAWITEMSTRUCT stDrawItem;
                    stDrawItem.hDC = GetDC(m_hListWnd);
                    stDrawItem.itemID = pActive->iItem;
                    stDrawItem.itemState = ODS_SELECTED;
                    stDrawItem.CtlType = ODT_LISTVIEW;
                    stDrawItem.hwndItem = m_hListWnd;
                    SendMessage(GetParent(m_hListWnd), WM_DRAWITEM, (WPARAM)m_hListWnd, (LPARAM)&stDrawItem);

                    if (pActive->iSubItem == 1) 
                    {
                        OnChangeColor(0, 1);
                        SendMessage(GetParent(m_hListWnd), WM_DRAWITEM, (WPARAM)m_hListWnd, (LPARAM)&stDrawItem);
                    }

                    if (pActive->iSubItem == 2) 
                    {
                        OnChangeColor(1, 1);
                        SendMessage(GetParent(m_hListWnd), WM_DRAWITEM, (WPARAM)m_hListWnd, (LPARAM)&stDrawItem);
                    }

					if (pActive->iSubItem == 3) 
					{
						OnChangeColor(2, 1);
						SendMessage(GetParent(m_hListWnd), WM_DRAWITEM, (WPARAM)m_hListWnd, (LPARAM)&stDrawItem);
					}
                   
			    }
		    }
            break;
        case NM_DBLCLK:
            {
			    LPNMITEMACTIVATE pActive = (LPNMITEMACTIVATE)pNotify;
			    if (pActive->iItem >= 0 && pActive->iItem < ListView_GetItemCount(m_hListWnd) && !m_bManage)
			    {
                    m_nCurrentItemNum = pActive->iItem;
                    OnSave(m_szFontFileName);
                    m_bSel = TRUE;
                    PostMessage(m_hWnd, WM_CLOSE, 0, 0);
                }
            }
            break;

	    }
    }

    int KUiFontManage::IsFontExist(const char *pszNewFont)
    {
        int nResult = TRUE;

        KG_PROCESS_ERROR(pszNewFont && pszNewFont[0]);
        KG_PROCESS_ERROR(m_pFontIni);
        char szName[8] = "";
        char szNextName[8] = "";
        char szValue[32] = "";
        while (m_pFontIni->GetNextSection(szName, szNextName)) 
        {
            m_pFontIni->GetString(szNextName, "Name", "", szValue, sizeof(szValue));
            if (!strcmp(szValue, pszNewFont)) 
            {
                KG_PROCESS_ERROR(0);
            }
            strcpy(szName, szNextName);
        }

        nResult = FALSE;
    Exit0:
        return nResult;
    }


    void KUiFontManage::OnButtonClick(int nBtnId)
    {

	    switch(nBtnId)
	    {
            case IDC_WND_FONT_MANAGE_FONT_ADD:
                OnAdd();
                break;
            case IDC_WND_FONT_MANAGE_FONT_CHANGE:
                OnChange();
                break;
            case IDC_WND_FONT_MANAGE_FONT_SAVE:
                OnSave(m_szFontFileName);
                break;
            case IDC_WND_FONT_MANAGE_FONT_COLOR_SEL:
                OnChangeColor(0, 0);
                break;
            case IDC_WND_FONT_MANAGE_FONT_BCOLOR_SEL:
                OnChangeColor(1, 0);
                break;
			case IDC_WND_FONT_MANAGE_FONT_PCOLOR_SEL:
				OnChangeColor(2, 0);
				break;
            case IDC_WND_FONT_MANAGE_FONT_SEL:
                if (!m_bManage) 
                {
                    OnSave(m_szFontFileName);
                    m_bSel = TRUE;
                    PostMessage(m_hWnd, WM_CLOSE, 0, 0);
                }
                break;

            default:
                break;
	    }

    }


    void KUiFontManage::OnSave(const char *pszFontFileName)
    {
        KG_PROCESS_ERROR(pszFontFileName && pszFontFileName[0]);
        KG_PROCESS_ERROR(m_pFontIni);

        m_pFontIni->Save(pszFontFileName);

        //通知客户端重新载入字体
        g_pfnUIBehavior(PCB_UI_RELOAD_FONT_SCHEME, (WPARAM)0, (LPARAM)0);

    Exit0:
        return;

    }

    void KUiFontManage::Release()
    {
        if (m_hListWnd) 
        {
            ListView_DeleteAllItems(m_hListWnd);
            m_hListWnd = NULL;
        }
        
    }

    void KUiFontManage::OnChangeColor(int nIndex, BOOL bInIni)
    {
        KG_PROCESS_ERROR(m_hWnd);
        KG_PROCESS_ERROR(m_hListWnd);

        int nRetCode = false;
        char szColor[32] = "";
        nRetCode = UICOLORMANAGE::SelColor(m_hWnd, szColor);
        KG_PROCESS_ERROR(nRetCode);
        if (bInIni) 
        {
            char szName[8] = "";
            sprintf(szName, "%d", m_nCurrentItemNum);

			switch (nIndex)
			{
			case 0:
				ListView_SetItemText(m_hListWnd, m_nCurrentItemNum, 1, szColor);
				m_pFontIni->WriteString(szName, "Color", szColor);
				::SetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_COLOR, szColor);
				break;
			case 1:
				ListView_SetItemText(m_hListWnd, m_nCurrentItemNum, 2, szColor);
				m_pFontIni->WriteString(szName, "BorderColor", szColor);
				::SetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_BCOLOR, szColor);
				break;
			case 2:
				ListView_SetItemText(m_hListWnd, m_nCurrentItemNum, 2, szColor);
				m_pFontIni->WriteString(szName, "ProjectionSize", szColor);
				::SetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_PCOLOR, szColor);
				break;
			}
        }
        else
        {
			switch (nIndex)
			{
			case 0:
				::SetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_COLOR, szColor);
				break;
			case 1:
				::SetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_BCOLOR, szColor);
				break;
			case 2:
				::SetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_PCOLOR, szColor);
				break;
			}
        }

    Exit0:
        return;

    }

    void KUiFontManage::OnAdd()
    {
        KG_PROCESS_ERROR(m_hWnd);
        KG_PROCESS_ERROR(m_hListWnd);
        KG_PROCESS_ERROR(m_pFontIni);

        int nValue = 0; 
        char szValue[32] = "";
        char szName[8] = "";
        int nMaxNum = 0;
        nMaxNum = m_pFontIni->GetSectionCount();
        
        sprintf(szName, "%d", nMaxNum);
        ::GetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_NAME, szValue, sizeof(szValue));
        if (IsFontExist(szValue)) 
        {
            MessageBox(m_hWnd, "该字体已经存在", "添加失败", MB_OK | MB_ICONINFORMATION);
            KG_PROCESS_ERROR(0);
        }
        m_pFontIni->WriteString(szName, "Name", szValue);
        ::GetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_COLOR, szValue, sizeof(szValue));
        m_pFontIni->WriteString(szName, "Color", szValue);
        ::GetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_BCOLOR, szValue, sizeof(szValue));
        m_pFontIni->WriteString(szName, "BorderColor", szValue);
		::GetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_PCOLOR, szValue, sizeof(szValue));
		m_pFontIni->WriteString(szName, "ProjectionColor", szValue);

        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_FONT_MANAGE_FONT_SIZE,NULL, false);
        m_pFontIni->WriteInteger(szName, "Size", nValue);
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_FONT_MANAGE_FONT_BSIZE, NULL, false);
        m_pFontIni->WriteInteger(szName, "BorderSize", nValue);
		nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_FONT_MANAGE_FONT_PSIZE, NULL, false);
		m_pFontIni->WriteInteger(szName, "ProjectionSize", nValue);
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_FONT_MANAGE_FONT_TYPE, NULL, false);
        m_pFontIni->WriteInteger(szName, "FontID", nValue);

        m_pFontIni->GetString(szName, "Name", "", szValue, sizeof(szValue));
        LV_ITEM			lvi;
		memset(&lvi, 0, sizeof(LV_ITEM));
		lvi.mask = LVIF_TEXT;
		lvi.pszText		= (char*)szValue;
		lvi.cchTextMax  = 128;
		lvi.iItem       = ListView_GetItemCount(m_hListWnd);
		lvi.iItem		= ListView_InsertItem(m_hListWnd, &lvi);

        m_pFontIni->GetString(szName, "Color", "", szValue, sizeof(szValue));
        ListView_SetItemText(m_hListWnd, lvi.iItem, 1, szValue);
        m_pFontIni->GetString(szName, "BorderColor", "", szValue, sizeof(szValue));
        ListView_SetItemText(m_hListWnd, lvi.iItem, 2, szValue);
		m_pFontIni->GetString(szName, "ProjectionColor", "", szValue, sizeof(szValue));
		ListView_SetItemText(m_hListWnd, lvi.iItem, 3, szValue);
        
        m_nCurrentItemNum = lvi.iItem;

    Exit0:
        return;

    }

    void KUiFontManage::OnChange()
    {
        KG_PROCESS_ERROR(m_hWnd);
        KG_PROCESS_ERROR(m_hListWnd);
        KG_PROCESS_ERROR(m_pFontIni);

        int nValue = 0; 
        char szValue[32] = "";
        char szName[8] = "";
        
        sprintf(szName, "%d", m_nCurrentItemNum);
        ::GetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_NAME, szValue, sizeof(szValue));

        int nOringalId = -1;
        if (GetIdByName(szValue, &nOringalId)) 
        {
            if (IsFontExist(szValue) && nOringalId != m_nCurrentItemNum) 
            {
                MessageBox(m_hWnd, "该字体已经存在", "更改失败", MB_OK | MB_ICONINFORMATION);
                KG_PROCESS_ERROR(0);
            }
        }

        m_pFontIni->WriteString(szName, "Name", szValue);
        ::GetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_COLOR, szValue, sizeof(szValue));
        m_pFontIni->WriteString(szName, "Color", szValue);
        ::GetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_BCOLOR, szValue, sizeof(szValue));
        m_pFontIni->WriteString(szName, "BorderColor", szValue);
		::GetDlgItemText(m_hWnd, IDC_WND_FONT_MANAGE_FONT_PCOLOR, szValue, sizeof(szValue));
		m_pFontIni->WriteString(szName, "ProjectionColor", szValue);

        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_FONT_MANAGE_FONT_SIZE, NULL, false);
        m_pFontIni->WriteInteger(szName, "Size", nValue);
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_FONT_MANAGE_FONT_BSIZE, NULL, false);
        m_pFontIni->WriteInteger(szName, "BorderSize", nValue);
		nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_FONT_MANAGE_FONT_PSIZE, NULL, false);
		m_pFontIni->WriteInteger(szName, "ProjectionSize", nValue);

        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_FONT_MANAGE_FONT_TYPE, NULL, false);
        m_pFontIni->WriteInteger(szName, "FontID", nValue);

        m_pFontIni->GetString(szName, "Name", "", szValue, sizeof(szValue));
        ListView_SetItemText(m_hListWnd, m_nCurrentItemNum, 0, szValue);
        m_pFontIni->GetString(szName, "Color", "", szValue, sizeof(szValue));
        ListView_SetItemText(m_hListWnd, m_nCurrentItemNum, 1, szValue);
        m_pFontIni->GetString(szName, "BorderColor", "", szValue, sizeof(szValue));
        ListView_SetItemText(m_hListWnd, m_nCurrentItemNum, 2, szValue);
		m_pFontIni->GetString(szName, "ProjectionColor", "", szValue, sizeof(szValue));
		ListView_SetItemText(m_hListWnd, m_nCurrentItemNum, 3, szValue);

    Exit0:
        return;

    }

    int KUiFontManage::GetIdByName(const char *pszName, int *pnId)
    {
        int nReslut = FALSE;
        if (!m_pFontIni)
            ReInitIni();
        KG_PROCESS_ERROR(m_pFontIni);
        KG_PROCESS_ERROR(pszName && pszName[0]);
        KG_PROCESS_ERROR(pnId);

        int nMax = m_pFontIni->GetSectionCount();
        int i = 0;
        char szName[8] = "";
        char szValue[32] = "";
        for (i = 0; i < nMax; i++) 
        {
            sprintf(szName, "%d", i);
            m_pFontIni->GetString(szName, "Name", "", szValue, sizeof(szValue));
            if (!strcmp(szValue, pszName)) 
            {
                *pnId = i;
                nReslut = TRUE;
                KG_PROCESS_ERROR(0);
            }
        }
    
    Exit0:
        return nReslut;        
    }

    int KUiFontManage::GetNameById(int nId, char *pszName)
    {
        int nReslut = FALSE;
        if (!m_pFontIni)
            ReInitIni();
        KG_PROCESS_ERROR(m_pFontIni);
        KG_PROCESS_ERROR(pszName);
        int nMax = m_pFontIni->GetSectionCount();
        KG_PROCESS_ERROR(nId < nMax && nId >= 0);
        char szValue[32] = "";
        char szName[8]  = "";
        sprintf(szName, "%d", nId);
        m_pFontIni->GetString(szName, "Name", "", szValue, sizeof(szValue));
        strcpy(pszName, szValue);
    
        nReslut = TRUE;
    Exit0:
        return nReslut;
      
    }

    int KUiFontManage::ReInitIni()
    {
        if (m_pFontIni)
            return true;
        char szPath[MAX_PATH] = "";
        g_GetRootPath(szPath);
        strcat(szPath, "\\UI\\Scheme\\Elem\\Font.ini");
        strcpy(m_szFontFileName, szPath);
        m_pFontIni = g_OpenIniFile(m_szFontFileName);

        return true;
    }

    void KUiFontManage::OnFontManage(HWND hParent, BOOL bManage)
    {

        m_nCurrentItemNum = 0;
        m_hWnd = NULL;
        m_hListWnd = NULL;
        m_szFont[0] = 0;
        m_pFontIni = NULL;
        m_bSel = FALSE;
        m_bManage = bManage;

        char szPath[MAX_PATH] = "";
        g_GetRootPath(szPath);
        strcat(szPath, "\\UI\\Scheme\\Elem\\Font.ini");
        strcpy(m_szFontFileName, szPath);
        m_pFontIni = g_OpenIniFile(m_szFontFileName);

        KG_PROCESS_ERROR(hParent);
        ::DialogBox((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDD_WndFontManage), hParent, (DLGPROC)DlgProc);

    Exit0:
        return;
    }



    int FontManage(HWND hWnd)
    {
        FontManager.OnFontManage(hWnd, TRUE);

        return true;
    }

    int SelFont(HWND hWnd, int *pnFont)
    {
        FontManager.OnFontManage(hWnd, FALSE);

        if (FontManager.IsSeled() && pnFont) 
        {
            *pnFont = FontManager.GetFont();
            return TRUE;
        }
        else
        {
            return FALSE;
        }
                 
    }

    int GetNameById(int nId, char *pszName)
    {
        return FontManager.GetNameById(nId, pszName);
    }
};
