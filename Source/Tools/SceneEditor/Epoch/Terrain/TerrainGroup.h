// File: TerrainGroup.h
// Desc:

#ifndef TERRAINGROUP_H
#define TERRAINGROUP_H

#include"..\Render\Render.h"

namespace Terrain
{

class DLLAPI TerrainGroup: public Render::Renderable
{
public:
	Render::RSIndexBuffer m_RSIB;
	Render::RSDrawIndexed m_RSDI;

	unsigned int m_MaxNumOfIndices;
	unsigned int m_CurrentNumOfIndices;  // statistic
	D3DFORMAT m_IndexFormat;

public:
	TerrainGroup(): Render::Renderable(), m_RSIB(), m_RSDI(D3DPT_TRIANGLELIST, 0, 0), m_MaxNumOfIndices(0), m_CurrentNumOfIndices(0), m_IndexFormat(D3DFMT_INDEX16) {}
	TerrainGroup(const TerrainGroup& tg): Renderable(tg), m_RSIB(tg.m_RSIB), m_RSDI(tg.m_RSDI), m_MaxNumOfIndices(tg.m_MaxNumOfIndices), m_CurrentNumOfIndices(tg.m_CurrentNumOfIndices), m_IndexFormat(tg.m_IndexFormat) {}
	TerrainGroup& operator = (const TerrainGroup& tg);
	virtual ~TerrainGroup();

	void Create(unsigned int MaxNumOfIndices, D3DFORMAT IndexFormat = D3DFMT_INDEX16);
	unsigned int GetCurrentNumOfIndices() const { return m_CurrentNumOfIndices; }
	bool IsIndexBufferFull() const { return m_CurrentNumOfIndices + 3 > m_MaxNumOfIndices; }
	void Reset();
	void* LockIndexBuffer();
	void PushIndex(unsigned int Index, void* pIB);
	void AddNumOfPrimitives() { ++m_RSDI.m_NumOfPrimitives; }
	unsigned int GetNumOfPrimitives() const { return m_RSDI.m_NumOfPrimitives; }
	void SetNumOfVertices(unsigned int NumOfVertices) { m_RSDI.m_NumOfVertices = NumOfVertices; }
	void UnlockIndexBuffer();

	virtual void Destroy();
	virtual void Render();

	virtual IDirect3DIndexBuffer9* GetIndexBuffer() const { return m_RSIB.m_pIndexBuffer; }
};

}  // namespace

#endif