#pragma once

#include "IEEditor.h"
interface IEKG3DBip;
interface IEKG3DMesh;
interface IEKG3DModel;

class KG3DSkeletonTreeFiller
{
public:
	KG3DSkeletonTreeFiller(void);
	~KG3DSkeletonTreeFiller(void);
	
	void FillSKTree(IEKG3DModel* pModel, CTreeCtrl& Tree);
private:
	void FillSKTree(IEKG3DMesh* pMesh, CTreeCtrl& Tree);
	void FillSKTree(IEKG3DBip* pBip, CTreeCtrl& Tree);
	void Helper_FillSKTree(IEKG3DMesh* pMesh, CTreeCtrl& Tree, HTREEITEM& hItem);
	void Helper_FillSKTree(IEKG3DBip* pBip, CTreeCtrl& Tree, HTREEITEM& hItem);
};

KG3DSkeletonTreeFiller& g_GetSkeletonTreeFiller();