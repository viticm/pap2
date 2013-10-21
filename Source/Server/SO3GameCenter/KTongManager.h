/************************************************************************/
/* 帮会管理		                                                        */
/* Copyright : Kingsoft 2007										    */
/* Author	 : Zhao chunfeng										    */
/* History	 : 基于原来的TongData重构								    */
/*		2007.11.13	Create												*/
/************************************************************************/

#ifndef _TONG_MANAGER_H_
#define _TONG_MANAGER_H_

#include <map>
#include <list>
#include "SO3Result.h"
#include "ProtocolBasic.h"
#include "KTong.h"

#define TONG_SCRIPTS CENTER_SCRIPTS_DIR"/tong.lua"
#define JOIN_TONG_FUNCTION "OnJoinTong"

class KTongManager
{
public:
    KTongManager();

    BOOL Init();
    void UnInit();

    void Activate();
    void RemoveTimeOutInvite();
    void ProcessCheckTongState();

    BOOL CreateTong(DWORD dwMasterID, const char cszName[], int nTemplateIndex);

    KTong* GetTong(DWORD dwID);
    KTong* GetTongByName(const char* pszTongName);
    int    GetOperationIndex(KTONG_OPERATION_TYPE eOperationType);

    BOOL LoadTongData(DWORD dwTongID, BYTE* pbyTongData, size_t uDataLen);

    void ProcessApplyTongInfoRequest(DWORD dwPlayerID, int nLastUpdateFrame);
    void ProcessApplyRosterRequest(DWORD dwPlayerID, int nClientLastUpdateFrame, DWORD* pdwMemberList, int nMemberCount);

    void ProcessInvitePlayerJoinTongRequest(DWORD dwPlayerID, char* pszNewMemberName);
    void ProcessInvitePlayerJoinTongRespond(DWORD dwPlayerID, DWORD dwInviterID, DWORD dwTongID, INVITE_TONG_RESULT_CODE eAccept);
    void ProcessKickOutTongMemberRequest(DWORD dwPlayerID, DWORD dwMemberID);

    void ProcessModifyTongInfoRequest(DWORD dwPlayerID, int nInfoType, BYTE* pbyData, size_t uDataLen);
    BOOL ProcessModifyMemorabiliarequest(
            KTONG_MODIFY_MEMORABILIA_TYPE eType, 
            KTONG_MODIFY_MEMORABILIA_INFO* pInfo, 
            size_t uDataLen, 
            KTong* pTong
        );

    void ProcessModifyTongSchemaRequest(DWORD dwPlayerID, int nGroupIndex, int nModifyType, BYTE* pbyData, size_t uDataLen);
    
    void ProcessQuitRequest(DWORD dwPlayerID);
    void ProcessChangeMemberGroupRequest(DWORD dwPlayerID, DWORD dwTargetID, int nTargetGroup);
    void ProcessChangeMasterRequest(DWORD dwPlayerID, DWORD dwNewMasterID);
    void ProcessChangeTongMemberRemarkRequest(DWORD dwPlayerID, DWORD dwTargetID, char szRemark[]);
    void ProcessSaveMoneyInTong(KRole* pRole, int nMoney);
    void ProcessRenameConflictTong(int nConnIndex, DWORD dwPlayerID, DWORD dwTongID, const char cszNewName[]);

    void OnPlayerEnterGame(KRole* pRole);
    void OnPlayerLeaveGame(KRole* pRole);
    void SaveAll();

    void SendTongEventNotify(int nConnIndex, DWORD dwPlayerID, TONG_EVENT_NOTIFY_CODE eCode);
    void SendTongEventNotify(int nConnIndex, DWORD dwPlayerID, TONG_EVENT_NOTIFY_CODE eCode, const char cszMsg[]);
    void SendTongEventNotify(int nConnIndex, DWORD dwPlayerID, TONG_EVENT_NOTIFY_CODE eCode, int nParam);
    
private:
    BOOL LoadOperationConfig();
    BOOL LoadSchemaTemplate();

    BOOL OnJoinTong(DWORD dwPlayerID, DWORD dwTongID);
    
private:
    typedef KMemory::KAllocator<std::pair<DWORD, KTong> > KTONG_TABLE_ALLOCTOR;
    typedef std::map<DWORD, KTong, less<DWORD>, KTONG_TABLE_ALLOCTOR>  KTONG_TABLE;

    KTONG_TABLE                     m_TongTable;
    
    std::vector<KTongSchemaData>    m_SchemaTemplate;

public:
    int                             m_nOperationIndex[totTotal];

    struct KTONG_JOIN_APPLICATION
    {
        DWORD   dwTongID;
        DWORD   InviterID;
        time_t  nTimeOut;
    };
    typedef KMemory::KAllocator<std::pair<DWORD, KTONG_JOIN_APPLICATION> > KTONG_JOIN_APPLICATION_TABLE_ALLOCTOR;
    typedef std::map<DWORD, KTONG_JOIN_APPLICATION, less<DWORD>, KTONG_JOIN_APPLICATION_TABLE_ALLOCTOR> KTONG_JOIN_APPLICATION_TABLE;

    KTONG_JOIN_APPLICATION_TABLE    m_InviteApplication;
};


#endif	// _TONG_MANAGER_H_
