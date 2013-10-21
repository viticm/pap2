#include "header.h"
#include "KG3DMeshExporter.h"
#include "KG3DHelper.h"

KG3DMeshExporter::KG3DMeshExporter(INode* pNode)
{
	m_pNode = pNode;
}

KG3DMeshExporter::~KG3DMeshExporter(void)
{
	m_pNode = NULL;
}


int KG3DMeshExporter::GetNumNodes()
{
	return 1;
}

INode* KG3DMeshExporter::GetNode(unsigned int nIndex)
{
	return m_pNode;
}

void* KG3DMeshExporter::GetExtraInfo()
{
	return NULL;
}

void KG3DMeshExporter::ExportSkinInfo()
{
	if (!m_pNode)
		return;

	m_BoneInfo.clear();
	m_NodeBoneInfoTable.clear();

	Modifier* pMod = GetHelper().FindSkinModifier(m_pNode);
	if (pMod)
	{
		Helper_ExportISkin(pMod);
		return;
	}

	pMod = GetHelper().FindPhysiqueModifier(m_pNode);
	if (pMod)
	{
		Helper_ExportPhysique(pMod);
	}
	return;
}

void KG3DMeshExporter::Helper_ExportISkin(Modifier* pModifier)
{
	ISkin* pSkin = static_cast<ISkin*>(pModifier->GetInterface(I_SKIN));
	if (!pSkin)
		return;
	
	int nNumBones = pSkin->GetNumBones();
	
	if (!nNumBones)
		return;

	m_BoneInfo.resize(nNumBones);

	for (int i = 0; i < nNumBones; i++)
	{
		INode* pNode = pSkin->GetBone(i);
		m_BoneInfo[i].strBoneName = pNode->NodeName();
		m_NodeBoneInfoTable[pNode] = i;
	}

	ISkinContextData* pSkinContext = pSkin->GetContextInterface(m_pNode);
	if (!pSkinContext)
		return;
	
	int nNumVertices = pSkinContext->GetNumPoints();
	for (int j = 0; j < nNumVertices; j++)
	{
		int nNumAssignedBones = pSkinContext->GetNumAssignedBones(j);
		for (int k = 0; k < nNumAssignedBones; k++)
		{
			int nBoneIndex = pSkinContext->GetAssignedBone(j, k);
			float fWeight = pSkinContext->GetBoneWeight(j, k);
			INode* pNode = pSkin->GetBone(nBoneIndex);
			if (m_NodeBoneInfoTable.count(pNode))
			{
				int nIndex = m_NodeBoneInfoTable[pNode];
				m_BoneInfo[nIndex].VerticesIndexRef.push_back(j);
				m_BoneInfo[nIndex].VerticesWeightRef.push_back(fWeight);
			}
		}
	}
}

void KG3DMeshExporter::Helper_ExportPhysique(Modifier* pModifier)
{
	
}


void KG3DMeshExporter::ExportMeshData(TimeValue T)
{

}

Point3* KG3DMeshExporter::Helper_GetPosition(TimeValue T)
{
	return NULL;
}

Point3* KG3DMeshExporter::Helper_GetNormal(TimeValue T)
{
	return NULL;
}

Point3* KG3DMeshExporter::Helper_GetUV(TimeValue T)
{
	return NULL;
}

D3DXCOLOR* KG3DMeshExporter::Helper_GetDiffuse(TimeValue T)
{
	return NULL;
}



