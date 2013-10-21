////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGUI.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2007-11-29 10:09:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGUI_H_
#define _INCLUDE_KGUI_H_

////////////////////////////////////////////////////////////////////////////////

#include "SO3UI/IKGUI.h"
#include "./driver/kpointer.h"
#include "./driver/khotkeymgr.h"
#include "./driver/kinput.h"
#include "./driver/kconfig.h"
#include "./driver/kfilepathmgr.h"
#include "./driver/kimageinfomgr.h"
#include "./driver/kanimateimagemgr.h"
#include "./driver/kmusicmgr.h"
#include "./driver/UIEditorMgr.h"
#include "./elem/wndstation.h"
#include "./scheme/kcolorschememgr.h"
#include "./scheme/kfontschememgr.h"
#include "./scheme/kglobalstringvaluablemrg.h"
#include "./script/kscriptmgr.h"
#include "./case/uiconfig.h"
#include "./common/kbintab.h"
#include "./Common/KG_I18N.h"

namespace UI
{

	enum
	{
		START_GAME_LOGIN,
		RETURN_GAME_LOGIN,
		RETURN_ROLE_LIST,
		RETURN_CLEAR_UI,
        KICK_OUT_BY_GM,
        KICK_OUT_BY_OTHERS,
        AUTO_EXIT,
	};

	struct KIconModel
	{
		DWORD dwID;
		LPCSTR szFileName;
		DWORD dwFrame;
		LPCSTR szKind;
		LPCSTR szSubKind;
		LPCSTR szTag1;
		LPCSTR szTag2;

		bool operator<(KIconModel const& rhs) const
		{
			return dwID < rhs.dwID;
		}
	};

	class KGUI : public IKGUI
	{
	public:
		virtual int Init(const KGUI_INIT_PARAM *pParam);
		virtual void SetSoundShell(IKG3DSoundShell *pShell);
		virtual int NeedReset();
		virtual void Reset();
		virtual void Run();
		virtual void Exit();

		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual void Active();
		virtual void Paint();

		virtual lua_State* GetLuaState();

		virtual void RegisterMetaTable(LPCSTR pszTable, luaL_reg const* pMetaFunc);
		virtual void RegisterLibTable(LPCSTR pszTable, luaL_reg const* pLibFunc);
		virtual void RegisterFunctions(luaL_reg const* pFunc);
		virtual void RegisterConstList(KLuaConstList const* pConstList);
		virtual void RegisterFunctions(KLuaFunc const* pFunc, int nCount);

		virtual BOOL GetPassword(LPCSTR szTreePath, char szDestMD5String[64]);

		virtual void PushArg(LPCSTR szName, bool bParam);
		virtual void PushArg(LPCSTR szName, int nParam);
		virtual void PushArg(LPCSTR szName, DWORD dwParam);
		virtual void PushArg(LPCSTR szName, float fParam);
		virtual void PushArg(LPCSTR szName, double dblParam);
		virtual void PushArg(LPCSTR szName, LPCSTR pcszParam);
		virtual void FireEvent(LPCSTR szEvent, DWORD dwCookie);

        virtual int SetCodePage(unsigned int uCodePage);
        virtual unsigned int GetCodePage();

		void ReInit(int nReason);
		int GetInitReason();

		KIconModel const* GetIconModel(DWORD dwID) const;

	public:
		KConfig m_Config;
		KFilePathMgr m_FilePathMgr;
		KGlobalStrValuableMgr m_GlobalStrValuableMgr;
		KImageInfoMgr m_ImageInfoMgr;
		KAnimateImageMgr m_AnimateImageMgr;
		KInput m_Input;
		KPointer m_Pointer;
		KColorSchemeMgr m_ColorSchemeMgr;
		KFontSchemeMgr m_FontSchemeMgr;

		KBinTab m_IconModelTab;
		KIconModel* m_pIconModel;
		size_t m_nIconModelCount;

		KMusicMgr m_MusicMgr;
		KScriptMgr m_Script;
		KWndStation m_WndStation;
		KHotkeyMgr m_HotkeyMgr;
		KUIEditorMgr m_EditorMgr;

		IKG3DUI* m_p3DUI;
		IKG3DEngineManager* m_p3DEngineManager;
		IKG3DResourceManager* m_p3DModelManager;
		IKG3DSoundShellHolder* m_pSoundShellHolder;

		HINSTANCE m_hInst;
		HWND m_hWnd;	

        HWND* m_phwndEmbedWebPage;

        char m_szKLVideoDllPath[MAX_PATH];
        char m_szConfigPath[MAX_PATH];
        char m_szVersionPath[MAX_PATH];

	private:
	    BOOL LoadAllScriptTable();

	private:
		BOOL m_bReInit;
		int	m_nInitReason;
	};

	LPCSTR GetIconFullPath(LPCSTR szFilePath);

	extern KGUI* g_pUI;
}//namespace UI

#endif //_INCLUDE_KGUI_H_
