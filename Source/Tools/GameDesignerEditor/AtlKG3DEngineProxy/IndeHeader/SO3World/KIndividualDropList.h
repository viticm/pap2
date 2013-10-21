#ifndef _KINDIVIDUAL_DROP_LIST_H_
#define _KINDIVIDUAL_DROP_LIST_H_

struct DROP_DATA
{
	DWORD	dwTabType;
	DWORD	dwItemIndex;
	int		nDropRate;
	DWORD	dwMin;
	DWORD	dwMax;
	DWORD	dwQuestID;
	DWORD	dwQuestState;
	BOOL	bDropForEach;
};

class KIndividualDropList
{
public:
	KIndividualDropList(void);
	~KIndividualDropList(void);

	BOOL Init(char* pszFileName);
	BOOL UnInit(void);

	BOOL GetData(int nIndex, DROP_DATA& DropData);
	DWORD GetSize(void);

private:
	DROP_DATA*	m_IndividualDropList;
	DWORD		m_dwSize;
};

#endif	//_KINDIVIDUAL_DROP_LIST_H_
