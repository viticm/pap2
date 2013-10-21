// File: TerrainGroup.cpp
// Desc:

#include"TerrainGroup.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace Terrain
{

TerrainGroup& TerrainGroup::operator = (const TerrainGroup& tg)
{
	if(&tg != this)
	{
		Renderable::operator = (tg);

		m_RSIB = tg.m_RSIB;
		m_RSDI = tg.m_RSDI;

		m_MaxNumOfIndices = tg.m_MaxNumOfIndices;
		m_CurrentNumOfIndices = tg.m_CurrentNumOfIndices;
		m_IndexFormat = tg.m_IndexFormat;
	}

	return *this;
}

TerrainGroup::~TerrainGroup()
{
	if(GetCount() == 1)
	{
		Destroy();
	}
}

void TerrainGroup::Create(unsigned int MaxNumOfIndices, D3DFORMAT IndexFormat)
{
	// m_RSIB.m_pIndexBuffer should be 0 here.
	// Cause the problem of reference count, DON't call this function more than one time on a same object!

	m_MaxNumOfIndices = MaxNumOfIndices;
	m_IndexFormat = IndexFormat;

	m_RSIB.m_pIndexBuffer = g_GetRenderCore()->CreateIndexBuffer(m_MaxNumOfIndices, m_IndexFormat);
}

void TerrainGroup::Reset()
{
	m_CurrentNumOfIndices = 0;

	m_RSDI.m_NumOfVertices = 0;
	m_RSDI.m_NumOfPrimitives = 0;
}

void* TerrainGroup::LockIndexBuffer()
{
	if(!m_RSIB.IsValid())
	{
		// Push error-code into error-pipeline
		return 0;
	}

	void* pRet = 0;
	IDirect3DIndexBuffer9* pIB = m_RSIB.m_pIndexBuffer;
	if(FAILED(pIB->Lock(0, 0, &pRet, 0)))
	{
		// Push error-code into error-pipeline
		return 0;
	}

	return pRet;
}

void TerrainGroup::PushIndex(unsigned int Index, void* pIB)
{
	if(!pIB)
	{
		// Push error-code into error-pipeline
		return;
	}
	if(IsIndexBufferFull())
	{
		// Push error-code into error-pipeline
		return;
	}

	D3DINDEXBUFFER_DESC idesc;
	memset(&idesc, 0, sizeof(idesc));
	m_RSIB.m_pIndexBuffer->GetDesc(&idesc);
	unsigned int SizeOfIndex = Render::GetSizeOfIndex(idesc.Format);

	memcpy((unsigned char*)pIB + m_CurrentNumOfIndices * SizeOfIndex, &Index, SizeOfIndex);
	++m_CurrentNumOfIndices;
}

void TerrainGroup::UnlockIndexBuffer()
{
	if(!m_RSIB.IsValid())
	{
		// Push error-code into error-pipeline
		return;
	}

	IDirect3DIndexBuffer9* pIB = m_RSIB.m_pIndexBuffer;
	if(FAILED(pIB->Unlock()))
	{
		// Push error-code into error-pipeline
	}
}

void TerrainGroup::Destroy()
{
	SafeRelease(m_RSIB.m_pIndexBuffer);

	m_MaxNumOfIndices = 0;
	m_CurrentNumOfIndices = 0;
}

void TerrainGroup::Render()
{
	m_RSIB.Apply();
	m_RSDI.Apply();

	g_GetRenderCore()->DrawIndexedPrimitive(m_RSDI.m_NumOfVertices, m_RSDI.m_NumOfPrimitives, m_RSDI.m_PrimitiveType);
}

}  // namespace