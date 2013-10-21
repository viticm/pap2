/*
**************************************************************************
*
* Project		: SceneEditor
* FileName		: KRigidBodyTable.cpp
* Coded by		: YangLin
* Date			: 2004-05-27
* Comment	    : NONE
* Copyright 2004 Kingsoft Corp.
* All Rights Reserved.
*
**************************************************************************
*/
// KRigidBodyTable.cpp: implementation of the KRigidBodyTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "KRigidBodyTable.h"
//#include ".\krigidbodytable.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace Physics
{
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KRigidBodyTable::KRigidBodyTable()
{

}

KRigidBodyTable::~KRigidBodyTable()
{

}

//创建一个新的刚体
HRESULT KRigidBodyTable::Get1NewRigidBody(LPRIGIDBODY* ppRigid)
{
	LPRIGIDBODY pNewRigid = new KRigidBody;
	vecRigidBody.push_back(pNewRigid);
	(*ppRigid) = pNewRigid;
	return S_OK;
}


HRESULT KRigidBodyTable::CleanUp(void)
{
	for(DWORD i=0;i<vecRigidBody.size();i++)
	{
		LPRIGIDBODY pRigid = vecRigidBody[i];

		SAFE_DELETE(pRigid);
		
	}
	vecRigidBody.clear();
	return S_OK;
}

};