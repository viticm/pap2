#ifndef _KLEVEL_DROP_LIST_H_
#define _KLEVEL_DROP_LIST_H_

#include "KIndividualDropList.h"

struct LEVEL_DROP_DATA : DROP_DATA
{
	int nLevel;
};

class KLevelDropList
{
public:
	KLevelDropList(void);
	~KLevelDropList(void);

	BOOL Init(char* pszFileName);
	BOOL UnInit(void);

	BOOL GetData(DWORD dwIndex, LEVEL_DROP_DATA& DropData);
	DWORD GetSize(void);
	DWORD GetLevelCount(int nLevel);

private:
	LEVEL_DROP_DATA* m_LevelDropList;
	DWORD			 m_dwSize;
};

#endif	//_KLEVEL_DROP_LIST_H_
