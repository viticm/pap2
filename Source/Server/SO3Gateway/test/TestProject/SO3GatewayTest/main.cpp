///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName : main.cpp
// Creator  : liuzhibiao
// Date     : 2008-12-8
// Comment  : v1.1
//            2008-11-25    v1.0 定义控制台应用程序的入口点   
//            2008-12-8     v1.1 加入兼容linux的代码
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma   warning( disable: 4127)
#include <signal.h>

#ifdef WIN32
#include "cppunit/KG_Cppunit.h"
#else
#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/CompilerOutputter.h>
#include "unistd.h"    
#endif

#ifdef WIN32
void SetConsoleInfo()
{
    HANDLE      hConsole    = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD       BufferSize  = {108, 320};
    SMALL_RECT  WinRect     = {0, 0, BufferSize.X - 1, 27};

    SetConsoleScreenBufferSize(hConsole, BufferSize);
    SetConsoleWindowInfo(hConsole, true, &WinRect);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);

}

    #ifdef _DEBUG
    #define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)

    inline void EnableMemLeakCheck()
    {
        _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
    }

    #endif
#endif

int main(int argc, char* argv[])
{
    int         nResult      = false;
    int         nRetCode     = false;    
    int         nLogInitFlag = false;
    KGLOG_PARAM LogParam;  

    #ifdef WIN32
        SetConsoleInfo();
        #ifdef _DEBUG
            EnableMemLeakCheck();
        #endif
    #else
        signal(SIGCLD, SIG_IGN); //忽略 SIGCLD 软中断而不必要 wait()。
    #endif

    setlocale(LC_ALL, "");
    g_SetRootPath(NULL);

    strncpy(LogParam.szPath, "logs", sizeof(LogParam.szPath));
    strncpy(LogParam.szIdent, "SO3GatewayTest", sizeof(LogParam.szIdent));
    LogParam.Options = (KGLOG_OPTIONS)(KGLOG_OPTION_FILE | KGLOG_OPTION_CONSOLE);
    LogParam.nMaxLineEachFile = 160000;

    nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nLogInitFlag = true;  

    //there are two different cppunits in windows and linux
    #ifdef WIN32
    {
        CppUnit::KG_Cppunit TestRunner("SO3Gateway");
        std::string strOutputContent[1];
        TestRunner.RunTest();
        TestRunner.Output();   
    }
    #else
        {
            // Create the event manager and test controller
            CppUnit::TestResult Controller;

            // Add a listener that colllects test result
            CppUnit::TestResultCollector TestResult;
            Controller.addListener(&TestResult);        
            
            // Add a listener that print dots as test run.
            CppUnit::BriefTestProgressListener Progress;
            Controller.addListener(&Progress);      
            
            // Add the top suite to the test runner
            CppUnit::TestRunner Runner;
            Runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
            Runner.run(Controller);

            CppUnit::CompilerOutputter outputter(&TestResult, CppUnit::stdCOut());
            outputter.write(); 
        }
    #endif

    nResult = true;
Exit0:
    if (nLogInitFlag)
    {
        KGLogUnInit(NULL);
        nLogInitFlag = false;
    }
    return nResult ? 0 : 1;
}

