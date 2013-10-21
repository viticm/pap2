#ifndef _KITEMLIB_H_
#define	_KITEMLIB_H_

#include "KItemInfoList.h"
//#include "KAttribInfoList.h"
#include "KAttribGroup.h"
#include "KItem.h"

enum ITEM_TABLE_TYPE
{
	ittInvalid = 0,

	ittAttrib,
	ittWeapon,
	ittArmor,
	ittTrinket,
	ittOther,
	ittCustWeapon,
	ittCustArmor,
	ittCustTrinket,
	ittSet,
	
	ittTotal,
};

enum ITEM_INFO_TYPE
{
	iitInvaild = 0,

	iitEquipInfo,
	iitCustEquipInfo,
	iitOtherInfo,

	iitTotal,
};

class KItemLib
{
private:
	KItemInfoList<KEquipInfo>		m_Weapon;
	KItemInfoList<KEquipInfo>		m_Armor;
	KItemInfoList<KEquipInfo>		m_Trinket;
	KItemInfoList<KCustomEquipInfo>	m_CustomWeapon;
	KItemInfoList<KCustomEquipInfo>	m_CustomArmor;
	KItemInfoList<KCustomEquipInfo>	m_CustomTrinket;
	KItemInfoList<KOtherItemInfo>	m_Other;
	KItemInfoList<KAttribInfo>		m_Attrib;
	KAttribGroup					m_AttribGroup;
	int								m_nVerion;
private:
	BOOL GenerateRandItem(KItem* pItem, KEquipInfo* pInfo,
		int nMagicNum, int nQuality, DWORD dwRandSeed, int nMagic[]);
	BOOL GenerateCommonItem(KItem* pItem, KOtherItemInfo* pInfo,
		DWORD dwRandSeed);
	BOOL GenerateCustomItem(KItem* pItem, KCustomEquipInfo* pInfo,
		DWORD dwRandSeed);
	BOOL GenerateMagicAttrib(KItem* pItem, int nAttrGroup, 
		int nMagicNum);
	int  GenerateQuality(int nType, int nIndex);
	BOOL ExpandItemAttrib(KItem* pItem, KEquipInfo* pInfo);
	BOOL ExpandCustomItemAttrib(KItem* pItem, KCustomEquipInfo* pInfo);
public:
	KItemLib();
	~KItemLib();
	BOOL	Init(int nVersion);
	BOOL	GenerateItem(KItem* pItem, DWORD dwTabType, DWORD dwIndex, DWORD dwId = ERROR_ID, 
		DWORD dwRandSeed = 0, int nMagicNum = 0, int nQuality = 0, int nMagic[] = NULL,
		time_t nGenTime = 0);
	KItemInfo* GetItemInfo(DWORD dwTabType, DWORD dwIndex, int &nType);
	KAttribInfo* GetAttribInfo(int nAttribIndex);
};

template <> BOOL KItemInfoList<KEquipInfo>::Init(char* pszFile);
template <> BOOL KItemInfoList<KOtherItemInfo>::Init(char* pszFile);
template <> BOOL KItemInfoList<KCustomEquipInfo>::Init(char* pszFile);
template <> BOOL KItemInfoList<KAttribInfo>::Init(char* pszFile);

class KItemManager
{
public:
	KItemLib*		m_ItemLib;
public:
	KItemManager();
	~KItemManager();
	BOOL			Init();
	BOOL			FreeItem(KItem* pItem);
    
    // 2006.12.22: Added by Chen Jie, 在 pDestItem 指定的位置生成装备。
    BOOL            GenerateItemTo(KItem* pDestItem, 
        int nVersion, DWORD dwTabType, DWORD dwTabIndex, 
        DWORD dwId = ERROR_ID, DWORD dwRandSeed = 0, int nMagicNum = 0, 
        int nQuality = 0, int nMagic[] = NULL, time_t nGenTime = 0);

	KItem*			GenerateItem(int nVersion, DWORD dwTabType, DWORD dwTabIndex, 
		DWORD dwId = ERROR_ID, DWORD dwRandSeed = 0, int nMagicNum = 0, 
		int nQuality = 0, int nMagic[] = NULL, time_t nGenTime = 0);
	KItem*			CloneItem(KItem* pItem);

    // 2006.12.22: Added by Chen Jie, 根据传输数据在 pDestItem 指定的位置生成装备。
	BOOL			GenerateItemFromTransferDataTo(KItem* pDestItem, int nType, void* pBuffer, 
		size_t uBufferSize);

    KItem*			GenerateItemFromTransferData(int nType, void* pBuffer, 
		size_t uBufferSize);
	BOOL			GetItemTransferData(KItem* pItem, void* pBuffer, size_t& uBuffSize,
		int& nItemType);
	KItemInfo* GetItemInfo(int nVersion, DWORD dwTabType, DWORD dwIndex, int &nType);
};

extern KItemManager g_ItemLib;
#endif // _KITEMLIB_H