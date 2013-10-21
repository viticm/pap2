/************************************************************************/
/* 道具掉落表							                                */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.02.28	Create												*/
/*		2005.12.14	Corrected item drop algorithm						*/
/************************************************************************/
#ifndef _KDROP_LIST_H_
#define _KDROP_LIST_H_

#include "KIndividualDropList.h"

class KItem;

class KDropList
{
public:
	KDropList(void);
	~KDropList(void);

	//初始化，设置表的大小
	BOOL Init(DWORD dwSize);

	//设定道具掉落表，掉落表的序号，道具类型，道具编号，掉落率％
	BOOL AddItem(DROP_DATA& DropData);

	//计算掉落率区间
	BOOL PreProcess(void);

	//随机产生一个道具
	KItem* GetRandomItem(DROP_DATA& DropData);

private:
	DROP_DATA*		m_ItemList;
	DWORD			m_dwSize;
	DWORD			m_dwIndex;
};

class KMoneyDropList
{
public:
    KMoneyDropList(void){};
    ~KMoneyDropList(void){};

	BOOL Init();

	BOOL AddMoney(DWORD dwMinMoney, DWORD dwMaxMoney);
	BOOL SetMoneyDropRate(DWORD dwMoneyDropRate);
	DWORD GetRandomMoney();
	BOOL IsDropInRandom();

private:
	DWORD			m_dwMinMoney;
	DWORD			m_dwMaxMoney;
	DWORD			m_dwMoneyDropRate;
};
#endif	//_KDROP_LIST_H_
