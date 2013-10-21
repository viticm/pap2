#pragma once
#include "kg3drenderstate.h"
//#include "engine\KG_SimpleVector.h"
class KG3DTexture;
class KG3DRuntimeMaterial
{
public:
	KG3DRuntimeMaterial(void);
	~KG3DRuntimeMaterial(void);
	BOOL IsgUseGadualChange(D3DCOLORVALUE& A,D3DCOLORVALUE& B,float maxSpace);
	BOOL IsEnable() { return m_bEnable; }
	void Enable(BOOL bEnable) { m_bEnable = bEnable; }
	
	HRESULT BeginRender();
	void EndRender();
	
	HRESULT SetTexture(DWORD dwTextureStage, KG3DTexture* pTexture);
	void SetRenderState(
		D3DRENDERSTATETYPE State,
		DWORD Value
		);

	void SetTextureStageState(
		DWORD Stage,
		D3DTEXTURESTAGESTATETYPE Type,
		DWORD Value
		);

	void SetSamplerState(
		DWORD Sampler,
		D3DSAMPLERSTATETYPE Type,
		DWORD Value
		);

	void Clear();
	void SetAmbient(const D3DCOLORVALUE& Ambient)
	{ 
		m_bEnable = TRUE;
		m_Material.Ambient = Ambient;
		m_dwMaterialFlag |= MT_FLAG_AMBIENT;
	}
	void SetDiffuse(const D3DCOLORVALUE& Diffuse)
	{
		m_bEnable = TRUE;
		m_Material.Diffuse = Diffuse;
		m_dwMaterialFlag |= MT_FLAG_DIFFUSE;
	}

	void SetLightDiffuse(int nIndex, const D3DCOLORVALUE& Diffuse);
	void SetLightAmbient(DWORD dwAmbient)
	{
		m_bEnableLightAmbient = TRUE; 
		m_dwAmbient = dwAmbient;
	}

	//for subsetreneder only
	void ApplyMaterial(D3DMATERIAL9 &material);
	void SetAlpha(float fAlpha);
	void SetHighLight(float fIlluminationLevel);
protected:
	enum enuMaterialFlag
	{
		MT_FLAG_AMBIENT  = 1,
		MT_FLAG_DIFFUSE  = (1 << 1),
		MT_FLAG_SPECULAR = (1 << 2),
	};
	struct LightPara
	{
		D3DCOLORVALUE Value;
		D3DCOLORVALUE SaveValue;
	}; 
	std::map<int , LightPara> m_LightDiffuse;
	DWORD m_bEnableLightAmbient;
	DWORD m_dwAmbient;
	DWORD m_dwMaterialFlag;
	D3DMATERIAL9 m_Material;
	D3DMATERIAL9 m_MaterialSave;
	//std::vector<KG3DTexture*> m_Texture;
	BOOL m_bEnable;
	/*KG_SimpleVector<StateBlock>*/std::vector<StateBlock> m_State;//用vector的reserve就可以得到同样的效果了，不用把SimplerVector的头文件放上面造成耦合
	KG3DRenderState m_RenderState;
	D3DCOLORVALUE m_LastDiffuse;
	D3DCOLORVALUE m_CurDiffuse;
	D3DCOLORVALUE m_Diffuse;
	DWORD m_dwLastTime;
	//for subsetreneder only
	float m_fAlpha;
	float m_fIlluminationLevel;
};