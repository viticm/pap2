////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRenderSorterStrategeDefault.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-14 16:24:34
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DRenderSorterStrategeDefault.h"
#include "KG3DRenderData.h"
#include "KG3DRendererDefault.h"
////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HRESULT KG3DRenderSorterStrategeDefault::PutIntoRenderData( KG3DRepresentObject& Obj, KG3DRenderData& Data )
{
	return Data.AddObj(Obj, KG3DRendererDefault());
} 