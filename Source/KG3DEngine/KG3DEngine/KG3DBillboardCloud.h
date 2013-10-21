////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DBillboardCloud.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-6-26 9:43:02
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DBILLBOARDCLOUD_H_
#define _INCLUDE_KG3DBILLBOARDCLOUD_H_
class KG3DMaterial;
class KG3DTexture;

////////////////////////////////////////////////////////////////////////////////
struct KG3DBillboardCloudBuildStruct	//用于没有文件的加载，放在lParam里面，资源会AddRef，所以Build之后注意也要释放
{
	LPDIRECT3DVERTEXBUFFER9	m_pBuffer;
	KG3DTexture*			m_pTexture;
};
class KG3DBillboardCloud
{
public:
	enum
	{
		CUR_FVF = D3DFVF_XYZ | D3DFVF_TEX1,
		VERTEX_SIZE = sizeof(FLOAT) * 5,
		TEXTURE_WIDTH = 1024,
		TEXTURE_HEIGHT = 1024,
		TEXTURE_FORMAT = D3DFMT_A8R8G8B8,
	};
	struct VertexStruct 
	{
		D3DXVECTOR3 p;
		D3DXVECTOR2 uv;
	};
	typedef VertexStruct	TypeVertexStruct;
	DWORD GetPlaneCount()	{return m_dwPlaneCount;}

	static KG3DBillboardCloud* Build(LPCTSTR lpFileName, KG3DMaterial& Material, DWORD_PTR lParam);//如果第一参数传NULL，lParam要传入KG3DBillboardCloudBuildStruct，返回新的BC，Struct中的资源会在内部AddRef，所以外部要注意释放

	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(THIS_ REFIID riid, void** ppvObj){return E_FAIL;}

	HRESULT RenderBegin();
	HRESULT RenderEnd();
	HRESULT Render();
	VOID	SetTransform(const D3DXMATRIX& matTrans){m_matWorld = matTrans;}
	const D3DXMATRIX& GetTransform(){return m_matWorld;}
	static BOOL	CheckIsBillboardExist(LPCTSTR lpFileName);

	static HRESULT RenderBegin2();
	HRESULT Render2();
	static HRESULT RenderEnd2();
private:
	KG3DBillboardCloud(KG3DMaterial& Material);//用Build来创建
	~KG3DBillboardCloud();
	KG3DBillboardCloud(const KG3DBillboardCloud& Other);//无实现
	static KG3DBillboardCloud* BuildFromFile(LPCTSTR lpFileName, KG3DMaterial& Material, DWORD_PTR lParam);

private:
	static BOOL	GetBillboardPath(LPCTSTR lpFileName, LPTSTR lpBillboadPath, LPTSTR lpTextureColorPath,LPTSTR lpTextureNormalPath, size_t PathBufferSize);
private:

	ULONG	m_ulRefCount;	//实现引用技术
	LPDIRECT3DVERTEXBUFFER9	m_pVertexBuffer;
	KG3DTexture*			m_pTextureColor;
	KG3DTexture*			m_pTextureNormal;
	DWORD					m_dwPlaneCount;
	KG3DMaterial&			m_Material;
	D3DXMATRIX				m_matWorld;
	enum
	{
		em_mat_vp,
		em_mat_world,
		em_count,
	};
	static D3DXHANDLE		m_hEffectParams[em_count];
};

#endif //_INCLUDE_KG3DBILLBOARDCLOUD_H_
