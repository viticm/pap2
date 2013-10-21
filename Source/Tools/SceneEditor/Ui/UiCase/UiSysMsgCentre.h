/*****************************************************************************************
//	界面--系统消息中心
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-4-15
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once
#include "../Elem/WndImage.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndText.h"
#include "../../../core/src/gamedatadef.h"

#define	MAX_SYS_MSG_TYPE			6
#define	POPUPING_MSG_HEAP_INDEX		MAX_SYS_MSG_TYPE
#define	RECYCLE_MSG_HEAP_INDEX		POPUPING_MSG_HEAP_INDEX + 1

class KUiSysMsgCentre : protected KWndWindow
{
public:
	//----界面面板统一的接口函数----
	static KUiSysMsgCentre*	OpenWindow();		//打开窗口，返回唯一的一个类对象实例
	static void				CloseWindow();		//关闭窗口，同时可以选则是否删除对象实例
	static void				LoadScheme(const char* pScheme);//载入界面方案	

	static bool				AMessageArrival(KSystemMessage* pMsg, void* pParam);

private:
	KUiSysMsgCentre();
	~KUiSysMsgCentre();
	void	Initialize();
	void	Clear();
	void	LoadScheme(KIniFile* pIni);			//载入界面方案
	KSystemMessage*	GetAMsgSpace(int nParamSize);
	bool	AddAMsgToHeap(KSystemMessage*pMsg, int nHeapIndex, bool bSort);
	void	DeleteMsgInHeap(int nHeapIndex, int nMsgIndex);
	void	ConfirmMsg(int nHeapIndex, int nMsgIndex, bool bImmedDel);
	void	MovePopupedMsgToHeap();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数

	void	OnConfirmOperFinished(unsigned int uParam, int nSelAction);	//响应操作结束，玩家已经做出选择。
	void	SetPopupMsgDest();
	void	PaintWindow();							//绘制窗口
	int		PtInWindow(int x, int y);				//判断一个点是否在窗口范围内,传入的是绝对坐标
	void	Breathe();
	bool	FilterSameMsg(KSystemMessage* pMsg, void* pParam);
	int     FindTopmostPRI();                       //寻找最高优先级消息所在的堆

private:
	static KUiSysMsgCentre* m_pSelf;
private:
	struct	SYS_MSG_HEAP
	{
		KSystemMessage**	pMsgList;						//消息指针的列表
		int					nListSpace;						//消息指针列表的大小（包含多少个指针空间）
		int					nNumValid;						//列表中有效的消息的数目
	}						m_MsgHeap[MAX_SYS_MSG_TYPE + 2];

	KUiMsgParam				m_SysMsgParam;					//传给聊天信息窗口的参数

	int						m_nPopupMsgDestX;				//弹出消息的终点位置横向坐标
	int						m_nPopupMsgX;					//弹出消息的当前位置横向坐标
	int						m_nPopupMsgY;					//弹出消息的当前点位置纵向坐标
	int						m_nPopupMsgDestIndex;			//弹出消息的终点位置在哪个图标按钮上
	unsigned int			m_uLastMovementTime;
	unsigned int			m_uMoveInterval;
	unsigned int			m_uDisappearInterval;

	KWndText32				m_MsgTextWnd;
	int						m_bShowMsgText;

	KWndButton				m_MsgIconBtn[MAX_SYS_MSG_TYPE];

	KUiImageRef				m_MsgIcon[MAX_SYS_MSG_TYPE];	//消息图标的图形
	short					m_nMsgIconFrame[MAX_SYS_MSG_TYPE];//消息图标的图形帧索引

	KSystemMessage*			m_pHandlingMsg;
	BOOL                    m_bIsConfirmShowing;            //是否正在打开确认框
	int                     m_nShowingHeap;                 //最近点击的Heap
};