#ifndef _KTRANSLATE_LIST_H_
#define _KTRANSLATE_LIST_H_ 1

#include <map>
#include "SO3GlobalDef.h"
#include "Engine/KMemory.h"

class KTransmissionList
{
public:
    KTransmissionList();

    BOOL Init();
    void UnInit();

    BOOL GetTranslatePosition(DWORD dwCityID, DWORD* pdwRetMapID, int* pnRetX, int* pnRetY, int* pnRetZ);

private:
    BOOL LoadTab(char szFilePath[]);

    struct POSITION
    {
        DWORD dwMapID;
        int   nX;
        int   nY;
        int   nZ;
    };

    typedef KMemory::KAllocator<POSITION> KTRANSLATE_POSITION_ALLOC;
    typedef std::map<DWORD, POSITION, less<DWORD>, KTRANSLATE_POSITION_ALLOC> KTRANSLATE_MAP;

    KTRANSLATE_MAP m_TranslateMap;
};

#endif // _KTRANSLATE_LIST_H_

