#include "header.h"
#include "KG3DHelper.h"
#include "KG3DRTSAnimation.h"
#include "KG3DRelinkTable.h"

KG3DRTSAnimation::KG3DRTSAnimation(void)
{
}

KG3DRTSAnimation::~KG3DRTSAnimation(void)
{
}

void KG3DRTSAnimation::GetNodeData(INode* pNode, const TimeValue& tTime, void* pData)
{
	assert(m_pSupporter);
	assert(m_pRelinkTable);
	assert(pNode);

	Matrix3 Scale;
	Scale.SetScale(Point3(2.54f, 2.54f, 2.54f));

	RTSInfo* pInfo = static_cast<RTSInfo*>(pData);
	Matrix3 matNode;
	INode* pNodeParent = m_pRelinkTable->GetParentNode(pNode);
#ifdef _DEBUG
	TSTR strBoneName = pNode->NodeName();
	if (pNodeParent)
	{
		TSTR strParentName = pNodeParent->NodeName();
	}
#endif
	Matrix3 matLocal;
	if (pNodeParent)
	{
		Matrix3 matParent = pNodeParent->GetNodeTM(tTime);
		matParent.Invert();
		matNode = pNode->GetNodeTM(tTime);
		matLocal = matNode * matParent;
	}
	else 
	{
		//root node
		matNode = pNode->GetNodeTM(tTime);
		matLocal = matNode;
		if (GetExportOption()->m_bInch2CM)
		{
			matLocal = matLocal * Scale;
		}
	} 

	if (GetExportOption()->m_bExchangeYZ)
	{
		GetHelper().ChangeMatrixYZ(&matLocal);
	}

	ExchangeMatToRTS(matLocal, *pInfo);
}

void KG3DRTSAnimation::ExchangeMatToRTS(const Matrix3& matSrc, RTSInfo& rtsDest)
{
	AffineParts ap;
	decomp_affine(matSrc, &ap);

	rtsDest.Scale.x = ap.k.x;
	rtsDest.Scale.y = ap.k.y;
	rtsDest.Scale.z = ap.k.z;

	rtsDest.Rotation.x = ap.q.x;
	rtsDest.Rotation.y = ap.q.y;
	rtsDest.Rotation.z = ap.q.z;
	rtsDest.Rotation.w = ap.q.w;

	rtsDest.Translation.x = ap.t.x;
	rtsDest.Translation.y = ap.t.y;
	rtsDest.Translation.z = ap.t.z;

	rtsDest.Sign = ap.f;

	rtsDest.SRotation.x = ap.u.x;
	rtsDest.SRotation.y = ap.u.y;
	rtsDest.SRotation.z = ap.u.z;
	rtsDest.SRotation.w = ap.u.w;
}

size_t KG3DRTSAnimation::GetNodeDataLength()
{
	return sizeof(RTSInfo);
}

void KG3DRTSAnimation::Attach(KG3DAnimationSupporter* pSupporter)
{
	KG3DAnimationCollector::Attach(pSupporter);
	void* pExtraInfo = pSupporter->GetExtraInfo();
	assert(pExtraInfo);
	m_pRelinkTable = reinterpret_cast<KG3DRelinkTable*>(pExtraInfo);
}

void KG3DRTSAnimation::Dettach()
{
	m_pRelinkTable = NULL;
	KG3DAnimationCollector::Dettach();
}


KG3DRTSAnimation::AnimationHeader::AnimationHeader()
{
	dwMask = KG3DAnimationCollector::m_dwMask;
	dwBlockLength = 0;
	dwNumAnimations = 1;
	dwAnimationType = ANIMAIION_TYPE_BONE_RTS;
	memset(strAnimationName, 0, sizeof(TCHAR) * 30);
}

HRESULT KG3DRTSAnimation::SaveToFile(const TCHAR* strFileName)
{
	if (!strFileName)
		return E_FAIL;

	FILE* fpFile = fopen(strFileName, "wb");
	if (!fpFile)
		return E_FAIL;

	AnimationHeader Header;
	GetExportOption()->m_strAnimationName[29] = '\0';//ǿ�ƵĽض�desc
	strcpy_s(Header.strAnimationName, 30, GetExportOption()->m_strAnimationName);
	
	BoneRTSHeader RTSHeader;
	RTSHeader.dwNumBones = m_pSupporter->GetNumNodes();
	RTSHeader.dwNumFrames = static_cast<DWORD>(m_Data.size());
	if (GetExportOption()->m_dwInterval != 0)
	{
		RTSHeader.fFrameLength = static_cast<DWORD>(GetExportOption()->m_dwInterval);
	}
	else
	{
		RTSHeader.fFrameLength = GetTicksPerFrame() * GetExportOption()->m_nKeyFrameStep;
	}

	TCHAR* pNameBuffer = new TCHAR[RTSHeader.dwNumBones * MAX_BONE_NAME_LENGTH];
	for(int i = 0; i < RTSHeader.dwNumBones; i++)
	{
		strcpy(pNameBuffer + i * MAX_BONE_NAME_LENGTH, m_pSupporter->GetNode(i)->NodeName());
	}
	
	fwrite(&Header, sizeof(AnimationHeader), 1, fpFile);
	fwrite(&RTSHeader, sizeof(BoneRTSHeader), 1, fpFile);
	fwrite(pNameBuffer, sizeof(TCHAR) * RTSHeader.dwNumBones * MAX_BONE_NAME_LENGTH, 1, fpFile);
	
	//Here we do some exchange to fit the old version
	//for(size_t j = 0; j < m_Data.size(); j++)
	//{
	//	fwrite(m_Data[j].pData, sizeof(RTSInfo) * RTSHeader.dwNumBones, 1, fpFile);
	//}

	RTSInfo* pReArrage = new RTSInfo[RTSHeader.dwNumFrames];

	for (DWORD i = 0; i < RTSHeader.dwNumBones; i++)
	{
		for(DWORD j = 0; j < RTSHeader.dwNumFrames; j++)
		{
			RTSInfo* pData = (RTSInfo*)m_Data[j].pData;
			memcpy(&pReArrage[j], &pData[i], sizeof(RTSInfo));
		}
		fwrite(pReArrage, sizeof(RTSInfo) * RTSHeader.dwNumFrames, 1, fpFile);
	}

	SAFE_DELETE_ARRAY(pReArrage);
	SAFE_DELETE_ARRAY(pNameBuffer);
	fclose(fpFile);
	return S_OK;
}

void KG3DRTSAnimation::GetFrameData(const TimeValue& t)
{
	unsigned int uNumNode = m_pSupporter->GetNumNodes();
	if (!uNumNode)
		return;

	RTSInfo* pBuffer = new RTSInfo[uNumNode];
	
	for (int i = 0; i < uNumNode; i++)
	{
		INode* pNode = m_pSupporter->GetNode(i);
		GetNodeData(pNode, t, (void*)(pBuffer + i));
	}
	AnimationData AniData = { t, pBuffer };
	m_Data.push_back(AniData);
}

