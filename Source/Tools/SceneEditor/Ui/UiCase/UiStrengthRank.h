/********************************************************************
File        : UiStrengthRank.h
Creator     : Fyt(Fan Zhanpeng)
create data : 08-11-2003(mm-dd-yyyy)
Description : 排名窗口的头文件
*********************************************************************/

#if !defined(AFX_KUISTRENGTHORDER_H__7801F7C7_BAE8_4A49_BEA8_63B5F8FA0BB1__INCLUDED_)
#define AFX_KUISTRENGTHORDER_H__7801F7C7_BAE8_4A49_BEA8_63B5F8FA0BB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KIniFile.h"
#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndList.h"
#include "../Elem/WndMessageListBox.h"
#include "../../../Engine/src/LinkStruct.h"
#include "../../Core/Src/GameDataDef.h"

#define MAX_RANK_MESSAGE_LENGTH 128
#define NO_SAVE_ID				10000 //这个数字以上的ID号的数据不存临时文件
									  //即客户端不记录

struct KRankIndexNode : public KRankIndex
{
	char szMsg[MAX_RANK_MESSAGE_LENGTH];
	int  nMsgLen;
	char szUnit[16];
	KRankIndexNode *next;
	KRankIndexNode *previous;
};

struct KRankNode : public KRankMessage
{
	KRankNode* next;
	KRankNode* previous;
};

class KUiStrengthRank : public KWndShowAnimate
{
public:
	KUiStrengthRank();
	virtual ~KUiStrengthRank();

	static        KUiStrengthRank* OpenWindow();    //打开窗口
	static        KUiStrengthRank* OpenDefaultWindow(int nId = -1);
	static        KUiStrengthRank* GetIfVisible();  //如果窗口正被显示，则返回实例指针
	static void   CloseWindow(bool bDestory = TRUE);//关闭窗口，同时可以选则是否删除对象实例
	static void   LoadScheme(const char* pScheme);  //载入界面方案

	//新的排名数据来了
	static void   NewRankArrive(unsigned int uParam, KRankMessage *pRankData);
	//新的索引数据来了
	static void   NewIndexArrive(unsigned int uParam, struct KRankIndex *pIndex);

	static int    AttachTempFile();                            //检测临时文件是否存在，不存在则创建
	static void   RemoveTempFile();                            //删除临时文件

private:
	static void   StoreTempData( );                            //把索引和排名记录进临时文件
	static void   StoreNewDataToTempFile(unsigned int uParam,  //把新收到的排名资料存到临时文件
		                                 KRankMessage *pInData);
	static void   StoreNewDataToTempFile(unsigned int uParam,  //把新收到的索引资料存到临时文件
		                                 KRankIndex *pInData);

private:
	int	   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	virtual void Breathe();                                    //咦？为什么这里也有？

private:
	static KUiStrengthRank* m_pSelf;

private:
	void   Initialize();
//	void   UpdateIndexData();                   //更新索引的数据
	void   UpdateRankData(int nID);             //更新指定排名的数据

	static void StoreTempIni();                 //把索引和排名记录进临时文件
	BOOL   LoadIndexIni(KIniFile *Ini);         //从INI中载入索引数据
	BOOL   LoadRankIni(KIniFile *Ini, int nID); //从INI中载入排名数据
                                                //这是空行
//	void   SendRobIndexToCore();                //从Core那里抢劫索引数据
//	void   SendGetIndexToCore();                //功能同上，从Core那里获取索引数据
	void   SendGetRankToCore(int nID);          //向Core发送获取排名数据的请求
                                                //这也是空行
	void   ConstructIndexView();                //把Index的数据(m_data)构造进去m_indexList里面，以显示出来
	void   ConstructRankView();                 //把Rank的排名数据构造进去m_RankList里面，以显示出来
	void   ReleasePic();                        //释放图形资源
	void   ReleaseData();                       //释放数据和重置显示

	void   ArrangeDefaultData();                //安排默认数据的显示

	static void   AddRank(int nID, int nCount, KRankMessage *pRankData);
	static void   AddIndex(unsigned int uParam, struct KRankIndex *pIndex);
	
	void          AutoRemoveTempFile();         //自动检测和删除临时文件

private:
	KWndButton				  m_BtnClose;
	KWndList				  m_IndexList;
	KWndScrollBar			  m_IndexScroll;
	KWndMessageListBox		  m_RankList;
	KWndScrollBar			  m_RankScroll;

	static KIniFile           m_IndexIDIni;
	static KLinkStruct<KRankIndexNode> m_IndexData; //所有索引数据的链表
	static KLinkStruct<KRankNode> m_RankData;       //一项排名数据的链表

	unsigned int              m_uColor;
	static unsigned int       m_nLastTime;          //用于自动检测的时间
	static BOOL               m_bIsIndexDataChange; //索引数据改变了吗？
	static BOOL               m_bIsRankDataChange;  //排名数据改变了吗？
	KRankIndexNode           *m_pCurrentIndexNode;  //当前正在显示的排名的索引

	int                       m_nFontSize;          //字体大小
	int                       m_nLineLen;           //计算出来的行长度
	int                       m_nDefaultID;          //默认的排名窗口的ID，现在只有一个，就用一个变量记了
	unsigned char            *m_pIDontThinkSo;      //我是为了WndList而存在，好伟大的爱情啊~~(这是传给WndList的SetContent的数组指针)
    unsigned short            m_usUpPic;            //上升的图片
	unsigned short            m_usSteadyPic;        //持平的图片
	unsigned short            m_usDownPic;          //下降的图片

	int						  m_nLastIndex;
	unsigned int             *m_pLastGettimes;
	unsigned int              m_uLastGetIndexTime;
	unsigned int              m_uLastGetRankTime;

	char                      m_szUpPicPath[128];
	char                      m_szDownPicPath[128];
	char                      m_szSteadyPicPath[128];
};

int TDrawOutString(char *szBuf, int nBufLen, int nLineLen, int nFontSize);
#endif // !defined(AFX_KUISTRENGTHORDER_H__7801F7C7_BAE8_4A49_BEA8_63B5F8FA0BB1__INCLUDED_)
