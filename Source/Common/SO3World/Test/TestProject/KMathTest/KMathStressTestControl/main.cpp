///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName : main
// Creator  : ChenShiCai
// Date     : 2008-10-28
// Comment  : 
//
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KMathStressTestControl.h"

int main(int argc, char* argv[])
{
    int nResult = false;    
    int nRetCode = false;
    KMathStressTestControl TestControl;
    int nTestControlInitFlag = false;

    nRetCode = TestControl.Init();
    KG_PROCESS_ERROR(nRetCode);
    nTestControlInitFlag = true;

    nRetCode = TestControl.Run();
    KG_PROCESS_ERROR(nRetCode);

    
    nResult = true;
Exit0:
    if (nTestControlInitFlag)
    {
        TestControl.UnInit();
        nTestControlInitFlag = false;
    }    
    return nResult ? 0 : 1;
}



