// MeshTable.h: interface for the CMeshTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHTABLE_H__E23707A5_E869_454F_A939_1F9818A266AE__INCLUDED_)
#define AFX_MESHTABLE_H__E23707A5_E869_454F_A939_1F9818A266AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "windows.h"
#include "mesh.h"

class CMeshTable  
{
private:
	int FindBone(LPSTR pBoneName,LPSTR* ppBones,DWORD NumBone);
	int FindBone(LPSTR pBoneName, CMesh::LPBONEINFO ppBones, DWORD NumBone);
	DWORD  m_dMaxNumMesh;

protected:
	HRESULT Get1NewMeshPointer(LPMESH** pppMesh);
public:
	HRESULT UnitMesh(DWORD* pNewMeshID);
	HRESULT CleanUp();
	HRESULT GetMesh(LPMESH *ppMesh,DWORD ID);
	HRESULT Get1NewMesh(LPMESH* ppMesh);

	DWORD   m_dNumMesh;
	LPMESH* m_lpMeshes;

	CMeshTable();
	virtual ~CMeshTable();

};


#endif // !defined(AFX_MESHTABLE_H__E23707A5_E869_454F_A939_1F9818A266AE__INCLUDED_)
