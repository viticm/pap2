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

#ifdef KMathTest

int main(int argc, char* argv[])
{
    int nRetCode = false;

#ifndef TEST_NEW_VERSION
    nRetCode = g_InitMath();
    KG_PROCESS_ERROR(nRetCode);
#endif

Exit0:
#ifndef TEST_NEW_VERSION
    g_UnInitMath();
#endif
    return 0;
}

#endif


