/*******************************************************************************
File        : UiPortrait.h
Creator     : Fyt(Fan Zhanpeng)
create data : 11-18-2003(mm-dd-yyyy)
Description : 头像选择界面以及相关处理
********************************************************************************/

#if !defined(AFX_UIPORTRAIT_H__A0ED4640_37B6_4731_991A_0E65248B991C__INCLUDED_)
#define AFX_UIPORTRAIT_H__A0ED4640_37B6_4731_991A_0E65248B991C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../elem/wndscrollbar.h"
#include "../elem/wndbutton.h"
#include "../elem/wndimage.h"
#include "../elem/uiimage.h"

class KUiPortrait : public KWndImage
{
public:  //界面标准的对外接口
	KUiPortrait();
	virtual ~KUiPortrait();

	static        KUiPortrait* OpenWindow();		//打开窗口
	static        KUiPortrait* GetIfVisible();		//如果窗口正被显示，则返回实例指针
	static void   CloseWindow(bool bDestory = TRUE);//关闭窗口，同时可以选则是否删除对象实例
	static void   LoadScheme(const char* pScheme);	//载入界面方案

private: //界面标准的内部处理方法
	void          Initialize();						//初始化界面

													//处理界面消息的处理方法
	virtual int   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

	void		  PaintWindow();					//绘画界面的方法

private: //界面需要的静态变量
	static KUiPortrait*		ms_pSelf;

public:  //类自有的结构、媒举定义
	typedef struct KPortraitParam
	{
		int nPortraitCount;
		int nPortraitWidth;
		int nPortraitHeight;
	} PORTRAIT_PARAM;

public:  //类对外的静态接口
	/*你是哪里来的？梦幻般的样子啊，我誓要找到你们的所在，把你们都装进我脑海里*/
	static int		InitializeProtrait();

	/*梦，醒来的时候了吗？真的很可惜，梦中的一切，也只能忘记了吧。。。*/
	static int		ReleasePortrait();

	/*绘画出梦一般色彩的头像:D,传入的坐标是Screen坐标,nbDarkness代表是否变暗*/
	static int		DrawPortrait(int nIndex, int nX, int nY, int nbDarkness = FALSE);

	/*获取梦幻一般的肖像的来源之地*/
	static char*	GetPortraitImagePath(int nPortraitIndex, char *szImage);
	
	/*天神的资料夹，记录了制造的各个梦幻模样，这里可以得知其数量*/
	static int		GetPortraitCount(){return m_PortraitParam.nPortraitCount;};

private:	//类自身的静态数据成员，包括控件类和变量类
	static PORTRAIT_PARAM	m_PortraitParam;
	static KUiImageRef	   *m_pImage;

private: //类自身的方法
	int	   GetIndexAtPos(int nX, int nY);		//通过坐标取得所选择的头像的索引
	int	   UpdateInterface();					//根据玩家的头像更新界面的显示

private:	//控件类数据成员
	KWndButton		m_Confirm;	/*确认的按钮*/
	KWndButton		m_Cancel;	/*取消的按钮*/
	KWndScrollBar	m_ScrollBar;/*滚动条*/

private:	//变量数据成员
	int			m_nColumnCount;	/*头像列表列数*/
	int			m_nLineCount;	/*头像列表行数*/
	int			m_nSpacingH;	/*头像列间的间隔*/
	int			m_nSpacingV;	/*头像行间的间隔*/
	int			m_nFirstLine;	/*正在显示的第一行是哪一行*/
	int			m_nSelectIndex; /*选择了的头像的索引*/
	POINT		m_LTPosition;	/*头像列表的左上角，相对于界面窗口来说*/
};

#endif // !defined(AFX_UIPORTRAIT_H__A0ED4640_37B6_4731_991A_0E65248B991C__INCLUDED_)
