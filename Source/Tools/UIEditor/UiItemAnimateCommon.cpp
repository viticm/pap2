////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemAnimateCommon.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-5 13:22:45
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include <Windows.h>
#include <tchar.h>

#include "UiItemAnimateCommon.h"
#include "UiItemImageCommon.h"
#include "UiEditor.h"
#include "resource.h"
#include "UiItemCommon.h"
#include "ClientManage.h"
#include "UiWndSelAnimate.h"
////////////////////////////////////////////////////////////////////////////////

KUiItemAnimateCommon::KUiItemAnimateCommon()
{
    m_szName[0] = '\0';
}

KUiItemAnimateCommon::~KUiItemAnimateCommon()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow *KUiItemAnimateCommon::Create()
{
	KUiItemAnimateCommon* pWnd = new KUiItemAnimateCommon;
	return pWnd;
}

int KUiItemAnimateCommon::Init(HWND hWnd, RECT* pRc, const char *pszName)
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
        MAKEINTRESOURCE(IDD_ItemAnimateCommon), 
        hWnd, (DLGPROC)WndProc
    );
    KG_PROCESS_ERROR(m_hWnd);

	::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top,
		pRc->right - pRc->left, pRc->bottom - pRc->top,
		SWP_SHOWWINDOW);
	::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

    hSpin = ::GetDlgItem(m_hWnd, IDC_ITEM_ANIMATE_LOOPCOUNT_CHANGE);
    if (hSpin) 
        ::SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, -1));

    nResult = true;
Exit0:
    if (!nResult)
        Release();
    return nResult;
}

void KUiItemAnimateCommon::Release()
{
    delete this;
}

LRESULT CALLBACK KUiItemAnimateCommon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KUiItemAnimateCommon*	pView = (KUiItemAnimateCommon*)GetWindowLong(hWnd, GWL_USERDATA);
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

void KUiItemAnimateCommon::OnButtonClick(int nBtnId)
{
    int nUpdateShower = true;
    int nRetCode = false;
    int nValue = 0;
    switch(nBtnId)
    {
    case IDC_ITEM_ANIMATE_IMAGE_NAME_SEL:
        OnChangeImage();
        break;
    case IDC_ITEM_ANIMATE_IMAGE_NAME_CLEAR:
        g_Ui.GetIni(true)->WriteString(m_szName, "Image", "");
        ::SetDlgItemText(m_hWnd, IDC_ITEM_ANIMATE_IMAGE_NAME, "");
        break;
    case IDC_ITEM_ANIMATE_GROUP_SEL:
        {
            char szUiTexFileName[MAX_PATH];
            char szRoot[MAX_PATH];
            int nSelGroup    = -1;

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
            nSelGroup = SELANIMATE::OnSelAnimate(szUiTexFileName, m_hWnd);
            if (nSelGroup >= 0)
            {
                g_Ui.GetIni()->WriteInteger(m_szName, "Group", nSelGroup);
                ::SetDlgItemInt(m_hWnd, IDC_ITEM_ANIMATE_GROUP, nSelGroup, true);
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

void KUiItemAnimateCommon::OnUpdateEditText(int nId)
{
    int nValue = 0;
    char szValue[MAX_PATH];
    szValue[0] = '\0';
    int nUpdateShower = true;
    
    switch(nId)
    {
    case IDC_ITEM_ANIMATE_IMAGE_NAME:
        ::GetDlgItemText(m_hWnd, IDC_ITEM_ANIMATE_IMAGE_NAME, szValue, sizeof(szValue));
        g_Ui.GetIni()->WriteString(m_szName, "Image", szValue);
        break;
    case IDC_ITEM_ANIMATE_GROUP:
        nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_ANIMATE_GROUP, NULL, true);
        g_Ui.GetIni()->WriteInteger(m_szName, "Group", nValue);
        break;
    case IDC_ITEM_ANIMATE_LOOPCOUNT:
        nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_ANIMATE_LOOPCOUNT, NULL, true);
        g_Ui.GetIni()->WriteInteger(m_szName, "LoopCount", nValue);
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

void KUiItemAnimateCommon::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
    int nRetCode = false;
    int nValue = 0;
    char szImage[MAX_PATH];
    KG_PROCESS_ERROR(pszName);
    KG_PROCESS_ERROR(pIni);

    strncpy(m_szName, pszName, sizeof(m_szName));

    pIni->GetString(m_szName, "Image", "", szImage, sizeof(szImage));
    ::SetDlgItemText(m_hWnd, IDC_ITEM_ANIMATE_IMAGE_NAME, szImage);
    pIni->GetInteger(m_szName, "Group", -1, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_ITEM_ANIMATE_GROUP, nValue, TRUE);
    pIni->GetInteger(m_szName, "LoopCount", -1, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_ITEM_ANIMATE_LOOPCOUNT, nValue, TRUE);

Exit0:
    return;
}

void KUiItemAnimateCommon::Show(int bShow, int nSubWnd /* = -1 */)
{
	if (m_hWnd)
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
}

char * KUiItemAnimateCommon::EquirotalStrStr(const char *pString, const char *pStrSearch)
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

int KUiItemAnimateCommon::OnChangeImage()
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
    if (KUiItemImageCommon::ms_szLastImgPath[0])
		ofn.lpstrInitialDir = KUiItemImageCommon::ms_szLastImgPath;
	if (!GetOpenFileName(&ofn))
		return false;

	char* pSplit = strrchr(szFile, '\\');
	*pSplit = 0;
	strcpy(KUiItemImageCommon::ms_szLastImgPath, szFile);
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
	::SetDlgItemText(m_hWnd, IDC_ITEM_ANIMATE_IMAGE_NAME, szFile + nLen);
	g_Ui.GetIni(true)->WriteString(m_szName, "Image", szFile + nLen);

	return true;
}

