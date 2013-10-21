////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemImageCommon.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-4 17:44:47
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include <Windows.h>
#include <tchar.h>

#include "UiItemImageCommon.h"
#include "UiEditor.h"
#include "resource.h"
#include "UiItemCommon.h"
#include "ClientManage.h"
#include "UiWndSelTgaFram.h"

////////////////////////////////////////////////////////////////////////////////

char KUiItemImageCommon::ms_szLastImgPath[MAX_PATH] = "";


KUiItemImageCommon::KUiItemImageCommon()
{
    m_szName[0] = '\0';
}

KUiItemImageCommon::~KUiItemImageCommon()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow *KUiItemImageCommon::Create()
{
	KUiItemImageCommon* pWnd = new KUiItemImageCommon;
	return pWnd;
}

int KUiItemImageCommon::Init(HWND hWnd, RECT* pRc, const char *pszName)
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
        MAKEINTRESOURCE(IDD_ItemImageCommon), 
        hWnd, (DLGPROC)WndProc
    );
    KG_PROCESS_ERROR(m_hWnd);

	::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top,
		pRc->right - pRc->left, pRc->bottom - pRc->top,
		SWP_SHOWWINDOW);
	::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

	hSpin = ::GetDlgItem(m_hWnd, IDC_ITEM_IMAGE_ALPHA_CHANGE);
	if (hSpin) 
		::SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(255, 0));

    nResult = true;
Exit0:
    if (!nResult)
        Release();
    return nResult;
}

void KUiItemImageCommon::Release()
{
    delete this;
}

LRESULT CALLBACK KUiItemImageCommon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KUiItemImageCommon*	pView = (KUiItemImageCommon*)GetWindowLong(hWnd, GWL_USERDATA);
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

void KUiItemImageCommon::OnButtonClick(int nBtnId)
{
    int nUpdateShower = true;
    int nRetCode = false;
    int nValue = 0;
    switch(nBtnId)
    {
    case IDC_ITEM_IMAGE_NAME_SEL:
        OnChangeImage();
        break;
    case IDC_ITEM_IMAGE_NAME_CLEAR:
        g_Ui.GetIni(true)->WriteString(m_szName, "Image", "");
        ::SetDlgItemText(m_hWnd, IDC_WND_IMAGE_COMMON_NAME, "");
        break;
    case IDC_ITEM_IMAGE_FRAME_SEL:
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
            g_Ui.GetIni()->GetString(m_szName, "Image", "", szUiTexFileName, sizeof(szUiTexFileName));
            if (!szUiTexFileName[0]) 
                break;
            strcat(szRoot, "\\");
            strcat(szRoot, szUiTexFileName);
            strcpy(szUiTexFileName, szRoot);
            nSelFram = SELTGAFRAME::OnSelTgaFram(szUiTexFileName, m_hWnd, &nFrameWidth, &nFrameHeight);           
            if (nSelFram >= 0)
            {
                g_Ui.GetIni()->WriteInteger(m_szName, "Frame", nSelFram);
                ::SetDlgItemInt(m_hWnd, IDC_ITEM_IMAGE_FRAME, nSelFram, true);
            }
        }
        break;
	case IDC_ITEM_IMAGE_NORMAL:
		g_Ui.GetIni()->WriteInteger(m_szName, "ImageType", 0);
		break;
	case IDC_ITEM_IMAGE_LEFT_RIGHT:
		g_Ui.GetIni()->WriteInteger(m_szName, "ImageType", 1);
		break;
	case IDC_ITEM_IMAGE_RIGHT_LEFT:
		g_Ui.GetIni()->WriteInteger(m_szName, "ImageType", 2);
		break;
	case IDC_ITEM_IMAGE_TOP_BOTTOM:
		g_Ui.GetIni()->WriteInteger(m_szName, "ImageType", 3);
		break;
	case IDC_ITEM_IMAGE_BOTTOM_TOP:
		g_Ui.GetIni()->WriteInteger(m_szName, "ImageType", 4);
		break;
	case IDC_ITEM_IMAGE_TIMER:
		g_Ui.GetIni()->WriteInteger(m_szName, "ImageType", 5);
		break;
	case IDC_ITEM_IMAGE_ROTATE:
		g_Ui.GetIni()->WriteInteger(m_szName, "ImageType", 6);
		break;
	case IDC_ITEM_IMAGE_TEX_UP_DOWN:
		g_Ui.GetIni()->WriteInteger(m_szName, "ImageType", 7);
		break;
	case IDC_ITEM_IMAGE_TEX_LEFT_RIGHT:
		g_Ui.GetIni()->WriteInteger(m_szName, "ImageType", 8);
		break;
	case IDC_ITEM_IMAGE_TEX_LEFT_RIGHT_TOP_BOTTOM:
		g_Ui.GetIni()->WriteInteger(m_szName, "ImageType", 9);
		break;
	case IDC_ITEM_IMAGE_TEX_NINE_PART:
		g_Ui.GetIni()->WriteInteger(m_szName, "ImageType", 10);
		break;
	case IDC_ITEM_IMAGE_TEX_LCR:
		g_Ui.GetIni()->WriteInteger(m_szName, "ImageType", 11);
		break;
	case IDC_ITEM_IMAGE_TEX_TCB:
		g_Ui.GetIni()->WriteInteger(m_szName, "ImageType", 12);
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

void KUiItemImageCommon::OnUpdateEditText(int nId)
{
    int nValue = 0;
    char szValue[MAX_PATH];
    szValue[0] = '\0';
    int nUpdateShower = true;
    
    switch(nId)
    {
    case IDC_ITEM_IMAGE_NAME:
        ::GetDlgItemText(m_hWnd, IDC_ITEM_IMAGE_NAME, szValue, sizeof(szValue));
        g_Ui.GetIni()->WriteString(m_szName, "Image", szValue);
        break;
    case IDC_ITEM_IMAGE_FRAME:
        nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_IMAGE_FRAME, NULL, true);
        g_Ui.GetIni()->WriteInteger(m_szName, "Frame", nValue);
        break;
	case IDC_ITEM_IMAGE_ALPHA:
		{
			nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_IMAGE_ALPHA, NULL, true);
			g_Ui.GetIni()->WriteInteger(m_szName, "Alpha", nValue);
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

void KUiItemImageCommon::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
    int nRetCode = false;
    int nValue = 0;
    char szImage[MAX_PATH];
    KG_PROCESS_ERROR(pszName);
    KG_PROCESS_ERROR(pIni);

    strncpy(m_szName, pszName, sizeof(m_szName));

    pIni->GetString(m_szName, "Image", "", szImage, sizeof(szImage));
    ::SetDlgItemText(m_hWnd, IDC_ITEM_IMAGE_NAME, szImage);
    pIni->GetInteger(m_szName, "Frame", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_ITEM_IMAGE_FRAME, nValue, TRUE);
	pIni->GetInteger(m_szName, "ImageType", 0, &nValue);
	switch(nValue)
	{
	case 1:
		::CheckDlgButton(m_hWnd, IDC_ITEM_IMAGE_LEFT_RIGHT, BST_CHECKED);
		break;
	case 2:
		::CheckDlgButton(m_hWnd, IDC_ITEM_IMAGE_RIGHT_LEFT, BST_CHECKED);
		break;
	case 3:
		::CheckDlgButton(m_hWnd, IDC_ITEM_IMAGE_TOP_BOTTOM, BST_CHECKED);
		break;
	case 4:
		::CheckDlgButton(m_hWnd, IDC_ITEM_IMAGE_BOTTOM_TOP, BST_CHECKED);
		break;
	case 5:
		::CheckDlgButton(m_hWnd, IDC_ITEM_IMAGE_TIMER, BST_CHECKED);
		break;
	case 6:
		::CheckDlgButton(m_hWnd, IDC_ITEM_IMAGE_ROTATE, BST_CHECKED);
		break;
	case 7:
		::CheckDlgButton(m_hWnd, IDC_ITEM_IMAGE_TEX_UP_DOWN, BST_CHECKED);
		break;
	case 8:
		::CheckDlgButton(m_hWnd, IDC_ITEM_IMAGE_TEX_LEFT_RIGHT, BST_CHECKED);
		break;
	case 9:
		::CheckDlgButton(m_hWnd, IDC_ITEM_IMAGE_TEX_LEFT_RIGHT_TOP_BOTTOM, BST_CHECKED);
		break;
	case 10:
		::CheckDlgButton(m_hWnd, IDC_ITEM_IMAGE_TEX_NINE_PART, BST_CHECKED);
		break;
	case 11:
		::CheckDlgButton(m_hWnd, IDC_ITEM_IMAGE_TEX_LCR, BST_CHECKED);
		break;
	case 12:
		::CheckDlgButton(m_hWnd, IDC_ITEM_IMAGE_TEX_TCB, BST_CHECKED);
		break;
	default:
		::CheckDlgButton(m_hWnd, IDC_ITEM_IMAGE_NORMAL, BST_CHECKED);
		break;
	}

	pIni->GetInteger(m_szName, "Alpha", 255, &nValue);
	::SetDlgItemInt(m_hWnd, IDC_ITEM_IMAGE_ALPHA, nValue, FALSE);

Exit0:
    return;
}

void KUiItemImageCommon::Show(int bShow, int nSubWnd /* = -1 */)
{
	if (m_hWnd)
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
}

char * KUiItemImageCommon::EquirotalStrStr(const char *pString, const char *pStrSearch)
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

int KUiItemImageCommon::OnChangeImage()
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
	::SetDlgItemText(m_hWnd, IDC_ITEM_IMAGE_NAME, szFile + nLen);
	g_Ui.GetIni(true)->WriteString(m_szName, "Image", szFile + nLen);

	return true;
}