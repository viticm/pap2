#include "stdafx.h"
#include "def.h"
#include "fseye_protocol.h"
#include "klordguard.h"
#ifndef WIN32
#include <unistd.h>
#endif

//#include "mespump.h"
#include "lord.h"

BOOL g_bRunning = TRUE;

int main()
{
	Lord cLord;
	if (!cLord.Initialize())
	{
		printf("Failed to initialize!\n");
		return -1;
	}
	while (g_bRunning)
	{
		cLord.Run();
#ifdef WIN32
		Sleep(1000);
#else
		usleep(50000);
#endif
	}
	cLord.UnInitialize();
	return 0;
}
