/*****************************************************************************************
//	界面窗口体系结构--列表窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-11
------------------------------------------------------------------------------------------
    列表窗口只列出文字。但是每项都可以关联其他数据。
*****************************************************************************************/
#pragma once
#include "WndWindow.h"
#include "WndScrollBar.h"

#define	WNDLIST2_ES_HIGHLIGHT_ENABLE		0x0001			//以不同颜色显示鼠标指针指向的项
#define	WNDLIST_ERROR					-1

class KWndList2 : public KWndWindow
{
public:
	virtual int	Init(KIniFile* pIniFile, const char* pSection);//初始化
	virtual int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	virtual void	PaintWindow();							//绘制窗口
	int				AddString(int nIndex, const char* pString);//添加项
	int				GetString(int nIndex, char* pBuffer, unsigned int nSize);//获取列表项的字符串内容
	int				DeleteString(int nIndex);				//删除列表中的一项
	void			ResetContent();							//删除列表中全部的内容
	//在列表中查找某个字符串。
	int				FindString(int nPrecedingStart, const char* pString);
	int				SetItemData(int nIndex, int nData);		//设置列表项关联数据的值
	int				GetItemData(int nIndex);				//获得列表项关联数据的值
	void			SetTopItemIndex(int nTopItemIndex);		//获得列表窗口顶部显示的数据项的索引
	int				GetVisibleLineCount() const;			//获得列表框可以同时显示的项的数目
	int				GetCount()  { return m_nNumItem; }		//获取数据项的总数目
	int				GetCurSel() { return m_nSelItemIndex;  }//得到当前选择的数据项的索引
	int				SetCurSel(int nSel);
	void			SetScrollbar(KWndScrollBar* pScroll);
	KWndList2();
	virtual ~KWndList2();
protected:
	void			Clone(KWndList2* pCopy);
private:
	void			OnLButtonDown(int x, int y);			//响应鼠标左键按下
	void			OnMouseMove(int x, int y);				//响应鼠标在列表窗口上移动
	
private:
	void**			m_pContent;				//列表项内容数据
	int				m_nItemSpace;			//可以保存的item的数目
	int				m_nNumItem;				//数据项的数目

	int				m_nTopItemIndex;		//窗口顶部显示的数据项的索引
	int				m_nSelItemIndex;		//选中的数据项的索引
	int				m_nHighLightItemIndex;	//高亮度显示（鼠标指针下方）的数据项的索引

	unsigned int	m_ItemColor;			//显示数据项文本的颜色
	unsigned int	m_ItemBorderColor;		//显示数据项文本的边缘颜色
	unsigned int	m_SelItemColor;			//被选中的数据项文本的颜色
	unsigned int	m_SelItemBorderColor;	//被选中的数据项文本的边缘颜色
	unsigned int	m_HighLightColor;		//高亮度显示的数据项文本的颜色
	unsigned int	m_HighLightBorderColor;	//高亮度显示的数据项文本的边缘颜色
	int				m_nFontSize;			//显示字体大小

	KWndScrollBar  *m_pScrollbar;
};