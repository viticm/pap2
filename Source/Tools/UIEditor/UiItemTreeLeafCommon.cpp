////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemTreeLeafCommon.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-5 15:27:12
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include <Windows.h>
#include <tchar.h>

#include "UiItemTreeLeafCommon.h"
#include "UiItemImageCommon.h"
#include "UiEditor.h"
#include "resource.h"
#include "UiItemCommon.h"
#include "ClientManage.h"
#include "UiWndSelTgaFram.h"
#include "UiColorManage.h"

////////////////////////////////////////////////////////////////////////////////

char KUiItemTreeLeafCommon::ms_szLastImgPath[MAX_PATH] = "";


KUiItemTreeLeafCommon::KUiItemTreeLeafCommon()
{
    m_szName[0] = '\0';
}

KUiItemTreeLeafCommon::~KUiItemTreeLeafCommon()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow *KUiItemTreeLeafCommon::Create()
{
	KUiItemTreeLeafCommon* pWnd = new KUiItemTreeLeafCommon;
	return pWnd;
}

int KUiItemTreeLeafCommon::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult = false;
    HWND hwndCombo = NULL;
    HWND hSpin = NULL;
    KG_PROCESS_ERROR(hWnd);
    KG_PROCESS_ERROR(pRc);
    KG_PROCESS_ERROR(pszName);

    strncpy(m_szName, pszName, sizeof(m_szName));
    m_hWnd = ::CreateDialog(
        (HINSTANCE)g_hPluginInstance, 
        MAKEINTRESOURCE(IDD_ItemTreeLeafCommon), 
        hWnd, (DLGPROC)WndProc
    );
    KG_PROCESS_ERROR(m_hWnd);

	::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top,
		pRc->right - pRc->left, pRc->bottom - pRc->top,
		SWP_SHOWWINDOW);
	::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

    hSpin = ::GetDlgItem(m_hWnd, IDC_ITEM_TREELEAF_ICON_WIDTH_CHANGE);
    if (hSpin) 
        ::SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, 0));
    hSpin = ::GetDlgItem(m_hWnd, IDC_ITEM_TREELEAF_ICON_HEIGHT_CHANGE);
    if (hSpin) 
        ::SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, 0));
    hSpin = ::GetDlgItem(m_hWnd, IDC_ITEM_TREELEAF_INDENT_WIDTH_EACH_CHANGE);
    if (hSpin) 
        ::SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, 0));
    hSpin = ::GetDlgItem(m_hWnd, IDC_ITEM_TREELEAF_INDENT_CHANGE);
    if (hSpin) 
        ::SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, 0));

    nResult = true;
Exit0:
    if (!nResult)
        Release();
    return nResult;
}

void KUiItemTreeLeafCommon::Release()
{
    delete this;
}

LRESULT CALLBACK KUiItemTreeLeafCommon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KUiItemTreeLeafCommon*	pView = (KUiItemTreeLeafCommon*)GetWindowLong(hWnd, GWL_USERDATA);
	if (!pView)
		return 0;
	if (message == WM_COMMAND)
	{
		int nNotify = HIWORD(wParam);
		switch(nNotify)
		{
		case EN_CHANGE:
			pView->OnUpdateEditText(LOWORD(wParam));
			break;
		case 0:
			pView->OnButtonClick(LOWORD(wParam));
			break;
		}
	}
	return 0;
}

void KUiItemTreeLeafCommon::OnButtonClick(int nBtnId)
{
    int nUpdateShower = true;
    int nRetCode = false;
    int nValue = 0;
    switch(nBtnId)
    {
    case IDC_ITEM_TREELEAF_IMAGE_NAME_SEL:
        OnChangeImage();
        break;
    case IDC_ITEM_TREELEAF_IMAGE_NAME_CLEAR:
        g_Ui.GetIni(true)->WriteString(m_szName, "IconImage", "");
        ::SetDlgItemText(m_hWnd, IDC_ITEM_TREELEAF_IMAGE_NAME, "");
        break;
    case IDC_ITEM_TREELEAF_EXPAND_FRAME_SEL:
        {
            char szUiTexFileName[MAX_PATH];
            char szRoot[MAX_PATH];
            int nSelFram     = 0;
            int nFrameHeight = 0;
            int nFrameWidth  = 0;

            g_GetRootPath(szRoot);
            if (!szRoot[0]) 
                break;
            szUiTexFileName[0] = 0;
            g_Ui.GetIni()->GetString(m_szName, "IconImage", "", szUiTexFileName, sizeof(szUiTexFileName));
            if (!szUiTexFileName[0]) 
                break;
            strcat(szRoot, "\\");
            strcat(szRoot, szUiTexFileName);
            strcpy(szUiTexFileName, szRoot);
            nSelFram = SELTGAFRAME::OnSelTgaFram(szUiTexFileName, m_hWnd, &nFrameWidth, &nFrameHeight);           
            if (nSelFram >= 0)
            {
                g_Ui.GetIni()->WriteInteger(m_szName, "ExpandIconFrame", nSelFram);
                ::SetDlgItemInt(m_hWnd, IDC_ITEM_TREELEAF_EXPAND_FRAME, nSelFram, true);
            }
        }
        break;
    case IDC_ITEM_TREELEAF_COLLASPE_FRAME_SEL:
        {
            char szUiTexFileName[MAX_PATH];
            char szRoot[MAX_PATH];
            int nSelFram     = 0;
            int nFrameHeight = 0;
            int nFrameWidth  = 0;

            g_GetRootPath(szRoot);
            if (!szRoot[0]) 
                break;
            szUiTexFileName[0] = 0;
            g_Ui.GetIni()->GetString(m_szName, "IconImage", "", szUiTexFileName, sizeof(szUiTexFileName));
            if (!szUiTexFileName[0]) 
                break;
            strcat(szRoot, "\\");
            strcat(szRoot, szUiTexFileName);
            strcpy(szUiTexFileName, szRoot);
            nSelFram = SELTGAFRAME::OnSelTgaFram(szUiTexFileName, m_hWnd, &nFrameWidth, &nFrameHeight);           
            if (nSelFram >= 0)
            {
                g_Ui.GetIni()->WriteInteger(m_szName, "CollapseIconFrame", nSelFram);
                ::SetDlgItemInt(m_hWnd, IDC_ITEM_TREELEAF_COLLASPE_FRAME, nSelFram, true);
            }
        }
        break;
    case IDC_ITEM_TREELEAF_AUTOSIZE:
        {
            nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_ITEM_TREELEAF_AUTOSIZE);
            if (nRetCode)
            {
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TREELEAF_ICON_WIDTH), FALSE);
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TREELEAF_ICON_HEIGHT), FALSE);
                g_Ui.GetIni()->EraseKey(m_szName, "IconWidth");
                g_Ui.GetIni()->EraseKey(m_szName, "IconHeight");
            }
            else
            {
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TREELEAF_ICON_WIDTH), TRUE);
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TREELEAF_ICON_HEIGHT), TRUE);
                nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_TREELEAF_ICON_WIDTH, NULL, false);
                g_Ui.GetIni()->WriteInteger(m_szName, "IconWidth", nValue);
                nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_TREELEAF_ICON_HEIGHT, NULL, false);
                g_Ui.GetIni()->WriteInteger(m_szName, "IconHeight", nValue);
            }
        }
        break;
    case IDC_ITEM_TREELEAF_SHOW_LINE:
        nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_ITEM_TREELEAF_SHOW_LINE);
        if (nRetCode)
            g_Ui.GetIni()->WriteInteger(m_szName, "ShowLine", 1);
        else
            g_Ui.GetIni()->WriteInteger(m_szName, "ShowLine", 0);

        break;
	case IDC_ITEM_TREELEAF_ALWAYS_NODE:
		nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_ITEM_TREELEAF_ALWAYS_NODE);
		if (nRetCode)
			g_Ui.GetIni()->WriteInteger(m_szName, "AlwaysNode", 1);
		else
			g_Ui.GetIni()->WriteInteger(m_szName, "AlwaysNode", 0);

		break;
    case IDC_ITEM_TREELEAF_LINE_COLOR_SEL:
        {
            char szValue[32];
            szValue[0] = '\0';
            if (UICOLORMANAGE::SelColor(m_hWnd, szValue)) 
            {
                ::SetDlgItemText(m_hWnd, IDC_ITEM_TREELEAF_LINE_COLOR, szValue);
                g_Ui.GetIni(true)->WriteString(m_szName, "LineColor", szValue);
            }
        }
        break;
    default:
        nUpdateShower = false;
        break;
    }
    if (nUpdateShower)
    {
        g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
        ShowErrorInformation();
    }
}

void KUiItemTreeLeafCommon::OnUpdateEditText(int nId)
{
    int nValue = 0;
    char szValue[MAX_PATH];
    szValue[0] = '\0';
    int nUpdateShower = true;
    
    switch(nId)
    {
    case IDC_ITEM_TREELEAF_IMAGE_NAME:
        ::GetDlgItemText(m_hWnd, IDC_ITEM_TREELEAF_IMAGE_NAME, szValue, sizeof(szValue));
        g_Ui.GetIni()->WriteString(m_szName, "IconImage", szValue);
        break;
    case IDC_ITEM_TREELEAF_EXPAND_FRAME:
        nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_TREELEAF_EXPAND_FRAME, NULL, true);
        g_Ui.GetIni()->WriteInteger(m_szName, "ExpandIconFrame", nValue);
        break;
    case IDC_ITEM_TREELEAF_COLLASPE_FRAME:
        nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_TREELEAF_COLLASPE_FRAME, NULL, true);
        g_Ui.GetIni()->WriteInteger(m_szName, "CollapseIconFrame", nValue);
        break;
    case IDC_ITEM_TREELEAF_ICON_WIDTH:
        nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_TREELEAF_ICON_WIDTH, NULL, true);
        g_Ui.GetIni()->WriteInteger(m_szName, "IconWidth", nValue);
        break;
    case IDC_ITEM_TREELEAF_ICON_HEIGHT:
        nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_TREELEAF_ICON_HEIGHT, NULL, true);
        g_Ui.GetIni()->WriteInteger(m_szName, "IconHeight", nValue);
        break;
    case IDC_ITEM_TREELEAF_INDENT_WIDTH_EACH:
        nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_TREELEAF_INDENT_WIDTH_EACH, NULL, true);
        g_Ui.GetIni()->WriteInteger(m_szName, "IndentWidth", nValue);
        break;
    case IDC_ITEM_TREELEAF_INDENT:
        nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_TREELEAF_INDENT, NULL, true);
        g_Ui.GetIni()->WriteInteger(m_szName, "Indent", nValue);
        break;
    case IDC_ITEM_TREELEAF_LINE_COLOR:
        ::GetDlgItemText(m_hWnd, IDC_ITEM_TREELEAF_LINE_COLOR, szValue, sizeof(szValue));
        g_Ui.GetIni()->WriteString(m_szName, "LineColor", szValue);
        break;
    default:
        nUpdateShower = false;
        break;
    }

    if (nUpdateShower)
    {
        g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
        ShowErrorInformation();
    }
}

void KUiItemTreeLeafCommon::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
    int nRetCode = false;
    int nValue = 0;
    char szImage[MAX_PATH];
    KG_PROCESS_ERROR(pszName);
    KG_PROCESS_ERROR(pIni);

    strncpy(m_szName, pszName, sizeof(m_szName));

    pIni->GetString(m_szName, "IconImage", "", szImage, sizeof(szImage));
    ::SetDlgItemText(m_hWnd, IDC_ITEM_TREELEAF_IMAGE_NAME, szImage);
    pIni->GetInteger(m_szName, "ExpandIconFrame", -1, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_ITEM_TREELEAF_EXPAND_FRAME, nValue, TRUE);
    pIni->GetInteger(m_szName, "CollapseIconFrame", -1, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_ITEM_TREELEAF_COLLASPE_FRAME, nValue, TRUE);


    nRetCode = pIni->GetInteger(pszName, "IconWidth", 0, &nValue);
    if (nRetCode)
    {
        ::CheckDlgButton(m_hWnd, IDC_ITEM_TREELEAF_AUTOSIZE, BST_UNCHECKED);
        ::SetDlgItemInt(m_hWnd, IDC_ITEM_TREELEAF_ICON_WIDTH, nValue, false);
    }
    else
        ::CheckDlgButton(m_hWnd, IDC_ITEM_TREELEAF_AUTOSIZE, BST_CHECKED);

    nRetCode = pIni->GetInteger(pszName, "IconHeight", 0, &nValue);
    if (nRetCode)
    {
        ::CheckDlgButton(m_hWnd, IDC_ITEM_TREELEAF_AUTOSIZE, BST_UNCHECKED);
        ::SetDlgItemInt(m_hWnd, IDC_ITEM_TREELEAF_ICON_HEIGHT, nValue, false);
    }
    else
        ::CheckDlgButton(m_hWnd, IDC_ITEM_TREELEAF_AUTOSIZE, BST_CHECKED);
    OnButtonClick(IDC_ITEM_TREELEAF_AUTOSIZE);

    pIni->GetInteger(m_szName, "IndentWidth", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_ITEM_TREELEAF_INDENT_WIDTH_EACH, nValue, TRUE);

    pIni->GetInteger(m_szName, "Indent", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_ITEM_TREELEAF_INDENT, nValue, TRUE);

    pIni->GetString(m_szName, "LineColor", "", szImage, sizeof(szImage));
    ::SetDlgItemText(m_hWnd, IDC_ITEM_TREELEAF_LINE_COLOR, szImage);

    pIni->GetInteger(pszName, "ShowLine", 1, &nValue);
    if (nValue)
        ::CheckDlgButton(m_hWnd, IDC_ITEM_TREELEAF_SHOW_LINE, BST_CHECKED);
    else
        ::CheckDlgButton(m_hWnd, IDC_ITEM_TREELEAF_SHOW_LINE, BST_UNCHECKED);

	pIni->GetInteger(pszName, "AlwaysNode", 0, &nValue);
	if (nValue)
		::CheckDlgButton(m_hWnd, IDC_ITEM_TREELEAF_ALWAYS_NODE, BST_CHECKED);
	else
		::CheckDlgButton(m_hWnd, IDC_ITEM_TREELEAF_ALWAYS_NODE, BST_UNCHECKED);

Exit0:
    return;
}

void KUiItemTreeLeafCommon::Show(int bShow, int nSubWnd /* = -1 */)
{
	if (m_hWnd)
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
}

char * KUiItemTreeLeafCommon::EquirotalStrStr(const char *pString, const char *pStrSearch)
{
    const char *pszResult = NULL;
    KG_PROCESS_ERROR(pString);
    KG_PROCESS_ERROR(pStrSearch);

    char *pStringCopy = new char[strlen(pString) + 10];
    KG_PROCESS_ERROR(pStringCopy);
    char *pStrSearchCopy = new char[strlen(pStrSearch) + 10];
    KG_PROCESS_ERROR(pStrSearchCopy);
    strcpy(pStringCopy, pString);
    strcpy(pStrSearchCopy, pStrSearch);

    char *pChange = pStringCopy;
    while (*pChange != '\0') 
    {
        if(*pChange >= 'a' && *pChange <= 'z') 
            *pChange += 'A' - 'a';
		if (*pChange == '\\')
			*pChange = '/';
        pChange++;
    }

    pChange = pStrSearchCopy;
    while (*pChange != '\0') 
    {
        if(*pChange >= 'a' && *pChange <= 'z') 
            *pChange += 'A' - 'a';
		if (*pChange == '\\')
			*pChange = '/';
        pChange++;
    }
    char *pStrSearchResult = strstr(pStringCopy, pStrSearchCopy);
    KG_PROCESS_ERROR(pStrSearchResult);
    
    pszResult = pString + (pStrSearchResult - pStringCopy); 
Exit0:
    if (pStringCopy) 
    {
        delete[] pStringCopy;
        pStringCopy = NULL;
    }
    if (pStrSearchCopy) 
    {
        delete[] pStrSearchCopy;
        pStrSearchCopy = NULL;
    }
    return (char *)pszResult;
}

int KUiItemTreeLeafCommon::OnChangeImage()
{
	OPENFILENAME	ofn;
	char			szFile[MAX_PATH] = "", szRoot[MAX_PATH];
	szFile[0] = '\0';
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR |
		OFN_HIDEREADONLY | OFN_EXPLORER | OFN_EXTENSIONDIFFERENT;
	ofn.lpstrTitle = "请选择界面图文件";
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFile = szFile;
	ofn.lpstrFilter = _T("UITex文件\0*.UITex\0\0");
	if (ms_szLastImgPath[0])
		ofn.lpstrInitialDir = ms_szLastImgPath;
	if (!GetOpenFileName(&ofn))
		return false;

	char* pSplit = strrchr(szFile, '\\');
	*pSplit = 0;
	strcpy(ms_szLastImgPath, szFile);
	*pSplit = '\\';
	pSplit = strrchr(szFile, '.');
	g_GetRootPath(szRoot);

    if (EquirotalStrStr(szFile, szRoot) != szFile)
	{
		char szMsg[512];
		sprintf(szMsg, "图文件:%s必须在游戏根目录%s范围内!", szFile, szRoot);
		MessageBox(m_hWnd, szMsg, "提示", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	int nLen = strlen(szRoot) + 1;
	::SetDlgItemText(m_hWnd, IDC_ITEM_TREELEAF_IMAGE_NAME, szFile + nLen);
	g_Ui.GetIni(true)->WriteString(m_szName, "IconImage", szFile + nLen);

	return true;
}