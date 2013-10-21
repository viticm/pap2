#ifndef _KINVENTORY_H_
#define	_KINVENTORY_H_

class KItem;

#define		MAX_ITEM_PACKAGE_SIZE		32
#define		FIRST_PACKAGE_SIZE			20
#define		MAX_SOLDLIST_PACKAGE_SIZE	10

class KInventory
{
	friend class KItemList;
private:
	BOOL	m_bTakeWith;							// 是否是随身携带的包
	int		m_nBoxType;								// 空间类型
	DWORD	m_dwSize;
	DWORD	m_dwUsed;
	KItem*	m_pItemArray[MAX_ITEM_PACKAGE_SIZE];
	union	// 该变量用于不同的背包类型，可以表示不同的含义
	{
		int		m_nSoldListHead;						// 待购回表中当前第一个待购回物品的位置
		int		m_nContainItemType;						// 限制容纳的物品类型
	};
public:
	KInventory();
	~KInventory();
	BOOL	Init(DWORD dwSize, int nBoxType, BOOL bTakeWith);
	KItem*	GetItem(DWORD dwX);
	BOOL	PlaceItem(KItem* pItem, DWORD dwX);
	KItem*	PickUpItem(DWORD dwX);
	int		FindFreePlace();
	int		FindFreeSoldList(int& pDelPos);		// 找到放置新待购物品的位置，如果有东西说明满了，则清除原有物品
	BOOL	IsOpened();
	void	Open();
	void	Close();
    int     GetSoldListHead();
	int		GetFreeCount();
	int		GetUsedCount() { return m_dwUsed; }
};
#endif