#ifndef KG_WIN32_APP_H
#define KG_WIN32_APP_H

struct KSO3ClientFilePath
{
    char szKG3DEngine[MAX_PATH];
    char szKG3DSound[MAX_PATH];
    char szKGUI[MAX_PATH];
    char szJX3UI[MAX_PATH];
    char szJX3Represent[MAX_PATH];
    char szJX3Interaction[MAX_PATH];
    char szKLVideo[MAX_PATH];
    char szIceClientlib[MAX_PATH];

    char szPackageList[MAX_PATH];
    char szConfig[MAX_PATH];
    char szVersion[MAX_PATH];
};

struct KSO3ClientConfig
{
    BOOL bConsole;
    BOOL bPakFirst;
};

class KGWin32App
{
    friend LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);;

private:
    enum 
    {
        EVENT_TIMER,
        EVENT_RENDER,
        EVENT_TOTAL,
    };

public:
	KGWin32App();
    virtual ~KGWin32App();

	int	Init(HINSTANCE hInstance, LPCSTR szAppName, int nIcon);
	void Run();
    void SetMouseHoverTime(DWORD dwHoverTime);
    const char * GetTitle();
    int GetIcon();

	static HWND	GetHWND();
    static HINSTANCE GetHINSTANCE();
    static KSO3ClientFilePath* GetFilePath(); 
    static void Enable(BOOL bEnable);

protected:
    virtual int OnInit() = 0;
    virtual void OnExit() = 0;
	virtual int	OnLoop() = 0;
    virtual void OnPaint() = 0;
	virtual LRESULT	OnMsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

private:
	int	InitClass();
	int	InitWindow();
    int PreparePath();

	void ExitClass();
    void Exit();

	void SetTimerFrame(int nFrame);
    LRESULT	MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	char m_szClass[128];
	char m_szTitle[128];
    char m_szExePath[MAX_PATH];

	DWORD m_dwHoverTime;

    int m_nIcon;
	HWND m_hWnd;
    HINSTANCE m_hInstance;

    HANDLE m_Events[EVENT_TOTAL];

    BOOL m_bEnabled;
    BOOL m_bLoop;

    KSO3ClientFilePath m_FilePath;
    KSO3ClientConfig m_Config;
    static KGWin32App* ms_pSelf;
};


#endif // KG_WIN32_APP_H

