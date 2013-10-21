#include "Stdafx.h"
#include "UiPROJECTManage.h"
#include "resource.h"

namespace UIPROJECTMANAGE
{
    static KUiProjectManage ProjectManager;

    KUiProjectManage::KUiProjectManage()
    {   
        m_bManage                 = FALSE;
        m_hWnd                    = NULL;
        m_hProjectListWnd         = NULL;
        m_hFileListWnd            = NULL;
        m_nCurrentProject         = -1;
        m_nCurrentFile            = -1;
        m_szSelFileName[0]        = 0;
        m_szProjectSettingFile[0] = 0;
        m_pIni                    = NULL;

    }

	void KUiProjectManage::InitMgr()
	{
		if (m_pIni) 
		{
			m_pIni->Release();
			m_pIni = NULL;
		}

		char szPath[MAX_PATH]     = "";
		g_pfnUIBehavior(PCB_UI_GET_ROOT_PATH, (WPARAM)szPath, 0);
		strcat(szPath, "\\plugin\\UiEditor_Project.ini");
		strcpy(m_szProjectSettingFile, szPath);
		if (g_IsFileExist(m_szProjectSettingFile))
		{
			m_pIni = g_OpenIniFile(m_szProjectSettingFile);
		}
	}

    KUiProjectManage::~KUiProjectManage()
    {

        Save();

        if (m_pIni) 
        {
            m_pIni->Release();
            m_pIni = NULL;
        }
	    if (m_hWnd)
	    {
		    ::DestroyWindow(m_hWnd);
		    m_hWnd = NULL;
	    }
    }

    int KUiProjectManage::Init(HWND hWnd)
    {
        int nResult  = false;
        int nRetCode = false;

        KG_PROCESS_ERROR(hWnd);
        KG_PROCESS_ERROR(m_szProjectSettingFile[0]);
        m_hWnd = hWnd;

        RECT rcWindow;
        ::GetWindowRect(m_hWnd, &rcWindow);

        ::SetWindowPos(m_hWnd, HWND_TOP, 300, 200,
		    rcWindow.right -rcWindow.left, rcWindow.bottom - rcWindow.top,
		    SWP_SHOWWINDOW);

        //如果前面没有读入,再次读入工程配置文件
        if (m_pIni == NULL) 
        {
			if (g_IsFileExist(m_szProjectSettingFile))
			{
				m_pIni = g_OpenIniFile(m_szProjectSettingFile);
			}
        }
        KG_PROCESS_ERROR(m_pIni);

        //设置工程列表和文件列表
        FillProjectList();
        char szProjectName[128] = "";
        ListView_GetItemText(m_hProjectListWnd, m_nCurrentProject, 0, szProjectName, sizeof(szProjectName));
        FillFileList(szProjectName);


        nResult = true;
    Exit0:

	    return nResult;
    }

    void KUiProjectManage::FillProjectList()
    {
        if (m_hProjectListWnd) 
        {
            ListView_DeleteColumn(m_hProjectListWnd, 1);
            ListView_DeleteColumn(m_hProjectListWnd, 0);
            ListView_DeleteAllItems(m_hProjectListWnd);
        }

        KG_PROCESS_ERROR(m_hWnd);

	    m_hProjectListWnd = ::GetDlgItem(m_hWnd, IDC_WND_SOLUTION_PROJECT_NAME_LIST);
        KG_PROCESS_ERROR(m_hProjectListWnd);
	    //设置文件列表框的属性与分栏信息
	    ListView_SetExtendedListViewStyle(m_hProjectListWnd,
		    ListView_GetExtendedListViewStyle(m_hProjectListWnd) |	LVS_EX_FULLROWSELECT);

	    RECT	rc;
	    ::GetClientRect(m_hProjectListWnd, &rc);
	    LVCOLUMN	lvc;
	    char		szHeaderText[128];
	    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
	    lvc.cx = (rc.right - rc.left) / 2;
	    lvc.pszText = szHeaderText;
	    lvc.cchTextMax = 128;
        strcpy(szHeaderText, "工程名:");
	    ListView_InsertColumn(m_hProjectListWnd, 0, &lvc);

        lvc.cx = (rc.right - rc.left)  / 2;
	    lvc.mask |= LVCF_SUBITEM;
	    lvc.iSubItem = 1;
        strcpy(szHeaderText, "工程路径:");
	    ListView_InsertColumn(m_hProjectListWnd, 1, &lvc);

        KG_PROCESS_ERROR(m_pIni);

        char szSectionName[128] = "";
        char szNextSectionName[128] = "";
        char szParent[128] = "";
        char szPath[128] = "";
        while (m_pIni->GetNextSection(szSectionName, szNextSectionName)) 
        {
            m_pIni->GetString(szNextSectionName, "._Parent", "", szParent, sizeof(szParent));
            if (!strcmp(szParent, "TopParent")) 
            {
                LV_ITEM			lvi;
		        memset(&lvi, 0, sizeof(LV_ITEM));
		        lvi.mask = LVIF_TEXT;
		        lvi.pszText		= (char*)szNextSectionName;
		        lvi.cchTextMax  = 128;
		        lvi.iItem       = ListView_GetItemCount(m_hProjectListWnd);
		        lvi.iItem		= ListView_InsertItem(m_hProjectListWnd, &lvi);

                m_pIni->GetString(szNextSectionName, "Path", "", szPath, sizeof(szPath));
                ListView_SetItemText(m_hProjectListWnd, lvi.iItem, 1, szPath);
            }

            strcpy(szSectionName, szNextSectionName);
        }

    Exit0:
        return;

    }


    void KUiProjectManage::FillFileList(LPCTSTR pszProjectName)
    {
        if (m_hFileListWnd) 
        {
            ListView_DeleteColumn(m_hFileListWnd, 1);
            ListView_DeleteColumn(m_hFileListWnd, 0);
            ListView_DeleteAllItems(m_hFileListWnd);
        }

        KG_PROCESS_ERROR(m_hWnd);

	    m_hFileListWnd = ::GetDlgItem(m_hWnd, IDC_WND_SOLUTION_PROJECT_FILE_NAME_LIST);
        KG_PROCESS_ERROR(m_hFileListWnd);

	    //设置文件列表框的属性与分栏信息
	    ListView_SetExtendedListViewStyle(m_hFileListWnd,
		    ListView_GetExtendedListViewStyle(m_hFileListWnd) |	LVS_EX_FULLROWSELECT);
	    RECT	rc;
	    ::GetClientRect(m_hFileListWnd, &rc);
	    LVCOLUMN	lvc;
	    char		szHeaderText[128];
	    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
	    lvc.cx = (rc.right - rc.left) / 2;
	    lvc.pszText = szHeaderText;
	    lvc.cchTextMax = 128;
        strcpy(szHeaderText, "Ini文件:");
	    ListView_InsertColumn(m_hFileListWnd, 0, &lvc);

        lvc.cx = (rc.right - rc.left)  / 2;
	    lvc.mask |= LVCF_SUBITEM;
	    lvc.iSubItem = 1;
        strcpy(szHeaderText, "Lua文件:");
	    ListView_InsertColumn(m_hFileListWnd, 1, &lvc);

        KG_PROCESS_ERROR(m_pIni);
        KG_PROCESS_ERROR(pszProjectName && pszProjectName[0]);
        
        char szKey[128] = "";
        char szKeyValue[128] = "";
        int  nKeyValue = 0;
        int  i = 0;
        
        m_pIni->GetInteger(pszProjectName, "FileCount", 0, &nKeyValue);
        for (i = 0; i < nKeyValue; i++) 
        {
            sprintf(szKey, "File_%d_Ini", i);
            m_pIni->GetString(pszProjectName, szKey, "", szKeyValue, sizeof(szKeyValue));

            LV_ITEM			lvi;
		    memset(&lvi, 0, sizeof(LV_ITEM));
		    lvi.mask = LVIF_TEXT;
		    lvi.pszText		= (char*)szKeyValue;
		    lvi.cchTextMax  = 128;
		    lvi.iItem       = ListView_GetItemCount(m_hFileListWnd);
		    lvi.iItem		= ListView_InsertItem(m_hFileListWnd, &lvi);

            sprintf(szKey, "File_%d_Lua", i);
            m_pIni->GetString(pszProjectName, szKey, "", szKeyValue, sizeof(szKeyValue));
            ListView_SetItemText(m_hFileListWnd, lvi.iItem, 1, szKeyValue);

        }

    Exit0:
        return;

    }


    void KUiProjectManage::Release()
    {
        if (m_hFileListWnd) 
        {
            ListView_DeleteAllItems(m_hFileListWnd);
            m_hFileListWnd = NULL;
        }
        if (m_hProjectListWnd) 
        {
            ListView_DeleteAllItems(m_hProjectListWnd);
            m_hProjectListWnd = NULL;
        }
        
    }

    BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        int nResult = FALSE;

        switch(message) 
        {
        case WM_INITDIALOG:
            ProjectManager.Init(hWnd);
        	break;
        case WM_CLOSE:
            ProjectManager.Release();
            EndDialog(hWnd, FALSE);
            nResult = TRUE;
            break;

        case WM_NOTIFY:
            {
		        LPNMHDR pNotify = (LPNMHDR)lParam;
		        if (pNotify->hwndFrom == ProjectManager.GetProjectListHandle())
                {
                    ProjectManager.OnProjectListNotyfy(pNotify);
                }
                else if (pNotify->hwndFrom == ProjectManager.GetFileListHandle()) 
                {
                    ProjectManager.OnFileListNotify(pNotify);
                }
            }
            break;
        case WM_COMMAND:
	        {
		        int nNotify = HIWORD(wParam);
		        switch(nNotify)
		        {
		        case EN_CHANGE:
			        break;
		        case 0:
                    ProjectManager.OnButtonClick(LOWORD(wParam));
			        break;
		        }
	        }
            break;
        default:
            break;
        }

	    return nResult;
    }


    void KUiProjectManage::OnProjectListNotyfy(LPNMHDR pNotify)
    {
	    switch(pNotify->code)
	    {
	    case LVN_ITEMACTIVATE:
	    case NM_CLICK:
		    {
			    LPNMITEMACTIVATE pActive = (LPNMITEMACTIVATE)pNotify;
			    if (pActive->iItem >= 0)
			    {
                    m_nCurrentProject = pActive->iItem;

                    char szName[128] = "";
                    char szpath[128] = "";
                    ListView_GetItemText(m_hProjectListWnd, m_nCurrentProject, 0, szName, sizeof(szName));
                    ListView_GetItemText(m_hProjectListWnd, m_nCurrentProject, 1, szpath, sizeof(szpath));
                    ::SetDlgItemText(m_hWnd, IDC_WND_SOLUTION_PROJECT_NAME, szName);
                    ::SetDlgItemText(m_hWnd, IDC_WND_SOLUTION_PROJECT_PATH, szpath);

                    //设置显示的文件!
                    FillFileList(szName);
                    szName[0] = 0;
                    if (m_hFileListWnd) 
                    {
                        m_nCurrentFile = m_nCurrentFile < ListView_GetItemCount(m_hFileListWnd) ? m_nCurrentFile : ListView_GetItemCount(m_hFileListWnd) - 1;
                        ListView_GetItemText(m_hFileListWnd, m_nCurrentFile, 0, szName, sizeof(szName));
                        ListView_EnsureVisible(m_hFileListWnd, m_nCurrentFile, FALSE);
                        //ListView_SetSelectionMark(m_hFileListWnd, m_nCurrentFile)
                        //ListView_SetItemState(m_hFileListWnd, m_nCurrentFile, LVIS_FOCUSED, LVIS_SELECTED);
                    }
                    ::SetDlgItemText(m_hWnd, IDC_WND_SOLUTION_PROJECT_FILE_NAME, szName);
			    }
		    }
		    break;

	    }
    }

    void KUiProjectManage::OnFileListNotify(LPNMHDR pNotify)
    {
	    switch(pNotify->code)
	    {
	    case LVN_ITEMACTIVATE:
	    case NM_CLICK:
		    {
			    LPNMITEMACTIVATE pActive = (LPNMITEMACTIVATE)pNotify;
			    if (pActive->iItem >= 0)
			    {
                    m_nCurrentFile = pActive->iItem;

                    char szName[128] = "";
                    ListView_GetItemText(m_hFileListWnd, m_nCurrentFile, 0, szName, sizeof(szName));
                    ::SetDlgItemText(m_hWnd, IDC_WND_SOLUTION_PROJECT_FILE_NAME, szName);
			    }
		    }
		    break;
        case NM_DBLCLK:
            {
			    LPNMITEMACTIVATE pActive = (LPNMITEMACTIVATE)pNotify;
			    if (pActive->iItem >= 0)
			    {
                    m_nCurrentFile = pActive->iItem;

                    char szName[128] = "";
                    ListView_GetItemText(m_hFileListWnd, m_nCurrentFile, 0, szName, sizeof(szName));
                    ::SetDlgItemText(m_hWnd, IDC_WND_SOLUTION_PROJECT_FILE_NAME, szName);

                    //OnSelFile();
			    }
            }
            break;

	    }
    }


    void KUiProjectManage::OnButtonClick(int nBtnId)
    {

	    switch(nBtnId)
	    {
            case IDC_WND_SOLUTION_PROJECT_PATH_SEL: // 选择工程路径
                OnSelProjectPath();
                break;
            case IDC_WND_SOLUTION_PROJECT_NEW: //新建工程
                OnNewProject();
                break;
            case IDC_WND_SOLUTION_PROJECT_DEL: //删除工程
                OnDelProject();
                break;
            case IDC_WND_SOLUTION_PROJECT_FILE_NAME_DEL: //删除文件
                OnDelFile();
                break;
            case IDC_WND_SOLUTION_PROJECT_FILE_NAME_ADD: //添加文件
                OnAddFile();
                break;
            case IDC_WND_SOLUTION_PROJECT_FILE_CREATE_LUA: //新建文件时要不要创建lua
                break;
            case IDC_WND_SOLUTION_PROJECT_FILE_NEW: //新建文件
                OnNewFile();
                break;
            case IDC_WND_SOLUTION_PROJECT_FILE_EDIT: //打开文件
                OnEditFile();
                break;

            default:
                break;
	    }

    }

    void KUiProjectManage::OnSelProjectPath()
    {
        KG_PROCESS_ERROR(m_hWnd);

	    BROWSEINFO	bi;
	    ITEMIDLIST	*pItemList = NULL;
	    char		szPath[MAX_PATH] = "";

	    memset(&bi, 0, sizeof(BROWSEINFO));
	    bi.hwndOwner		= m_hWnd;
	    bi.lpszTitle		= "指定工程所在目录：";
	    bi.ulFlags			= BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS
								    |BIF_VALIDATE | BIF_RETURNFSANCESTORS	;
	    if ((pItemList = SHBrowseForFolder(&bi)) == NULL)
		    return;
	    if( pItemList)
	    {
		    SHGetPathFromIDList(pItemList, szPath);
		    ::SetDlgItemText(m_hWnd, IDC_WND_SOLUTION_PROJECT_PATH, szPath);
	    }

    Exit0:
        return;

    }

    void KUiProjectManage::OnNewProject()
    {
        KG_PROCESS_ERROR(m_hProjectListWnd);
        KG_PROCESS_ERROR(m_pIni);

        char szProjectName[128] = "";
        char szProjectPath[128] = "";
        ::GetDlgItemText(m_hWnd, IDC_WND_SOLUTION_PROJECT_NAME, szProjectName, sizeof(szProjectName));
        ::GetDlgItemText(m_hWnd, IDC_WND_SOLUTION_PROJECT_PATH, szProjectPath, sizeof(szProjectPath));

        KG_PROCESS_ERROR(szProjectName[0]);
        KG_PROCESS_ERROR(szProjectPath[0]);

        if(m_pIni->IsSectionExist(szProjectName))
        {
            ::MessageBox(m_hWnd, "该工程已存在！", "错误：", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            m_pIni->WriteString(szProjectName, "._Parent", "TopParent");    
            m_pIni->WriteString(szProjectName, "Path", szProjectPath);
            m_pIni->WriteInteger(szProjectName, "FileCount", 0);
        }

        FillProjectList();

        m_nCurrentProject = ListView_GetItemCount(m_hProjectListWnd) -1;
        ListView_GetItemText(m_hProjectListWnd, m_nCurrentProject, 0, szProjectName, sizeof(szProjectName));
        ListView_GetItemText(m_hProjectListWnd, m_nCurrentProject, 1, szProjectPath, sizeof(szProjectPath));
        ::SetDlgItemText(m_hWnd, IDC_WND_SOLUTION_PROJECT_NAME, szProjectName);
        ::SetDlgItemText(m_hWnd, IDC_WND_SOLUTION_PROJECT_PATH, szProjectPath);

        FillFileList(szProjectName);

    Exit0:
        return;
    }

    void KUiProjectManage::OnDelProject()
    {
        KG_PROCESS_ERROR(m_hProjectListWnd);
        KG_PROCESS_ERROR(m_pIni);
        KG_PROCESS_ERROR(m_nCurrentProject >= 0);
        KG_PROCESS_ERROR(m_nCurrentProject < ListView_GetItemCount(m_hProjectListWnd));

        char szProjectName[128] = "";
        char szProjectPath[128] = "";
        ListView_GetItemText(m_hProjectListWnd, m_nCurrentProject, 0, szProjectName, sizeof(szProjectName));
        m_pIni->EraseSection(szProjectName);

        m_nCurrentProject = m_nCurrentProject < ListView_GetItemCount(m_hProjectListWnd) ? m_nCurrentProject : ListView_GetItemCount(m_hProjectListWnd) - 1;

        if (m_nCurrentProject >= 0) 
        {
            ListView_GetItemText(m_hProjectListWnd, m_nCurrentProject, 0, szProjectName, sizeof(szProjectName));
            ListView_GetItemText(m_hProjectListWnd, m_nCurrentProject, 1, szProjectPath, sizeof(szProjectPath));

        }
        else
        {
            szProjectName[0] = 0;
            szProjectPath[0] = 0;
        }

        ::SetDlgItemText(m_hWnd, IDC_WND_SOLUTION_PROJECT_NAME, szProjectName);
        ::SetDlgItemText(m_hWnd, IDC_WND_SOLUTION_PROJECT_PATH, szProjectPath);

    Exit0:
        return;
    }

    void KUiProjectManage::OnDelFile()
    {
        KG_PROCESS_ERROR(m_hProjectListWnd);
        KG_PROCESS_ERROR(m_hFileListWnd);
        KG_PROCESS_ERROR(m_pIni);
        KG_PROCESS_ERROR(m_nCurrentFile >= 0);
        KG_PROCESS_ERROR(m_nCurrentFile < ListView_GetItemCount(m_hFileListWnd));
        KG_PROCESS_ERROR(m_hWnd);

        ListView_DeleteItem(m_hFileListWnd, m_nCurrentFile);
        m_nCurrentFile = m_nCurrentFile < ListView_GetItemCount(m_hFileListWnd) ? m_nCurrentFile : ListView_GetItemCount(m_hFileListWnd) - 1;
    
        SetIniByCurrentFileList();

        char szName[128] = "";
        ListView_GetItemText(m_hFileListWnd, m_nCurrentFile, 0, szName, sizeof(szName));    
        ::SetDlgItemText(m_hWnd, IDC_WND_SOLUTION_PROJECT_FILE_NAME, szName);

    Exit0:
        return;

    }

    void KUiProjectManage::SetIniByCurrentFileList()
    {
        KG_PROCESS_ERROR(m_pIni);
        KG_PROCESS_ERROR(m_hFileListWnd);
        KG_PROCESS_ERROR(m_nCurrentProject >= 0);
        KG_PROCESS_ERROR(m_nCurrentProject < ListView_GetItemCount(m_hProjectListWnd));

        char szProjectName[128] = "";
        char szFileName[128] = "";
        char szKeyName[32] = "";
        int i = 0;
        int nMaxFileCount = 0;
        char szNextKeyName[128] = "";

        ListView_GetItemText(m_hProjectListWnd, m_nCurrentProject, 0, szProjectName, sizeof(szProjectName));
        KG_PROCESS_ERROR(szProjectName);

        //清除Key值
        m_pIni->GetInteger(szProjectName, "FileCount", 0, &nMaxFileCount);
        for(i = 0; i < nMaxFileCount; i++)
        {
            sprintf(szKeyName, "File_%d_Ini", i);
            m_pIni->EraseKey(szProjectName, szKeyName);

            sprintf(szKeyName, "File_%d_Lua", i);
            m_pIni->EraseKey(szProjectName, szKeyName);
        }

        nMaxFileCount = ListView_GetItemCount(m_hFileListWnd);
        m_pIni->WriteInteger(szProjectName, "FileCount", nMaxFileCount);
        for (i = 0; i < nMaxFileCount; i++) 
        {
            sprintf(szKeyName, "File_%d_Ini", i);
            ListView_GetItemText(m_hFileListWnd, i, 0, szFileName, sizeof(szFileName));
            m_pIni->WriteString(szProjectName, szKeyName, szFileName);

            sprintf(szKeyName, "File_%d_Lua", i);
            ListView_GetItemText(m_hFileListWnd, i, 1, szFileName, sizeof(szFileName));
            m_pIni->WriteString(szProjectName, szKeyName, szFileName);
        }
        


    Exit0:
        return;
    }

    void KUiProjectManage::OnAddFile()
    {
        KG_PROCESS_ERROR(m_hFileListWnd);
        KG_PROCESS_ERROR(m_hProjectListWnd);
        KG_PROCESS_ERROR(m_nCurrentProject >= 0);
        KG_PROCESS_ERROR(m_nCurrentProject < ListView_GetItemCount(m_hProjectListWnd));
        
        //获得工程的完整路径
        char szFullPath[MAX_PATH] = "";
        char szPath[128] = "";
        g_GetRootPath(szFullPath);
        strcat(szFullPath, "\\");
        ListView_GetItemText(m_hProjectListWnd, m_nCurrentProject, 1, szPath, sizeof(szPath));
        strcat(szFullPath, szPath);
        if (szFullPath[strlen(szFullPath) - 1] == '\\') 
        {
            szFullPath[strlen(szFullPath) - 1] = '\0';
        }

        //选择添加的文件名
	    OPENFILENAME	ofn;
	    char			szFile[1024] = "";
	    szFile[0] = '\0';
	    memset(&ofn, 0, sizeof(OPENFILENAME));
	    ofn.lStructSize = sizeof(OPENFILENAME);
	    ofn.hwndOwner = m_hWnd;
	    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_NOCHANGEDIR |
		    OFN_HIDEREADONLY | OFN_EXPLORER | OFN_EXTENSIONDIFFERENT;
	    ofn.lpstrTitle = "请选择ini文件或者Lua文件";
	    ofn.nMaxFile = sizeof(szFile);
	    ofn.lpstrFile = szFile;
	    ofn.lpstrFilter = "界面配置ini文件或者Lua文件\0*.ini;*.lua\0\0";
		ofn.lpstrInitialDir = szFullPath; //初始化路径名
	    KG_PROCESS_ERROR(GetOpenFileName(&ofn));

        //找到所有的文件名
        if (EquirotalStrStr(szFile, szFullPath) != szFile) 
        {
            ::MessageBox(m_hWnd, "工程文件必须在工程目录下！", "错误：", MB_OK | MB_ICONINFORMATION);
            KG_PROCESS_ERROR(0);
        }

        char szName[128] = "";
        char *pClip = NULL;
        if (szFile[strlen(szFile) + 1] == '\0') 
        {
            //如果只有一个文件
            strcpy(szName, szFile + strlen(szFullPath) + 1);
            AddAFileToProject(szName);

        }
        else
        {
            //多个文件的处理情况
            
            pClip = szFile + strlen(szFile) + 1;

            while (*pClip) 
            {
                strcpy(szName, szFile + strlen(szFullPath) + 1);
                strcpy(szName, "\\");
                strcat(szName, pClip);
                AddAFileToProject(szName);
                pClip = pClip + strlen(pClip) + 1;
            }

        }
        


    Exit0:
        return;

    }

    void KUiProjectManage::AddAFileToProject(LPCTSTR pszFileName)
    {
        KG_PROCESS_ERROR(pszFileName);
        KG_PROCESS_ERROR(m_hFileListWnd);

        PROJECT_FILE_TYPE eFileType = GetProjectFileType(pszFileName);

        if (eFileType == INI_FILE) 
        {
            if (!IsFileExistInProject(pszFileName, INI_FILE)) 
            {
                char szIniFileName[128] = "";
                char szLuaFileName[128] = "";
                int nIndex = 0;
                GetAdjustLuaFromIni(pszFileName, szLuaFileName);
                strcpy(szIniFileName, pszFileName);
                if (IsFileExistInProject(szLuaFileName, LUA_FILE, &nIndex)) 
                {
                    ListView_SetItemText(m_hFileListWnd, nIndex, 0, szIniFileName);
                }
                else
                {
                    LV_ITEM			lvi;
		            memset(&lvi, 0, sizeof(LV_ITEM));
		            lvi.mask = LVIF_TEXT;
		            lvi.pszText		= (char*)szIniFileName;
		            lvi.cchTextMax  = 128;
		            lvi.iItem       = ListView_GetItemCount(m_hFileListWnd);
		            lvi.iItem		= ListView_InsertItem(m_hFileListWnd, &lvi);

                }
                
                SetIniByCurrentFileList();
            }
        }
        else if (eFileType == LUA_FILE) 
        {
            if (!IsFileExistInProject(pszFileName, LUA_FILE)) 
            {
                char szIniFileName[128] = "";
                char szLuaFileName[128] = "";
                int nIndex = 0;
                GetAdjustIniFromLua(pszFileName, szIniFileName);
                strcpy(szLuaFileName, pszFileName);
                if (IsFileExistInProject(szIniFileName, INI_FILE, &nIndex)) 
                {
                    ListView_SetItemText(m_hFileListWnd, nIndex, 1, szLuaFileName);
                }
                else
                {
                    szIniFileName[0] = 0;

                    LV_ITEM			lvi;
		            memset(&lvi, 0, sizeof(LV_ITEM));
		            lvi.mask = LVIF_TEXT;
		            lvi.pszText		= (char*)szIniFileName;
		            lvi.cchTextMax  = 128;
		            lvi.iItem       = ListView_GetItemCount(m_hFileListWnd);
		            lvi.iItem		= ListView_InsertItem(m_hFileListWnd, &lvi);

                    ListView_SetItemText(m_hFileListWnd, lvi.iItem, 1, szLuaFileName);
                }

                SetIniByCurrentFileList();
            }
        }


    Exit0:
        return;
    }

    int KUiProjectManage::GetAdjustIniFromLua(IN LPCTSTR pszLua, OUT LPTSTR pszIni)
    {
        int nResult = FALSE; 
        KG_PROCESS_ERROR(pszLua);
        KG_PROCESS_ERROR(pszIni);
        
        char *pClip = NULL;
        pClip = EquirotalStrStr(pszLua + strlen(pszLua) - 4, ".lua");
        KG_PROCESS_ERROR(pClip);
        
        strcpy(pszIni, pszLua);
        pClip = pszIni + strlen(pszIni) - 4;
        strcpy(pClip, ".ini");

        nResult = TRUE;
    Exit0:
        return nResult;

    }

    int KUiProjectManage::GetAdjustLuaFromIni(IN LPCTSTR pszIni, OUT LPTSTR pszLua)
    {
            int nResult = FALSE; 
            KG_PROCESS_ERROR(pszIni);
            KG_PROCESS_ERROR(pszLua);
            
            char *pClip = NULL;
            pClip = EquirotalStrStr(pszIni + strlen(pszIni) - 4, ".ini");
            KG_PROCESS_ERROR(pClip);
            
            strcpy(pszLua, pszIni);
            pClip = pszLua + strlen(pszLua) - 4;
            strcpy(pClip, ".lua");

            nResult = TRUE;
        Exit0:
            return nResult;
    }

    KUiProjectManage::PROJECT_FILE_TYPE KUiProjectManage::GetProjectFileType(LPCTSTR pszName)
    {
        PROJECT_FILE_TYPE eResult = UNKNOWN_FILE;
        KG_PROCESS_ERROR(pszName && pszName[0]);
        KG_PROCESS_ERROR(strlen(pszName) >= 4);

        // Todo:判断是不是ini文件或者Lua文件的算法可能要改以防止出现 *.ini.lua的情况。
        if (EquirotalStrStr(pszName + strlen(pszName) - 4, ".ini")) 
        {
            eResult = INI_FILE;
        }
        else if(EquirotalStrStr(pszName + strlen(pszName) - 4, ".lua"))
        {
            eResult = LUA_FILE;
        }

    Exit0:
        return eResult;
    }

    int KUiProjectManage::IsFileExistInProject(LPCTSTR pszFileName, PROJECT_FILE_TYPE eType, OUT int *pnWhere)
    {
        int nReSult = TRUE;
        KG_PROCESS_ERROR(m_hFileListWnd);
        KG_PROCESS_ERROR(pszFileName);
        KG_PROCESS_ERROR(eType != UNKNOWN_FILE);
        int i = 0;
        char szExistFileName[128] = "";
        int nMaxFile = ListView_GetItemCount(m_hFileListWnd);
        for (i = 0; i < nMaxFile; i++) 
        {
            ListView_GetItemText(m_hFileListWnd, i, eType == INI_FILE ? 0 : 1, szExistFileName, sizeof(szExistFileName));
            if (!stricmp(szExistFileName, pszFileName)) 
            {
                if(pnWhere)
                    *pnWhere = i;
                KG_PROCESS_ERROR(0);
            }
        }

        nReSult = FALSE;
    Exit0:
        return nReSult;
    }


    char * KUiProjectManage::EquirotalStrStr(const char *pString, const char *pStrSearch)
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

    void KUiProjectManage::OnEditFile()
    {
        KG_PROCESS_ERROR(m_hFileListWnd);
        KG_PROCESS_ERROR(m_nCurrentFile >= 0);
        KG_PROCESS_ERROR(m_nCurrentFile < ListView_GetItemCount(m_hFileListWnd));
        KG_PROCESS_ERROR(m_hProjectListWnd);
        KG_PROCESS_ERROR(m_nCurrentProject >= 0);
        KG_PROCESS_ERROR(m_nCurrentProject < ListView_GetItemCount(m_hProjectListWnd));
        KG_PROCESS_ERROR(m_pIni);

        //取得路径并检验其正确性
        ListView_GetItemText(m_hProjectListWnd, m_nCurrentProject, 1, m_szSelFileName, sizeof(m_szSelFileName));
        if (m_szSelFileName[strlen(m_szSelFileName) - 1] != '\\' ) 
        {
            strcat(m_szSelFileName, "\\");
        }

        char szName[128] = "";
        ListView_GetItemText(m_hFileListWnd, m_nCurrentFile, 0, szName, sizeof(szName));
        KG_PROCESS_ERROR(szName);

        strcat(m_szSelFileName, szName);

        m_bSeled = TRUE;
        PostMessage(m_hWnd, WM_CLOSE, 0, 0);

    Exit0:
        return;
    }

    void KUiProjectManage::OnNewFile()
    {
    }

    void KUiProjectManage::Save()
    {
        KG_PROCESS_ERROR(m_pIni);
        KG_PROCESS_ERROR(m_szProjectSettingFile);

        m_pIni->Save(m_szProjectSettingFile);

    Exit0:
        return;
    }

    void KUiProjectManage::OnProjectManage(HWND hParent, BOOL bManage /* = FALSE */)
    {
        m_bManage = bManage;
        m_bSeled = FALSE;
        m_szSelFileName[0] = 0;
        m_nCurrentProject = -1;
        m_nCurrentFile = -1;
        KG_PROCESS_ERROR(hParent);
        
        ::DialogBox((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDD_WndSolutionManage), hParent, (DLGPROC)DlgProc);

    Exit0:
        return;

    }

    int ProjectManage(HWND hParent, OUT char *pszOpenIniFileName)
    {
        int nResult = FALSE;
        KG_PROCESS_ERROR(hParent);
        KG_PROCESS_ERROR(pszOpenIniFileName);
        
        ProjectManager.OnProjectManage(hParent, TRUE);

        if (ProjectManager.IsSel()) 
        {
            strcpy(pszOpenIniFileName, ProjectManager.GetSelFileName());
        }

        nResult = TRUE;
    Exit0:
        return nResult;
    }

	int InitProjectManage()
	{
		ProjectManager.InitMgr();
		return true;
	}

}