/*****************************************************************************************
//	界面--小地图
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-4-21
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once
#include "LinkStruct.h"
#include "../Elem/WndImage.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndText.h"
#include "../Elem/WndShadow.h"
#include "../Elem/PopupMenu.h"
#include "../elem/wndpuretextbtn.h"
#include "../../Core/Src/GameDataDef.h"

//小地图的显示内容模式
enum MINIMAP_MODE
{
	MINIMAP_M_BRIEF_PIC = 0,			//缩略图
	MINIMAP_M_BRIEF_PIC_BROWSE,			//缩略图浏览/放大模式
	MINIMAP_M_WORLD_MAP,				//世界地图

	//MINIMAP_M_OBSTACLE,				//障碍图模式
	MINIMAP_M_NONE,					    //不显示地图

	MINIMAP_M_COUNT,
};

struct KUiSceneTimeInfo;

struct KMinimapTraffic
{
	POINT	TrafficPoint;
	POINT	TargetPoint;
	int		TargetIndex;
	KMinimapTraffic *previous;
	KMinimapTraffic *next;
};

class KUiMiniMap : public KWndImage
{
public:
	//----界面面板统一的接口函数----
	static KUiMiniMap*	OpenWindow();					//打开窗口，返回唯一的一个类对象实例
	static KUiMiniMap*	GetIfVisible();					//如果窗口正被显示，则返回实例指针
	static void			CloseWindow();					//关闭窗口

	static void			SetMode(MINIMAP_MODE eMode);	//设置显示内容模式
	static void			LoadScheme(const char* pScheme);//载入界面方案
	static void			UpdateSceneTimeInfo(KUiSceneTimeInfo* pInfo);

	static void			HandleMapInfo(int nIndex);		//设置当前角色的真身所在
														//的地域的大五行推断值(编号)

	//设置该大地图显不显示详细小地图
	static void         SetIsShowMapDetail(int nbIsDetail);

	//把小地图设回正确的地图和坐标
	static void			RestoreMapPos();

	static KUiMiniMap*	GetSelf() {return ms_pSelf;}

	void	Show();										//显示窗口
	void	Hide();										//隐藏窗口

private:
	KUiMiniMap();
	~KUiMiniMap() {}
	void	Initialize();
	void	PaintWindow();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	LoadScheme(KIniFile* pIni);		//载入界面方案
	void	SetStatus(bool bSmall);			//切换状态
	void    MapScroll(int nbScrollScene);	//地图卷动
	void	StopScrollMap();				//停止拖动地图
	void	MapMoveBack();					//地图还原原始偏移
	virtual void	Breathe();

	//功能：小地图区域子窗口左上角为0点，把当前的鼠标光标坐标转换为在整张小地图中的象素坐标
	//		参数为in out
	int     PosToPhotoPos(int *pX, int *pY);

	int		DoTransfer();					//对小地图施展移形换影大法，传入的为鼠标
											//在窗口上的位置，要转换为在小地图区域上
											//的位置

	int		HandleCursor(int nX, int nY);	//处理鼠标移动和HOVER，传入的为鼠标
											//在窗口上的位置，要转换为在小地图区域上
											//的位置

	//通过小地图窗口坐标计算以小地图区域子窗口左上角为0点的坐标
	//值为象素，参数为in out
	int		GetCursorPosOnMap(int *pX, int *pY);

	void	LoadTrafficData();				//在掌管天地五行推断值的天书上边
											//拿到意志所在地域的五行推断值
											//(载入交通数据)

	void	ReleaseTrafficData();			//重置交通数据

	void	SetMapFocus(int nX, int nY);	//设置小地图的焦点

	void	SetFlag(int nX, int nY);		//设置小旗

private:
	static KUiMiniMap*	ms_pSelf;
	static BOOL         m_bIsShowMapDetail; // 是否显示详细小地图的旗标
	static int			m_nRealMapIndex;	// 当角色对小地图使用了移形换影
											// 大法，他的真身所在的地域的
											// 大五行推断数值(Index编号)

	static int			m_nCurrentMapIndex; // 当前意志所在区域的大五行数值(Index)
	static int			m_nbInTrafficMode;	// 是否进入了小地图拖动模式

public:
	enum enumMINIMAP_CTRL_MODE
	{
		enumMCM_SCROLL = 1,
		enumMCM_FLAG = 2,
		enumMCM_WAITING_TO_SET_BACK = 4,
	};

private:
	unsigned int	m_uShowElem;
	POINT			m_MapPos;
	SIZE			m_MapSize;
	KWndButton		m_SwitchBtn;
	KWndButton		m_BtnFlag;
	KWndText32		m_SceneName;
	KWndText32		m_SceneNameBG;
	KWndText32		m_ScenePos;
	KWndShadow		m_Shadow;
	KWndButton		m_WorldMapBtn;

	POINT           m_OldPos;
	POINT			m_FlagPos;
	unsigned int	m_uLastScrollTime;             // 离最后一次滚动的时间(秒)
	int             m_nOffsetX;                    // 每次Loop，检测到跟上次鼠标位置的偏移X
	int             m_nOffsetY;                    // 每次Loop，检测到跟上次鼠标位置的偏移Y

	KLinkStruct<KMinimapTraffic>
					m_Traffic;					   // 来自五行的区域传送力数据
												   // (小地图交通数据)

	KMinimapTraffic
				   *m_pTrafficNow;				   // 指向当前鼠标所指的交通数据的指针

	KUiImageRef		m_FlagImage;				   // 旗子的图形引用

	int				m_nScaleH, m_nScaleV;		   // 横向和纵向比例
	
	int				m_nDefaultCursor;			   // 鼠标进入小地图前的鼠标状态

	int				m_nCtrlMode;				   // 小地图处在一个什么样的操作模式
												   // 位变量
};

void	MapToggleStatus();
void	MapSetMode(MINIMAP_MODE eMode);	//设置显示内容模式