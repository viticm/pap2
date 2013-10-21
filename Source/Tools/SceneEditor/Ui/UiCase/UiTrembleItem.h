/*******************************************************************************
File        : UiTrembleItem.h
Creator     : Fyt(Fan Zhanpeng)
create data : 09-08-2003(mm-dd-yyyy)
Description : 打造武器的界面
********************************************************************************/

#if !defined(AFX_UITREMBLEITEM_H__69079BE2_10C3_4AA0_A3C2_190285964E36__INCLUDED_)
#define AFX_UITREMBLEITEM_H__69079BE2_10C3_4AA0_A3C2_190285964E36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../elem/wndobjcontainer.h"
#include "../elem/wndvalueimage.h"
#include "../elem/wndbutton.h"
#include "../elem/wndimage.h"

class KCanGetNumImage : public KWndValueImage
{
public:
	int GetMaxFrame();
	int GetCurrentFrame();
};

class KUiTrembleItem : public KWndImage
{
public:
	KUiTrembleItem();
	virtual ~KUiTrembleItem();

	static        KUiTrembleItem* OpenWindow();      //打开窗口
	static        KUiTrembleItem* GetIfVisible();    //如果窗口正被显示，则返回实例指针
	static void   CloseWindow(bool bDestory = TRUE); //关闭窗口，同时可以选则是否删除对象实例
	static void   LoadScheme(const char* pScheme);   //载入界面方案

	//更新界面显示
	void		  UpdateView(KUiObjAtRegion *pItem, int nbAdd);
	void		  Clear();

private:
	static        KUiTrembleItem *ms_pSelf;

private:
	void          Initialize();
	virtual int   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	virtual void  Breathe();

private:
	void          ApplyAssemble();                   //向Core发送打造请求和数据
	void          ResultReturn();                    //打造结果返回
	int           PlayEffect();                      //播放特效
	int           UpdateResult();                    //把结果更新到界面上
	void		  OnConfirm();						 //响应确定的操作
	void          OnPickDropItem(                    //响应宝石的拿起放下
		                         ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);

	//检查其他位置是否已经防置宝石
	int			  CheckIsOtherGemAlreadyPut(KWndWindow *pWhere);
	
	//界面数据合法性检查
	int			  CheckValid(KUiBesetOperationParam *pParam);
	void		  Lock(BOOL bLock);					 //是否锁住界面(不响应任何操作)

private:
	enum THIS_INTERFACE_STATUS
	{
		STATUS_WAITING_MATERIALS,
		STATUS_BEGIN_TREMBLE,
		STATUS_PLAYING_EFFECT,
		STATUS_TREMBLING,
		STATUS_CHANGING_ITEM,
		STATUS_FINISH,
	};

private:
	KWndObjectBox m_Item[UITP_NUM];					 //宝石和物品

	KWndButton    m_Confirm;                         //合成按钮
	KWndButton    m_Cancel;                          //关闭按钮

	KCanGetNumImage
		          m_TrembleEffect;                   //合成中的特效

private:
	int           m_nStatus;                         //当前界面所处的状态

	//界面状态句柄(这个指UI状态，不是指这个界面的状态)
	int			  m_nStatusHandle;

private:
	char		  m_szErrorNoItem[64];				 //请放上装备 错误串
	char		  m_szErrorNoGem[64];				 //请放上至少一颗宝石 错误串
	char		  m_szErrorOnlyOne[64];				 //五行宝石只能够放一颗 错误串
};

#endif // !defined(AFX_UITREMBLEITEM_H__69079BE2_10C3_4AA0_A3C2_190285964E36__INCLUDED_)
