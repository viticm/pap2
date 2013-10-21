///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName : main
// Creator  : ChenShiCai
// Date     : 2008-10-22
// Comment  : 
//
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cppunit/kg_cppunit.h"
#include "KMathFunctionTest.h"

int main(int argc, char* argv[])
{
    int nResult = false;
    int nRetCode = false; 
    CppUnit::KG_Cppunit TestRunner("KMathFunctionTest");
    std::string strOutputContent[1];
    TestRunner.RunTest();
    TestRunner.Output();

    nResult = true;
Exit0:
    return nResult ? 0 : 1;
}



