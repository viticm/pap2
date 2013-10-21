#pragma once
#include "Engine/KMutex.h"

class KG3DBip;
class KG3DBipTable
{
public:
	KMutex m_Lock;

	KG3DBipTable(void);
	~KG3DBipTable(void);
	void ReleaseBip(DWORD dwID);
    KG3DBip* GetBip(LPCSTR strFileName);
	enum
	{
		INVALIDATE_ID = 0xffffffff,
	};

private:
	typedef std::map<DWORD, KG3DBip*> BipTable;
	typedef std::map<DWORD, KG3DBip*>::iterator BipTableIterator;
	std::map<DWORD, KG3DBip*> m_BipTable;
};

extern KG3DBipTable g_cBipTable;
