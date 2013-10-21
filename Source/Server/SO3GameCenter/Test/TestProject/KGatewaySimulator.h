#pragma  once
#include "ProtocolBasic.h"

#include "KRoleDBDataDef.h"
#include "Protocol/bishoprelayprotocol.h"
#include "gatewayrelayprotocol.h"
#include "KSimulateSocket.h"

#define  GATEWAY KGatewaySimulator::Instance()

class KGatewaySimulator : public KSimulateSocket
{
public:
	static KGatewaySimulator* Instance();
	~KGatewaySimulator(void);
	bool Init();
public:
	KGatewaySimulator(void);
    BOOL CheckSetGameWorldVersion(int nLowerVersion, int nUpperVersion);
    BOOL CheckSyncRoleList(int nRoleIndex, int nRoleCount, KROLE_LIST_INFO* pBaseData);
    BOOL CheckSyncNewbieMaps();
    BOOL CheckCreateRoleRespond(KCREATE_ROLE_RESPOND_CODE eCode, KROLE_LIST_INFO* pBaseData);
    BOOL CheckDeleteRoleRespond(DWORD dwRoleID, BOOL bSucceed);
    BOOL CheckReplyLoginData(DWORD dwRoleID, int nRespondCode);
    BOOL CheckPlayerLeaveGame(const char cszAccountName[]);
    BOOL CheckGameWorldVersion(int nLowerProtocolVersion, int nUpperProtocolVersion);
    BOOL CheckGameLoginRespond(R2G_GAME_LOGIN_RESPOND* pGameLoginRepsond);
    BOOL CheckPingSignal(DWORD* pdwTime);
	bool CheckLoginRespond(BYTE byCode, DWORD dwRoleID, DWORD& dwQueueID);

    bool DoPingSignal();
    BOOL DoQueryAccountState(char szAccount[]);
    BOOL DonQueryRoleList( char szAccount[]);
    BOOL DoQueryNewbieMaps();
    BOOL DoKickAccountNotify(char szAccount[]);
    BOOL DoCreateRoleRequest(char szAccount[], KUSER_ROLE_GENERATOR_PARAM& stRoleGenParam);
    BOOL DoDeleteRoleRequest(DWORD   dwRoleID);
    BOOL DoGameLoginRequest(char szAccountName[], DWORD dwRoleID, KROLE_LIST_INFO& BaseData, DWORD dwQueueID = 0);
    BOOL DoExtPointsOptRespond(BYTE* pbyData, size_t uDataLen);

	int GetProtocolIndex(IKG_Buffer* pBuffer);
};