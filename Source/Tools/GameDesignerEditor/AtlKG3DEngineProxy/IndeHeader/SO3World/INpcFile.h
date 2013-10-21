//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   INpcFile.h
//  Version     :   1.0
//  Creater     :   Zhao Chunfeng
//  Date        :   2008-05-28 15:11:19
//  Comment     :   NPC file interface
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _INPCFILE_H_
#define _INPCFILE_H_

#include "KSUnknown.h"

#pragma	pack(1)
struct KNPC_DATA                        // 96 bytes in total
{
    char    szNickName[_NAME_LEN];
    DWORD   dwTemplateID;
    int     nX;
    int     nY;
    int     nZ;
    BYTE    byFaceDirection;
    BYTE    byKind;						// Npc类型，见CHARACTER_KIND
    DWORD   dwScriptID;
    DWORD   dwReliveID;
    DWORD   dwRandomID;                 // 随机分组ID，如果是0，表示不随机
    DWORD   dwAIType;
    DWORD   dwThreatLinkID;             // 仇恨链接的分组ID
    int     nPatrolPathID;              // 寻路路线ID，同ID的NPC自动分为一组
    int     nOrderIndex;                // NPC在队形中的位置
    DWORD   dwRepresentID;
    int     nProgressID;
    BYTE    byReserved[10];
};
#pragma pack()

struct INpcFile : IUnknown
{
    virtual BOOL Push(const KNPC_DATA& crNpcInfo) = 0;
    virtual BOOL Save(const char cszName[]) = 0;

    virtual BOOL Load(const char cszName[]) = 0;
    virtual BOOL Pop(KNPC_DATA* pNpcInfo) = 0;

    virtual int  GetCount() = 0;
};

INpcFile* CreateNpcFileInterface();

#endif
