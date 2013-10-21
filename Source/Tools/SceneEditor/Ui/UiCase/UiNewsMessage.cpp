/*********************************************************************************
*文件    ：UiNewsMessage
*创建人  ：Fyt(Fan Zhanpeng)
*创建时间：08-01-2003
*功能描述：游戏中的新消息通知窗口
**********************************************************************************/
#include "KWin32.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "UiNewsMessage.h"
#include "../Elem/Wnds.h"
#include "../Elem/UiImage.h"
#include "../UiBase.h"
#include "../../../Engine/Src/Text.h"
#include "UiNewsMessage.h"
#include <time.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define NEWS_MESSAGE_INI "新闻消息来了.ini"
#define DEFAULT_MESSAGE  "\\Ui\\DefaultMessage.ini"
//最长无消息空闲时间，空闲时间持续超过此值时，就从ini中取消息来显示(单位：毫秒)
#define	MAX_IDLE_TIME	600000	//10分钟
//同一消息两次显示之间的间隔(单位：毫秒)
#define SHOW_INTERVAL 30000
//两次字符滚动之间的时间(单位：毫秒)
#define SCROLL_INTERVAL 250
//一般消息最多滚动出现的次数
#define	MAX_NORMAL_SHOW_TIMES	3

#define NOT_NEED_INSERT	-1

extern iRepresentShell *g_pRepresentShell;
KUiNewsMessage* KUiNewsMessage::m_pSelf = NULL;
BOOL			KUiNewsMessage::ms_bEnableIdleMsg = FALSE;

/*********************************************************************************
*功能：构造
**********************************************************************************/
KUiNewsMessage::KUiNewsMessage()
{
	m_pHead    = NULL;
    m_pHandling= NULL;
	m_nCharasVisibleLimitNum = 0;
	m_nVisionWidth = 0;
	m_nFontSize = 8;
	m_nFontHalfWidth[0] = m_nFontHalfWidth[1] = 4;
	srand(IR_GetCurrentTime());
}

void KUiNewsMessage::EnableIdleMsg(BOOL bEnable)
{
	if (!bEnable != !ms_bEnableIdleMsg)
	{
		ms_bEnableIdleMsg = bEnable;
		if (m_pSelf)
		{
			if (ms_bEnableIdleMsg)
			{
				m_pSelf->m_uLastShowTime = IR_GetCurrentTime();
				m_pSelf->m_IniFile.Load(DEFAULT_MESSAGE);
			}
			else
			{
				m_pSelf->m_IniFile.Clear();
			}
		}
	}
}

/*********************************************************************************
*功能：增加一条消息在队列中
**********************************************************************************/
bool KUiNewsMessage::AddMessage(KNewsMessage* pMsg, unsigned int uTime, unsigned int uTimeParam)
{
	KNewsMessageNode *pNew = NULL;
	if (pMsg)
	{
		pNew = (KNewsMessageNode*)malloc(sizeof(KNewsMessageNode));
		if (pNew)
		{
			*(KNewsMessage*)pNew = *pMsg;
			pNew->uTime = uTime;
			pNew->uStartTime = uTimeParam;
			pNew->pNext = m_pHead;
			m_pHead = pNew;
		}
	}
	return (pNew != NULL);
}

/*********************************************************************************
*功能：把m_pHandling指针所指的消息从链表中分离出来
**********************************************************************************/
KNewsMessageNode* KUiNewsMessage::SeparateMsg()
{
	KNewsMessageNode *pNode = m_pHandling;
	if(m_pHandling)
	{
		if (m_pHandling == m_pHead)
		{
			m_pHead = m_pHead->pNext;
		}
		else
		{
			pNode = m_pHead;
			while(pNode->pNext != m_pHandling)
			{
				pNode = pNode->pNext;
				//_ASSERT(pNode);
			};
			pNode->pNext = m_pHandling->pNext;
			pNode = m_pHandling;
		}
		m_pHandling = NULL;
		pNode->pNext = NULL;
	}
	return pNode;
}

// 把pNode所指的消息加到链表末端！
void KUiNewsMessage::AddToTail(KNewsMessageNode* pNode)
{
	if (pNode)
	{
		if (m_pHead)
		{
			KNewsMessageNode *pPrev = m_pHead;
			while(pPrev->pNext)
				pPrev = pPrev->pNext;
			pPrev->pNext = pNode;
		}
		else
		{
			m_pHead = pNode;
		}
	}
}

// 清除所有的消息
void KUiNewsMessage::Clear()
{
	m_IniFile.Clear();
	while (m_pHandling = m_pHead)
	{
		m_pHead = m_pHead->pNext;
		free (m_pHandling);
	};
}

/*********************************************************************************
*功能：打开窗口
**********************************************************************************/
KUiNewsMessage* KUiNewsMessage::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiNewsMessage;
		if (m_pSelf)
		{
			m_pSelf->Initialize();
		}
	}
	if (m_pSelf)
	{
		m_pSelf->m_uLastShowTime = IR_GetCurrentTime();
		m_pSelf->Show();
		m_pSelf->BringToTop();
	}
	return m_pSelf;
}


/*********************************************************************************
*功能：如果窗口正被显示，则返回实例指针
**********************************************************************************/
KUiNewsMessage* KUiNewsMessage::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}


/*********************************************************************************
*功能：关闭窗口
**********************************************************************************/
void KUiNewsMessage::CloseWindow(BOOL bDestory)
{
	if (m_pSelf)
	{
		if(bDestory == TRUE)
		{
			m_pSelf->Clear();
			m_pSelf->Destroy();
		    m_pSelf = NULL;
		}
		else
		{
			m_pSelf->Hide();
		}
	}
}

/*********************************************************************************
*功能：初始化
**********************************************************************************/
void KUiNewsMessage::Initialize()
{
	if (ms_bEnableIdleMsg)
		m_IniFile.Load(DEFAULT_MESSAGE);

	char Scheme[128];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this, WL_TOPMOST);
}

/*********************************************************************************
*功能：载入界面方案
**********************************************************************************/
void KUiNewsMessage::LoadScheme(const char* pszScheme)
{
	if (m_pSelf && pszScheme && pszScheme[0])
	{
		char    szBuf[128];
		sprintf(szBuf, "%s\\%s", pszScheme, NEWS_MESSAGE_INI);
		KIniFile	Ini;
		if (Ini.Load(szBuf))
		{
			m_pSelf->Init(&Ini, "Main");
			Ini.GetInteger("Main", "IndentH",	0, &m_pSelf->m_nIndentH);
			Ini.GetInteger("Main", "IndentV",   0, &m_pSelf->m_nIndentV);
			Ini.GetInteger("Main", "Font",      0, &m_pSelf->m_nFontSize);
			if(m_pSelf->m_nFontSize < 8)
		    	m_pSelf->m_nFontSize = 8;
			m_pSelf->m_nFontHalfWidth[0] = (m_pSelf->m_nFontSize + 1) / 2;
			m_pSelf->m_nFontHalfWidth[1] = m_pSelf->m_nFontSize / 2;;
			m_pSelf->m_nVisionWidth = m_pSelf->m_Width - m_pSelf->m_nIndentH * 2;
			m_pSelf->m_nCharasVisibleLimitNum =
				(m_pSelf->m_nVisionWidth * 2) / m_pSelf->m_nFontSize;
			Ini.GetString ("Main", "TextColor", "0,0,0", szBuf, sizeof(szBuf));
		    m_pSelf->m_uTextColor = GetColor(szBuf);
			Ini.GetString ("Main", "TextBorderColor", "0,0,0", szBuf, sizeof(szBuf));
			m_pSelf->m_uTextBorderColor = GetColor(szBuf);
			Ini.GetInteger("Main", "MaxIdleTime",    MAX_IDLE_TIME,   (int*)&m_pSelf->m_uMaxIdleTime);
			Ini.GetInteger("Main", "ShowInterval" ,  SHOW_INTERVAL,   (int*)&m_pSelf->m_uShowInterval);
			Ini.GetInteger("Main", "ScrollInterval", SCROLL_INTERVAL, (int*)&m_pSelf->m_uScrollInterval);
		}
	}
}

/*********************************************************************************
*功能：两个字 - 穿透！！！
**********************************************************************************/
int KUiNewsMessage::PtInWindow(int x, int y)
{
	return 0;
}

//新闻消息真的来了
void KUiNewsMessage::MessageArrival(KNewsMessage* pMsg, SYSTEMTIME* pTime)
{
	unsigned int uTime;
	if (m_pSelf && pMsg &&
		pMsg->nMsgLen > 0 && pMsg->nMsgLen <= sizeof(pMsg->sMsg))
	{
		switch(pMsg->nType)
		{
		case NEWSMESSAGE_NORMAL://一般消息
			m_pSelf->AddMessage(pMsg, 0, 0);
			break;
		case NEWSMESSAGE_COUNTING://倒计（秒）数消息
			if (pTime && pTime->wSecond)
				uTime = pTime->wSecond * 1000;
			else
				uTime = 3000;
			m_pSelf->AddMessage(pMsg, uTime, IR_GetCurrentTime());
			break;
		case NEWSMESSAGE_TIMEEND: //定时消息
			if (pTime)
			{
				SYSTEMTIME Local;
				SystemTimeToTzSpecificLocalTime(NULL, pTime, &Local);
				tm		TimeBuf;
				TimeBuf.tm_isdst= 0;
				TimeBuf.tm_year = Local.wYear - 1900;
				TimeBuf.tm_mon  = Local.wMonth - 1;
				TimeBuf.tm_mday = Local.wDay;
				TimeBuf.tm_hour = Local.wHour;
				TimeBuf.tm_min  = Local.wMinute;
				TimeBuf.tm_sec  = Local.wSecond;
				TimeBuf.tm_wday = Local.wDayOfWeek;
				TimeBuf.tm_yday = 0;
				uTime = mktime(&TimeBuf);
				if(uTime == (unsigned int)(time_t)-1)
					uTime = 0;
			}
			else
				uTime = 0;
			if (uTime)
				m_pSelf->AddMessage(pMsg, uTime, 0);
			else
			{
				pMsg->nType = NEWSMESSAGE_NORMAL;
				m_pSelf->AddMessage(pMsg, 0, 0);
				pMsg->nType = NEWSMESSAGE_TIMEEND;
			}
			break;
		}
	}
}

/*********************************************************************************
*功能：显示消息，并在显示完成后对消息进行进一步处理
**********************************************************************************/
void KUiNewsMessage::PaintWindow()
{
    if(m_pHandling && g_pRepresentShell)
	{
		KWndShadow::PaintWindow();

		//画字串了  -___________-|||b
		KOutputTextParam Param;
		Param.Color       = m_uTextColor;
		Param.BorderColor = m_uTextBorderColor;
		Param.nSkipLine   = 0;
		Param.nNumLine    = 1;
		Param.nX		  = m_nTextPosX;
		Param.nY          = m_nAbsoluteTop + m_nIndentV;
		Param.nZ          = TEXT_IN_SINGLE_PLANE_COORD;
		Param.nVertAlign  = 0;
		Param.bPicPackInSingleLine = true;

		if (m_bJustIncoming)
		{	//消息正在进入,(头)还未开始离开显示区域
			g_pRepresentShell->OutputRichText(m_nFontSize, &Param,
				m_CurrentMsg.sMsg, m_nCharIndex, 0);
		}
		else
		{	//消息头已经离开显示区域
			KTP_CTRL	Ctrl0, Ctrl1;
			TGetEncodedTextEffectCtrls(m_CurrentMsg.sMsg, m_nCharIndex, Ctrl0, Ctrl1);
			char		sBuffer[sizeof(m_CurrentMsg.sMsg)];
			int nRemainEffectCtrlsSize = 0;
			if (Ctrl0.cCtrl != KTC_INVALID)
			{
				*(int*)(sBuffer) = *(int*)(&Ctrl0);
				nRemainEffectCtrlsSize += 4;
			}
			if (Ctrl1.cCtrl != KTC_INVALID)
			{
				*(int*)(&sBuffer[nRemainEffectCtrlsSize]) = *(int*)(&Ctrl1);
				nRemainEffectCtrlsSize += 4;
			}
			if (nRemainEffectCtrlsSize == 0)
			{
				g_pRepresentShell->OutputRichText(m_nFontSize, &Param,
					&m_CurrentMsg.sMsg[m_nCharIndex], m_CurrentMsg.nMsgLen - m_nCharIndex,
					m_nVisionWidth);
			}
			else
			{
				memcpy(sBuffer + nRemainEffectCtrlsSize, m_CurrentMsg.sMsg + m_nCharIndex,
					m_CurrentMsg.nMsgLen - m_nCharIndex);
				g_pRepresentShell->OutputRichText(m_nFontSize, &Param,
					sBuffer, m_CurrentMsg.nMsgLen - m_nCharIndex + nRemainEffectCtrlsSize,
					m_nVisionWidth);
			}
		}
	}
}

/*********************************************************************************
*功能：在队列中，寻找是否有符合显示条件的消息，使指针m_pHandling指向找到的消息
**********************************************************************************/
bool KUiNewsMessage::PickAMessage()
{
	m_pHandling = NULL;
	unsigned int	uCurrentTime = IR_GetCurrentTime();

	if (m_pHead == NULL)
	{
		if (ms_bEnableIdleMsg && uCurrentTime > m_uLastShowTime + m_uMaxIdleTime)
		{	//如果过了时间了，还没有新消息到，就随机在ini中选一条消息，插入队列中
			PickFromIni();
			if (m_pHandling = m_pHead)
				m_pHandling->uShowTimes++;
			//ASSERT(m_pHandling->nType == NEWSMESSAGE_NORMAL);
			m_uLastShowTime = uCurrentTime;	//对付从ini中也不能取到消息的情况
		}
	}
	else
	{
		KNewsMessageNode* pNode = m_pHead;
		while(pNode)
		{
			bool bTobeFree = false;
			if (pNode->nType == NEWSMESSAGE_NORMAL)
			{
				m_pHandling = pNode;
				if ((++pNode->uShowTimes) > MAX_NORMAL_SHOW_TIMES)
					bTobeFree = true;
			}
			else if (pNode->nType == NEWSMESSAGE_COUNTING)
			{
				m_pHandling = pNode;
				if (uCurrentTime - pNode->uStartTime >= pNode->uTime)
					bTobeFree = true;
			}
			else if (uCurrentTime > pNode->uLastShowTime + m_uShowInterval)
			{
				m_pHandling = pNode;
				if ((unsigned int)time(NULL) >= pNode->uTime)
					bTobeFree =  true;	//限时消息到时了，消灭它
				else
					pNode->uLastShowTime = uCurrentTime;
			}
			else
			{
				pNode = pNode->pNext;
				continue;
			}
			if (bTobeFree == false)
				break;
			pNode = pNode->pNext;
			free(SeparateMsg());
		};
	}
	return (m_pHandling != NULL);
}

/*********************************************************************************
*功能：在Ini文件中随机抽取一条消息插入队列
**********************************************************************************/
void KUiNewsMessage::PickFromIni()
{
	int         i, nValue;
	char        szNum[16];
	KNewsMessage	Msg;

	m_IniFile.GetInteger("Main", "Count", 0, &nValue);
	if(nValue > 0)
	{
    	i = rand() % nValue;

		m_IniFile.GetString("Main", itoa(i, szNum, 10), "", Msg.sMsg, sizeof(Msg.sMsg));
		Msg.nMsgLen = TEncodeText(Msg.sMsg, strlen(Msg.sMsg));
		if (Msg.nMsgLen > 0)
		{
			Msg.nType = NEWSMESSAGE_NORMAL;
			MessageArrival(&Msg, NULL);
		}
	}
}


/*********************************************************************************
*功能：保持活动，更新消息信息
**********************************************************************************/
void KUiNewsMessage::Breathe()
{
	if(m_pHandling)
	{	//有消息正在显示
		if (IR_IsTimePassed(m_uScrollInterval, m_uLastScrollTime))
		{
			if (ScrollMsg())
				AddToTail(SeparateMsg());	// 当前消息显示完毕,移到末尾去
		}
	}
	if (m_pHandling == NULL)
	{
		if (PickAMessage())
			Reset();
	}
}

void KUiNewsMessage::ConvertMsg()
{
	//_ASSERT(m_pHandling);
	//填充当前显示消息的内容
	m_CurrentMsg.nMsgLen = 0;

	m_nInsertPlace = NOT_NEED_INSERT;
   	if(m_pHandling->nType == NEWSMESSAGE_COUNTING)
	{
		m_nInsertPlace = TFindSpecialCtrlInEncodedText(m_pHandling->sMsg,
			m_pHandling->nMsgLen, 0, '%');

		if (m_nInsertPlace < 0 &&
			m_nInsertPlace > m_pHandling->nMsgLen - 2)
		{
			m_nInsertPlace = NOT_NEED_INSERT;
		}
		else if (m_pHandling->sMsg[m_nInsertPlace + 1] != 'd')
		{
			m_nInsertPlace = NOT_NEED_INSERT;
		}
	}

	if (m_nInsertPlace == NOT_NEED_INSERT)
	{
		memcpy(m_CurrentMsg.sMsg, m_pHandling->sMsg, m_pHandling->nMsgLen);
		m_CurrentMsg.nMsgLen = m_pHandling->nMsgLen;
	}
}

bool KUiNewsMessage::MakeCountingMsg()
{
	//_ASSERT(m_pHandling &&
	//	m_pHandling->nType == NEWSMESSAGE_COUNTING &&
	//	m_nInsertPlace != NOT_NEED_INSERT);
	unsigned int uCurrent = IR_GetCurrentTime();
	if (uCurrent - m_pHandling->uStartTime >= m_pHandling->uTime)
		return false;

	char	sBuffer[sizeof(m_pHandling->sMsg) + 8];
	memcpy(sBuffer, m_pHandling->sMsg, m_nInsertPlace);
	m_CurrentMsg.nMsgLen = m_nInsertPlace;
	int nNumLen = sprintf(sBuffer + m_nInsertPlace, "%d",
		(m_pHandling->uTime - uCurrent + m_pHandling->uStartTime + 999) / 1000);
	m_CurrentMsg.nMsgLen += nNumLen;
	m_nInsertPlace += 2;
	if (m_pHandling->nMsgLen > m_nInsertPlace)
	{
		memcpy(sBuffer + m_CurrentMsg.nMsgLen,
			m_pHandling->sMsg + m_nInsertPlace ,
			m_pHandling->nMsgLen - m_nInsertPlace);
		m_CurrentMsg.nMsgLen += m_pHandling->nMsgLen - m_nInsertPlace;
	}
	m_nInsertPlace -= 2;

	//此段特殊的额外处理是为了...
	{
		//_ASSERT(nNumLen > 5);
		nNumLen = 5 - nNumLen;
		if (nNumLen > 0)
		{
			memset(sBuffer + m_CurrentMsg.nMsgLen, 0, nNumLen);
			m_CurrentMsg.nMsgLen += nNumLen;
		}
	}

	if (m_CurrentMsg.nMsgLen > sizeof(m_CurrentMsg.sMsg))
	{
		m_CurrentMsg.nMsgLen = TSplitEncodedString(sBuffer, m_CurrentMsg.nMsgLen,
			sizeof(m_CurrentMsg.sMsg), true);
	}
	memcpy(m_CurrentMsg.sMsg, sBuffer, m_CurrentMsg.nMsgLen);
	return true;
}

/*********************************************************************************
*功能：重置当前所显示消息的显示状态，重置计时器
**********************************************************************************/
void KUiNewsMessage::Reset()
{
	if(m_pHandling == NULL)
		return;

	m_uLastShowTime = m_uLastScrollTime = IR_GetCurrentTime();

	ConvertMsg();
	m_bJustIncoming = true;
	m_nCharIndex = 0;
	m_nHalfIndex = 0;
	m_nTextPosX = m_nAbsoluteLeft + m_nIndentH + m_nVisionWidth;
}

bool KUiNewsMessage::ScrollMsg()
{
	static int snSkipTimes = 0;
	if (snSkipTimes > 0)
	{
		m_nTextPosX -= m_nFontHalfWidth[m_nHalfIndex];
		m_nHalfIndex ^= 1;
		snSkipTimes--;
		if (snSkipTimes == 0 && m_bJustIncoming == false)
		{
			m_nTextPosX = m_nAbsoluteLeft + m_nIndentH;
		}
		return false;
	}
	//_ASSERT(m_pHandling);
	if (m_nInsertPlace != NOT_NEED_INSERT)
	{
		if (MakeCountingMsg() == false)
			return true;
	}
	int	nPos, nLen;
	if (m_bJustIncoming)
	{
		nLen = 1;
		if (m_nCharIndex < m_CurrentMsg.nMsgLen)
		{
			nPos = TGetEncodedTextOutputLenPos(m_CurrentMsg.sMsg + m_nCharIndex,
				m_CurrentMsg.nMsgLen - m_nCharIndex, nLen, false, m_nFontSize);
			m_nCharIndex += nPos;
		}
		if (nLen)
		{
			m_nTextPosX -= m_nFontHalfWidth[m_nHalfIndex];
			m_nHalfIndex ^= 1;
			snSkipTimes = nLen - 1;
		}
		if (m_nTextPosX < m_nAbsoluteLeft + m_nIndentH)
		{
			m_nTextPosX = m_nAbsoluteLeft + m_nIndentH;
			m_bJustIncoming = false;
			nLen = 1;
			m_nCharIndex = TGetEncodedTextOutputLenPos(m_CurrentMsg.sMsg,
				m_CurrentMsg.nMsgLen, nLen, false, m_nFontSize);
			if (m_nCharIndex >= m_CurrentMsg.nMsgLen)
			{
				snSkipTimes = 0;
				return true;
			}
		}
	}
	else
	{
		nLen = 1;
		nPos = TGetEncodedTextOutputLenPos(m_CurrentMsg.sMsg + m_nCharIndex,
				m_CurrentMsg.nMsgLen - m_nCharIndex, nLen, false, m_nFontSize);
		m_nCharIndex += nPos;
		if (nLen > 1)
		{
			m_nTextPosX += nLen / 2 * m_nFontSize;
			if ((nLen % 2) == 0)
			{
				m_nTextPosX -= m_nFontHalfWidth[m_nHalfIndex];
				m_nHalfIndex ^= 1;
			}
			snSkipTimes = nLen - 1;
		}
		if (m_nCharIndex >= m_CurrentMsg.nMsgLen)
		{
			snSkipTimes = 0;
			return true;
		}
	}
	return false;
}