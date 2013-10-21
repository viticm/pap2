#pragma once
#include "kg3dmodel.h"
#include "KG3DModelSpeedTree.h"
#include "KG3DStructs.h"
#include "KG3DSceneEntity.h"

class KG3DSceneSelectionTool;
class KG3DSceneModelEditor;
class KG3DSpeedTreeLeafGroup : public KG3DModel
{
public:
	//virtual HRESULT ComputeBBox();
	virtual HRESULT Render(unsigned uOption, void* pExtInfo);
public:
	KG3DSpeedTreeLeafGroup(void);
	~KG3DSpeedTreeLeafGroup(void);
	void OnRotation(const D3DXQUATERNION &vec3Rotation);
	void OnMove(const D3DXVECTOR3 &vec3Offset);
	void OnScale(const D3DXVECTOR3 &vec3Scale);
	BOOL AddLeaf(Leaf* pLeaf);
	void RemoveLeaf(Leaf* pLeaf);
	void Clear(KG3DSceneModelEditor* pScene);
	Leaf* GetFirstLeaf();
	void GetCenter(D3DXVECTOR3& vec3Center);
	DWORD GetScaleMode();
	void SetScaleMode(DWORD dwScale);
    //KG3DSceneEntityList ConvertLeavesToEntityList();
	HRESULT SetAllLeavesSelectable(KG3DSceneEditorBase& editBase);

    D3DXMATRIX GetWorldMatrix();
    D3DXMATRIX GetMatrix();
    void UpdateByMatrix(const D3DXMATRIX& mat);
    
private:
	void OnLeafScale(const D3DXVECTOR3 &vec3Scale);
	void OnScaleUniform(const D3DXVECTOR3& vec3Scale);
	void CalculateCenter(D3DXVECTOR3& vec3Center);
	std::set<Leaf*> m_Leaves;
	D3DXVECTOR3 m_vec3Center;
	BBox m_BBox;
	DWORD m_dwScaleMode;
};
