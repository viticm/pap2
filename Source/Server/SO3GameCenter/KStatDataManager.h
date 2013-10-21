#ifndef _KSTAT_DATA_MANAGER_H_
#define _KSTAT_DATA_MANAGER_H_

#include "KStatDataDef.h"
#include "KRole.h"

#define MAX_STAT_DATA_COUNT 1024 * 512

class KMapInfo;

#pragma pack(1)
struct KStatData_DB
{
    int     nDataCount;
    int64_t nData[0];
};
#pragma pack()

class KStatDataManager
{
public:
    KStatDataManager();
    ~KStatDataManager();

    BOOL Init();
    void UnInit();
    
    void Activate();
    BOOL OnGSUpdate(KSTAT_DATA_MODIFY* pData, int nCount);
    int  GetNameID(const char cszName[]);
    BOOL SetDataValue(const char szName[], int64_t nValue);
    BOOL SaveAll();
    BOOL LoadDataName(const char szName[], int nID);
    BOOL LoadData(time_t nTime, BYTE* pbyData, size_t uDataLen);

    BOOL Update(const char cszName[], int64_t nValue);

private:
    void UpdateRoleStat();

public:
    void UpdateCreateMap(KMapInfo* pMapInfo);
    void UpdateCampStat(KCAMP eCamp, int nLevel);
    void UpdataLeaveCampStat(KCAMP eCamp, int nLevel);

    void UpdataAuctionItemStat(DWORD dwTabType, DWORD dwTabIndex, int nMoney);
    void UpdateMoneyStat(KRole* pRole, int nMoney, const char cszMethod[]);

    void UpdataCreateOrDeleteRole(BOOL bCreate);
    void UpdateGameCardDealAmmount(int nCoin, int nMoney);

    void UpdateMentorCreateStat(DWORD dwMentorID, DWORD dwApprenticeID);
    void UpdateMentorDeleteStat();
    void UpdateMentorGraduateStat(DWORD dwMentorID, DWORD dwApprenticeID);
    void UpdateMentorBrokenStat(DWORD dwMentorID, DWORD dwApprenticeID);

private:
    time_t  m_nStatTime;
    int     m_nMaxUsedID;
    int64_t m_StatValues[MAX_STAT_DATA_COUNT];

    typedef KMemory::KAllocator<std::pair<kstring, int> > KDATA_NAME_TABLE_ALLOCATOR;
    typedef std::map<kstring, int, std::less<kstring>, KDATA_NAME_TABLE_ALLOCATOR> KDATA_NAME_TABLE;

    KDATA_NAME_TABLE m_DataNameTable;
};

#endif
