////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : itemeditmodemgr.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-22 15:18:40
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_ITEMEDITMODEMGR_H_
#define _INCLUDE_ITEMEDITMODEMGR_H_

////////////////////////////////////////////////////////////////////////////////

#ifndef DISABLE_UI_EDIT

enum {EDIT_DRAG_MODE = 1};
enum {EDIT_RESIZE_MODE = 2};
enum {EDIT_NOT_DRAG_AND_NOT_RESIZE_MODE = 3};

#include "./item.h"
namespace UI
{

class KItemEditModeMgr
{
public:
    ~KItemEditModeMgr();
    KItemEditModeMgr();
	static bool Init();
	static void Exit();
    static KItemEditModeMgr& GetSelf();
    LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    int SetCurrentEditItem(KItemNull *pItem);
    KItemNull *GetCurrentEditItem();
    int ClearCurrentEditItem();
    int SetDragStartPos(float fX, float fY);
    int GetFragStartPos(float &fX, float &fY);
    int SetEditMode(int nMode);
    int GetEditMode();
protected:
private:
    static KItemEditModeMgr *ms_pSelf;
    KItemNull *m_pCurrentEditItem;
    float m_fDragStartX;
    float m_fDragStartY;
    int m_nDragMode;
    int m_nResizeMode;
};

} //namespace UI

#endif


#endif //_INCLUDE_ITEMEDITMODEMGR_H_
