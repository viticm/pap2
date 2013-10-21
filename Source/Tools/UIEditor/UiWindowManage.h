# pragma once
#include "UiEditor.h"

namespace UIWINDOWMANAGE
{
    class KUiWindowManage
    {
    public:
        KUiWindowManage();
        ~KUiWindowManage();

	    int Init(HWND hWnd);
	    void Release();
        void OnWindowManage(HWND hParent, IIniFile *pOut, const char *pcszparentName, BOOL bManage = FALSE);

		void InitMgr();

        void	OnButtonClick(int nBtnId);
        void    OnListNotify(LPNMHDR pNotify);
        int     IsWndExist(const char *pszWndName);
        void    Save();
        int     SelWindow(HWND hWnd, IN IIniFile *pIni, IN const char *pcszName, OUT char *pszNewName);
		int     SelWindow(HWND hWnd, IN IIniFile *pIni, IN const char *pcszName, IN const char *pcszFatherName, OUT char *pszNewName);

        void    AddWnd(const char *pszWndName, IIniFile *pIni, OUT char *pszNewName);
        void    OnChangeWndName();
        void    OnDelWnd();
        void    OnSelWnd();

        HWND    GetHandle() { return m_hWnd;  };
        HWND    GetListHandle(){ return m_hListWnd; };
        char   *GetSelWnd(){ return m_szSelWndName; };
        BOOL    IsSeled() { return m_bSel;};
    private:
        BOOL m_bManage;
        HWND m_hListWnd;
        HWND m_hWnd;
        int m_nCurrentItemNum;
        BOOL m_bSel;
        char m_szWndFileName[MAX_PATH];
        char m_szSelWndName[128];
		char m_szParentName[128];
        IIniFile *m_pWndIni;
        IIniFile *m_pOutIni;
    };

    BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    int SelWindow(HWND hWnd, IIniFile *pIni, IN const char *pszFatherName, OUT char *pszName);
    int SelWindow(HWND hWnd, IN IIniFile *pIni, IN const char *pcszName, IN const char *pPatherName, OUT char *pszNewName);
    int AddWindow(const char *pszWndName, IIniFile *pIni, OUT char *pszNewName);
    int WindowManage(HWND hWnd);
    void SaveWindowManageData();

	int InitWindowManage();
}