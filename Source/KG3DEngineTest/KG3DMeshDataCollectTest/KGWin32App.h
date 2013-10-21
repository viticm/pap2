#ifndef KG_WIN32_APP_H
#define KG_WIN32_APP_H

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
    static void Enable(BOOL bEnable);

protected:
    virtual int OnInit() = 0;
    virtual void OnExit() = 0;
	virtual int	OnLoop() = 0;
    virtual void OnTest() = 0;
	virtual LRESULT	OnMsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

private:
	int	InitClass();
	int	InitWindow();

	void ExitClass();
    void Exit();

	void SetTimerFrame(int nFrame);
    LRESULT	MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	char m_szClass[128];
	char m_szTitle[128];

	DWORD m_dwHoverTime;

    int m_nIcon;
	HWND m_hWnd;
    HINSTANCE m_hInstance;

    HANDLE m_Events[EVENT_TOTAL];

    BOOL m_bEnabled;
    BOOL m_bLoop;

    static KGWin32App* ms_pSelf;
};


#endif // KG_WIN32_APP_H

