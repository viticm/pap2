#ifndef _KGUARDSIMULATOR_H_ 
#define _KGUARDSIMULATOR_H_
#include "FSEyes\kipcstream.h"

class KGuardSimulator
{
public:
    KGuardSimulator();
    ~KGuardSimulator();

    BOOL Init();
    void UnInit();

    void DoE2LStop();
    BOOL CheckE2LStop(HANDLE hProcess, DWORD dwWaitTime);
    void DoE2LGetPid();
    BOOL CheckE2LGetPid(int nProcessID, time_t* pTime = NULL);
    void DoG2LProtocol();
    BOOL CheckG2LProtocol(int nProcessID, time_t* pTime = NULL);
    BOOL CheckPlayerCount(int nPlayerCount, time_t* pTime = NULL);

    BOOL DoE2LGmCommandGS(const char szGmName[], const char szRoleName[], const char szCommand[]);
    BOOL DoE2LGmCommandGC(const char szGmName[], const char szCommand[]);

	bool DoE2LRemoteLuaCall(char   szFunction[], size_t uParamLen, BYTE* byParam);
    BOOL CheckMessage(const char  szGmName[], const char szMessage[], time_t* pTime = NULL);
private:
    KIpcStream  m_Stream;

private:
    BYTE* GetData(BYTE byProtocolID, BYTE bySubProtocolID = 0, time_t* pTime = NULL);

    static void WorkFucntion(void *pvParam);
    void ThreadFunction();

private:
    struct RECEIVE_DATA 
    {
        RECEIVE_DATA() : pData(NULL), tReceiveTime(0)
        {

        };
        BYTE* pData;
        time_t tReceiveTime;
    };

    RECEIVE_DATA m_ReceiveDataCach[UCHAR_MAX + 1][UCHAR_MAX + 1];

    KThread m_Thread;
    BOOL m_RunFlag;
};
#endif
