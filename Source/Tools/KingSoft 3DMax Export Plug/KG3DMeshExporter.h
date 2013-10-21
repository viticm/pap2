#pragma once
#include "kg3danimationSupporter.h"

class KG3DMeshExporter : public KG3DAnimationSupporter
{
	struct BoneInfo
	{
		std::string strBoneName;
		std::vector<DWORD> VerticesIndexRef;
		std::vector<float> VerticesWeightRef;
		D3DXMATRIX MatrixOffset;
		D3DXMATRIX BaseBoneMatrix;
	};

	struct BoneInfoSave
	{
		
	};

public:
	KG3DMeshExporter(INode* pNode);
	~KG3DMeshExporter(void);
	virtual int GetNumNodes();
	virtual INode* GetNode(unsigned int nIndex);
	virtual void* GetExtraInfo();
	
	void ExportSkinInfo();
	void ExportMeshData();
	void Helper_ExportISkin(Modifier* pModifier);
	void Helper_ExportPhysique(Modifier* pModifier);

	void ExportMeshData(TimeValue T);
	Point3* Helper_GetPosition(TimeValue T);
	Point3* Helper_GetNormal(TimeValue T);
	Point3* Helper_GetUV(TimeValue T);
	D3DXCOLOR* Helper_GetDiffuse(TimeValue T);


private:
	INode* m_pNode;
	std::vector<BoneInfo> m_BoneInfo; 
	std::map<INode*, int> m_NodeBoneInfoTable;
};
