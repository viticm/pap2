#ifndef _KLEVEL_MONEY_DROP_LIST_H_
#define _KLEVEL_MONEY_DROP_LIST_H_

class KLevelMoneyDropList
{
public:
	KLevelMoneyDropList(void);
	~KLevelMoneyDropList(void);

	BOOL Init(char* pszFileName);
	BOOL UnInit(void);

	BOOL GetData(DWORD dwIndex, int& rnLevel, int& rnClassID, int& rnMinMoney, int& rnMaxMoney);
	DWORD GetSize(void);

private:
	typedef struct LEVEL_MONEY_DROP
	{
		int		nLevel;
		int		nClassID;
		int		nMinMoney;
		int		nMaxMoney;
	}LEVEL_MONEY_DROP;

	LEVEL_MONEY_DROP*	m_LevelMoneyDropList;
	DWORD				m_dwSize;
};

#endif	//_KLEVEL_MONEY_DROP_LIST_H_
