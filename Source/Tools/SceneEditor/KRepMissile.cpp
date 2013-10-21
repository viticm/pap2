// KRepMissile.cpp: implementation of the KRepMissile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "KRepMissile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KRepMissile::KRepMissile()
{

}

KRepMissile::~KRepMissile()
{

}

HRESULT KRepMissile::Render()
{
	return KComponentObject::Render();
}