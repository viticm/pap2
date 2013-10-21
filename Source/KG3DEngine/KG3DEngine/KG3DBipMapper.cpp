#include "StdAfx.h"
#include "KG3DBipMapper.h"
#include "KG3DBip.h"

KG3DBipMapper::KG3DBipMapper(void)
{
	m_lpBipSrc = NULL;
	m_lpBipDest = NULL;
}

KG3DBipMapper::~KG3DBipMapper(void)
{
	;
}

HRESULT KG3DBipMapper::Init(KG3DBip* pSrc,KG3DBip* pDest)
{
	m_lpBipSrc = pSrc;
	m_lpBipDest = pDest;

	return S_OK;
}

HRESULT KG3DBipMapper::GenerateMapping()
{
	HRESULT hr = E_FAIL;

	vector<int>vecNotFound;

	KGLOG_PROCESS_ERROR(m_lpBipSrc);
	KGLOG_PROCESS_ERROR(m_lpBipDest);

	int nDestBone = m_lpBipDest->GetNumBones();
	BoneInfoData* pDestBones = NULL;
	int nSrcBone = m_lpBipDest->GetNumBones();
	BoneInfoData* pSrcBones = NULL;

	m_lpBipDest->GetBoneInfoDataPoint(&pDestBones);
	m_lpBipDest->GetBoneInfoDataPoint(&pSrcBones);


	for (int i=0;i<nDestBone;i++)
	{
		BoneInfoData* pDestBone = &pDestBones[i];
		BOOL bFind = FALSE;

		for (int j=0;j<nSrcBone;j++)
		{
			BoneInfoData* pSrcBone = &pSrcBone[j];
			if(_strcmpi(pDestBone->strBoneName,pSrcBone->strBoneName)==0)
			{
				bFind = TRUE;
				DirectMaping mapping;
				mapping.dwDestBoneIndex = i;
				mapping.dwSrcBoneIndex  = j;
				m_vecDirectMaping.push_back(mapping);
				break;
			}

			if(!bFind)
			{
				vecNotFound.push_back(i);
			}
		}
	}

	hr = S_OK;
Exit0:
	return hr;
}
