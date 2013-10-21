#include "StdAfx.h"
#include "KSO3WorldInterface.h"

KSO3WorldInterface g_SO3WorldInterface;

KSO3WorldInterface::KSO3WorldInterface(void)
{
	OnNpcHurt = NULL;
	OnPlayerHurt = NULL;
}

KSO3WorldInterface::~KSO3WorldInterface(void)
{
}
