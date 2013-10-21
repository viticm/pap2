// UpdateCore.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "IUpdate.h"
#include "KUpdate.h"

IUpdate* CreateUpdate()
{
	return (IUpdate *)new KUpdate();
}

void DestroyUpdate(IUpdate *pUpdate)
{
	delete (KUpdate *)pUpdate;
}