#ifndef _KFELLOWSHIP_DEF_H_
#define _KFELLOWSHIP_DEF_H_

//#include "SO3GlobalDef.h"

#define KG_FELLOWSHIP_DB_CURRENT_DATA_VERSION   1

#pragma pack(1)

struct KG_FELLOWSHIP_BIT_FLAG
{
    uint8_t dwGroupID           :   8;
    uint8_t bMarried            :   1;
    uint8_t bBrother            :   1;
    uint8_t bReserve            :   6;
    int8_t  nWhisperDailyCount  :   4;
    int8_t  nDuelDailyCount     :   4;
};

struct KG_FELLOWSHIP_DB 
{
    BYTE    nVersion;
};

struct KG_FELLOWSHIP_ENTRY_V1 
{
    DWORD                   dwAlliedPlayerID;
    int                     nAttraction;
    KG_FELLOWSHIP_BIT_FLAG  Flags;
    char                    szRemark[_NAME_LEN];
};

struct KG_FELLOWSHIP_FOE_ENTRY_V1 
{
    DWORD   dwAlliedPlayerID;
};

struct KG_FELLOWSHIP_BLACKLIST_ENTRY_V1 
{
    DWORD   dwAlliedPlayerID;
};

struct KG_FELLOWSHIP_DB_V1 : KG_FELLOWSHIP_DB
{
    char    szGroupName[KG_FELLOWSHIP_MAX_CUSTEM_GROUP][_NAME_LEN];
    int     nFriendCount;
    int     nFoeCount;
    int     nBlackListCount;
    BYTE    byData[0];
};
#pragma pack()

#endif // _KFELLOWSHIP_DEF_H_
