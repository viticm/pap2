#pragma once
#include "KGPublic.h"
#include "SO3UI/ipluginshell.h"
#include "UiWndData.h"
#include "UiWndWindow.h"
#include "Ui.h"
#include "UiShowWndTree.h"


class KUiEditor : public IPluginShell
{
public:
	KUiEditor();

	//IPluginShell定义的接口函数
	virtual int	 Create(HWND hWnd, void* pReserved);
	virtual int	 Init();
	virtual void Breathe();
	virtual void Release();
	virtual LRESULT Operation(UINT uOper, WPARAM wParam, LPARAM lParam);
	virtual LRESULT EventNotify(UINT uEvent, WPARAM wParam, LPARAM lParam);
	virtual void SetCallback(PLUGIN_CALLBACK_FUNC eFuncIndex, FnPluginBehaviorCallback pFunc);

private:
	bool	Create();
	void	Destroy();
	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	void	OnButtonClick(int nBtnId, HWND hWnd);
	void	OnUiOpenIni();
	void	OnUiSel();
	void	OnUiSave();
    void    OnUiSaveAs();
	void	OnClear();
    void	OnTreeNotify(LPNMHDR pNotify, LPARAM lParam);
    void    CreateNewWndByType(IN UI_WND_TYPE eType, IN const char *pPrefixTex, OUT char *pNewWndName, IN const char *pParent = NULL);
	void	OnWndDel();
    void    OnMoveWndUp();
    void    OnMoveWndDown();
	void	OnWndNameChange();
	void	OnUpdateEditText(int nId);
    void    OnChangeParent(KSHOWWNDNODE *pNode);

	void	UpdateAll(const char* pszDetailWnd, bool bForceReCreate, bool bUpdataInClient);
	void	ClearDetailInfo();
	void	FillDetailInfo(const char* pszWndUnit, bool bForceReCreate, bool bUpdataInClient);
	int		CreateDetailInfoWnd(UI_WND_TYPE eType, char * pszName, bool bForceReCreate = false);
    void    InitWindowShow(RECT *pRc, const char *pszName);
    int     CreatShowTreeWnd();
	void	GetSettingFileName(LPTSTR szPath);
	void	LoadSetting();
	void	SaveSetting();
    void    SetDefaultData(const char * pszUnitName, const UI_WND_TYPE cnType);

    //tab相关。
    void    OnTabNotify(LPNMHDR pNotify);
    void    ReBulidTab();
    void    ClearTab();

    void    OnTreeItemRightButtonClick();
    void    OnFrameRightButtonClick(KSHOWWNDNODE *pNode);
    void    OnWndRightButtonClick(KSHOWWNDNODE *pNode);
    void    OnHandleRightButtonClick(KSHOWWNDNODE *pNode);
    void    OnItemRightButtonClick(KSHOWWNDNODE *pNode);
    void    OnNewFrame();
    void    ChangeParent(const char *pszMe, const char *pszNewParent);
    void    CreateWnd(IN UI_WND_TYPE eType, IN const char *pszParent = NULL, OUT char *pszCreatedWnd = NULL);

private:
	bool			m_bEnable;
	HWND			m_hWnd;
    HWND            m_hTreeWnd;
	KUiWndWindow*	m_pDetailInfoWnd;

	char			m_szCurrentWndUnit[128];
	int				m_nNewWndCounter;
	SIZE			m_sizeWindow;
	bool			m_bLarge;
	bool			m_bShowMainUI;
    IIniFile*       m_pSettingIniFile;
    KUiShowWndTree  m_ShowWndTree;

    char            m_szCurrentFile[MAX_PATH];
    char            m_szCurrentSelWndName[128];


    KUIWNDPAGEINFOMATION m_PageInfomation;

    HWND m_hWndTab;

public:
    static COLORREF		CustClr[16]; 

    struct KUIWNDERRORINFOMATION  
    {
        char szErrorMsg[256];
        BOOL bHasError;
    };

	struct KSwordPlusSetting
	{
		char		m_szLastUiIniPath[MAX_PATH];
	};

    static KSwordPlusSetting m_Settings;
    static KUIWNDERRORINFOMATION ms_ErrorInfomation;

public:
    static int NormalizeIniData(IIniFile *pIni);
    static int NormalizeIniKey(IIniFile *pIni, const char *pszSectionName, const char *pszKeyName);
    static int CopyWndAndChildWnd(IIniFile *pIn, LPCTSTR pszWndName, IIniFile *pOut, OUT LPTSTR pszNewWndName, LPCSTR pszParent);
    static int CopyAllChildWnd(IIniFile *pIn, LPCTSTR pszWndName, IIniFile *pOut, OUT LPTSTR pszNewWndName, LPCSTR pszParent);
    static int FindNextChildWnd(IIniFile *pIn, LPCTSTR pszParent,LPCTSTR pszChild, LPTSTR pszNextChild);
    static int GetFrameWndName(IIniFile *pIni, OUT LPTSTR pszFrameName);
};


//游戏程序句柄
extern	HANDLE					g_hPluginInstance;
//游戏窗口
extern	HWND					g_hSwordWnd;

extern FnPluginBehaviorCallback	g_pfnPluginBehavior;
extern FnPluginBehaviorCallback	g_pfnUIBehavior;

#ifndef PLUGIN_INTERNAL_SIGNATURE

extern "C"
namespace SwordPluginUiEditor
{
	//----接口函数的导出----
	IPluginShell* CreatePlugin();
};


#endif