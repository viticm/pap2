# pragma once
#include "UiEditor.h"

namespace UIPROJECTMANAGE
{
    class KUiProjectManage
    {
    public:
        enum PROJECT_FILE_TYPE
        {
            INI_FILE = 0,
            LUA_FILE,

            UNKNOWN_FILE,
        };
    public:
        KUiProjectManage();
        ~KUiProjectManage();

	    int     Init(HWND hWnd);
	    void    Release();
        void    OnProjectManage(HWND hParent, BOOL bManage = FALSE);

        HWND    GetProjectListHandle(){ return m_hProjectListWnd; };
        HWND    GetFileListHandle(){ return m_hFileListWnd; };
        char   *GetSelFileName() { return m_szSelFileName; };
        BOOL    IsSel(){ return m_bSeled; };

        void OnProjectListNotyfy(LPNMHDR pNotify);
        void OnFileListNotify(LPNMHDR pNotify);
        void OnButtonClick(int nBtnId);

        void Save();

		void InitMgr();

    private:

        char* EquirotalStrStr(const char *pString, const char *pStrSearch);
        void FillProjectList();
        void FillFileList(LPCTSTR pszProjectName);

        void OnSelProjectPath();
        void OnNewProject();
        void OnDelProject();
        void OnDelFile();
        void OnAddFile();
        void OnNewFile();
        void OnEditFile();

        void SetIniByCurrentFileList();
        void AddAFileToProject(LPCTSTR pszFileName);

        int GetAdjustIniFromLua(IN LPCTSTR pszLua, OUT LPTSTR pszIni);
        int GetAdjustLuaFromIni(IN LPCTSTR pszIni, OUT LPTSTR pszLua);

        int IsFileExistInProject(LPCTSTR pszFileName, PROJECT_FILE_TYPE eType, OUT int *pnWhere = NULL);

        PROJECT_FILE_TYPE GetProjectFileType(LPCTSTR pszName);

    private:
        BOOL m_bManage;
        BOOL m_bSeled;

        HWND m_hWnd;
        HWND m_hProjectListWnd;
        HWND m_hFileListWnd;

        int m_nCurrentProject;  //当前选中第几个工程的索引!
        int m_nCurrentFile;

        char m_szSelFileName[MAX_PATH]; //被选中的ini文件名(包含路径).
        char m_szProjectSettingFile[MAX_PATH];

        IIniFile *m_pIni;


    };

    BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	int InitProjectManage();

    int ProjectManage(HWND hParent, OUT char *pszOpenIniFileName);

}