////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : SceneEditorCommon.h
//  Version     : 
//  Creator     : 
//  Create Date : 
//  Comment     : 这个文件用于定义全局参数。请不要在这里和cpp放和全局参数无关的东西
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_SCENEEDITORCOMMON_H_
#define _INCLUDE_SCENEEDITORCOMMON_H_

////////////////////////////////////////////////////////////////////////////////
#include "IEOther.h"
#if defined(_DEBUG) | defined(DEBUG)
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
			_ASSERTE(m_pEngineManager);
			return m_pEngineManager;
		}
		typedef IEKG3DEngineManager* LPENGINEMANAGER;		
		void operator = (IEKG3DEngineManager*);
		IEKG3DEngineManager* GetInterface();
	private:
		struct PrivateStruct{};
		typedef PrivateStruct* LPPrivateStruct;
/*
不允许指针自动转换成IE，但可以用->，因为外部很多地方都是不检查g_pEngineManager就用的。
在g_pEngineManager初始化失败的极少数情况中，g_pEngineManager是需要检查
是否为空的，而如果是提供自动转型，则如果g_pEngineManager，各个释放函数
中用到的g_pEngineManager就拼命出ASSERT了。而且根据SmartPtr的约定，尽量也不
应该提供原指针自动转型，不然在函数的参数中发生的自动转型会不知不觉绕开
我们的检查机制。代替的，我们如果非要得到原指针，可以用GetInterface。
恕我在代码中罗嗦了，过一段时间就删掉这些东西吧。
*/
	public:
		operator LPPrivateStruct(){return m_pEngineManager? (PrivateStruct*)0x00000001 : NULL;}///用于if(g_pEngineManager)
	};
	extern CEngineManagerHelper g_pEngineManager;
	extern IEKG3DGraphicsTool* g_pGraphicsTool;
	extern IKG3DRTTITypeManager* g_pKG3DRTTITypeManager;///<RTTI_INTERFACE>
#else
	extern IEKG3DEngineManager* g_pEngineManager;
	extern IEKG3DGraphicsTool* g_pGraphicsTool;
	extern IKG3DRTTITypeManager* g_pKG3DRTTITypeManager;///<RTTI_INTERFACE>
#endif

inline IEKG3DEngineManager* g_GetEngineIEInterface()
{
#if defined(_DEBUG) | defined(DEBUG)
	return g_pEngineManager.GetInterface();
#else
	return g_pEngineManager;
#endif
}

extern TCHAR g_szDefWorkDirectory[256];
extern TCHAR g_szDefExeDirectory[MAX_PATH];
extern LPDIRECT3DDEVICE9 g_pd3dDevice;

extern TCHAR g_strWeaponDirectory[MAX_PATH];
extern TCHAR g_strItemDirectory[MAX_PATH];

void InitGlobalVars(IEKG3DEngineManager* pInterface);

class CDllHolder
{
public:
	HRESULT Init(LPCTSTR lpstrDllName);
	void UnInit();
	HMODULE GetDllHandle(){	return m_hMod;}

	CDllHolder():m_hMod(NULL){}
	~CDllHolder(){UnInit();}
	
private:
	HMODULE m_hMod;
	std::tstring	m_strDllName;
};


#endif //_INCLUDE_SCENEEDITORCOMMON_H_
