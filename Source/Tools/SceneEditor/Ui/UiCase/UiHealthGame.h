/*******************************************************************************
File        : UiHealthGame.h
Creator     : Fyt(Fan Zhanpeng)
create data : 11-21-2003(mm-dd-yyyy)
Description : 健康游戏忠告
********************************************************************************/

#if !defined(AFX_UIHEALTHGAME_H__A667B514_B34E_4F6B_8E06_74065B61B5CB__INCLUDED_)
#define AFX_UIHEALTHGAME_H__A667B514_B34E_4F6B_8E06_74065B61B5CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../elem/wndtext.h"
#include "../elem/wndimage.h"
#include "../elem/wndbutton.h"
#include "../Elem/WndShowAnimate.h"

class KUiHealthGame : public KWndShowAnimate
{
public:  //界面标准的对外接口
	KUiHealthGame();
	virtual ~KUiHealthGame();

	static        KUiHealthGame* OpenWindow();		//打开窗口
	static        KUiHealthGame* GetIfVisible();	//如果窗口正被显示，则返回实例指针
	static void   CloseWindow(bool bDestory = TRUE);//关闭窗口，同时可以选则是否删除对象实例
	static void   LoadScheme(const char* pScheme);	//载入界面方案

private: //界面标准的内部处理方法
	void          Initialize();						//初始化界面

													//处理界面消息的处理方法
	virtual int   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

private: //界面需要的静态变量
	static KUiHealthGame*	ms_pSelf;

private:	//控件类数据成员
	KWndButton				m_BtnClose;
	KWndText256				m_Text;
};

#endif // !defined(AFX_UIHEALTHGAME_H__A667B514_B34E_4F6B_8E06_74065B61B5CB__INCLUDED_)
