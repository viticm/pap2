////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : Helper_Func.h
//  Version     : 1.0
//  Creator     : huangshan
//  Create Date : 2006-11-14 17:46:30
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_HELPER_FUNC_H_
#define _INCLUDE_HELPER_FUNC_H_

////////////////////////////////////////////////////////////////////////////////


interface IEKG3DMesh;

extern void Helper_FillBoneTree(LPVOID pMeshA, CTreeCtrl* pTree);
static void Helper_FillBoneTree_Sub(CTreeCtrl* pTree, HTREEITEM& hItem, IEKG3DMesh* pMesh, int nBoneID);


#endif //_INCLUDE_HELPER_FUNC_H_
