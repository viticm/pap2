/************************************************************************/
/* 客户端主循环				                                            */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.01.27	Create												*/
/************************************************************************/
#ifndef _KSO3CLIENT_H_
#define _KSO3CLIENT_H_

#include "./KGWin32App.h"
#include "./KPlayerClient.h"
#include "./KDllHolder.h"
#include "./KJX3Sdoa.h"
#include "./KJX33DDeviceCallback.h"
#include "SO3World/KSO3World.h"
#include "SO3UI/KJX3UIDefine.h"
#include "IKG3DSoundShell.h"

enum { JX3_MAX_USER = 4 };

struct IPluginShell;
class IKSO3RepresentInterface;
class IKSO3RepresentHandler;
class IKG3DEngineManager;
class ISO3UI;

struct KSO3ClientFilePath
{
    char szExePath[MAX_PATH];

    char szKG3DEngine[MAX_PATH];
    char szKG3DSound[MAX_PATH];
    char szKGUI[MAX_PATH];
    char szJX3UI[MAX_PATH];
    char szJX3Represent[MAX_PATH];
    char szJX3Interaction[MAX_PATH];
    char szKLVideo[MAX_PATH];
    char szIceClientlib[MAX_PATH];
    char szSdoaGameWidget[MAX_PATH];

    char szPackageList[MAX_PATH];
    char szConfig[MAX_PATH];
    char szVersion[MAX_PATH];
};

class KSO3Client : public KGWin32App, public IKG3DSoundShellHolder
{
public:
	KSO3Client();
	~KSO3Client();

	BOOL GameExit();	//退出客户端

	virtual BOOL InitSoundShell();
	virtual void UnInitSoundShell();
	virtual IKG3DSoundShell *GetSoundShell();

protected:
    virtual int OnInit();
    virtual void OnExit();
	virtual int	OnLoop();
    virtual void OnPaint();
	virtual LRESULT	OnMsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void UpdateSoundShell();

private:
	void MemoryLeakDetect();

    int InitSdoa();
    void UnInitSdoa();

	int Init3DEngine();
	void UnInit3DEngine();

	int InitRepresent();
	void UnInitRepresent();

	int InitUI();
	void UnInitUI();

    void InitPackage(BOOL bPakFirst);
    void UnInitPackage();

    int InitLua();

	int Reset();

    int UpdateUI(DWORD dwTimeNow);
    void Paint();

public:
    KJX3Sdoa m_JX3Sdoa;

	IKG3DEngineManager* m_p3DEngine;
	IKSO3RepresentInterface* m_pRepresent;
	ISO3UI* m_pUI;
    KSO3World* m_pSO3World;
	IKG3DSoundShell* m_pSoundShell;

    KJX3Config m_Config;
    KJX3Version m_Version;

private:
	DWORD				m_dwStartTime;			//启动时间

	long long			m_nLastBreatheTime;		//上次界面Breathe时间 *GAME_FPS

    DWORD               m_dwRenderTime;
	
	DWORD				m_dwPrevFrameRelTime;

	KDllHolder			m_Module3DEngine;
	KDllHolder			m_ModuleRepresent;
	KDllHolder			m_ModuleUI;
	KDllHolder          m_ModuleSound;
	KDllHolder			m_ModuleSdoaGameWidget;

    BOOL m_bHasContentToPresent;

    KSO3ClientFilePath m_FilePath;
    char m_szExePath[MAX_PATH];

    KJX33DDeviceCallback m_JX33DDeviceCallback;
};

#endif	//_KSO3CLIENT_H_
