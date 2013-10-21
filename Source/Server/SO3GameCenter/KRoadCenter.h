#ifndef _KROAD_CENTER_H_
#define _KROAD_CENTER_H_

#include <map>
#include "SO3GlobalDef.h"
#include "Engine/KMemory.h"

class KRoadCenter
{
public:
    KRoadCenter();

    BOOL Init();
    void UnInit();

    BOOL SyncNewServer(int nConnIndex);

    BOOL UpdateRoadInfo(int nRoadNodeID, KCAMP eCamp, BOOL bEnable);

private:
    struct KROAD_NODE
    {
        KCAMP eCamp;
        BOOL  bEnable;
    };

    typedef KMemory::KAllocator<std::pair<int, KROAD_NODE> > KROAD_INFO_ALLOC;
    typedef std::map<int, KROAD_NODE, less<DWORD>, KROAD_INFO_ALLOC> KROAD_INFO;

    KROAD_INFO m_RoadInfo;
};

#endif // _KROAD_CENTER_H_
