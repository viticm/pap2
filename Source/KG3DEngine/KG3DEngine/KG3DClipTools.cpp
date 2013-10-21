#include "StdAfx.h"
#include "KG3DClipTools.h"
#include "kg3dbip.h"
#include "KG3DMesh.h"
#include "KG3DClip.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL g_bClient;

KG3DClipTools& GetClipTool()
{
	static KG3DClipTools g_ClipTool;
	return g_ClipTool;
}

KG3DClipTools::KG3DClipTools(void)
{
}

KG3DClipTools::~KG3DClipTools(void)
{
}

//HRESULT KG3DClipTools::CheckClipByBipPreload(KG3DClip *pClip,
//											 const KG3DBip *pBip)
//{
//	HRESULT hResult = E_FAIL;
//	HRESULT hRetCode = E_FAIL;
//	unsigned int uType = 0;
//	unsigned uNumAnimatedBone = 0;
//
//	KG_PROCESS_ERROR(pClip);
//	KG_PROCESS_ERROR(pBip);
//	
//	uType = pClip->GetType();
//	hResult = pClip->GetNumAnimatedBone(&uNumAnimatedBone);
//	KGLOG_COM_PROCESS_ERROR(hResult);
//	
//	switch(uType)
//	{
//	case ANIMATION_BONE:
//	case ANIMATION_BONE_RELATIVE:
//	case ANIMATION_BONE_RTS:
//		{
//			KG_PROCESS_ERROR(pBip->m_nNumBones == static_cast<int>(uNumAnimatedBone));
//			break;
//		}
//	default:
//		break;
//	}
//	hRetCode = S_OK;
//Exit0:
//	if (FAILED(hRetCode) && pBip && pClip)
//	{
//		KGLogPrintf(KGLOG_ERR, 
//			"Mesh: %s Animation: %s not match.", 
//			pBip->m_scName.c_str(),
//			pClip->m_scName.c_str());
//	}
//	return hRetCode;
//}

HRESULT KG3DClipTools::CheckClipByBip(KG3DClip* pClip, KG3DBip* pBip)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    unsigned uType = 0;
    DWORD dwNumAnimatedBone = 0;
    int nAdjustFlag = false;

	KG_PROCESS_ERROR(pClip && pClip->IsLoad());
	KG_PROCESS_ERROR(pBip);

    uType = pClip->GetType();
    dwNumAnimatedBone = pClip->GetNumAnimatedBone();

	switch (uType)
	{
	case ANIMATION_BONE:
	case ANIMATION_BONE_RELATIVE:
	case ANIMATION_BONE_RTS:
		{
			KG_ASSERT_EXIT(pBip->m_nNumBones == static_cast<int>(dwNumAnimatedBone));
		    KG_PROCESS_SUCCESS(g_bClient);

			for (int i = 0; i < pBip->m_nNumBones; i++)
			{
				int nBoneIndex = pClip->FindBone(pBip->m_pSkeleton[i].strBoneName);
				KGLOG_PROCESS_ERROR(nBoneIndex != -1);
				//If bone in mesh and animation index do not match, fix it
				if (nBoneIndex != i)
                {
					pClip->SwapBoneAnimationData(i, nBoneIndex);

                    nAdjustFlag = true;
				}
			}
			break;
		}
	default:
		break;
	}
	
    if (nAdjustFlag)
    {
        hrRetCode = pClip->SaveToFile(pClip->m_scName.c_str(), 0);
        KG_COM_PROCESS_ERROR(hrRetCode);

        nAdjustFlag = false;

        MessageBox(NULL, "骨骼顺序已经自动校正存盘！", "提示", MB_OK);
        KGLogPrintf(KGLOG_INFO, "骨骼顺序校正 %s %s", pClip->m_scName.c_str(), pBip->GetBipFileName());
    }

Exit1:
	hrResult = S_OK;
Exit0:
	if(FAILED(hrResult))
	{
		KGLogPrintf(KGLOG_ERR, "bip clip not match, %s %s", pClip->m_scName.c_str(), pBip->GetBipFileName());
	}
	return hrResult;
}

//HRESULT KG3DClipTools::CheckClipByMeshPreload(KG3DClip* pClip, 
//							   KG3DMesh* pMesh)
//{
//	HRESULT hRetCode = E_FAIL;
//	DWORD dwNumAnimatedBone = 0;
//
//	KGLOG_PROCESS_ERROR(pClip);
//	KGLOG_PROCESS_ERROR(pMesh);
//
//	hRetCode = pClip->GetNumAnimatedBone((unsigned *)(&dwNumAnimatedBone));
//	KGLOG_COM_PROCESS_ERROR(hRetCode);
//
//	switch(pMesh->GetType())
//	{
//	case MESHTYPE_DEFAULT:
//		{
//			KGLOG_PROCESS_ERROR(dwNumAnimatedBone == pMesh->m_dwNumBone);
//
//			break;
//		}
//	default:
//		break;
//	}
//	hRetCode = S_OK;
//Exit0:
//	return hRetCode;
//}
//
HRESULT KG3DClipTools::CheckClipByMesh(KG3DClip* pClip, KG3DMesh* pMesh)
{
	HRESULT hr = E_FAIL;
    unsigned uType = 0;

	KG_PROCESS_ERROR(pClip);
	KG_PROCESS_ERROR(pMesh);
    
    uType = pClip->GetType();
	switch(uType)
	{
	case ANIMATION_VERTICES:
	case ANIMATION_VERTICES_RELATIVE:
		{
			hr = S_OK;
			break;
		}
	case ANIMATION_BONE:
	case ANIMATION_BONE_RELATIVE:
	case ANIMATION_BONE_RTS:
		{
			hr = Helper_CheckClipByMeshBone(pClip, pMesh);
			break;
		}
	}
Exit0:
	return hr;
}


HRESULT KG3DClipTools::Helper_CheckClipByMeshBone(KG3DClip* pClip, KG3DMesh* pMeshIn)
{
    HRESULT hrResult = E_FAIL;

	DWORD dwNumAnimatedBone = 0;
    
	KGLOG_PROCESS_ERROR(pClip);
	KGLOG_PROCESS_ERROR(pMeshIn);
	
    dwNumAnimatedBone = pClip->GetNumAnimatedBone();
    
    switch(pMeshIn->GetType())
	{
	case MESHTYPE_DEFAULT:
        {
			KGLOG_PROCESS_ERROR(dwNumAnimatedBone == pMeshIn->m_dwNumBone);
			
            for (int i = 0; i < static_cast<int>(pMeshIn->m_dwNumBone); i++)
			{
				int nBoneIndex = pClip->FindBone(pMeshIn->m_pBoneInfo[i].strBoneName);
				KGLOG_PROCESS_ERROR(nBoneIndex != -1);
				//If bone in mesh and animation index do not match, fix it
				if (nBoneIndex != i)
				    pClip->SwapBoneAnimationData(nBoneIndex, i);
			}
			break;
		}
	default:
		break;
	}

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (pClip && pMeshIn)
        {
            LPCSTR szClip = NULL;
            LPCSTR szMesh = NULL;

            pClip->GetName(&szClip);
            pMeshIn->GetName(&szMesh);

            KGLogPrintf(KGLOG_ERR, "KG3DClipTools::Helper_CheckClipByMeshBone(%s, %s)\n", szClip, szMesh);
        }
    }
	return hrResult;
}