#ifndef _KPROFESSION_H_
#define	_KPROFESSION_H_

#include "Global.h"
#include "Luna.h"

#define PROFESSION_NAME_LEN			32
#define PROFESSION_MAX_LEVEL		100

#define PROFESSION_LIMIT_COUNT		3

#define PROFESSION_Branch_NAME_LEN	32
#define PROFESSION_BRANCH_COUNT		4

// 生活技能
class KProfession
{
public:
    KProfession();

	BOOL LoadFromFile(const char* szFileName);
	BOOL LoadBranchData(const char* szFileName);
	DWORD GetLevelProficiency(DWORD dwLevel);

	DWORD				m_dwID;
	DWORD				m_dwCraftRadarID;
	int					m_nMaxProficiencyPerLevel[PROFESSION_MAX_LEVEL + 1];
	int					m_nPrice[PROFESSION_LIMIT_COUNT]; 
	char				m_szProfessionName[PROFESSION_NAME_LEN];

	// 分支信息
	BOOL				m_bBranchUsed[PROFESSION_BRANCH_COUNT];

public:
	DECLARE_LUA_CLASS(KProfession);

	DECLARE_LUA_DWORD(ID);
	DECLARE_LUA_DWORD(CraftRadarID);
	DECLARE_LUA_STRING(ProfessionName, sizeof(m_szProfessionName));

	int LuaGetLevelProficiency(Lua_State* L);

	//static int nLimitLevel[PROFESSION_LIMIT_COUNT] = {30, 50, 70};
};
#endif //_KPROFESSION_H_
