//////////////////////////////////////////////////////////////////////////

#pragma once
#include "IEOther.h"


extern LPDIRECT3DDEVICE9 g_pd3dDevice;
extern TCHAR g_strWeaponDirectory[MAX_PATH];
extern TCHAR g_strItemDirectory[MAX_PATH];



extern IEKG3DEngineManager* g_pEngineManager;
extern IEKG3DGraphicsTool* g_pGraphicsTool;

BOOL Init3DEngine(unsigned uMode, unsigned int uFlag, HWND hBaseWindow, HWND hRenderWindow, BSTR strEnginePath, BSTR strStartUpPath);

inline IEKG3DEngineManager* g_GetEngineIEInterface()
{
	return g_pEngineManager;
}


void InitGlobalVars(IEKG3DEngineManager* pInterface);

class CDllHolder
{
public:
	CDllHolder()
	{
		m_hMod = NULL;
	}
	void Init(LPCTSTR lpstrDllName)
	{
		m_hMod = LoadLibrary(lpstrDllName);
		_ASSERTE(m_hMod);
	}
	void UnInit()
	{
		if (m_hMod)
		{
			FreeLibrary(m_hMod);
			m_hMod = NULL;
		}
	}

	~CDllHolder()
	{
		UnInit();
	}
	HMODULE GetDllHandle()
	{
		return m_hMod;
	}
private:
	HMODULE m_hMod;
};