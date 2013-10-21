////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiGlobalStringValueMgr.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-2-14 13:40:09
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_UIGLOBALSTRINGVALUEMGR_H_
#define _INCLUDE_UIGLOBALSTRINGVALUEMGR_H_

////////////////////////////////////////////////////////////////////////////////

#include "UiEditor.h"

////////////////////////////////////////////////////////////////////////////////

class KUiGlobalStrMgr
{
private:
	struct KGLOBALSTRINFO 
	{
		char szID[32];
		int nSize;
		_tstring strContent;

		KGLOBALSTRINFO()
		{
			szID[0] = '\0';
			nSize = 8;
			strContent[0] = _T('\0');
		}
	};
	typedef std::vector<KGLOBALSTRINFO> KGlobalStrArray;
	KUiGlobalStrMgr();
	~KUiGlobalStrMgr();
public:
	static int Init();
	static int Exit();
	static KUiGlobalStrMgr &GetSelf();
	static BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	int Load();
	int Save();
	const char *GetString(const char *pcszKey);

	const char *SelString(HWND hWndParent); //return the Key
	int StringManage(HWND hParent);
	int Add(const char *pcszKey, int nLength, const char *pcszValue);

	int InitShow(HWND hWnd);

	int OnListNotify(LPNMHDR pNotify);
	int OnUpdataEditText(int nId);
	int OnButtonClick(int nId);

	int OnDel();
	int OnAdd();
	int OnChange();
	int OnSel();
	int IskeyExist(const char *pcszkey);
	int IsValueLegality(const char *pcszKey);
protected:
	int UpDataShowFormStrArray();
	int UpDataStrArrayFormShow();
private:
	static KUiGlobalStrMgr *ms_pSelf;
	KGlobalStrArray m_aGlobalStr;

	HWND	m_hWnd;
	HWND	m_hWndList;
	char	m_szSelKey[128];
	char    m_szSelValue[1024];
	int		m_nCurrentEditItem;
	int		m_nTabChange;
	char	m_szStringFileName[MAX_PATH];
};


#endif //_INCLUDE_UIGLOBALSTRINGVALUEMGR_H_
