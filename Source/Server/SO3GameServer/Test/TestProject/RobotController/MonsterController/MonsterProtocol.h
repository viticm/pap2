#ifndef _Monster_Protocol_H_
#define _Monster_Protocol_H_

#define _NAME_LEN 32

enum MonsterProtocol
{
    mpBegin,
    S2C_Init,
    S2C_CreateMonster,
    S2C_DestroyMonster,
    S2C_DestroyAllMonster,
    C2S_Statistic,
    mpEnd
};

struct ProtocolHead
{
    unsigned char byProtocolID;
};

struct KS2C_Init : ProtocolHead
{
    char szBishopIP[_NAME_LEN];
    int  nBishopPort;

    char szAccountNamePrefix[_NAME_LEN];
    int  nStartIndex;
    char szPassword[_NAME_LEN];

    int nArrangement;
    int nRange;
    int nRunRate;
    int nStartInterval;
    int nReconnect;
};

struct KS2C_CreateMonster : ProtocolHead
{
    int nCount;
};

struct KS2C_DestroyMonster : ProtocolHead
{
    int nCount;
};

struct KS2C_DestroyAllMonster : ProtocolHead
{
};

struct KC2S_Statistic : ProtocolHead
{
    int nOnlineCount;
    int nOfflineCount;
    int nLoginCount;

    int nCpuIdle;
    int nCpuUsage;
};

#endif //_Monster_Protocol_H_