/*****************************************************************************************
//	浮动提示窗口
//	Copyright : Kingsoft 2002-2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-12-23
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "WndMessage.h"
#include "../../Public/KRepresentUnit.h"
#include "../../Public/GameDataDef.h"
#define	GOD_MAX_OBJ_TITLE_LEN	1024	//128临时改为1024为了兼容旧代码 to be modified
#define	GOD_MAX_OBJ_PROP_LEN	516
#define	GOD_MAX_OBJ_DESC_LEN	516

class KMouseOver
{
public:
	KMouseOver();
	int		IsMoseHoverWndObj(void* pWnd, int nObj);
	void    SetMouseHoverInfo(void* pWnd, int nObj, int x, int y,
		                        bool bHeadTailImg, bool bFollowCursor);
    void    SetMouseHoverTitle(const char *pcszTitleText, int nTitleTextLen, UINT uColor);
	void    SetMouseHoverProp(const char *pcszPropText, int nPropTextLen, UINT uColor);
	void    SetMouseHoverDesc(const char *pcszDescText, int nDescTextLen, UINT uColor);
	void	CancelMouseHoverInfo();
	void	PaintMouseHoverInfo();
	void	LoadScheme(const char* pScheme);		//载入界面方案
	void	UpdateCursorPos(int nX, int nY);		//鼠标的位置更新了
	void	OnWndClosed(void* pWnd);
private:
	void	Update(int nX, int nY);
private:
	int		m_nImgWidth;	        //图形水平长度
	int		m_nImgHeight;	        //图形垂直高度
	int		m_nLeft;		        //窗口左上角横坐标
	int		m_nTop;			        //窗口左上角纵坐标
	int		m_nWndWidth;	        //动态的窗口水平宽
	int		m_nWndHeight;	        //动态的窗口垂直宽

	int		m_nIndent;		        //文字缩进（与边框的水平相隔距离）
	int		m_nFontSize;	        //文字字体(大小)

	int     m_nApplyX;              //外部提交的浮动窗口的X坐标
	int     m_nApplyY;              //外部提交的浮动窗口的Y坐标
	int     m_nTitleLineNum;        //标题(名字)部分的行数
	int     m_nPropLineNum;         //属性部分的行数
	int     m_nDescLineNum;         //描述部分的行数
	int     m_nMaxLineLen;          //三部分一起算最长的行的长度

	unsigned int m_uTitleBgColor;   //标题(名字)背景颜色
	unsigned int m_uTitleColor;     //标题(名字)文字颜色
	unsigned int m_uPropBgColor;    //属性背景颜色
	unsigned int m_uPropColor;      //属性文字颜色
	unsigned int m_uDescBgColor;    //描述部分背景颜色
	unsigned int m_uDescColor;      //描述部分文字颜色
	KRUImagePart m_Image;	        //边框图形

	void*	m_pMouseHoverWnd;		//鼠标停留的窗口
	int		m_nObj;
	char    m_ObjTitle[GOD_MAX_OBJ_TITLE_LEN];
	char    m_ObjProp[GOD_MAX_OBJ_PROP_LEN];
	char	m_ObjDesc[GOD_MAX_OBJ_DESC_LEN];
	int		m_nTitleLen;
	int		m_nPropLen;
	int		m_nDescLen;
	bool	m_bHeadTailImg;			//是否显示提头与结尾边框图
	bool	m_bFollowCursor;		//是否跟踪鼠标指针的位置，一起移动，（同时没有横宽最短限制）
	BOOL	m_bShow;
};

extern KMouseOver	g_MouseOver;

void SetHoverObjDescColor(unsigned int uColor);
void SetMouseHoverObjectDesc(void* pWnd, int nObj, unsigned int uGenre,
			unsigned int uId, int nContainer, int x, int y);
//绘制被拖动游戏对象的函数
int DrawDraggingGameObjFunc(int x, int y, const KUiDraggedObject& Obj, int nDropQueryResult);
