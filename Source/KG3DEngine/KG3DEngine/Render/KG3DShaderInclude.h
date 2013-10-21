////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DShaderInclude.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-9-1 11:50:03
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSHADERINCLUDE_H_
#define _INCLUDE_KG3DSHADERINCLUDE_H_

////////////////////////////////////////////////////////////////////////////////
class KG3DShaderInclude : public ID3DXInclude 
{
	KG_TEST_DECLARE_CLASS_TEST(KG3DShaderInclude)
public:
	KG3DShaderInclude(LPCTSTR pFileIncludePath);
	STDMETHOD(Open)(THIS_ D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName
		, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
	STDMETHOD(Close)(THIS_ LPCVOID pData);
#if defined(DEBUG) | defined(_DEBUG)
protected:
#else
private:
#endif
	virtual IFile* GetFilePtr(LPCTSTR FileName);
private:
	std::tstring	GetFullFilePath(LPCTSTR FileName);
	std::tstring	m_FileIncludePath;
};

#endif //_INCLUDE_KG3DSHADERINCLUDE_H_
