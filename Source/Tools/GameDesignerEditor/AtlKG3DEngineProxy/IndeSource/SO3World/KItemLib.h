#ifndef _KITEMLIB_H_
#define	_KITEMLIB_H_

#include "KCustomEquipInfoList.h"
#include "KItemInfoList.h"
#include "KAttribInfoList.h"
#include "KAttribGroup.h"
#include "KItem.h"
#include "KEnchantLib.h"

enum ITEM_TABLE_TYPE
{
	ittInvalid = 0,

	ittAttrib,
	ittOther = 5,
	ittCustWeapon,
	ittCustArmor,
	ittCustTrinket,
	ittSet,
	
	ittTotal,
};

enum ITEM_INFO_TYPE
{
	iitInvaild = 0,

	iitCustEquipInfo,
	iitOtherInfo,

	iitTotal,
};

class KItemLib
{
public:

	BOOL	Init(int nVersion);
    void    UnInit();

	BOOL	GenerateItem(
        KItem*  pItem,          DWORD dwTabType,      DWORD dwIndex,      DWORD dwId     = ERROR_ID, 
		DWORD   dwRandSeed = 0, int   nMagicNum = 0,  int   nQuality = 0, int   nMagic[] = NULL,
		time_t  nGenTime   = 0
    );

	KItemInfo*   GetItemInfo(DWORD dwTabType, DWORD dwIndex);
	KAttribInfo* GetAttribInfo(int nAttribIndex);
	KSetInfo*	 GetSetInfo(DWORD dwID);

private:
	BOOL GenerateCommonItem(KItem* pItem, KOtherItemInfo* pInfo, DWORD dwRandSeed);
	BOOL GenerateCustomItem(KItem* pItem, KCustomEquipInfo* pInfo, DWORD dwRandSeed);
	BOOL ExpandCustomItemAttrib(KItem* pItem, KCustomEquipInfo* pInfo);

#ifdef _CLIENT
	BOOL InsertSetEquipList(KCustomEquipInfoList* pEquipInfo);
#endif

private:
	KCustomEquipInfoList	m_CustomWeapon;
	KCustomEquipInfoList	m_CustomArmor;
	KCustomEquipInfoList	m_CustomTrinket;
	KOtherItemInfoList	    m_Other;

	KAttribInfoList		    m_Attrib;
	KSetInfoList			m_Set;
	KAttribGroup			m_AttribGroup;
	int						m_nVerion;
};
#endif // _KITEMLIB_H
