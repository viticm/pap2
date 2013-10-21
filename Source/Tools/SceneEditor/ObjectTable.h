// ObjectTable.h: interface for the KComponentObjectTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTTABLE_H__7FFF9929_D931_49E7_A348_8A5BE7122948__INCLUDED_)
#define AFX_OBJECTTABLE_H__7FFF9929_D931_49E7_A348_8A5BE7122948__INCLUDED_

#include "TextureTable.h"	// Added by ClassView
//#include "XMeshTable.h"	// Added by ClassView
#include "DataObject.h"

#include "KRepNPC.h"
#include "./KRepGirlNo1.h"
#include "KEntity.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class KComponentObjectTable  
{
public:
	KComponentObjectTable();
	virtual ~KComponentObjectTable();

private:
	DWORD     m_dMaxNumObject;
	LPOBJECT* m_lpObjects;
	
public:
	HRESULT LoadbjectFromFile(LPOBJECT *ppOutObject,LPSTR pFileName);
	HRESULT CopyObject(LPOBJECT *ppOutObject,LPOBJECT pInObject);
	HRESULT SetMeshTable(KModelTable* pTable);
	virtual HRESULT CleanUp();
	KModelTable* m_lpMeshTable;

	HRESULT GetObject(LPOBJECT* ppObject,DWORD ID);
	HRESULT Get1NewObject(LPOBJECT* ppObject,DWORD Type);
	HRESULT Get1NewObjectPointer(LPOBJECT** pppObject);
	DWORD   m_dNumObject;

};

#endif // !defined(AFX_OBJECTTABLE_H__7FFF9929_D931_49E7_A348_8A5BE7122948__INCLUDED_)
