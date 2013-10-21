#ifndef _KMAP_LIST_FILE_H_
#define _KMAP_LIST_FILE_H_

#include "Global.h"

struct KMapParams
{
	DWORD	dwMapID;
	char	szMapName[_NAME_LEN];
	char	szDropName[MAX_PATH];
	int	    nRegionWidth;
	int	    nRegionHeight;
	DWORD	dwCellTotal;
	DWORD	dwServerNpcTotal;
	DWORD	dwClientNpcTotal;
	DWORD	dwServerDoodadTotal;
	DWORD	dwClientDoodadTotal;
	char	szNpcReliveTabName[MAX_PATH];
	char	szDoodadReliveTabName[MAX_PATH];
	char	szNpcTeamTabName[MAX_PATH];
	char	szPatrolPathTabName[MAX_PATH];
#ifdef _CLIENT
	char	szResourceFilePath[MAX_PATH];
#endif // #ifdef _CLIENT
    BOOL    bReviveInSitu;
};

class KMapListFile
{
public:
	KMapListFile(void);
	~KMapListFile(void);

	BOOL Init(void);

	KMapParams*	GetData(DWORD dwMapID);

private:
	KMapParams m_MapParamsList[MAX_MAP_INDEX];
};
#endif	//_KMAP_LIST_FILE_H_