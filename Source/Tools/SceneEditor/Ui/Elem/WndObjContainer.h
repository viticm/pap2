/*****************************************************************************************
//	界面窗口体系结构--容纳游戏对象的窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-25
*****************************************************************************************/
#pragma once

#include "Windows.h"
#include "WndWindow.h"
#include "../Elem/WndMessage.h"

#define	OBJCONT_S_ENABLE_CLICK_EMPTY	0x00008000
#define	OBJCONT_S_ACCEPT_FREE			0x00004000
#define	OBJCONT_S_HAVEOBJBGCOLOR		0x00002000	//放有物品时是否有背景色
#define	OBJCONT_S_TRACE_PUT_POS			0x00001000
#define	OBJCONT_S_DISABLE_PICKPUT		0x00000800	//不允许拿起东西
#define	OBJCONT_F_MOUSE_HOVER			0x00000400

//============================
//	单个对象的容器窗口
//============================
class KWndObjectBox : public KWndWindow
{
public:
	KWndObjectBox();
	virtual int		Init(KIniFile* pIniFile, const char* pSection);	//初始化
	void	LoadScheme(const char* pScheme);			///载入界面方案
	void	Celar();									//清除对象物品
	void	SetObjectGenre(unsigned int uGenre);		//设置可以容纳的对象的类型
	int		GetObject(KUiDraggedObject& Obj) const;		//获取容纳的对象信息
	void	HoldObject(unsigned int uGenre, unsigned int uId, int nDataW, int nDataH);//设置容纳的对象
	void	Clone(KWndObjectBox* pCopy);
	void	SetContainerId(int nId);
	void	EnablePickPut(bool bEnable);
protected:
	virtual int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	int		DropObject(bool bTestOnly);				//放置物品
	void	PaintWindow();							//窗体绘制
	unsigned int		m_uAcceptableGenre;			//可接纳的对象类型
	KUiDraggedObject	m_Object;
	int					m_nContainerId;
};

//============================
//	容纳多个对象的容器窗口
//============================
class KWndObjectMatrix : public KWndWindow
{
public:
	KWndObjectMatrix();
	virtual ~KWndObjectMatrix();
	virtual int		Init(KIniFile* pIniFile, const char* pSection);	//初始化
	void			Clear();									//清除全部的对象物品
	int				AddObject(KUiDraggedObject* pObject, int nCount);	//增加对象物品
	int				RemoveObject(KUiDraggedObject* pOjbect);			//减少一个对象物品
	int				GetObject(KUiDraggedObject& Obj, int x, int y) const;//获取容纳的某个对象信息
//	int				GetObjects(KUiDraggedObject* pObjects, int nCount) const;//获取容纳的对象信息
	void			EnableTracePutPos(bool bEnable);
	void			SetContainerId(int nId);
	void			EnablePickPut(bool bEnable);
	int				GetGridInfo(int &nWidth, int &nHeight);
protected:
	void			Clone(KWndObjectMatrix* pCopy);
private:
	virtual int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);	//窗口函数
	void	PaintWindow();										//窗体绘制
	int		GetObjectAt(int x, int y);							//获得某个位置上的物品的索引
	int		PickUpObjectAt(int x, int y);						//捡起某个位置上的对象
	int		DropObject(int x, int y, bool bTestOnly);			//放置物品
	int		TryDropObjAtPos(const RECT& dor, KUiDraggedObject*& pOverlaped);//尝试放置物品
	void	DropObject(int x, int y, KUiDraggedObject* pToPickUpObj);		//放下物品

protected:
	int				m_nNumUnitHori;		//横向格数
	int				m_nNUmUnitVert;		//纵向格数
	int				m_nUnitWidth;		//横向格宽
	int				m_nUnitHeight;		//纵向格宽
	int				m_nUnitBorder;		//格子的边框的宽高度
	int				m_nNumObjects;		//容纳的对象的数目
	KUiDraggedObject* m_pObjects;		//容纳的对象列表
	int				m_nMouseOverObj;

	int				m_nPutPosX;
	int				m_nPutPosY;
	int				m_nPutWidth;
	int				m_nPutHeight;
	int				m_nContainerId;
};

void WndObjContainerInit(KIniFile* pIni);