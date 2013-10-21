#include "stdafx.h"
#include "KG3DHOcclusion.h"
#include "KG3DCamera.h"
#include "KG3DGraphicsTool.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern KG3DGraphicsTool		g_cGraphicsTool;
extern bool					g_bIsUseMinCorrect;
	
KG3DHOcclusion::KG3DHOcclusion(const int W,const int H)
{
	m_nWidth = W;
	m_nHeight = H;

	m_lpHerizonVB = NULL;
	m_pfHeightBuffer = new float[m_nWidth];
	assert(m_pfHeightBuffer);
}

KG3DHOcclusion::~KG3DHOcclusion()
{
	SAFE_DELETE_ARRAY(m_pfHeightBuffer);
	SAFE_RELEASE(m_lpHerizonVB);
}

void KG3DHOcclusion::ReSet(const int &W,const int &H)
{
	m_nWidth = W;
	m_nHeight = H;

	SAFE_DELETE_ARRAY(m_pfHeightBuffer);
	m_pfHeightBuffer = new float[W * H];

	SAFE_RELEASE(m_lpHerizonVB);

	if(FAILED(g_pd3dDevice->CreateVertexBuffer(m_nWidth * sizeof(VFormat::_Faces_Diffuse_Texture0),
		0, D3DFVF_Faces_Diffuse_Texture0,
		D3DPOOL_MANAGED, &m_lpHerizonVB, NULL)))
	{
		return ;  
	}
}

HRESULT KG3DHOcclusion::Init(const int &W,const int &H)
{
	m_nWidth = W;
	m_nHeight = H;

	m_pfHeightBuffer = new float[W * H];

	if(FAILED(g_pd3dDevice->CreateVertexBuffer(m_nWidth * sizeof(VFormat::_Faces_Diffuse_Texture0),
		0, D3DFVF_Faces_Diffuse_Texture0,
		D3DPOOL_MANAGED, &m_lpHerizonVB, NULL)))
	{
		return S_FALSE;  
	}

	return S_OK;
}

// true == below false == top
bool KG3DHOcclusion::Clip_Test_OBBLine(D3DXVECTOR4 a,D3DXVECTOR4 b,D3DXVECTOR2 &outa,D3DXVECTOR2 &outb)
{
	bool IsDeop;

	IsDeop = ClipOBBLine(a,b);

	if(!IsDeop)
	{	
		D3DVIEWPORT9  ViewPort;
		float aSX,aSY,bSX,bSY,dis;
		float aw = 1.0f / a.w;
		float bw = 1.0f / b.w;

		g_pd3dDevice->GetViewport(&ViewPort);

		aSX = (a.x * aw * 0.5f + 0.5f) * ViewPort.Width;
		aSY = (a.y * aw * 0.5f + 0.5f) * ViewPort.Height;

		bSX = (b.x * bw * 0.5f + 0.5f) * ViewPort.Width;
		bSY = (b.y * bw * 0.5f + 0.5f) * ViewPort.Height;

		outa.x = aSX;
		outa.y = aSY;
		outb.x = bSX;
		outb.y = bSY;

		if(aSX > bSX)
		{
			swap(aSX,bSX);
			swap(aSY,bSY);
		}

		if(a.z < b.z)
			dis = a.z;
		else
			dis = b.z;

		bool bIsBelow = IsBelow(aSX,aSY,bSX,bSY/*,Node*/);


		if(bIsBelow)
			return true;

		return false;
	}
	else
	{
		return true;
	}
} 


bool KG3DHOcclusion::ClipOBBLine(D3DXVECTOR4 &a,D3DXVECTOR4 &b)
{
	byte ba,bb;
	KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
	if(NULL == pCamera)
		return false;
	const float n = pCamera->GetPerspectiveData().fzNear;

	if(a.z < n)
		ba = 0;
	else
		ba = 1;

	if(b.z < n)
		bb = 0;
	else
		bb = 1;

	if((ba | bb) == 0)
		return true;

	if(ba & bb)
		return false;

	const float dx = b.x - a.x;
	const float dy = b.y - a.y;
	const float dz = b.z - a.z;
	const float dw = b.w - a.w;

	if (!ba)
	{
		const float t = (- a.z + n) / dz;

		a.x = a.x + dx * t;
		a.y = a.y + dy * t;
		a.z = n;//a.z + dz * t;
		a.w = a.w + dw * t;
	}
	else if (!bb)
	{
		const float t = (- b.z + n) / dz;

		b.x = a.x + dx * t;
		b.y = a.y + dy * t;
		b.z = n;//a.z + dz * t;
		b.w = a.z + dz * t;
	}

	return false;
}

bool KG3DHOcclusion::IsBelow(float &aSX,float &aSY,float &bSX,float &bSY)
{
	if(bSX < 0.0f || aSX > m_nWidth * 1.0f)
		return true;
	const float dx = bSX - aSX;
	const float dy = bSY - aSY;
	const float dydx = dy / dx;
	float h;
	D3DXMATRIX matView;
	/*g_lpD3DDevice->GetTransform(D3DTS_VIEW,&matView);
	dis = Node.Center.x * matView._13 + Node.Center.y * matView._23 + Node.Center.z * matView._33 + matView._43;*/

	//assert(Dis > 0.0f);
	/*if(Dis < 0.0f)
	{
	h = 0.0f;
	}
	correct = (REGION_OBB_ERROR - Node.fDy) / Dis;*/

	// clip left
	if(aSX < 0.0f)
	{
		aSY += -aSX * dydx;
		aSX = 0.0f;
	}

	const float right = m_nWidth * 1.0f - 1.0f;


	h = bSY;
	// clip right
	if(bSX > right)
	{
		bSY += (right - bSX) * dy;
       
		//	bSY -= sqrt(dy * dy / dx / dx * (bSX - right + 1) * (bSX - right));
		bSX = right;
	}

	if(bSY > m_nHeight * 1.0f || aSY > m_nHeight * 1.0f)
	{
		int d = 0;
		d++;
	}

	int i;
	int xstart = (int)aSX;
	int xend = (int)bSX;

	if(dx < 0.0f)
		xstart = xend;
	//	if(abs(dy) < EPSILON)

	float yh = aSY;
	for(i = xstart;i <= xend; ++i)
	{
		//	assert(i >= 0);
		//	assert(i < m_nWidth);
		if(i < 0)
		{
			i = m_nWidth;
		}
		h = m_pfHeightBuffer[i];

		if(yh > h)
			return false;

		yh += dydx;
	}

	return true;
}

//void KG3DHOcclusion::Insert(D3DXVECTOR4 a,D3DXVECTOR4 b,const OBBNode &refNode)
//{
//	bool IsDeop; 
//
//	IsDeop = ClipOBBLine(a,b);
//
//	if(IsDeop)
//		return;
//
//	const float dis = a.w > b.w ? b.w : a.w;
//
//	/*if(a.z < b.z)
//	dis = a.z;
//	else
//	dis = b.z;*/
//
//	D3DVIEWPORT9  ViewPort;
//	float aSX,aSY,bSX,bSY;
//	float aw = 1.0f / a.w;
//	float bw = 1.0f / b.w;
//	float correct ;
//
//	g_pd3dDevice->GetViewport(&ViewPort);
//
//	aSX = (a.x * aw * 0.5f + 0.5f) * ViewPort.Width;
//	aSY = (a.y * aw * 0.5f + 0.5f) * ViewPort.Height;
//
//	bSX = (b.x * bw * 0.5f + 0.5f) * ViewPort.Width;
//	bSY = (b.y * bw * 0.5f + 0.5f) * ViewPort.Height;
//
//	if(aSX > bSX)
//	{
//		swap(aSX,bSX);
//		swap(aSY,bSY);
//	}
//
//	KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
//	const float np = pCamera->GetNearDis();
//
//	if(g_bIsUseMinCorrect)
//		correct = /*REGION_OBB_ERROR * */refNode.fDy * np / dis;
//	else
//		correct = /*REGION_OBB_ERROR **/ REGION_OBB_ERROR * np / dis;
//
///////////////////////////////////////////////////////////////////////
//
//	D3DXMATRIX matView,matProj,m;
//	D3DXVECTOR4 p1,p2;
//	float h1,h2;
//
//	g_pd3dDevice->GetTransform(D3DTS_VIEW,&matView);
//	g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&matProj);
//
//	m = matView * matProj;
//
//	D3DXVec3Transform(&p1,&(refNode.CorrectPos[0]),&m);
//	D3DXVec3Transform(&p2,&(refNode.CorrectPos[1]),&m);
//
//	h1 = 0.5f + p1.y / p1.w * 0.5f;
//	h2 = 0.5f + p2.y / p2.w * 0.5f;
//
//	h2 = h2 > 1.0f ? 1.0f : h2;
//	h1 = h1 < 0.0f ? 0.0f : h1;
//
//	correct = abs(h2 - h1) * ViewPort.Height; 
//
//
///////////////////////////////////////////////////////////
//
//	int xstart = (int)aSX;
//	int xend = (int)bSX;
//	int i;
//	float h;
//
//	if(xend < 0 || xstart >= m_nWidth)
//		return ;
//
//	if(xend >= m_nWidth)
//		xend = m_nWidth - 1;
//
//	const float dydx = (bSY - aSY) / (bSX - aSX);
//	const float dy = bSY - aSY;
//
//	if(aSX < 0.0f)
//	{
//		aSY += -aSX * dydx;
//		aSX = 0.0f;
//	}
//
//	if(bSX > m_nWidth - 1)
//	{
//		bSY -= (m_nWidth - 1 - bSX) * dy;
//		bSX = (m_nWidth - 1) * 1.0f;
//	}
//
//
//	float yh = aSY;
//
//	for(i = xstart;i <= xend; ++i)
//	{
//		//	assert(i >= 0);
//		assert(i < m_nWidth);
//		if(i < 0)
//		{
//			i = m_nWidth;
//		}
//
//		h = m_pfHeightBuffer[i];
//
//		if(yh > h)
//			m_pfHeightBuffer[i] = yh - correct;
//
//		yh += dydx;
//	}
//}

void KG3DHOcclusion::Insert(D3DXVECTOR2 &aS,D3DXVECTOR2 &bS)
{
	int xstart = (int)aS.x;
	int xend = (int)bS.x;
	int i;
	float h;

	if(xend < 0 || xstart >= m_nWidth)
		return ;

	if(xend >= m_nWidth)
		xend = m_nWidth - 1;

	const float dydx = (bS.y - aS.y) / (bS.x - aS.x);
	const float dy = bS.y - aS.y;

	
	if(aS.x < 0.0f)
	{
		aS.y += -aS.x * dydx;
		aS.x = 0.0f;
	}

	
	if(bS.x > m_nWidth - 1)
	{
		bS.y -= (m_nWidth - 1 - bS.x) * dy;
		bS.x = (m_nWidth - 1) * 1.0f;
	}


	float yh = aS.y;
	for(i = xstart;i <= xend; ++i)
	{
		//	assert(i >= 0);
		assert(i < m_nWidth);
		if(i < 0)
		{
			i = m_nWidth;
		}

		h = m_pfHeightBuffer[i];

		if(yh > h)
			m_pfHeightBuffer[i] = yh;

		yh += dydx;
	}
}

void KG3DHOcclusion::Insert(float &aSX,float &aSY,float &bSX,float &bSY)
{
	int xstart = (int)aSX;
	int xend = (int)bSX;
	int i;
	const float dydx = (bSY - aSY) / (bSX - aSX);

	float yh = aSY;
	for(i = xstart;i <= xend; ++i)
	{
		assert(i >= 0);
		//	assert(i < m_nWidth);
		if(i > m_nWidth)
		{
			i = m_nWidth;
		}

		if(yh > m_pfHeightBuffer[i])
			m_pfHeightBuffer[i] = yh;

		yh += dydx;
	}
}

void KG3DHOcclusion::DrawHerizon()
{
	int i,ih;
	float h;
	//init
	VFormat::_Faces_Diffuse_Texture0 *pv;
	m_lpHerizonVB->Lock(0,m_nWidth * sizeof(VFormat::_Faces_Diffuse_Texture0),(void **)&pv,0);

	for(i = 0;i < m_nWidth; ++i)
	{
		pv[i].diffuse = 0xffff00ff;
		pv[i].p = D3DXVECTOR3(i - m_nWidth / 2.0f,m_pfHeightBuffer[i] - m_nHeight / 2.0f,0.0f);

		h = m_pfHeightBuffer[i];

		if(h > m_nHeight * 1.0f )
		{
			ih = (int)h; 
			h++;
		}
	}

	m_lpHerizonVB->Unlock();

	D3DXMATRIX matSaveView,matSaveProj,matView,matProj,m;

	g_pd3dDevice->GetTransform(D3DTS_VIEW,&matSaveView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&matSaveProj);


	D3DXMatrixIdentity(&matView);
	D3DXMatrixOrthoLH(&matProj,(float)m_nWidth,(float)m_nHeight,0.0f,1.0f);

	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,false);
	g_pd3dDevice->SetTexture(0,NULL);
	g_pd3dDevice->SetTransform(D3DTS_VIEW,&matView);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION,&matProj);

	g_pd3dDevice->SetStreamSource(0, m_lpHerizonVB, 0, sizeof(VFormat::_Faces_Diffuse_Texture0));  
	g_pd3dDevice->SetFVF(D3DFVF_Faces_Diffuse_Texture0);
	g_pd3dDevice->DrawPrimitive(D3DPT_POINTLIST,0,m_nWidth);

	g_pd3dDevice->SetTransform(D3DTS_VIEW,&matSaveView);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION,&matSaveProj);
}