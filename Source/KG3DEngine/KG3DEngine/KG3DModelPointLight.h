#pragma once
#include "kg3dmodel.h"
#include "IEOther.h" 
#include "IEKG3DSceneSFXEditor.h"

class KG3DSFX;

struct KG3DModelLightExtData
{
	D3DCOLORVALUE vColor;
	int           nType;
	int           dwOption;
	DWORD         dwExtend[64];

	KG3DModelLightExtData()
	{
		ZeroMemory(this,sizeof(KG3DModelLightExtData));
	}
};

class KG3DModelPointLight : public KG3DModel
{
protected:
	friend struct KG3D_PointLight_Info;

public :
	D3DCOLORVALUE m_vColor;//ÑÕÉ«
	//float         m_fRadius;//°ë¾¶

	BOOL          m_bEnable;
	LPDIRECT3DCUBETEXTURE9 m_lpCubeTexture;

	KG3DLightType m_eType;

public:
	D3DXVECTOR3 GetPositionWorld();

	virtual BOOL RayIntersection(const D3DXVECTOR3& vSrc , const D3DXVECTOR3& vNormalizedDir, FLOAT* pfRet ,D3DXVECTOR3* pFaceNormal , BOOL bIntersectBoxOnly , KG3DIntersectionFilter* pFilter /* = NULL */);

    static HRESULT StaticInit();
    static HRESULT StaticUnInit();
    
    virtual HRESULT SetModelColor(DWORD dwSubset, D3DCOLORVALUE color); 
    virtual HRESULT GetModelColor(DWORD dwSubset, D3DCOLORVALUE* color);   

    virtual HRESULT	RenderSelectMark(DWORD dwColor = 0xFFFFFFFF);
    virtual BOOL    IsResourceReady(BOOL bTestTexture);

    virtual const D3DXVECTOR3* GetPointLightPos()
    {
        return (D3DXVECTOR3*)&m_matWorld._41;
    }

    virtual const D3DCOLORVALUE* GetPointLightColor()
    {
        return &m_vColor;
    }

    virtual float GetPointLightSize()
    {
        return m_BBox.GetDiameter() * 0.5f; 
    }

	BOOL IsEnable()
	{
		return m_bEnable;
	}
	virtual HRESULT ComputeBBox();

	HRESULT Enable(BOOL bEnable);
	virtual HRESULT LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption);
	virtual HRESULT SaveToFile(const char cszFileName[], unsigned uOption);

	virtual HRESULT GetMeshFileName(LPTSTR strFileName);

	virtual HRESULT Render(unsigned uOption, void* pExtInfo);

    static KG3DSFX* m_spSign;

	KG3DModelPointLight(void);
	~KG3DModelPointLight(void);

	float GetRadius();

	HRESULT GetExtData(KG3DModelLightExtData& vData);
	HRESULT SetExtData(KG3DModelLightExtData& vData);
};
