#ifndef _KRAND_GENERATOR_H_
#define _KRAND_GENERATOR_H_

#include <map>

#define MAX_RAND_COUNT		128
#define MAX_RP_GROUP_COUNT	64

class KRandGenerator
{
public:
	KRandGenerator(void);
	~KRandGenerator(void);

	char* Load(char* pData);
	char* Save(char* pData);

	BOOL Generate(int nProbability, int nMaxNumber, WORD wRandIndex, 
		DWORD dwRPGroupIndex, int nRPBonus);

	WORD m_wGlobalRPValue;

private:
	typedef std::map<WORD, time_t>	RAND_LIST;
	RAND_LIST						m_RandList;

	WORD m_wRPList[MAX_RP_GROUP_COUNT];

	int	GetRPRand(int nRand, WORD wRPValue);
};

#endif	//_KRAND_GENERATOR_H_
