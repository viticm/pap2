#pragma once
#include "kg3dmodel.h"
#include "kg3dtexture.h"

#define RENDER_GRASS_LOD0 0
#define RENDER_GRASS_LOD1 1
#define RENDER_GRASS_LOD2 2

class KSceneSceneEditorDialogGrass;
class KG3DInstanceBillboard;

class KG3DInstanceHighDefGrass/* : public KG3DModel*/
{
	friend class KSceneSceneEditorDialogGrass;
	friend class KG3DInstanceBillboard;
	
	struct HighDefGrassVertex
	{
		D3DXVECTOR3 vec3Pos;
		D3DXVECTOR3 vec3Normal;
		DWORD       dwColor;
		D3DXVECTOR2 vec2UV;
		D3DXVECTOR2 vec2UVSrc;
		//float       fInstanceIndex;
		const static DWORD dwFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2;
	};
	struct RenderInfo
	{
		IDirect3DVertexBuffer9* pVB;
		IDirect3DIndexBuffer9*  pIB;
		DWORD                   dwVBSize;
		DWORD                   dwIBSize;
		DWORD                   dwUsedVBSize;
		DWORD                   dwUsedIBSize;
		RenderInfo();
		~RenderInfo();
	};


	struct GrassCell
	{
		std::vector<int> vecIndices;
		int nCellIndex;
		DWORD dwLODLevel;
		short* pIndices;
		BOOL bDirtyFlag;
		AABBOX BBox;
		GrassCell();
		~GrassCell();
	};

	
	
public:
	HRESULT RenderEnd();
	HRESULT RenderBegin(KG3DTexture* pTexture,KG3DTexture* pTextureCloud);

	static HRESULT InitStaticResource();
	static HRESULT UnInitStaticResource();

	static LPDIRECT3DVERTEXDECLARATION9    m_lpVertexDeclaration;
	//////////////////////////////////////////////////////////////////////////

	DWORD m_dwLastTime;
	PVOID m_pUser;

	KG3DInstanceHighDefGrass(void);
	~KG3DInstanceHighDefGrass(void);
	virtual HRESULT Init();
	virtual HRESULT Render(unsigned uOption, void* pExtInfo,KG3DTexture* pTexture);
	virtual	HRESULT RenderStone(KG3DTexture* pTexture);
	virtual HRESULT RenderWithoutStateChange();

    HRESULT SetTexture(const TCHAR* strFileName, 
		DWORD dwStrideX, 
		DWORD dwStrideY);

	HRESULT SetTexture(KG3DTexture* pTexture,
		DWORD dwStrideX,
		DWORD dwStrideY);

		
	void SetPatchRenderID(DWORD dwID);
	DWORD GetPatchRenderID();

	/*HRESULT LoadPattern(const TCHAR* strModelName, 
		size_t uIndex);*/

	HRESULT ClearGrass();

	//DWORD GetPatternIndexSize(DWORD dwIndex);
	BOOL IsAutoRandomRotated();
	void SetAutoRandomRotated(BOOL bRotated);
	//void ClearPattern();

	HRESULT GetPatternBBox(BBox& Box, unsigned int uIndex,vector<int>&m_Patterns);
	int GetPatternIndex(int nPatternIndexInContainer,vector<int>&m_Patterns);
protected:

	/*void GetUVFromIndex(DWORD dwTexIndex,
		DWORD dwVertexIndex,
		D3DXVECTOR2& vec2UV,DWORD m_dwStrideX,DWORD m_dwStrideY);*/

	void GetUVFromIndex(DWORD dwTexIndex,
		D3DXVECTOR2& vec2UV,DWORD m_dwStrideX,DWORD m_dwStrideY);
	
	D3DXVECTOR3 m_vec3ViewPoint;
	BOOL m_bDirtyData;
	DWORD m_dwBufferSize;
	DWORD m_dwPathchRenderID;
	DWORD m_dwNumGrassToRender;

	IDirect3DVertexBuffer9* m_pVB;
	IDirect3DIndexBuffer9* m_pIB;
	std::vector<GrassCell> m_GrassCells;
    void ReleaseVideoBuffer();
private:
	//std::vector<int>   m_Patterns;
	RenderInfo         m_RenderInfo;
	BOOL               m_bAutoRotated;
};

class KG3DInstanceHighDefGrassManager
{
public:
	DWORD m_dwNumMaxShareBillboard;
	vector<KG3DInstanceHighDefGrass*>m_vecShareBillboard;

public:
	HRESULT Init()
	{
		for (DWORD i=0;i<m_dwNumMaxShareBillboard;i++)
		{
			KG3DInstanceHighDefGrass* pGrass = NULL;
			NewOneShareInstanceHighDefGrass(&pGrass);
			m_vecShareBillboard.push_back(pGrass);
		}
		return S_OK;
	}
	HRESULT UnInit()
	{
		for (size_t i =0;i<m_vecShareBillboard.size();i++)
		{
			KG3DInstanceHighDefGrass* pBillboard = m_vecShareBillboard[i];
			pBillboard->ClearGrass();
			SAFE_DELETE(pBillboard);
		}
		m_vecShareBillboard.clear();
		return S_OK;
	}

	HRESULT NewOneShareInstanceHighDefGrass(KG3DInstanceHighDefGrass** ppShareBillboard)
	{
		KG3DInstanceHighDefGrass* pText = new KG3DInstanceHighDefGrass;
		KGLOG_PROCESS_ERROR(pText);
		//ZeroMemory(pText,sizeof(KG3DInstanceBillboard));

		/*hr  = g_pd3dDevice->CreateTexture(64,64,4,0,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&pText->pTexture,NULL);
		KGLOG_COM_PROCESS_ERROR(hr);*/

		(*ppShareBillboard) = pText;
		return S_OK;
Exit0:
		SAFE_DELETE(pText);

		(*ppShareBillboard) = NULL;
		return E_FAIL;
	}

	HRESULT GetOneShareInstanceHighDefGrass(KG3DInstanceHighDefGrass** ppShareBillboard,PVOID pUser)
	{
		HRESULT hr = S_OK;
		if(m_vecShareBillboard.size()<m_dwNumMaxShareBillboard)
		{
			hr = NewOneShareInstanceHighDefGrass(ppShareBillboard);
			KGLOG_COM_PROCESS_ERROR(hr);
			m_vecShareBillboard.push_back(*ppShareBillboard);
		}
		else
		{
			DWORD dwNowTime = timeGetTime();
			DWORD maxTime = 0;
			DWORD maxPos = 0;
			for (size_t i =0;i<m_vecShareBillboard.size();i++)
			{
				KG3DInstanceHighDefGrass* pText = m_vecShareBillboard[i];
				if(pText)
				{
					DWORD T = dwNowTime - pText->m_dwLastTime;
					if(T>maxTime)
					{
						maxTime = T;
						maxPos = (DWORD)i;
					}
				}
			}

			KG3DInstanceHighDefGrass* pText = m_vecShareBillboard[maxPos];
			KGLOG_PROCESS_ERROR(pText);
			(*ppShareBillboard) = pText;
		}

		(*ppShareBillboard)->m_dwLastTime = timeGetTime();
		(*ppShareBillboard)->m_pUser = pUser;

		return S_OK;
Exit0:
		return E_FAIL;
	}

	KG3DInstanceHighDefGrassManager(void)
	{
		m_dwNumMaxShareBillboard = 360;
	}

	~KG3DInstanceHighDefGrassManager(void)
	{
		UnInit();
	}
};