///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName : KMathStressTest
// Creator  : ChenShiCai
// Date     : 2008-10-27
// Comment  : 
//
///////////////////////////////////////////////////////////////

struct IKG_SocketStream;

class KMathStressTest
{
public:
    int Init();
    void UnInit();
    int Run();

private:
    int GetPara();
    //int SendCommand(int nCommandType, int nGetCount);
    //int RecvCommand(int nCommandType, int nGetCount, void *pvRetData, int nDataSize);

    int Random_Distance3Array(g_GetDistance3_Para *nRetData);
    int Random_Distance2Array(g_GetDistance2_Para *nRetData);
    int Random_DirectionArray(g_GetDirection_Para *nRetData);
    int Random_SinArray(g_Sin_Para *nRetData);
    int Random_RandomArray(int *nRetData);
    int Random_InRangeArray(g_InRange_Para *nRetData);

private:
    IKG_SocketStream *m_piSocketStream;

    int *m_pnRandomArray;
    g_GetDistance3_Para *m_pGetDistance3_ParaArray;
    g_GetDistance2_Para *m_pGetDistance2_ParaArray;
    g_GetDirection_Para *m_pGetDirection_ParaArray;
    g_InRange_Para *m_pInRange_Para_ParaArray;
    g_Sin_Para *m_pSin_ParaArray;
};
