#include "stdafx.h"
#include "def.h"
#include "fseye_protocol.h"

//#include "mespump.h"

#include "lord.h"

Lord::Lord()
{
}

BOOL Lord::Initialize()
{
	if (!m_cStream.Initialize())
		return FALSE;
	return m_cGuard.Initialize(&m_cStream);
}

VOID Lord::UnInitialize()
{
	m_cGuard.UnInitialize();
	m_cStream.UnInitialize();
}

VOID Lord::Run()
{
	m_cGuard.Activate();
}
