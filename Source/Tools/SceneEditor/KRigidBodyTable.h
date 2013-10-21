// KRigidBodyTable.h: interface for the KRigidBodyTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KRIGIDBODYTABLE_H__182546A6_6731_4B77_9F8C_E9288B33E52F__INCLUDED_)
#define AFX_KRIGIDBODYTABLE_H__182546A6_6731_4B77_9F8C_E9288B33E52F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "KRigidBody.h"

namespace Physics
{
class KRigidBodyTable  
{
public:
	vector<LPRIGIDBODY>vecRigidBody;

public:
	HRESULT Get1NewRigidBody(LPRIGIDBODY* ppRigid);//创建一个新的刚体

	KRigidBodyTable();
	virtual ~KRigidBodyTable();

	HRESULT CleanUp(void);
};

};
#endif // !defined(AFX_KRIGIDBODYTABLE_H__182546A6_6731_4B77_9F8C_E9288B33E52F__INCLUDED_)
