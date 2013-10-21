///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName : KMathStressTestControl
// Creator  : ChenShiCai
// Date     : 2008-10-28
// Comment  : 
//
///////////////////////////////////////////////////////////////

#include "KG_Socket.h"
class KMathStressTestControl
{
public:
    int Init();
    int Run();
    void UnInit();

private:
    void SaveTestResult(IKG_SocketStream *piStream,  FILE *pFile);

private:
    KG_SocketServerAcceptor m_Acceptor;
};

