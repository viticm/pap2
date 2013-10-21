// File: RenderState.h
// Desc: RenderState类家族还需要扩展，基本上凡是需要调用IDirect3DDevice9::SetXXX()的，都有一个对应的RenderState的派生类

#ifndef RENDERSTATE_H
#define RENDERSTATE_H

#include"..\Common\DLLAPI.h"
#include<vector>
using std::vector;

#include<d3d9.h>

namespace Render
{

class DLLAPI RenderState
{
public:
	RenderState() {}
	RenderState& operator = (const RenderState& rs);
	virtual ~RenderState() {}

	virtual bool IsValid() const = 0;
	virtual void Apply(bool bCalledByCore = false) = 0;
};

class DLLAPI RSStream: public RenderState
{
public:
	//IDirect3DVertexDeclaration9* m_pVertexDeclaration;
	DWORD m_dwFVF;
	//vector<IDirect3DVertexBuffer9*> m_VertexBuffers;
	IDirect3DVertexBuffer9* m_pVertexBuffer;
	unsigned int m_SizeOfVertex;

public:
	//RSStream(): RenderState(), m_pVertexDeclaration(0), m_VertexBuffers() {}
	RSStream(): RenderState(), m_dwFVF(0), m_pVertexBuffer(0), m_SizeOfVertex(0) {}
	//RSStream(IDirect3DVertexDeclaration9* pVertexDeclaration, IDirect3DVertexBuffer9** ppVertexBuffers, unsigned int NumOfVertexBuffers);
	RSStream(DWORD dwFVF, IDirect3DVertexBuffer9* pVertexBuffer, unsigned int SizeOfVertex): RenderState(), m_dwFVF(dwFVF), m_pVertexBuffer(pVertexBuffer), m_SizeOfVertex(SizeOfVertex) {}
	RSStream& operator = (const RSStream& rss);

	virtual bool IsValid() const;
	virtual void Apply(bool bCalledByCore = false);

	bool operator == (const RSStream& rsvb);
	bool operator != (const RSStream& rsvb);
};

class DLLAPI RSIndexBuffer: public RenderState
{
public:
	IDirect3DIndexBuffer9* m_pIndexBuffer;

public:
	RSIndexBuffer(): RenderState(), m_pIndexBuffer(0) {}
	RSIndexBuffer(IDirect3DIndexBuffer9* pIndexBuffer): RenderState(), m_pIndexBuffer(pIndexBuffer) {}
	RSIndexBuffer& operator = (const RSIndexBuffer& rsib);

	virtual bool IsValid() const { return m_pIndexBuffer != 0; }
	virtual void Apply(bool bCalledByCore = false);

	bool operator == (const RSIndexBuffer& rsib);
	bool operator != (const RSIndexBuffer& rsib);
};

class DLLAPI RSDrawIndexed: public RenderState
{
public:
	D3DPRIMITIVETYPE m_PrimitiveType;
	unsigned int m_NumOfVertices;
	unsigned int m_NumOfPrimitives;

public:
	RSDrawIndexed(): RenderState(), m_PrimitiveType(D3DPT_TRIANGLELIST), m_NumOfVertices(0), m_NumOfPrimitives(0) {}
	RSDrawIndexed(D3DPRIMITIVETYPE PrimitiveType, unsigned int NumOfVertices, unsigned int NumOfPrimitives): RenderState(), m_PrimitiveType(PrimitiveType), m_NumOfVertices(NumOfVertices), m_NumOfPrimitives(NumOfPrimitives) {}
	RSDrawIndexed& operator = (const RSDrawIndexed& rsdi);

	virtual bool IsValid() const;
	virtual void Apply(bool bCalledByCore = false);

	bool operator == (const RSDrawIndexed& rsdi);
	bool operator != (const RSDrawIndexed& rsdi);
};

enum RenderStateID
{
	RSID_STREAM = 0,
	RSID_INDEXBUFFER = 1,
	RSID_DRAWINDEXED = 2,
	RSID_NUMBER = 3  // Will be more...
};

}  // namespace

#endif