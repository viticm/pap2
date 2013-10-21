/* -------------------------------------------------------------------------
//	文件名		：	kclientipcstream.h
//	创建者		：	wangbin
//	创建时间	：	2008-1-4 16:30:00
//	功能描述	：	客户端(非Guard)进程间通信实现
// -----------------------------------------------------------------------*/
#ifndef __KCLIENTIPCSTREAM_H__
#define __KCLIENTIPCSTREAM_H__

#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#include <strings.h>
#endif
#include "kipcstream.h"
#include "engine.h"

class KClientIpcStream : public KIpcStream
{
private:
	BOOL StrCaseCmp(LPCSTR pszStr1, LPCSTR pszStr2)
	{
		ASSERT(pszStr1 && pszStr2);
#ifdef WIN32
		return stricmp(pszStr1, pszStr2) == 0;
#else
		return strcasecmp(pszStr1, pszStr2) == 0;
#endif
	}
public:
	BOOL Initialize()
	{
#ifdef __linux
		CHAR chSep = '/';
#else
		CHAR chSep = '\\';
#endif
		CHAR szPath[MAX_PATH];
		getcwd(szPath, MAX_PATH);
		CHAR* pszApplication = strrchr(szPath, chSep);
		if (!pszApplication)
		{
			printf("Invalid path:%s?\n", szPath);
			return FALSE;
		}
		pszApplication++;
		struct KSERVER
		{
			LPCSTR pszServer;
			LPCSTR pszAlias;
			INT	 nShmId;
		} aryServerList[emKGUARDSERVER_COUNT] = {
			{"Goddess",			NULL,			emKGUARDSERVER_GODDESS},
			{"Bishop",			NULL,			emKGUARDSERVER_BISHOP},
			{"GameCenter",		NULL,			emKGUARDSERVER_GAMECENTER},
			{"LogServer",		NULL,			emKGUARDSERVER_LOGSERVER},
			{"GameServer01",	"GameServer",	emKGUARDSERVER_GAMESERVER01},
			{"GameServer02",	NULL,			emKGUARDSERVER_GAMESERVER02},
			{"GameServer03",	NULL,			emKGUARDSERVER_GAMESERVER03},
			{"GameServer04",	NULL,			emKGUARDSERVER_GAMESERVER04},
			{"GameServer05",	NULL,			emKGUARDSERVER_GAMESERVER05},
			{"GameServer06",	NULL,			emKGUARDSERVER_GAMESERVER06},
			{"GameServer07",	NULL,			emKGUARDSERVER_GAMESERVER07},
			{"GameServer08",	NULL,			emKGUARDSERVER_GAMESERVER08},
			{"GameServer09",	NULL,			emKGUARDSERVER_GAMESERVER09},
			{"GameServer10",	NULL,			emKGUARDSERVER_GAMESERVER10},
			{"GameServer11",	NULL,			emKGUARDSERVER_GAMESERVER11},
			{"GameServer12",	NULL,			emKGUARDSERVER_GAMESERVER12},
		};
		INT nShmId = 0;
		for (INT i = 0; i < emKGUARDSERVER_COUNT; ++i)
		{
			KSERVER* pServer = &aryServerList[i];
			if (StrCaseCmp(pServer->pszServer, pszApplication) ||
				(pServer->pszAlias != NULL && StrCaseCmp(pServer->pszServer, pszApplication)))
			{
				nShmId = pServer->nShmId + emKGUARDSERVER_FIRSTID;
				break;
			}
		}
		if (nShmId == 0)
		{
			printf("Invalid server:%s?\n", pszApplication);
			return FALSE;
		}
		if (!KIpcStream::Initialize(nShmId, 200 * 1024))
		{
			printf("Failed to initialize share memory for %s!\n", pszApplication);
			return FALSE;
		}
		printf("Initialize share memory for %s OK!\n", pszApplication);
		return TRUE;
	}
};

#endif //__KCLIENTIPCSTREAM_H__
