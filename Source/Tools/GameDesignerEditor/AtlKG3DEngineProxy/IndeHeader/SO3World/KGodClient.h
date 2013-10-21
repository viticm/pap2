
#ifndef _KGOD_CLIENT_H_
#define _KGOD_CLIENT_H_

#if defined(_CLIENT)

#include "Common/KG_Socket.h"
#include "Global.h"
#include "GE_Protocol.h"

class KCharacter;
class KNpc;
class KPlayer;
class KDoodad;
class IKG_Buffer;
class KTarget;
class KRegion;
class KItem;
class KQuest;
class KShop;
class KSkill;
class KScene;

class KGodClient
{
public:
	KGodClient();
	~KGodClient();

	BOOL Init();
	void UnInit();

	void Breathe();

public:
	BOOL Connect(const char cszIP[], u_short uPort);
	void Disconnect();

    BOOL Send(void* pvData, size_t uSize);

public:
    BOOL    SyncNpcData();
    BOOL    SyncDoodadData();
    BOOL    IsSyncComplete();
    KScene* GetCurrentScene();
    BOOL    Editor2Game();
    BOOL    Game2Editor();

public:

    BOOL DoLoadSceneRequest(DWORD dwMapID);

    BOOL DoAppendNpcData(const KNPC_DATA& crNpcData);
    BOOL DoAppendDoodadData(const KDOODAD_DATA& crDoodadData);

    BOOL DoDelNpcData(KNpc* pNpc);
    BOOL DoDelDoodadData(KDoodad* pDoodad);

    BOOL DoDeleteAllObjects();

    BOOL DoMoveCharacter(KCharacter* pCharacter, int nX, int nY, int nZ);

private:
    typedef BOOL (KGodClient::*PGOD_PROTOCOL_FUNC)(BYTE* pbyData, size_t uSize);
     // 变长协议uPakSize填UNDEFINED_PROTOCOL_SIZE
    void RegisterProtocol(int nProtocolID, size_t uPakSize, PGOD_PROTOCOL_FUNC pfnProtocolFunction);
    BOOL CheckProtocol(BYTE* pbyBuffer, size_t uBufferSize);

    BOOL OnLoadSceneRespond(BYTE* pbyData, size_t uSize);
    BOOL OnCreateNpc(BYTE* pbyData, size_t uSize);
    BOOL OnCreateDoodad(BYTE* pbyData, size_t uSize);

private:
    IKG_SocketStream* m_piSocket;

    typedef std::map<DWORD, DWORD> KOBJ_2_DATA_MAP;

    typedef std::map<DWORD, KNPC_DATA> KNPC_DATA_TABLE;
    KNPC_DATA_TABLE             m_NpcData;
    DWORD                       m_dwNpcDataIDAlloc;
    std::list<DWORD>            m_NpcSyncList;
    KOBJ_2_DATA_MAP             m_Npc2Data;

    typedef std::map<DWORD, KDOODAD_DATA> KDOODAD_DATA_TABLE;
    KDOODAD_DATA_TABLE          m_DoodadData;
    DWORD                       m_dwDoodadDataIDAlloc;
    std::list<DWORD>            m_DoodadSyncList;
    KOBJ_2_DATA_MAP             m_Doodad2Data;

    KScene*                     m_pCurrentScene;

	PGOD_PROTOCOL_FUNC	m_pfnProtocolFunctions[g2e_protocol_end];
	size_t              m_uProtocolSize[g2e_protocol_end];
};

extern KGodClient g_GodClient;

#endif

#endif	
