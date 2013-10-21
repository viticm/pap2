#include "StdAfx.h"
#include "KPlayerData.h"
#include "KSO3GameCenter.h"
#include "KRoleDBDataDef.h"
#include "KPlayerDataLoader.h"


KPlayerData::KPlayerData(void)
{
}

KPlayerData::~KPlayerData(void)
{
	m_RoleList.clear();
}

BOOL KPlayerData::Init(void)
{
    BOOL                bResult             = false;
	BOOL                bRetCode            = false;
    KPlayerDataLoader   PlayerDataLoader;
    BOOL                bLoaderInitFlag     = false;

    bRetCode = PlayerDataLoader.Init(
        g_SO3GameCenter.m_Settings.m_szRoleDBIP, 
        g_SO3GameCenter.m_Settings.m_szRoleDBName,
        g_SO3GameCenter.m_Settings.m_szRoleDBAccount, 
        g_SO3GameCenter.m_Settings.m_szRoleDBPassword
    );
    KGLOG_PROCESS_ERROR(bRetCode);
    bLoaderInitFlag = true;

    bRetCode = PlayerDataLoader.LoadAllPlayerData(this, g_SO3GameCenter.m_Settings.m_szRoleDBGroup);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    if (bLoaderInitFlag)
    {
        PlayerDataLoader.UnInit();
        bLoaderInitFlag = false;
    }
    return bResult;
}

KRole* KPlayerData::AddRole(DWORD dwPlayerID, KRole& Role)
{
	std::pair<ROLE_LIST::iterator, BOOL> InsRet1;
	std::pair<ROLE_NAME_LIST::iterator, BOOL> InsRet2;
	std::pair<ROLE_ACCOUNT_LIST::iterator, BOOL> InsRet3;
	ROLE_ACCOUNT_LIST::iterator itAccount;

	KGLOG_PROCESS_ERROR(m_RoleList.find(dwPlayerID) == m_RoleList.end());

	InsRet1 = m_RoleList.insert(std::make_pair(dwPlayerID, Role));
	KG_PROCESS_ERROR(InsRet1.second);

	InsRet2 = m_RoleNameList.insert(std::make_pair(
		InsRet1.first->second.m_szRoleName, 
		&InsRet1.first->second));
	KG_PROCESS_ERROR(InsRet2.second);

	itAccount = m_RoleAccountList.find(InsRet1.first->second.m_szAccountName);
	if (itAccount == m_RoleAccountList.end())
	{
		KAccount Account;

		memset(&Account, 0, sizeof(Account));
		Account.pRoleList[0] = &(InsRet1.first->second);
		
		InsRet3 = m_RoleAccountList.insert(std::make_pair(
			InsRet1.first->second.m_szAccountName, Account));
		KG_PROCESS_ERROR(InsRet3.second);
	}
	else
	{
		int nIndex = 0;
		while (itAccount->second.pRoleList[nIndex] != NULL &&
			nIndex < MAX_ROLE_PER_ACCOUNT)
		{
			nIndex++;
		}
		KGLOG_PROCESS_ERROR(nIndex < MAX_ROLE_PER_ACCOUNT);

		itAccount->second.pRoleList[nIndex] = &(InsRet1.first->second);
	}

	return &(InsRet1.first->second);

Exit0:

	if (InsRet1.second && InsRet2.second)
	{
		itAccount = m_RoleAccountList.find(Role.m_szAccountName);
		if (itAccount != m_RoleAccountList.end())
		{
			for (int nIndex = 0; nIndex < MAX_ROLE_PER_ACCOUNT; nIndex++)
			{
				if (itAccount->second.pRoleList[nIndex] == &(InsRet1.first->second))
					itAccount->second.pRoleList[nIndex] = NULL;
			}
		}
	}

	if (InsRet1.second && InsRet2.second)
	{
		ROLE_NAME_LIST::iterator itName = m_RoleNameList.find(Role.m_szRoleName);
		if (itName != m_RoleNameList.end())
			m_RoleNameList.erase(itName);
	}

	if (InsRet1.second)
	{
		ROLE_LIST::iterator itPlayer = m_RoleList.find(dwPlayerID);
		if (itPlayer != m_RoleList.end())
			m_RoleList.erase(itPlayer);
	}

	return NULL;
}

BOOL KPlayerData::DelRole(DWORD dwPlayerID)
{
	ROLE_LIST::iterator it;
	ROLE_ACCOUNT_LIST::iterator itAcc;
	it = m_RoleList.find(dwPlayerID);

	KGLOG_PROCESS_ERROR(it != m_RoleList.end());

	itAcc = m_RoleAccountList.find(it->second.m_szAccountName);
	KGLOG_PROCESS_ERROR(itAcc != m_RoleAccountList.end());

	int nIndex = 0;
	for (; nIndex < MAX_ROLE_PER_ACCOUNT; nIndex++)
	{
		if (itAcc->second.pRoleList[nIndex] &&
			STR_CASE_CMP(itAcc->second.pRoleList[nIndex]->m_szRoleName, it->second.m_szRoleName) == 0)
			break;
	}
	KGLOG_PROCESS_ERROR(nIndex < MAX_ROLE_PER_ACCOUNT);

	itAcc->second.pRoleList[nIndex] = NULL;

	m_RoleNameList.erase(it->second.m_szRoleName);
	m_RoleList.erase(it);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KPlayerData::UnRegisterRoleOnGS(int nConnIndex)
{
	ROLE_LIST::iterator it          = m_RoleList.begin();
    KMapCopy*           pMapCopy    = NULL;

	while (it != m_RoleList.end())
	{
        KRole* pRole = &it->second;

		if (pRole->m_nConnIndex == nConnIndex || pRole->m_nDestConnIndex == nConnIndex)
        {
            pRole->OnLeaveGS(nConnIndex);
        }

		++it;
	}

	return true;
}

KRole* KPlayerData::GetRole(DWORD dwPlayerID)
{
	ROLE_LIST::iterator it = m_RoleList.find(dwPlayerID);

	if (it != m_RoleList.end())
    {	
	    return  &it->second;
    }

	return NULL;
}

KRole* KPlayerData::GetRoleByName(const char* pszRoleName)
{
	ROLE_NAME_LIST::iterator it = m_RoleNameList.find(pszRoleName);

	if (it != m_RoleNameList.end())
    {
	    return it->second;
    }

	return NULL;
}

KAccount* KPlayerData::GetAccount(const char* pszAccountName)
{
	ROLE_ACCOUNT_LIST::iterator it = m_RoleAccountList.find(pszAccountName);
	if (it != m_RoleAccountList.end())
    {
	    return &it->second;
    }

	return NULL;
}
