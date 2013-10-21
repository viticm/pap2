#ifndef _KREPUTATION_H_
#define _KREPUTATION_H_

#include "KRoleDBDataDef.h"
#define ENEMY_REPUTE_LEVEL		2
#define NEUTRAL_REPUTE_LEVEL	3
#define ALLY_REPUTE_LEVEL		4

class KPlayer;

typedef struct tagFORCE_INFO
{
	int		nRepute;
	int		nReputeLevel;
	BOOL	bFightFlag;
	BOOL	bUsed;
	BOOL	bShow;
} FORCE_INFO;

class KReputation
{
public:
	BOOL Init(KPlayer* pPlayer);
    BOOL Load(BYTE* pbyData, size_t uDataLen);
    BOOL Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);

	int  GetReputation(DWORD dwForceID);
	int  GetReputeLevel(DWORD dwForceID);
	BOOL GetFightFlag(DWORD dwForceID);
	BOOL SetFightFlag(DWORD dwForceID, BOOL bFight, BOOL bForce = false);
	BOOL SetReputation(DWORD dwForceID, int nLevel, int nReputation);
	BOOL ModifyReputation(DWORD dwForceID, int nAdjustValue, int nLowLimit = -1, int nHighLimit = -1);
	BOOL IsFightFlagLocked(DWORD dwForceID);
	BOOL Show(DWORD dwForceID, BOOL bLoginLoading = false);
	BOOL IsShow(DWORD dwForceID);
    BOOL SetPrinkForce(DWORD dwForceID, int nLevel);

private:
	KPlayer*	m_pPlayer;
	FORCE_INFO	m_ForceInfo[MAX_FORCE_COUNT];
    
    DWORD       m_dwPrinkForceID;
    int         m_nPrinkForceLevel;
};

#endif	//_KREPUTATION_H_
