#ifndef _LORD_H_
#define _LORD_H_

#include "ksdef.h"
#include "klordguard.h"
#include "kclientipcstream.h"

class Lord
{
public:
	Lord();
	BOOL Initialize();
	VOID UnInitialize();
	VOID Run();
private:
	KLordGuard			m_cGuard;
	KClientIpcStream	m_cStream;
};

#endif
