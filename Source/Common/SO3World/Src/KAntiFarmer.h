#ifndef _KANTI_FARMER_H_
#define _KANTI_FARMER_H_

#include <list>
#include "Engine/KMemory.h"

#define ANTI_FARMER_CHECK_ONLINE_TIME_INTERVAL  30      // 在线时间检查周期(秒)

class KAntiFarmer
{
public:
    KAntiFarmer();

    BOOL Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
    BOOL Load(BYTE* pbyData, size_t uDataLen);

    void RecordOnlineTime();
    void RecordKillNpc(DWORD dwNpcTemplateID);

    BOOL Judge();

private:
    #pragma pack(1)
    struct KOnlineRecord
    {
        time_t nLoginTime;
        time_t nLogoutTime;
    };
    #pragma pack()

    typedef std::list<KOnlineRecord, KMemory::KAllocator<KOnlineRecord> > KONLINE_RECORD_LIST;
    KONLINE_RECORD_LIST m_OnlineRecordList;

    std::vector<DWORD>  m_KilledNpcRecords;
    int                 m_nNextKillNpcRecordPos;
};

#endif // _KANTI_FARMER_H_
