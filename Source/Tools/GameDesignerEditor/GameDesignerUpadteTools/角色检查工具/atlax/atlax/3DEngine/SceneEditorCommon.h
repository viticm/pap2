#pragma once
#include "IEOther.h"
#ifdef _DEBUG
	//extern IEKG3DEngineManager* g_pEngineManager;
	class CEngineManagerHelper
	{
		IEKG3DEngineManager* m_pEngineManager;
	public:
		BOOL Init(IEKG3DEngineManager* p)
		{
			_ASSERTE(p);
			m_pEngineManager = p;
			return p != NULL;
		}
		IEKG3DEngineManager* operator ->()
		{
			//_ASSERTE(m_pEngineManager);
			return m_pEngineManager;
		}
		typedef IEKG3DEngineManager* LPENGINEMANAGER;
		operator LPENGINEMANAGER ()
 		{
 			//_ASSERTE(m_pEngineManager);
 			return m_pEngineManager;
 		}
		void operator = (IEKG3DEngineManager*);
		IEKG3DEngineManager* GetInterface();
	};
	extern CEngineManagerHelper g_pEngineManager;
	extern IEKG3DGraphicsTool* g_pGraphicsTool;
#else
	extern IEKG3DEngineManager* g_pEngineManager;
	extern IEKG3DGraphicsTool* g_pGraphicsTool;
#endif

extern TCHAR g_szDefAppDirectory[256];
extern LPDIRECT3DDEVICE9 g_pd3dDevice;

extern TCHAR g_strWeaponDirectory[MAX_PATH];
extern TCHAR g_strItemDirectory[MAX_PATH];

void InitGlobalVars(IEKG3DEngineManager* pInterface);
