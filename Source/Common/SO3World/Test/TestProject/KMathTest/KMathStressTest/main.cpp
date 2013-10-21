///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName : KMathTest
// Creator  : ChenShiCai
// Date     : 2008-10-21
// Comment  : 
//
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KMathStressTest.h"


int main(int argc, char* argv[])
{
    int nRetCode = false;
    KMathStressTest MathStressTest;
    int nMathStressTestFlag = false;

#ifndef TEST_NEW_VERSION
    nRetCode = g_InitMath();
    KG_PROCESS_ERROR(nRetCode);
#endif

    nRetCode = MathStressTest.Init();
    KG_PROCESS_ERROR(nRetCode);
    nMathStressTestFlag = true;

    for (int i = 0; i < 1024; ++i)
    {
        nRetCode = MathStressTest.Run();
        KG_PROCESS_ERROR(nRetCode);
    }   

Exit0:
    if (nMathStressTestFlag)
    {
        MathStressTest.UnInit();
        nMathStressTestFlag = false;
    }
#ifndef TEST_NEW_VERSION
    g_UnInitMath();
#endif
    return 0;
}



