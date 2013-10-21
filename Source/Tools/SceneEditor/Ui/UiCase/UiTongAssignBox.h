/*******************************************************************************
File        : UiTongAssignBox.h
Creator     : Fyt(Fan Zhanpeng)
create data : 08-29-2003(mm-dd-yyyy)
Description : 任命帮会职位的界面
********************************************************************************/

#if !defined(AFX_UITONGASSIGNBOX_H__1D36E55C_C9D4_44AF_8E20_0FF51BDB2BE0__INCLUDED_)
#define AFX_UITONGASSIGNBOX_H__1D36E55C_C9D4_44AF_8E20_0FF51BDB2BE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../elem/wndlabeledbutton.h"
#include "../elem/WndPureTextBtn.h"
#include "../elem/wndlist2.h"
#include "../elem/wndimage.h"
#include "../elem/wndtext.h"

#include "GameDataDef.h"

class KUiTongAssignBox : public KWndImage
{
public:
	KUiTongAssignBox();
	virtual ~KUiTongAssignBox();

	static        KUiTongAssignBox* OpenWindow();   //打开窗口
	static        KUiTongAssignBox* GetIfVisible(); //如果窗口正被显示，则返回实例指针
	static void   CloseWindow(bool bDestory = TRUE);//关闭窗口，同时可以选则是否删除对象实例
	static void   LoadScheme(const char* pScheme);  //载入界面方案

public:
	static void   LinkToMainWindow(                 //建立这个界面和主界面的关联
		                      KWndWindow *pMain);

	static void   ArrangeData(                      //为任命案的书写安排数据
		                      int nType, char* szTarName, char* szTarAgName, int nCurrentFigure, int nCurrentPosition = 0);

	virtual int   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

	                                                //根据nType插入数据
	void          InjectData(KTongMemberItem *pData, int nDataCount, int nType);

	
private:
	static        KUiTongAssignBox* ms_pSelf;

private:
	void          PopupConfirmWindow(               //弹出确认窗口
		                        const char* pszInfo, unsigned int uHandleID);

	void          UpdateCheckButton(int nFigure);   //Check管理函数
	void          Initialize();
	void          OnConfirm();                      //响应确认操作
	void          UpdateAssignList(int nType);      //更新职位列表

													//判断一个位置是否已经有人
	int			  IsPlaceEmpty(int nFigure, int nPlace);

private:
	enum RESULT_TYPE
	{
		RESULT_T_ASSIGN,
	};

private:
	KWndPureTextBtn          m_BtnElder;
	KWndPureTextBtn          m_BtnCaptain;
	KWndPureTextBtn          m_BtnPilgarlic;
	KWndPureTextBtn          m_BtnConfirm;
	KWndPureTextBtn          m_BtnCancel;

	KWndText80               m_TargetName;
	KWndText80               m_TextError;

	KWndWindow              *m_pMain;

	KWndList2                m_AssignPlace;
	KWndScrollBar            m_ScrollBar;

private:
	int                      m_nType, m_nCurrentFigure, m_nCurrentPostion;
	char                     m_szTargetPlayerName[32], m_szTargetPlayerAgName[32];

	char                     m_szAssign[64], m_szConfirm[8], m_szCancel[8];
	int                      m_nSelectFigure;

	char                     m_szErrorNotSelectFigure[32];
	char                     m_szErrorNotSelectPlace[32];
	char					 m_szIsNotEmpty[32];

	KTongMemberItem         *m_pDirectorData;
	int                      m_nDirectorDataCount;
	KTongMemberItem         *m_pManagerData;
	int                      m_nManagerDataCount;
};

#endif // !defined(AFX_UITONGASSIGNBOX_H__1D36E55C_C9D4_44AF_8E20_0FF51BDB2BE0__INCLUDED_)
