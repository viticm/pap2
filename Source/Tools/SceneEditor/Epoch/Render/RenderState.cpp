// File: RenderState.cpp
// Desc:

#include"RenderState.h"
#include"RenderDLLFuncs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


namespace Render
{

// RenderState

RenderState& RenderState::operator = (const RenderState& rs)
{
	if(&rs != this)
	{
		// Do something...
	}

	return *this;
}

// RSStream

/*
RSStream::RSStream(IDirect3DVertexDeclaration9* pVertexDeclaration, IDirect3DVertexBuffer9** ppVertexBuffers, unsigned int NumOfVertexBuffers): RenderState(), m_pVertexDeclaration(pVertexDeclaration)
{
	for(unsigned int i = 0; i < NumOfVertexBuffers; ++i)
	{
		m_VertexBuffers.push_back(ppVertexBuffers[i]);
	}
}
*/

RSStream& RSStream::operator = (const RSStream& rss)
{
	if(&rss != this)  // First, must not self-assignment
	{
		RenderState::operator = (rss);

		m_dwFVF = rss.m_dwFVF;  // m_pVertexDeclaration = rss.m_pVertexDeclaration;
		m_pVertexBuffer = rss.m_pVertexBuffer;  // m_VertexBuffers = rss.m_VertexBuffers;
		m_SizeOfVertex = rss.m_SizeOfVertex;
	}

	return *this;
}

bool RSStream::IsValid() const
{
	if(m_dwFVF == 0)
	{
		return false;
	}

	/*
	if(m_VertexBuffers.empty())
	{
		return false;
	}

	for(unsigned int i = 0; i < m_VertexBuffers.size(); ++i)
	{
		if(!m_VertexBuffers[i])
		{
			return false;
		}
	}
	*/

	if(m_SizeOfVertex <= 0)
	{
		return false;
	}

	return m_pVertexBuffer != 0;
}

void RSStream::Apply(bool bCalledByCore)
{
	if(!IsValid())
	{
		// Push error-code into error-pipeline
		return;
	}

	bool bApply = false;

	if(!bCalledByCore)
	{
		RSStream* Cached = static_cast<RSStream*>(g_GetRenderCore()->GetCachedRenderState(RSID_STREAM));
		if(operator != (*Cached))
		{
			*Cached = *this;  // Update the cache
			bApply = true;
		}
	}

	if(bApply || bCalledByCore)
	{
		g_GetRenderCore()->SetVertexStream(0, m_pVertexBuffer, m_SizeOfVertex);
		g_GetRenderCore()->GetD3DDevice()->SetFVF(m_dwFVF);
	}
}

bool RSStream::operator == (const RSStream& rsvb)
{
	/*
	if(m_pVertexDeclaration != rsvb.m_pVertexDeclaration)
	{
		return false;
	}

	if(m_VertexBuffers.size() != rsvb.m_VertexBuffers.size())
	{
		return false;
	}

	for(unsigned int i = 0; i < m_VertexBuffers.size(); ++i)
	{
		if(m_VertexBuffers[i] != rsvb.m_VertexBuffers[i])
		{
			return false;
		}
	}
	*/

	return true;
}

bool RSStream::operator != (const RSStream& rsvb)
{
	if(m_dwFVF != rsvb.m_dwFVF)
	{
		return true;
	}

	if(m_pVertexBuffer != rsvb.m_pVertexBuffer)
	{
		return true;
	}

	return false;
	/*
	if(m_pVertexDeclaration != rsvb.m_pVertexDeclaration)
	{
		return true;
	}

	if(m_VertexBuffers.size() != rsvb.m_VertexBuffers.size())
	{
		return true;
	}

	for(unsigned int i = 0; i < m_VertexBuffers.size(); ++i)
	{
		if(m_VertexBuffers[i] != rsvb.m_VertexBuffers[i])
		{
			return true;
		}
	}

	return false;
	*/
}

// RSIndexBuffer

RSIndexBuffer& RSIndexBuffer::operator = (const RSIndexBuffer& rsib)
{
	if(&rsib != this)
	{
		RenderState::operator = (rsib);

		m_pIndexBuffer = rsib.m_pIndexBuffer;
	}

	return *this;
}

void RSIndexBuffer::Apply(bool bCalledByCore)
{
	if(!IsValid())
	{
		// Push error-code into error-pipeline
		return;
	}

	bool bApply = false;

	if(!bCalledByCore)
	{
		RSIndexBuffer* Cached = static_cast<RSIndexBuffer*>(g_GetRenderCore()->GetCachedRenderState(RSID_INDEXBUFFER));
		if(operator != (*Cached))
		{
			*Cached = *this;  // Update the cache
			bApply = true;
		}
	}

	if(bApply || bCalledByCore)
	{
		// Apply the render states
		g_GetRenderCore()->SetIndexBuffer(m_pIndexBuffer);
	}
}

bool RSIndexBuffer::operator == (const RSIndexBuffer& rsib)
{
	if(rsib.m_pIndexBuffer == m_pIndexBuffer)
	{
		return true;
	}

	return false;
}

bool RSIndexBuffer::operator != (const RSIndexBuffer& rsib)
{
	if(rsib.m_pIndexBuffer != m_pIndexBuffer)
	{
		return true;
	}

	return false;
}

// RSDrawIndexed

RSDrawIndexed& RSDrawIndexed::operator = (const RSDrawIndexed& rsdi)
{
	if(&rsdi != this)
	{
		RenderState::operator = (rsdi);

		m_PrimitiveType = rsdi.m_PrimitiveType;
		m_NumOfVertices = rsdi.m_NumOfVertices;
		m_NumOfPrimitives = rsdi.m_NumOfPrimitives;
	}

	return *this;
}

bool RSDrawIndexed::IsValid() const
{
	if(m_PrimitiveType == D3DPT_POINTLIST)
	{
		return false;  // This is because Direct3D9 does NOT support D3DPT_POINTLIST any more.
	}

	return (m_NumOfVertices > 0) && (m_NumOfPrimitives > 0);
}

void RSDrawIndexed::Apply(bool bCalledByCore)
{
	if(!IsValid())
	{
		// Push error-code into error-pipeline
		return;
	}

	bool bApply = false;

	if(!bCalledByCore)
	{
		RSDrawIndexed* Cached = static_cast<RSDrawIndexed*>(g_GetRenderCore()->GetCachedRenderState(RSID_DRAWINDEXED));
		if(operator != (*Cached))
		{
			*Cached = *this;  // Update the cache
			bApply = true;
		}
	}

	if(bApply || bCalledByCore)
	{
		// DON't apply the render state cause we don't want to do drawing operation now
	}
}

bool RSDrawIndexed::operator == (const RSDrawIndexed& rsdi)
{
	if(m_PrimitiveType == rsdi.m_PrimitiveType && m_NumOfVertices == rsdi.m_NumOfVertices && m_NumOfPrimitives == rsdi.m_NumOfPrimitives)
	{
		return true;
	}

	return false;
}

bool RSDrawIndexed::operator != (const RSDrawIndexed& rsdi)
{
	if(m_PrimitiveType != rsdi.m_PrimitiveType || m_NumOfVertices != rsdi.m_NumOfVertices || m_NumOfPrimitives != rsdi.m_NumOfPrimitives)
	{
		return true;
	}

	return false;
}

}  // namespace