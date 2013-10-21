#ifndef _KINDIVIDUAL_DROP_LIST_H_
#define _KINDIVIDUAL_DROP_LIST_H_

class KIndividualDropList
{
public:
	KIndividualDropList(void);
	~KIndividualDropList(void);

	BOOL Init(char* pszFileName);
	BOOL UnInit(void);

	BOOL GetData(int nIndex, DWORD& rdwTabType, DWORD& rdwItemIndex, int& fnDropRate, 
						DWORD& rdwMin, DWORD& rdwMax);
	DWORD GetSize(void);

private:
	typedef struct _INDIVIDUAL_DROP
	{
		DWORD	dwTabType;
		DWORD	dwItemIndex;
		int		nDropRate;
		DWORD	dwMin;
		DWORD	dwMax;

	}INDIVIDUAL_DROP;

	INDIVIDUAL_DROP*	m_IndividualDropList;
	DWORD				m_dwSize;
};

#endif	//_KINDIVIDUAL_DROP_LIST_H_