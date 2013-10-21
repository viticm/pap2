#ifndef SO3REPRESENT_H
#define SO3REPRESENT_H

#include "./krloption.h"
#include "./krloptimization.h"
#include "./krlresourcelimit.h"
#include "./krlprofile.h"
#include "./krltracecharacter.h"
#include "./base/file/kfilepath.h"
#include "./case/actionobject/krlscenemgr.h"
#include "./case/gameworld/ktablelist.h"
#include "./case/gameworld/kgameworldhandler.h"

class SO3Represent : public IKSO3RepresentInterface
{
public:
	SO3Represent();
	~SO3Represent();

	virtual BOOL Init();
	virtual void Release();

	virtual IKSO3GameWorldRepresentHandler* GetGameWorldRepresentHandler();
	virtual IKSO3RepresentHandler* GetRepresentHandelr();

	virtual void SetSoundShell(IKG3DSoundShell *pSoundShell);

private:
	void Exit();

public:
	double m_fTime;
    double m_fTimeLast;

    double m_fFrameTime;
    double m_fFrameTimeLast;
    double m_fFrameTimeStart;

    DWORD m_dwGameLoop;
    int m_nStartLoop;

    KFilePath m_FilePath;
    KTableList m_TableList;
    KGameWorldHandler m_GameWorldHandler;

    KRLOption m_Option;

    KRLTraceOption m_TraceOption;
    KRLTraceOption m_CurrentTraceOption;

    KRLDebugOption m_DebugOption;
    KRLDebugOption m_CurrentDebugOption;

    KRLOptimizationOption m_OptimizationOption;
    KRLOptimizationOption m_CurrentOptimizationOption;

    KRLTerrainLeanOption m_TerrainLeanOption;
    KRLTerrainLeanOption m_CurrentTerrainLeanOption;

    KRLSceneMgr m_SceneMgr;

    KRLOptimization m_Optimization;
    KRLResourceLimit m_ResourceLimit;
    KRLProfile m_Profile;
    KRLTraceCharacter m_TraceCharacter;

	IKG3DEngineManager* m_p3DEngineManager;
	IKG3DResourceManager* m_p3DModelManager;
	IKG3DUI* m_p3DUI;
	IKG3DSoundShell* m_pSound;

    ISO3UI* m_pSO3UI;
    IKSO3GameWorldUIHandler* m_pGameWorldUIHandler;

	KSO3World* m_pSO3World;
    IKGSO3WorldClient* m_pSO3WorldClient;
    
	HWND m_hWnd;

    BOOL m_bRunning;
    BOOL m_bActivated;

	static HANDLE ms_hInstance;
};

extern SO3Represent* g_pRL;

#endif // SO3REPRESENT_H