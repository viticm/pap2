// File: ConsoleInput.cpp
// Desc:

#include"ConsoleInput.h"
#include"CommonDLLFuncs.h"
#include"String.h"

#include<conio.h>

namespace Common
{

bool WaitInputND(unsigned int WaitMS, char ExpectChar, bool bCase)
{
	unsigned int BeginTime = g_GetDateTimeSystem()->GetTimeValueMS();
	while(!kbhit())
	{
		if((g_GetDateTimeSystem()->GetTimeValueMS() - BeginTime) > WaitMS)
		{
			return false;
		}
	}

	char InputChar = getch();

	if(ExpectChar == 0)
	{
		return true;
	}
	else
	{
		if(InputChar == ExpectChar)
		{
			return true;
		}

		if(!bCase)
		{
			if(InputChar == Common::GetAnotherCase(ExpectChar))
			{
				return true;
			}
		}
	}

	return false;
}

bool WaitInputD(unsigned int WaitMS, char ExpectChar, bool bCase)
{
	unsigned int BeginTime = g_GetDateTimeSystem()->GetTimeValueMS();
	while(!kbhit())
	{
		if((g_GetDateTimeSystem()->GetTimeValueMS() - BeginTime) > WaitMS)
		{
			return false;
		}
	}

	char InputChar = getche();

	if(ExpectChar == 0)
	{
		return true;
	}
	else
	{
		if(InputChar == ExpectChar)
		{
			return true;
		}

		if(!bCase)
		{
			if(InputChar == GetAnotherCase(ExpectChar))
			{
				return true;
			}
		}
	}

	return false;
}

}  // namespace