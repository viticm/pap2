// ObjectTable.cpp: implementation of the KComponentObjectTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
//#include "ObjectTable.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KComponentObjectTable::KComponentObjectTable()
{
	m_dMaxNumObject = 0;
	m_dNumObject = 0;
	m_lpObjects = NULL;
	m_lpMeshTable = NULL;
}

KComponentObjectTable::~KComponentObjectTable()
{

}

HRESULT KComponentObjectTable::Get1NewObjectPointer(LPOBJECT** ppObject)
{
	if (m_dMaxNumObject ==0 )
	{
		m_lpObjects = new LPOBJECT[5];
		ZeroMemory(m_lpObjects ,sizeof(LPOBJECT)*5);
		m_dMaxNumObject = 5;
	}
	if (m_dNumObject >= m_dMaxNumObject)
	{
		LPOBJECT* Temp = m_lpObjects;
		m_dMaxNumObject +=5;
		m_lpObjects = new LPOBJECT[m_dMaxNumObject];
		ZeroMemory(m_lpObjects ,sizeof(LPOBJECT)*m_dMaxNumObject);
		for(DWORD i=0;i<m_dMaxNumObject-5 ;i++) m_lpObjects[i] = Temp[i];
		delete [] Temp;
	}	
	m_dNumObject ++;
	(*ppObject) = &m_lpObjects[ m_dNumObject -1 ];

	return S_OK;
}

HRESULT KComponentObjectTable::Get1NewObject(LPOBJECT *ppObject, DWORD Type)
{
	LPOBJECT * NewPointer = NULL;
	if (FAILED(Get1NewObjectPointer(&NewPointer)))
		return E_FAIL;
	switch(Type)
	{
	case OBJECT_DEFAULT:
		{
			(*NewPointer) = new KComponentObject;
			(*NewPointer)->Initialize(this); 
			break;
		}
	case OBJECTTYPE_GIRLNO1:
		{
			(*NewPointer) = new KRepGirlNo1;
			(*NewPointer)->Initialize(this); 
			break;
		}
	case OBJECTTYPE_NPC:
		{
			(*NewPointer) = new KRepNPC;
			(*NewPointer)->Initialize(this); 
			break;
		}
	default:
		{
			return E_FAIL;
			break;
		}
	}
	(*NewPointer)->m_dwType = Type;
	(*NewPointer)->ID = m_dNumObject;
	(*ppObject) = (*NewPointer);
	return S_OK;
}

HRESULT KComponentObjectTable::GetObject(LPOBJECT *ppObject, DWORD ID)
{
	if ((ID==0)||(ID>m_dNumObject)) return E_FAIL;
	(*ppObject) = m_lpObjects[ID-1]; 
	return S_OK;
}

HRESULT KComponentObjectTable::CleanUp()
{
	for(DWORD i=1;i<=m_dNumObject;i++)
	{
		LPOBJECT pObject = NULL;
		if (SUCCEEDED(GetObject(&pObject,i)))
		{
			pObject->CleanUp();
		}
		SAFE_DELETE(pObject);
	}
	SAFE_DELETE_ARRAY(m_lpObjects);
	m_dNumObject = 0;
	m_dMaxNumObject = 0;
	return S_OK;
}

HRESULT KComponentObjectTable::SetMeshTable(KModelTable* pTable)
{
	m_lpMeshTable = pTable;
	return S_OK;
}

HRESULT KComponentObjectTable::CopyObject(LPOBJECT *ppOutObject,LPOBJECT pInObject)
{
	LPOBJECT pNewObject = NULL;
	if (FAILED(Get1NewObject(&pNewObject,0)))
	{
		return E_FAIL;
	}
	for(DWORD i=0;i<pInObject->m_vecMeshKeeper.size();i++)
	{
		pNewObject->AddMesh(pInObject->m_vecMeshKeeper[i].m_dMeshID);
		pNewObject->m_vecMeshKeeper[i].m_Matrix = pInObject->m_vecMeshKeeper[i].m_Matrix;
	}
	pNewObject->m_Position = pInObject->m_Position + D3DXVECTOR3(0,0,200);
	pNewObject->m_Rotation = pInObject->m_Rotation;
	pNewObject->m_Scaling = pInObject->m_Scaling;
	(*ppOutObject) = pNewObject;
	return S_OK;
}

HRESULT KComponentObjectTable::LoadbjectFromFile(LPOBJECT *ppOutObject,LPSTR pFileName)
{
	TCHAR Name[256];
	if(FAILED(g_cFileFinder.FindFile(Name,pFileName)))
	{
		(*ppOutObject) = NULL;
		MessageBox(NULL,pFileName,"Can not find file!",0);
		return E_FAIL;
	}

	FILE* pFile = fopen(Name,"rb");
	if(!pFile)
		return E_FAIL;

	KComponentObject::_ObjectFileHeaderBase Header;
	fread(&Header,sizeof(KComponentObject::_ObjectFileHeaderBase),1,pFile);


	if(FAILED(Get1NewObject(ppOutObject,Header.Type)))
	{
		fclose(pFile);
		return E_FAIL;
	}

	fclose(pFile);

	if(FAILED((*ppOutObject)->LoadFromFile(Name)))
	{
		return E_FAIL;
	}
	return S_OK;
}