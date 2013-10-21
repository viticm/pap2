#ifndef _KREPUTATION_LIMIT_H_
#define _KREPUTATION_LIMIT_H_

class KReputationLimit
{
public:
	KReputationLimit();
	~KReputationLimit();
	BOOL	Init();
    void    UnInit();

	int		GetReputeLimit(int nLevel);
	int		GetMaxLevel();

private:
	int		m_nMaxLevel;
	int*	m_pnReputeLimit;
};
#endif
