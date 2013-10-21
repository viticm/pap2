////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : ConsoleMode.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-5-12 10:05:35
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "ConsoleMode.h"


////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <io.h>
#include <Fcntl.h>

int SetAppIntoConsoleMode()
{
	int nResult  = false;
	int nRetCode = false;
	int fdConsoleHandle = -1;
	HANDLE hStdHandle = INVALID_HANDLE_VALUE;
	FILE *fpConsole = NULL;

	//nRetCode = ::AllocConsole();
	nRetCode = AttachConsole(ATTACH_PARENT_PROCESS);
	PROCESS_ERROR(nRetCode);

	// redirect unbuffered STDOUT to the console
	hStdHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
	PROCESS_ERROR(hStdHandle != INVALID_HANDLE_VALUE);

	fdConsoleHandle = _open_osfhandle((intptr_t)hStdHandle, _O_TEXT);
	PROCESS_ERROR(fdConsoleHandle != -1);

	fpConsole = _fdopen(fdConsoleHandle, "w");
	PROCESS_ERROR(fpConsole);

	*stdout = *fpConsole;
	setvbuf( stdout, NULL, _IONBF, 0 );

	// redirect unbuffered STDIN to the console
	hStdHandle = ::GetStdHandle(STD_INPUT_HANDLE);
	PROCESS_ERROR(hStdHandle != INVALID_HANDLE_VALUE);

	fdConsoleHandle = _open_osfhandle((intptr_t)hStdHandle, _O_TEXT);
	PROCESS_ERROR(fdConsoleHandle != -1);

	fpConsole = _fdopen(fdConsoleHandle, "r");
	PROCESS_ERROR(fpConsole);

	*stdin = *fpConsole;
	setvbuf(stdin, NULL, _IONBF, 0);

	// redirect unbuffered STDERR to the console
	hStdHandle = ::GetStdHandle(STD_ERROR_HANDLE);
	PROCESS_ERROR(hStdHandle != INVALID_HANDLE_VALUE);

	fdConsoleHandle = _open_osfhandle((intptr_t)hStdHandle, _O_TEXT);
	PROCESS_ERROR(fdConsoleHandle != -1);

	fpConsole = _fdopen(fdConsoleHandle, "w");
	PROCESS_ERROR(fpConsole);

	*stderr = *fpConsole;
	setvbuf(stderr, NULL, _IONBF, 0);

	nResult = true;
Exit0:
	return nResult;
}