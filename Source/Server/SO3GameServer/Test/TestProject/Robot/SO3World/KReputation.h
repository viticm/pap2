#ifndef _KREPUTATION_H_
#define _KREPUTATION_H_


#define MIN_REPUTE	-32767
#define MAX_REPUTE	32767

#define ENEMY_REPUTE	0
#define DIALOG_REPUTE	200
#define ALLY_REPUTE		600

class KPlayer;

typedef struct tagFORCE_INFO
{
	int		nRepute;
	BOOL	bFightFlag;
} FORCE_INFO;

class KReputation
{
public:
	KReputation(void);
	~KReputation(void);

	BOOL Init(KPlayer* pPlayer);
	char* Load(char* pData);
	char* Save(char* pData);

	int GetReputation(DWORD dwForceID);
	int GetReputeLevel(DWORD dwForceID);
	BOOL GetFightFlag(DWORD dwForceID);
	BOOL SetFightFlag(DWORD dwForceID, BOOL bFight);
	BOOL SetReputation(DWORD dwForceID, int nReputation);
	BOOL ModifyReputation(DWORD dwForceID, int nAdjustValue);

private:
	KPlayer*	m_pPlayer;
	FORCE_INFO	m_ForceInfo[MAX_FORCE_COUNT];
};

#endif	//_KREPUTATION_H_