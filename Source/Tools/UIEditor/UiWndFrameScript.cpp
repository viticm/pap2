#include "Stdafx.h"
#include "UiWndFrameScript.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"


KUiWndFrameScript::KUiWndFrameScript()
{
    m_szName[0]     = 0;
}

KUiWndFrameScript::~KUiWndFrameScript()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow* KUiWndFrameScript::Create()
{
	KUiWndFrameScript* pWnd = new KUiWndFrameScript;
	if (pWnd == NULL)
		return NULL;


	return pWnd;
}

int KUiWndFrameScript::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
	if (hWnd == NULL) 
    {
		return false;
	}
    strcpy(m_szName, pszName);
	m_hWnd = ::CreateDialog((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDD_WndFrameScript), hWnd, (DLGPROC)WndProc);
	if (m_hWnd)
	{
		::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top,
			pRc->right - pRc->left, pRc->bottom - pRc->top,
			SWP_SHOWWINDOW);
		::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)(KUiWndWindow*)this);

	}
	else
	{
		return false;
	}

	return true;
}

void KUiWndFrameScript::Release()
{
	delete this;
}

LRESULT CALLBACK KUiWndFrameScript::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KUiWndFrameScript*	pView = (KUiWndFrameScript*)GetWindowLong(hWnd, GWL_USERDATA);
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



void KUiWndFrameScript::OnButtonClick(int nBtnId)
{
	switch(nBtnId)
	{
    case IDC_WND_FRAME_COMMON_LUA_CLEAR:
        {
	        ::SetDlgItemText(m_hWnd, IDC_WND_FRAME_COMMON_LUA_NAME, "");
	        g_Ui.GetIni(true)->WriteString(m_szName, "ScriptFile", "");
	        g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
            ShowErrorInformation();
        }
        break;
    case IDC_WND_FRAME_COMMON_LUA_SEL:
        OnSelLua();
        break;
    case IDC_WND_FRAME_COMMON_LUA_EDIT:
        OnEditLua();
        break;
    case IDC_WND_FRAME_COMMON_AUTO_LUA:
        OnCreateLua();
        break;
	case IDC_FRAME_SCRIP_DISABLE_BREATHE:
		{
			int nValue = ::IsDlgButtonChecked(m_hWnd, IDC_FRAME_SCRIP_DISABLE_BREATHE) == BST_CHECKED;
			g_Ui.GetIni(true)->WriteInteger(m_szName, "DisableBreath", nValue);
			g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
			ShowErrorInformation();
		}
		break;
	case IDC_FRAME_SCRIP_FRAME_RENDER:
		{
			int nValue = ::IsDlgButtonChecked(m_hWnd, IDC_FRAME_SCRIP_FRAME_RENDER) == BST_CHECKED;
			g_Ui.GetIni(true)->WriteInteger(m_szName, "RenderEvent", nValue);
			g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
			ShowErrorInformation();
		}
		break;
    }
}

void KUiWndFrameScript::OnCreateLua()
{
    char szLuaFile[MAX_PATH];
    szLuaFile[0] = '\0';
    char szLuaFullPath[MAX_PATH];
    szLuaFullPath[0] = '\0';
    FILE *fpLua = NULL;
    char szName[128];
    szName[0] = '\0';
    char szpath[MAX_PATH];
    szpath[0] = '\0';
    char szType[128];
    szType[0] = '\0';

    ::GetDlgItemText(m_hWnd, IDC_WND_FRAME_COMMON_LUA_NAME, szLuaFile, sizeof(szLuaFile));
    if (!szLuaFile)
    {
        ::MessageBox(m_hWnd, "请选择Lua文件", "错误", MB_OK | MB_ICONINFORMATION);
        goto Exit0;
    }
    g_GetRootPath(szLuaFullPath);
    strcat(szLuaFullPath, "\\");
    strcat(szLuaFullPath, szLuaFile);

	if (MessageBox(m_hWnd, "将要覆盖原来所写的脚本, 是否继续?", "注意：", MB_YESNO | MB_ICONQUESTION) == IDNO)
		goto Exit0;

    fpLua = fopen(szLuaFullPath, "w");
    if (!fpLua)
    {
        ::MessageBox(m_hWnd, "不能打开Lua文件", "错误", MB_OK | MB_ICONINFORMATION);
        goto Exit0;
    }

    fprintf(fpLua, "%s\n", "--自动生成脚本");
    fprintf(fpLua, "%s\n", "--请将没用的代码和注释删去，以便提高脚本速度");
    fprintf(fpLua, "\n");

    fprintf(fpLua, "%s\n", "--以下是路径");
    szName[0] = '\0';
    while (g_Ui.GetNextUnit(szName))
    {
        g_Ui.GetIni()->GetString(szName, "._WndType", "", szType, sizeof(szType));
        int nType = g_GetWndType(szType);
        if (nType < UI_ITEM_NULL || nType > UI_ITEM_TREE_LEAFE)
            g_Ui.GetUnitTreePath(szpath, sizeof(szpath), szName);
        else
            g_Ui.GetItemTreePath(szpath, sizeof(szpath), szName);

        fprintf(fpLua, "%s_%s = \"%s\"\n", "szPath", szName, szpath);
    }
    fprintf(fpLua, "\n");

    szName[0] = '\0';
    g_Ui.GetFrameUnit(szName);
    fprintf(fpLua, "%s %s_%s\n", "function", szName, "OnFrameCreate()");
    fprintf(fpLua, "%s\n", "end");
    fprintf(fpLua, "\n");
    fprintf(fpLua, "%s %s_%s\n", "function", szName, "OnFrameBreathe()");
    fprintf(fpLua, "%s\n", "end");

    //todo add more here
    szName[0] = '\0';
    while (g_Ui.GetNextUnit(szName))
    {
        g_Ui.GetIni()->GetString(szName, "._WndType", "", szType, sizeof(szType));
        int nType = g_GetWndType(szType);
        if (nType == UI_WND_SCROLLBAR)
        {
            fprintf(fpLua, "%s %s_%s\n", "function", szName, "OnScorllBarPosChagned(nPos)");
            fprintf(fpLua, "%s\n", "end");
            continue;
        }

        if (nType == UI_WND_BUTTON)
        {
            fprintf(fpLua, "%s %s_%s\n", "function", szName, "OnLButtonClick()");
            fprintf(fpLua, "%s\n", "end");
            continue;
        }

        if (nType == UI_WND_CHECKBOX)
        {
            fprintf(fpLua, "%s %s_%s\n", "function", szName, "OnButtonCheck()");
            fprintf(fpLua, "%s\n", "end");
            fprintf(fpLua, "%s %s_%s\n", "function", szName, "OnButtonUncheck()");
            fprintf(fpLua, "%s\n", "end");
            continue;
        }

        if (nType == UI_WND_EDIT)
        {
            fprintf(fpLua, "%s %s_%s\n", "function", szName, "OnEditSpecialKeyDown(szkey)");
            fprintf(fpLua, "\t%s\n", "local nResult = 1");
            fprintf(fpLua, "\t%s\n", "if szkey == \"Enter\" then");
            fprintf(fpLua, "\t%s\n", "elseif szkey == \"Up\" then");
            fprintf(fpLua, "\t%s\n", "elseif szkey == \"Down\" then");
            fprintf(fpLua, "\t\t%s\n", "nResult = 0");
            fprintf(fpLua, "\t%s\n", "end");
            fprintf(fpLua, "\t%s\n", "return nResult");
            fprintf(fpLua, "%s\n", "end");
            continue;
        }

        char szEventName[128];
        g_Ui.GetIni()->GetString(szName, "EventName", "", szEventName, sizeof(szEventName));
        if (szEventName[0] == '\0')
			strcpy(szEventName, szName);
        DWORD dwKey = 0;
        int nValue = 0;
        g_Ui.GetIni()->GetInteger(szName, "EventID", 0, &nValue);
        dwKey = DWORD(nValue);

        if (dwKey & 0x00000001)
        {
            fprintf(fpLua, "%s %s_%s\n", "function", szEventName, "OnLButtonDown()");
            fprintf(fpLua, "%s\n", "end");
        }
        if (dwKey & 0x00000002)
        {
            fprintf(fpLua, "%s %s_%s\n", "function", szEventName, "OnRButtonDown()");
            fprintf(fpLua, "%s\n", "end");
        }
        if (dwKey & 0x00000004)
        {
            fprintf(fpLua, "%s %s_%s\n", "function", szEventName, "OnLButtonUp()");
            fprintf(fpLua, "%s\n", "end");
        }
        if (dwKey & 0x00000008)
        {
            fprintf(fpLua, "%s %s_%s\n", "function", szEventName, "OnRButtonUp()");
            fprintf(fpLua, "%s\n", "end");
        }
        if (dwKey & 0x00000010)
        {
            fprintf(fpLua, "%s %s_%s\n", "function", szEventName, "OnLButtonClick()");
            fprintf(fpLua, "%s\n", "end");
        }
        if (dwKey & 0x00000020)
        {
            fprintf(fpLua, "%s %s_%s\n", "function", szEventName, "OnRButtonClick()");
            fprintf(fpLua, "%s\n", "end");
        }
        if (dwKey & 0x00000040)
        {
            fprintf(fpLua, "%s %s_%s\n", "function", szEventName, "OnLButtonDBClick()");
            fprintf(fpLua, "%s\n", "end");
        }
        if (dwKey & 0x00000080)
        {
            fprintf(fpLua, "%s %s_%s\n", "function", szEventName, "OnRButtonDBClick()");
            fprintf(fpLua, "%s\n", "end");
        }
        if (dwKey & 0x00000100)
        {
            fprintf(fpLua, "%s %s_%s\n", "function", szEventName, "OnMouseEnter()");
            fprintf(fpLua, "%s\n", "end");
        }

        if (dwKey & 0x00000200)
        {
            fprintf(fpLua, "%s %s_%s\n", "function", szEventName, "OnMouseLeave()");
            fprintf(fpLua, "%s\n", "end");
        }
        if (dwKey & 0x00000400)
        {
            fprintf(fpLua, "%s %s_%s\n", "function", szEventName, "OnMouseMove()");
            fprintf(fpLua, "%s\n", "end");
        }
        if (dwKey & 0x00000800)
        {
            fprintf(fpLua, "%s %s_%s\n", "function", szEventName, "OnMouseWheel(nDistance)");
            fprintf(fpLua, "%s\n", "end");
        }
        if (dwKey & 0x00001000)
        {
            fprintf(fpLua, "%s %s_%s\n", "function", szEventName, "OnKeyDown(szKey, nShif, nCtrl, nAlt)");
            fprintf(fpLua, "%s\n", "end");
        }
        if (dwKey & 0x00002000)
        {
            fprintf(fpLua, "%s %s_%s\n", "function", szEventName, "OnKeyUp(szKey, nShif, nCtrl, nAlt)");
            fprintf(fpLua, "%s\n", "end");
        }

    }


Exit0:
    if (fpLua)
    {
        fclose(fpLua);
        fpLua = NULL;
    }
    return;
}

void KUiWndFrameScript::OnEditLua()
{
    char szPath[MAX_PATH] = "";
    char szFile[MAX_PATH] = "";
    char *pClip = NULL;

    g_GetRootPath(szPath);
    ::GetDlgItemText(m_hWnd, IDC_WND_FRAME_COMMON_LUA_NAME, szFile, sizeof(szFile));
    strcat(szPath, "\\");
    strcat(szPath, szFile);
    pClip = strrchr(szPath, '\\');
    if(pClip)
    {
        strcpy(szFile, pClip + 1);
        *pClip = '\0';
    }
	ShellExecute(NULL, "open", szFile, NULL, szPath, SW_SHOW);
}

int KUiWndFrameScript::OnSelLua()
{
    HANDLE hFile = NULL;
    WIN32_FIND_DATA FindFileData;

	OPENFILENAME	ofn;
	char			szFile[MAX_PATH] = "", szRoot[MAX_PATH];
	szFile[0] = '\0';
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.Flags = OFN_PATHMUSTEXIST | //OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR |
		OFN_HIDEREADONLY | OFN_EXPLORER | OFN_EXTENSIONDIFFERENT;
	ofn.lpstrTitle = "请选择脚本文件";
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFile = szFile;
	ofn.lpstrFilter = _T("Lua文件\0*.lua\0\0");
	if (KUiEditor::m_Settings.m_szLastUiIniPath[0])
        ofn.lpstrInitialDir = KUiEditor::m_Settings.m_szLastUiIniPath;
	if (!GetOpenFileName(&ofn))
		return false;
	char* pSplit = strrchr(szFile, '\\');
	*pSplit = 0;
	strcpy(KUiEditor::m_Settings.m_szLastUiIniPath, szFile);
	*pSplit = '\\';
	pSplit = strrchr(szFile, '.');
	g_GetRootPath(szRoot);

    if (EquirotalStrStr(szFile, szRoot) != szFile)
	{
		char szMsg[512];
		sprintf(szMsg, "LUA文件:%s必须在游戏根目录%s范围内!", szFile, szRoot);
		MessageBox(m_hWnd, szMsg, "提示", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	int nLen = strlen(szRoot) + 1;
	::SetDlgItemText(m_hWnd, IDC_WND_FRAME_COMMON_LUA_NAME, szFile + nLen);
	g_Ui.GetIni(true)->WriteString(m_szName, "ScriptFile", szFile + nLen);
	g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
    ShowErrorInformation();

    hFile = FindFirstFile(szFile, &FindFileData);
    if(hFile == INVALID_HANDLE_VALUE)
    {
        FILE *fp = fopen(szFile, "wb");
        if(!fp)
        {
            MessageBox(m_hWnd, "创建文件失败!", "提示", MB_OK | MB_ICONINFORMATION);
        }
        fclose(fp);
        fp = NULL;
    }
    FindClose(hFile);
    hFile = NULL;

	return true;
}

void KUiWndFrameScript::OnUpdateEditText(int nId)
{
	if (nId == IDC_WND_FRAME_COMMON_LUA_CLASS)
	{
		char szValue[128];
		::GetDlgItemText(m_hWnd, IDC_WND_FRAME_COMMON_LUA_CLASS, szValue, sizeof(szValue));
		if (szValue[0])
		{
			g_Ui.GetIni(true)->WriteString(m_szName, "ClassName", szValue);
			g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
			ShowErrorInformation();
		}
		else
		{
			g_Ui.GetIni(true)->EraseKey(m_szName, "ClassName");
			g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
			ShowErrorInformation();
		}
	}
}

char * KUiWndFrameScript::EquirotalStrStr(const char *pString, const char *pStrSearch)
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

void KUiWndFrameScript::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	if (!m_hWnd || !pIni || !pszName)
		return;
    strcpy(m_szName, pszName);
    char    szBuffer[128] = "";
	int nValue = 0;

    pIni->GetString(pszName, "ScriptFile", "", szBuffer, sizeof(szBuffer));
    ::SetDlgItemText(m_hWnd, IDC_WND_FRAME_COMMON_LUA_NAME, szBuffer);

	pIni->GetString(pszName, "ClassName", "", szBuffer, sizeof(szBuffer));
	::SetDlgItemText(m_hWnd, IDC_WND_FRAME_COMMON_LUA_CLASS, szBuffer);


	pIni->GetInteger(pszName, "DisableBreath", 0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_FRAME_SCRIP_DISABLE_BREATHE, nValue ? BST_CHECKED : 0);

	pIni->GetInteger(pszName, "RenderEvent", 0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_FRAME_SCRIP_FRAME_RENDER, nValue ? BST_CHECKED : 0);

}
void KUiWndFrameScript::Show(int bShow, int nSubWnd)
{
	if (m_hWnd)
	{
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
	} 
}

