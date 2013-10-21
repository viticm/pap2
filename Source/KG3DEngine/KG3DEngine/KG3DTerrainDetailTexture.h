#pragma once
#include "IEEditor.h"
class KG3DTexture;
class KG3DMemoryFile;
class KG3DSceneOutDoorSpaceMgr;

class KG3DTerrainDetailTexture
{
public:
	BYTE         m_byteIndex;
	string       m_szDiffuseFileName;//文件名
	KG3DTexture* m_lpDiffuseTexture;
	KG3DTexture* m_lpNormalTexture;

	BYTE         m_byTexCoordIndex;
	float        m_fScaleU;
	float        m_fScaleV;
	float        m_fEnvReflection;//反射系数
	float        m_fSpecular;//高光系数
	float        m_fEmissive;//自发光系数 ，这三个系数都基于地表贴图的ALPHA通道数值
	DWORD        m_dwMaterialType;//音效+表现类型

	D3DXVECTOR4 m_vUT;
	D3DXVECTOR4 m_vVT;

	BOOL m_bAssociateGrass;//是否关联草
	BYTE m_byGrassTexIndex[8];
	BYTE m_byPatternIndex[8];

	void ComputeUVTransformation();

	KG3DTerrainDetailTexture(void);
	~KG3DTerrainDetailTexture(void);

	HRESULT SaveToInformationBuffer(KG3DMemoryFile* pFile);
	HRESULT LoadFromInformationBuffer(KG3DMemoryFile* pFile);

	inline D3DXVECTOR4 GetMtlFactor()
	{
		D3DXVECTOR4 out;
		out.x = m_fEnvReflection;
		out.y = m_fSpecular;
		out.z = m_fEmissive;
		out.w = 0;
		return out;
	}
};

class KG3DTerrainDetailMaterial
{
public:
	vector<BYTE>m_vecTextures;

	HRESULT SaveToInformationBuffer(KG3DMemoryFile* pFile);
	HRESULT LoadFromInformationBuffer(KG3DMemoryFile* pFile);

};

class KG3DTerrainDetailMaterialMgr : public IEKG3DDetailMaterialMgr
{
public:
	UINT m_uHandle;
	KG3DSceneOutDoorSpaceMgr* m_lpOutDoorMgr;
protected:
	
	BYTE m_byteCurrentTextureIndex;
	BYTE m_byteCurrentMaterialIndex;
public:
	KG3DTerrainDetailTexture* GetDetailTexture(BYTE Index);
	HRESULT AddDetailTexture(KG3DTerrainDetailTexture** ppTex,LPCTSTR pFileName);
	HRESULT DelDetailTexture(BYTE byIndex);
	HRESULT GetAllDetailTexture(map<BYTE,std::string>* pmapDetailTex);
	HRESULT ChangeDetailTexture(BYTE byIndex,LPCTSTR pFileName);
	HRESULT GetDetailTextureCoordIndex(INT nIndex,BYTE& byCoord);
	HRESULT SetDetailTextureCoordIndex(INT nIndex,BYTE byCoord);
	HRESULT GetDetailTextureUV(INT nIndex,FLOAT* pfScaleU,FLOAT* pfScaleV,FLOAT* pfEnv,FLOAT* pfSpecular,FLOAT* pfEmissive);
	HRESULT SetDetailTextureUV(INT nIndex,FLOAT m_fScaleU,FLOAT m_fScaleV,FLOAT   fEnv,FLOAT   fSpecular,FLOAT   fEmissive);
	BYTE GetCurrentTextureIndex();
	BYTE GetCurrentMaterialIndex();
	HRESULT GetDetailTextureGrassData(BYTE byIndex,BOOL* pbGrass,BYTE* pbyGrassTexIndex,BYTE* pbyPatternIndex);
	HRESULT SetDetailTextureGrassData(BYTE byIndex,BOOL* pbGrass,BYTE* pbyGrassTexIndex,BYTE* pbyPatternIndex,BOOL bSave = FALSE);
	map<BYTE,KG3DTerrainDetailTexture*> m_mapDetailTexture;
	//KG3DTerrainDetailTexture* m_szDetailTexture[256];
	KG3DTerrainDetailMaterial* m_szDetailMaterial[256];

	KG3DTerrainDetailMaterialMgr();
	KG3DTerrainDetailMaterialMgr(KG3DSceneOutDoorSpaceMgr* lpOutDoorMgr);
	virtual ~KG3DTerrainDetailMaterialMgr();

	HRESULT Copy(KG3DTerrainDetailMaterialMgr* pSrcMgr);

	HRESULT Init();
	HRESULT UnInit();

	HRESULT SaveToInformationBuffer(DWORD& dwLength,KG3DMemoryFile* pFile);
	HRESULT LoadFromInformationBuffer(DWORD dwLength,KG3DMemoryFile* pFile);

	//////////////////////////////////////////////////////////////////////////

	virtual HRESULT GetMaterial(int nIndex,LPSTR pName,int& nTextureCount);
	virtual HRESULT GetTextureForMaterial(int nMtlIndex,int nIndex,int& nTextID);

	virtual HRESULT SetCurrentPaintMtl(int nIndex);
	virtual HRESULT SetCurrentPaintLevel(int nLevel);

};