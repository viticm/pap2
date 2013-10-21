#include "stdafx.h"
#include "KLightning.h"
#include "ParticleSystem.h"
#include<d3dx9math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


extern LPDIRECT3DDEVICE9  g_pd3dDevice;
extern KGraphicsTool      g_cGraphicsTool;
extern D3DCAPS9           g_D3DCaps;
extern HWND               g_hBaseWnd;

extern TCHAR              g_Def_AppDirectory[256];
extern TCHAR              g_Def_ModelDirectory[256];
extern TCHAR              g_Def_TextureDirectory[256];
extern TCHAR              g_Def_ModelTextureDirectory[256];
extern IMusicApplication  *g_pSoundSys;


/***************************************
Class KLightningNode Implementation
***************************************/

KLightningVertex* KLightningNode::GetVertex(KLightningVertex* pLVBuffer, unsigned int index) const
{
	if(!pLVBuffer)
	{
		return 0;
	}
	if(index >= m_NumOfVertices)
	{
		return 0;
	}

	return pLVBuffer + m_VertexOffset + index;
}


/***************************************
Class KLightning Implementation
***************************************/

HRESULT KLightning::LoadMesh(LPSTR pFileName)
{
	//m_Path = pFileName;
	//unsigned int PosOfLastSplash = m_Path.find_last_of('\\');
	//m_Name = m_Path.substr(PosOfLastSplash + 1);
	//m_Path = m_Path.substr(0, PosOfLastSplash + 1);
	FILE* fp = fopen(pFileName, "rb");
	m_Name = pFileName;
	//Common::ROFile rof;
	//rof.SetFilename(pFileName);
	//rof.Open();

	m_TexName.clear();
	char sz[64];
	memset(sz, 0, sizeof(sz));
	//rof.ReadBuffer(sz, sizeof(sz));  // The texture name
	fread(sz, sizeof(sz), 1, fp);
	m_pTexture = 0;
	if(sz[0] != '\0')
	{
		sz[63] = '\0';
		m_TexName = sz;
		D3DXCreateTextureFromFile(g_pd3dDevice, m_TexName.c_str(), &m_pTexture);
	}

	/*rof.ReadBuffer(&m_MaxNumOfNodes, sizeof(m_MaxNumOfNodes));
	rof.ReadBuffer(&m_LengthOfNode, sizeof(m_LengthOfNode));
	rof.ReadBuffer(&m_WidthOfNode, sizeof(m_WidthOfNode));
	rof.ReadBuffer(&m_FrameInterval, sizeof(m_FrameInterval));
	rof.ReadBuffer(&m_Alpha, sizeof(m_Alpha));

	rof.Close();*/

	fread(&m_MaxNumOfNodes, sizeof(m_MaxNumOfNodes), 1, fp);
	fread(&m_LengthOfNode, sizeof(m_LengthOfNode), 1, fp);
	fread(&m_WidthOfNode, sizeof(m_WidthOfNode), 1, fp);
	fread(&m_FrameInterval, sizeof(m_FrameInterval), 1, fp);
	fread(&m_Alpha, sizeof(m_Alpha), 1, fp);

	fclose(fp);

	m_StartPoint.x = 0.f;
	m_StartPoint.y = 0.f;
	m_StartPoint.z = 0.f;
	m_EndPoint.x = 500.f;
	m_EndPoint.y = 0.f;
	m_EndPoint.z = 500.f;

	Init(m_MaxNumOfNodes, m_pTexture, m_FrameInterval, m_Path, m_Name);

	return S_OK;
}

HRESULT KLightning::SaveMesh(FILE* pFile)
{
	//Common::WOFile wof;
	//wof.SetFilename(m_Path + m_Name);
	//wof.Open();
	FILE* fp = fopen(m_Name.c_str(), "wb");

	m_TexName.clear();
	char sz[64];
	unsigned int len = m_TexName.size();
	len = len > 64? 64: len;
	if(len == 0)
	{
		memset(sz, 0, sizeof(sz));
	}
	else
	{
		memcpy(sz, m_TexName.c_str(), len);
	}
	//wof.WriteBuffer(sz, sizeof(sz));  // The texture name

	//wof.WriteBuffer(&m_MaxNumOfNodes, sizeof(m_MaxNumOfNodes));
	//wof.WriteBuffer(&m_LengthOfNode, sizeof(m_LengthOfNode));
	//wof.WriteBuffer(&m_WidthOfNode, sizeof(m_WidthOfNode));
	//wof.WriteBuffer(&m_FrameInterval, sizeof(m_FrameInterval));
	//wof.WriteBuffer(&m_Alpha, sizeof(m_Alpha));

	//wof.Close();
	fwrite(sz, sizeof(sz), 1, fp);  // The texture name

	fwrite(&m_MaxNumOfNodes, sizeof(m_MaxNumOfNodes), 1, fp);
	fwrite(&m_LengthOfNode, sizeof(m_LengthOfNode), 1, fp);
	fwrite(&m_WidthOfNode, sizeof(m_WidthOfNode), 1, fp);
	fwrite(&m_FrameInterval, sizeof(m_FrameInterval), 1, fp);
	fwrite(&m_Alpha, sizeof(m_Alpha), 1, fp);

	fclose(fp);

	return S_OK;
}

HRESULT KLightning::FrameMove()
{
	unsigned int CurrentTime = timeGetTime();  // The current time in ms

	if(!m_bCreate || CurrentTime - m_LastTime >= m_FrameInterval)
	{
		m_LastTime = CurrentTime;  // Update the last time

		if(!Create(m_StartPoint, m_EndPoint, m_LengthOfNode, m_WidthOfNode))  // Recreate the lightning
		{
			return S_FALSE;
		}

		m_bCreate = true;
	}

	return S_OK;
}

HRESULT KLightning::Render()
{
	HRESULT hr = 0;

	// Backup the previous render states

	DWORD dwPrevLightEnable = 0;
	g_pd3dDevice->GetRenderState(D3DRS_LIGHTING, &dwPrevLightEnable);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	DWORD dwPrevCullMode = 0;
	g_pd3dDevice->GetRenderState(D3DRS_CULLMODE, &dwPrevCullMode);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	DWORD dwPrevFillMode = 0;
	g_pd3dDevice->GetRenderState(D3DRS_FILLMODE, &dwPrevFillMode);
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	DWORD dwPrevTexFactor = 0;
	g_pd3dDevice->GetRenderState(D3DRS_TEXTUREFACTOR, &dwPrevTexFactor);
	g_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, (0x00000000 | m_Alpha) << 24);

	DWORD dwPrevAlphaBlendEnable = 0;
	g_pd3dDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwPrevAlphaBlendEnable);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	DWORD dwPrevBlendOP = 0;
	g_pd3dDevice->GetRenderState(D3DRS_BLENDOP, &dwPrevBlendOP);
	g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	DWORD dwPrevSrcBlend = 0;
	g_pd3dDevice->GetRenderState(D3DRS_SRCBLEND, &dwPrevSrcBlend);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);

	DWORD dwPrevDestBlend = 0;
	g_pd3dDevice->GetRenderState(D3DRS_DESTBLEND, &dwPrevDestBlend);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	D3DXMATRIX mtxPrevWorld, mtxMine;
	D3DXMatrixIdentity(&mtxMine);
	g_pd3dDevice->GetTransform(D3DTS_WORLD, &mtxPrevWorld);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &mtxMine);

	// Draw

	g_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(KLightningVertex));
	g_pd3dDevice->SetFVF(LIGHTNING_VERTEX_FVF);
	g_pd3dDevice->SetTexture(0, m_pTexture);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	unsigned int NumOfPTs = m_CurrentNumOfNodes * 2;
	if(FAILED(hr = g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NumOfPTs)))
	{
		return hr;
	}

	// Recover all the render states

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, dwPrevLightEnable);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, dwPrevCullMode);
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, dwPrevFillMode);
	g_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, dwPrevTexFactor);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, dwPrevAlphaBlendEnable);
	g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, dwPrevBlendOP);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, dwPrevSrcBlend);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, dwPrevDestBlend);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &mtxPrevWorld);

	return S_OK;
}

bool KLightning::Init(unsigned int MaxNumOfNodes, IDirect3DTexture9* pTexture, unsigned int FrameInterval, const string& sPath, const string& sName)
{
	Destroy();

	m_Path = sPath;
	m_Name = sName;
	wsprintf(m_szMeshFileName, "%s", m_Name.c_str());

	m_MaxNumOfNodes = MaxNumOfNodes;
	m_pNodes = new KLightningNode[m_MaxNumOfNodes];

	unsigned int i = 0;
	for(i = 0; i < m_MaxNumOfNodes; ++i)
	{
		KLightningNode* pLN = m_pNodes + i;
		pLN->SetVertexOffset(i * 2);
		pLN->SetNumOfVertices(4);
	}

	unsigned int NumOfVertices = (m_MaxNumOfNodes + 1) * 2;
	if(FAILED(g_pd3dDevice->CreateVertexBuffer(NumOfVertices * sizeof(KLightningVertex), D3DUSAGE_WRITEONLY, LIGHTNING_VERTEX_FVF, D3DPOOL_MANAGED, &m_pVB, 0)))
	{
		return false;
	}
	m_pTexture = pTexture;

	KLightningVertex* pLVBuffer = 0;
	m_pVB->Lock(0, 0, (void**)&pLVBuffer, 0);
	for(i = 0; i < NumOfVertices;)
	{
		KLightningVertex* pLV = 0;

		pLV = pLVBuffer + i;
		pLV->u = 0;
		pLV->v = 0;

		pLV = pLVBuffer + i + 1;
		pLV->u = 1;
		pLV->v = 0;

		pLV = pLVBuffer + i + 2;
		pLV->u = 0;
		pLV->v = 1;

		pLV = pLVBuffer + i + 3;
		pLV->u = 1;
		pLV->v = 1;

		i += 4;
	}
	m_pVB->Unlock();

	m_LastTime = timeGetTime();
	srand(m_LastTime);
	m_FrameInterval = FrameInterval;

	m_StartPoint.x = 0.f;
	m_StartPoint.y = 0.f;
	m_StartPoint.z = 0.f;
	m_EndPoint.x = 500.f;
	m_EndPoint.y = 0.f;
	m_EndPoint.z = 500.f;

	return true;
}

bool KLightning::Create(const D3DXVECTOR3& StartPoint, const D3DXVECTOR3& EndPoint, float LengthOfNode, float WidthOfNode)
{
	m_StartPoint = StartPoint;
	m_EndPoint = EndPoint;
	m_LengthOfNode = LengthOfNode;
	m_WidthOfNode = WidthOfNode;

	D3DXVECTOR3 DirOfLightning = m_EndPoint - m_StartPoint;  // 闪电总体走向
	float LengthOfLightning = ModelSFX::GetVec3Len(DirOfLightning);  // 闪电总体长度
	D3DXVec3Normalize(&DirOfLightning, &DirOfLightning);

	D3DXVECTOR3 verAxis(0.f, 1.f, 0.f);  // The up axis
	D3DXVECTOR3 horAxis;
	D3DXVec3Cross(&horAxis, &DirOfLightning, &verAxis);  // The horizontal axis
	D3DXVec3Normalize(&horAxis, &horAxis);

	D3DXVECTOR3 Vertex0, Vertex1;  // The first two vertices in the current node; used in the following loop
	Vertex0 = m_StartPoint;
	D3DXVECTOR3 CurrentDir = DirOfLightning;  // The direction of the current node; used in the following loop

	KLightningVertex* pLVBuffer = 0;  // The vertex buffer
	m_pVB->Lock(0, 0, (void**)&pLVBuffer, 0);

	// 先处理头两个顶点
	KLightningNode* pLNFirst = m_pNodes;  //  The 1st node
	KLightningVertex* pLVFirst = pLNFirst->GetVertex(pLVBuffer, 0);  // The 1st vertex
	pLVFirst->x = m_StartPoint.x;
	pLVFirst->y = m_StartPoint.y;
	pLVFirst->z = m_StartPoint.z;
	KLightningVertex* pLVSecond = pLNFirst->GetVertex(pLVBuffer, 1);  // The 2nd vertex
	Vertex1 = Vertex0 + horAxis * m_WidthOfNode;
	pLVSecond->x = Vertex1.x;
	pLVSecond->y = Vertex1.y;
	pLVSecond->z = Vertex1.z;

	float RemainingLengthOfLightning = LengthOfLightning;  // The remaining length
	m_CurrentNumOfNodes = 0;
	KLightningVertex* pLV2 = 0;
	KLightningVertex* pLV3 = 0;
	while(m_CurrentNumOfNodes < m_MaxNumOfNodes)  // If the remaining length is longer than length of a node
	{
		KLightningNode* pLN = m_pNodes + m_CurrentNumOfNodes;  // The current node in legend; 传说中的当前结点

		D3DXVECTOR3 Vertex2 = Vertex0 + CurrentDir * m_LengthOfNode;  // The 2nd vertex in the current node
		D3DXVECTOR3 Vertex3 = Vertex1 + CurrentDir * m_LengthOfNode;  // The 3rd vertex in the current node

		// Then rotate the two vertices
		float Angle = ModelSFX::GetRandom(-45, 45) * PI / 180;  // The rotation angle around the up axis; the range of value is from -PI/2 to PI/2
		if(Angle > 0)  // Stretch the vertex to refine the corner
		{
			Vertex0 = Vertex0 + CurrentDir * m_WidthOfNode;
			KLightningVertex* pLV0 = pLN->GetVertex(pLVBuffer, 0);
			pLV0->x = Vertex0.x;
			pLV0->y = Vertex0.y;
			pLV0->z = Vertex0.z;
		}
		else if(Angle < 0)  // Stretch the vertex to refine the corner
		{
			Vertex1 = Vertex1 + CurrentDir * m_WidthOfNode;
			KLightningVertex* pLV1 = pLN->GetVertex(pLVBuffer, 1);
			pLV1->x = Vertex1.x;
			pLV1->y = Vertex1.y;
			pLV1->z = Vertex1.z;
		}

		D3DXMATRIX mtx;  // The rotation matrix
		//D3DXMATRIX matTransInv;
		//D3DXMATRIX matTrans;
		//D3DXMatrixTranslation(&matTransInv, -Vertex0.x, -Vertex0.y, -Vertex0.z);
		//D3DXMatrixTranslation(&matTrans, Vertex0.x, Vertex0.y, Vertex0.z);
		D3DXMatrixRotationAxis(&mtx, &verAxis, Angle); // The rotation matrix
		//D3DXMatrixMultiply(&mtx, &matTransInv, &mtx);
		//D3DXMatrixMultiply(&mtx, &mtx, &matTrans);

		Vertex2 -= -Vertex0;
		D3DXVECTOR4 vec4Temp;
		D3DXVec3Transform(&vec4Temp, &Vertex2, &mtx);
		Vertex2.x = vec4Temp.x / vec4Temp.w;
		Vertex2.y = vec4Temp.y / vec4Temp.w;
		Vertex2.z = vec4Temp.z / vec4Temp.w;
		Vertex2 += Vertex0;

		Vertex3 -= -Vertex0;
		D3DXVec3Transform(&vec4Temp, &Vertex3, &mtx);
		Vertex3.x = vec4Temp.x / vec4Temp.w;
		Vertex3.y = vec4Temp.y / vec4Temp.w;
		Vertex3.z = vec4Temp.z / vec4Temp.w;
		Vertex3 += Vertex0;

		// Update the vertices
		pLV2 = pLN->GetVertex(pLVBuffer, 2);
		pLV2->x = Vertex2.x;
		pLV2->y = Vertex2.y;
		pLV2->z = Vertex2.z;
		pLV3 = pLN->GetVertex(pLVBuffer, 3);
		pLV3->x = Vertex3.x;
		pLV3->y = Vertex3.y;
		pLV3->z = Vertex3.z;

		// Update some variables for the next loop
		Vertex0 = Vertex2;
		Vertex1 = Vertex3;
		//CurrentDir = CurrentDir * mtx;
		RemainingLengthOfLightning -= m_LengthOfNode;
		++m_CurrentNumOfNodes;
	}
	pLV2->x = m_EndPoint.x;
	pLV2->y = m_EndPoint.y;
	pLV2->z = m_EndPoint.z;
	pLV3->x = m_EndPoint.x;
	pLV3->y = m_EndPoint.y;
	pLV3->z = m_EndPoint.z;

	m_pVB->Unlock();

	return true;
}

void KLightning::Destroy()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pTexture);
	SAFE_DELETE_ARRAY(m_pNodes);

	m_MaxNumOfNodes = 0;
	m_CurrentNumOfNodes = 0;
	m_Alpha = 0;
}

HRESULT KLightning::Acquire()
{
	if(!m_bNeedReAcquire)
	{
		return S_OK;
	}

	PropertyBase::_PropertyPage* pPage = 0;
	int PageIndex = -1;

	PageIndex = this->AddPage("动态闪电编辑面板");

	pPage = this->FindPage(PageIndex);
	if(pPage)
	{
		int index_Group = pPage->AddGroup(PropertyBase::_PropertyGroup(0,"闪电",FALSE));
		PropertyBase::_PropertyGroup* pGroup = pPage->FindGroup(index_Group);

		pGroup->AddPropertyFloat("结点长度",&m_LengthOfNode, 0,100000,1);
		pGroup->AddPropertyFloat("宽度",&m_WidthOfNode,0,100000,1);

		pGroup->AddPropertyInt("每桢时间(毫秒)", (int*)&m_FrameInterval, 0, 5000);
		pGroup->AddPropertyInt("不透明程度", (int*)&m_Alpha, 0, 255);
	}
	m_bNeedReAcquire = FALSE;
	return S_OK;
}