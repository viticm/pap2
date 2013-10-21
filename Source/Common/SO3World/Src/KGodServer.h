
#ifndef _KGOD_SERVER_H_
#define _KGOD_SERVER_H_

#if defined(_SERVER)
#include "IServer.h"
#include "KGod.h"
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

class KGodServer
{
public:
	KGodServer();
	~KGodServer();

	BOOL Init();
	void UnInit();

	void Breathe();

public:
    BOOL Send(int nConnectionIndex, void* pvBuffer, size_t uBufferSize);

private:
	typedef BOOL (KGodServer::*PGOD_PROTOCOL_FUNC)(int nConnIndex, BYTE* pbyData, size_t uSize);
    void RegisterProtocol(int nProtocolID, size_t uPakSize, PGOD_PROTOCOL_FUNC pfnProtocolFunction);
    BOOL CheckProtocol(BYTE* pbyBuffer, size_t uBufferSize);

    BOOL OnLoadSceneRequest(int nConnIndex, BYTE* pbyData, size_t uSize);
    BOOL OnSyncNpcData(int nConnIndex, BYTE* pbyData, size_t uSize);
    BOOL OnSyncDoodadData(int nConnIndex, BYTE* pbyData, size_t uSize);
    BOOL OnDeleteNpc(int nConnIndex, BYTE* pbyData, size_t uSize);
    BOOL OnDeleteDoodad(int nConnIndex, BYTE* pbyData, size_t uSize);
    BOOL OnDeleteAllObjects(int nConnIndex, BYTE* pbyData, size_t uSize);
    BOOL OnMoveCharacter(int nConnIndex, BYTE* pbyData, size_t uSize);

private:
	PGOD_PROTOCOL_FUNC	m_pfnProtocolFunctions[e2g_protocol_end];
	size_t              m_uProtocolSize[e2g_protocol_end];

    KGod*               m_pGod;
};

extern KGodServer g_GodServer;

#endif	//_SERVER
#endif	//_KGOD_SERVER_H_
