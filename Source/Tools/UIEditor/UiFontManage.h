# pragma once
#include "UiEditor.h"

namespace UIFONTMANAGE
{

    class KUiFontManage
    {
    public:
        KUiFontManage();
        ~KUiFontManage();

	    int Init(HWND hWnd);
	    void Release();
        void OnFontManage(HWND hParent, BOOL bManage = FALSE);

        void    Draw(LPDRAWITEMSTRUCT lpDrawItem);
        void	OnButtonClick(int nBtnId);
        void	OnUpdateEditText(int nId);
        void    OnListNotify(LPNMHDR pNotify);
        int     IsFontExist(const char *pszNewFont);
        int     GetIdByName(const char *pszName, int *pnId);
        void    OnSave(const char *pszFontFileName);

        void    OnChangeColor(int nIndex, BOOL bInIni);
        void    OnAdd();
        void    OnChange();

        HWND    GetHandle() { return m_hWnd;  };
        HWND    GetListHandle(){ return m_hListWnd; };
        int     GetFont(){  return m_nCurrentItemNum; };
        BOOL    IsSeled() { return m_bSel;};
        int     GetNameById(int nId, char *pszName);
        int     ReInitIni();
    private:
        BOOL m_bManage;
        HWND m_hListWnd;
        HWND m_hWnd;
        int m_nCurrentItemNum;
        BOOL m_bSel;
        char m_szFontFileName[MAX_PATH];
        char m_szFont[32];
        IIniFile *m_pFontIni;
    };

    BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    int SelFont(HWND hWnd, int *pnFont);
    int FontManage(HWND hWnd);
    
    int GetNameById(int nId, char *pszName);

}

