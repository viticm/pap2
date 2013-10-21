////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KIconManager.h
//  Version     : 1.0
//  Creator     : Lin Jiaqi
//  Create Date : 2005-12-22 14:14:13
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KICONMANAGER_H_
#define _INCLUDE_KICONMANAGER_H_

////////////////////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////////////////////

class KIconBlock;

class KIconManager
{
	typedef std::vector<KIconBlock *> KIconArray;
protected:
	KIconManager();
	~KIconManager();

public:
	static int Init();
	static int Exit();
	static KIconManager &GetSelf();

	int UpDataListView();

    int SetHwnd(HWND hWnd);
    HWND GetHwnd();

	int Append(KIconBlock *pIcon);
	int Clear();
	int ClearSaveData();
    int GetIconCount();
	KIconBlock *Get(int nIndex);

protected:

private:
	static KIconManager *ms_pSelf;
    HWND m_hWnd;
	KIconArray m_aIcon;
};




#endif //_INCLUDE_KICONMANAGER_H_
