#pragma once

interface IEKG3DSceneEditorBase;

#define WM_ACTIONEXECUTED WM_USER + 2000
typedef struct tagKeyStateSnap
{
	unsigned MainKey : 8;		//主要键名，可以是所有的数字字母，以及CTRL等控制键
	unsigned CtrlState:1;		//是否需要按下 ctrl 键才生效，1 表示需要按下
	unsigned ShiftState:1;
	unsigned AltState:1;
	unsigned LBTState:1;		//鼠标左键
	unsigned MBTState:1;		//鼠标中键	
	unsigned RBTState:1;		//鼠标右键
	unsigned Reverse:2;			//保留位
}KeyStateSnap;

struct PROXYKEYMAP
{
	KeyStateSnap keystate;
	DWORD TriggerMsgID;				//触发的消息Id, 如，当WM_MOUSEMOVE时，需要判断旋转镜头的键是否按下
	WORD wActionID;					//ActionID
	BOOL bPrevState;				//记录最近一次是否已经按下些组合键
	WORD wIgnoreState;				//忽略比较的状态，0不忽略，1忽略shift,ctrl,alt比较，2忽略lmr鼠标键比较, 3全部忽略，只比较MainKey
	BOOL bExclusive;				//是否独占，独占时，忽略判断其他的组合键位
};

interface IEKG3DSceneSceneEditor;
class CInputProxy
{
public:
	CInputProxy();
	virtual ~CInputProxy(void);

public:
	void Init(IEKG3DSceneEditorBase* pSceneEditor, CWnd* pWnd, PROXYKEYMAP* pKeyMapArray);
	INT CheckInputEvent(MSG* pMsg);
	BOOL IsCaptured();
private:
	IEKG3DSceneEditorBase* m_pSceneEditor;
	CWnd* m_pWnd;
	PROXYKEYMAP* m_pKeyMapArray;

	static WORD m_wIgnoreKeyBoardMask, m_wIgnoreMouseMask, m_wIgnoreAllMask;
	static BOOL m_bIgnoreInit;

	//屏蔽未实现的动作
	CInputProxy& operator=(const CInputProxy& rhs);
	CInputProxy(const CInputProxy& src);

	BOOL m_bInCapture;
	INT m_nMouseX, m_nMouseY;
};
