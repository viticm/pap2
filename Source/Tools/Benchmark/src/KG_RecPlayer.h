////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_RecPlayer.h
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2008-8-11 15:47:03
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG_RECPLAYER_H_
#define _INCLUDE_KG_RECPLAYER_H_
#include "KG3DMessenger.h"
#include "IEKG3DRecPlayer.h"
class KG_LoadingWindow;
////////////////////////////////////////////////////////////////////////////////
class IKG_RecPlayerNotify
{
public:
    virtual int OnPlayBegin(IKG3DEngineManager *piEngineManager, const char cszRecFileName[]) = 0;
    virtual int OnPlayEnd(IKG3DEngineManager *piEngineManager) = 0;
    virtual int OnFrameMove(IKG3DEngineManager *piEngineManager) = 0;
};
////////////////////////////////////////////////////////////////////////////////
struct IERecPlayer;

class KG_RecPlayer : public KG3DMessageReceiver
{
public:
    KG_RecPlayer(void); 
    ~KG_RecPlayer(void);

    int Init(unsigned uX,  unsigned uY,  unsigned uWidth,  unsigned uHeight, int nFullScreen);
    int UnInit();
    
    int Play(const char cszRecFileName[], IKG_RecPlayerNotify *piNotify);
    
    HRESULT GetEngineOption(KG3DEngineOption* pEngineOption);
    HRESULT SetEngineOption(const KG3DEngineOption* pEngineOption);
    int SetResolution(unsigned uWidth, unsigned uHeight);
    HWND GetWindow() const { return m_hWnd; };
	const IERecPlayerInfo& GetInfo() const;


private:
    int _ThreadFunction();
    static void _WorkThreadFunction(void *pvParam); 

    int _RegisterClass(HINSTANCE hInstance);//创建WndClass
    int _InitEngine();
    int _UnInitEngine();

	HRESULT GetNewIERecPlayer();

    static LRESULT CALLBACK _WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT _MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    int _ShowState();
    VOID _ShowNotice();
	VOID _ShowLoadPercent(LPCTSTR strLoadContent, FLOAT fLoadPercent);
    int _ShowStatue_WaitForExit(HDC hDC, unsigned uClientWidth, unsigned uClientHeight);

	//Messenger
	virtual void __stdcall OnReceiveMessage(const KG3DMessage& Message , IKG3DMessageSenderBase* pSender); 
private:
    HWND m_hWnd;	//主窗口
	//HFONT	m_hFont;	//默认绘制字体
	//HBRUSH	m_hBrush;
    RECT m_WndInitRect;
    int  m_nFullScreen;
    HMODULE             m_hEngineDLL;
    IKG3DEngineManager *m_piEngineManager;
    IERecPlayer         *m_piRecPlayer;// 原来这里是IRecPlayer的，但是I系列接口要照顾几个分支的编译，不适合这个应用型工程，用IE接口比较合适

    KThread m_WorkThread;
    int     m_nExitFlag;
    enum STATE
    {
        STATE_NOT_INIT,	//初始化
        STATE_READY,	//Init完毕
        STATE_LOADING,	//LoadRecFile
        STATE_PLAYING,	//开始播放，FrameMove
		STATE_SHOW_RESULT,
        STATE_WAIT_FOR_EXIT,	//开始Uninit
        STATE_EXIT,	//Uninit完毕
    } m_dwState;

    static KG_RecPlayer* ms_pSelf;

	/*struct DrawStateElem 
	{
		HDC hDC;
		HFONT hOldFont;
		RECT clientRect;
		HWND hWnd;

		DrawStateElem(){ZeroMemory(this, sizeof(*this));}
		~DrawStateElem()
		{
			if (NULL != hOldFont)
			{
				::SelectObject(hDC, (HGDIOBJ)hOldFont);
				hOldFont = NULL;
			}
			if (hDC)
			{
				::ReleaseDC (hWnd, hDC);
				hDC = NULL;
			}
		}
	};
	VOID _PrepareDrawState(DrawStateElem& drawElem);*/

private:
	HBITMAP m_hBg;
	HBITMAP m_hProgress;
	//HBITMAP m_hSprite;

	int m_nBgWidth;
	int m_nBgHeight;

	int m_nProgressWidth;
	int m_nProgressHeight;

	//int m_nSpriteWidth;
	//int m_nSpriteHeight;

	int m_nProgressX;
	int m_nProgressY;
	
	std::string m_strfont;
	std::string m_strfontFPS;
	int m_nFontSize;
	int m_nFontFPSSize;
	int m_nFontPosX;
	int m_nFontPosY;
	float m_fCurrentPercentage;
	
	HINSTANCE m_hInstance; 
	
	BLENDFUNCTION m_bldFunc;

	//int m_nRestorX;
	//int m_nRestorY;

	BOOL LoadSource(int nWidth,int nHeight);
	void UnloadSource();
	void PaintLoading(BOOL bProgress);
	HRESULT PaintResult();
	void _OnPaint();
//public:
//	std::string m_strCPU;
//	std::string m_strDisplayCrad;
//	DWORD m_dwAverageFPS;
//	DWORD m_dwMaxFPS;
//	DWORD m_dwMinFPS;
//	BOOL  m_bUseGpu;
};

inline LONG RECT_GetWidth(const RECT& rect){return rect.right - rect.left;}
inline LONG RECT_GetHeight(const RECT& rect){return rect.bottom - rect.top;}


#endif //_INCLUDE_KG_RECPLAYER_H_
