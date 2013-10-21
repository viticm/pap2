
/************************************************************************/
/* Copyright : Kingsoft 2008										    */
/* Author	 : Zhao chunfeng, Xia yong								    */
/* FSEyes м╗педё©И                                                      */
/************************************************************************/
#ifndef _KGAMESERVEREYES_H_ 
#define _KGAMESERVEREYES_H_

#include "FSEyes/kipcstream.h"

class KGameServerEyes
{
public:
    KGameServerEyes();
    ~KGameServerEyes(){};
    
    BOOL Init();
    void UnInit();

    void Activate();
    
private:
    KIpcStream  m_Stream;
    time_t      m_nNextPingTime;
    time_t      m_nNextSendPlayerTime;
    time_t      m_nNextSendCustomInfoTime;
    time_t      m_nNextSendMapInfoTime;
    float       m_fLastPerformance;
    float       m_fLastClientFlux;
    char        m_szSelfName[PATH_MAX];

    BYTE*       m_pbyTempData;

private:
    typedef void (KGameServerEyes::*PROTOCOL_FUNCTION)(BYTE* pbyData, size_t uDataLen);
    PROTOCOL_FUNCTION m_ProtocolFunctions[UCHAR_MAX + 1];

    void OnE2LGetPid(BYTE* pbyData, size_t uDataLen);
    void OnE2LStop(BYTE* pbyData, size_t uDataLen);
    void OnG2LProtocol(BYTE* pbyData, size_t uDataLen);

private:
    void Ping();
    void SendPlayerCount();
    void SendCustomInfo();
    void SendMapInfo();
};
#endif
