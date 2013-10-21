/*******************************************************************************
File        : UiTongManager.h
Creator     : Fyt(Fan Zhanpeng)
create data : 08-29-2003(mm-dd-yyyy)
Description : 管理帮会的界面
********************************************************************************/

#if !defined(AFX_UITONGMANAGER_H__13BA213D_11EC_4F24_BF98_F51C3F414D6D__INCLUDED_)
#define AFX_UITONGMANAGER_H__13BA213D_11EC_4F24_BF98_F51C3F414D6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../elem/wndlabeledbutton.h"
#include "../elem/wndpuretextbtn.h"
#include "../elem/wndscrollbar.h"
#include "../elem/wndimage.h"
#include "../elem/wndlist2.h"

#include "../../Engine/Src/LinkStruct.h"
#include "../../Core/Src/GameDataDef.h"

#define TONG_MANAGER_STATUS_SHOW_ALL_MEMBER 0x01

struct KPilgarlicItem : public KTongMemberItem
{
	int nHashID;
	KPilgarlicItem *next;
    KPilgarlicItem *previous;
};


class KUiTongManager : KWndImage
{
public:
	KUiTongManager();
	virtual ~KUiTongManager();

	static        KUiTongManager* OpenWindow();      //打开窗口
	static        KUiTongManager* OpenWindow(        //打开窗口，同时安排界面布局
		                                     char* pszPlayerName);
	static        KUiTongManager* GetIfVisible();    //如果窗口正被显示，则返回实例指针
	static void   CloseWindow(bool bDestory = TRUE); //关闭窗口，同时可以选则是否删除对象实例
	static void   LoadScheme(const char* pScheme);   //载入界面方案

	                                                 //接受新的数据
	static void   NewDataArrive(KUiGameObjectWithName *ParamInfo, KTongMemberItem *pIncome);

	                                                 //接受帮会信息
	static void   TongInfoArrive(KUiPlayerRelationWithOther* Relation, KTongInfo *pTongInfo);
	static int    RemoveTempFile();                  //删除临时文件

	static void   ResponseResult(                    //对结果的响应入口
		                         KUiGameObjectWithName *pResult, int nbIsSucceed);


public:
	virtual int   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void          ArrangeComposition(                //根据传入的玩家名字，安排界面布局
		                             char* pszPlayerName);


private:
	void          Initialize();

	void          PopupConfirmWindow(                //弹出确认窗口
		                        const char* pszInfo, unsigned int uHandleID, int nbSelf = 0, int nbLeague = 0);

	static void   RequestData(int nType, int nIndex);//向Core发出数据请求
	static void   SaveNewData(                       //把新到的列表信息存到临时文件
		                        KUiGameObjectWithName *ParamInfo, KTongMemberItem *pIncome);

	static void   SaveWholeData(int nType);          //把指定类型的列表数据整个存储起来

	static int    IsPlayerExist(                     //查询特定名字的玩家是否已经存在
		                        KPilgarlicItem *MeToCmp, KLinkStruct<KPilgarlicItem> *pData);

	KLinkStruct<KPilgarlicItem>*                     //查询特定名字的玩家的数据位置
		          FindPlayer(char *pszName);

	KLinkStruct<KPilgarlicItem>*                     //查找玩家处理函数
		          HandleFind(KLinkStruct<KPilgarlicItem> *pData, char *pszName);

	void          LoadData(int nType);               //载入资料
	int           LoadDataHandler(                   //载入资料的处理函数
		                        KLinkStruct<KPilgarlicItem> *pData,
							    const char* szSection);

	void          UpdateBtnTheme(                    //根据打开人的身份更新按钮配置方案
		                        int nType, BOOL IsDissable = FALSE);

	void		  UpdateThemeForList(int nType);	 //根据选择的列表安排按钮方案

	KLinkStruct<KPilgarlicItem>*                     //返回当前所选择的列表类型所对应的数据集
		          GetCurrentSelectedData();

	KPilgarlicItem*                                  //返回所选择的会员
		          GetSelectedPlayer();

	void          UpdateTongInfoView();              //更新帮会讯息的显示

	//更新列表栏
	void          UpdateTongListView(BOOL bAll = FALSE);

	void          ClearTongData();                   //把帮会信息栏清空

	void          UpdateListCheckButton(int nType);  //三个列表CheckBox的管理

	//各种帮会操作的处理函数
	void          OnAction(int nType, int nbForceBondingName = FALSE);

	//带有参数的帮会操作函数
	void		  OnParamAction(int nType, unsigned int uParam, int nParam);

	void          OnRefresh();			             //响应刷新按钮的操作函数
	
	void		  OnAssign();						 //响应任命的操作

	void		  OnSalary();						 //响应发钱操作

	void		  OnMemberTitle(int nParam);		 //响应帮众称号修改的操作

	void          ResponseLeave(                     //响应离帮后的操作
		                        KUiGameObjectWithName *pResult, int nbIsSucceed);

	void          ResponseDismiss(                   //响应踢人后的操作
		                        KUiGameObjectWithName *pResult, int nbIsSucceed);

	void          ResponseDemise(                    //响应传位后的操作
		                        KUiGameObjectWithName *pResult, int nbIsSucceed);
	
	void          ResponseAssign(                    //响应任命后的操作
		                        KUiGameObjectWithName *pResult, int nbIsSucceed);

	void          ResponseApply(                     //响应申请后的操作
		                        KUiGameObjectWithName *pResult, int nbIsSucceed);

	void          ResponseTitle(                     //响应改变称号后的操作
		                        KUiGameObjectWithName *pResult, int nbIsSucceed);

	void          ResponseCamp(                      //响应改变阵营后的操作
		                        KUiGameObjectWithName *pResult, int nbIsSucceed);

	void		  ResponseCreateLeague(				 //响应创建联盟后的操作
								KUiGameObjectWithName *pResult, int nbIsSucceed);

	void		  ResponseDismisLeague(				 //响应解散联盟后的操作
								KUiGameObjectWithName *pResult, int nbIsSucceed);
	
	void		  ResponseDepositMoney(				 //响应存钱后的操作
								KUiGameObjectWithName *pResult, int nbIsSucceed);

	void		  ResponseTakeMoney(				 //响应取钱后的操作
								KUiGameObjectWithName *pResult, int nbIsSucceed);
	
	void		  ResponseLeagueAction(				 //联盟的一般操作的响应入口
								KUiGameObjectWithName *pResult, int nbIsSucceed);

	/**
	 *brief		  响应发奖金后的操作
	 */
	void		  ResponseSalary(KUiGameObjectWithName *pResult, int nbIsSucceed);

	int           FormArrayData(                     //把链表数据组织成数组数据
		                        KTongMemberItem **pData, int nType);

	void		  OnPopupSelectSex(int nX, int nY);	 //弹出帮众称号更改的性别选择菜单

	void		  Refresh(int nType);				 //刷新的主体

	int			  WorkoutIndex(int nSelectIndex,	 //根据列表的选择和当前界面状态运算出
							   int nbLeft);			 //对于数据的index


	//*ATTENTION*必须m_nRelation赋值之后，TongInfoArrive()里面或之后即可
	//根据m_nRelation的值取得信息
	void		  PrepareData(BOOL bIsRefresh = FALSE);

	//帮会管理界面居然要充当数据中心！！！Faint
	//这个方法的作用是把数据主动传递给其他需要这些数据的界面
	void		  GiveDataToOthers(KTongMemberItem *pItem, int nCount, int nType);

private:
	static        KUiTongManager* ms_pSelf;

	static int    m_nElderDataIndex;
	static int    m_nMemberDataIndex;
	static int    m_nCaptainDataIndex;
	static int	  m_nTongLeagueDataIndex;

private:
	enum RESULT_TYPE
	{
		RESULT_T_DISMISS,
		RESULT_T_ASSIGN,
		RESULT_T_DEMISE,
		RESULT_T_LEAVE,
		RESULT_T_PASSWORD_TO_DEMISE,
		RESULT_T_ASSIGN_TITLE,
		RESULT_T_SELECT_SEX,
		RESULT_T_GET_LEAGUE_NAME,
		RESULT_T_REMOVE_LEAGUE_MEMBER_CONFIRM,
		RESULT_T_LEAVE_LEAGUE_CONFIRM,
		RESULT_T_DISMISS_LEAGUE_CONFIRM,
		RESULT_T_DEPOSIT_MONEY,
		RESULT_T_TAKE_MONEY,
	};

private:
	KWndLabeledButton                                //踢人，任命，传位按钮
		          m_BtnDismiss, m_BtnAssign, m_BtnDemise;
	
	KWndLabeledButton
				  m_BtnApplyLeague;					 //申请加入联盟

	KWndLabeledButton
		          m_BtnLeave, m_BtnRecruit;          //离帮，招人按钮

	KWndLabeledButton
		          m_BtnApply, m_BtnCancel;           //申请，取消按钮

	KWndLabeledButton                                //长老、队长、成员列表
		          m_BtnElderList, m_BtnCaptainList, m_BtnMemberList;
	
	KWndLabeledButton
				  m_BtnLeagueList;					 //大联盟帮会列表。一般帮会为了
													 //各自的利益关系，会组成一个联盟
													 //如有名的七帮十一会、风云镖局等
													 //这里就是察看帮会的联盟

	KWndButton                                       //关闭、刷新、称号按钮
		          m_BtnClose, m_BtnRefresh, m_BtnTitle;

	KWndButton    m_BtnExtendList;                   //显示和隐藏没有任命人的位置的按钮

	KWndButton	  m_BtnAddLeague;					 //添加联盟
	KWndButton	  m_BtnLeaveLeague;					 //离开联盟
	KWndButton	  m_BtnRemoveLeague;				 //移除联盟
	KWndButton	  m_BtnCreateLeague;				 //创建联盟
	KWndButton	  m_BtnDepositMoney;				 //存钱
	KWndButton	  m_BtnDismissLeague;				 //解散联盟
	KWndButton	  m_BtnTakeMoney;					 //取钱

	KWndText32    m_TongName;                        //帮会名字
	KWndPureTextBtn
				  m_MasterName;						 //老大名字(五月)

	KWndPureTextBtn
				  m_Alignment;                       //阵营名字

	KWndText32    m_MemberCount, m_Money;            //人数，资金

	KWndText32	  m_TongExp;						 //帮会经验
	KWndText32	  m_TongLevel;						 //帮会等级

	KWndList2     m_ListLeft;                        //中间的列表控件
	KWndList2	  m_ListRight;
	KWndScrollBar m_ListScroll;                      //列表的滚动条

	TONG_MEMBER_FIGURE
		          m_Relation;                        //打开这个界面的人与目标帮会的关系

	KWndButton	  m_MaleTitle, m_FemaleTitle;		 //男帮众和女帮众的称号(图形版)

	KWndButton	  m_btnSalary;						 //发钱咯，不要拉倒


private:
	KLinkStruct<KPilgarlicItem>
		          m_MemberData, m_CaptainData;       //成员列表和队长列表的数据

	KLinkStruct<KPilgarlicItem> m_ElderData;         //长老列表

	KLinkStruct<KPilgarlicItem> m_LeagueData;		 //联盟列表

	static KTongInfo            m_TongData;          //帮会的资料数据


private:
	unsigned int  m_uLastRequestTongDataTime;        //上一次请求帮会资料的时间
	unsigned int  m_uLastRequestElderListTime;       //上一次请求长老列表的时间
	unsigned int  m_uLastRequestMemberListTime;      //上一次请求帮众列表的时间
	unsigned int  m_uLastRequestCaptainListTime;     //上一次请求队长列表的时间

	int           m_nRelation;                       //下面那个玩家与所打开的帮会的关系
	int           m_nIndex;                          //与下面那个对应的玩家的Index
	char          m_szPlayerName[32];                //帮会界面打开，都有一个绑定的玩家，
	                                                 //或别人，或自己，这就是他(她)的
	                                                 //名字。另一个角度说，每一次打开界面
	                                                 //都绑定一个帮会，而帮会绑定到人

	int           m_nSelectIndex;                    //在列表上选择的项的下标
	int           m_nCurrentCheckBox;                //正在选择的CheckBox

	char          m_szDismiss[64], m_szAssign[64];   //踢人和任命的确认框的字符串
	char          m_szDemise[64], m_szLeave[64];     //传位和离开的确认框的字符串
	char		  m_szRemoveLeagueMember[64];		 //剔除帮会的确认框字符
	char		  m_szLeaveLeague[64];				 //离开联盟的确认框字符
	char		  m_szDismissLeague[64];			 //解散联盟的确认框字符
	char          m_szJustice[16], m_szBalance[16];  //正和中的描述串
	char          m_szEvil[16];                      //邪的描述串
	char          m_szConfirm[16], m_szCancel[16];   //确认窗口上，确认和取消的文字

	char          m_szPassword[32];                  //MD5编码之后的密码串
	char          m_szTargetPlayerName[32];          //操作对象玩家的名字

	unsigned int  m_uStatus;                         //帮会界面的各种状态(Bit Value)
	                                                 //对应于 TONG_MEMBER_STATUS_ 系列define
};

unsigned long StringToHash(const char *pString, BOOL bIsCaseSensitive = TRUE);

#endif // !defined(AFX_UITONGMANAGER_H__13BA213D_11EC_4F24_BF98_F51C3F414D6D__INCLUDED_)
