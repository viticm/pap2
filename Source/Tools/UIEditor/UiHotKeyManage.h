# pragma once
#include "UiEditor.h"

class KUiHotKeyManage
{
public:
    KUiHotKeyManage();
    ~KUiHotKeyManage();
    int     Init(HWND hWnd);

    bool    GetGetKeyState() const { return m_bNowGetKey;};
    void    OnListNotify(LPNMHDR pNotify, LPARAM lParam);
    void    OnHotKeyManage(HWND hWnd);
    HWND    GetHandle() { return m_hWnd; };
    HWND    GetListHandle(){ return m_hListWnd; };
private:
    HWND m_hWnd;
    HWND m_hListWnd;
    IIniFile *m_pIni;
    bool m_bNowGetKey;
};

BOOL CALLBACK g_HotKeyDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int HotKeyManage(HWND hWnd);


