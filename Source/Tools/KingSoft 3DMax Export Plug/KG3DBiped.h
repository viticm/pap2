#pragma once

#include "kg3dsk.h"
#include "kg3drelinktable.h"
#include "kg3dblacklist.h"
#include "KG3DAnimationSupporter.h"

class KG3DBiped : public KG3DAnimationSupporter
{
	typedef std::vector<INode*> Biped;
	
	class BoneInfo
	{
	public:
		HRESULT SaveBoneInfo(FILE* fpFile);
		void FillBoneInfo(INode* pNode, KG3DRelinkTable* pRelinkTable);
		BoneInfoData m_Data;
		INode* m_pNode;
	};
	
public:

	KG3DBiped();
	~KG3DBiped(void);
	HRESULT ExportRTSAnimation(const TCHAR* strFileName);

	INode* FindBone(const TCHAR* strInBoneName);

	HRESULT SaveSkeletonToFile(const TCHAR* strFileName);
	HRESULT GenerateSkeletonFromNode(INode* pNode, BOOL bBipOnly);
	HRESULT LoadConfigFile(const TCHAR* strFileName);
	void SetIP(Interface* pIP) { assert(pIP); m_ip = pIP; }
	void OutputBipedText(const TCHAR* strFilename);
	HRESULT GenerateSkeletonFromMesh(CMesh* pMesh);
	
							 
	void ApplyBlackList();

	INode* GetParentNode(INode* pNode);
	int GetNumChildren(INode* pNode); 
	INode* GetChildNode(INode* pNode, int i);

	virtual int GetNumNodes();
	virtual INode* GetNode(unsigned int nIndex);
	virtual void* GetExtraInfo();

private:
	void Helper_GenerateSKFromMesh(int i, CMesh* pMesh);
	int InsertChildBone(INode* pNode);
	void EraseBone(int nIndex);
	int GetBoneIndex(const INode* pNode);
private:
	BOOL m_bStandardBipeLoaded;
	Interface* m_ip; 
	Biped m_Skeleton;
	KG3DBlackList m_BlackList;
	KG3DRelinkTable m_RelinkTable;
	std::vector<int> m_BaseBones;
};
