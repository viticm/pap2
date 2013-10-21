#include "StdAfx.h"
#include "KClientConsole.h"

#define COMMAND_SCRIPT		"script"
#define COMMAND_CONTINUE	"continue"
#define COMMAND_EXIT		"exit"

KClientConsole::KClientConsole(void)
{
}

KClientConsole::~KClientConsole(void)
{
}

//返回值
//-1 退出应用程序
//0  退出本次控制台操作
//1  继续
int KClientConsole::Input(const char* pszMessage)
{
	BOOL bRetCode = FALSE;
	char szCommand[256] = "";

	sscanf(pszMessage, "%s", szCommand);

	if (strncmp(szCommand, COMMAND_SCRIPT, sizeof(COMMAND_SCRIPT)) == 0)
	{
		const char* pszScript;
		pszScript = pszMessage + strlen(szCommand) + 1;

		bRetCode =  g_SO3World.m_ScriptCenter.ExcuteScript(pszScript);
		KG_PROCESS_ERROR(bRetCode);
	}
	else if (strncmp(szCommand, COMMAND_CONTINUE, sizeof(COMMAND_CONTINUE)) == 0)
	{
		return 0;
	}
	else if (strncmp(szCommand, COMMAND_EXIT, sizeof(COMMAND_EXIT)) == 0)
	{
		return -1;
	}

	return 1;

Exit0:
	

	return FALSE;
}
