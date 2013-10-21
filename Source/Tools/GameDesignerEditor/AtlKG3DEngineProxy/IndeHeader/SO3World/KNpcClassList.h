#ifndef _KNPC_CLASS_LIST_H_
#define _KNPC_CLASS_LIST_H_

#include "Global.h"

struct KNpcClass
{
	char	szName[_NAME_LEN];
	char	szDropName[MAX_PATH];
	DWORD	dwParentID;
};

class KNpcClassList
{
public:
	KNpcClassList(void);
	~KNpcClassList(void);

	BOOL Init(void);
	BOOL UnInit(void);

	KNpcClass* GetData(DWORD dwClassID);
	DWORD GetParentID(DWORD dwClassID);
	DWORD GetMaxClassID(void);

private:
	KNpcClass*	m_NpcClassList;
	DWORD		m_dwMaxClassID;
};

#endif	//_KNPC_CLASS_LIST_H_
