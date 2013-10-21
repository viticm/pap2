// XMeshTable.h: interface for the KModelTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMESHTABLE_H__08F5C0C5_152E_4968_B669_A7D41968B596__INCLUDED_)
#define AFX_XMESHTABLE_H__08F5C0C5_152E_4968_B669_A7D41968B596__INCLUDED_

#include "TextureTable.h"	// Added by ClassView
#include "XMesh.h"




#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class KModelTable  
{
public:
	KModelTable();
	virtual ~KModelTable();

public:
	DWORD m_dNumXMesh;

private:
	DWORD m_dMaxNumXMesh;
	LPMODEL* m_lpXMeshes;
public:
	HRESULT LoadFromFile(DWORD *pMeshID,LPSTR pFileName);
	virtual HRESULT  CleanUp();
	PVOID    m_lpParentTable;
	void Initialize(PVOID pObjectTable);
	void SetTextureTable(KTextureTable* pTextureTable);
	KTextureTable* m_lpTextureTable;
	HRESULT GetXMesh(LPMODEL* ppXMesh,DWORD ID);
	HRESULT Get1NewXMesh(LPMODEL* ppXMesh);
	HRESULT Get1NewXMesh(LPMODEL *ppXMesh,DWORD Type);
	HRESULT Get1NewXMeshPointer(LPMODEL** pppXMesh);

	HRESULT InvalidateDeviceObjects(void);
	HRESULT RestoreDeviceObjects(void);

	HRESULT GetCopy(DWORD* pNewMeshID,DWORD SrcMeshID);
};

#endif // !defined(AFX_XMESHTABLE_H__08F5C0C5_152E_4968_B669_A7D41968B596__INCLUDED_)
