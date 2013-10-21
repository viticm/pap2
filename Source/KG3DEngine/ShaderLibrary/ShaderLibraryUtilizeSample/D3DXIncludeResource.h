////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : D3DXIncludeResource.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-7-28 16:51:42
//  Comment     : 
/*
shader打包到资源里面的需求如下：
1.shader打包到exe，随引擎exe发布。这个是必须的，符合敏捷原则："重用发布等价原则REP"所指出的：重用的粒度就是发布的粒度。
不做资源dll，即使是生成了资源dll，这也不能根本上解决引擎和shader必须一致发布的问题，只要不是同一个文件一起发布也会造成不匹配。
况且所有shader打包也不过500k以下，要做压缩也很容易做，所以不会影响发布的大小。
2.shader变化之后，工程能自动识别，并自动重新打包。做不到就没有意义，一样会造成版本不匹配
3.shader可以实时编辑并重加载，这个也是必须的，不然每次都编译超级麻烦
*/
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_D3DXINCLUDERESOURCE_H_
#define _INCLUDE_D3DXINCLUDERESOURCE_H_

////////////////////////////////////////////////////////////////////////////////

class KG3DShaderIncludeForRC : public ID3DXInclude
{
public:
	STDMETHOD(Open)(THIS_ D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
	STDMETHOD(Close)(THIS_ LPCVOID pData);
	KG3DShaderIncludeForRC();
	~KG3DShaderIncludeForRC();
private:
	HGLOBAL	m_hResource;
};

HGLOBAL MyLoadResourceShader(DWORD dwType, LPCVOID* ppData, DWORD* pSize);

LPCSTR	MyGetShaderPath();

enum
{	
	DEFST_SKYBOX = 0,
};

#endif //_INCLUDE_D3DXINCLUDERESOURCE_H_
