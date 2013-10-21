/************************************************************************/
/* 道具集合基类							                                */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.01.25	Create												*/
/************************************************************************/
#ifndef _KITEM_LIST_H_
#define _KITEM_LIST_H_

#include "SO3Result.h"
#include "KItem.h"
#include "KInventory.h"

#define TRADE_BOX_WIDTH		8
#define TRADE_BOX_HEIGHT	4

#define MONEY_LIMIT 2000000000

enum ITEM_LIST_STATE
{
	ilsInvalid = 0,

	ilsLocked,			//锁定状态
	ilsOpened,			//解锁状态

	ilsTotal
};

typedef struct 
{
	DWORD	dwBox;
	DWORD	dwX;
} TItemPos;

class KPlayer;
class KEquipInfo;
class KCustomEquipInfo;

class KItemList
{
public:
	int		m_nMoney;			//金钱
	int		m_nMoneyLimit;		//金钱上限
public:
	KItemList(void);
	~KItemList(void);

	BOOL	Init(KPlayer* pPlayer);
	BOOL	UnInit();

	//添加一个物品，主要是服务端使用，自动帮助玩家找到放置的位置。
	BOOL	AddItem(KItem* pItem);

	//添加一个物品，主要是客户使用，同步下来的装备直接放到指定位置。
	BOOL	AddItem(KItem* pItem, DWORD dwBoxIndex, DWORD dwX);

	//交换两个物品的位置
	ITEM_RESULT_CODE	ExchangeItem(const TItemPos& Src, const TItemPos& Dest, DWORD dwAmount = 0);
	//从身上删除一个物品，不从游戏世界删除掉
	KItem*	DelItem(DWORD dwBoxIndex, DWORD dwX);
	//销毁物品，既从身上删除，也从游戏世界删除
	ITEM_RESULT_CODE	DestoryItem(const TItemPos& Pos, BOOL bSync = TRUE);
	ITEM_RESULT_CODE	DestoryItem(KItem* pItem, BOOL bSync = TRUE);
	//根据箱子中的坐标找物品
	KItem*	GetItem(DWORD dwBoxIndex, DWORD dwX);
	//查找指定大小的空闲空间
	BOOL	FindFreeRoom(DWORD dwBoxIndex, DWORD& dwX);
	BOOL	FindFreeRoomInPackage(DWORD& dwBoxIndex, DWORD& dwX);
	BOOL	FindFreeSoldList(DWORD& dwX);
	DWORD	GetFreeRoomSize(void);

	DWORD	GetBoxSize(DWORD dwBox);
	//钱相关的方法
	int		GetMoney() { return m_nMoney; };
	BOOL	SetMoney(int nMoney);
	BOOL	AddMoney(int nMoney);
	//钱上限相关的方法
	int		GetMoneyLimit() { return m_nMoneyLimit; };
	BOOL	SetMoneyLimit(int nLimit);
	BOOL	AddMoneyLimit(int nLimit);
	//解锁
	BOOL	UnLock(DWORD dwPassword);
	//加锁
	BOOL	Lock(void);
	//获取物品数量
	DWORD	GetItemAmount(DWORD dwTabType, DWORD dwIndex);
	KItem*	FindItem(DWORD dwTabType, DWORD dwIndex, DWORD& dwBox, DWORD& dwX);
	//找到该类物品的第一个位置，如果没有，返回失败
	BOOL	GetItemPos(int nVersion, DWORD dwTabType, DWORD dwIndex, DWORD &dwBox, DWORD &dwX);
	BOOL	GetItemPos(DWORD dwID, DWORD &dwBox, DWORD &dwX);
	int		GetBoxType(DWORD dwBox);
	//消耗某类物品，会消耗除所有同类型物品，直到数量足够为止，不能消耗装备类物品
	BOOL	CostItem(DWORD dwTabType, DWORD dwIndex, DWORD dwAmount);
	//扣取某个指定物品的数量
	BOOL	CostSingleItem(DWORD dwBoxIndex, DWORD dwX, DWORD dwAmount);
	//弹药消耗，包括可以自动填弹
	BOOL	ConsumeAmmo(DWORD dwAmount);
	//按指定类型填弹
	BOOL	ReloadAmmo(DWORD dwTabType, DWORD dwIndex);

	//使用道具
	ITEM_RESULT_CODE	UseItem(TItemPos ItemPos, KTarget& rTarget);
	//检测物品是否可以装备
	ITEM_RESULT_CODE CanEquip(KItem* pItem, DWORD dwX);
	ITEM_RESULT_CODE CanEquip(KEquipInfo* pItem, DWORD dwX);
	ITEM_RESULT_CODE CanEquip(KCustomEquipInfo* pItem, DWORD dwX);

	BOOL	CanPut(KItem* pItem, DWORD OrgBox, DWORD dwBox, DWORD dwX, BOOL bSrc);

	BOOL	AbradeArmor();
	BOOL	AbradeMeleeWeapon();
	BOOL	AbradeRangeWeapon();      
    //按装备最大耐久度的百分比, 磨损一个包包中的所有装备(主要用在玩家复活时,对其的惩罚)
    BOOL    AbradeEquipment(int nBoxType, int nAbradePercent);
	BOOL	Repair(DWORD dwBox, DWORD dwX);

    BOOL    TidyUpSoldList();

    BOOL	OpenBox(DWORD dwBox);
	BOOL	CloseBox(DWORD dwBox);

	BOOL	ApplyAttrib(KItem* pItem);
	BOOL	UnApplyAttrib(KItem* pItem);

	char*	Load(char* pData);
	char*	Save(char* pData);
private:
	BOOL	UnEquip(KItem* pItem, DWORD dwX);
	BOOL	Equip(KItem* pItem, DWORD dwX);
	//检测全身装备是否还符合需求
	BOOL	CheckEquipRequire(void);
	BOOL	PushItem(KItem* pItem, DWORD dwBox);
	BOOL	IsPackageSlot(DWORD dwBox, DWORD dwX);
	ITEM_RESULT_CODE	ExchangePackageSlot(DWORD dwBox1, DWORD dwBox2);
	ITEM_RESULT_CODE	EnablePackage(DWORD dwSize, int nContainType, DWORD dwBox);
	ITEM_RESULT_CODE	DisablePackage(DWORD dwBox);

private:
	KInventory			m_Box[ibTotal];
	DWORD				m_dwBoxCount;									//箱子的总个数

	ITEM_LIST_STATE		m_State;										//箱子的状态
	DWORD				m_dwPassword;									//密码
	KPlayer*			m_pPlayer;

private:
	//存盘结构定义
	// 道具结构
	enum ITEM_DB_STRUCT_TYPE
	{
		idst_Common = 0,
		idst_CustomEquip,
		idst_Equip,
		idst_Train,
		idst_Total,
	};

#pragma	pack(1)
	// 所有类型的装备同步必须从这个结构派生
	struct KCommonItemDBData
	{
		BYTE	byVersion;
		BYTE	byBoxIndex;
		BYTE	byX;
		BYTE	byReserved1;
		BYTE	byReserved2;
		BYTE	byTabType;
		WORD	wTabIndex;
		time_t	nGenTime;
		WORD	wDurability;
	};

	struct KCustomEquipDBData : KCommonItemDBData
	{
		DWORD	dwRandSeed;
		WORD	wExtendMagic;
		WORD	wTemporaryMagic;
	};

	struct KRandEquipDBData : KCommonItemDBData
	{
		WORD	wMagic[MAX_ITEM_MAGIC_COUNT];
		WORD	wExtendMagic;
		WORD	wTemporaryMagic;
		DWORD	dwRandSeed;
		BYTE	byQuality;
	};

#pragma	pack()
};
#endif	//_KITEM_LIST_H_