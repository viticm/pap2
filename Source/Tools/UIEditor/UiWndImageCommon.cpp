#include "Stdafx.h"
#include <windows.h>
#include <tchar.h>
#include "UiWndImageCommon.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"
#include "UiWndSelTgaFram.h"

char	KUiWndImageCommon::ms_szLastImgPath[MAX_PATH] = "";

KUiWndImageCommon::KUiWndImageCommon(KUiWndWindow* pUiWndWindowCommon)
	: m_pUiWndWindowCommon(pUiWndWindowCommon)
{
    m_szName[0] = 0;
}

KUiWndImageCommon::~KUiWndImageCommon()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow* KUiWndImageCommon::Create(KUiWndWindow* pUiWndWindowCommon)
{
	KUiWndImageCommon* pWnd = new KUiWndImageCommon(pUiWndWindowCommon);
	if (pWnd == NULL)
		return NULL;
	return pWnd;
}

int KUiWndImageCommon::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
	if (hWnd == NULL) {
		return FALSE;
	}
    strcpy(m_szName, pszName);
	m_hWnd = ::CreateDialog((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDD_WndImagecommon), hWnd, (DLGPROC)WndProc);
	if (m_hWnd)
	{
		::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top,
			pRc->right - pRc->left, pRc->bottom - pRc->top,
			SWP_SHOWWINDOW);
		::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)(KUiWndWindow*)this);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

void KUiWndImageCommon::Release()
{
	delete this;
}

LRESULT CALLBACK KUiWndImageCommon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KUiWndImageCommon* pView = (KUiWndImageCommon*)GetWindowLong(hWnd, GWL_USERDATA);
	if (!pView)
		return 0;
	int bUpdateShower = false;
	int	nValue;
	if (message == WM_COMMAND)
	{
		switch(LOWORD(wParam))
		{
		case IDC_WND_IMAGE_COMMON_SEL_IMG:
			bUpdateShower = pView->OnChangeImage();
			break;
        case IDC_WND_IMAGE_COMMON_CLEAR:

            g_Ui.GetIni(true)->WriteString(pView->m_szName, "Image", "");
            ::SetDlgItemText(pView->m_hWnd, IDC_WND_IMAGE_COMMON_NAME, "");
            bUpdateShower = true;
            break;
		case IDC_WND_IMAGE_COMMON_TRANSPARENT:
			nValue = (::IsDlgButtonChecked(pView->m_hWnd, IDC_WND_IMAGE_COMMON_TRANSPARENT) == BST_CHECKED);
			g_Ui.GetIni()->WriteInteger(pView->m_szName, "Trans", nValue);
			bUpdateShower = true;
			break;
		case IDC_WND_IMAGE_COMMON_SIZE_TO_WND_SIZE:
			{
				int nFrame = 0;
				g_Ui.GetIni(false)->GetInteger(pView->m_szName, "Frame", 0, &nFrame);
				pView->OnSetImageSizeToWndSize(NULL, nFrame);
			}
            bUpdateShower = true;
			break;
        case IDC_WND_IMAGE_COMMON_ON_SEL_FRAM:
            {
                char szUiTexFileName[MAX_PATH];
                char szRoot[MAX_PATH];
                int nSelFram     = 0;
                int nFrameHeight = 0;
                int nFrameWidth  = 0;

                g_GetRootPath(szRoot);
                if (szRoot[0] == 0) 
                {
                    break;
                }
                szUiTexFileName[0] = 0;
                g_Ui.GetIni()->GetString(pView->m_szName, "Image", "", szUiTexFileName, sizeof(szUiTexFileName));
                if (szUiTexFileName[0] == 0) 
                {
                    break;
                }
                strcat(szRoot, "\\");
                strcat(szRoot, szUiTexFileName);
                strcpy(szUiTexFileName, szRoot);
                nSelFram = SELTGAFRAME::OnSelTgaFram(szUiTexFileName, pView->m_hWnd, &nFrameWidth, &nFrameHeight);           
                if (nSelFram != -1) 
                {
                    if (MessageBox(pView->m_hWnd, "要把窗口大小自动调整为与图形一样大么？", "提示", MB_YESNO | MB_ICONQUESTION) == IDYES)
	                {
                        g_Ui.GetIni(true)->WriteInteger(pView->m_szName, "Frame", nSelFram);
                        pView->OnSetImageSizeToWndSize(NULL, nSelFram);
                        ::SetDlgItemInt(pView->m_hWnd, IDC_WND_IMAGE_COMMON_FRAME, nSelFram, true);
	                }
                    else
                    {
                        g_Ui.GetIni(true)->WriteInteger(pView->m_szName, "Frame", nSelFram);
                        ::SetDlgItemInt(pView->m_hWnd, IDC_WND_IMAGE_COMMON_FRAME, nSelFram, true);
                    }
                }

            }
            bUpdateShower = true;
            break;

		case IDC_WND_IMAGE_COMMON_FRAME:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				nValue = ::GetDlgItemInt(hWnd, IDC_WND_IMAGE_COMMON_FRAME, NULL, true);
				g_Ui.GetIni(true)->WriteInteger(pView->m_szName, "Frame", nValue);
				//pView->UpdateWndInDawn();
                //pView->OnSetImageSizeToWndSize(NULL, nValue);
				
			}
            bUpdateShower = true;
			break;
		}
	}
	if (bUpdateShower)
	{
        g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
        pView->ShowErrorInformation();
	}
	return 0;
}

void KUiWndImageCommon::OnSetImageSizeToWndSize(const char* pszImage, int nFrame)
{
	char szImage[MAX_PATH] = "";
	if (pszImage == NULL)
	{
		::GetDlgItemText(m_hWnd, IDC_WND_IMAGE_COMMON_NAME, szImage, sizeof(szImage));
		pszImage = szImage;
	}

	SIZE size = { 0, 0 };
	KPluginUIImage sParam;
	sParam.szFilePath = pszImage;
	sParam.nFrame = nFrame;
	if (g_pfnUIBehavior(PCB_UI_GET_IMAGE_FRAME_SIZE, (WPARAM)&sParam, (LPARAM)&size))
	{
		::SetDlgItemInt(m_pUiWndWindowCommon->GetHWND(), IDC_WND_SIZE_WIDTH, size.cx, FALSE);
		::SetDlgItemInt(m_pUiWndWindowCommon->GetHWND(), IDC_WND_SIZE_HEIGHT, size.cy, FALSE);
	}
}

int KUiWndImageCommon::GetImageSize(LPCTSTR pszImageName, int nFrame, SIZE *pSize)
{
    int nResult = FALSE;
    KG_PROCESS_ERROR(pszImageName);
    KG_PROCESS_ERROR(pSize);

	KPluginUIImage sParam;
	sParam.szFilePath = pszImageName;
	sParam.nFrame = nFrame;

    g_pfnUIBehavior(PCB_UI_GET_IMAGE_FRAME_SIZE, (WPARAM)&sParam, (LPARAM)pSize);

    nResult = TRUE;
Exit0:
    return nResult;
}

char * KUiWndImageCommon::EquirotalStrStr(const char *pString, const char *pStrSearch)
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

int KUiWndImageCommon::OnChangeImage()
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
	::SetDlgItemText(m_hWnd, IDC_WND_IMAGE_COMMON_NAME, szFile + nLen);
	g_Ui.GetIni(true)->WriteString(m_szName, "Image", szFile + nLen);

	//if (MessageBox(m_hWnd, "要把窗口大小自动调整为与图形一样大么？", "提示", MB_YESNO | MB_ICONQUESTION) == IDYES)
	//{
	//	int nFrame = 0;
	//	g_Ui.GetIni(false)->GetInteger(m_szName, "Frame", 0, &nFrame);
	//	OnSetImageSizeToWndSize(szFile + nLen, nFrame);
	//}
	return true;
}

void KUiWndImageCommon::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	if (!m_hWnd || !pIni || !pszName)
		return;
	strcpy(m_szName, pszName);
	char	szImage[128];
	int		nValue;
	pIni->GetString(pszName, "Image", "", szImage, sizeof(szImage));
	::SetDlgItemText(m_hWnd, IDC_WND_IMAGE_COMMON_NAME, szImage);
	pIni->GetInteger(pszName, "Frame", 0, &nValue);
	::SetDlgItemInt(m_hWnd, IDC_WND_IMAGE_COMMON_FRAME, nValue, true);
	pIni->GetInteger(pszName, "Trans", 0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_IMAGE_COMMON_TRANSPARENT, nValue ? BST_CHECKED : 0);
}

void KUiWndImageCommon::Show(int bShow, int nSubWnd)
{
	if (m_hWnd)
	{
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
	}
}


