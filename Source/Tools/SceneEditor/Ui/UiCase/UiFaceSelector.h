/* 
 * File:     UiFaceSelector.h
 * Desc:     表情选择窗口
 * Author:   flying
 * Creation: 2003/7/8
 * 今天是偶老婆的生日哦，呵呵
 */
//-----------------------------------------------------------------------------
#pragma once

#if !defined _FACESELECTOR
#define _FACESELECTOR

#include "../Elem/WndButton.h"

class KUiFaceSelector : public KWndWindow
{
public:
	//打开窗口，返回唯一的一个类对象实例
	static KUiFaceSelector* OpenWindow(KWndWindow* pCaller, void* pvParam);
	//关闭窗口，同时可以选则是否删除对象实例
	static void				CloseWindow(bool bDestroy);
	static KUiFaceSelector*	GetIfVisible();
	void					LoadScheme(const char* pScheme);	//载入界面方案

	static void				LoadFaceList();
	static void				Clear();

	static int				ConvertFaceText(char* pDest, const char* pSrc, int nCount);
	
private:
	KUiFaceSelector();
	virtual		~KUiFaceSelector() {}
	static int	ConvertFace(char* pDest, int& nConvertCount, const char* pSrc, int nCount, int& nReadPos);
	void		PaintWindow();
	int			WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void		Show();
	void		Hide();

	void		Initialize();
	int			GetFaceAtPos(int x, int y);
	void		UpdateFaceTip(int x, int y);
	void		OnSelFace(BOOL bClose = TRUE);
	void		OnExpandWnd();
	void		OnUnExpandWnd();

private:
	struct	KFaceItem
	{
		char	szFaceText[32];
		char	szFaceTip[32];
		WORD	wFaceID;
	};
	static KUiFaceSelector* m_pSelf;
	static KFaceItem*	ms_pFaceList;
	static int			ms_nNumFaces;

private:
	KWndButton			m_LessBtn;
	KWndButton			m_MoreBtn;

	int					m_nNormColuCount;
	int                 m_nExpColuCount;
	int					m_nRowCount;

	int                 m_nIndentH, m_nIndentV;
	int					m_nBtnWidth,  m_nBtnHeight;	//表情符号按钮的长宽

	unsigned int		m_nBgColor;
	unsigned int		m_nBgOver;
	unsigned int		m_nBgAlpha;
	unsigned int		m_nBorderColor;

	int					m_nCurrIndex;		//当前选中/指向的表情图标
	BOOL                m_bIsExpandMode;	//是否在扩展状态

	KWndWindow*			m_pCallerWnd;		//调用窗口
	void*				m_pvCallerParam;	//调用窗口调用时的自定义参数
};

#endif