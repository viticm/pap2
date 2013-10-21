////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DCollocator.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-4-10 17:58:55
//  Comment     : //类关系配置器，主要用于注册一些构造器，在EngineManager的构造函数中执行
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DCOLLOCATOR_H_
#define _INCLUDE_KG3DCOLLOCATOR_H_

//用于配置类关系
////////////////////////////////////////////////////////////////////////////////

interface IEKG3DCollocator;

typedef const BOOL* LPCONSTBOOL;
class KG3DCollocator
{
public:
	static void CollocateBeforeInit();
	static void CollocateAfterInit(LPDIRECT3DDEVICE9 pDevice);	//初始化的什么都可以放这里，这样就不用撑大KG3DEngineManger.cpp的代码了
	static void UnCollocateBeforeUnInit(LPDIRECT3DDEVICE9 pDevice);	//和上面的对应，释放不需要的资源

	static IEKG3DCollocator* GetAnotherInterface();

	//<GLOBAL_PATH>
	template<size_t uBufferSize>
	static HRESULT GetSavePath(DWORD dwType, const KGCH::StrPathSplited& scenePathSplited, KGCH::StrPathSplited* pRetPathSplited
								, TCHAR (&pathRet)[uBufferSize])
	{
		return GetSavePath(dwType, scenePathSplited, pRetPathSplited, pathRet, uBufferSize);
	}
	static HRESULT GetSavePath(DWORD dwType, const KGCH::StrPathSplited& scenePathSplited,  KGCH::StrPathSplited* pRetPathSplited
								, LPTSTR pathRet, size_t uPathBufferSize);//从map文件的路径得到其它附属文件的保存路径
	//</GLOBAL_PATH>

	//<GLOBAL_SWITCH>
	static VOID	ToggleGlobalSwitch(const BOOL& filter, BOOL bEnable);
	static const BOOL& GetSwitchByName(LPCTSTR name, BOOL* pSucceeded);//如果不成功的话，返回一个无关值。后面的参数返回是否成功，如果不关心就填NULL
	//</GLOBAL_SWITCH>
private:
	static void ReadSettings();
	static void RegisterTypes();
	static void CheckRelationships();
	static void CheckResources(LPDIRECT3DDEVICE9 pDevice);
};

struct KG3DGlobalSwitch
{
	BOOL bNewSelection;
	BOOL bSelectableRefactorFinished;
	BOOL bFloorChecker;
	KG3DGlobalSwitch();

	static const BOOL* GetSwitchByName(const KG3DGlobalSwitch& switchIn, LPCTSTR strName);
};

extern const KG3DGlobalSwitch g_Switch;	//不能直接改里面的配置的，必须通过ToggleGlobalSwitch。不然乱改的就没法跟踪了

#endif //_INCLUDE_KG3DCOLLOCATOR_H_
