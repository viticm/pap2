///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName : KMathStressTest
// Creator  : ChenShiCai
// Date     : 2008-10-27
// Comment  : 
//
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KMathStressTest.h"
#include "KG_Socket.h"
#include <limits.h>

#define MAX_X 131072
#define MAX_Y 131072
#define MAX_Z 65536

int KMathStressTest::Init()
{
	int nResult = false;    
	int nRetCode = false;
    KG_SocketConnector SocketConnector;

    m_piSocketStream = NULL;

    m_pGetDistance3_ParaArray = NULL;
    m_pGetDistance2_ParaArray = NULL;
    m_pGetDirection_ParaArray = NULL;
    m_pInRange_Para_ParaArray = NULL;
    m_pSin_ParaArray = NULL;

    m_piSocketStream = SocketConnector.ConnectSecurity(SERVER_IP, SERVER_PORT, KSG_ENCODE_DECODE);
    KG_PROCESS_ERROR(m_piSocketStream);

    m_pGetDistance3_ParaArray = new g_GetDistance3_Para[KMATH_TEST_ARRAYSIZE];
    KG_PROCESS_ERROR(m_pGetDistance3_ParaArray);

    m_pGetDistance2_ParaArray = new g_GetDistance2_Para[KMATH_TEST_ARRAYSIZE];
    KG_PROCESS_ERROR(m_pGetDistance2_ParaArray);

    m_pGetDirection_ParaArray = new g_GetDirection_Para[KMATH_TEST_ARRAYSIZE];
    KG_PROCESS_ERROR(m_pGetDirection_ParaArray);

    m_pInRange_Para_ParaArray = new g_InRange_Para[KMATH_TEST_ARRAYSIZE];
    KG_PROCESS_ERROR(m_pInRange_Para_ParaArray);

    m_pSin_ParaArray = new g_Sin_Para[KMATH_TEST_ARRAYSIZE];
    KG_PROCESS_ERROR(m_pSin_ParaArray);

    m_pnRandomArray = new int[KMATH_TEST_ARRAYSIZE];
    KG_PROCESS_ERROR(m_pnRandomArray);

	nResult = true;
Exit0:
    if (!nResult)
    {
        UnInit();
    }
	return nResult;
}

void KMathStressTest::UnInit()
{
    KG_DELETE_ARRAY(m_pnRandomArray);
    KG_DELETE_ARRAY(m_pSin_ParaArray);
    KG_DELETE_ARRAY(m_pInRange_Para_ParaArray);
    KG_DELETE_ARRAY(m_pGetDirection_ParaArray);
    KG_DELETE_ARRAY(m_pGetDistance2_ParaArray);
    KG_DELETE_ARRAY(m_pGetDistance3_ParaArray);
    KG_COM_RELEASE(m_piSocketStream);
}

int KMathStressTest::Run()
{
	int          nResult            = false;    
	int          nRetCode           = false;
    unsigned     uGetTickCountBegin = 0;
    unsigned     uGetTickCountEnd   = 0;
    IKG_Buffer  *piBuffer           = NULL;
    C2S_COMMAND *pCommand           = NULL;
    Test_Result  TestResult;


    TestResult.nVersion = VERSION;

    nRetCode = GetPara();
    KG_PROCESS_ERROR(nRetCode);    

    //1
    uGetTickCountBegin = GetTickCount();
    for (int i = 0; i < KMATH_TEST_ARRAYSIZE; ++i)
    {
        g_GetDirection_Para *pParaArray = m_pGetDirection_ParaArray + m_pnRandomArray[i];
        g_GetDirection(0, 0, pParaArray->nX, pParaArray->nY);
    }
    uGetTickCountEnd = GetTickCount();
    TestResult.uGetDirectionTime = uGetTickCountEnd - uGetTickCountBegin;

    //2
    uGetTickCountBegin = GetTickCount();
    for (int i = 0; i < KMATH_TEST_ARRAYSIZE; ++i)
    {
        g_GetDistance2_Para *pParaArray = m_pGetDistance2_ParaArray + m_pnRandomArray[i];
        g_GetDistance2(pParaArray->nSourceX, pParaArray->nSourceY, pParaArray->nDestX, pParaArray->nDestY);
    }
    uGetTickCountEnd = GetTickCount();
    TestResult.uGetDistance2Time = uGetTickCountEnd - uGetTickCountBegin;

    //3
    uGetTickCountBegin = GetTickCount();
    for (int i = 0; i < KMATH_TEST_ARRAYSIZE; ++i)
    {
        g_GetDistance3_Para *pParaArray = m_pGetDistance3_ParaArray + m_pnRandomArray[i];
        g_GetDistance3(pParaArray->nSourceX, pParaArray->nSourceY, pParaArray->nSourceZ, pParaArray->nDestX, pParaArray->nDestY, pParaArray->nDestZ);
    }
    uGetTickCountEnd = GetTickCount();
    TestResult.uGetDistance3Time = uGetTickCountEnd - uGetTickCountBegin;

    //4
    uGetTickCountBegin = GetTickCount();
    for (int i = 0; i < KMATH_TEST_ARRAYSIZE; ++i)
    {
        g_InRange_Para *pParaArray = m_pInRange_Para_ParaArray + m_pnRandomArray[i];
        g_InRange(pParaArray->nXa, pParaArray->nXb, pParaArray->nZa, pParaArray->nXb, pParaArray->nYb, pParaArray->nZb, pParaArray->nMinRange, pParaArray->nMaxRange);
    }
    uGetTickCountEnd = GetTickCount();
    TestResult.uInRangeTime = uGetTickCountEnd - uGetTickCountBegin;

    //5
    uGetTickCountBegin = GetTickCount();
    for (int i = 0; i < KMATH_TEST_ARRAYSIZE; ++i)
    {
        g_Sin_Para *pParaArray = m_pSin_ParaArray + m_pnRandomArray[i];
        g_Sin(pParaArray->nDirection);
    }
    uGetTickCountEnd = GetTickCount();
    TestResult.uSinTime = uGetTickCountEnd - uGetTickCountBegin;

    piBuffer = KG_MemoryCreateBuffer(sizeof(C2S_COMMAND) + sizeof(TestResult));
    KG_PROCESS_ERROR(piBuffer);

    pCommand = (C2S_COMMAND *)piBuffer->GetData();
    ASSERT(pCommand);

    pCommand->nCommand = CT_Result;
    pCommand->nNum = 0;
    memcpy(pCommand->chData, &TestResult, sizeof(TestResult));

    nRetCode = m_piSocketStream->Send(piBuffer);
    KG_PROCESS_ERROR(nRetCode == 1);   

	nResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
	return nResult;
}

int KMathStressTest::Random_RandomArray(int *nRetData)
{
	int nResult = false;    
	int nRetCode = false;
	
    nRetCode = g_Random(KMATH_TEST_ARRAYSIZE);
    ASSERT(nRetCode > - 1);
    ASSERT(nRetCode < KMATH_TEST_ARRAYSIZE);

    *nRetData = nRetCode;
	nResult = true;
//Exit0:
	return nResult;
}

int KMathStressTest::Random_SinArray(g_Sin_Para *nRetData)
{
    int nResult = false;    
    int nRetCode = false;

    nRetData->nDirection = g_Random(INT_MAX);

    nResult = true;
//Exit0:
    return nResult;
}

int KMathStressTest::Random_DirectionArray(g_GetDirection_Para *nRetData)
{
    int nResult = false;    
    int nRetCode = false;

    nRetData->nX = g_Random(MAX_X);
    nRetData->nY = g_Random(MAX_Y);

    nResult = true;
//Exit0:
    return nResult;
}

int KMathStressTest::Random_Distance2Array(g_GetDistance2_Para *nRetData)
{
    int nResult = false;    
    int nRetCode = false;

    nRetData->nSourceX = g_Random(MAX_X);
    nRetData->nSourceY = g_Random(MAX_Y);
   
    nRetData->nDestX = g_Random(MAX_X);
    nRetData->nDestY = g_Random(MAX_Y);

    nResult = true;
//Exit0:
    return nResult;
}

int KMathStressTest::Random_Distance3Array(g_GetDistance3_Para *nRetData)
{
    int nResult = false;    
    int nRetCode = false;

    nRetData->nSourceX = g_Random(MAX_X);
    nRetData->nSourceY = g_Random(MAX_Y);
    nRetData->nSourceZ = g_Random(MAX_Z);

    nRetData->nDestX = g_Random(MAX_X);
    nRetData->nDestY = g_Random(MAX_Y);
    nRetData->nDestZ = g_Random(MAX_Z);


    nResult = true;
//Exit0:
    return nResult;
}

int KMathStressTest::Random_InRangeArray(g_InRange_Para *nRetData)
{
    int nResult = false;    
    int nRetCode = false;

    nRetData->nXa = g_Random(MAX_X);
    nRetData->nYa = g_Random(MAX_Y);
    nRetData->nZa = g_Random(MAX_Z);
    nRetData->nXb = g_Random(MAX_X);
    nRetData->nYb = g_Random(MAX_Y);
    nRetData->nZb = g_Random(MAX_Z);

    nRetData->nMinRange = 0;
    nRetData->nMaxRange = g_Random(MAX_X);

    nResult = true;
//Exit0:
    return nResult;
}

int KMathStressTest::GetPara() 
{
	int nResult = false;    
	int nRetCode = false;
	
    for (int i = 0; i < KMATH_TEST_ARRAYSIZE; ++i)
    {
        nRetCode = Random_RandomArray(m_pnRandomArray + i);
        KG_PROCESS_ERROR(nRetCode);

        nRetCode = Random_SinArray(m_pSin_ParaArray + i);
        KG_PROCESS_ERROR(nRetCode);

        nRetCode = Random_DirectionArray(m_pGetDirection_ParaArray + i);
        KG_PROCESS_ERROR(nRetCode);

        nRetCode = Random_Distance2Array(m_pGetDistance2_ParaArray + i);
        KG_PROCESS_ERROR(nRetCode);

        nRetCode = Random_Distance3Array(m_pGetDistance3_ParaArray + i);
        KG_PROCESS_ERROR(nRetCode);

        nRetCode = Random_InRangeArray(m_pInRange_Para_ParaArray + i);
        KG_PROCESS_ERROR(nRetCode);
    }    

	nResult = true;
Exit0:
	return nResult;
}


//int KMathStressTest::GetPara() 
//{
//	int nResult = false;    
//	int nRetCode = false;
//
//    KG_PROCESS_ERROR(m_pSin_ParaArray);
//    KG_PROCESS_ERROR(m_pInRange_Para_ParaArray);
//    KG_PROCESS_ERROR(m_pGetDirection_ParaArray);
//    KG_PROCESS_ERROR(m_pGetDistance2_ParaArray);
//    KG_PROCESS_ERROR(m_pGetDistance3_ParaArray);
//
//    nRetCode = SendCommand(CT_Random, KMATH_TEST_ARRAYSIZE);
//    KG_PROCESS_ERROR(nRetCode);
//
//    for (int i = 0; i < KMATH_TEST_ARRAYSIZE; ++i)
//    {
//        nRetCode = RecvCommand(CT_Random, i, (m_pnRandomArray + i), sizeof(int));
//        KG_PROCESS_ERROR(nRetCode);
//        KG_PROCESS_ERROR(m_pnRandomArray[i] < KMATH_TEST_ARRAYSIZE);
//        KG_PROCESS_ERROR(m_pnRandomArray[i] > -1);
//
//    }
//
//    nRetCode = SendCommand(CT_Sin, KMATH_TEST_ARRAYSIZE);
//    KG_PROCESS_ERROR(nRetCode);
//
//    for (int i = 0; i < KMATH_TEST_ARRAYSIZE; ++i)
//    {
//        nRetCode = RecvCommand(CT_Sin, i, (m_pSin_ParaArray + i), sizeof(int));
//        KG_PROCESS_ERROR(nRetCode);
//    }
//
//
//    for (int i = 0; i < KMATH_TEST_ARRAYSIZE; ++i)
//    {
//        nRetCode = RecvCommand(CT_InRange, i, (m_pInRange_Para_ParaArray + i), sizeof(int));
//        KG_PROCESS_ERROR(nRetCode);
//    }
//
//    nRetCode = SendCommand(CT_GetDirection, KMATH_TEST_ARRAYSIZE);
//    KG_PROCESS_ERROR(nRetCode);
//
//    for (int i = 0; i < KMATH_TEST_ARRAYSIZE; ++i)
//    {
//        nRetCode = RecvCommand(CT_GetDirection, i, (m_pGetDirection_ParaArray + i), sizeof(int));
//        KG_PROCESS_ERROR(nRetCode);
//    }
//
//    nRetCode = SendCommand(CT_GetDistance2, KMATH_TEST_ARRAYSIZE);
//    KG_PROCESS_ERROR(nRetCode);
//
//    for (int i = 0; i < KMATH_TEST_ARRAYSIZE; ++i)
//    {
//        nRetCode = RecvCommand(CT_GetDistance2, i, (m_pGetDistance2_ParaArray + i), sizeof(int));
//        KG_PROCESS_ERROR(nRetCode);
//    }
//
//    nRetCode = SendCommand(CT_GetDistance3, KMATH_TEST_ARRAYSIZE);
//    KG_PROCESS_ERROR(nRetCode);
//
//    for (int i = 0; i < KMATH_TEST_ARRAYSIZE; ++i)
//    {
//        nRetCode = RecvCommand(CT_GetDistance3, i, (m_pGetDistance3_ParaArray + i), sizeof(int));
//        KG_PROCESS_ERROR(nRetCode);
//    }
//
//	nResult = true;
//Exit0:
//	return nResult;
//}

//int KMathStressTest::RecvCommand(int nCommandType, int nGetCount, void *pvRetData, int nDataSize)
//{
//	int nResult = false;    
//	int nRetCode = false;
//    IKG_Buffer *piBuffer = NULL;
//    S2C_COMMAND *pCommand = NULL;
//
//    KG_PROCESS_ERROR(m_piSocketStream);
//
//    nRetCode = m_piSocketStream->Recv(&piBuffer);
//    KG_PROCESS_ERROR(nRetCode == 1);
//    KG_PROCESS_ERROR(piBuffer->GetSize() == (nDataSize + sizeof(S2C_COMMAND)));
//    
//    pCommand = (S2C_COMMAND *)piBuffer->GetData();
//    ASSERT(pCommand);
//
//    KG_PROCESS_ERROR(pCommand->nCommand == nCommandType);
//    KG_PROCESS_ERROR(pCommand->nNum == nCommandType);
//
//    memcpy(pvRetData, pCommand->chData, nDataSize);   
//
//	nResult = true;
//Exit0:
//	return nResult;
//}
//
//int KMathStressTest::SendCommand(int nCommandType, int nGetCount) 
//{
//	int nResult = false;    
//	int nRetCode = false;
//    IKG_Buffer *piBuffer = NULL;
//    C2S_COMMAND *pCommand = NULL;
//
//    KG_PROCESS_ERROR(m_piSocketStream);
//
//    piBuffer = KG_MemoryCreateBuffer(sizeof(C2S_COMMAND));
//    KG_PROCESS_ERROR(piBuffer);
//
//    pCommand = (C2S_COMMAND *)piBuffer->GetData();
//    ASSERT(pCommand);
//
//    pCommand->nCommand = nCommandType;
//    pCommand->nNum = nGetCount;
//    
//    nRetCode = m_piSocketStream->Send(piBuffer);
//    KG_PROCESS_ERROR(nRetCode == 1);
//
//	nResult = true;
//Exit0:
//	return nResult;
//}
//
