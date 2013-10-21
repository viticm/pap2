#ifndef _KCLIENT_CONSOLE_H_
#define _KCLIENT_CONSOLE_H_

#include "KSO3World.h"

class KClientConsole
{
public:
	KClientConsole(void);
	~KClientConsole(void);

	int Input(const char* pszMessage);
};
#endif	//_KCLIENT_CONSOLE_H_