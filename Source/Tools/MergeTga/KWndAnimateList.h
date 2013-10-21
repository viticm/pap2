////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KWndAnimateList.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-27 16:14:16
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KWNDANIMATELIST_H_
#define _INCLUDE_KWNDANIMATELIST_H_

////////////////////////////////////////////////////////////////////////////////

class KWndAnimateList
{
    enum DRAW_TYPE
    {
        DRAW_NOTHING,
        DRAW_ANIMATE,
        DRAW_FRAME,
    };

    enum LBUTTONDOW_DOTHING
    {
        DO_NOTING,
        DO_APPENDFRAME,
        DO_INSERTFRAME,
    };
protected:
    KWndAnimateList();
    ~KWndAnimateList();
public:
    static int Init(HINSTANCE hInst, HWND hWnd);
    static int Exit();
    static KWndAnimateList &GetSelf();

    int CreateDialogWnd(HINSTANCE hInst, HWND hWnd);
    int DestroyDialogWnd();
    HWND GetHWND();
    HWND GetAnimateListHWND();
    HWND GetFrameListHWND();
    HWND GetParentHWND();

    int AppendAnimateGroup(LPCTSTR pcszGroupName);
    int ClearAnimateGroup();
    int AppendAnimateFrame(LPCTSTR pcszFrameName);    
    int ClearAnimateFrame();

    int OnAnimateListNotify(LPNMHDR pNotify);
    int OnFrameListNotify(LPNMHDR pNotify);

    int Show();
    int Hide();

    int SetTimer();
    int ClearTimer();
    int OnTimer();

    int DrawFrame(int nGroup, int nFrame);
    int ClearDraw();

    int OnSelGroup(int nGroup);
    int OnSelFrame(int nFrame);

private:
    int OnRButtonClickGroup(int nIndex);  
    int OnRButtonClickFrame(int nIndex);  
    int OnExportTgaFrame();
    int OnExportTgaGroup();
    int OnLButtonDown();
    int OnNewAnimateGroup();
    static int OnNewAnimateGroupNameSeted(HWND hWnd);
protected:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK NewAnimateGroupProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
private:
    static KWndAnimateList *ms_pself;
    HWND m_hWndParent;
    HWND m_hWnd;
    HWND m_hWndAnimateList;
    HWND m_hWndFrameList;
    HWND m_hWndDrawWindow;
    HINSTANCE m_hInstance;

    int m_nCurrentSelGroup;
    int m_nCurrentSelFrame;

    int m_nCurrentDrawFrame;
    int m_nDrawType;

    int m_nLButtonDownDoWhat;

    UINT_PTR m_puTimer;

};







#endif //_INCLUDE_KWNDANIMATELIST_H_
