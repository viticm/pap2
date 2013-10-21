/*****************************************************************************************
//	界面--新闻消息窗口
//	Copyright : Kingsoft 2003
//	Author	:   Fyt(Fan Zhanpeng)
//	CreateTime:	2003-08-01
------------------------------------------------------------------------------------------
*****************************************************************************************/
#ifndef _UINEWSMESSAGE_H
#define _UINEWSMESSAGE_H    1

#include "KEngine.h"
#include "KIniFile.h"
#include "../Elem/WndText.h"
#include "../Elem/WndShadow.h"
#include "../../../Core/Src/GameDataDef.h"

struct KNewsMessageNode : public KNewsMessage
{
	unsigned int		uTime;			//一般消息-此数固定为0；倒计数消息-表示倒计数的毫秒数
										//定时消息，定时时间限，(time()的时间量)
	union
	{
		unsigned int	uShowTimes;		// 此消息已经显示的次数（用于一般消息）
		unsigned int	uStartTime;		// 到计时开始的时间（用于倒计时消息）
		unsigned int	uLastShowTime;	// 最后一次显示的时间（用于定时消息）
	};
	KNewsMessageNode*	pNext;
};

class KUiNewsMessage : protected KWndShadow
{
public:
	//----界面面板统一的接口函数----
	static KUiNewsMessage*	OpenWindow();						// 打开窗口，返回唯一的一个类对象实例
	static KUiNewsMessage*  GetIfVisible();                     //如果窗口正被显示，则返回实例指针
	static void				CloseWindow(BOOL bDestory = FALSE);	// 关闭窗口
	static void				LoadScheme(const char* pszScheme);	// 载入界面方案
	static void				MessageArrival(KNewsMessage* pMsg, SYSTEMTIME* pTime);
	static void				EnableIdleMsg(BOOL bEnable);

private:
    static KUiNewsMessage *m_pSelf;
	static BOOL				ms_bEnableIdleMsg;
private:
	KUiNewsMessage();
	~KUiNewsMessage() {}
	void	Initialize();
	void	LoadScheme(KIniFile* pIni);			// 载入界面方案
	virtual void	Breathe();					// 我要活动，处理消息队列-____-|||b阿
	virtual int		PtInWindow(int x, int y);	// 穿透！！！
	virtual void	PaintWindow();

	bool	AddMessage(KNewsMessage* pMsg, unsigned int uTime, unsigned int uTimeParam = 0);
												// 在消息队列-__-||b的最前面增加一条消息-__-||b
	KNewsMessageNode* 	SeparateMsg();			// 把m_pHandling所指的消息从链表分离出来！！！！！
	void	AddToTail(KNewsMessageNode* pNode);	// 把pNode所指的消息加到链表末端！
	bool	PickAMessage();						// 在队列中，寻找是否有符合显示条件的消息，并且负责删除过时消息
	void	Reset();							// 重置消息的显示状态
	bool	ScrollMsg();						// 滚动当前消息
	void	PickFromIni();						// 在Ini文件中随机抽取一条消息插入队列
	void	Clear();							// 清除所有的消息
	void	ConvertMsg();						// 转换消息到最终显示内容
	bool	MakeCountingMsg();					// 生成倒计时消息的显示内容

private:

	KNewsMessageNode*	m_pHead;			// 消息链表中的头一个消息
	KNewsMessageNode*	m_pHandling;		// 正在显示/处理的新闻消息

	KIniFile		m_IniFile;				// 纪录Ini文件中要抽出来显示的一些消息

	int				m_nIndentH;				// 前缀和显示消息部份分隔多少
	int				m_nIndentV;				// 文字相对于窗口顶部的向下缩近（单位：像素点）
	int				m_nFontSize;			// 字体大小
	unsigned int	m_uTextColor;			// 前景字符的颜色
	unsigned int	m_uTextBorderColor;		// 文字边缘颜色
	int				m_nVisionWidth;			// 显示消息的空间的像素宽度
	int				m_nCharasVisibleLimitNum;// 最多可的字符的限制
	int				m_nFontHalfWidth[2];	// 字符的两个一半宽度

	unsigned int	m_uMaxIdleTime;			// 最长无消息空闲时间
	unsigned int	m_uShowInterval;		// 同一个消息两次显示之间的间隔
	unsigned int	m_uScrollInterval;		// 字符滚动的时间间隔

	unsigned int	m_uLastShowTime;		// 上一次显示消息的时候
	KNewsMessage	m_CurrentMsg;			// 当前显示的消息内容
	int				m_nLineLen;				// 当前显示消息的总显示字符数长度

	unsigned int	m_uLastScrollTime;		// 上次滚动的时间
	bool	m_bJustIncoming;				// 消息正在进入,(头)还未开始离开显示区域
	int		m_nCharIndex;					// 消息字符的滚动下标
	int		m_nTextPosX;					// 消息显示区域的左上角X坐标(相对)
	int		m_nHalfIndex;					// 字符半宽的索引
	int		m_nInsertPlace;					// 只对倒计数消息有效
};

#endif
