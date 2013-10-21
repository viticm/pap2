////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DMeshHelpers.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-11-19 16:49:50
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DMESHHELPERS_H_
#define _INCLUDE_KG3DMESHHELPERS_H_
#include "KGMathFunctions.h"
////////////////////////////////////////////////////////////////////////////////

HRESULT D3DXMeshTransformation(LPD3DXMESH pMesh, const D3DXMATRIX* pMatrix, VisitorForD3DXVECTOR3* pPointFilter);//只对FVF里面有D3DFVF_XYZ的Mesh起作用，而且顶点要排最前面, pointFilter用于过滤某些不需要变换的点,可以传NULL。用了pointFilter要一个个点检查，比较慢
HRESULT D3DXMeshClone(LPD3DXMESH pSrc, LPD3DXMESH* pCloned);
HRESULT D3DXMeshCreateByVerticesPos(DWORD dwFVF, D3DVECTOR* pPosArray, DWORD dwVertexCount
									, DWORD* pIndexArray, DWORD dwIndexCount
									, LPDIRECT3DDEVICE9 pDevice, DWORD dwOption, LPD3DXMESH* pRet);//3个Index一个面
HRESULT D3DXMeshCreatePlane(LPDIRECT3DDEVICE9 pDevice, DWORD dwFVF, DWORD dwOption, UINT uXSlice, UINT uZSlice
						, FLOAT fXLength, FLOAT fZLength, LPD3DXMESH* pRet);	//一个在XZ平面上的Mesh

HRESULT D3DXMeshCreateDiamond(LPDIRECT3DDEVICE9 pDevice, DWORD dwFVF, DWORD dwOption, UINT uSlice
							, FLOAT fRadius, FLOAT fUpperConeHeight, FLOAT fLowerConeHeight, LPD3DXMESH* pRet);//创建两个圆锥组成的钻石型

//////////////////////////////////////////////////////////////////////////

class D3DXMeshVertexEnumer;

HRESULT D3DXMeshCreateVertexEnumer(LPD3DXMESH pMesh, D3DXMeshVertexEnumer& enumer);

class D3DXMeshVertexEnumer	
{
	friend HRESULT D3DXMeshCreateVertexEnumer(LPD3DXMESH pMesh, D3DXMeshVertexEnumer& enumer);
public:
	BOOL	IsValid();//用D3DXMeshCreateEnumer创建出来的Enumer用这个来检查是否有效
	VOID		SetPos(UINT uIndex, const D3DXVECTOR3& vPos);
	const D3DXVECTOR3& GetPos(UINT uIndex);
	UINT		GetVertexCount();
	BYTE*		GetVertexByteHead(UINT uIndex);
	LPD3DXMESH	GetMesh();

	D3DXMeshVertexEnumer();
	~D3DXMeshVertexEnumer();//析构的时候自动UnLock
private:	//一定要是private的
	LPD3DXMESH m_pMesh;
	DWORD m_dwNumBytePerVertex;
	DWORD m_dwNumVertexCount;
	BYTE* m_pBuffer;

	D3DXMeshVertexEnumer(const D3DXMeshVertexEnumer&);	//不能复制
	D3DXMeshVertexEnumer& operator=(const D3DXMeshVertexEnumer&);//不能赋值
};

//////////////////////////////////////////////////////////////////////////
class D3DXMeshVertexTexCoordEnumer;

HRESULT D3DXMeshCreateVertexTexCoordEnumer(D3DXMeshVertexEnumer& vEnumer, D3DXMeshVertexTexCoordEnumer& texCoordEnumer);//vEnumer必须已经Valid了

class D3DXMeshVertexTexCoordEnumer	//需要在VertexEnumer的生命周期内使用
{
	friend HRESULT D3DXMeshCreateVertexTexCoordEnumer(D3DXMeshVertexEnumer& vEnumer, D3DXMeshVertexTexCoordEnumer& texCoordEnumer);
public:
	FLOAT&	GetTexCoord1(UINT uIndex);
	D3DXVECTOR2&	GetTexCoord2(UINT uIndex);
	D3DXVECTOR3&	GetTexCoord3(UINT uIndex);
	D3DXVECTOR4&	GetTexCoord4(UINT uIndex);
	BOOL	IsValid();
	UINT	GetTexCoordSize();//即用多少个浮点
	BYTE*	GetTexCoordHead(UINT uIndex);
	
	D3DXMeshVertexTexCoordEnumer();
private:
	D3DXMeshVertexEnumer* m_pVertexEnumer;
	DWORD	m_dwByteOffsetOfTexture;
	DWORD	m_dwTexCoordSize;

	D3DXMeshVertexTexCoordEnumer(const D3DXMeshVertexTexCoordEnumer&);	//不能复制
	D3DXMeshVertexTexCoordEnumer& operator=(const D3DXMeshVertexTexCoordEnumer&);//不能赋值
};
//////////////////////////////////////////////////////////////////////////
class D3DXMeshIndexEnumer;

HRESULT D3DXMeshCreateIndexEnumer(LPD3DXMESH pMesh, D3DXMeshIndexEnumer& enumer);

class D3DXMeshIndexEnumer
{
	friend HRESULT D3DXMeshCreateIndexEnumer(LPD3DXMESH pMesh, D3DXMeshIndexEnumer& enumer);
public:
	BOOL	IsValid();//用这个来检查是否有效
	VOID	SetIndex(UINT uWhichIndex, DWORD dwIndex);//16位IndexBuffer的时候，内部会强制降成WORD，只要传值的时候保证不越WORD_MAX就没事了
	DWORD	GetIndex(UINT uWhichIndex);
	BOOL	Is32Bit();
	UINT	GetIndexCount();
	D3DXMeshIndexEnumer();
	~D3DXMeshIndexEnumer();//析构的时候自动UnLock
private:	//一定要是private的
	LPD3DXMESH m_pMesh;
	DWORD m_dwNumBytePerIndex;
	DWORD m_dwNumIndexCount;
	BYTE* m_pBuffer;

	D3DXMeshIndexEnumer(const D3DXMeshIndexEnumer&);	//不能复制
	D3DXMeshIndexEnumer& operator=(const D3DXMeshIndexEnumer&);//不能赋值
};
//////////////////////////////////////////////////////////////////////////
class D3DXMeshAttributeEnumer;

HRESULT D3DXMeshCreateAttributeEnumer(LPD3DXMESH pMesh, D3DXMeshAttributeEnumer& enumer);
HRESULT D3DXMeshZeroMeshAttributes(LPD3DXMESH pMesh );

class D3DXMeshAttributeEnumer
{
	friend HRESULT D3DXMeshCreateAttributeEnumer(LPD3DXMESH pMesh, D3DXMeshAttributeEnumer& enumer);
public:
	BOOL	IsValid();//用这个来检查是否有效
	
	VOID	SetAttribute(UINT uWhichIndex, DWORD dwAttribute);
	DWORD	GetAttribute(UINT uWhichIndex);

	UINT	GetAttributeCount();
	VOID	SetAllAttributeToValue(DWORD dwValue);

	D3DXMeshAttributeEnumer();
	~D3DXMeshAttributeEnumer();//析构的时候自动UnLock
private:	//一定要是private的
	LPD3DXMESH m_pMesh;
	DWORD m_dwNumFaceCount;
	BYTE* m_pBuffer;

	D3DXMeshAttributeEnumer(const D3DXMeshAttributeEnumer&);	//不能复制
	D3DXMeshAttributeEnumer& operator=(const D3DXMeshAttributeEnumer&);//不能赋值
};


#endif //_INCLUDE_KG3DMESHHELPERS_H_
