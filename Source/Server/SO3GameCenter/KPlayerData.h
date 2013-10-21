/************************************************************************/
/* 玩家分布数据							                                */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2004.11.19	Create												*/
/************************************************************************/
#ifndef _PLAYERDATA_H_
#define _PLAYERDATA_H_

#include <map>
#include "KRole.h"

#define MAX_ROLE_PER_ACCOUNT	3

struct KAccount
{
	KRole* pRoleList[MAX_ROLE_PER_ACCOUNT];
};

class KPlayerData
{
public:
	KPlayerData(void);
	~KPlayerData(void);

	BOOL Init(void);

	KRole* AddRole(DWORD dwPlayerID, KRole& Role);
	BOOL DelRole(DWORD dwPlayerID);

	BOOL UnRegisterRoleOnGS(int nConnIndex);

	KRole* GetRole(DWORD dwPlayerID);
	KRole* GetRoleByName(const char* pszRoleName);
	KAccount* GetAccount(const char* pszAccountName);

private:
	struct role_ltstr
	{
		BOOL operator()(const char* X, const char* Y) const
		{
			return strncmp(X, Y, _NAME_LEN) < 0;
		}
	};

/*	struct account_ltstr
	{
		BOOL operator()(const char* X, const char* Y) const
		{
			return strncmp(X, Y, _NAME_LEN) < 0;
		}
	};
*/
	typedef std::map<DWORD, KRole>						ROLE_LIST;
	ROLE_LIST			m_RoleList;

	typedef std::map<const char*, KRole*, role_ltstr>		ROLE_NAME_LIST;
	ROLE_NAME_LIST		m_RoleNameList;

	typedef std::map<string, KAccount>	ROLE_ACCOUNT_LIST;
	ROLE_ACCOUNT_LIST	m_RoleAccountList;
};
#endif	//_PLAYERDATA_H_