#pragma once
#include "kg3dmodel.h"
#include "KG3DTexture.h"
#include "KG3DStructs.h"
#include "KG3DInstanceHighDefGrass.h"

class KG3DInstanceBillboard/* : public KG3DModel*/
{
	struct InstanceData
	{
		D3DXVECTOR3 vec3Pos;
		D3DXVECTOR3 vec3Normal;
		D3DXVECTOR2 vec2UV;
		D3DXVECTOR2 vec2Size;
		float fIntanceIndex;
		float fVertexIndex;
		DWORD dwColor;
	};

public:
	DWORD m_dwLastTime;
	PVOID m_pUser;

	struct BillboardInfo
	{
		D3DXVECTOR3 m_vec3Pos;
		D3DXVECTOR3 m_vec3Normal;
		D3DXVECTOR2 m_vec2Size;
		DWORD       m_dwColor;
		DWORD       m_TextureIndex;
	};


	KG3DInstanceBillboard(void);
	~KG3DInstanceBillboard(void);

	virtual HRESULT Render(unsigned uOption, void* pExtInfo,KG3DTexture* pTexture);
	virtual HRESULT FrameMove(DWORD m_dwStrideX,DWORD m_dwStrideY);
	virtual HRESULT Init();
	virtual HRESULT UnInit();
	void ClearBillboard();
	
	void AddBillBoard(const BillboardInfo* pInfo, 
		unsigned int uNum);
	
	HRESULT SetTexture(LPCSTR strTexture, 
		DWORD dwStrideX, 
		DWORD dwStrideY);

	HRESULT SetTexture(KG3DTexture* pTexture,
		DWORD dwStrideX,
		DWORD dwStrideY);
    void ReleaseVideoBuffer();

	HRESULT RenderBegin(KG3DTexture* pTexture,KG3DTexture* pTextureCloud);
	HRESULT RenderEnd();
	HRESULT RenderWithoutEffectChange();

private:
	HRESULT UpdateVertexData(DWORD m_dwStrideX,DWORD m_dwStrideY);
	void GetUVFromIndex(DWORD dwTexIndex, 
		DWORD dwVertexIndex,
		D3DXVECTOR2& vec2UV,DWORD m_dwStrideX,DWORD m_dwStrideY);
	
	DWORD        m_dwInstanceBufferSize;
	DWORD        m_dwMaxBatch;
	
	D3DXVECTOR4* m_pVec4BillBoardPos;

	IDirect3DIndexBuffer9*  m_pIndexBuffer;
	IDirect3DVertexBuffer9* m_pInstanceInfo;
	BOOL         m_bDirtyFlag;
	
	std::vector<BillboardInfo> m_BillBoardData;
	std::vector<KG3DTexture*>  m_Textures;
};

class KG3DInstanceBillboardManager
{
public:
	DWORD m_dwNumMaxShareBillboard;
	vector<KG3DInstanceBillboard*>m_vecShareBillboard;

public:
	HRESULT Init()
	{
		for (DWORD i=0;i<m_dwNumMaxShareBillboard;i++)
		{
			KG3DInstanceBillboard* pBillboard = NULL;
			NewOneShareBillboard(&pBillboard);
			m_vecShareBillboard.push_back(pBillboard);
		}
		return S_OK;
	}
	HRESULT UnInit()
	{
		for (size_t i =0;i<m_vecShareBillboard.size();i++)
		{
			KG3DInstanceBillboard* pBillboard = m_vecShareBillboard[i];
			pBillboard->ClearBillboard();
			SAFE_DELETE(pBillboard);
		}
		m_vecShareBillboard.clear();
		return S_OK;
	}

	HRESULT NewOneShareBillboard(KG3DInstanceBillboard** ppShareBillboard)
	{
		KG3DInstanceBillboard* pText = new KG3DInstanceBillboard;
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

	HRESULT GetOneShareBillboard(KG3DInstanceBillboard** ppShareBillboard,PVOID pUser)
	{
		HRESULT hr = S_OK;
		if(m_vecShareBillboard.size()<m_dwNumMaxShareBillboard)
		{
			hr = NewOneShareBillboard(ppShareBillboard);
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
				KG3DInstanceBillboard* pText = m_vecShareBillboard[i];
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

			KG3DInstanceBillboard* pText = m_vecShareBillboard[maxPos];
			KGLOG_PROCESS_ERROR(pText);
			(*ppShareBillboard) = pText;
		}

		(*ppShareBillboard)->m_dwLastTime = timeGetTime();
		(*ppShareBillboard)->m_pUser = pUser;

		return S_OK;
Exit0:
		return E_FAIL;
	}

	KG3DInstanceBillboardManager(void)
	{
		m_dwNumMaxShareBillboard = 360;
	}

	~KG3DInstanceBillboardManager(void)
	{
		UnInit();
	}
};

class KG3DBillboardEffect
{
public:
    KG3DBillboardEffect();
    ~KG3DBillboardEffect();

	HRESULT Init();
	HRESULT UnInit();

	HRESULT OnLostDevice();
	HRESULT OnResetDevice();

    IDirect3DVertexDeclaration9* m_pBillBoardInstance;
    //LPD3DXEFFECT m_pBillBoardShader;
};

extern KG3DBillboardEffect g_BillBoardEffect;
