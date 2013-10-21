#ifndef _K_LOADING_WINDOW_H_
#define _K_LOADING_WINDOW_H_

class KLoadingWindow
{
public:
	KLoadingWindow();
	virtual ~KLoadingWindow();

	BOOL Init(HINSTANCE hInstance, LPCSTR szAppName, int nIcon);
	void Exit();

	void StartLoading(DWORD dwTime);
	void SetPercentage(float fPercentage, float fNextPercentage);
	BOOL IsFinished();
	void EndLoading();
    void HideWindow();

	BOOL IsCanceled();
    HWND GetHWND() const;

private:
	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static	void ThreadFunction(void* lpParam);

	void ThreadFunction();
	void Calculate();

	void Paint(BOOL bRepaint);
	BOOL LoadSource();
	void UnloadSource();

	void SetCanceled(BOOL bCanceled);

private:
	KThread m_Thread;
	HWND m_hWnd;
	float m_fCurrentPercentage;
	float m_fPercentage;
	float m_fNextPercentage;
	DWORD m_dwStartTime;
	DWORD m_dwTimeLast;
	DWORD m_dwTime;
	BOOL m_bLoading;
	BOOL m_bFinished;
	BOOL m_bCanceled;

	HINSTANCE m_hInstance;
	char m_szClass[128];
	char m_szTitle[128];
	int m_nIcon;

	HBITMAP m_hBg;
	HBITMAP m_hProgress;
	HBITMAP m_hSprite;

	int m_nBgWidth;
	int m_nBgHeight;

	int m_nProgressWidth;
	int m_nProgressHeight;

	int m_nSpriteWidth;
	int m_nSpriteHeight;

	int m_nProgressX;
	int m_nProgressY;

	BLENDFUNCTION m_bldFunc;

	int m_nRestorX;
	int m_nRestorY;
};



#endif