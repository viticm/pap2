#ifndef _KPROFESSION_LIST_
#define _KPROFESSION_LIST_

#include <map>
#include <vector>
#include "Global.h"
#include "KProfession.h"

class KPlayer;
class KProfession;

#pragma pack(1)
struct KProfessionItem
{
	BYTE    m_byProfessionID;
	BYTE	m_byMaxLevel;
	BYTE	m_byCurrentLevel;
	WORD	m_wProficiency;
	BYTE	m_byBranchID;
};

struct KProfessionDBData 
{
    BYTE            byCount;
    KProfessionItem Profession[0];
};
#pragma pack()

class KProfessionList
{
public:
	BOOL    Init(KPlayer* pPlayer);
	void    UnInit();

#ifdef _SERVER
    BOOL    Load(BYTE* pbyData, size_t uDataLen);
    BOOL    Save(size_t* puUseSize, BYTE* pbyBuffer, size_t uBufferSize);
#endif

	BOOL    CanLearnProfession(DWORD dwProfessionID);
	BOOL	LearnProfession(DWORD dwProfessionID);
	BOOL	ForgetProfession(DWORD dwProfessionID);
	DWORD	GetProfessionLevel(DWORD dwProfessionID);
    int     GetProfessionAdjustLevel(DWORD dwProfessionID);
	DWORD	GetProfessionMaxLevel(DWORD dwProfessionID);
	int		GetProficiency(DWORD dwProfessionID);
	DWORD	GetBranchID(DWORD dwProfessionID);

	BOOL	IsProfessionLearned(DWORD dwProfessionID);
	DWORD	GetProfessionCount(void);

	BOOL	AddProfessionProficiency(DWORD dwProfessionID, DWORD dwExp);

    BOOL    SetAdjustLevel(DWORD dwProfessionID, int nAdjustLevel);
	BOOL	SetMaxLevel(DWORD dwProfessionID, DWORD dwMaxLevel);
	BOOL	LearnBranch(DWORD dwProfessionID, DWORD Branch);

	BOOL	UpdatePlayerProfession(
        DWORD dwProfessionID, DWORD dwMaxLevel, DWORD dwCurrentLevel, 
        DWORD dwCurrentProficiency, DWORD dwBranchID
    );
	BOOL	GetProfessionList(std::vector<DWORD>& vecProfessionIDList);

public:
	KPlayer* m_pPlayer;

	struct KPlayerProfession	//玩家学会的技艺，记录了每个玩家之间不同的信息
	{
		BOOL	m_bIsLearnd;
		DWORD	m_dwMaxLevel;	//已经学会的技艺最大等级
		DWORD	m_dwCurrentLevel;	//当前技艺等级
        int     m_nAdjustLevel;    // 被其他因素影响的等级
		DWORD	m_dwCurrentProficiency;//当前技艺熟练度
		DWORD	m_dwBranchID;
	};

private:
	KPlayerProfession m_PlayerProfessionList[MAX_PROFESSION_COUNT];
};

#endif //_KPROFESSION_LIST_
