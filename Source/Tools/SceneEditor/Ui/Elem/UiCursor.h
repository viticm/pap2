/*****************************************************************************************
//	鼠标指针窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-8-17
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once
#include "UiImage.h"

#define KUI_USE_HARDWARE_MOUSE  1
//#undef  KUI_USE_HARDWARE_MOUSE

#define	MAX_CURSOR_IMAGE	16

class KUiCursor
{
public:
	void	Show(int bShow);								//设置鼠标指针的显示状态
	void	SetPosition(int h, int v);						//设置鼠标指针位置
	void	GetPosition(int& h, int& v);					//获取鼠标指针位置
	void	SetImage(int nIndex, const char* pImgFile);		//载入鼠标指针图形
	int		SwitchImage(int nIndex);						//切换当前鼠标指针图形
	int		GetCursorImageIndex(){return m_nCurImage;}		//得到当前鼠标图形的索引
	void	RestoreCursor();	        					//重新激活鼠标
	void	Paint();										//绘制鼠标指针
	void	Cleanup();										//清空全部资源
	KUiCursor();
	~KUiCursor();
private:
	int			m_bShow;
	int			m_nCurImage;
	int			m_nPositionH;
	int			m_nPositionV;

    #ifdef KUI_USE_HARDWARE_MOUSE
	    HCURSOR     m_CursorImages[MAX_CURSOR_IMAGE];
    #else // KUI_USE_HARDWARE_MOUSE
		KUiImageRef	m_CursorImages[MAX_CURSOR_IMAGE];
		int			m_nHotspotH;
		int			m_nHotspotV;
    #endif
};
