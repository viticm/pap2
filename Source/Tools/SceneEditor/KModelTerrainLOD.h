// KModelTerrainLOD.h: interface for the KModelTerrainLOD class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KMODELTERRAINLOD_H__2A9DED39_9B28_4DD4_8053_2B68DA076049__INCLUDED_)
#define AFX_KMODELTERRAINLOD_H__2A9DED39_9B28_4DD4_8053_2B68DA076049__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XMesh.h"
#include "MeshTerran.h"

#include"Epoch\Maths\Maths.h"
#include"Epoch\Terrain\Terrain.h"
#include"Epoch\Render\Render.h"

#define DEF_TTD_NAME "Default.ttd"

class KTerrainTree: public Terrain::TerrainTree
{
private:
	KModelTerrain* m_lpTerrainData;

public:
	KTerrainTree(): Terrain::TerrainTree(), m_lpTerrainData(0) {}

	virtual float GetHeightValue(float& x, float& z);
	virtual void Render();

	void SetTerrainData(KModelTerrain* lpTerrainData) { m_lpTerrainData = lpTerrainData; }
	KModelTerrain* GetTerrainData() const { return m_lpTerrainData; }
};

class KModelTerrainLOD : public KModel
{
public:
	KModelTerrain* m_lpTerrainData;
	KTerrainTree* m_pKTT;
	Maths::Camera m_Camera;
	IDirect3DTexture9* m_TestTexture;

public:
	KModelTerrainLOD();
	virtual ~KModelTerrainLOD() { SafeDelete(m_pKTT); }

	virtual HRESULT LoadMesh(LPSTR pFileName);
	virtual HRESULT Render();
	virtual HRESULT FrameMove();
	virtual HRESULT CleanUp();

	HRESULT SetTerrainData(KModelTerrain* lpTerrainData);

};

#endif // !defined(AFX_KMODELTERRAINLOD_H__2A9DED39_9B28_4DD4_8053_2B68DA076049__INCLUDED_)
