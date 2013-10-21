// KGNormalMapCreatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGNormalMapCreatorDlg.h"
#include "IEEditor.h"

// KGNormalMapCreatorDlg dialog

IMPLEMENT_DYNAMIC(KGNormalMapCreatorDlg, CDialog)

KGNormalMapCreatorDlg::KGNormalMapCreatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(KGNormalMapCreatorDlg::IDD, pParent),
      m_bProcess(FALSE)
{
    m_Params.np[0] = 100;
    m_Params.np[1] = 200;

    m_Params.sp[0] = 100;
    m_Params.sp[1] = -50;
    m_Params.sp[2] = 52;

    m_Params.dp[0] = 60;
    m_Params.dp[1] = 100;
    m_Params.dp[2] = 12;

    m_Params.ap[0] = 100;
    m_Params.ap[1] = 10;
    m_Params.ap[2] = 10;
    m_Params.ap[3] = 35;
    m_Params.ap[4] = 25;
    m_Params.ap[5] = 0;
}

KGNormalMapCreatorDlg::~KGNormalMapCreatorDlg()
{
    IIniFile* pIniFile = g_OpenIniFile(ParamFile, false, true);

    if (pIniFile)
    {
        for (int i = 0; i < 8; ++i)
        {
            char key[32];
            sprintf(key, "p%d", i);
            pIniFile->WriteInteger("norm", key, m_Params.np[i]);
        }

        for (int i = 0; i < 8; ++i)
        {
            char key[32];
            sprintf(key, "p%d", i);
            pIniFile->WriteInteger("spec", key, m_Params.sp[i]);
        }

        for (int i = 0; i < 8; ++i)
        {
            char key[32];
            sprintf(key, "p%d", i);
            pIniFile->WriteInteger("disp", key, m_Params.dp[i]);
        }

        for (int i = 0; i < 8; ++i)
        {
            char key[32];
            sprintf(key, "p%d", i);
            pIniFile->WriteInteger("ambo", key, m_Params.ap[i]);
        }

        pIniFile->Save(ParamFile);
        pIniFile->Release();
    }

}

void KGNormalMapCreatorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS_TOTAL, m_Progress);
    DDX_Control(pDX, IDC_LIST_INFO, m_InfoList);
}

BOOL KGNormalMapCreatorDlg::OnInitDialog()
{
    BOOL bRes = CDialog::OnInitDialog();

    m_Progress.ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC_NUM)->ShowWindow(SW_HIDE);

    m_InfoList.InsertColumn(0, "Diffuse Map");
    m_InfoList.InsertColumn(1, "Normal(rgb) Spec(a)");
    m_InfoList.InsertColumn(2, "Disp(r) AO(g)");
    m_InfoList.SetColumnWidth(0, 500);
    m_InfoList.SetColumnWidth(1, 190);
    m_InfoList.SetColumnWidth(2, 190);

    m_InfoList.SetExtendedStyle(m_InfoList.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

    IIniFile* pIniFile = g_OpenIniFile(ParamFile);

    if (pIniFile)
    {
        for (int i = 0; i < 8; ++i)
        {
            char key[32];
            sprintf(key, "p%d", i);
            pIniFile->GetInteger("norm", key, m_Params.np[i], &m_Params.np[i]);
        }

        for (int i = 0; i < 8; ++i)
        {
            char key[32];
            sprintf(key, "p%d", i);
            pIniFile->GetInteger("spec", key, m_Params.sp[i], &m_Params.sp[i]);
        }

        for (int i = 0; i < 8; ++i)
        {
            char key[32];
            sprintf(key, "p%d", i);
            pIniFile->GetInteger("disp", key, m_Params.dp[i], &m_Params.dp[i]);
        }

        for (int i = 0; i < 8; ++i)
        {
            char key[32];
            sprintf(key, "p%d", i);
            pIniFile->GetInteger("ambo", key, m_Params.ap[i], &m_Params.ap[i]);
        }


        pIniFile->Release();
    }

    return bRes;
}

BEGIN_MESSAGE_MAP(KGNormalMapCreatorDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_ADD_DIR, &KGNormalMapCreatorDlg::OnBnClickedButtonAddDir)
    ON_LBN_SELCHANGE(IDC_LIST2, &KGNormalMapCreatorDlg::OnLbnSelchangeList2)
    ON_BN_CLICKED(IDC_BUTTON_CLEAR, &KGNormalMapCreatorDlg::OnBnClickedButtonClear)
    ON_BN_CLICKED(IDOK, &KGNormalMapCreatorDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDC_BUTTON_ADD_FILE, &KGNormalMapCreatorDlg::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON_DEL_FILE, &KGNormalMapCreatorDlg::OnBnClickedButton3)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_INFO, &KGNormalMapCreatorDlg::OnLvnItemchangedListInfo)
    ON_NOTIFY(HDN_ITEMDBLCLICK, 0, &KGNormalMapCreatorDlg::OnHdnItemdblclickListInfo)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_INFO, &KGNormalMapCreatorDlg::OnNMDblclkListInfo)
    ON_BN_CLICKED(IDC_BUTTON_GEN_SEL, &KGNormalMapCreatorDlg::OnBnClickedButtonGenSel)
    ON_BN_CLICKED(IDC_BUTTON_DELETE, &KGNormalMapCreatorDlg::OnBnClickedButtonDelete)
    ON_BN_CLICKED(IDC_BUTTON_SETTING, &KGNormalMapCreatorDlg::OnBnClickedButtonSetting)
END_MESSAGE_MAP()

void KGNormalMapCreatorDlg::FillList(CString strPath)
{
    strPath += TEXT("*.*");

    CFileFind fileFind;
    BOOL bContinue = fileFind.FindFile(strPath);

    while (bContinue)
    {
        bContinue = fileFind.FindNextFile();

        if (fileFind.IsDots() || fileFind.IsHidden())
            continue;

        if (fileFind.IsDirectory())
        {
            FillList(fileFind.GetFilePath() + TEXT("\\"));
            continue;
        }

        CString strName = fileFind.GetFileName();
        int nDot = strName.ReverseFind('.');
        CString strExt = strName.Mid(++nDot);
        strExt.MakeLower();

        if (strExt != "jpg" && 
            strExt != "bmp" &&
            strExt != "tga" &&
            strExt != "png"
            )
            continue;

        char szRetPaht[MAX_PATH];
        char szNormalPath[MAX_PATH];
        char szDispAoPath[MAX_PATH];

        g_GetFilePathFromFullPath(szRetPaht, fileFind.GetFilePath().GetBuffer());
        fileFind.GetFilePath().ReleaseBuffer();
        g_pEngineManager->GetHDFilePath(szNormalPath, szDispAoPath, szRetPaht);

        int idx = m_InfoList.GetItemCount();
        m_InfoList.InsertItem(idx, "");

        m_InfoList.SetItemText(idx, 0, szRetPaht);

        if (g_IsFileExist(szNormalPath))
            m_InfoList.SetItemText(idx, 1, strrchr(szNormalPath, '\\') + 1);
        
        if (g_IsFileExist(szDispAoPath))
            m_InfoList.SetItemText(idx, 2, strrchr(szDispAoPath, '\\') + 1);
    }

}

void KGNormalMapCreatorDlg::OnBnClickedButtonAddDir()
{
    TCHAR strDirectory[MAX_PATH];
    BROWSEINFO   BrowseInfo;
    LPITEMIDLIST pItemList;

    memset(&BrowseInfo, 0, sizeof(BROWSEINFO));

    wsprintf(strDirectory, TEXT("%sdata\\source\\maps_source"), g_szDefWorkDirectory);

    USES_CONVERSION;
    SHParseDisplayName(A2CW(strDirectory), NULL, &pItemList, 0, NULL);
    BrowseInfo.hwndOwner = GetSafeHwnd();
    BrowseInfo.pidlRoot = pItemList;

    LPCITEMIDLIST pItem = SHBrowseForFolder(&BrowseInfo);

    if (pItem)
    {
        if (!SHGetPathFromIDList(pItem, strDirectory))
            wsprintf(strDirectory, TEXT("%sdata\\source\\maps_source\\"), g_szDefWorkDirectory);

        if (strDirectory[strlen(strDirectory) - 1] !=  '\\')
            _tcscat(strDirectory, TEXT("\\"));

        FillList(strDirectory);
    }



}

void KGNormalMapCreatorDlg::OnLbnSelchangeList2()
{
    // TODO: Add your control notification handler code here
}

void KGNormalMapCreatorDlg::OnBnClickedButtonClear()
{
    m_InfoList.DeleteAllItems();
}

BOOL KGNormalMapCreatorDlg::ExecuteCmd(const char* cmd, DWORD dwWait)
{
    if (!cmd)
        return FALSE;

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE; 
    ZeroMemory(&pi, sizeof(pi));

    if (CreateProcess(NULL, (LPSTR)cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        WaitForSingleObject(pi.hProcess, dwWait);
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return TRUE;
    }

    return FALSE;
}


void KGNormalMapCreatorDlg::CheckBuildPath(const char file_name[]) {

    char file_path[MAX_PATH];
    char temp_path[MAX_PATH];
    char* t = 0;

    strcpy(file_path, file_name);
    t = strrchr(file_path, '\\');

    if (!t) {
        return;
    }

    (*++t) = 0;

    if (access(file_path, 0) != -1) {
        return;
    }

    t = file_path;

    while ((t = strchr(t, '\\'))) {

        ++t;

        strncpy(temp_path, file_name, t - file_path);
        temp_path[t - file_path] = 0;

        if (access(temp_path, 0) == -1) {

            if (mkdir(temp_path) != 0) {
                return;
            }
        }

    }
}

void KGNormalMapCreatorDlg::ClearSelected()
{
    POSITION pos = m_InfoList.GetFirstSelectedItemPosition();
    while (pos)
    {
        int idx = m_InfoList.GetNextSelectedItem(pos);
        m_InfoList.SetItemState(
            idx,
            0,
            LVIS_SELECTED | LVIS_FOCUSED
            );
    }
}

namespace
{
    void GetToolsFilePath(char szPath[], const char szToolName[])
    {
        *szPath = 0;

        char szResPath[MAX_PATH];
        g_GetRootPath(szResPath);

        size_t pathLen = strlen(szResPath);

        if (szResPath[pathLen - 1] == '\\')
            szResPath[pathLen - 1] = 0;

        char* t = strrchr(szResPath, '\\');

        if (!t)
            return;

        *t = 0;

        sprintf(szPath, "%s\\tools\\%s", szResPath, szToolName);

    }

    void process_impl(void* param)
    {
        KGNormalMapCreatorDlg* pDlg = (KGNormalMapCreatorDlg*)param;

        if (!pDlg)
            return;

        RECT rectItem;
        pDlg->m_InfoList.GetItemRect(0, &rectItem, LVIR_BOUNDS);

        pDlg->GetDlgItem(IDC_BUTTON_ADD_DIR)->EnableWindow(FALSE);
        pDlg->GetDlgItem(IDC_BUTTON_ADD_FILE)->EnableWindow(FALSE);
        pDlg->GetDlgItem(IDC_BUTTON_DEL_FILE)->EnableWindow(FALSE);
        pDlg->GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(FALSE); 
        pDlg->GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(FALSE); 
        pDlg->GetDlgItem(IDC_BUTTON_SETTING)->EnableWindow(FALSE);

        char fullPath[MAX_PATH];
        char cmd[1024];
        char num[32];

        GetToolsFilePath(fullPath, "shadermap.exe");

        if (!g_IsFileExist(fullPath))
        {
            char error[MAX_PATH];
            sprintf(error, "Can not find : %s", fullPath);
            pDlg->MessageBox(error, "Error", MB_OK | MB_ICONWARNING);
        }

        pDlg->m_Progress.ShowWindow(SW_SHOW);
        pDlg->GetDlgItem(IDC_STATIC_NUM)->ShowWindow(SW_SHOW);
        pDlg->m_Progress.SetRange32(0, pDlg->m_InfoList.GetItemCount());
        pDlg->m_Progress.SetPos(0); 
        sprintf(num, "%d/%d", 0, pDlg->m_InfoList.GetItemCount());
        pDlg->SetDlgItemText(IDC_STATIC_NUM, num);

        sprintf(cmd, "%s fprop -disp (BMP8,_d) -norm (TGA24,_n) -spec (BMP8,_s) -ambo (BMP8,_a)",  fullPath);

        if (!pDlg->ExecuteCmd(cmd))
        {
            char error[MAX_PATH];
            sprintf(error, "Set File Format Fail ");
            pDlg->MessageBox(error, "Error", MB_OK | MB_ICONWARNING);
        }

        for (int i = 0; i < pDlg->m_InfoList.GetItemCount() && pDlg->m_bProcess; ++i)
        {
            char srcPath[MAX_PATH];
            char temp[MAX_PATH];

            pDlg->m_InfoList.GetItemText(i, 0, temp, sizeof(temp));
            g_GetFullPath(srcPath, temp);

            char* t = strrchr(srcPath, '.');

            if (!t)
                continue;

            sprintf(temp, "c:\\__temp%s", t);

            if (!CopyFile(srcPath, temp, FALSE))
                continue;
           
            sprintf(cmd, 
                "%s cdiff \"%s\" -disp (%d,%d,%d,xy) -norm (%d,%d,xy,0) -spec (%d,%d,%d,xy) -ambo (%d,%d,%d,%d,%d,%d,xy)",  
                fullPath, 
                temp,
                pDlg->m_Params.dp[0],
                pDlg->m_Params.dp[1],
                pDlg->m_Params.dp[2],
                pDlg->m_Params.np[0],
                pDlg->m_Params.np[1],
                pDlg->m_Params.sp[0],
                pDlg->m_Params.sp[1],
                pDlg->m_Params.sp[2],
                pDlg->m_Params.ap[0],
                pDlg->m_Params.ap[1],
                pDlg->m_Params.ap[2],
                pDlg->m_Params.ap[3],
                pDlg->m_Params.ap[4],
                pDlg->m_Params.ap[5]
                );

            if (pDlg->ExecuteCmd(cmd, 50000))
            {
                const char path_hd1[] = "c:\\__temp_f1.tga";
                const char path_hd2[] = "c:\\__temp_f2.tga";
                const char path_n[] = "c:\\__temp_n.tga";
                const char path_d[] = "c:\\__temp_d.bmp";
                const char path_s[] = "c:\\__temp_s.bmp";
                const char path_a[] = "c:\\__temp_a.bmp";
                const char* p = path_hd1;

                ASSERT(g_IsFileExist(path_n));
                ASSERT(g_IsFileExist(path_d));
                ASSERT(g_IsFileExist(path_s));
                ASSERT(g_IsFileExist(path_a));

                for (int j = 1; j <= 2; ++j)
                {
                    if (j == 1)
                    {
                        sprintf(cmd, "%s pchan \"%s\" -src (\"%s\",rgb,rgb,*) -src (\"%s\",r,a,*)", fullPath, path_hd1, path_n, path_s);
                        p = path_hd1;
                    }
                    else
                    {
                        sprintf(cmd, "%s pchan \"%s\" -src (\"%s\",r,r,*) -src (\"%s\",r,g,*)", fullPath, path_hd2, path_d, path_a);
                        p = path_hd2;
                    }

                    if (pDlg->ExecuteCmd(cmd, 50000))
                    {
                        if (g_IsFileExist(p))
                        {
                            char srcRetPath[MAX_PATH];
                            char path_hd_n[MAX_PATH];
                            char path_hd_d[MAX_PATH];
                            char path_hd_n_full[MAX_PATH];
                            char path_hd_d_full[MAX_PATH];

                            char* p2 = path_hd_n_full;

                            if (j == 2)
                                p2 = path_hd_d_full;

                            g_GetFilePathFromFullPath(srcRetPath, srcPath);
                            g_pEngineManager->GetHDFilePath(path_hd_n, path_hd_d, srcRetPath);

                            if (j == 1)
                                g_GetFullPath(p2, path_hd_n);
                            else
                                g_GetFullPath(p2, path_hd_d);

                            pDlg->CheckBuildPath(p2);

                            if (CopyFile(p, p2, FALSE))
                            {
                                char* t = strrchr(p2, '\\');

                                if (t)
                                    pDlg->m_InfoList.SetItemText(i, j, ++t);
                            }
                        }
                    }
                }

                DeleteFile(path_hd1);
                DeleteFile(path_hd2);
                DeleteFile(path_n);
                DeleteFile(path_d);
                DeleteFile(path_s);
                DeleteFile(path_a);
            }

            DeleteFile(temp);

            pDlg->m_Progress.SetPos(i);
            sprintf(num, "%d/%d", i + 1, pDlg->m_InfoList.GetItemCount());
            pDlg->SetDlgItemText(IDC_STATIC_NUM, num);

            pDlg->ClearSelected();
            pDlg->m_InfoList.SetItemState(
                i,
                LVIS_SELECTED | LVIS_FOCUSED,
                LVIS_SELECTED | LVIS_FOCUSED
                );    

            SIZE sizeScroll;
            ZeroMemory(&sizeScroll, sizeof(sizeScroll));
            int a = i - pDlg->m_InfoList.GetTopIndex();
            sizeScroll.cy = (i - pDlg->m_InfoList.GetTopIndex() - pDlg->m_InfoList.GetCountPerPage() + 5) * (rectItem.bottom - rectItem.top);
            pDlg->m_InfoList.Scroll(sizeScroll);
        }

        pDlg->m_Progress.ShowWindow(SW_HIDE);
        pDlg->GetDlgItem(IDC_STATIC_NUM)->ShowWindow(SW_HIDE);
        pDlg->GetDlgItem(IDOK)->EnableWindow();
        pDlg->GetDlgItem(IDC_BUTTON_ADD_DIR)->EnableWindow();
        pDlg->GetDlgItem(IDC_BUTTON_ADD_FILE)->EnableWindow();
        pDlg->GetDlgItem(IDC_BUTTON_DEL_FILE)->EnableWindow();
        pDlg->GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(); 
        pDlg->GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(); 
        pDlg->GetDlgItem(IDC_BUTTON_SETTING)->EnableWindow();
        pDlg->SetDlgItemText(IDOK, "生成");
        pDlg->m_bProcess = FALSE;
    }
}

void KGNormalMapCreatorDlg::OnBnClickedOk()
{
    if (m_bProcess)
    {
        GetDlgItem(IDOK)->EnableWindow(FALSE);
        m_bProcess = FALSE;
        SetDlgItemText(IDOK, "生成");
    }
    else
    {
        ClearSelected();
        m_InfoList.SetFocus();

        SetDlgItemText(IDOK, "停止");
        m_bProcess = TRUE;
        _beginthread(process_impl, 0, this);
    }
    
}

void KGNormalMapCreatorDlg::OnBnClickedButton2()
{
    TCHAR strDirectory[MAX_PATH];
    wsprintf(strDirectory, TEXT("%sdata\\source\\maps_source"), g_szDefWorkDirectory);

    TCHAR strFileNameBuffer[MAX_PATH * 10] = "";
    TCHAR strFilter[] = "tga Files (*.tga)|*.tga|bmp Files (*.bmp)|*.bmp|jpg Files (*.jpg)|*.jpg|all Files (*.*)|*.*||";
    TCHAR strDefaultPath[MAX_PATH];
    LPCTSTR pszFullName = NULL;

    strDefaultPath[0] = '\0';

    CFileDialog DlgLoad(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, strFilter);

    DlgLoad.m_ofn.lpstrFile = strFileNameBuffer;
    DlgLoad.m_ofn.nMaxFile = sizeof(strFileNameBuffer);
    DlgLoad.m_ofn.lpstrInitialDir = strDirectory;

    if (DlgLoad.DoModal() != IDOK)
        return;


    POSITION FilePos = DlgLoad.GetStartPosition();

    while (FilePos)
    {
        CString strPathName = DlgLoad.GetNextPathName(FilePos);
        CString strName = strPathName.Mid(strPathName.ReverseFind('\\') + 1);

        char szRetPaht[MAX_PATH];
        char szNormalPath[MAX_PATH];
        char szDispAoPath[MAX_PATH];

        g_GetFilePathFromFullPath(szRetPaht, strPathName);
        strPathName.ReleaseBuffer();
        g_pEngineManager->GetHDFilePath(szNormalPath, szDispAoPath, szRetPaht);

        int idx = m_InfoList.GetItemCount();
        m_InfoList.InsertItem(idx, "");

        m_InfoList.SetItemText(idx, 0, szRetPaht);

        if (g_IsFileExist(szNormalPath))
            m_InfoList.SetItemText(idx, 1, strrchr(szNormalPath, '\\') + 1);

        if (g_IsFileExist(szDispAoPath))
            m_InfoList.SetItemText(idx, 2, strrchr(szDispAoPath, '\\') + 1);

    }
}

void KGNormalMapCreatorDlg::OnBnClickedButton3()
{
    while (true)
    {
        POSITION pos = m_InfoList.GetFirstSelectedItemPosition();

        if (!pos)
            break;

        m_InfoList.DeleteItem(m_InfoList.GetNextSelectedItem(pos));
    }
}

void KGNormalMapCreatorDlg::OnLvnItemchangedListInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

   

    *pResult = 0;
}

void KGNormalMapCreatorDlg::OnHdnItemdblclickListInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
}

void KGNormalMapCreatorDlg::OnNMDblclkListInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

    char path_dif[MAX_PATH];
    char path_hd_n[MAX_PATH];
    char path_hd_d[MAX_PATH];
    char full_path[MAX_PATH];

    if (phdr->iItem == -1)
        return;

    m_InfoList.GetItemText(phdr->iItem, 0, path_dif, sizeof(path_dif));
    
    if (g_IsFileExist(path_dif))
    {
        g_GetFullPath(full_path, path_dif);
        ShellExecute(NULL, "open", full_path, NULL, NULL, SW_SHOWNORMAL);
        Sleep(500);
    }

    g_pEngineManager->GetHDFilePath(path_hd_n, path_hd_d, path_dif);

    if (g_IsFileExist(path_hd_n))
    {
        g_GetFullPath(full_path, path_hd_n);
        ShellExecute(NULL, "open", full_path, NULL, NULL, SW_SHOWNORMAL);
        Sleep(500);
    }

    if (g_IsFileExist(path_hd_d))
    {
        g_GetFullPath(full_path, path_hd_d);
        ShellExecute(NULL, "open", full_path, NULL, NULL, SW_SHOWNORMAL);
    }

    *pResult = 0;
}

void KGNormalMapCreatorDlg::OnBnClickedButtonGenSel()
{
    // TODO: Add your control notification handler code here
}

void KGNormalMapCreatorDlg::OnBnClickedButtonDelete()
{
    POSITION pos = m_InfoList.GetFirstSelectedItemPosition();
    while (pos)
    {
        int idx = m_InfoList.GetNextSelectedItem(pos);
        char path_diff[MAX_PATH];
        m_InfoList.GetItemText(idx, 0, path_diff, sizeof(path_diff));

        char path_hd_n[MAX_PATH];
        char path_hd_d[MAX_PATH];
        char path_hd_n_full[MAX_PATH];
        char path_hd_d_full[MAX_PATH];

        g_pEngineManager->GetHDFilePath(path_hd_n, path_hd_d, path_diff);
        g_GetFullPath(path_hd_n_full, path_hd_n);
        g_GetFullPath(path_hd_d_full, path_hd_d);
        
        if (DeleteFile(path_hd_n_full))
            m_InfoList.SetItemText(idx, 1, "");

        if (DeleteFile(path_hd_d_full))
            m_InfoList.SetItemText(idx, 2, "");
    }
}

void KGNormalMapCreatorDlg::OnBnClickedButtonSetting()
{
    KGNormalMapCtreatorSetting dlg(m_Params, this);

    if (dlg.DoModal() == IDOK)
    {
        m_Params = dlg.GetParams();
    }
}
