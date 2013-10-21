//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   GE_Protocol.h 
//  Version     :   1.0
//  Creater     :   Zhao Chunfeng
//  Date        :   2006-11-09 15:27:39
//  Comment     :   God 模式协议支持: Game server <==> Editor 协议定义
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _GE_PROTOCOL_H_ 
#define _GE_PROTOCOL_H_ 


enum EDITOR_2_GS_PROTOCOL
{
    e2g_protocol_begin = 0,
    e2g_load_scene_request = e2g_protocol_begin,
    e2g_sync_npc_data,
    e2g_sync_doodad_data,
    e2g_delete_npc,
    e2g_delete_doodad,
    e2g_delete_all_objects,
    e2g_move_character,
    e2g_protocol_end
};


enum GS_2_EDITOR_PROTOCOL
{
    g2e_protocol_begin = 0,
    g2e_load_scene_respond = g2e_protocol_begin,
    g2e_create_npc,
    g2e_create_doodad,
    g2e_protocol_end
};

struct KGOD_PROTOCOL_HEADER 
{
    BYTE byProtocolID;
};

struct  E2G_LOAD_SCENE_REQUEST : KGOD_PROTOCOL_HEADER
{
    DWORD dwMapID;
};

struct E2G_SYNC_NPC_DATA : KGOD_PROTOCOL_HEADER
{
    DWORD       dwDataID;
    KNPC_DATA   NpcData;
};

struct E2G_SYNC_DOODAD_DATA : KGOD_PROTOCOL_HEADER
{
    DWORD           dwDataID;
    KDOODAD_DATA    DoodadData;
};

struct E2G_DELETE_NPC : KGOD_PROTOCOL_HEADER 
{
    DWORD dwNpcID;
};

struct E2G_DELETE_DOODAD : KGOD_PROTOCOL_HEADER 
{
    DWORD dwDoodadID;
};

struct E2G_DELETE_ALL_OBJECTS : KGOD_PROTOCOL_HEADER
{
    DWORD dwMapID;
    DWORD dwMapCopyIndex;
};

struct E2G_MOVE_CHARACTER : KGOD_PROTOCOL_HEADER
{
    DWORD dwCharacterID;
    int   nX;
    int   nY;
    int   nZ;
};

struct G2E_LOAD_SCENE_RESPOND : KGOD_PROTOCOL_HEADER
{
    DWORD dwMapID;
    DWORD dwCopyIndex;
};

struct G2E_CREATE_NPC : KGOD_PROTOCOL_HEADER
{
    DWORD dwDataID;
    DWORD dwNpcID;
};

struct G2E_CREATE_DOODAD : KGOD_PROTOCOL_HEADER
{
    DWORD dwDataID;
    DWORD dwDoodadID;
};


#endif
