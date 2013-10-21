#include "StdAfx.h"
#include "kg3dmodel.h"
#include "kg3dani.h"
#include "kg3danitable.h"
#include "kg3dgraphicstool.h"
#include "KG3DAnimationController.h"
#include "KG3DTexture.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void KG3DAni::RTSWeighted(RTS& Frame, float fWeight)
{
	Frame.Rotation.z *= fWeight;
	Frame.Scale *= fWeight;
	Frame.Translation *= fWeight;
}

void KG3DAni::InterpolateRTSKeyFrame(RTS& FrameResult, 
									  const RTS& FrameA, 
									  const RTS& FrameB, 
									  float fWeight)
{
	D3DXQUATERNION sRot, Rot;
	D3DXVECTOR3 Scale, Translation;
	D3DXQuaternionSlerp(&FrameResult.SRotation, &FrameA.SRotation, &FrameB.SRotation, fWeight);
	D3DXQuaternionSlerp(&FrameResult.Rotation, &FrameA.Rotation, &FrameB.Rotation, fWeight);
	D3DXVec3Lerp(&FrameResult.Scale, &FrameA.Scale, &FrameB.Scale, fWeight);
	D3DXVec3Lerp(&FrameResult.Translation, &FrameA.Translation, &FrameB.Translation, fWeight);
	FrameResult.Sign = FrameA.Sign;
}

void KG3DAni::ComposeRTS(RTS &rtsResult,
						  RTS *pRTS,
						  DWORD *pFlags,
						  DWORD dwNumRTS)
{
	for (DWORD i = 0; i < dwNumRTS; i++)
	{
		if (pFlags[i] & Bone_Scale_Mask)
		{
			rtsResult.Scale = pRTS[i].Scale;
			rtsResult.Sign = pRTS[i].Sign;
			rtsResult.SRotation = pRTS[i].SRotation;
		}
		else if (pFlags[i] & Bone_Rotation_Mask)
		{
			rtsResult.Rotation = pRTS[i].Rotation;
		}
		else if (pFlags[i] & Bone_Translation_Mask)
		{
			rtsResult.Translation = pRTS[i].Translation;
		}
		else
		{
			ASSERT(0);
		}
	}
}

void KG3DAni::ComposeRTS(RTS &rtsResult,
						  RTS **ppRTS,
						  DWORD *pFlags,
						  DWORD dwNumRTS)
{
	for (DWORD i = 0; i < dwNumRTS; i++)
	{
		DWORD dwCurFlag = pFlags[i] & Bone_Scale_Mask;

		switch (dwCurFlag)
		{
		case Bone_Scale:
			{
				rtsResult.Scale = ppRTS[i]->Scale;
				rtsResult.Sign = ppRTS[i]->Sign;
				rtsResult.SRotation = ppRTS[i]->SRotation;
			}
			break;
		case Bone_Scale_Reverse:
		case Bone_Scale_None:
			break;

		}

		dwCurFlag = pFlags[i] & Bone_Rotation_Mask;

		switch (dwCurFlag)
		{
		case Bone_Rotation:
			{
				rtsResult.Rotation = ppRTS[i]->Rotation;
			}
			break;
		case Bone_Rotation_Reverse:
		case Bone_Rotation_None:
			break;
		}

		dwCurFlag = pFlags[i] & Bone_Translation_Mask;
		switch (dwCurFlag)
		{
		case Bone_Translation:
			{
				rtsResult.Translation = ppRTS[i]->Translation;
			}
			break;
		case Bone_Translation_None:
		case Bone_Translation_Reverse:
			//            int nDebug = 0;
			break;
		}
	}
}


void KG3DAni::RTS2Matrix(D3DXMATRIX& matResult,
						  const RTS& RtsSource,
						  DWORD dwFlag)
{
	static D3DXMATRIX matIdentity;
	static BOOL bInited = FALSE;
	if (!bInited)
	{
		D3DXMatrixIdentity(&matIdentity);
		bInited = TRUE;
	}

	D3DXMATRIX matSRot, matSRotInv, matRot, matScale, matTrans, matScaleSign;

	//Scale
	DWORD dwScaleFlag = dwFlag & Bone_Scale_Mask;
	switch (dwScaleFlag)
	{
	case Bone_Scale_None:
		{
			matSRot = matIdentity;
			matSRotInv = matIdentity;
			matScale = matIdentity;
			matScaleSign = matIdentity;
		}
		break;
	case Bone_Scale:
		{
			D3DXMatrixScaling(&matScale, 
				RtsSource.Scale.x, 
				RtsSource.Scale.y, 
				RtsSource.Scale.z);

			D3DXMatrixScaling(&matScaleSign,
				RtsSource.Sign,
				RtsSource.Sign,
				RtsSource.Sign);

			D3DXMatrixRotationQuaternion(&matSRot, &RtsSource.SRotation);
			D3DXMatrixTranspose(&matSRot, &matSRot);

			D3DXMatrixInverse(&matSRotInv, 
				NULL, 
				&matSRot);
		}
		break;
	case Bone_Scale_Reverse:
		{
			//Not Implemented
		}
		break;
	}

	//Rotation
	DWORD dwRotationFlag = dwFlag & Bone_Rotation_Mask;
	switch (dwRotationFlag)
	{
	case Bone_Rotation:
		{
			D3DXMatrixRotationQuaternion(&matRot, 
				&RtsSource.Rotation);

			D3DXMatrixTranspose(&matRot, &matRot);
		}
		break;
	case Bone_Rotation_None:
		{
			matRot = matIdentity;
		}
		break;
	case Bone_Rotation_Reverse:
		{
			//Not Implemented
		}
		break;
	}

	//Translation
	DWORD dwTranslationFlag = dwFlag & Bone_Translation_Mask;
	switch (dwTranslationFlag)
	{
	case Bone_Translation:
		{
			D3DXMatrixTranslation(&matTrans, 
				RtsSource.Translation.x, 
				RtsSource.Translation.y, 
				RtsSource.Translation.z);
		}
		break;
	case Bone_Translation_None:
		{
			matTrans = matIdentity;
		}
		break;
	case Bone_Translation_Reverse:
		{
			//Not implemented
		}
		break;
	}
	//Calculate final result
	matResult = matSRotInv * matScale * matSRot * matRot * matScaleSign * matTrans;
}

void KG3DAni::RTS2Matrix(D3DXMATRIX& matResult,
						  const RTS& RtsSource)
{
	D3DXMATRIX matSRot, matSRotInv, matRot, matScale, matTrans, matScaleSign;
	D3DXMatrixRotationQuaternion(&matSRot, &RtsSource.SRotation);
	D3DXMatrixTranspose(&matSRot, &matSRot);

	D3DXMatrixInverse(&matSRotInv, 
		NULL, 
		&matSRot);

	D3DXMatrixRotationQuaternion(&matRot, 
		&RtsSource.Rotation);

	D3DXMatrixTranspose(&matRot, &matRot);

	D3DXMatrixScaling(&matScale, 
		RtsSource.Scale.x, 
		RtsSource.Scale.y, 
		RtsSource.Scale.z);

	D3DXMatrixScaling(&matScaleSign,
		RtsSource.Sign,
		RtsSource.Sign,
		RtsSource.Sign);

	D3DXMatrixTranslation(&matTrans, 
		RtsSource.Translation.x, 
		RtsSource.Translation.y, 
		RtsSource.Translation.z);

	matResult = matSRotInv * matScale * matSRot * matRot * matScaleSign * matTrans;
}
KG3DAni::KG3DAni(void)
{
	m_dwType = 0; 
	m_dwID   = 0;
	m_nReference = 0;

	m_dwNumFrames = 0;
	m_fFrameLength = 0.0f;

	m_dwNumAnimatedVertice = 0;
	m_pAnimatedVerticesIndex = NULL;
	m_pAnimatedVerticesPosition = NULL;
	m_pAnimationUV = NULL;
	m_dwNumAnimatedBone = 0;
	m_ppAnimatedBoneName = NULL;

	m_pAnimatedBoneMatrix = NULL;
	m_pAnimatedBoneRTS = NULL;

	m_strClipName = "";
	m_scName = "";
}

KG3DAni::~KG3DAni(void)
{
	m_dwType = 0;
	m_dwID   = 0;
	m_nReference = 0;
	UnInit();
}

HRESULT KG3DAni::GetFrameLength(FLOAT *fRetLength)
{
	HRESULT hResult = E_FAIL;

	KGLOG_PROCESS_ERROR(fRetLength);

	*fRetLength = m_fFrameLength;

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DAni::SetFrameLength(FLOAT fFrameLength)
{
	HRESULT hResult = E_FAIL;

	KGLOG_PROCESS_ERROR(fFrameLength > 0.0f); 

	m_fFrameLength = fFrameLength;

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DAni::GetNumFrames(DWORD *pdwRetNum)                        
{
	HRESULT hResult = E_FAIL;

	KGLOG_PROCESS_ERROR(pdwRetNum);

	*pdwRetNum = m_dwNumFrames;

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DAni::GetName(LPCSTR *ppszName)
{
	HRESULT hResult = E_FAIL;

	KGLOG_PROCESS_ERROR(ppszName);

	*ppszName = m_strClipName.c_str();

	hResult = S_OK;
Exit0:
	return hResult;
}
//////////////////////////////////////////////////////////////////////////
HRESULT KG3DAni::GetNumAnimatedBone(unsigned *puRetNum)
{
	HRESULT hResult = E_FAIL;

	KGLOG_PROCESS_ERROR(puRetNum);

	*puRetNum = m_dwNumAnimatedBone;

	hResult = S_OK;
Exit0:
	return hResult;
}

//////////////////////////////////////////////////////////////////////////
HRESULT KG3DAni::GetRTSPoint(RTS ** ppRTS)
{
	HRESULT hResult = E_FAIL;

	KGLOG_PROCESS_ERROR(ppRTS);

	*ppRTS = m_pAnimatedBoneRTS;

	hResult = S_OK;
Exit0:
	return hResult;
}
////////////////////////////////////////////////////////////////////////////
HRESULT KG3DAni::GetAnimatedBoneName(LPTSTR **pppAnimatedBoneName)
{
	HRESULT hResult = E_FAIL;

	KGLOG_PROCESS_ERROR(pppAnimatedBoneName);

	*pppAnimatedBoneName = m_ppAnimatedBoneName;

	hResult = S_OK;
Exit0:
	return hResult;
}
//////////////////////////////////////////////////////////////////////////
DWORD KG3DAni::GetID()
{
	return m_dwID;
}

HRESULT KG3DAni::Init()
{
	m_eDynamicLoadState = STATE_NONE;
	return S_OK;
}

HRESULT KG3DAni::UnInit()
{
	m_dwNumFrames = 0;
	m_fFrameLength = 0.0f;

	m_dwNumAnimatedVertice = 0;
	SAFE_DELETE_ARRAY(m_pAnimatedVerticesIndex);
	SAFE_DELETE_ARRAY(m_pAnimatedVerticesPosition);
	SAFE_DELETE_ARRAY(m_pAnimationUV);


	if (m_ppAnimatedBoneName)
	{
		for(DWORD i = 0; i < m_dwNumAnimatedBone; i++)
		{
			SAFE_DELETE_ARRAY(m_ppAnimatedBoneName[i]);
		}
	}

	SAFE_DELETE_ARRAY(m_ppAnimatedBoneName);
	SAFE_DELETE_ARRAY(m_pAnimatedBoneMatrix);
	SAFE_DELETE_ARRAY(m_pAnimatedBoneRTS);

	//m_strClipName = "";
	m_dwNumAnimatedBone = 0;
	m_eDynamicLoadState = STATE_NONE;
	return S_OK;
}

HRESULT KG3DAni::LoadFromFileMultiThread()
{
	HRESULT hRetCode = E_FAIL;
	unsigned long uSizeRead = 0;
	DWORD dwMask = 0;
	DWORD dwBlockLength = 0;
	DWORD dwNumAnimation = 0;
	TCHAR strClipDesc[30];
	DWORD dwUVAniSize = 0;
	IFile* pFile = NULL;

	m_eDynamicLoadState = STATE_FAILED;

	pFile = g_OpenFile(m_scName.c_str());
	KG_PROCESS_ERROR(pFile);

	uSizeRead = pFile->Read(&dwMask, sizeof(DWORD));
	KG_PROCESS_ERROR(uSizeRead == sizeof(DWORD));
	KGLOG_PROCESS_ERROR(dwMask == m_dwMask);

	uSizeRead = pFile->Read(&dwBlockLength, sizeof(DWORD));
	KG_PROCESS_ERROR(uSizeRead == sizeof(DWORD));

	//Warning!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	uSizeRead = pFile->Read(&dwNumAnimation, sizeof(DWORD));
	KG_PROCESS_ERROR(uSizeRead == sizeof(DWORD));
	//assert(dwNumAnimation == 1);

	uSizeRead = pFile->Read(&m_dwType, sizeof(DWORD));
	KG_PROCESS_ERROR(uSizeRead == sizeof(DWORD));

	uSizeRead = pFile->Read(strClipDesc, sizeof(TCHAR) * MAX_CLIPDESC_LEN);
	KG_PROCESS_ERROR(uSizeRead == sizeof(TCHAR) * MAX_CLIPDESC_LEN);

	switch (m_dwType)
	{
	case ANIMATION_BONE:
	case ANIMATION_BONE_RELATIVE:
		{
			uSizeRead = pFile->Read(&m_dwNumAnimatedBone, sizeof(DWORD));
			KG_PROCESS_ERROR(uSizeRead == sizeof(DWORD));

			uSizeRead = pFile->Read(&m_dwNumFrames, sizeof(DWORD));
			KG_PROCESS_ERROR(uSizeRead == sizeof(DWORD));

			uSizeRead = pFile->Read(&m_fFrameLength, sizeof(float));
			KG_PROCESS_ERROR(uSizeRead == sizeof(float));

			m_ppAnimatedBoneName = new LPSTR[m_dwNumAnimatedBone];
			KGLOG_PROCESS_ERROR(m_ppAnimatedBoneName);
			ZeroMemory(m_ppAnimatedBoneName, sizeof(LPSTR) * m_dwNumAnimatedBone);

			m_pAnimatedBoneMatrix = new D3DXMATRIX[m_dwNumFrames * m_dwNumAnimatedBone];
			KGLOG_PROCESS_ERROR(m_pAnimatedBoneMatrix);
			ZeroMemory(m_pAnimatedBoneMatrix, sizeof(D3DXMATRIX) * m_dwNumFrames * m_dwNumAnimatedBone);

			for(DWORD i = 0; i < m_dwNumAnimatedBone; i++)
			{
				m_ppAnimatedBoneName[i] = new TCHAR[MAX_BONENAME_LEN];
				KGLOG_PROCESS_ERROR(m_ppAnimatedBoneName[i]);
				uSizeRead = pFile->Read(m_ppAnimatedBoneName[i], sizeof(TCHAR) * MAX_BONENAME_LEN); 
				KG_PROCESS_ERROR(uSizeRead == sizeof(TCHAR) * MAX_BONENAME_LEN);
			}		

			uSizeRead = pFile->Read(m_pAnimatedBoneMatrix, sizeof(D3DXMATRIX) * m_dwNumFrames * m_dwNumAnimatedBone); 
			KG_PROCESS_ERROR(uSizeRead == sizeof(D3DXMATRIX) * m_dwNumFrames * m_dwNumAnimatedBone);				
			break;
		}
	case ANIMATION_BONE_RTS:
		{
			uSizeRead = pFile->Read(&m_dwNumAnimatedBone, sizeof(DWORD));
			KG_PROCESS_ERROR(uSizeRead == sizeof(DWORD));

			uSizeRead = pFile->Read(&m_dwNumFrames, sizeof(DWORD));
			KG_PROCESS_ERROR(uSizeRead == sizeof(DWORD));

			uSizeRead = pFile->Read(&m_fFrameLength, sizeof(float));
			KG_PROCESS_ERROR(uSizeRead == sizeof(float));

			m_ppAnimatedBoneName = new LPSTR[m_dwNumAnimatedBone];
			KGLOG_PROCESS_ERROR(m_ppAnimatedBoneName);
			ZeroMemory(m_ppAnimatedBoneName, sizeof(LPSTR) * m_dwNumAnimatedBone);

			DWORD dwRTS = m_dwNumAnimatedBone * m_dwNumFrames;
			m_pAnimatedBoneRTS = new RTS[dwRTS];
			KGLOG_PROCESS_ERROR(m_pAnimatedBoneRTS);
			ZeroMemory(m_pAnimatedBoneRTS, sizeof(RTS) * dwRTS);

			for(DWORD i = 0; i < m_dwNumAnimatedBone; i++)
			{
				m_ppAnimatedBoneName[i] = new TCHAR[MAX_BONENAME_LEN];
				KGLOG_PROCESS_ERROR(m_ppAnimatedBoneName[i]);
				uSizeRead = pFile->Read(m_ppAnimatedBoneName[i], sizeof(TCHAR) * MAX_BONENAME_LEN); 
				KG_PROCESS_ERROR(uSizeRead == sizeof(TCHAR) * MAX_BONENAME_LEN);
				_strlwr_s(m_ppAnimatedBoneName[i], MAX_BONENAME_LEN);
			}		

			uSizeRead = pFile->Read(m_pAnimatedBoneRTS, sizeof(RTS) * dwRTS);
			KG_PROCESS_ERROR(uSizeRead == sizeof(RTS) * dwRTS);
			break;
		}
	case ANIMATION_VERTICES_RELATIVE:
	case ANIMATION_VERTICES:
		{
			uSizeRead = pFile->Read(&m_dwNumVertices, sizeof(DWORD));
			KG_PROCESS_ERROR(uSizeRead == sizeof(DWORD));

			uSizeRead = pFile->Read(&m_dwNumAnimatedVertice, sizeof(DWORD));
			KG_PROCESS_ERROR(uSizeRead == sizeof(DWORD));

			uSizeRead = pFile->Read(&m_dwNumFrames, sizeof(DWORD));
			KG_PROCESS_ERROR(uSizeRead == sizeof(DWORD));

			uSizeRead = pFile->Read(&m_fFrameLength, sizeof(float));
			KG_PROCESS_ERROR(uSizeRead == sizeof(float));

			m_pAnimatedVerticesIndex = new DWORD[m_dwNumAnimatedVertice];
			KGLOG_PROCESS_ERROR(m_pAnimatedVerticesIndex);
			ZeroMemory(m_pAnimatedVerticesIndex, sizeof(DWORD) * m_dwNumAnimatedVertice);

			m_pAnimatedVerticesPosition = new D3DXVECTOR3[m_dwNumAnimatedVertice * m_dwNumFrames];
			KGLOG_PROCESS_ERROR(m_pAnimatedVerticesPosition);
			ZeroMemory(m_pAnimatedVerticesPosition, sizeof(D3DXVECTOR3) * m_dwNumAnimatedVertice * m_dwNumFrames);

			m_pAnimationUV = new D3DXVECTOR3[m_dwNumAnimatedVertice * m_dwNumFrames];
			KGLOG_PROCESS_ERROR(m_pAnimationUV);
			ZeroMemory(m_pAnimationUV, sizeof(D3DXVECTOR3) * m_dwNumAnimatedVertice * m_dwNumFrames);

			uSizeRead = pFile->Read(m_pAnimatedVerticesIndex, sizeof(DWORD) * m_dwNumAnimatedVertice);
			KG_PROCESS_ERROR(uSizeRead == sizeof(DWORD) * m_dwNumAnimatedVertice);

			uSizeRead = pFile->Read(m_pAnimatedVerticesPosition, sizeof(D3DXVECTOR3) * m_dwNumAnimatedVertice * m_dwNumFrames);
			KG_PROCESS_ERROR(uSizeRead == sizeof(D3DXVECTOR3) * m_dwNumAnimatedVertice * m_dwNumFrames);

			dwUVAniSize = sizeof(D3DXVECTOR3) * m_dwNumAnimatedVertice * m_dwNumFrames;
			uSizeRead = pFile->Read(m_pAnimationUV, dwUVAniSize);

			if(uSizeRead == dwUVAniSize)
			{
				//something is read, UV animation exist
				m_dwOptions |= ENUM_CLIPOP_UVANI;
			}
			else
			{
				SAFE_DELETE_ARRAY(m_pAnimationUV);
			}
			break;
		}
	}

	m_strClipName = strClipDesc;
	m_eDynamicLoadState = STATE_READY;
	hRetCode = S_OK;
Exit0:
	SAFE_RELEASE(pFile);
	return hRetCode;
}

HRESULT KG3DAni::LoadFromFile(const char cszFileName[], 
							  unsigned uFileNameHash, 
							  unsigned uOption)
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult = E_FAIL;
	
	m_dwOptions = 0;

	HeaderInfo Header;
	hResult = LoadHeaderInfo(cszFileName, &Header);
	KG_COM_PROCESS_ERROR(hResult);

	m_dwType = Header.dwType;
	m_strClipName = Header.strDesc;
	switch (m_dwType)
	{
	case ANIMATION_VERTICES:
	case ANIMATION_VERTICES_RELATIVE:
		{
			m_dwNumVertices = Header.FrameInfo.Vertex.dwNumVertices;
			m_dwNumAnimatedVertice = Header.FrameInfo.Vertex.dwNumAnimatedVertices;
			m_dwNumFrames = Header.FrameInfo.Vertex.dwNumFrames;
			m_fFrameLength = Header.FrameInfo.Vertex.fFrameLength;
		}
		break;
	case ANIMATION_BONE:
	case ANIMATION_BONE_RELATIVE:
	case ANIMATION_BONE_RTS:
		{
			m_dwNumFrames = Header.FrameInfo.Bone.dwNumFrames;
			m_dwNumAnimatedBone = Header.FrameInfo.Bone.dwNumBones;
			m_fFrameLength = Header.FrameInfo.Bone.fFrameLength;
		}
		break;
	default:
		assert(0);
	}
	
	uOption = LOAD_MULTITHREAD;
	if (uOption & LOAD_NORMAL)
	{
		hResult = LoadFromFileMultiThread();
		KG_COM_PROCESS_ERROR(hResult);
		m_scName = cszFileName;
		m_eDynamicLoadState = STATE_READY;
	}
	else if (uOption & LOAD_MULTITHREAD)
	{
		
		m_scName = cszFileName;
		m_eDynamicLoadState = STATE_NONE;
		g_setMultiThreadReadAni.insert(this);
	}

	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT KG3DAni::SaveToFile(const char cszFileName[], unsigned uOption)
{
	HRESULT hr = E_FAIL;
	DWORD dwBlockEnd  = 0xFFFFFFFF;  
	DWORD dwBlockLength = 0;
	FILE* fpFileSave = NULL;
	DWORD dwNumAnimations = 1;
	LPSTR strBoneName = NULL;
	size_t uSize = 0;

	fpFileSave = fopen(cszFileName,"wb");
	KGLOG_PROCESS_ERROR(fpFileSave);

	//File type mask
	uSize = fwrite(&m_dwMask, sizeof(DWORD), 1, fpFileSave);

	//Data length, not used?
	uSize = fwrite(&dwBlockLength, sizeof(DWORD), 1, fpFileSave);

	//assert(m_dwNumAnimations == 1);
	//Num animation must be 1
	uSize = fwrite(&dwNumAnimations, sizeof(DWORD), 1, fpFileSave);

	//Animation type
	uSize = fwrite(&m_dwType, sizeof(DWORD), 1, fpFileSave);

	//Animation description
	uSize = fwrite(m_strClipName.c_str(), sizeof(TCHAR), MAX_CLIPDESC_LEN, fpFileSave);

	switch(m_dwType)
	{
	case ANIMATION_BONE:
	case ANIMATION_BONE_RELATIVE:
		{
			uSize = fwrite(&m_dwNumAnimatedBone, sizeof(DWORD), 1, fpFileSave);
			uSize = fwrite(&m_dwNumFrames, sizeof(DWORD), 1, fpFileSave);
			uSize = fwrite(&m_fFrameLength, sizeof(float), 1, fpFileSave);

			for (DWORD i = 0; i < m_dwNumAnimatedBone; i++)
			{
				strBoneName = m_ppAnimatedBoneName[i];
				uSize = fwrite(strBoneName, sizeof(TCHAR), MAX_BONENAME_LEN, fpFileSave); 
			}
			uSize = fwrite(m_pAnimatedBoneMatrix, 
				sizeof(D3DXMATRIX) * m_dwNumAnimatedBone * m_dwNumFrames, 
				1,
				fpFileSave);
			break;
		}
	case ANIMATION_BONE_RTS:
		{
			uSize = fwrite(&m_dwNumAnimatedBone, sizeof(DWORD), 1, fpFileSave);
			uSize = fwrite(&m_dwNumFrames, sizeof(DWORD), 1, fpFileSave);
			uSize = fwrite(&m_fFrameLength, sizeof(float), 1, fpFileSave);

			for(DWORD i = 0; i < m_dwNumAnimatedBone; i++)
			{
				strBoneName = m_ppAnimatedBoneName[i];
				uSize = fwrite(strBoneName, sizeof(TCHAR), MAX_BONENAME_LEN, fpFileSave); 
			}

			uSize = fwrite(m_pAnimatedBoneRTS, 
				sizeof(RTS) * m_dwNumAnimatedBone * m_dwNumFrames, 
				1, 
				fpFileSave);
			break;
		}
	case ANIMATION_VERTICES_RELATIVE:
		{
			uSize = fwrite(&m_dwNumVertices, sizeof(DWORD), 1, fpFileSave);
			uSize = fwrite(&m_dwNumAnimatedVertice, sizeof(DWORD),1, fpFileSave);
			uSize = fwrite(&m_dwNumFrames, sizeof(DWORD), 1, fpFileSave);
			uSize = fwrite(&m_fFrameLength, sizeof(float), 1, fpFileSave);
			uSize = fwrite(m_pAnimatedVerticesIndex, sizeof(DWORD), m_dwNumAnimatedVertice, fpFileSave); 
			uSize = fwrite(m_pAnimatedVerticesPosition, 
				sizeof(D3DXVECTOR3) * m_dwNumAnimatedVertice * m_dwNumFrames,
				1,
				fpFileSave);
			break;
		}
	case ANIMATION_VERTICES:
		{
			uSize = fwrite(&m_dwNumVertices, sizeof(DWORD), 1, fpFileSave);
			uSize = fwrite(&m_dwNumAnimatedVertice, sizeof(DWORD), 1, fpFileSave);
			uSize = fwrite(&m_dwNumFrames, sizeof(DWORD), 1, fpFileSave);
			uSize = fwrite(&m_fFrameLength, sizeof(float), 1, fpFileSave);

			uSize = fwrite(m_pAnimatedVerticesIndex, sizeof(DWORD), m_dwNumAnimatedVertice, fpFileSave);
			uSize = fwrite(m_pAnimatedVerticesPosition, 
				sizeof(D3DXVECTOR3) * m_dwNumAnimatedVertice * m_dwNumFrames,
				1,
				fpFileSave);
			if(m_dwOptions & ENUM_CLIPOP_UVANI)
			{
				//Save uv animation
				DWORD dwUVAnimationSize = sizeof(D3DXVECTOR3) * m_dwNumAnimatedVertice * m_dwNumFrames;
				uSize = fwrite(m_pAnimationUV, dwUVAnimationSize, 1, fpFileSave);
			}
			break;
		}
	}

	uSize = fwrite(&dwBlockEnd, sizeof(DWORD), 1, fpFileSave);
	hr = S_OK;

Exit0:
	if (fpFileSave)
	{
		fclose(fpFileSave);
	}
	return hr;
}

unsigned KG3DAni::GetType()
{
	return m_dwType;
}

HRESULT KG3DAni::PrepareRender(unsigned uOption)
{
	return S_OK;
}

HRESULT KG3DAni::PostRender()
{
	return S_OK;
}

HRESULT KG3DAni::GetDateInformation(int *pnMemory, int *pnVideoMemory, int *pnCompressMem, int *pnCompressVideoMem)
{
	return S_OK;
}

unsigned KG3DAni::GetLastUseTime()
{
	return 0;
}

unsigned KG3DAni::GetUseFrequency()
{
	return 0;
}

ULONG STDMETHODCALLTYPE KG3DAni::AddRef()
{
	m_nReference++;
	return S_OK;
}

ULONG STDMETHODCALLTYPE KG3DAni::Release()
{
	m_nReference--;
	if(m_nReference <= 0)
	{
		return OnReferenceZero();
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE KG3DAni::QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
{
	return S_OK;
}

HRESULT KG3DAni::OnReferenceZero()
{
	HRESULT hr = g_cAni2Table.ReleaseResource(this, RELEASE_INQUEUE);
	return hr;
}


HRESULT KG3DAni::UpdateClip(KG3DAnimationController* pController, 
							 const std::vector<int>* pBoneLODList,
							 KG3DAnimationUpdateExtraInfoCollector *pExtraInfo)
{
	HRESULT hr = E_FAIL;

	KG_PROCESS_ERROR(pController);

	pController->m_AnimationInfo.ClipType = m_dwType;
	switch(m_dwType)
	{
	case ANIMATION_VERTICES_RELATIVE:
		{
			ASSERT(0);
			//hr = UpdateVerticesRelative();
			break;
		}
	case ANIMATION_VERTICES:
		{
			hr = UpdateVertices(pController);
			break;
		}
	case ANIMATION_BONE:
		{
			hr = UpdateWorld(pController, pBoneLODList);
			break;
		}
	case ANIMATION_BONE_RELATIVE:
		{
			hr = UpdateRelative(pController, pBoneLODList);
			break;
		}
	case ANIMATION_BONE_RTS:
		{
			hr = UpdateRTS(pController, pBoneLODList, pExtraInfo);
			break;
		}
	default:
		ASSERT(0);
	}
Exit0:
	return hr;	
}

HRESULT KG3DAni::UpdateRTS(KG3DAnimationController* pController, 
							const std::vector<int>* pBoneLODList,
							KG3DAnimationUpdateExtraInfoCollector *pExtraInfo)
{
	int nFrame = 0;
	int nFrameTo = 0;
	float fWeight = 0.0f;

	float fOrgStartTime = pController->GetStartTime();
	GetCurFrame(pController, nFrame, nFrameTo, fWeight);
	if (fOrgStartTime != pController->GetStartTime())
	{
		pController->SetModifyType(KG3DAnimationController::OFMT_FININSHED);
	}

	if (pBoneLODList)
	{
		if (!pExtraInfo)
		{
			KG_PROCESS_ERROR(pController->m_AnimationInfo.pMatrix);
			for (int i = 0; i < static_cast<int>(pBoneLODList->size()); i++)
			{
				int nA = (*pBoneLODList)[i] * m_dwNumFrames + nFrame;
				int nB = (*pBoneLODList)[i] * m_dwNumFrames + nFrameTo;
				RTS& rtsFrameA = m_pAnimatedBoneRTS[nA];
				RTS& rtsFrameB = m_pAnimatedBoneRTS[nB];
				InterpolateRTSBoneKeyFrame(pController->m_AnimationInfo.pMatrix[(*pBoneLODList)[i]], 
					rtsFrameA, 
					rtsFrameB, 
					fWeight);
			}
		}
		else
		{
			for (int i = 0; i < static_cast<int>(pBoneLODList->size()); i++)
			{
				int nA = (*pBoneLODList)[i] * m_dwNumFrames + nFrame;
				int nB = (*pBoneLODList)[i] * m_dwNumFrames + nFrameTo;
				RTS& rtsFrameA = m_pAnimatedBoneRTS[nA];
				RTS& rtsFrameB = m_pAnimatedBoneRTS[nB];
				RTS rtsResult;
				InterpolateRTSKeyFrame(rtsResult, 
					rtsFrameA,
					rtsFrameB,
					fWeight);					
				pExtraInfo->SaveData(i, &rtsResult);
			}
		}
	}
	else
	{
		if (!pExtraInfo)
		{
			KG_PROCESS_ERROR(pController->m_AnimationInfo.pMatrix);
			for (DWORD i = 0; i < m_dwNumAnimatedBone; i++)
			{
				int nA = i * m_dwNumFrames + nFrame;
				int nB = i * m_dwNumFrames + nFrameTo;
				RTS& rtsFrameA = m_pAnimatedBoneRTS[nA];
				RTS& rtsFrameB = m_pAnimatedBoneRTS[nB];
				InterpolateRTSBoneKeyFrame(pController->m_AnimationInfo.pMatrix[i], 
					rtsFrameA, 
					rtsFrameB, 
					fWeight);
			}
		}
		else		
		{
			for (DWORD i = 0; i < m_dwNumAnimatedBone; i++)
			{
				int nA = i * m_dwNumFrames + nFrame;
				int nB = i * m_dwNumFrames + nFrameTo;
				RTS& rtsFrameA = m_pAnimatedBoneRTS[nA];
				RTS& rtsFrameB = m_pAnimatedBoneRTS[nB];
				RTS rtsResult;
				D3DXMATRIX matResult;
				InterpolateRTSBoneKeyFrame(matResult, 
					rtsFrameA,
					rtsFrameB,
					fWeight);

				InterpolateRTSKeyFrame(rtsResult, 
					rtsFrameA, 
					rtsFrameB, 
					fWeight);
				RTS2Matrix(matResult, rtsResult);
				pExtraInfo->SaveData(i, &rtsResult);
			}
		}
	}
	pController->m_AnimationInfo.bNeedUpdateToWorld = TRUE;
Exit0:
	return S_OK;
}

HRESULT KG3DAni::UpdateRelative(KG3DAnimationController* pController,
								 const std::vector<int>* pBoneLODList)
{
	int nFrame = 0;
	int nFrameTo = 0;
	float fWeight = 0.0f;
	float fReverse = 0.0f;

	GetCurFrame(pController, nFrame, nFrameTo, fWeight);
	fReverse = 1.0f - fWeight;

	KG_PROCESS_ERROR(pController->m_AnimationInfo.pMatrix);
	if (pBoneLODList)
	{
		for (int i = 0; i < static_cast<int>(pBoneLODList->size()); i++)
		{
			int nA = (*pBoneLODList)[i] * m_dwNumFrames + nFrame;
			int nB = (*pBoneLODList)[i] * m_dwNumFrames + nFrameTo;
			D3DXMATRIX& matA = m_pAnimatedBoneMatrix[nA];
			D3DXMATRIX& matB = m_pAnimatedBoneMatrix[nB];
			pController->m_AnimationInfo.pMatrix[(*pBoneLODList)[i]] = matA * fReverse + matB * fWeight;
		}
	}
	else
	{
		for (DWORD i = 0; i < m_dwNumAnimatedBone; i++)
		{
			int nA = i * m_dwNumFrames + nFrame;
			int nB = i * m_dwNumFrames + nFrameTo;
			D3DXMATRIX& matA = m_pAnimatedBoneMatrix[nA];
			D3DXMATRIX& matB = m_pAnimatedBoneMatrix[nB];
			pController->m_AnimationInfo.pMatrix[i] = matA * fReverse + matB * fWeight;
		}
	}

	pController->m_AnimationInfo.bNeedUpdateToWorld = TRUE;
Exit0:
	return S_OK;
}

HRESULT KG3DAni::UpdateWorld(KG3DAnimationController* pController,
							  const std::vector<int>* pBoneLODList)
{
	int nFrame = 0;
	int nFrameTo = 0;
	float fWeight = 0.0f;
	float fReverse = 0.0f;

	GetCurFrame(pController, nFrame, nFrameTo, fWeight);
	fReverse = 1.0f - fWeight;

	KG_PROCESS_ERROR(pController->m_AnimationInfo.pMatrix);
	if (pBoneLODList)
	{
		for (int i = 0; i < static_cast<int>(pBoneLODList->size()); i++)
		{
			int nA = (*pBoneLODList)[i] * m_dwNumFrames + nFrame;
			int nB = (*pBoneLODList)[i] * m_dwNumFrames + nFrameTo;
			D3DXMATRIX& matA = m_pAnimatedBoneMatrix[nA];
			D3DXMATRIX& matB = m_pAnimatedBoneMatrix[nB];
			pController->m_AnimationInfo.pMatrix[(*pBoneLODList)[i]] = matA * fReverse + matB * fWeight;
		}
	}
	else
	{
		for (DWORD i = 0; i < m_dwNumAnimatedBone; i++)
		{
			int nA = i * m_dwNumFrames + nFrame;
			int nB = i * m_dwNumFrames + nFrameTo;
			D3DXMATRIX& matA = m_pAnimatedBoneMatrix[nA];
			D3DXMATRIX& matB = m_pAnimatedBoneMatrix[nB];
			pController->m_AnimationInfo.pMatrix[i] = matA * fReverse + matB * fWeight;
		}
	}

	pController->m_AnimationInfo.bNeedUpdateToWorld = FALSE;
Exit0:
	return S_OK;
}

HRESULT KG3DAni::UpdateVertices(KG3DAnimationController* pController)
{
	HRESULT hr = E_FAIL;
	VFormat::FACES_NORMAL_TEXTURE1* pVers = NULL;
	VFormat::FACES_NORMAL_TEXTURE1* pVersSrc = NULL;
	VFormat::FACES_NORMAL_TEXTURE2* pVersSrc1 = NULL;
	VFormat::FACES_NORMAL_TEXTURE2* pVers1 = NULL;
	int nFrame = 0;
	int nFrameTo = 0;
	float fWeight = 0.0f;
	float fReverse = 0.0f;
	DWORD dwFVF = 0;
	LPD3DXMESH pSrcMesh = pController->m_AnimationInfo.pMeshSrc;

	KG_PROCESS_ERROR(pSrcMesh);

	KG_PROCESS_ERROR(pSrcMesh->GetNumVertices() == m_dwNumAnimatedVertice);

	dwFVF = pSrcMesh->GetFVF();

	if (!pController->m_AnimationInfo.pMesh)
	{
		DWORD dwOption = pSrcMesh->GetOptions();

		hr = pSrcMesh->CloneMeshFVF(dwOption & (~D3DXMESH_MANAGED) | D3DXMESH_VB_DYNAMIC, 
			dwFVF,
			g_pd3dDevice, 
			&(pController->m_AnimationInfo.pMesh));
		KG_COM_PROCESS_ERROR(hr);
	}
	GetCurFrame(pController, nFrame, nFrameTo, fWeight);
	fReverse = 1.0f - fWeight;

	if (dwFVF & D3DFVF_TEX2)
	{
		hr = pController->m_AnimationInfo.pMeshSrc->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVersSrc1);
		KG_COM_PROCESS_ERROR(hr);
		hr = pController->m_AnimationInfo.pMesh->LockVertexBuffer(D3DLOCK_DISCARD, (void**)&pVers1);
		KG_COM_PROCESS_ERROR(hr);

		//Update vertices position data
		for (DWORD i = 0; i < m_dwNumAnimatedVertice; i++)
		{
			DWORD dwIndex = m_pAnimatedVerticesIndex[i];
			D3DXVECTOR3& PosA = m_pAnimatedVerticesPosition[i * m_dwNumFrames + nFrame];
			D3DXVECTOR3& PosB = m_pAnimatedVerticesPosition[i * m_dwNumFrames + nFrameTo];
			pVers1[dwIndex].p = PosA * fReverse + PosB * fWeight;
			pVers1[dwIndex].Normal = pVersSrc1[dwIndex].Normal;
			pVers1[dwIndex].tu2 = pVersSrc1[dwIndex].tu2;
			pVers1[dwIndex].tv2 = pVersSrc1[dwIndex].tv2;
			pVers1[dwIndex].tu1 = pVersSrc1[dwIndex].tu1;
			pVers1[dwIndex].tv1 = pVersSrc1[dwIndex].tv1;
		}


		if(m_dwOptions & ENUM_CLIPOP_UVANI)
		{
			//Update vertices uv data
			for (DWORD i = 0; i < m_dwNumAnimatedVertice; i++)
			{
				DWORD dwIndex = m_pAnimatedVerticesIndex[i];
				D3DXVECTOR3& UVA = m_pAnimationUV[i * m_dwNumFrames + nFrame];
				D3DXVECTOR3& UVB = m_pAnimationUV[i * m_dwNumFrames + nFrameTo];

				pVers1[dwIndex].tu1 = UVA.x * fReverse + UVB.x * fWeight;
				pVers1[dwIndex].tv1 = UVA.y * fReverse + UVB.y * fWeight;
			}
		}
	}
	else
	{
		hr = pController->m_AnimationInfo.pMeshSrc->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVersSrc);
		KG_COM_PROCESS_ERROR(hr);
		hr = pController->m_AnimationInfo.pMesh->LockVertexBuffer(D3DLOCK_DISCARD, (void**)&pVers);
		KG_COM_PROCESS_ERROR(hr);

		//Update vertices position data
		for (DWORD i = 0; i < m_dwNumAnimatedVertice; i++)
		{
			DWORD dwIndex = m_pAnimatedVerticesIndex[i];
			D3DXVECTOR3& PosA = m_pAnimatedVerticesPosition[i * m_dwNumFrames + nFrame];
			D3DXVECTOR3& PosB = m_pAnimatedVerticesPosition[i * m_dwNumFrames + nFrameTo];
			pVers[dwIndex].p = PosA * fReverse + PosB * fWeight;
			pVers[dwIndex].Normal = pVersSrc[dwIndex].Normal;
			pVers[dwIndex].tu1 = pVersSrc[dwIndex].tu1;
			pVers[dwIndex].tv1 = pVersSrc[dwIndex].tv1;
		}


		if(m_dwOptions & ENUM_CLIPOP_UVANI)
		{
			//Update vertices uv data
			for (DWORD i = 0; i < m_dwNumAnimatedVertice; i++)
			{
				DWORD dwIndex = m_pAnimatedVerticesIndex[i];
				D3DXVECTOR3& UVA = m_pAnimationUV[i * m_dwNumFrames + nFrame];
				D3DXVECTOR3& UVB = m_pAnimationUV[i * m_dwNumFrames + nFrameTo];

				pVers[dwIndex].tu1 = UVA.x * fReverse + UVB.x * fWeight;
				pVers[dwIndex].tv1 = UVA.y * fReverse + UVB.y * fWeight;
			}
		}
	}

Exit0:
	if (pController->m_AnimationInfo.pMesh)
	{
		pController->m_AnimationInfo.pMesh->UnlockVertexBuffer();
	}
	if (pController->m_AnimationInfo.pMeshSrc)
	{
		pController->m_AnimationInfo.pMeshSrc->UnlockVertexBuffer();
	}

	return hr;
}

HRESULT KG3DAni::UpdateVerticesRelative(KG3DAnimationController* pController)
{
	HRESULT hr = E_NOTIMPL;

	//Exit0:
	return hr;
}

void KG3DAni::GetCurFrame(KG3DAnimationController* pController,  int& nFrame, int& nFrameTo, float& fWeight)
{
	float fTime = (g_cGraphicsTool.GetNowTime()- pController->m_fAnimationStartTime) * pController->m_fSpeed;
	//	ATLTRACE("Time Span = %f\n", fTime);
	if (fTime < 0.0f)
	{
		pController->m_fAnimationStartTime = g_cGraphicsTool.GetNowTime();
		fTime = 0.0f;
	}

	switch(pController->m_dwAnimationPlayType)
	{
	case ENU_ANIMATIONPLAY_CIRCLE:
		{
			int nRepeatTimes = (int) (fTime / (m_fFrameLength * (m_dwNumFrames - 1)));
			float fSpanTime = fTime - (m_fFrameLength * (m_dwNumFrames - 1)) * nRepeatTimes;
			nFrame = (int)(fSpanTime / m_fFrameLength);
			pController->m_AnimationInfo.dwCurrentFrame = nFrame;
			if (nRepeatTimes != pController->m_nAnimationRepeatTime)
			{
				pController->m_AnimationInfo.dwCurrentFrame = m_dwNumFrames - 1;
				pController->m_nAnimationRepeatTime = nRepeatTimes;
				if (pController->GetEventHandler())
				{
					pController->GetEventHandler()->OnPlayAnimationFinish(pController);
				}
			}
			nFrameTo = nFrame + 1;

			if (nFrameTo >= static_cast<int>(m_dwNumFrames))
			{
				nFrameTo = 0;
			}
			fWeight = (fSpanTime -  nFrame * m_fFrameLength) / m_fFrameLength;
			break;
		}
	case ENU_ANIMATIONPLAY_ONCE:
		{
			int nRepeatTimes = (int) (fTime / (m_fFrameLength * (m_dwNumFrames - 1)));
			if (nRepeatTimes >= 1)
			{
				nFrame = static_cast<int>(m_dwNumFrames) - 1;
				nFrameTo = nFrame;
				fWeight = 1.0f;
			}
			else
			{
				float fSpanTime = fTime;
				nFrame = (int)(fSpanTime / m_fFrameLength);
				nFrameTo = nFrame + 1;
				fWeight = (fSpanTime -  nFrame * m_fFrameLength) / m_fFrameLength;
			}

			pController->m_AnimationInfo.dwCurrentFrame = nFrame;
			if (nRepeatTimes != pController->m_nAnimationRepeatTime)
			{
				pController->m_nAnimationRepeatTime = nRepeatTimes;
				pController->m_AnimationInfo.dwCurrentFrame = m_dwNumFrames - 1;
				if (pController->GetEventHandler())
				{
					pController->GetEventHandler()->OnPlayAnimationFinish(pController);
				}
			}
			break;
		}
		/*case ENU_CLIP_PLAY_NONE:
		{
		if (fTime > m_fFrameLength * m_dwNumFrames)
		{
		nFrame = m_dwNumFrames - 1;
		nFrameTo = m_dwNumFrames - 1;
		}
		else
		{
		nFrame = (int)(fTime / m_fFrameLength);
		if (nFrame >= static_cast<int>(m_dwNumFrames)) 
		{
		nFrame = m_dwNumFrames - 1;
		}
		nFrameTo = nFrame + 1;
		if (nFrameTo >= static_cast<int>(m_dwNumFrames))
		{
		nFrameTo = m_dwNumFrames - 1;
		}
		fWeight = (fTime - nFrame * m_fFrameLength) / m_fFrameLength;
		}
		break;
		}*/

		//case ENU_CLIP_PLAY_PINGPANG:
		//	{
		//		/*int T = (int)(fTime / (m_fFrameLength * m_dwNumFrames));
		//		float m_Time = fTime - (m_fFrameLength * m_dwNumFrames) * T;
		//		if (T % 2 == 0)
		//		{
		//			(*pnFrame) = (int)(m_Time / m_fFrameLength);
		//			if ((DWORD) *pnFrame >= m_dwNumFrames)
		//				*pnFrame = m_dwNumFrames - 1;
		//			*pnFrameTo = *pnFrame + 1;
		//			if ((DWORD) *pnFrameTo >= m_dwNumFrames) * pnFrameTo = m_dwNumFrames - 1;
		//			*S = (m_Time - *pnFrame * m_fFrameLength) / m_fFrameLength;
		//		}
		//		else
		//		{
		//			int K =  (int)(m_Time/m_fFrameLength);
		//			(*pnFrame) = m_dwNumFrames - K ;

		//			if ((DWORD) *pnFrame>=m_dwNumFrames) *pnFrame = m_dwNumFrames - 1;
		//			*pnFrameTo = *pnFrame - 1;
		//			if ((DWORD) *pnFrameTo<0) *pnFrameTo = 0;
		//			*S = (m_Time - K * m_fFrameLength)/m_fFrameLength;
		//		}*/
		//		break;
		//	}
		//case ENU_CLIP_PLAY_HOLD:
		//	{
		//		/*fTime = pModel->m_fCurAniStartTime;
		//		if (fTime > m_fFrameLength * m_dwNumFrames)
		//		{
		//			*pnFrame = m_dwNumFrames - 1;
		//			*pnFrameTo = m_dwNumFrames - 1;
		//		}
		//		else
		//		{
		//			(*pnFrame) = (int)(fTime / m_fFrameLength);
		//			if ((DWORD) *pnFrame >= m_dwNumFrames)
		//				*pnFrame = m_dwNumFrames - 1;
		//			*pnFrameTo = *pnFrame+1;
		//			if ((DWORD) *pnFrameTo>=m_dwNumFrames)
		//				*pnFrameTo = m_dwNumFrames - 1;
		//			*S = (fTime - *pnFrame * m_fFrameLength)/m_fFrameLength;
		//		}*/
		//	}
		//	break;
	}
	pController->m_dwLastFrame = pController->m_dwCurrentFrame;
	pController->m_dwCurrentFrame = nFrame;
}

void KG3DAni::ExchangePercentage2Time(float fPercentage, float& fTime)
{
	ASSERT(fPercentage >= 0.0f && fPercentage <= 1.0f && m_dwNumFrames);
	fTime = (m_dwNumFrames - 1) * m_fFrameLength * fPercentage;
}

void KG3DAni::InterpolateRTSBoneKeyFrame(D3DXMATRIX& OutRelativeMat, RTS& FrameA, RTS& FrameB, float fWeight)
{
	D3DXMATRIX matSRot, matSRotInv, matRot, matScale, matTrans, matScaleSign;
	D3DXQUATERNION sRot, Rot;
	D3DXVECTOR3 Scale, Translation;
	D3DXMATRIX MatFrameA, MatFrameB;
	D3DXQuaternionSlerp(&sRot, &FrameA.SRotation, &FrameB.SRotation, fWeight);
	D3DXMatrixRotationQuaternion(&matSRot, &sRot);
	D3DXMatrixTranspose(&matSRot, &matSRot);
	D3DXMatrixInverse(&matSRotInv, NULL, &matSRot);
	D3DXQuaternionSlerp(&Rot, &FrameA.Rotation, &FrameB.Rotation, fWeight);
	D3DXMatrixRotationQuaternion(&matRot, &Rot);
	D3DXMatrixTranspose(&matRot, &matRot);
	D3DXVec3Lerp(&Scale, &FrameA.Scale, &FrameB.Scale, fWeight);
	D3DXVec3Lerp(&Translation, &FrameA.Translation, &FrameB.Translation, fWeight);
	D3DXMatrixScaling(&matScale, Scale.x, Scale.y, Scale.z);
	D3DXMatrixScaling(&matScaleSign, FrameA.Sign, FrameA.Sign, FrameA.Sign);
	D3DXMatrixTranslation(&matTrans, Translation.x, Translation.y, Translation.z);
	OutRelativeMat = matSRotInv * matScale * matSRot * matRot * matScaleSign * matTrans;
}



int KG3DAni::FindBone(const TCHAR* strBoneName)
{
	int nReturn = -1;
	for (int i = 0; i < (int)m_dwNumAnimatedBone; i++)
	{
		if (!strcmp(m_ppAnimatedBoneName[i], strBoneName))
		{
			nReturn = i;
			break;
		}
	}
	return nReturn;
}

void KG3DAni::Rename(const TCHAR* strNewName)
{
	if (!strNewName)
	{
		m_strClipName = "";
	}
	else
	{
		m_strClipName = strNewName;
	}
} 

HRESULT KG3DAni::NormalizeClipData(IEKG3DMesh* piMesh, BOOL bSaveToFile)
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hr = E_FAIL;
	int nNumBones = 0;
	KG3DMesh *pMesh = NULL;

	pMesh = dynamic_cast<KG3DMesh *>(piMesh);
	KGLOG_PROCESS_ERROR(pMesh);

	if (m_dwType != ANIMATION_BONE_RELATIVE &&
		m_dwType != ANIMATION_BONE_RTS)
		return S_OK;

	KGLOG_PROCESS_ERROR(pMesh->m_BoneInfo);
	KGLOG_PROCESS_ERROR(pMesh->m_dwNumBone == m_dwNumAnimatedBone);

	hr = pMesh->GetNumBones(&nNumBones);
	KGLOG_COM_PROCESS_ERROR(hr);

	for (int i = 0; i < nNumBones; i++)
	{
		int j = FindBone(pMesh->m_BoneInfo[i].strBoneName);
		KG_PROCESS_ERROR(j != -1);

		if ( j != i)
			SwapBoneAnimationData(j, i);
	}

	if (bSaveToFile)
	{
		hr = SaveToFile(m_scName.c_str(), 0);
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
	}

	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT KG3DAni::NormalizeClipData(IEKG3DAni* pIEClip, BOOL bSaveToFile)
{
	HRESULT hr = E_FAIL;

	KG3DAni *pClip = dynamic_cast<KG3DAni *>(pIEClip);
	KGLOG_PROCESS_ERROR(pClip);

	KG_PROCESS_SUCCESS(m_dwType == ANIMATION_VERTICES);
	KG_PROCESS_SUCCESS(m_dwType == ANIMATION_VERTICES_RELATIVE);
	KG_PROCESS_SUCCESS(pClip->m_dwType == ANIMATION_VERTICES);
	KG_PROCESS_SUCCESS(pClip->m_dwType == ANIMATION_VERTICES_RELATIVE);

	for (int i = 0; i < static_cast<int>(m_dwNumAnimatedBone); i++)
	{
		int	j = pClip->FindBone(m_ppAnimatedBoneName[i]);
		KG_PROCESS_ERROR(j != -1);

		if (j != i)
			pClip->SwapBoneAnimationData(j, i);
	}

	if (bSaveToFile)
	{
		hr = pClip->SaveToFile(pClip->m_scName.c_str(), 0);
		KG_COM_PROCESS_ERROR(hr);
	}
Exit1:
	hr = S_OK;
Exit0:
	return hr;
}




void KG3DAni::SwapBoneAnimationData(int nBone1, int nBone2)
{
	TCHAR strBoneNameTemp[MAX_BONENAME_LEN];
	KG_PROCESS_ERROR(nBone1 != nBone2);

	switch(m_dwType)
	{
	case ANIMATION_BONE_RELATIVE:
		{
			size_t SizeToCopy = sizeof(D3DXMATRIX) * m_dwNumFrames;

			D3DXMATRIX* pMatTemp = new D3DXMATRIX[m_dwNumFrames];
			KG_PROCESS_ERROR(pMatTemp);

			D3DXMATRIX* pData1 = &m_pAnimatedBoneMatrix[nBone1 * m_dwNumFrames];
			D3DXMATRIX* pData2 = &m_pAnimatedBoneMatrix[nBone2 * m_dwNumFrames];

			memcpy(pMatTemp, pData1, SizeToCopy);
			memcpy(pData1, pData2, SizeToCopy);
			memcpy(pData2, pMatTemp, SizeToCopy);
			SAFE_DELETE_ARRAY(pMatTemp);
		}
		break;
	case ANIMATION_BONE_RTS:
		{
			size_t SizeToCopy = sizeof(RTS) * m_dwNumFrames;

			RTS* pMatTemp = new RTS[m_dwNumFrames];
			KG_PROCESS_ERROR(pMatTemp);

			RTS* pData1 = &m_pAnimatedBoneRTS[nBone1 * m_dwNumFrames];
			RTS* pData2 = &m_pAnimatedBoneRTS[nBone2 * m_dwNumFrames];

			memcpy(pMatTemp, pData1, SizeToCopy);
			memcpy(pData1, pData2, SizeToCopy);
			memcpy(pData2, pMatTemp, SizeToCopy);
			SAFE_DELETE_ARRAY(pMatTemp);
		}
		break;
	}

	//Swap bone name here
	strcpy(strBoneNameTemp, m_ppAnimatedBoneName[nBone1]);
	strcpy(m_ppAnimatedBoneName[nBone1], m_ppAnimatedBoneName[nBone2]);
	strcpy(m_ppAnimatedBoneName[nBone2], strBoneNameTemp);

Exit0:
	return;
}

int KG3DAni::GetMatrixsRange(
							  vector<D3DXMATRIX>& vecMatrixs,
							  INT    nBoneIndex,
							  FLOAT  fBeginFrame, 
							  FLOAT  fEndFrame, 
							  FLOAT  fPrecision
							  )
{
	HRESULT hResult  = E_FAIL;

	//vector<D3DXMATRIX> vecMatrixs;

	ASSERT(fPrecision >= 0.0f);

	FLOAT fIter = fBeginFrame + fPrecision;

	if (fIter > fEndFrame)
		fIter = fEndFrame;

	while (fIter <= fEndFrame)
	{
		INT   nFrame   = static_cast<INT>(fIter);
		INT   nFrameTo = nFrame + 1;
		FLOAT fWeight  = fIter - nFrame;

		if (nFrame >= static_cast<int>(m_dwNumFrames))
			nFrame = 0;

		if (nFrameTo >= static_cast<int>(m_dwNumFrames))
			nFrameTo = 0;

		INT S = nBoneIndex * m_dwNumFrames + nFrame;
		INT T = nBoneIndex * m_dwNumFrames + nFrameTo;

		RTS& rtsFrameS = m_pAnimatedBoneRTS[S];
		RTS& rtsFrameT = m_pAnimatedBoneRTS[T];

		D3DXMATRIX matrix;
		InterpolateRTSBoneKeyFrame(
			matrix, 
			rtsFrameS, 
			rtsFrameT, 
			fWeight
			);
		vecMatrixs.push_back(matrix);

		fIter += fPrecision;
	}

	if (fIter > fEndFrame && fIter < fEndFrame + fPrecision)
	{
		fIter = fEndFrame;

		INT   nFrame   = static_cast<INT>(fIter);
		INT   nFrameTo = nFrame + 1;
		FLOAT fWeight  = fIter - nFrame;

		if (nFrame >= static_cast<int>(m_dwNumFrames))
			nFrame = 0;

		if (nFrameTo >= static_cast<int>(m_dwNumFrames))
			nFrameTo = 0;

		INT S = nBoneIndex * m_dwNumFrames + nFrame;
		INT T = nBoneIndex * m_dwNumFrames + nFrameTo;

		RTS& rtsFrameS = m_pAnimatedBoneRTS[S];
		RTS& rtsFrameT = m_pAnimatedBoneRTS[T];

		D3DXMATRIX matrix;
		InterpolateRTSBoneKeyFrame(
			matrix, 
			rtsFrameS, 
			rtsFrameT, 
			fWeight
			);
		vecMatrixs.push_back(matrix);
	}
	hResult = S_OK;
	//Exit0:
	return (int)vecMatrixs.size();
}


float KG3DAni::ConvertFrameToTime(UINT uFrameIndex)
{
	float fReturn = 0.0f;
	KG_PROCESS_ERROR(uFrameIndex < m_dwNumFrames);
	uFrameIndex = (static_cast<int>(uFrameIndex) - 1) > 0 ? uFrameIndex - 1 : 0;
	fReturn = m_fFrameLength * uFrameIndex;
Exit0:
	return fReturn;
}

UINT KG3DAni::TimeToFrame(float fTime, 
						   float fSpeed, 
						   UINT* pRepeatTimes)
{
	UINT uReturn = 0;
	KG_PROCESS_ERROR(fSpeed != 0.0f);
	float fTimeRange = (m_dwNumFrames - 1) * m_fFrameLength / fSpeed;
	UINT uRepeatTimes = static_cast<int>(fTime / fTimeRange);
	if (pRepeatTimes)
	{
		*pRepeatTimes = uRepeatTimes;
	}
	uReturn = static_cast<int>((fTime - uRepeatTimes * fTimeRange) / m_fFrameLength);
Exit0:
	return uReturn;
}

HRESULT KG3DAni::LoadHeaderInfo(LPCSTR strFileName, HeaderInfo* pInfo)
{
	HRESULT hr = E_FAIL;
	IFile* fpFile = NULL;
	KG_PROCESS_ERROR(strFileName);
	KG_PROCESS_ERROR(pInfo);

	fpFile = g_OpenFile(strFileName);
	KG_PROCESS_ERROR(fpFile);

	//    size_t size = sizeof(HeaderInfo);

	fpFile->Read(pInfo, sizeof(HeaderInfo));
	hr = S_OK;
Exit0:
	if (fpFile)
	{
		SAFE_RELEASE(fpFile);
	}
	return hr;
}

HRESULT KG3DAni::CheckLoadState()
{
	HRESULT hRetCode = E_FAIL;
	if (m_eDynamicLoadState == STATE_READY)
	{
		hRetCode = S_OK;
	}
	return hRetCode;
}