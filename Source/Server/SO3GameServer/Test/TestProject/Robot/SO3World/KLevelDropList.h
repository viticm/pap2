#ifndef _KLEVEL_DROP_LIST_H_
#define _KLEVEL_DROP_LIST_H_

class KLevelDropList
{
public:
	KLevelDropList(void);
	~KLevelDropList(void);

	BOOL Init(char* pszFileName);
	BOOL UnInit(void);

	BOOL GetData(DWORD dwIndex, int& rnLevel, DWORD& rdwTabType, DWORD& rdwItemIndex, int& rnDropRate,
				 DWORD& rdwMin, DWORD& rdwMax);
	DWORD GetSize(void);
	DWORD GetLevelCount(int nLevel);

private:
	typedef struct LEVEL_DROP
	{
		int		nLevel;
		DWORD	dwTabType;
		DWORD	dwItemIndex;
		int		nDropRate;
		DWORD	dwMin;
		DWORD	dwMax;
	}LEVEL_DROP;

	LEVEL_DROP*		m_LevelDropList;
	DWORD			m_dwSize;
};

#endif	//_KLEVEL_DROP_LIST_H_