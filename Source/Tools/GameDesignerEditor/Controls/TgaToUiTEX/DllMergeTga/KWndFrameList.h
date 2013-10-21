////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KWndFrameList.h
//  Version     : 1.0
//  Creator     : Lin Jiaqi
//  Create Date : 2005-12-20 10:46:53
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KWNDFRAMELIST_H_
#define _INCLUDE_KWNDFRAMELIST_H_

#define TGA_HEIGHT 32
////////////////////////////////////////////////////////////////////////////////
class KWndFrameList
{
protected:
	KWndFrameList();
	~KWndFrameList();
public:
	static int Init(HINSTANCE hInst, HWND hWnd);
	static int Exit();
	static KWndFrameList &GetSelf();

	int CreateDialogWnd(HINSTANCE hInst, HWND hWnd);
	int DestroyDialogWnd();
	HWND GetHWND();
	HWND GetListHWND();
	HWND GetParentHWND();

    int AppendFrameGroup(
        LPCTSTR pcszGroupName, LPCTSTR pcszFrameName, 
        int nId, int nFrame, int nWidth, int nHeight, 
        LPCTSTR pcszRemark, BYTE *pbyBuffer, int nBufferSize
        );
	int ClearFrameGroup();

	int Show();
	int Hide();
    int Clear();
    int Draw(LPDRAWITEMSTRUCT lParam);

protected:
	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	static KWndFrameList *ms_pself;
	HWND m_hWndParent;
	HWND m_hWnd;
	HWND m_hListWnd;
	int m_nCurrentSortType;
	HINSTANCE m_hInstance;
	int m_nGetSelBlockFlag;
	int m_nThePelaceBlockIndex;
};



#endif //_INCLUDE_KWNDFRAMELIST_H_