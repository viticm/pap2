#ifndef _KATTRIB_GROUP_H_
#define _KATTRIB_GROUP_H_

#include "Global.h"
#include <vector>

#define	MAX_ATTRIB_GROUP_NUM	1024

struct KAttribRate
{
	int		nAttribIndex;
	int		nRate;
};

class KAttribGroup
{
private:
	typedef std::vector<KAttribRate>	ATTRIB_RATE;
	ATTRIB_RATE	m_Rate[MAX_ATTRIB_GROUP_NUM];
public:
	KAttribGroup();
	~KAttribGroup();
	BOOL	Init(const char* pszFile);
	BOOL	UnInit();
	int		GetAttribIndex(int nGroup);
};
#endif