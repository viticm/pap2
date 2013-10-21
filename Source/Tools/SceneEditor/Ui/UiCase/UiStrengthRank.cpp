/********************************************************************
File        : UiStrengthRank.cpp
Creator     : Fyt(Fan Zhanpeng)
create data : 08-11-2003(mm-dd-yyyy)
Description : 排名窗口的实现部分
*********************************************************************/

#include "KWin32.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "../Elem/WndMessageListBox.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/gamedatadef.h"
#include "../../../core/src/CoreShell.h"
#include "../../../Engine/src/Text.h"
#include "../../../Engine/src/KFilePath.h"
#include "UiStrengthRank.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define INDEX_AND_RANK_DATA_INI "排名的临时文件.ini"
#define INDEX_AND_RANK_MAPPING "\\Ui\\排名ID.ini"
#define STRENGTH_RANK_INI "排名.ini"
#define REMOVE_TEMP_FILE_TIME 7200000

//发送获取信息请求以后，得不到响应的超时时间
#define GET_RANK_TIME_OUT 10000

extern iCoreShell* g_pCoreShell;
KUiStrengthRank* KUiStrengthRank::m_pSelf;


KLinkStruct<KRankIndexNode> KUiStrengthRank::m_IndexData;
KLinkStruct<KRankNode>      KUiStrengthRank::m_RankData;
KIniFile                    KUiStrengthRank::m_IndexIDIni;
BOOL                        KUiStrengthRank::m_bIsIndexDataChange = FALSE;
BOOL                        KUiStrengthRank::m_bIsRankDataChange = FALSE;
unsigned int                KUiStrengthRank::m_nLastTime = 0;


extern IInlinePicEngineSink* g_pIInlinePicSink;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


KUiStrengthRank::KUiStrengthRank()
{
	m_pCurrentIndexNode= NULL;
	m_pIDontThinkSo  = NULL;
	m_pLastGettimes = NULL;
	m_uLastGetRankTime = 0;
	m_uLastGetIndexTime  = 0;
	m_nLastIndex = -1;
}

KUiStrengthRank::~KUiStrengthRank()
{
	if(m_pLastGettimes)
	{
		delete(m_pLastGettimes);
		m_pLastGettimes = NULL;
	}
}


/****************************************************************************
*功能：如果窗口正被显示，则返回实例指针
*****************************************************************************/
KUiStrengthRank* KUiStrengthRank::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}


/****************************************************************************
*功能：打开窗口，返回唯一的一个类对象实例
*****************************************************************************/
KUiStrengthRank* KUiStrengthRank::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiStrengthRank;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->BringToTop();
		m_pSelf->Show();
		m_pSelf->AutoRemoveTempFile();
	}
	return m_pSelf;
}


/****************************************************************************
*功能：打开窗口，显示默认数据，返回唯一的一个类对象实例
*****************************************************************************/
KUiStrengthRank* KUiStrengthRank::OpenDefaultWindow(int nId)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiStrengthRank;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->BringToTop();
		m_pSelf->Show();
		m_pSelf->AutoRemoveTempFile();
		m_pSelf->ArrangeDefaultData();
	}
	return m_pSelf;
}


/****************************************************************************
*功能：安排默认数据的显示
*****************************************************************************/
void KUiStrengthRank::ArrangeDefaultData()
{
	KIniFile Ini;

	if(Ini.Load(INDEX_AND_RANK_MAPPING))
	{
		KRankIndex *pIndex = NULL;
		int nCount;

    	m_IndexData.Clear();
		m_pCurrentIndexNode = NULL;
    	m_RankData.Clear();
		Ini.GetInteger("Main", "ListCount", 0, &nCount);
		pIndex = new KRankIndex[nCount];

		if(pIndex)
		{
    		for(int i=0;i < nCount;i++)
		    {
        	    pIndex[i].usIndexId = i + 1;
    	        pIndex[i].bValueAppened = 1;
    	        pIndex[i].bSortFlag = 1;
		    }
			NewIndexArrive(nCount, pIndex);
			m_pCurrentIndexNode = m_IndexData.Reset();
			delete(pIndex);
		}
	}
}


/****************************************************************************
*功能：自动检测和删除临时文件
*****************************************************************************/
void KUiStrengthRank::AutoRemoveTempFile()
{
	if(m_nLastTime == 0)
	{
		m_nLastTime = IR_GetCurrentTime();
	}
	else if(IR_GetCurrentTime() - m_nLastTime > REMOVE_TEMP_FILE_TIME)
	{
		m_nLastTime = IR_GetCurrentTime();
		RemoveTempFile();
	}
}


/****************************************************************************
*功能：初始化窗口
*****************************************************************************/
void KUiStrengthRank::Initialize()
{
	AddChild(&m_BtnClose);
	AddChild(&m_IndexList);
	AddChild(&m_IndexScroll);
	AddChild(&m_RankList);
	m_IndexList.SetScrollbar(&m_IndexScroll);
	AddChild(&m_RankScroll);
	m_RankList.SetScrollbar(&m_RankScroll);

	m_IndexList.SetTopItemIndex(0);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	if(g_pIInlinePicSink)
	{
		g_pIInlinePicSink->AddCustomInlinePic(m_usSteadyPic, m_szSteadyPicPath);
		g_pIInlinePicSink->AddCustomInlinePic(m_usDownPic, m_szDownPicPath);
		g_pIInlinePicSink->AddCustomInlinePic(m_usUpPic, m_szUpPicPath);
	}

	int nWidth, nHeight;
	m_RankList.GetSize(&nWidth, &nHeight);
	m_nLineLen = nWidth * 2 / m_nFontSize - 1;

	if(m_pLastGettimes == NULL && m_IndexData.GetCount())
	{
		m_pLastGettimes = new unsigned int[m_IndexData.GetCount()];
		if(m_pLastGettimes)
		    memset(m_pLastGettimes, 0, m_IndexData.GetCount());
	}

	Wnd_AddWindow(this);
}


/****************************************************************************
*功能：关闭窗口，同时可以选则是否删除对象实例
*****************************************************************************/
void KUiStrengthRank::CloseWindow(bool bDestory)
{
	if (m_pSelf)
	{
		m_pSelf->Hide();
		m_pSelf->ReleaseData();
		if (bDestory)
		{
			m_pSelf->ReleasePic();
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}


/****************************************************************************
*功能：更新控件数据的统一调度函数
*****************************************************************************/
void KUiStrengthRank::Breathe()
{
   	if(m_bIsIndexDataChange)
   	{
	    ConstructIndexView();
	    m_bIsIndexDataChange = FALSE;
		if (m_nLastIndex < 0 || m_nLastIndex >= m_IndexData.GetCount())
		{
			m_IndexList.SetCurSel(0);
		}
    }
    if(m_bIsRankDataChange)
    {
   		ConstructRankView();
	    m_bIsRankDataChange = FALSE;
    }
}


/****************************************************************************
*功能：响应窗口消息的窗口函数
*****************************************************************************/
int	KUiStrengthRank::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_LIST_ITEM_SEL:
		if (uParam == (unsigned int)(KWndWindow*)&m_IndexList)
		{
			int nIndex = nParam;
			if (nIndex < 0)
			{
				if (m_nLastIndex >= 0)
					m_IndexList.SetCurSel(m_nLastIndex);
			}
			else if (nIndex != m_nLastIndex)
			{
				m_nLastIndex = nIndex;
				// Get the content of "nParam" index.
				KRankIndexNode *SelNode = m_IndexData.GetItem(nIndex);
				if(SelNode)
				{
					m_pCurrentIndexNode = SelNode;
				    UpdateRankData(SelNode->usIndexId);
				}
			}
		}
		break;

    case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)(KWndWindow*)&m_IndexScroll)
			m_IndexList.SetTopItemIndex(nParam);
		else if (uParam == (unsigned int)(KWndWindow*)&m_RankScroll)
			m_RankList.SetFirstShowLine(nParam);
		break;

    case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_BtnClose)
			CloseWindow(false);
		break;

	case WM_KEYDOWN:
		if(uParam == VK_ESCAPE)
			CloseWindow(false);
		break;
	default:
		nRet  = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}


/****************************************************************************
*功能：载入界面方案
*****************************************************************************/
void KUiStrengthRank::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, STRENGTH_RANK_INI);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->m_BtnClose.Init(&Ini, "BtnClose");
			m_pSelf->m_IndexList.Init(&Ini, "IndexList");
			m_pSelf->m_IndexScroll.Init(&Ini, "IndexScroll");
			m_pSelf->m_RankScroll.Init(&Ini, "MessageScroll");
			m_pSelf->m_RankList.Init(&Ini, "MessageList");
			Ini.GetString("Main", "Color", "0,0,0", Buff, sizeof(Buff));
			m_pSelf->m_uColor = GetColor(Buff);
			Ini.GetInteger("MessageList", "Font", 12, &m_pSelf->m_nFontSize);

			Ini.GetString("Main", "UpPic", "", m_pSelf->m_szUpPicPath, sizeof(m_pSelf->m_szUpPicPath));
			Ini.GetString("Main", "DownPic", "", m_pSelf->m_szDownPicPath, sizeof(m_pSelf->m_szDownPicPath));
			Ini.GetString("Main", "SteadyPic", "", m_pSelf->m_szSteadyPicPath, sizeof(m_pSelf->m_szSteadyPicPath));
			Ini.GetInteger("Main", "DefaultID", 0, &m_pSelf->m_nDefaultID);
		}
	}
}


/****************************************************************************
*功能：卸载图形文件
*****************************************************************************/
void KUiStrengthRank::ReleasePic()
{
	if(g_pIInlinePicSink)
	{
		g_pIInlinePicSink->RemoveCustomInlinePic(m_usUpPic);
		g_pIInlinePicSink->RemoveCustomInlinePic(m_usDownPic);
		g_pIInlinePicSink->RemoveCustomInlinePic(m_usSteadyPic);
	}
}


/****************************************************************************
*功能：释放数据和重置显示
*****************************************************************************/
void KUiStrengthRank::ReleaseData()
{
	m_IndexData.Clear();
	m_RankData.Clear();
	m_pCurrentIndexNode = NULL;
	m_IndexList.SetContent(m_pIDontThinkSo,0,0,0);
	m_RankList.Clear();
	if(m_pIDontThinkSo)
	{
		delete(m_pIDontThinkSo);
		m_pIDontThinkSo = NULL;
	}
	m_nLastIndex = -1;
}


/****************************************************************************
*功能：更新索引数据-有INI就取INI，没有就问Core要，WAKAKA
*****************************************************************************/
//void KUiStrengthRank::UpdateIndexData()
//{
//    KIniFile Ini;
//
//	m_IndexData.Clear();
//	if(!(Ini.Load(INDEX_AND_RANK_DATA_INI) && LoadIndexIni(&Ini)))
//		SendGetIndexToCore();
//}


/****************************************************************************
*功能：更新排名数据-有INI就取INI，没有就问Core要，WAKAKA
*****************************************************************************/
void KUiStrengthRank::UpdateRankData(int nID)
{
	KIniFile Ini;

	m_RankData.Clear();
	m_RankList.Clear();
	char	szFileName[128];
	sprintf(szFileName, "%s\\%s", g_UiBase.GetUserTempDataFolder(), INDEX_AND_RANK_DATA_INI);
	if(!(Ini.Load(szFileName) && LoadRankIni(&Ini, nID)))
		SendGetRankToCore(nID);
}


/****************************************************************************
*功能：从INI中载入索引数据
*****************************************************************************/
BOOL KUiStrengthRank::LoadIndexIni(KIniFile *Ini)
{
	BOOL       nRet   = FALSE;
	KRankIndex *pData = NULL;
	int        nCount, i, j;
	char       szKey[32];
	KRankIndex NewNode;

	if(Ini->IsSectionExist("Index"))
	{
		Ini->GetInteger("Index", "Count", 0, &nCount);
		if(!pData)
		    pData = new KRankIndex[nCount];
		if(pData && nCount)
		{
            for(i = 1;i <= nCount;i++)
	        {
				memset(&NewNode, 0, sizeof(KRankIndex));
				sprintf(szKey, "%d_ID", i);
				Ini->GetInteger("Index", szKey, 0, &j);
				NewNode.usIndexId = (unsigned short)j;
				sprintf(szKey, "%d_Appened", i);
				Ini->GetInteger("Index", szKey, 0, &j);
				NewNode.bValueAppened = (j != 0);
				sprintf(szKey, "%d_SortFlag", i);
				Ini->GetInteger("Index", szKey, 0, &j);
				NewNode.bSortFlag = (j != 0);
                memcpy(pData + i - 1, &NewNode, sizeof(KRankIndex));
	        }

			AddIndex(nCount, pData);
			m_bIsIndexDataChange = nRet = TRUE;
			delete(pData);
			pData = NULL;
		}
	}

	return nRet;
}


/****************************************************************************
*功能：从INI中载入排名数据
*****************************************************************************/
BOOL KUiStrengthRank::LoadRankIni(KIniFile *Ini, int nID)
{
	char szSection[32], szKey[32];
	BOOL nRet           = FALSE;
	KRankMessage *pData = NULL;
	KRankMessage  NewNode;
    int  nCount, i;

	sprintf(szSection, "%d", nID);
	if(Ini->IsSectionExist(szSection))
	{
		Ini->GetInteger(szSection, "Count", 0, &nCount);
		if(!pData)
		    pData = new KRankMessage[nCount];
		if(pData && nCount)
		{
	        for(i = 1;i <= nCount;i++)
	        {
				memset(&NewNode, 0, sizeof(KRankMessage));
        		sprintf(szKey, "%d", i);
			    Ini->GetStruct(szSection, szKey, &NewNode, sizeof(KRankMessage));
				memcpy(pData + i - 1, &NewNode, sizeof(KRankMessage));
	        }

			AddRank(nID, nCount, pData);
			m_bIsRankDataChange = nRet = TRUE;
			delete(pData);
			pData = NULL;
		}
	}

	return nRet;
}


/****************************************************************************
*功能：从Core那里抢劫索引数据
*****************************************************************************/
//void KUiStrengthRank::SendRobIndexToCore()
//{
//	SendGetIndexToCore();
//}


/****************************************************************************
*功能：从Core那里获取索引数据
*****************************************************************************/
//void KUiStrengthRank::SendGetIndexToCore()
//{
//	if(g_pCoreShell && IR_GetCurrentTime() - m_uLastGetIndexTime > GET_RANK_TIME_OUT)
//	{
//		m_uLastGetIndexTime = IR_GetCurrentTime();
//		g_pCoreShell->OperationRequest(GOI_QUERY_RANK_INFORMATION, 0, TRUE);
//	}
//}


/****************************************************************************
*功能：从Core那里获取排名数据
*****************************************************************************/
void KUiStrengthRank::SendGetRankToCore(int nID)
{
	if(g_pCoreShell && m_pLastGettimes && IR_GetCurrentTime() - m_pLastGettimes[m_nLastIndex] > GET_RANK_TIME_OUT)
	{
		m_pLastGettimes[m_nLastIndex] = IR_GetCurrentTime();
		g_pCoreShell->OperationRequest(GOI_QUERY_RANK_INFORMATION, (unsigned int)nID, 0);
	}
}


/****************************************************************************
*功能：把Index的数据(m_data)构造进去m_indexList里面，那就可以绘制的时候显示出来
*****************************************************************************/
void KUiStrengthRank::ConstructIndexView()
{
	KRankIndexNode *pNode;
	unsigned char  *pPos;
	int i, nCount;

	nCount = KUiStrengthRank::m_IndexData.GetCount();
	pNode  = KUiStrengthRank::m_IndexData.Reset();
	if(m_pIDontThinkSo)
	{
		delete(m_pIDontThinkSo);
		m_pIDontThinkSo = NULL;
	}
	m_pIDontThinkSo = new unsigned char[MAX_RANK_MESSAGE_LENGTH * nCount];
	if(m_pIDontThinkSo)
	{
		pPos = m_pIDontThinkSo;
    	for(i = 0;i < nCount;i++)
    	{
			memcpy(pPos, pNode->szMsg, pNode->nMsgLen);
			pPos[pNode->nMsgLen] = 0;
		    pNode = KUiStrengthRank::m_IndexData.NextItem();
			pPos += MAX_RANK_MESSAGE_LENGTH;
	    }
		m_IndexList.SetContent(m_pIDontThinkSo, nCount, MAX_RANK_MESSAGE_LENGTH, 0);
	}
}


/****************************************************************************
*功能：把Rank的排名数据构造进去m_RankList里面，以可以绘制的时候显示出来
*****************************************************************************/
void KUiStrengthRank::ConstructRankView()
{
	int i, nCount;
	KRankNode *pNode;

	nCount = KUiStrengthRank::m_RankData.GetCount();
	pNode  = KUiStrengthRank::m_RankData.Reset();
	m_RankList.SetFirstShowLine(0);
	if(m_pCurrentIndexNode && (m_pCurrentIndexNode->bValueAppened || m_pCurrentIndexNode->bSortFlag) )
	{
		char szBuf[32];
    	for(i = 0;i < nCount;i++)
	    {
			pNode->szMsg[pNode->usMsgLen] = KTC_TAB;
			pNode->usMsgLen ++;
			//构造额外数据字符串
			if(m_pCurrentIndexNode->bValueAppened)
			{
    			sprintf(szBuf, "%d", pNode->nValueAppend);
			    //有单位就写下单位
			    if(m_pCurrentIndexNode->szUnit[0])
			    {
    				sprintf(szBuf, "%s%s", szBuf, m_pCurrentIndexNode->szUnit);
			    }
                //构造好的字符串扩展进排名字符串里
			    memcpy(pNode->szMsg + pNode->usMsgLen, szBuf, strlen(szBuf));
			    pNode->usMsgLen += strlen(szBuf);
			}
            //加进上升持平下降图片
			if(m_pCurrentIndexNode->bSortFlag)
			{
        		pNode->szMsg[pNode->usMsgLen] = KTC_INLINE_PIC;
		        if(pNode->nValueAppend > 0)
			    {
    		        *((short *)(&pNode->szMsg[pNode->usMsgLen + 1])) = m_usUpPic;
			    }
			    else if(pNode->nValueAppend < 0)
			    {
    			    *((short *)(&pNode->szMsg[pNode->usMsgLen + 1])) = m_usDownPic;
			    }
			    else
			    {
    			    *((short *)(&pNode->szMsg[pNode->usMsgLen + 1])) = m_usSteadyPic;
			    }
			    pNode->usMsgLen += 3;
			}
			//展开
			pNode->usMsgLen = TDrawOutString(pNode->szMsg, pNode->usMsgLen,
				                             m_nLineLen, m_nFontSize);

		    m_RankList.AddOneMessage(pNode->szMsg, pNode->usMsgLen, 0);
		    pNode = KUiStrengthRank::m_RankData.NextItem();
	    }
	}
	else
	{
		for(i = 0;i < nCount;i++)
	    {
			pNode->usMsgLen = TDrawOutString(pNode->szMsg, pNode->usMsgLen,
				                             m_nLineLen, m_nFontSize);

		    m_RankList.AddOneMessage(pNode->szMsg, pNode->usMsgLen, 0);
		    pNode = KUiStrengthRank::m_RankData.NextItem();
	    }
	}
}


/****************************************************************************
*功能：新的索引消息来了
*****************************************************************************/
void KUiStrengthRank::NewIndexArrive(unsigned int uParam, struct KRankIndex *pIndex)
{
	if(m_pSelf)
	{
    	if(m_pSelf->m_pLastGettimes)
    	{
		    delete(m_pSelf->m_pLastGettimes);
		    m_pSelf->m_pLastGettimes = NULL;
	    }
	    if(uParam > 0)
	    {
    		m_pSelf->m_pLastGettimes = new unsigned int[uParam];
		    if(m_pSelf->m_pLastGettimes)
			{
				memset(m_pSelf->m_pLastGettimes, 0, sizeof(unsigned int) * uParam);
			}
    	}
	}
	AddIndex(uParam, pIndex);
	//StoreNewDataToTempFile(uParam, pIndex);
}


/****************************************************************************
*功能：新的排名信息来了
*****************************************************************************/
void KUiStrengthRank::NewRankArrive(unsigned int uParam, KRankMessage *pRankData)
{
	int nCount  = uParam & 0xffff;
	int nID     = uParam >> 16;
	AddRank(nID, nCount, pRankData);
	if(nID >= 0 && nID < NO_SAVE_ID)
	{
		StoreNewDataToTempFile(uParam, pRankData);
	}
	else if(m_pSelf)
	{
		m_pSelf->ConstructRankView();
	}
}


/****************************************************************************
*功能：添加索引信息
*****************************************************************************/
void KUiStrengthRank::AddIndex(unsigned int uParam, struct KRankIndex *pIndex)
{
	KRankIndex  *pIncome;
	KRankIndexNode Node;
	char szKey[32];
	int nCount, i;

	nCount = uParam;
	pIncome = pIndex;
	if(!m_IndexIDIni.IsSectionExist("Index"))
		m_IndexIDIni.Load(INDEX_AND_RANK_MAPPING);
    for(i = 0;i < nCount;i++)
	{
		memcpy(&Node, pIncome, sizeof(KRankIndex));
		sprintf(szKey, "%d_Title", pIncome->usIndexId);

	    m_IndexIDIni.GetString("Index", szKey, "", Node.szMsg,
    	                    MAX_RANK_MESSAGE_LENGTH);

		if(!Node.szMsg[0])
		{
			pIncome++;
			continue;
		}
		else
		{
		    Node.nMsgLen = strlen(Node.szMsg);

		    sprintf(szKey, "%d_Unit", pIncome->usIndexId);
		    m_IndexIDIni.GetString("Index", szKey, "", Node.szUnit, sizeof(Node.szUnit));
		    m_IndexData.AddItem(Node);
		    pIncome++;
		}
	}
	m_bIsIndexDataChange = TRUE;
}


/****************************************************************************
*功能：添加排名信息
*****************************************************************************/
void KUiStrengthRank::AddRank(int nID, int nCount, KRankMessage *pRankData)
{
	KRankMessage  *pIncome;
	int i;
	KRankNode Node;

	pIncome = pRankData;

	if(m_pSelf && m_pSelf->m_pCurrentIndexNode && nID == m_pSelf->m_pCurrentIndexNode->usIndexId)
	{
		for(i = 0;i < nCount;i++)
	    {
			if(pIncome->szMsg[0])
			{
    		    memcpy(&Node, pIncome, sizeof(KRankMessage));
		        m_RankData.AddItem(Node);
			}
		    pIncome++;
	    }
		m_bIsRankDataChange = TRUE;
	}
}


/****************************************************************************
*功能：把索引和排名记录进临时文件
*****************************************************************************/
void KUiStrengthRank::StoreTempIni()
{
	KIniFile Ini;
	int  nCount, i;
	KRankNode *pNode;
	KRankIndexNode *pIndexNode;
	char szSection[32], szKey[32], szFileName[128];

    sprintf(szFileName, "%s\\%s", g_UiBase.GetUserTempDataFolder(), INDEX_AND_RANK_DATA_INI);
    Ini.Load(szFileName);

	//先写索引
	nCount = KUiStrengthRank::m_IndexData.GetCount();
	if(nCount)
	{
		Ini.WriteInteger("Index", "Count", nCount);
		pIndexNode = KUiStrengthRank::m_IndexData.Reset();
		for(i = 0;i < nCount;i++)
		{
			sprintf(szKey, "%d_ID", i + 1);
			Ini.WriteInteger("Index", szKey, pIndexNode->usIndexId);
			sprintf(szKey, "%d_Appened", i + 1);
			Ini.WriteInteger("Index", szKey, pIndexNode->bValueAppened);
			sprintf(szKey, "%d_SortFlag", i + 1);
			Ini.WriteInteger("Index", szKey, pIndexNode->bSortFlag);
			pIndexNode = KUiStrengthRank::m_IndexData.NextItem();
			if(!pIndexNode) break;
		}
	}
    //然后写排名
	nCount = KUiStrengthRank::m_RankData.GetCount();
	if(nCount && m_pSelf->m_pCurrentIndexNode)
	{
		sprintf(szSection, "%d", m_pSelf->m_pCurrentIndexNode->usIndexId);
		Ini.WriteInteger(szSection, "Count", nCount);
		pNode = KUiStrengthRank::m_RankData.Reset();
		for(i = 0;i < nCount;i++)
		{
			sprintf(szKey, "%d", i + 1);
			Ini.WriteStruct(szSection, szKey, pNode, sizeof(KRankMessage));
			pNode = KUiStrengthRank::m_RankData.NextItem();
			if(!pNode) break;
		}
	}
	Ini.Save(szFileName);
}


/****************************************************************************
*功能：把新收到的索引资料存到临时文件
*****************************************************************************/
void KUiStrengthRank::StoreNewDataToTempFile(unsigned int uParam, KRankIndex *pInData)
{
	KIniFile Ini;
	char szKey[32], szFileName[128];
	KRankIndex *pIncome;
	int  nCount, nIniCount, i;

    nCount = uParam;
	pIncome= pInData;

	sprintf(szFileName, "%s\\%s", g_UiBase.GetUserTempDataFolder(), INDEX_AND_RANK_DATA_INI);

	Ini.Load(szFileName);
	Ini.GetInteger("Index", "Count", 0, &nIniCount);
    Ini.WriteInteger("Index", "Count", nIniCount + nCount);
	for(i = 0;i < nCount;i++)
    {
		sprintf(szKey, "%d_ID", i + 1 + nIniCount);
		Ini.WriteInteger("Index", szKey, pIncome[i].usIndexId);
		sprintf(szKey, "%d_Appened", i + 1 + nIniCount);
		Ini.WriteInteger("Index", szKey, pIncome[i].bValueAppened);
		sprintf(szKey, "%d_SortFlag", i + 1 + nIniCount);
		Ini.WriteInteger("Index", szKey, pIncome[i].bSortFlag);
    }
	Ini.Save(szFileName);
}


/****************************************************************************
*功能：把新收到的排名资料存到临时文件
*****************************************************************************/
void KUiStrengthRank::StoreNewDataToTempFile(unsigned int uParam, KRankMessage *pInData)
{
	KIniFile Ini;
	KRankMessage *pIncome;
	int  nCount, nIniCount, nID, i;
	char szSection[32], szKey[32], szFileName[128];

	nCount = uParam & 0xffff;
	nID    = uParam >> 16;
	pIncome= pInData;

	sprintf(szFileName, "%s\\%s", g_UiBase.GetUserTempDataFolder(), INDEX_AND_RANK_DATA_INI);

	Ini.Load(szFileName);
    sprintf(szSection, "%d", nID);
    Ini.GetInteger(szSection, "Count", 0, &nIniCount);
    Ini.WriteInteger(szSection, "Count", nIniCount + nCount);
    for(i = 0;i < nCount;i++)
    {
		sprintf(szKey, "%d", i + 1 + nIniCount);
	    Ini.WriteStruct(szSection, szKey, pIncome + i, sizeof(KRankMessage));
    }
	Ini.Save(szFileName);
}


/****************************************************************************
*功能：检测临时文件是否存在，不存在则创建
*****************************************************************************/
int KUiStrengthRank::AttachTempFile()
{
	FILE *fp;
	char szPath[128], szFile[128];

	sprintf(szFile, "%s\\%s", g_UiBase.GetUserTempDataFolder(), INDEX_AND_RANK_DATA_INI);
	g_GetFullPath(szPath, szFile);
    if(!(fp = fopen(szPath, "r")))
	{
		fp = fopen(szPath, "w");
		fclose(fp);
		if(!(fp = fopen(szPath, "r")))
			return 0;
		else
			fclose(fp);
	}
	else
	{
		fclose(fp);
	}
	return 1;
}


/****************************************************************************
*功能：清空临时文件
*****************************************************************************/
void KUiStrengthRank::RemoveTempFile()
{
	char szPath[128], szFile[128];

	sprintf(szFile, "%s\\%s", g_UiBase.GetUserTempDataFolder(), INDEX_AND_RANK_DATA_INI);
	g_GetFullPath(szPath, szFile);
	remove(szPath);
}


/****************************************************************************
*功能：把字串左右扩展到指定长度，中间用Tab分开
*****************************************************************************/
int TDrawOutString(char *szBuf, int nBufLen, int nLineLen, int nFontsize)
{
	int nTabPos, nOldLineLen;

	nTabPos = TFindSpecialCtrlInEncodedText(szBuf, nBufLen, 0, KTC_TAB);
	if(nTabPos > 0)
	{
		TGetEncodedTextLineCount(szBuf, nBufLen, 0, nOldLineLen, nFontsize);
		if(nOldLineLen < nLineLen)
		{
			memmove(szBuf + nTabPos + nLineLen - nOldLineLen + 1, szBuf + nTabPos + 1, nBufLen - nTabPos - 1);
			memset(szBuf + nTabPos, 0x20, nLineLen - nOldLineLen + 1);
			return nBufLen + nLineLen - nOldLineLen;
		}
	}
	return nBufLen;
}
