////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DShaderIncludeForRC.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-8-4 10:14:52
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSHADERINCLUDEFORRC_H_
#define _INCLUDE_KG3DSHADERINCLUDEFORRC_H_

////////////////////////////////////////////////////////////////////////////////
struct IKG3DShaderLibraryResource;
class KG3DShaderIncludeForRC : public ID3DXInclude
{
public:
	STDMETHOD(Open)(THIS_ D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
	STDMETHOD(Close)(THIS_ LPCVOID pData);
	KG3DShaderIncludeForRC();
	~KG3DShaderIncludeForRC();
private:
	enum{em_max_include_files = 20,};
	std::vector<IKG3DShaderLibraryResource*> m_vecResources;
};

#endif //_INCLUDE_KG3DSHADERINCLUDEFORRC_H_
