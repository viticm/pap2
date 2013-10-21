#include "Stdafx.h"
#include"UiColorManage.h"
#include "UiEditor.h"
#include "resource.h"

namespace UICOLORMANAGE
{

    static KUiColorManage ColorManager;

    KUiColorManage::KUiColorManage()
    {   
        m_nCurrentItemNum = 0;
        m_hWnd = NULL;
        m_szColorFileName[0] = 0;
        m_hListWnd = NULL;
        m_szColor[0] = 0;
        m_bManage = FALSE;

    }
    KUiColorManage::~KUiColorManage()
    {
	    if (m_hWnd)
	    {
		    ::DestroyWindow(m_hWnd);
		    m_hWnd = NULL;
	    }
    }

    int KUiColorManage::Init(HWND hWnd)
    {
        int nResult  = false;
        int nRetCode = false;

        KG_PROCESS_ERROR(hWnd);
        m_hWnd = hWnd;

        RECT rcWindow;
        ::GetWindowRect(m_hWnd, &rcWindow);

        ::SetWindowPos(m_hWnd, HWND_TOP, 400, 200,
		    rcWindow.right -rcWindow.left, rcWindow.bottom - rcWindow.top,
		    SWP_SHOWWINDOW);

	    m_hListWnd = ::GetDlgItem(hWnd, IDC_WND_COLOR_MANAGE_COLOR_LIST);
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
        strcpy(szHeaderText, "颜色名:");
	    ListView_InsertColumn(m_hListWnd, 0, &lvc);

        lvc.cx = (rc.right - rc.left)  / 3;
	    lvc.mask |= LVCF_SUBITEM;
	    lvc.iSubItem = 1;
	    strcpy(szHeaderText, "颜色值");
	    ListView_InsertColumn(m_hListWnd, 1, &lvc);

        lvc.cx = (rc.right - rc.left)  / 3;
	    lvc.mask |= LVCF_SUBITEM;
	    lvc.iSubItem = 1;
	    strcpy(szHeaderText, "颜色");
	    ListView_InsertColumn(m_hListWnd, 2, &lvc);  


		/*
        {
            int r , g, b;
            char szValue[128] = "";
            char szColorValue[128] = "";
            ifstream ReadColor(m_szColorFileName);

            ReadColor>>szValue;
            ReadColor>>szValue;
            ReadColor>>szValue;
            ReadColor>>szValue;
            while (!ReadColor.eof()) 
            {
                ReadColor>>szValue;
                if (!szValue[0]) 
                {
                    break;
                }
                ReadColor>>r;
                ReadColor>>g;
                ReadColor>>b;
                sprintf(szColorValue, "%d,%d,%d", r, g, b);

                LV_ITEM			lvi;
		        memset(&lvi, 0, sizeof(LV_ITEM));
		        lvi.mask = LVIF_TEXT;
		        lvi.pszText		= (char*)szValue;
		        lvi.cchTextMax  = 128;
		        lvi.iItem       = ListView_GetItemCount(m_hListWnd);
		        lvi.iItem		= ListView_InsertItem(m_hListWnd, &lvi);

                ListView_SetItemText(m_hListWnd, lvi.iItem, 1, szColorValue);
                ListView_SetItemText(m_hListWnd, lvi.iItem, 2, "          ");
            }

            ReadColor.close();
        }
		*/

		{
			FILE *fpColorFile = NULL;
			char szValue[128] = "";
			char szColorValue[128] = "";
			int r,g,b;

			fpColorFile = fopen(m_szColorFileName, "r");
			KG_PROCESS_ERROR(fpColorFile);

			fscanf(fpColorFile, "%s\t%s\t%s\t%s\n", szValue, szValue, szValue, szValue);

			szValue[0] = 0;
			while (!feof(fpColorFile)) 
			{
				fscanf(fpColorFile, "%s", szValue);
				fscanf(fpColorFile, "\t%d\t%d\t%d\n", &r, &g, &b);
				sprintf(szColorValue, "%d,%d,%d", r, g, b);

				LV_ITEM			lvi;
				memset(&lvi, 0, sizeof(LV_ITEM));
				lvi.mask = LVIF_TEXT;
				lvi.pszText		= (char*)szValue;
				lvi.cchTextMax  = 128;
				lvi.iItem       = ListView_GetItemCount(m_hListWnd);
				lvi.iItem		= ListView_InsertItem(m_hListWnd, &lvi);

				ListView_SetItemText(m_hListWnd, lvi.iItem, 1, szColorValue);
				ListView_SetItemText(m_hListWnd, lvi.iItem, 2, "          ");
			}
			fclose(fpColorFile);
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
            ColorManager.Init(hWnd);
        	break;
        case WM_CLOSE:
            ColorManager.Release();
            EndDialog(hWnd, FALSE);
            nResult = TRUE;
            break;
        case WM_DRAWITEM:
            ColorManager.Draw((LPDRAWITEMSTRUCT)lParam);
            break;
        case WM_NOTIFY:
            {
		        LPNMHDR pNotify = (LPNMHDR)lParam;
		        if (pNotify->hwndFrom == ColorManager.GetListHandle())
			        ColorManager.OnListNotify(pNotify);
            }
            break;
        case WM_COMMAND:
	        {
		        int nNotify = HIWORD(wParam);
		        switch(nNotify)
		        {
		        case EN_CHANGE:
                    ColorManager.OnUpdateEditText(LOWORD(wParam));
			        break;
		        case 0:
                    ColorManager.OnButtonClick(LOWORD(wParam));
			        break;
		        }
	        }
            break;
        default:
            break;
        }

	    return nResult;
    }

    void KUiColorManage::Draw(LPDRAWITEMSTRUCT lpDrawItem)
    {
        KG_PROCESS_ERROR(lpDrawItem->CtlType == ODT_LISTVIEW && lpDrawItem->hwndItem == m_hListWnd);

        HDC hDc = lpDrawItem->hDC;
        int r, g, b;
        COLORREF bkColor;
        char szValue[128] = "";
        char szColorValue[128] = "";
        ListView_GetItemText(m_hListWnd, lpDrawItem->itemID, 0, szValue, sizeof(szValue));
        ListView_GetItemText(m_hListWnd, lpDrawItem->itemID, 1, szColorValue, sizeof(szColorValue));
        sscanf(szColorValue, "%d,%d,%d", &r, &g, &b);
        
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

        ListView_GetSubItemRect(m_hListWnd, lpDrawItem->itemID, 1, LVIR_BOUNDS, &rc);

        DrawText(hDc, szColorValue, sizeof(szColorValue), &rc, NULL);


        SetTextColor(hDc, RGB(r, g, b));
        COLORREF bkBkcolor =SetBkColor(hDc, RGB(r, g, b)); 
        ListView_GetSubItemRect(m_hListWnd, lpDrawItem->itemID, 2, LVIR_BOUNDS, &rc);
        DrawText(hDc, "                            ", 128, &rc, NULL);
        SetBkColor(hDc, bkBkcolor);
        
        if (lpDrawItem->itemState == ODS_SELECTED) 
        {
            SetBkColor(hDc, bkColor);
        }
        SetBkMode(hDc, TRANSPARENT);

    Exit0:
        return;
    }

    void KUiColorManage::OnUpdateEditText(int nId)
    {

    }


    void KUiColorManage::OnListNotify(LPNMHDR pNotify)
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
                    char szColorName[32] = "";
                    ListView_GetItemText(m_hListWnd, pActive->iItem, 0, szColorName, sizeof(szColorName));
                    ::SetDlgItemText(m_hWnd, IDC_WND_COLOR_MANAGE_COLOR_NAME, szColorName);

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
                    char szColorName[32] = "";
                    ListView_GetItemText(m_hListWnd, pActive->iItem, 0, szColorName, sizeof(szColorName));
                    ::SetDlgItemText(m_hWnd, IDC_WND_COLOR_MANAGE_COLOR_NAME, szColorName);

                    DRAWITEMSTRUCT stDrawItem;
                    stDrawItem.hDC = GetDC(m_hListWnd);
                    stDrawItem.itemID = pActive->iItem;
                    stDrawItem.itemState = ODS_SELECTED;
                    stDrawItem.CtlType = ODT_LISTVIEW;
                    stDrawItem.hwndItem = m_hListWnd;
                    SendMessage(GetParent(m_hListWnd), WM_DRAWITEM, (WPARAM)m_hListWnd, (LPARAM)&stDrawItem);

                    if (pActive->iSubItem == 2) 
                    {
                        OnChangeColor();
                        SendMessage(GetParent(m_hListWnd), WM_DRAWITEM, (WPARAM)m_hListWnd, (LPARAM)&stDrawItem);
                    }        
			    }
		    }
            break;
        case NM_DBLCLK:
            {
                LPNMITEMACTIVATE pActive = (LPNMITEMACTIVATE)pNotify;
                if (pActive->iItem >= 0 &&pActive->iItem < ListView_GetItemCount(m_hListWnd) && !m_bManage) 
                {
                    ListView_GetItemText(m_hListWnd, m_nCurrentItemNum, 0, m_szColor, sizeof(m_szColor));
                    OnSave(m_szColorFileName);
                    PostMessage(m_hWnd, WM_CLOSE, 0, 0);               
                }
            }
            break;

	    }
    }

    int KUiColorManage::IsColorExist(const char *pszNewColor)
    {
        int nTotalNum = ListView_GetItemCount(m_hListWnd);
        char szColorName[32] = "";
        int i = 0;
        for (i = 0; i < nTotalNum; i++) 
        {
            ListView_GetItemText(m_hListWnd, i, 0, szColorName, sizeof(szColorName));
            if (!strcmp(pszNewColor, szColorName)) 
            {
                return TRUE;
            }
        }

        return FALSE;
    }

    void KUiColorManage::OnButtonClick(int nBtnId)
    {

	    switch(nBtnId)
	    {
            case IDC_WND_COLOR_MANAGE_ADD:
                {
                    char szNewColor[32] = "";
                    ::GetDlgItemText(m_hWnd, IDC_WND_COLOR_MANAGE_COLOR_NAME, szNewColor, sizeof(szNewColor));
                    if (!IsColorExist(szNewColor)) 
                    {
             		    LV_ITEM			lvi;
		                memset(&lvi, 0, sizeof(LV_ITEM));
		                lvi.mask = LVIF_TEXT;
		                lvi.pszText		= (char*)szNewColor;
		                lvi.cchTextMax  = 128;
		                lvi.iItem       = ListView_GetItemCount(m_hListWnd);
		                lvi.iItem		= ListView_InsertItem(m_hListWnd, &lvi);

		                ListView_SetItemText(m_hListWnd, lvi.iItem, 1, "0,0,0");

                        m_nCurrentItemNum = lvi.iItem;
                    }
                    else
                    {
                        MessageBox(m_hWnd, "该颜色已经存在", "添加颜色失败", MB_OK | MB_ICONINFORMATION);
                    }
                }
                break;
            case IDC_WND_COLOR_MANAGE_CHANGE:
                {
                    char szNewColor[32] = "";
                    ::GetDlgItemText(m_hWnd, IDC_WND_COLOR_MANAGE_COLOR_NAME, szNewColor, sizeof(szNewColor));

                    if (!IsColorExist(szNewColor)) 
                    {
		                ListView_SetItemText(m_hListWnd, m_nCurrentItemNum, 0, szNewColor);        
                    }
                    else
                    {
                        MessageBox(m_hWnd, "该颜色已经存在", "更改颜色失败", MB_OK | MB_ICONINFORMATION);
                    }
                }
                break;
            case IDC_WND_COLOR_MANAGE_SAVE:
                OnSave(m_szColorFileName);
                break;
            case IDC_WND_COLOR_MANAGE_SEL:
                OnChangeColor();
                break;
            case IDC_WND_COLOR_MANAGE_COLOR_SEL:
                if (!m_bManage) 
                {
                    ListView_GetItemText(m_hListWnd, m_nCurrentItemNum, 0, m_szColor, sizeof(m_szColor));
                    OnSave(m_szColorFileName);
                    PostMessage(m_hWnd, WM_CLOSE, 0, 0);
                }
                break;
            default:
                break;
	    }

    }

    void KUiColorManage::OnSave(const char *pszColorFileName)
    {
        FILE *fpColorFile = NULL;
        int nMaxCount = ListView_GetItemCount(m_hListWnd);
        int i = 0;
        char szValue[32] = "";
        int r,g,b;
        fpColorFile = fopen(pszColorFileName, "w");
        if (!fpColorFile) 
        {
            return;
        }

        fprintf(fpColorFile, "name\tr\tg\tb\n");
        for (i = 0; i < nMaxCount; i++) 
        {
            ListView_GetItemText(m_hListWnd, i, 0, szValue, sizeof(szValue));
            fprintf(fpColorFile, szValue);
            ListView_GetItemText(m_hListWnd, i, 1, szValue, sizeof(szValue));
            sscanf(szValue, "%d,%d,%d", &r,&g,&b);
            fprintf(fpColorFile, "\t%d\t%d\t%d\n", r, g, b);
        }
		fflush(fpColorFile);
        fclose(fpColorFile);
		fpColorFile = NULL;

		LoadColorArray(pszColorFileName);
        //让客户端重新载入颜色列表
        g_pfnUIBehavior(PCB_UI_RELOAD_COLOR_SCHEME, (WPARAM)0, (LPARAM)0);
    }

    void KUiColorManage::Release()
    {
        if (m_hListWnd) 
        {
            ListView_DeleteAllItems(m_hListWnd);
            m_hListWnd = NULL;
        }
        
    }

    void KUiColorManage::OnChangeColor()
    {

	    CHOOSECOLOR		color = { 0 };

	    color.lStructSize = sizeof(CHOOSECOLOR);
	    color.hwndOwner   = m_hWnd;
	    color.Flags = CC_RGBINIT | CC_FULLOPEN;
        color.lpCustColors = KUiEditor::CustClr;

        char szOringalColor[32] = "";
	    int	rgb[3] = { 0, 0, 0 };

        ListView_GetItemText(m_hListWnd, m_nCurrentItemNum, 1, szOringalColor, sizeof(szOringalColor));
        sscanf(szOringalColor, "%d,%d,%d", &rgb[0], &rgb[1], &rgb[2]);

	    color.rgbResult = (rgb[0] | (rgb[1] << 8) | (rgb[2] << 16));
	    if (!ChooseColor(&color))
		    return;

	    rgb[0] =  (color.rgbResult & 0xff);
	    rgb[1] =  ((color.rgbResult >> 8) & 0xff);
	    rgb[2] =  ((color.rgbResult >> 16) & 0xff);
	    char	szColor[32];
	    sprintf(szColor, "%d,%d,%d", rgb[0], rgb[1], rgb[2]);

        ListView_SetItemText(m_hListWnd, m_nCurrentItemNum, 1, szColor);

    }

    void KUiColorManage::OnColorManage(char *pszColorFileName, HWND hParent, BOOL bManage)
    {
        KG_PROCESS_ERROR(pszColorFileName);

        m_nCurrentItemNum = 0;
        m_hWnd = NULL;
        m_szColorFileName[0] = 0;
        m_hListWnd = NULL;
        m_szColor[0] = 0;
        m_bManage = bManage;
        
        strcpy(m_szColorFileName, pszColorFileName);
        KG_PROCESS_ERROR(hParent);
        ::DialogBox((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDD_WndColorManage), hParent, (DLGPROC)DlgProc);

    Exit0:
        return;
    }

    int ColorManage(HWND hWnd)
    {
          char szPath[MAX_PATH] = "";//"D:\\KingSoft\\BlackHole\\Client\\Debug\\UI\\Scheme\\Elem\\Color.txt";
          g_GetRootPath(szPath);
		  //::GetCurrentDirectory(sizeof(szPath), szPath);
          strcat(szPath, "\\UI\\Scheme\\Elem\\Color.txt");
          ColorManager.OnColorManage(szPath, hWnd, TRUE);

          return true;
    }

    int SelColor(HWND hWnd, char *pszColor)
    {
          char szPath[MAX_PATH] = "";//"D:\\KingSoft\\BlackHole\\Client\\Debug\\UI\\Scheme\\Elem\\Color.txt";
		  //::GetCurrentDirectory(sizeof(szPath), szPath);
          g_GetRootPath(szPath);
		  strcat(szPath, "\\UI\\Scheme\\Elem\\Color.txt");
          ColorManager.OnColorManage(szPath, hWnd);
          
          char *pClip = ColorManager.GetColor();
          if (pClip && *pClip) 
          {
              strcpy(pszColor, pClip);
              return TRUE;
          }
          else
          {
              return FALSE;
          }
          
    }

	struct KCOLORBLOCK
	{
		char szName[32];
		int r;
		int g;
		int b;
	};

	static std::vector<KCOLORBLOCK> g_aColor;

	void ClearColorArray()
	{
		g_aColor.clear();
	}

	int LoadColorArray(const char *szPath)
	{
		KCOLORBLOCK colorBlock;
		int nResult = false;
		FILE *fpColorFile = NULL;
		fpColorFile = fopen(szPath, "r");
		KG_PROCESS_ERROR(fpColorFile);
		
		ClearColorArray();
		fscanf(fpColorFile, "%s\t%s\t%s\t%s\n", colorBlock.szName, colorBlock.szName, 
			colorBlock.szName, colorBlock.szName);
		while (!feof(fpColorFile)) 
		{
			fscanf(fpColorFile, "%s\t%d\t%d\t%d\n", colorBlock.szName, 
				&colorBlock.r, &colorBlock.g, &colorBlock.b);
			g_aColor.push_back(colorBlock);
		}
		

		nResult = true;
Exit0:
		if (fpColorFile)
		{
			fclose(fpColorFile);
			fpColorFile = NULL;
		}
		return nResult;
	}

    COLORREF GetColorValueByName(const char *pszColorName)
    {
        COLORREF Result = 0;

        KG_PROCESS_ERROR(pszColorName && pszColorName[0]);

		if (!g_aColor.size())
		{
			char szPath[MAX_PATH] = "";
			g_GetRootPath(szPath);
			strcat(szPath, "\\UI\\Scheme\\Elem\\Color.txt");
			LoadColorArray(szPath);
		}

		{
			std::vector<KCOLORBLOCK>::iterator it = g_aColor.begin();
			std::vector<KCOLORBLOCK>::iterator ie = g_aColor.end();
			for (; it != ie; ++it)
			{
				if (strcmp((*it).szName, pszColorName) == 0)
				{
					Result = RGB((*it).r, (*it).g, (*it).b);
					break;
				}
			}
		}

    Exit0:
        return Result;
    }
}
