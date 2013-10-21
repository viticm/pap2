#include "stdafx.h"
#include "plane.h"
#include "KVolumeFog.h"

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


/*******************************
class KFogPlane implementation
*******************************/

KFogPlane& KFogPlane::operator = (const KFogPlane& fp)
{
	if(&fp != this)
	{
		m_VertexOffset = fp.m_VertexOffset;
		m_NumOfVertices = fp.m_NumOfVertices;
	}

	return *this;
}

KFogVertex* KFogPlane::GetVertex(KFogVertex* pFVBuffer, unsigned index) const
{
	if(!pFVBuffer)
	{
		return 0;
	}
	if(index >= m_NumOfVertices)  // Beyond our field
	{
		return 0;
	}

	return pFVBuffer + m_VertexOffset + index;
}

HRESULT KFogPlane::Render()
{
	return g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, m_VertexOffset, m_NumOfVertices - 2);
}


/*******************************
class KVolumeFog implementation
*******************************/

HRESULT KVolumeFog::LoadMesh(LPSTR pFileName)
{
	HRESULT hr;

	m_FilePath = pFileName;
	//TCHAR strDrive[MAX_PATH];
	//TCHAR strPath[MAX_PATH];
	//TCHAR strFileName[MAX_PATH];
	//TCHAR strExt[MAX_PATH];

	//_splitpath(pFile, strDrive, strPath, strFileName, strExt);

	//unsigned int PosOfLastSplash = m_FilePath.find_last_of('\\');
	//m_Filename = m_FilePath.substr(PosOfLastSplash + 1);
	//m_FilePath = m_FilePath.substr(0, PosOfLastSplash + 1);
	//wsprintf(m_szMeshFileName, "%s", pFileName);

	FILE* fp = fopen(pFileName, "rb");

	//Common::ROFile rof;
	//rof.SetFilename(pFileName);
	//rof.Open();

	// Jump over the file header
	// rof.ReadBuffer(...)  // rof.SetPosition(...)

	// The vertices
	fread(&m_MaxNumOfVertices, sizeof(m_MaxNumOfVertices), 1, fp);
	fread(&m_BaseNumOfVertices, sizeof(m_BaseNumOfVertices), 1, fp);
	//rof.ReadBuffer(&m_MaxNumOfVertices, sizeof(m_MaxNumOfVertices));
	//rof.ReadBuffer(&m_BaseNumOfVertices, sizeof(m_BaseNumOfVertices));
	if(FAILED(hr = g_pd3dDevice->CreateVertexBuffer(m_BaseNumOfVertices * sizeof(KFogVertex), 0, FOG_VERTEX_FVF, D3DPOOL_MANAGED, &m_pVB, 0)))
	{
		return hr;
	}
	if(FAILED(hr = g_pd3dDevice->CreateVertexBuffer(m_MaxNumOfVertices * sizeof(KFogVertex), 0, FOG_VERTEX_FVF, D3DPOOL_MANAGED, &m_pClippedVB, 0)))
	{
		return hr;
	}
	void* pVBBuffer = 0, *pClippedVBBuffer = 0;
	m_pVB->Lock(0, 0, &pVBBuffer, 0);
	m_pClippedVB->Lock(0, 0, &pClippedVBBuffer, 0);
	//rof.ReadBuffer(pVBBuffer, m_BaseNumOfVertices * sizeof(KFogVertex));
	fread(pVBBuffer, m_BaseNumOfVertices * sizeof(KFogVertex), 1, fp);
	memcpy(pClippedVBBuffer, pVBBuffer, m_BaseNumOfVertices * sizeof(KFogVertex));

	// The FogPlanes
	unsigned int NumOfFogPlanes = 0;
	//rof.ReadBuffer(&NumOfFogPlanes, sizeof(NumOfFogPlanes));
	fread(&NumOfFogPlanes, sizeof(NumOfFogPlanes), 1, fp);
	unsigned int i = 0;
	for(i = 0; i < NumOfFogPlanes; ++i)
	{
		KFogPlane* pFP = new KFogPlane;
		unsigned int VertexOffset = 0;
		//rof.ReadBuffer(&VertexOffset, sizeof(VertexOffset));
		fread(&VertexOffset, sizeof(VertexOffset), 1, fp);
		pFP->SetVertexOffset(VertexOffset);
		unsigned int NumOfVertices = 0;
		//rof.ReadBuffer(&NumOfVertices, sizeof(NumOfVertices));
		fread(&NumOfVertices, sizeof(NumOfVertices),1, fp);
		pFP->SetNumOfVertices(NumOfVertices);
		m_FogPlanes.push_back(pFP);

		KFogPlane* pClippedFP = new KFogPlane(*pFP);
		m_ClippedFogPlanes.push_back(pClippedFP);
	}

	CalBBox((KFogVertex*)pVBBuffer);

	m_pVB->Unlock();
	m_pClippedVB->Unlock();
	m_ClippedNumOfVertices = m_BaseNumOfVertices;

	// The TopPlane
	/*rof.ReadBuffer(&m_TopPlane.m_vNormal.x, sizeof(m_TopPlane.m_vNormal.x));
	rof.ReadBuffer(&m_TopPlane.m_vNormal.y, sizeof(m_TopPlane.m_vNormal.y));
	rof.ReadBuffer(&m_TopPlane.m_vNormal.z, sizeof(m_TopPlane.m_vNormal.z));
	rof.ReadBuffer(&m_TopPlane.m_fDistanceToOrigin, sizeof(m_TopPlane.m_fDistanceToOrigin));*/
	fread(&m_TopPlane.m_vNormal.x, sizeof(m_TopPlane.m_vNormal.x), 1, fp);
	fread(&m_TopPlane.m_vNormal.y, sizeof(m_TopPlane.m_vNormal.y), 1, fp);
	fread(&m_TopPlane.m_vNormal.z, sizeof(m_TopPlane.m_vNormal.z), 1, fp);
	fread(&m_TopPlane.m_fDistanceToOrigin, sizeof(m_TopPlane.m_fDistanceToOrigin), 1, fp);

	// The LeftPlane
	/*rof.ReadBuffer(&m_LeftPlane.m_vNormal.x, sizeof(m_LeftPlane.m_vNormal.x));
	rof.ReadBuffer(&m_LeftPlane.m_vNormal.y, sizeof(m_LeftPlane.m_vNormal.y));
	rof.ReadBuffer(&m_LeftPlane.m_vNormal.z, sizeof(m_LeftPlane.m_vNormal.z));
	rof.ReadBuffer(&m_LeftPlane.m_fDistanceToOrigin, sizeof(m_LeftPlane.m_fDistanceToOrigin));*/
	fread(&m_LeftPlane.m_vNormal.x, sizeof(m_LeftPlane.m_vNormal.x), 1, fp);
	fread(&m_LeftPlane.m_vNormal.y, sizeof(m_LeftPlane.m_vNormal.y), 1, fp);
	fread(&m_LeftPlane.m_vNormal.z, sizeof(m_LeftPlane.m_vNormal.z), 1, fp);
	fread(&m_LeftPlane.m_fDistanceToOrigin, sizeof(m_LeftPlane.m_fDistanceToOrigin), 1, fp);


	// The RightPlane
	/*rof.ReadBuffer(&m_RightPlane.m_vNormal.x, sizeof(m_RightPlane.m_vNormal.x));
	rof.ReadBuffer(&m_RightPlane.m_vNormal.y, sizeof(m_RightPlane.m_vNormal.y));
	rof.ReadBuffer(&m_RightPlane.m_vNormal.z, sizeof(m_RightPlane.m_vNormal.z));
	rof.ReadBuffer(&m_RightPlane.m_fDistanceToOrigin, sizeof(m_RightPlane.m_fDistanceToOrigin));*/
	fread(&m_RightPlane.m_vNormal.x, sizeof(m_RightPlane.m_vNormal.x), 1, fp);
	fread(&m_RightPlane.m_vNormal.y, sizeof(m_RightPlane.m_vNormal.y), 1, fp);
	fread(&m_RightPlane.m_vNormal.z, sizeof(m_RightPlane.m_vNormal.z), 1, fp);
	fread(&m_RightPlane.m_fDistanceToOrigin, sizeof(m_RightPlane.m_fDistanceToOrigin), 1, fp);


	// The ZUpPlane
	/*rof.ReadBuffer(&m_ZUpPlane.m_vNormal.x, sizeof(m_ZUpPlane.m_vNormal.x));
	rof.ReadBuffer(&m_ZUpPlane.m_vNormal.y, sizeof(m_ZUpPlane.m_vNormal.y));
	rof.ReadBuffer(&m_ZUpPlane.m_vNormal.z, sizeof(m_ZUpPlane.m_vNormal.z));
	rof.ReadBuffer(&m_ZUpPlane.m_fDistanceToOrigin, sizeof(m_ZUpPlane.m_fDistanceToOrigin));*/
	fread(&m_ZUpPlane.m_vNormal.x, sizeof(m_ZUpPlane.m_vNormal.x), 1, fp);
	fread(&m_ZUpPlane.m_vNormal.y, sizeof(m_ZUpPlane.m_vNormal.y), 1, fp);
	fread(&m_ZUpPlane.m_vNormal.z, sizeof(m_ZUpPlane.m_vNormal.z), 1, fp);
	fread(&m_ZUpPlane.m_fDistanceToOrigin, sizeof(m_ZUpPlane.m_fDistanceToOrigin), 1, fp);


	// The ZBottomPlane
	/*rof.ReadBuffer(&m_ZBottomPlane.m_vNormal.x, sizeof(m_ZBottomPlane.m_vNormal.x));
	rof.ReadBuffer(&m_ZBottomPlane.m_vNormal.y, sizeof(m_ZBottomPlane.m_vNormal.y));
	rof.ReadBuffer(&m_ZBottomPlane.m_vNormal.z, sizeof(m_ZBottomPlane.m_vNormal.z));
	rof.ReadBuffer(&m_ZBottomPlane.m_fDistanceToOrigin, sizeof(m_ZBottomPlane.m_fDistanceToOrigin));*/
	fread(&m_ZBottomPlane.m_vNormal.x, sizeof(m_ZBottomPlane.m_vNormal.x), 1, fp);
	fread(&m_ZBottomPlane.m_vNormal.y, sizeof(m_ZBottomPlane.m_vNormal.y), 1, fp);
	fread(&m_ZBottomPlane.m_vNormal.z, sizeof(m_ZBottomPlane.m_vNormal.z), 1, fp);
	fread(&m_ZBottomPlane.m_fDistanceToOrigin, sizeof(m_ZBottomPlane.m_fDistanceToOrigin), 1, fp);


	// The TopFogPlaneCenter
	/*rof.ReadBuffer(&m_TopFogPlaneCenter.x, sizeof(m_TopFogPlaneCenter.x));
	rof.ReadBuffer(&m_TopFogPlaneCenter.y, sizeof(m_TopFogPlaneCenter.y));
	rof.ReadBuffer(&m_TopFogPlaneCenter.z, sizeof(m_TopFogPlaneCenter.z));*/
	fread(&m_TopFogPlaneCenter.x, sizeof(m_TopFogPlaneCenter.x), 1, fp);
	fread(&m_TopFogPlaneCenter.y, sizeof(m_TopFogPlaneCenter.y), 1, fp);
	fread(&m_TopFogPlaneCenter.z, sizeof(m_TopFogPlaneCenter.z), 1, fp);


	// The ViewOffset
	/*rof.ReadBuffer(&m_ViewOffset, sizeof(m_ViewOffset));*/
	fread(&m_ViewOffset, sizeof(m_ViewOffset), 1, fp);
	// The color
	fread(&m_Color, sizeof(m_Color), 1, fp);
	// The m_DistBetweenFPs
	fread(&m_DistBetweenFPs, sizeof(m_DistBetweenFPs), 1, fp);

	// Set the parameters that are used for editing

	m_PrevTopFogPlaneCenter = m_CurrTopFogPlaneCenter = m_TopFogPlaneCenter;
	m_HalfXSpan = m_PrevHalfSpanX = m_CurrHalfSpanX = fabs(m_BBox_B.x - m_BBox_A.x) * 0.5f;
	m_HalfZSpan = m_PrevHalfSpanZ = m_CurrHalfSpanZ = fabs(m_BBox_B.z - m_BBox_A.z) * 0.5f;
	m_PrevTopPlaneY = m_CurrTopPlaneY = m_TopPlane.m_fDistanceToOrigin;
	m_PrevNumOfFPs = m_CurrNumOfFPs = NumOfFogPlanes;
	m_PrevColorR = m_CurrColorR = (m_Color >> 16) & 0x000000FF;
	m_PrevColorG = m_CurrColorG = (m_Color >> 8) & 0x000000FF;
	m_PrevColorB = m_CurrColorB = m_Color & 0x000000FF;
	m_PrevColorA = m_CurrColorA = (m_Color >> 24) & 0x000000FF;
	m_PrevViewOffset = m_CurrViewOffset = m_ViewOffset;
	m_PrevDistBetweenFPs = m_CurrDistBetweenFPs = m_DistBetweenFPs;

	fclose(fp);
	return S_OK;
}

HRESULT KVolumeFog::SaveMesh(FILE* pFile)
{
	if(!pFile)
	{
		return S_FALSE;
	}

	// The vertices
	fwrite(&m_MaxNumOfVertices, sizeof(m_MaxNumOfVertices), 1, pFile);
	fwrite(&m_BaseNumOfVertices, sizeof(m_BaseNumOfVertices), 1, pFile);
	void* pVBBuffer = 0;
	m_pVB->Lock(0, 0, &pVBBuffer, D3DLOCK_READONLY);
	fwrite(pVBBuffer, m_BaseNumOfVertices * sizeof(KFogVertex), 1, pFile);
	m_pVB->Unlock();

	// The FogPlanes
	unsigned int NumOfFogPlanes = m_FogPlanes.size();
	fwrite(&NumOfFogPlanes, sizeof(NumOfFogPlanes), 1, pFile);
	unsigned int i = 0;
	for(i = 0; i < NumOfFogPlanes; ++i)
	{
		KFogPlane* pFP = m_FogPlanes[i];
		unsigned int VertexOffset = pFP->GetVertexOffset();
		fwrite(&VertexOffset, sizeof(VertexOffset), 1, pFile);
		unsigned int NumOfVertices = pFP->GetNumOfVertices();
		fwrite(&NumOfVertices, sizeof(NumOfVertices), 1, pFile);
	}

	// The TopPlane
	fwrite(&m_TopPlane.m_vNormal.x, sizeof(m_TopPlane.m_vNormal.x), 1, pFile);
	fwrite(&m_TopPlane.m_vNormal.y, sizeof(m_TopPlane.m_vNormal.y), 1, pFile);
	fwrite(&m_TopPlane.m_vNormal.z, sizeof(m_TopPlane.m_vNormal.z), 1, pFile);
	fwrite(&m_TopPlane.m_fDistanceToOrigin, sizeof(m_TopPlane.m_fDistanceToOrigin), 1, pFile);

	// The LeftPlane
	fwrite(&m_LeftPlane.m_vNormal.x, sizeof(m_LeftPlane.m_vNormal.x), 1, pFile);
	fwrite(&m_LeftPlane.m_vNormal.y, sizeof(m_LeftPlane.m_vNormal.y), 1, pFile);
	fwrite(&m_LeftPlane.m_vNormal.z, sizeof(m_LeftPlane.m_vNormal.z), 1, pFile);
	fwrite(&m_LeftPlane.m_fDistanceToOrigin, sizeof(m_LeftPlane.m_fDistanceToOrigin), 1, pFile);

	// The RightPlane
	fwrite(&m_RightPlane.m_vNormal.x, sizeof(m_RightPlane.m_vNormal.x), 1, pFile);
	fwrite(&m_RightPlane.m_vNormal.y, sizeof(m_RightPlane.m_vNormal.y), 1, pFile);
	fwrite(&m_RightPlane.m_vNormal.z, sizeof(m_RightPlane.m_vNormal.z), 1, pFile);
	fwrite(&m_RightPlane.m_fDistanceToOrigin, sizeof(m_RightPlane.m_fDistanceToOrigin), 1, pFile);

	// The ZUpPlane
	fwrite(&m_ZUpPlane.m_vNormal.x, sizeof(m_ZUpPlane.m_vNormal.x), 1, pFile);
	fwrite(&m_ZUpPlane.m_vNormal.y, sizeof(m_ZUpPlane.m_vNormal.y), 1, pFile);
	fwrite(&m_ZUpPlane.m_vNormal.z, sizeof(m_ZUpPlane.m_vNormal.z), 1, pFile);
	fwrite(&m_ZUpPlane.m_fDistanceToOrigin, sizeof(m_ZUpPlane.m_fDistanceToOrigin), 1, pFile);

	// The ZBottomPlane
	fwrite(&m_ZBottomPlane.m_vNormal.x, sizeof(m_ZBottomPlane.m_vNormal.x), 1, pFile);
	fwrite(&m_ZBottomPlane.m_vNormal.y, sizeof(m_ZBottomPlane.m_vNormal.y), 1, pFile);
	fwrite(&m_ZBottomPlane.m_vNormal.z, sizeof(m_ZBottomPlane.m_vNormal.z), 1, pFile);
	fwrite(&m_ZBottomPlane.m_fDistanceToOrigin, sizeof(m_ZBottomPlane.m_fDistanceToOrigin), 1, pFile);

	// The TopFogPlaneCenter
	fwrite(&m_TopFogPlaneCenter.x, sizeof(m_TopFogPlaneCenter.x), 1, pFile);
	fwrite(&m_TopFogPlaneCenter.y, sizeof(m_TopFogPlaneCenter.y), 1, pFile);
	fwrite(&m_TopFogPlaneCenter.z, sizeof(m_TopFogPlaneCenter.z), 1, pFile);

	// The ViewOffset
	fwrite(&m_ViewOffset, sizeof(m_ViewOffset), 1, pFile);
	// The color
	fwrite(&m_Color, sizeof(m_Color), 1, pFile);
	// The m_DistBetweenFPs
	fwrite(&m_DistBetweenFPs, sizeof(m_DistBetweenFPs), 1, pFile);

	// OK now
	return S_OK;
}

HRESULT KVolumeFog::FrameMove()
{
	m_bInFogVolume = IsCameraInside();

	if(m_bInFogVolume)  // The camer is inside the fog volume, then rotate the fog planes and clip them
	{
		// Prepare

		D3DXVECTOR3 dxvViewpoint, dxvLookDir;
		g_cGraphicsTool.GetCameraPos(&dxvViewpoint);
		g_cGraphicsTool.GetCameraFront(&dxvLookDir);
		D3DXVECTOR3 Viewpoint(dxvViewpoint), LookDir(dxvLookDir), FogPlaneNormal(0.f, 1.f, 0.f);  // The FogPlaneNormal
		D3DXVec3Normalize(&LookDir, &LookDir); // if LookDir is not normalized

		float c = D3DXVec3Dot(&FogPlaneNormal, &LookDir);  // cos
		float Angle = acosf(c);  // the angle;
		Angle = PI - Angle;

		D3DXVECTOR3 Axis;
		D3DXVec3Cross(&Axis, &LookDir, &FogPlaneNormal);  // The rotation axis
		D3DXVec3Normalize(&Axis, &Axis);

		D3DXMATRIX mtx;  // The transform matrix
		D3DXMATRIX mtxTranslation;
		D3DXVECTOR3 vec3Trans = Viewpoint - m_TopFogPlaneCenter;
		D3DXMatrixIdentity(&mtx);
		D3DXMatrixTranslation(&mtxTranslation, vec3Trans.x, vec3Trans.y, vec3Trans.z);
		D3DXMATRIX mtxRotation;  // The rotation matrix
		D3DXMATRIX matTransInv;
		D3DXMATRIX matTrans;
		D3DXMatrixTranslation(&matTransInv, -Viewpoint.x, -Viewpoint.y, -Viewpoint.z);
		D3DXMatrixTranslation(&matTrans, Viewpoint.x, Viewpoint.y, Viewpoint.z);

		D3DXMatrixRotationAxis(&mtxRotation, &Axis, Angle);
		D3DXMatrixMultiply(&mtxRotation, &matTransInv, &mtxRotation);
		D3DXMatrixMultiply(&mtxRotation, &mtxRotation, &matTrans);
		D3DXMatrixMultiply(&mtx, &mtx, &mtxTranslation);
		D3DXMatrixMultiply(&mtx, &mtx, &mtxRotation);

		//mtx = mtx * mtxTranslation * mtxRotation;  // Multiply the transform matrices
		//mtxTranslation.Translate(LookDir * m_ViewOffset);  // Translate the FogPlanes off the Viewpoint
		D3DXMatrixTranslation(&mtxTranslation, LookDir.x * m_ViewOffset, LookDir.y * m_ViewOffset, LookDir.z * m_ViewOffset);
		//mtx = mtx * mtxTranslation;  // OK, this is the final matrix
		D3DXMatrixMultiply(&mtx, &mtx, &mtxTranslation);

		// Transform and clip the vertices
		KFogVertex* pFVBuffer = 0, *pClippedFVBuffer = 0;  // The two buffer
		m_pVB->Lock(0, 0, (void**)&pFVBuffer, D3DLOCK_READONLY);
		m_pClippedVB->Lock(0, 0, (void**)&pClippedFVBuffer, 0);

		unsigned int NumOfFPs = m_FogPlanes.size();
		unsigned int VertexOffset = 0;
		unsigned int fp = 0;
		for(fp = 0; fp < NumOfFPs; ++fp)
		{
			KFogPlane* pFP = m_FogPlanes[fp];
			KFogPlane* pClippedFP = m_ClippedFogPlanes[fp];

			vector<D3DXVECTOR3> vOrigin, vFront, vBack;

			unsigned int NumOfFVs = pFP->GetNumOfVertices();  // The original num of vertices
			unsigned int fv = 0;
			for(fv = 0; fv < NumOfFVs; ++fv)
			{
				KFogVertex* pFV = pFP->GetVertex(pFVBuffer, fv);
				D3DXVECTOR3 vTemp(pFV->x, pFV->y, pFV->z);
				D3DXVECTOR4 vec4Temp;
				D3DXVec3Transform(&vec4Temp, &vTemp, &mtx);
				vTemp.x  = vec4Temp.x / vec4Temp.w;
				vTemp.y  = vec4Temp.y / vec4Temp.w;
				vTemp.z  = vec4Temp.z / vec4Temp.w;
				vOrigin.push_back(vTemp);
			}

			m_TopPlane.Split(vOrigin, vFront, vBack);  // Clip the original mesh into 2 parts

			unsigned int NumOfClippedFVs = vFront.size();
			pClippedFP->SetVertexOffset(VertexOffset);
			pClippedFP->SetNumOfVertices(NumOfClippedFVs);
			for(fv = 0; fv < NumOfClippedFVs; ++fv)
			{
				KFogVertex* pFV = pClippedFP->GetVertex(pClippedFVBuffer, fv);
				pFV->x = vFront[fv].x;
				pFV->y = vFront[fv].y;
				pFV->z = vFront[fv].z;
				pFV->diffuse = m_Color;
			}

			VertexOffset += NumOfClippedFVs;
		}

		m_pVB->Unlock();
		m_pClippedVB->Unlock();
	}

	Update();

	return S_OK;
}

HRESULT KVolumeFog::Render()
{
	HRESULT hr = 0;

	KModel::Render();

	// Backup the previous render states and set new render states

	DWORD dwPrevLightEnable = 0;
	g_pd3dDevice->GetRenderState(D3DRS_LIGHTING, &dwPrevLightEnable);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	DWORD dwPrevCullMode = 0;
	g_pd3dDevice->GetRenderState(D3DRS_CULLMODE, &dwPrevCullMode);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	DWORD dwPrevFillMode = 0;
	g_pd3dDevice->GetRenderState(D3DRS_FILLMODE, &dwPrevFillMode);
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	DWORD dwPrevAlphaBlendEnable = 0;
	g_pd3dDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwPrevAlphaBlendEnable);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	DWORD dwPrevSrcBlend = 0;
	g_pd3dDevice->GetRenderState(D3DRS_SRCBLEND, &dwPrevSrcBlend);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

	DWORD dwPrevDestBlend = 0;
	g_pd3dDevice->GetRenderState(D3DRS_DESTBLEND, &dwPrevDestBlend);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// The materials
	/*D3DMATERIAL9 d3Material;
	g_pd3dDevice->GetMaterial(&d3Material);*/
	IDirect3DBaseTexture9* pPrevTexture = 0;
	g_pd3dDevice->GetTexture(0, &pPrevTexture);
	g_pd3dDevice->SetTexture(0, 0);  // Disable texture
	DWORD dwPrevColorOP = 0;
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_COLOROP, &dwPrevColorOP);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
	DWORD dwPrevAlphaOP = 0;
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwPrevAlphaOP);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);

	D3DXMATRIX PrevMtxWorld;
	g_pd3dDevice->GetTransform(D3DTS_WORLD, &PrevMtxWorld);


	// Render
	//g_pd3dDevice->SetTransform(D3DTS_WORLD, &m_CurMatrix);
	if(m_bInFogVolume)  // The camera is inside the fog volume，FogPlane必须面向照相机，并作裁减
	{
		// VertexBuffer, and FVF
		g_pd3dDevice->SetStreamSource(0, m_pClippedVB, 0, sizeof(KFogVertex));
		g_pd3dDevice->SetFVF(FOG_VERTEX_FVF);

		// Draw
		unsigned int NumOfFogPlanes = m_ClippedFogPlanes.size();
		unsigned int i = 0;
		for(i = 0; i < NumOfFogPlanes; ++i)
		{
			KFogPlane* pFP = m_ClippedFogPlanes[i];
			if(FAILED(hr = pFP->Render()))
			{
				return hr;
			}
		}
	}
	else  // 照相机位于FogVolume以外，FogPlane只需垂直于Y轴即可
	{
		// Set VertexBuffer, and FVF
		g_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(KFogVertex));
		g_pd3dDevice->SetFVF(FOG_VERTEX_FVF);

		// Draw
		unsigned int NumOfFogPlanes = m_FogPlanes.size();
		unsigned int i = 0;
		for(i = 0; i < NumOfFogPlanes; ++i)
		{
			KFogPlane* pFP = m_FogPlanes[i];
			if(FAILED(hr = pFP->Render()))
			{
				return hr;
			}
		}
	}

	// Recover the previous render states
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &PrevMtxWorld);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, dwPrevLightEnable);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, dwPrevCullMode);
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, dwPrevFillMode);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, dwPrevAlphaBlendEnable);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, dwPrevSrcBlend);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, dwPrevDestBlend);

	g_pd3dDevice->SetTexture(0, pPrevTexture);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, dwPrevColorOP);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, dwPrevAlphaOP);

	return S_OK;
}

bool KVolumeFog::Create(const D3DXVECTOR3& TopFogPlaneCenter, float HalfSpanX, float HalfSpanZ, float TopPlaneY, unsigned int NumOfFogPlanes, DWORD dwColor, float ViewOffset, float DistBetweenFPs, const string& sPath, const string& sName)
{
	Destroy();

	// Some parameters
	m_HalfXSpan = HalfSpanX;
	m_HalfZSpan = HalfSpanZ;

	// The 5 bounding planes
	m_TopPlane.m_vNormal.x = 0;
	m_TopPlane.m_vNormal.y = -1;
	m_TopPlane.m_vNormal.z = 0;
	m_TopPlane.m_fDistanceToOrigin = TopPlaneY;

	m_LeftPlane.m_vNormal.x = 1;
	m_LeftPlane.m_vNormal.y = 0;
	m_LeftPlane.m_vNormal.z = 0;
	m_LeftPlane.m_fDistanceToOrigin = -(TopFogPlaneCenter.x - HalfSpanX);

	m_RightPlane.m_vNormal.x = -1;
	m_RightPlane.m_vNormal.y = 0;
	m_RightPlane.m_vNormal.z = 0;
	m_RightPlane.m_fDistanceToOrigin = TopFogPlaneCenter.x + HalfSpanX;

	m_ZUpPlane.m_vNormal.x = 0;
	m_ZUpPlane.m_vNormal.y = 0;
	m_ZUpPlane.m_vNormal.z = -1;
	m_ZUpPlane.m_fDistanceToOrigin = TopFogPlaneCenter.z + HalfSpanZ;

	m_ZBottomPlane.m_vNormal.x = 0;
	m_ZBottomPlane.m_vNormal.y = 0;
	m_ZBottomPlane.m_vNormal.z = 1;
	m_ZBottomPlane.m_fDistanceToOrigin = -(TopFogPlaneCenter.z - HalfSpanZ);

	m_TopFogPlaneCenter = TopFogPlaneCenter;
	m_TopFogPlaneCenter.y = TopPlaneY - ViewOffset;
	m_ViewOffset = ViewOffset;
	m_Color = dwColor;
	m_DistBetweenFPs = DistBetweenFPs;

	unsigned int i = 0;

	// The FogPlanes
	for(i = 0; i < NumOfFogPlanes; ++i)
	{
		KFogPlane* pFP = new KFogPlane;
		pFP->SetVertexOffset(0 + i * 4);
		pFP->SetNumOfVertices(4);
		m_FogPlanes.push_back(pFP);

		m_ClippedFogPlanes.push_back(new KFogPlane(*pFP));
	}

	// The vertices
	m_MaxNumOfVertices = NumOfFogPlanes * 5;
	m_BaseNumOfVertices = NumOfFogPlanes * 4;
	m_ClippedNumOfVertices = m_BaseNumOfVertices;
	if(FAILED(g_pd3dDevice->CreateVertexBuffer(m_BaseNumOfVertices * sizeof(KFogVertex), 0, FOG_VERTEX_FVF, D3DPOOL_MANAGED, &m_pVB, 0)))
	{
		return false;
	}
	if(FAILED(g_pd3dDevice->CreateVertexBuffer(m_MaxNumOfVertices * sizeof(KFogVertex), 0, FOG_VERTEX_FVF, D3DPOOL_MANAGED, &m_pClippedVB, 0)))
	{
		SAFE_RELEASE(m_pVB);
		return false;
	}

	KFogVertex* pFVBuffer = 0, *pClippedFVBuffer = 0;
	m_pVB->Lock(0, 0, (void**)&pFVBuffer, 0);
	m_pClippedVB->Lock(0, 0, (void**)&pClippedFVBuffer, 0);
	for(i = 0; i < NumOfFogPlanes; ++i)
	{
		KFogPlane* pFP = m_FogPlanes[NumOfFogPlanes - i - 1];
		KFogVertex* pFV = 0;

		float FogPlaneY = m_TopFogPlaneCenter.y - i * m_DistBetweenFPs;

		pFV = pFP->GetVertex(pFVBuffer, 0);
		pFV->x = m_TopFogPlaneCenter.x - HalfSpanX;
		pFV->z = m_TopFogPlaneCenter.z + HalfSpanZ;
		pFV->y = FogPlaneY;
		pFV->diffuse = m_Color;
		pFV->u = 0;
		pFV->v = 0;

		pFV = pFP->GetVertex(pFVBuffer, 1);
		pFV->x = m_TopFogPlaneCenter.x + HalfSpanX;
		pFV->z = m_TopFogPlaneCenter.z + HalfSpanZ;
		pFV->y = FogPlaneY;
		pFV->diffuse = m_Color;
		pFV->u = 1;
		pFV->v = 0;

		pFV = pFP->GetVertex(pFVBuffer, 2);
		pFV->x = m_TopFogPlaneCenter.x + HalfSpanX;
		pFV->z = m_TopFogPlaneCenter.z - HalfSpanZ;
		pFV->y = FogPlaneY;
		pFV->diffuse = m_Color;
		pFV->u = 1;
		pFV->v = 1;

		pFV = pFP->GetVertex(pFVBuffer, 3);
		pFV->x = m_TopFogPlaneCenter.x - HalfSpanX;
		pFV->z = m_TopFogPlaneCenter.z - HalfSpanZ;
		pFV->y = FogPlaneY;
		pFV->diffuse = m_Color;
		pFV->u = 0;
		pFV->v = 1;
	}
	memcpy(pClippedFVBuffer, pFVBuffer, m_BaseNumOfVertices * sizeof(KFogVertex));

	CalBBox(pFVBuffer);

	m_pVB->Unlock();
	m_pClippedVB->Unlock();

	m_PrevTopFogPlaneCenter = m_CurrTopFogPlaneCenter = m_TopFogPlaneCenter;
	m_PrevHalfSpanX = m_CurrHalfSpanX = fabs(m_BBox_B.x - m_BBox_A.x) * 0.5f;
	m_PrevHalfSpanZ = m_CurrHalfSpanZ = fabs(m_BBox_B.z - m_BBox_A.z) * 0.5f;
	m_PrevTopPlaneY = m_CurrTopPlaneY = m_TopPlane.m_fDistanceToOrigin;
	m_PrevNumOfFPs = m_CurrNumOfFPs = NumOfFogPlanes;
	m_PrevColorR = m_CurrColorR = (m_Color >> 16) & 0x000000FF;
	m_PrevColorG = m_CurrColorG = (m_Color >> 8) & 0x000000FF;
	m_PrevColorB = m_CurrColorB = m_Color & 0x000000FF;
	m_PrevColorA = m_CurrColorA = (m_Color >> 24) & 0x000000FF;
	m_PrevViewOffset = m_CurrViewOffset = m_ViewOffset;
	m_PrevDistBetweenFPs = m_CurrDistBetweenFPs = m_DistBetweenFPs;


	m_FilePath = sPath;
	m_Filename = sName;
	wsprintf(m_szMeshFileName,"%s",sName.c_str());

	return true;
}

void KVolumeFog::Destroy()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pClippedVB);

	unsigned i = 0;
	unsigned int NumOfFogPlanes = m_FogPlanes.size();
	for(i = 0; i < NumOfFogPlanes; ++i)
	{
		KFogPlane* pFP = m_FogPlanes[i];
		SAFE_DELETE(pFP);
	}
	m_FogPlanes.clear();

	unsigned int NumOfClippedFogPlanes = m_ClippedFogPlanes.size();
	for(i = 0; i < NumOfClippedFogPlanes; ++i)
	{
		KFogPlane* pFP = m_ClippedFogPlanes[i];
		SAFE_DELETE(pFP);
	}
}

bool KVolumeFog::IsCameraInside() const
{
	D3DXVECTOR3 dxvViewpoint;
	g_cGraphicsTool.GetCameraPos(&dxvViewpoint);
	D3DXVECTOR3 Viewpoint(dxvViewpoint);

	RelativeLocation rl;

	rl = m_TopPlane.TestPoint(Viewpoint);
	if(rl != RL_FRONT)
	{
		return false;
	}

	//return true;

	rl = m_LeftPlane.TestPoint(Viewpoint);
	if(rl != RL_FRONT)
	{
		return false;
	}

	rl = m_RightPlane.TestPoint(Viewpoint);
	if(rl != RL_FRONT)
	{
		return false;
	}

	rl = m_ZUpPlane.TestPoint(Viewpoint);
	if(rl != RL_FRONT)
	{
		return false;
	}

	rl = m_ZBottomPlane.TestPoint(Viewpoint);
	if(rl != RL_FRONT)
	{
		return false;
	}

	return true;
}

HRESULT KVolumeFog::Acquire()
{
	if (!m_bNeedReAcquire)
	{
		return S_OK;
	}

	PropertyBase::_PropertyPage* pPage = 0;
	int PageIndex = -1;

	PageIndex = this->AddPage("体积雾编辑面板");

	pPage = this->FindPage(PageIndex);
	if(pPage)
	{
		int index_Group = pPage->AddGroup(PropertyBase::_PropertyGroup(0,"体积雾",FALSE));
		PropertyBase::_PropertyGroup* pGroup = pPage->FindGroup(index_Group);

		pGroup->AddPropertyFloat("最顶层中心点X坐标",&m_CurrTopFogPlaneCenter.x,0,100000,1);
		pGroup->AddPropertyFloat("最顶层中心点Y坐标",&m_CurrTopFogPlaneCenter.y,0,100000,1);
		pGroup->AddPropertyFloat("最顶层中心点Z坐标",&m_CurrTopFogPlaneCenter.z,0,100000,1);

		pGroup->AddPropertyFloat("雾体积的半长",&m_CurrHalfSpanX,1,100000,1);
		pGroup->AddPropertyFloat("雾体积的半宽",&m_CurrHalfSpanZ,1,100000,1);

		pGroup->AddPropertyFloat("顶层包围平面Y坐标",&m_CurrTopPlaneY,0,100000,1);

		//pGroup->AddPropertyInt("雾平面的数量",(int*)&m_CurrNumOfFPs,1,100);
		pGroup->AddPropertyInt("雾的颜色(R分量)", (int*)&m_CurrColorR, 0, 255);
		pGroup->AddPropertyInt("雾的颜色(G分量)", (int*)&m_CurrColorG, 0, 255);
		pGroup->AddPropertyInt("雾的颜色(B分量)", (int*)&m_CurrColorB, 0, 255);
		pGroup->AddPropertyInt("不透明程度(Alpha分量)", (int*)&m_CurrColorA, 0, 255);

		pGroup->AddPropertyFloat("雾视距",&m_CurrViewOffset,0,100000,1);
		pGroup->AddPropertyFloat("雾平面的间距",&m_CurrDistBetweenFPs,1,100000,1);
	}
	m_bNeedReAcquire = FALSE;
	return S_OK;
}

void KVolumeFog::SetTopFogPlaneCenter(const D3DXVECTOR3& tfpc)
{
	D3DXVECTOR3 dv = tfpc - m_TopFogPlaneCenter;
	m_TopFogPlaneCenter = tfpc;

	KFogVertex * pFVBuffer = 0;
	m_pVB->Lock(0, 0, (void**)&pFVBuffer, 0);

	unsigned int i = 0;
	for(i = 0; i < m_BaseNumOfVertices; ++i)
	{
		KFogVertex* pFV = pFVBuffer + i;
		D3DXVECTOR3 vTemp(pFV->x, pFV->y, pFV->z);
		vTemp += dv;
		pFV->x = vTemp.x;
		pFV->y = vTemp.y;
		pFV->z = vTemp.z;
	}

	CalBBox(pFVBuffer);

	m_pVB->Unlock();

	CalBoundingPlanes();
}

void KVolumeFog::SetHalfSpanX(float HalfSpanX)
{
	m_HalfXSpan = HalfSpanX;

	KFogVertex * pFVBuffer = 0;
	m_pVB->Lock(0, 0, (void**)&pFVBuffer, 0);

	unsigned int NumOfFPs = m_FogPlanes.size();
	unsigned int i = 0;
	for(i = 0; i < NumOfFPs; ++i)
	{
		KFogPlane* pFP = m_FogPlanes[i];

		KFogVertex* pFV = pFP->GetVertex(pFVBuffer, 0);
		pFV->x = m_TopFogPlaneCenter.x - HalfSpanX;

		pFV = pFP->GetVertex(pFVBuffer, 1);
		pFV->x = m_TopFogPlaneCenter.x + HalfSpanX;

		pFV = pFP->GetVertex(pFVBuffer, 2);
		pFV->x = m_TopFogPlaneCenter.x + HalfSpanX;

		pFV = pFP->GetVertex(pFVBuffer, 3);
		pFV->x = m_TopFogPlaneCenter.x - HalfSpanX;
	}

	CalBBox(pFVBuffer);

	m_pVB->Unlock();

	CalBoundingPlanes();
}

void KVolumeFog::SetHalfSpanZ(float HalfSpanZ)
{
	m_HalfZSpan = HalfSpanZ;

	KFogVertex * pFVBuffer = 0;
	m_pVB->Lock(0, 0, (void**)&pFVBuffer, 0);

	unsigned int NumOfFPs = m_FogPlanes.size();
	unsigned int i = 0;
	for(i = 0; i < NumOfFPs; ++i)
	{
		KFogPlane* pFP = m_FogPlanes[i];

		KFogVertex* pFV = pFP->GetVertex(pFVBuffer, 0);
		pFV->z = m_TopFogPlaneCenter.z + HalfSpanZ;

		pFV = pFP->GetVertex(pFVBuffer, 1);
		pFV->z = m_TopFogPlaneCenter.z + HalfSpanZ;

		pFV = pFP->GetVertex(pFVBuffer, 2);
		pFV->z = m_TopFogPlaneCenter.z - HalfSpanZ;

		pFV = pFP->GetVertex(pFVBuffer, 3);
		pFV->z = m_TopFogPlaneCenter.z - HalfSpanZ;
	}

	CalBBox(pFVBuffer);

	m_pVB->Unlock();

	CalBoundingPlanes();
}

void KVolumeFog::SetTopPlaneY(float TopPlaneY)
{
	float dy = TopPlaneY - m_TopPlane.m_fDistanceToOrigin;
	m_TopPlane.m_fDistanceToOrigin = TopPlaneY;

	KFogVertex * pFVBuffer = 0;
	m_pVB->Lock(0, 0, (void**)&pFVBuffer, 0);

	unsigned int i = 0;
	for(i = 0; i < m_BaseNumOfVertices; ++i)
	{
		KFogVertex* pFV = pFVBuffer + i;
		pFV->y += dy;
	}

	CalBBox(pFVBuffer);

	m_pVB->Unlock();

	CalBoundingPlanes();
}

void KVolumeFog::SetNumOfFogPlanes(unsigned int NumOfFPs)
{
	return;

	if(NumOfFPs == m_FogPlanes.size())
	{
		return;
	}

	unsigned int NumOfOriginalFPs = m_FogPlanes.size();
	unsigned int i = 0;

	/*if(NumOfFPs < m_FogPlanes.size())
	{
		vector<KFogPlane*>::iterator it = m_FogPlanes.begin();
		for(i = 0; i < NumOfOriginalFPs - NumOfFPs && it != m_FogPlanes.end(); ++i)
		{
			KFogPlane* pFP = m_FogPlanes[i];
			SAFE_DELETE(pFP);
			m_FogPlanes.erase(it++);


		}
	}*/
}

void KVolumeFog::SetDiffuse(unsigned int Diffuse)
{
	m_Color = Diffuse;

	KFogVertex* pFVBuffer = 0;
	m_pVB->Lock(0, 0, (void**)&pFVBuffer, 0);
	unsigned int i = 0;
	for(i = 0; i < m_BaseNumOfVertices; ++i)
	{
		KFogVertex* pFV = pFVBuffer + i;
		pFV->diffuse = m_Color;
	}
	m_pVB->Unlock();
}

void KVolumeFog::SetColorR(unsigned char r)
{
	m_Color &= 0xFF00FFFF;  // Mark off the red channel
	m_Color |= ((DWORD)r) << 16;

	KFogVertex* pFVBuffer = 0;
	m_pVB->Lock(0, 0, (void**)&pFVBuffer, 0);
	unsigned int i = 0;
	for(i = 0; i < m_BaseNumOfVertices; ++i)
	{
		KFogVertex* pFV = pFVBuffer + i;
		pFV->diffuse = m_Color;
	}
	m_pVB->Unlock();
}

void KVolumeFog::SetColorG(unsigned char g)
{
	m_Color &= 0xFFFF00FF;  // Mark off the green channel
	m_Color |= ((DWORD)g) << 8;

	KFogVertex* pFVBuffer = 0;
	m_pVB->Lock(0, 0, (void**)&pFVBuffer, 0);
	unsigned int i = 0;
	for(i = 0; i < m_BaseNumOfVertices; ++i)
	{
		KFogVertex* pFV = pFVBuffer + i;
		pFV->diffuse = m_Color;
	}
	m_pVB->Unlock();
}

void KVolumeFog::SetColorB(unsigned char b)
{
	m_Color &= 0xFFFFFF00;  // Mark off the blue channel
	m_Color |= (DWORD)b;

	KFogVertex* pFVBuffer = 0;
	m_pVB->Lock(0, 0, (void**)&pFVBuffer, 0);
	unsigned int i = 0;
	for(i = 0; i < m_BaseNumOfVertices; ++i)
	{
		KFogVertex* pFV = pFVBuffer + i;
		pFV->diffuse = m_Color;
	}
	m_pVB->Unlock();
}

void KVolumeFog::SetColorA(unsigned char a)
{
	m_Color &= 0x00FFFFFF;  // Mark off the alpha channel
	m_Color |= ((DWORD)a) << 24;

	KFogVertex* pFVBuffer = 0;
	m_pVB->Lock(0, 0, (void**)&pFVBuffer, 0);
	unsigned int i = 0;
	for(i = 0; i < m_BaseNumOfVertices; ++i)
	{
		KFogVertex* pFV = pFVBuffer + i;
		pFV->diffuse = m_Color;
	}
	m_pVB->Unlock();
}

void KVolumeFog::SetViewOffset(float ViewOffset)
{
	//float dvo = ViewOffset - m_ViewOffset;
	m_ViewOffset = ViewOffset;

	/*KFogVertex* pFVBuffer = 0;
	m_pVB->Lock(0, 0, (void**)&pFVBuffer, 0);
	unsigned int i = 0;
	for(i = 0; i < m_BaseNumOfVertices; ++i)
	{
		KFogVertex* pFV = pFVBuffer + i;
		pFV->y += dvo;
	}
	m_pVB->Unlock();*/
}

void KVolumeFog::SetDistBetweenFPs(float DistBetweenFPs)
{
	m_DistBetweenFPs = DistBetweenFPs;

	KFogVertex* pFVBuffer = 0;
	m_pVB->Lock(0, 0, (void**)&pFVBuffer, 0);

	unsigned int NumOfFogPlanes = m_FogPlanes.size();
	unsigned int i = 0;
	for(i = 0; i < NumOfFogPlanes; ++i)
	{
		KFogPlane* pFP = m_FogPlanes[NumOfFogPlanes - i - 1];
		KFogVertex* pFV = 0;

		float FogPlaneY = m_TopFogPlaneCenter.y - i * m_DistBetweenFPs;

		pFV = pFP->GetVertex(pFVBuffer, 0);
		pFV->y = FogPlaneY;

		pFV = pFP->GetVertex(pFVBuffer, 1);
		pFV->y = FogPlaneY;

		pFV = pFP->GetVertex(pFVBuffer, 2);
		pFV->y = FogPlaneY;

		pFV = pFP->GetVertex(pFVBuffer, 3);
		pFV->y = FogPlaneY;
	}

	CalBBox(pFVBuffer);

	m_pVB->Unlock();
}

void KVolumeFog::CalBBox(KFogVertex* pFVBuffer)
{
	if(!m_FogPlanes.empty())
	{
		KFogPlane* pFP = 0;
		pFP = m_FogPlanes[0];  // The lowest FogPlane
		KFogVertex* pFV = 0;
		pFV = pFP->GetVertex(pFVBuffer, 3);
		m_BBox_A.x = pFV->x;
		m_BBox_A.y = pFV->y;
		m_BBox_A.z = pFV->z;

		pFP = m_FogPlanes[m_FogPlanes.size() - 1];  // The highest FogPlane
		pFV = pFP->GetVertex(pFVBuffer, 1);
		m_BBox_B.x = pFV->x;
		m_BBox_B.y = pFV->y;
		m_BBox_B.z = pFV->z;
	}
}

void KVolumeFog::CalBoundingPlanes()
{
	m_TopPlane.m_fDistanceToOrigin = m_CurrTopPlaneY;
	m_LeftPlane.m_fDistanceToOrigin = -(m_TopFogPlaneCenter.x - m_HalfXSpan);
	m_RightPlane.m_fDistanceToOrigin = m_TopFogPlaneCenter.x + m_HalfXSpan;
	m_ZUpPlane.m_fDistanceToOrigin = m_TopFogPlaneCenter.z + m_HalfZSpan;
	m_ZBottomPlane.m_fDistanceToOrigin = -(m_TopFogPlaneCenter.z - m_HalfZSpan);
}

void KVolumeFog::Update()
{
	if(m_PrevTopFogPlaneCenter != m_CurrTopFogPlaneCenter)
	{
		m_PrevTopFogPlaneCenter = m_CurrTopFogPlaneCenter;
		SetTopFogPlaneCenter(m_PrevTopFogPlaneCenter);
	}

	if(m_PrevHalfSpanX != m_CurrHalfSpanX)
	{
		m_PrevHalfSpanX = m_CurrHalfSpanX;
		SetHalfSpanX(m_PrevHalfSpanX);
	}

	if(m_PrevHalfSpanZ != m_CurrHalfSpanZ)
	{
		m_PrevHalfSpanZ = m_CurrHalfSpanZ;
		SetHalfSpanZ(m_PrevHalfSpanZ);
	}

	if(m_PrevTopPlaneY != m_CurrTopPlaneY)
	{
		m_PrevTopPlaneY = m_CurrTopPlaneY;
		SetTopPlaneY(m_PrevTopPlaneY);
	}

	if(m_PrevNumOfFPs != m_CurrNumOfFPs)
	{
		m_PrevNumOfFPs = m_CurrNumOfFPs;
		SetNumOfFogPlanes(m_PrevNumOfFPs);
	}

	if(m_PrevColorR != m_CurrColorR)
	{
		m_PrevColorR = m_CurrColorR;
		SetColorR(m_PrevColorR);
	}

	if(m_PrevColorG != m_CurrColorG)
	{
		m_PrevColorG = m_CurrColorG;
		SetColorG(m_PrevColorG);
	}

	if(m_PrevColorB != m_CurrColorB)
	{
		m_PrevColorB = m_CurrColorB;
		SetColorB(m_PrevColorB);
	}

	if(m_PrevColorA != m_CurrColorA)
	{
		m_PrevColorA = m_CurrColorA;
		SetColorA(m_PrevColorA);
	}

	if(m_PrevViewOffset != m_CurrViewOffset)
	{
		m_PrevViewOffset = m_CurrViewOffset;
		SetViewOffset(m_PrevViewOffset);
	}

	if(m_PrevDistBetweenFPs != m_CurrDistBetweenFPs)
	{
		m_PrevDistBetweenFPs = m_CurrDistBetweenFPs;
		SetDistBetweenFPs(m_PrevDistBetweenFPs);
	}
}