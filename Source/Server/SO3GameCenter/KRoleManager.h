/************************************************************************/
/* 角色管理		                                                        */
/* Copyright : Kingsoft 2007										    */
/* Author	 : Zhao chunfeng										    */
/* History	 : 基于原来的PlayerData重构								    */
/*		2007.11.13	Create												*/
/************************************************************************/
#ifndef _ROLE_MANAGER_H_
#define _ROLE_MANAGER_H_

#include <set>
#include <map>
#include <vector>

#include "SO3GlobalDef.h"
#include "KRole.h"

typedef std::vector<KRole*, KMemory::KAllocator<KRole*> > KACC_ROLE_TAB;
struct KAccount
{
    char          szName[_NAME_LEN];
    KACC_ROLE_TAB vRoleList;
};

class KRoleManager
{
public:
    KRoleManager();

    BOOL Init();
    void UnInit();

    void Activate();

    KAccount* GetAccount(const char cszAccount[]);

    KRole* AddRole(
        const char cszAccountName[], const char cszRoleName[], 
        DWORD dwID, BYTE byLevel, BYTE byType, KCAMP eCamp, 
        BYTE byForceID, time_t nLastSaveTime, time_t nCreateTime
    );

    KRole* GetRole(DWORD dwID);
    KRole* GetRole(const char cszName[]);

    KRole* GetOnlineRoleForAccount(const char cszAccount[]);

    void   OnGSCrash(int nConnIndex);
    
    BOOL   IsRoleBelongToAccount(DWORD dwRoleID, const char cszAccountName[]);

    int    GetRoleCountTotal() { return (int)m_RoleTable.size(); }
    int    GetAccountCountTotal() { return (int)m_AccountNameIndex.size(); }

    BOOL   FreezeRole(DWORD dwRoleID, time_t nFreezeEndTime);
    time_t GetRoleFreezeTime(DWORD dwRoleID);

    
    void   RegisterDelRole(DWORD dwID, time_t nEndTime);
    void   UnregisterDelRole(DWORD dwID);
    time_t GetRoleDeleteTime(DWORD dwID);

    void ProcessDeleteList();
    BOOL DelRole(DWORD dwID);

    void CalculateCampActivePlayerCount();

    BOOL RoleRename(DWORD dwID, char szNewName[]);
    void RecordRenameRole(DWORD dwRoleID);
    BOOL IsCanRename(DWORD dwRoleID);
    void RemoveRenameRole(DWORD dwRoleID);

public:
    template <class TFunc>
    void Traverse(TFunc& Func)
    {
        BOOL bContinue = false;
        KROLE_TABLE::iterator it = m_RoleTable.begin();
        while (it != m_RoleTable.end())
        {
            KROLE_TABLE::iterator Current = it++;

            bContinue = Func(Current->first, Current->second);
            if (!bContinue)
                break;
        }
    }

private:
	struct KNameLess
	{
		bool operator()(const char* pszX, const char* pszY) const
		{
			return STR_CASE_CMP(pszX, pszY) < 0;
		}
	};

    typedef KMemory::KAllocator<std::pair<DWORD, KRole> > KROLE_TABLE_ALLOCTOR;
    typedef std::map<DWORD, KRole, less<DWORD>,  KROLE_TABLE_ALLOCTOR>  KROLE_TABLE;
	KROLE_TABLE         m_RoleTable;

    typedef KMemory::KAllocator<std::pair<const char*, KRole*> > KROLE_NAME_INDEX_ALLOCTOR;
	typedef std::map<const char*, KRole*, KNameLess,  KROLE_NAME_INDEX_ALLOCTOR> KROLE_NAME_INDEX;
	KROLE_NAME_INDEX    m_RoleNameIndex;
    
    typedef KMemory::KAllocator<std::pair<const char*, KAccount*> > KACCOUNT_NAME_INDEX_ALLOCTOR;
    typedef std::map<const char*, KAccount*, KNameLess, KACCOUNT_NAME_INDEX_ALLOCTOR> KACCOUNT_NAME_INDEX;
    KACCOUNT_NAME_INDEX m_AccountNameIndex;

    typedef KMemory::KAllocator<std::pair<DWORD, time_t> > KROLE_BLACK_LIST_ALLOCATOR;
    typedef std::map<DWORD, time_t, less<DWORD>,  KROLE_BLACK_LIST_ALLOCATOR> KROLE_BLACK_LIST;
    KROLE_BLACK_LIST    m_RoleBlackList;

    typedef KMemory::KAllocator<std::pair<DWORD, time_t> > KROLE_DELETE_LIST_ALLOCATOR;
    typedef std::map<DWORD, time_t, less<DWORD>, KROLE_DELETE_LIST_ALLOCATOR> KROLE_DELETE_LIST;
    KROLE_DELETE_LIST  m_RoleDeleteList;

    typedef KMemory::KAllocator<DWORD> KRENAME_ROLE_TABLE_ALLOCATOR;
    typedef std::set<DWORD, less<DWORD>, KRENAME_ROLE_TABLE_ALLOCATOR>KRENAME_ROLE_TABLE;
    KRENAME_ROLE_TABLE m_RenameRoleTable;
};

#endif


