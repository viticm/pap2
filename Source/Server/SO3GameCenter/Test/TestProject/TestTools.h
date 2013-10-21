#ifndef _TEST_TOOLS_H_
#define _TEST_TOOLS_H_

#include <vector>
#include "ProtocolBasic.h"
#include "SO3ProtocolBasic.h"
#include "KMailDef.h"

class KGameServerSimulator;

#define  CASE_INFO(PreCondition, ExceptResult) RecordProperty("PreCondition", PreCondition); RecordProperty("ExceptResult", ExceptResult)

	
namespace TestTools
{
    struct KRoleInfo 
    {
        KRoleInfo() 
        {
            dwPlayerID = 0;
            strncpy(szRoleName, "", _NAME_LEN);
            strncpy(szAccount, "", _NAME_LEN);
            dwPartyID = ERROR_ID;
			dwTongID = ERROR_ID;
            pLoginOnGS = NULL;
        }
        DWORD dwPlayerID;
        char  szRoleName[_NAME_LEN];
        char  szAccount[_NAME_LEN];
        DWORD dwPartyID;
		DWORD dwTongID;
        KGameServerSimulator* pLoginOnGS;
    };

    BOOL PlayerEnterGame(KGameServerSimulator* pGS, KRoleInfo* pRoleInfo, DWORD dwMapID, __int64 nMapCopyID, DWORD dwQueue = 0);

    BOOL PlayerLeaveGame(KRoleInfo* pRoleInfo);

	KRoleInfo CreateRole();

    BOOL CreateRole(KRoleInfo* pRoleInfo);

    BOOL SetRoleDBData(KRoleInfo* pRoleInfo, KGameServerSimulator* pGS, DWORD dwMapID, __int64 nMapCopyID);

    void SetHighPerforGS( unsigned int uIndex);

	bool CreateParty(KRoleInfo* pLeader, KRoleInfo* pMember);

	bool DisbandParty(KRoleInfo* pLeader);

	bool JoinParty(KRoleInfo* pLeader, KRoleInfo* pJoiner);

	bool CreateMap(DWORD dwMapID);

	bool DeleteRole(KRoleInfo* pDeleteRole);

	void AccelerationTime(time_t Time);

	void AccelerationGC(DWORD dwGameLoop);

	BOOL PlayerApplyJoinParty(KRoleInfo* pApplySrc, KRoleInfo* pApplyDesLeader);
};
#endif _TEST_TOOLS_H_