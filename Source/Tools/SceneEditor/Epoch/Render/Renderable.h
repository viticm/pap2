// File: Renderable.h
// Desc: Warn: there's a big bug which can cause memory leak. However, don't have time to fix it.

#ifndef RENDERABLE_H
#define RENDERABLE_H


#include"..\Common\ReferenceCounter.h"
#include"RenderCommon.h"

namespace Render
{

class DLLAPI Renderable: public Common::ReferenceCounter
{
protected:
	D3DXMATRIX m_Matrix;

public:
	Renderable() { D3DXMatrixIdentity(&m_Matrix); }
	Renderable(const D3DXMATRIX& Matrix): m_Matrix(Matrix) {}
	Renderable(const Renderable& r): m_Matrix(r.m_Matrix) {}
	Renderable& operator = (const Renderable& r);
	virtual ~Renderable() {}

	virtual void Destroy() = 0;

	virtual void Render() = 0;  // First, call Apply() on each RenderState object. Then call DrawIndexedPrimitive() on Core
	                            // In fact, Render() would better return something, such as the number of rendered triangles or error-code(including HRESULT) occured in rendering. However, pushing all those possible infos into a global pipeline is a better solution.

	void GetMatrix(D3DXMATRIX& Matrix) const { Matrix = m_Matrix; }
	void SetMatrix(const D3DXMATRIX& Matrix) { m_Matrix = Matrix; }
	void MultiplyMatrix(const D3DXMATRIX& Matrix) { D3DXMatrixMultiply(&m_Matrix, &m_Matrix, &Matrix); }

	// Some trivial functions, really trivial!
	virtual IDirect3DVertexBuffer9* GetVertexBuffer() const { return 0; }
	virtual IDirect3DIndexBuffer9* GetIndexBuffer() const { return 0; }
	// More... (GetMaterial(), etc.)
	
	// More functionilities go here...
};

}  // namespace

#endif