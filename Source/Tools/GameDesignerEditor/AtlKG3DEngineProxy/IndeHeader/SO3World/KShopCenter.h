#ifndef _INCLUDE_KSHOPCENTER_H_
#define _INCLUDE_KSHOPCENTER_H_

#include <map>
#include "GS_Client_Protocol.h"
#include "Engine/KMutex.h"
class KShop;
class KNpc;

struct KNPC_SHOP_TEMPLATE_ITEM
{
    int   nType;
    int   nIndex;
    DWORD dwRandomSeed;
    int   nLimit;
    int   nDurability;
    int   nReputeLevel;
    int   nPrice;        // 需要的钱   
    int   nContribution; // 需要的贡献值
    int   nPrestige;     // 需要的威望
    int   nRequireAchievementRecord; // 需要达到的游戏完成度
    int   nAchievementPoint;         // 需要的成就点数
};

class KShopCenter
{
public:
    int  Init();
    void UnInit();

    KShop* GetShopInfo(DWORD dwShopID);
    
#ifdef _SERVER
public:
    void Activate();
 
    BOOL BindNpcShop(KNpc* pNPC, DWORD dwTemplateID);
    BOOL UnbindNpcShop(KNpc* pNPC);
 
private:
    void    DoRefresh();
    BOOL    LoadNpcShopTemplates();

    int m_nNextRefreshTime;
    DWORD m_dwAllocID;
#endif

    typedef std::vector<KNPC_SHOP_TEMPLATE_ITEM, KMemory::KAllocator<KNPC_SHOP_TEMPLATE_ITEM> > KNPC_SHOP_TEMPLATE_ITEMS;
    struct KNPC_SHOP_TEMPLATE 
    {
        char                        szShopName[_NAME_LEN];
        BOOL                        bCanRepair;
        KNPC_SHOP_TEMPLATE_ITEMS    vecTemplateItems;
    };

    typedef std::map<int, KNPC_SHOP_TEMPLATE> KNPC_SHOP_TEMPLATE_TABLE;
    KNPC_SHOP_TEMPLATE_TABLE m_NpcShopTemplateTable;

    BOOL LoadNpcShopTemplateItems(KNPC_SHOP_TEMPLATE* pShopTemplate, const char cszFileName[]);

public:
    KNPC_SHOP_TEMPLATE_ITEM* GetShopTemplateItem(DWORD dwShopTemplateID, int nItemTemplateIndex);

#ifdef _CLIENT
    BOOL UpDateShopInfo(DWORD dwTemplateID, DWORD dwShopID, int nShopType, DWORD dwValidPage, BOOL bCanRepair);
    BOOL LoadShopTemplate(DWORD dwShopTemplateID);
#endif

private:
    BOOL LoadReputationRebate();

    struct REPUTATION_REBATE
    {
        int nRebate;
        int nMaxRebate;
    };

    typedef map<uint64_t, REPUTATION_REBATE> REPUTATION_REBATE_MAP;
    REPUTATION_REBATE_MAP m_ReputationRebateMap;

public:
    int GetReputationRebate(DWORD dwForceID, int nReputeLevel);
    int GetMaxRebate(DWORD dwForceID, int nReputeLevel);

private:
    typedef KMemory::KAllocator<std::pair<DWORD, KShop> > KMAP_COPY_TABLE_ALLOCTOR;
    typedef std::map<DWORD, KShop, std::less<DWORD>, KMAP_COPY_TABLE_ALLOCTOR> KSHOP_LIST;
    KSHOP_LIST m_ShopList;
};
#endif //_INCLUDE_KSHOPCENTER_H_
