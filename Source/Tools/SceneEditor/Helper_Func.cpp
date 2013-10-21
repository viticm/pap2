#include "stdafx.h"
#include "Helper_Func.h"
#include "IEKG3DMesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void Helper_FillBoneTree(LPVOID pMeshA, CTreeCtrl* pTree)
{
	IEKG3DMesh* pMesh = static_cast<IEKG3DMesh*>(pMeshA);
	DWORD i = 0;
	HTREEITEM hItem = NULL;
	
    int nNumBone = 0;
	int nBoneID = 0;
    DWORD dwBaseBoneIDVectorSize = 0;
    LPCTSTR pszName = NULL;
	assert(pMesh);
	assert(pTree);

    pMesh->GetNumBones(&nNumBone);
	assert(nNumBone);

	pTree->DeleteAllItems();
	
	pMesh->GetBaseBoneIDVectorSize(&dwBaseBoneIDVectorSize);
    for (i = 0; i < dwBaseBoneIDVectorSize; i++)
	{
        pMesh->GetBaseBoneIDVectorItem(i, &nBoneID);
        pMesh->GetBoneInfoName(nBoneID, &pszName);
		hItem = pTree->InsertItem(pszName);
		pTree->SetItemData(hItem, (DWORD_PTR)(nBoneID));
		Helper_FillBoneTree_Sub(pTree, hItem, pMesh, nBoneID);
	}
}

static void Helper_FillBoneTree_Sub(CTreeCtrl* pTree, HTREEITEM& hItem, IEKG3DMesh* pMesh, int nBoneID)
{
	DWORD i = 0;
	int nChildID = 0;
	HTREEITEM Item = 0;
	DWORD dwNumChildBone = 0;
	LPCTSTR pszName = 0;
    
    pMesh->GetBoneInfoChildVectorSize(nBoneID, &dwNumChildBone);

    for(i = 0; i < dwNumChildBone; i++)
	{
		pMesh->GetBoneInfoChildVectorItem(nBoneID, i, &nChildID);
        pMesh->GetBoneInfoName(nChildID, &pszName);
        
		Item = pTree->InsertItem(pszName, hItem);
		pTree->SetItemData(Item, nChildID);
		Helper_FillBoneTree_Sub(pTree, Item, pMesh, nChildID);
	}
}