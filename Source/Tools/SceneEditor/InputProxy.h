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
	WORD wIgnoreState;				//忽略比较的状态，0不忽略，1忽略shift,ctrl,alt比较，2忽略lmr鼠标键比较, 3全部忽略，只比较MainKey,如:2时,不管鼠标键有没有按下去,只要MAINKEY按下了,以及键盘相关的键位正确,就算本ACTION被触发
	BOOL bExclusive;				//是否独占，独占时，忽略判断其他的组合键位, 为1时,其后定义的其他ACTION都不再被判断执行
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

	//屏蔽未实现的拷贝，这个类不能拷贝
	CInputProxy& operator=(const CInputProxy& rhs);
	CInputProxy(const CInputProxy& src);

	BOOL m_bInCapture;	//用来给外部检测是否在捕捉鼠标消息，只有当窗口被激活的时候，才会被SetCapture
	INT m_nMouseX, m_nMouseY;
};
