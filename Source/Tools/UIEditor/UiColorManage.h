# pragma once

namespace UICOLORMANAGE
{


    class KUiColorManage
    {
    public:
        KUiColorManage();
        ~KUiColorManage();

	    int Init(HWND hWnd);
	    void Release();
        void OnColorManage(char *pszColorFileName, HWND hParent, BOOL bManage = FALSE);

        void    Draw(LPDRAWITEMSTRUCT lpDrawItem);
        void	OnButtonClick(int nBtnId);
        void	OnUpdateEditText(int nId);
        void    OnListNotify(LPNMHDR pNotify);
        int     IsColorExist(const char *pszNewColor);
        void    OnSave(const char *pszColorFileName);
        void    OnChangeColor();
        HWND    GetHandle() { return m_hWnd;  };
        HWND    GetListHandle(){ return m_hListWnd; };
        char*   GetColor(){  return m_szColor; };
    private:
        BOOL m_bManage;
        HWND m_hListWnd;
        HWND m_hWnd;
        int m_nCurrentItemNum;
        char m_szColorFileName[MAX_PATH];
        char m_szColor[32];
    };


    BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    COLORREF  GetColorValueByName(const char *pszColorName);
    int SelColor(HWND hWnd, char *pszColor);
    int ColorManage(HWND hWnd);

	void ClearColorArray();
	int LoadColorArray(const char *szPath);

}