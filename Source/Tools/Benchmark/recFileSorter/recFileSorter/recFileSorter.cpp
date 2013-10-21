// recFileSorter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "recFileSorter.h"

#include "Core.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

//using namespace std;
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
        if (argc > 1)
        {
            LPCTSTR strError = coreFunc(argv[1]);
            if(NULL == strError)
            {
                std::cout << _T("\nfile resort ok!") << std::endl;
            }
            else
            {
                std::cout << _T("\nfile resort error : ") << strError << std::endl;
            }
        }

        ::system(_T("pause"));
       
	}

	return nRetCode;
}


