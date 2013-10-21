/* 
 * File:     KUiHelper.h
 * Desc:     简单帮助界面
 * Author:   flying
 * Creation: 2003/7/16
 */
//-----------------------------------------------------------------------------
#pragma once

#if !defined _UIHELPER1
#define _UIHELPER1

#include "../Elem/WndImage.h"

class KUiHelper : protected KWndImage
{
public:
	//打开窗口，返回唯一的一个类对象实例
	static KUiHelper* OpenWindow(bool bShow);
	//关闭窗口，同时可以选则是否删除对象实例
	static void				CloseWindow(bool bDestroy);
	static KUiHelper*		GetIfVisible();
	//载入界面方案
	void					LoadScheme(const char* pScheme);

	static void SelectInterfacePic(); //选择要显示界面按钮的帮助图
	static void SelectKeyboardPic();  //选择要显示快捷键帮助图，
	static void SelectWuxingPic();    //选择要显示五行激活图的图片

private:
	KUiHelper();
	virtual ~KUiHelper();

	static KUiHelper* m_pSelf;
	static int ms_nImgFrame;
	int					WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

	void Initialize();
	void	Breathe() {NextFrame();}
	virtual void	Show();									//显示窗口
};


#endif