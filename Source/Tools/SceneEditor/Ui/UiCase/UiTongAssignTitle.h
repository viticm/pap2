/*******************************************************************************
File        : UiTongAssignTitle.h
Creator     : Fyt(Fan Zhanpeng)
create data : 10-31-2003(mm-dd-yyyy)
Description : 管理帮会的界面
********************************************************************************/

#if !defined(AFX_UITONGASSIGNTITLE_H__2F4CCA33_DF93_4DED_AFFE_CB9535A861A6__INCLUDED_)
#define AFX_UITONGASSIGNTITLE_H__2F4CCA33_DF93_4DED_AFFE_CB9535A861A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../elem/wndpuretextbtn.h"
#include "../elem/WndWindow.h"
#include "../elem/wndbutton.h"
#include "../elem/wndimage.h"
#include "../elem/wndedit.h"
#include "GameDataDef.h"

class KUiTongAssignTitle : public KWndImage
{
public:
	KUiTongAssignTitle();
	virtual ~KUiTongAssignTitle();

	static        KUiTongAssignTitle* OpenWindow();  //打开窗口
	static        KUiTongAssignTitle* GetIfVisible();//如果窗口正被显示，则返回实例指针
	static void   CloseWindow(bool bDestory = TRUE); //关闭窗口，同时可以选则是否删除对象实例
	static void   LoadScheme(const char* pScheme);   //载入界面方案

public:
	//窗口函数，处理这个界面的消息
	virtual int   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

	//把数据注入到这个界面中
	void          InjectData(KTongMemberItem *pData, int nDataCount, int nFigure, int nSelect = 0);
	
	//更新界面显示数据
	void          UpdateView();

private:
	void          Initialize();                      //初始化界面
	void          PopupSelectMenu(int nX, int nY);   //弹出目标对象选择框

private:
	enum RESULT
	{
		RESULT_SELECT_TARGET_MENU,
	};

private:
	static KUiTongAssignTitle* ms_pSelf;             //指向自己这个对象的指针

private:
	KWndPureTextBtn            m_BtnTarget;          //写对象名字的域，可以点击选其他对象
	KWndEdit32                 m_EditTitle;          //改称号的时候写称号的地方

	KWndButton                                       //确定、取消按钮
		                       m_BtnConfirm, m_BtnCancel;

private:
	KTongMemberItem*           m_pData;              //搭载数据的内存的指针，内存需要分配
	int                        m_nDataCount;         //数据的项数
	KTongMemberItem            m_SelectData;         //当前选择的对象的数据
	int                        m_nSelected;          //当前选择对象的位置咯

	int                        m_nFigure;            //具体哪一个职位的称号群
};

#endif // !defined(AFX_UITONGASSIGNTITLE_H__2F4CCA33_DF93_4DED_AFFE_CB9535A861A6__INCLUDED_)
