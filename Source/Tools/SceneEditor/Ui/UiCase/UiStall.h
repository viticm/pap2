/* 
 * File:     UiStall.h
 * Desc:     嗷嗷摆摊ing
 * Author:   flying
 * Creation: 2003/10/23
 */
#if !defined _UISTALL_HEADER
#define _UISTALL_HEADER

#pragma once

#include "../elem/WndButton.h"
#include "../elem/WndText.h"
#include "../elem/WndObjContainer.h"
#include "../Elem/WndShowAnimate.h"

struct KUiObjAtRegion;

class KUiStall : public KWndShowAnimate
{
public:
	static KUiStall* OpenWindow();				//打开窗口，返回唯一的一个类对象实例
	static KUiStall* GetIfVisible();				//如果窗口正被显示，则返回实例指针
	static void		CloseWindow(bool bDestroy);	//关闭窗口，同时可以选则是否删除对象实例
	static void		LoadScheme(const char* pScheme);//载入界面方案
	void			RemoveItem(KUiObjAtRegion* pItem);//物品变化更新
	void			Breathe();									//活动函数

private:
	KUiStall();
	virtual ~KUiStall() {}
	void	Initialize();							//初始化
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	void	UpdateData();
	void	OnClickItem(KUiDraggedObject* pItem);
private:
	static	KUiStall*	m_pSelf;
private:
	KWndButton			m_CloseBtn;
	KWndObjectMatrix	m_ItemBox;
	KWndText256			m_Owner;

	//界面状态句柄
    int					m_nStatusHandle;
};

#endif