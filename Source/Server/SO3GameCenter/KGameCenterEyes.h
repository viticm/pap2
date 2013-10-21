
/************************************************************************/
/* Copyright : Kingsoft 2008										    */
/* Author	 : Zhao chunfeng, Xia yong								    */
/* FSEyes м╗педё©И                                                      */
/************************************************************************/
#ifndef _KGAMECENTEREYES_H_ 
#define _KGAMECENTEREYES_H_
#include "FSEyes/kipcstream.h"
#include "FSEyes/fseye_protocol.h"

class KGameCenterEyes
{
public:
    KGameCenterEyes();
    ~KGameCenterEyes();
    
    BOOL Init();
    void UnInit();

    void Activate();
    
private:
    KIpcStream  m_Stream;
    time_t      m_nNextPingTime;
    time_t      m_nNextSendPlayerTime;
    time_t      m_nNextSendCustomInfoTime;
    float       m_fFluxTotal;
    float       m_fMainLoopIdle;
    float       m_fRoleDBIdle;
    float       m_fMiscDBIdle;

private:
    typedef void (KGameCenterEyes::*PROTOCOL_FUNCTION)(BYTE* pbyData, size_t uDataLen);
    PROTOCOL_FUNCTION m_ProtocolFunctions[UCHAR_MAX + 1];

    void OnE2LStop(BYTE* pbyData, size_t uDataLen);
    void OnE2LGetPid(BYTE* pbyData, size_t uDataLen);
    void OnG2LProtocol(BYTE* pbyData, size_t uDataLen);
    void OnE2LProtocol(BYTE* pbyData, size_t uDataLen);
    void OnE2LGmCommandGS(BYTE* pbyData, size_t uDataLen);
    void OnE2LGmCommandGC(BYTE* pbyData, size_t uDataLen);
    void OnE2LRemoteLuaCall(BYTE* pbyData, size_t uDataLen);

public:
    void DoPing();
    void DoSendPlayerCount();
    void DoSendCustomInfo();
    void DoSendGmMessage(const char cszGmName[], const char cszMessage[]);
    void DoSendGmChn(ProtocolDef eType, const char cszAccount[], const char cszRole[], const char cszText[]);
    void DoSendReport(int nType, const char cszKey[], const char cszValue[]);
    void DoSendPrizeInfo(const char szAccount[], const char szRoleName[], DWORD dwRoleID, const char szType[], int nValue);
};

#endif
