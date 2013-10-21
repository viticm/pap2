////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KWndNameList.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-20 10:46:53
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KWNDNAMELIST_H_
#define _INCLUDE_KWNDNAMELIST_H_

////////////////////////////////////////////////////////////////////////////////

class KWndNameList
{
protected:
    KWndNameList();
    ~KWndNameList();
public:
    static int Init(HINSTANCE hInst, HWND hWnd);
    static int Exit();
    static KWndNameList &GetSelf();

    int CreateDialogWnd(HINSTANCE hInst, HWND hWnd);
    int DestroyDialogWnd();
    HWND GetHWND();
    HWND GetListHWND();
    HWND GetParentHWND();

    int Append(LPCTSTR pcszName);
    int Remove(LPCTSTR pcszName);
    int Clear();

    int OnListNotify(LPNMHDR pNotify);
    int Show();
    int Hide();
    int SetColumn();

private:
    int OnRButtonClick(int nIndex);  
    int OnExportTga(int nBlockIndex);
    int OnReplaceTgaBlock(int nBlockIndex);
    int OnLButtonDown();
protected:
    static	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
    static KWndNameList *ms_pself;
    HWND m_hWndParent;
    HWND m_hWnd;
    HWND m_hListWnd;
    int m_nCurrentSortType;
    HINSTANCE m_hInstance;
    int m_nGetSelBlockFlag;
    int m_nThePelaceBlockIndex;
};






#endif //_INCLUDE_KWNDNAMELIST_H_
