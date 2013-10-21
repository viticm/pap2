#ifndef _KDOODAD_CLASS_LIST_
#define _KDOODAD_CLASS_LIST_

#include "Global.h"

struct KDoodadClass
{
	char	szName[_NAME_LEN];
	char	szDropName[MAX_DROP_PER_DOODAD][MAX_PATH];
	DWORD	dwDropCount[MAX_DROP_PER_DOODAD];
};

class KDoodadClassList
{
public:
	KDoodadClassList(void);
	~KDoodadClassList(void);

	BOOL Init(void);
	BOOL UnInit(void);

	KDoodadClass* GetData(DWORD dwClassID);
	DWORD GetMaxClassID(void);

private:
	KDoodadClass*	m_DoodadClassList;
	DWORD			m_dwMaxClassID;
};

#endif //_KDOODAD_CLASS_LIST_
