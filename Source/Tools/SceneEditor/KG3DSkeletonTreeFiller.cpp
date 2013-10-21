#include "stdafx.h"
#include "IEKG3DModelSpeedTree.h"
#include "KG3DSkeletonTreeFiller.h"
#include "IEKG3DBip.h"
#include "IEKG3DMesh.h"
#include "IEKG3DModelST.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


KG3DSkeletonTreeFiller& g_GetSkeletonTreeFiller()
{
	static KG3DSkeletonTreeFiller g_SKTreeFiller;
	return g_SKTreeFiller;
}

KG3DSkeletonTreeFiller::KG3DSkeletonTreeFiller(void)
{
}

KG3DSkeletonTreeFiller::~KG3DSkeletonTreeFiller(void)
{
}

void KG3DSkeletonTreeFiller::FillSKTree(IEKG3DModel* pModel, CTreeCtrl& Tree)
{
    HRESULT hRetCode = E_FAIL;
    HRESULT hResult = E_FAIL;
	//IEKG3DMeshBase* pMeshBase = NULL;
    int nBones = 0;
    IEKG3DMesh* pMesh = NULL;
    IEKG3DBip *piBip = NULL;
    IEKG3DModelST *pModelST = NULL;
    DWORD dwType = 0;

	KG_PROCESS_ERROR(pModel);

    hRetCode = pModel->GetType(&dwType);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

	switch(dwType)
	{
	case MESHTYPE_DEFAULT:
		{
			hRetCode = pModel->GetIEKG3DMesh(&pMesh);
			KGLOG_COM_PROCESS_ERROR(hRetCode);

            hRetCode = pMesh->GetNumBones(&nBones);
            KGLOG_COM_PROCESS_ERROR(hRetCode);

			KG_PROCESS_ERROR(nBones != 0);

			FillSKTree(pMesh, Tree);
			break;
		}
	case MESHTYPE_MODELST:
		{
			pModel->GetIEKG3DModelST(&pModelST);
            KG_PROCESS_ERROR(pModelST);

			hRetCode = pModelST->GetIEBip(&piBip);
			KGLOG_COM_PROCESS_ERROR(hRetCode);
			FillSKTree(piBip, Tree);
			break;
		}
	case MESHTYPE_SPEEDTREE:
		{
			IEKG3DModelSpeedTree* pModelSpeedTree = NULL;
            hRetCode = pModel->GetIEKG3DModelSpeedTree(&pModelSpeedTree);
            KGLOG_COM_PROCESS_ERROR(hRetCode);
            IEKG3DModel *pModelTree = NULL;
            pModelSpeedTree->GetModelTree(&pModelTree);
			FillSKTree(pModelTree, Tree);
			break;
		}
	}
    hResult = S_OK;
Exit0:
	return;
}

void KG3DSkeletonTreeFiller::FillSKTree(IEKG3DMesh* pMesh, CTreeCtrl& Tree)
{
	Tree.DeleteAllItems();
	DWORD dwSize = 0;
    pMesh->GetBaseBoneIDVectorSize(&dwSize);
	for (DWORD i = 0; i < dwSize; i++)
	{
		int nBaseBone = 0;//pMesh->m_nBaseBoneID[i];
        LPCTSTR pszName = NULL;
        pMesh->GetBaseBoneIDVectorItem(i, &nBaseBone);
        pMesh->GetBoneInfoName(nBaseBone, &pszName);
		HTREEITEM hTreeItem = Tree.InsertItem(pszName);
		Tree.SetItemData(hTreeItem, nBaseBone);
		Helper_FillSKTree(pMesh, Tree, hTreeItem);
	}
}

void KG3DSkeletonTreeFiller::Helper_FillSKTree(IEKG3DMesh* pMesh, CTreeCtrl& Tree, HTREEITEM& hItem)
{
	int nBoneIndex = static_cast<int>(Tree.GetItemData(hItem));
    DWORD dwSize = 0;
    pMesh->GetBoneInfoChildVectorSize(nBoneIndex, &dwSize);
	for (DWORD i = 0; i < dwSize; i++)
	{
		int nChildIndex = 0;//pMesh->m_pBoneInfo[nBoneIndex].ChildIDVector[i];
		LPCTSTR pszName = NULL;
        pMesh->GetBoneInfoChildVectorItem(nBoneIndex, i, &nChildIndex);
        pMesh->GetBoneInfoName(nChildIndex, &pszName);
        HTREEITEM hChildItem = Tree.InsertItem(pszName, hItem);
		Tree.SetItemData(hChildItem, nChildIndex);
		Helper_FillSKTree(pMesh, Tree, hChildItem);
	}
}


void KG3DSkeletonTreeFiller::Helper_FillSKTree(IEKG3DBip* pBip, CTreeCtrl& Tree, HTREEITEM& hItem)
{
	HRESULT hRetCode = E_FAIL;
    int nBoneIndex = static_cast<int>(Tree.GetItemData(hItem));
    BoneInfoData* pSkeleton = NULL;

    ASSERT(pBip);

    hRetCode = pBip->GetBoneInfoDataPoint(&pSkeleton);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

	for (DWORD i = 0; i < pSkeleton[nBoneIndex].dwNumChildBones; i++)
	{
		DWORD dwChildIndex = pSkeleton[nBoneIndex].ChildIndices[i];
		HTREEITEM hChildItem = Tree.InsertItem(pSkeleton[dwChildIndex].strBoneName, hItem);
		Tree.SetItemData(hChildItem, dwChildIndex);
		Helper_FillSKTree(pBip, Tree, hChildItem);
    }
Exit0:
    return;
}

void KG3DSkeletonTreeFiller::FillSKTree(IEKG3DBip* pBip, CTreeCtrl& Tree)
{
    HRESULT hRetCode = E_FAIL;
    DWORD dwCount = 0;
    int nBaseBone = 0;
    KG_PROCESS_ERROR(pBip);

    Tree.DeleteAllItems();


    BoneInfoData* pSkeleton = NULL;
    hRetCode = pBip->GetBoneInfoDataPoint(&pSkeleton);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = pBip->GetBaseBoneCount(&dwCount);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

	for (DWORD i = 0; i < dwCount; i++)
	{
		hRetCode = pBip->GetBaseBoneByPosition(i, &nBaseBone);
		KGLOG_COM_PROCESS_ERROR(hRetCode);

        HTREEITEM hTreeItem = Tree.InsertItem(pSkeleton[nBaseBone].strBoneName);
		Tree.SetItemData(hTreeItem, nBaseBone);
		Helper_FillSKTree(pBip, Tree, hTreeItem);
	}
Exit0:
	return;
}
