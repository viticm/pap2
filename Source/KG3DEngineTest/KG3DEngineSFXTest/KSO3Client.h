#ifndef _KSO3CLIENT_H_
#define _KSO3CLIENT_H_

#include "./KGWin32App.h"
#include "./KDllHolder.h"

class IKG3DEngineManager;

enum TYPE_EXT
{
    EXT_MESH = 0,
    EXT_MDL,
    EXT_TANI,
    EXT_SFX,
    EXT_MTL,
    EXT_SPEEDTREE,
    EXT_T0TAL
};

const char g_cszExtName[EXT_T0TAL][32] ={
    ".mesh",
    ".mdl",
    ".tani",
    ".sfx",
    ".mtl",
    ".stree"
};

//////////////////////////////////////////////////////////////////////////
class KSO3Client : public KGWin32App
{
public:
	KSO3Client();
	~KSO3Client();
	
protected:
    virtual int OnInit();
    virtual void OnExit();
	virtual int	OnLoop();
    virtual void OnPaint();
	virtual LRESULT	OnMsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);


private:
    int InitConfig(BOOL *pbAllocConsole, BOOL *pbPakFirst);
    int UnInitConfig();

	BOOL Init3DEngine();
	void UnInit3DEngine();

    void InitPackage(BOOL bPakFirst);
    void UnInitPackage();

    int PushValidFile(const char *pcszFileName, TYPE_EXT ExtType);
    int GetFileList(const char *pcszDirPath, TYPE_EXT ExtType);

    HRESULT SfxFrameMoveAndRender(LPCTSTR pcszFileName);
    HRESULT GetVideoMemeryInfo();
    void TryRender();
    void GetFilterFileList();
    void MemoryLeakDetect();

	KDllHolder			m_Module3DEngine;
    KDllHolder          m_ModuleIPPCodec;

	IKG3DEngineManager  *m_p3DEngine;
    IKG3DScene          *m_pScene;
    int                 m_nOutputWindowID;
    vector<string>      m_vecSFXList; 
    set<string>         m_setFilterFileList;
    IIniFile            *m_pFile;
    BOOL                m_bUseLogicalSave;
    //IKG3DModel          *m_piSFX;
    
};

#endif	//_KSO3CLIENT_H_
