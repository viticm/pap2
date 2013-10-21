/*****************************************************************************************
//	界面窗口体系结构--按键窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-22
------------------------------------------------------------------------------------------
	按钮窗口。
	按钮可以包括以下特性：按钮按下与未被按下显示不同的图形；指针移动到按钮上方时显示动画；
	每点击一次按钮切换一次按钮的是否按下状态(CheckBox)。
*****************************************************************************************/
#pragma once
#include "WndImage.h"

#define	WNDBTN_ES_FILTER		0x00ff
#define	WNDBTN_ES_ANIMATION		0x0001		//鼠标指针停留在窗口上方时显示动画
#define	WNDBTN_ES_CHECKBOX		0x0002		//切换状态的按扭
#define	WNDBTN_ES_SEND_HOLD_MSG	0x0004		//发送鼠标被按住不放的消息
#define	WNDBTN_ES_NO_OVERSOUND	0x0008		//鼠标指针移到上方时，不播放声音效果

#define	WNDBTN_F_DOWN		0x0200			//按纽被按下
#define	WNDBTN_F_CHECKED	WNDBTN_F_DOWN	//按纽被选中
#define	WNDBTN_F_OVER		0x0400			//鼠标停留在窗口上方

class KWndButton : public KWndImage
{
protected:
	unsigned short	m_Flag;
private:
	short			m_nUpFrame;
	short			m_nDownFrame;
	short			m_nCheckOverFrame;
	short			m_nOverStartFrame;
	short			m_nDisableFrame;
	static	KWndButton* m_pPressedDownBtn;	//被点击但是没有被释放的按钮
	char			m_szTip[64];
	int				m_nTipLen;
public:
	KWndButton();
	virtual int		Init(KIniFile* pIniFile, const char* pSection);//初始化
	virtual const char*	GetShortKey() {return NULL;}	//取快捷键,用于Tip显示
	virtual int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	virtual void	PaintWindow();							//窗体绘制
	int				IsButtonChecked();						//按钮是否出于按下状态
	int				IsButtonActive();
	void			CheckButton(int bChecked);				//设置按钮的按下状态
	void			Enable(int bEnable);					//禁止或者允许使窗口被操作
	void			Clone(KWndButton* pCopy);
	static void		SetAllButtonTipTextColor(unsigned int uColor);	//设置所有按钮提示名称文字的颜色
	static void		EnableAllButtonTip(int bEnable);					//禁止/允许所有按钮的提示文字
	virtual void	OnButtonClick(){}
	virtual int		GetToolTipInfo(char* szTip, int nMax);
	int		SetToolTipInfo(char* szTip, int nMax);

private:
	void			OnLBtnDown(bool bDoubleClick);	//响应鼠标左键在此按下
	void			OnLBtnUp();						//响应鼠标左键在此放开
	void			OnLBtnDownMove();				//响应鼠标左键按下时的移动
private:
	static unsigned int	ms_uBtnTipTextColor;	//按钮提示名称文字的颜色
	static int			ms_nDisableBtnTip;		//是否禁止按钮的提示文字
};

#include "WndText.h"
#include "WndMovingImage.h"
#include "WndImagePart.h"

class KWndImageTextButton : public KWndButton
{
protected:
	KWndMovingImage m_Image;
	KWndImagePart m_ImagePart;
	KWndText32 m_Text;
	BOOL bPart;
public:
	KWndImageTextButton();
	virtual int		Init(KIniFile* pIniFile, const char* pSection);//初始化
	void			Set2IntText(int nNumber1, int nNumber2, char Separator);
	void			SetIntText(int nNumber, char Separator);
	void			Set2IntValue(int nNumber1, int nNumber2);
	void			SetText(const char* pText, int nLen = -1);	//设置文本文字
	int				GetText(char* pBuffer, int nSize);		//获取字符串内容
};
