////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : tipcenter.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-2-11 9:52:34
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_TIPCENTER_H_
#define _INCLUDE_TIPCENTER_H_

////////////////////////////////////////////////////////////////////////////////

#include "../Common/tfixedindexrefcountarray.h"

namespace UI
{

enum SHOW_TIP_TYPE
{
	SHOW_TIP_BY_MOUSE_SIDE = 0, //在鼠标旁边显示提示信息
	SHOW_TIP_BY_MOUSE_SIDE_AND_FOLLOW_MOUSE_MOVE = 1, //在鼠标旁，跟随鼠标移动
	SHOW_TIP_IN_A_CHANGELESS_POS = 2, //在一个固定的地点显示提示信息
};

enum { INVALID_TIP_INDEX = -1 };

struct KUITIPDATA   
{
	int nMessageIndex; 
	int nShowTipType;
	int nRichText;
};
bool operator==(KUITIPDATA const& lhs, KUITIPDATA const& rhs);

class KTipCenter
{
private:
	enum { TIP_TABLE_ITEM_COUNT = 1024 };
	typedef TFixedIndexRefCountArray<KUITIPDATA const> KTipArray;

	KTipCenter();
	~KTipCenter();

public:
	struct KTIPINFO
	{
		LPCSTR pcszTip;
		int nShowTipType;
		int nRichText;
		float fX;
		float fY;
		float fW;
		float fH;
	};

public:
	static bool Init();
	static void Exit();
	static KTipCenter& GetSelf();

	void Reserve(int nCount);
	int  Append(int nTipIndex, int nShowTipType, int nRichText);
	void Remove(int nIndex);
	LPCSTR GetTip(int nIndex) const;
	int GetShowTipType(int nIndex) const;
	int AddRef(int nIndex);
	int GetUsedCount() const;
	int GetTotalCount() const;

	int ShowTip(int nIndex, float fX, float fY, float fW, float fH);
	int HideTip();

	int SetCurrentShowTipType(int nType);
	int OnMouseMove();

	KTIPINFO &GetTipInfo();
private:
	KTipArray m_aTip;
	static KTipCenter *ms_pSelf;

	int m_nReserveCount;

	int m_nCurrentShowTipType;

	KTIPINFO m_Info;
};

} //namespace UI

#endif //_INCLUDE_TIPCENTER_H_
