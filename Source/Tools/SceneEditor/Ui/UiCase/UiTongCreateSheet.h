/*******************************************************************************
File        : UiTongCreateSheet.h
Creator     : Fyt(Fan Zhanpeng)
create data : 08-29-2003(mm-dd-yyyy)
Description : 创建帮会的表单
********************************************************************************/


#if !defined(AFX_KUITONGCREATESHEET_H__7CC8F62F_9A1C_4AE2_A73B_BC945DE5185F__INCLUDED_)
#define AFX_KUITONGCREATESHEET_H__7CC8F62F_9A1C_4AE2_A73B_BC945DE5185F__INCLUDED_

/*---------------------------*/
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*------------------------------------------*/
#include "../elem/wndtext.h"
#include "../elem/wndedit.h"
#include "../elem/wndimage.h"
#include "../elem/wndbutton.h"
#include "../elem/wndlabeledbutton.h"

/*---------------------------------------------------------*/

class KUiTongCreateSheet : KWndImage
{
public:
	KUiTongCreateSheet();
	virtual ~KUiTongCreateSheet();

	static        KUiTongCreateSheet* OpenWindow();  //打开窗口
	static        KUiTongCreateSheet* GetIfVisible();//如果窗口正被显示，则返回实例指针
	static void   CloseWindow(bool bDestory = TRUE); //关闭窗口，同时可以选则是否删除对象实例
	static void   LoadScheme(const char* pScheme);   //载入界面方案

public:
	void          SetUsage(int nUsage);              //设置界面用途

	//导入预设值
	void          InjectData(const char *szName, int nCamp);

private:
	static        KUiTongCreateSheet *ms_pSelf;

private:
	void          Initialize();               //初始化
	                      /*------------------------------------------------*/
	                                          //窗口函数
	virtual int   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

private:
	void          AlignmentButtonCheck(       //正中邪CheckBox的管理函数
		                               NPCCAMP eSide);

	void          OnDone();                   //响应确定按钮被按下

private:
	KWndText32    m_TextError;                //当按下确定按钮的时候，哪些地方没填好

	KWndEdit32    m_EditTongName;             //帮会的名字
	KWndButton    m_BtnOrder, m_BtnNatural;   //正中邪(在DnD文化里面，Order代表秩序、
	KWndButton    m_BtnChaos;                 //       Natural代表中立、Chaos代表混乱)

    KWndLabeledButton
		          m_BtnDone, m_BtnCancel;     //确定/取消按钮

	int           m_nSelectSide;              //选择了哪一个阵营
	int           m_nUsage;					  //界面的作用，创建，改名字还是改阵营，见
											  //enumCREATE_SHEET_USAGE

	int			  m_nOldAlignment;			  //旧的阵营

	char          m_szNameNullString[32];     //没有填名字的提示字符串
	char          m_szAlignmentNullString[32];//没有选阵营的提示字符串

	//修改阵营时的提示字符串
	char		  m_szChangeAlignmentString[32];

	//修改名字时的提示字符串
	char		  m_szChangeNameString[32];
};


#endif // !defined(AFX_KUITONGCREATESHEET_H__7CC8F62F_9A1C_4AE2_A73B_BC945DE5185F__INCLUDED_)
