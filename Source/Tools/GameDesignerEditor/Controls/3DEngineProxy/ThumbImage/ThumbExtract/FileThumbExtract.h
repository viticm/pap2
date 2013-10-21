// FileThumbExtract.h : Declaration of the CFileThumbExtract

#ifndef __FILETHUMBEXTRACT_H_
#define __FILETHUMBEXTRACT_H_

#include "resource.h"       // main symbols
#include <string>
using namespace std;
typedef basic_string<WCHAR> stdwstr;
//#include "IExtractImage.h"
#include "OBJSAFE.H"
/////////////////////////////////////////////////////////////////////////////
// CFileThumbExtract
class ATL_NO_VTABLE CFileThumbExtract : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CFileThumbExtract, &CLSID_FileThumbExtract>,
	public ISupportErrorInfo,
	public IDispatchImpl<IFileThumbExtract, &IID_IFileThumbExtract, &LIBID_THUMBEXTRACTLib>
{
public:
	CFileThumbExtract();

DECLARE_REGISTRY_RESOURCEID(IDR_FILETHUMBEXTRACT)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CFileThumbExtract)
	COM_INTERFACE_ENTRY(IFileThumbExtract)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()


BEGIN_CATEGORY_MAP(CFileThumbExtract)
  IMPLEMENTED_CATEGORY(CATID_SafeForScripting) 
  IMPLEMENTED_CATEGORY(CATID_SafeForInitializing) 
END_CATEGORY_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

protected:
	stdwstr m_wsDir,m_wsFile,m_wsPath,m_wsTempPath;
	long m_Width,m_Height;
	WCHAR m_wsBuffer[1000];
	CComPtr<IMalloc> m_pMalloc;
	HBITMAP m_hThumbnail;
	LPBYTE m_pBitmapData;
// IFileThumbExtract
public:
	STDMETHOD(get_ThumbJpgData)(/*[in]*/ long JpegQuality, /*[out, retval]*/ VARIANT *pVal);
	STDMETHOD(get_Version)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Stride)(/*[out, retval]*/ long *pVal);
	HRESULT FinalConstruct();
	HRESULT FinalRelease();
	HRESULT CreateThumbnail(DWORD dwWidth, DWORD dwHeight, HBITMAP* pThumbnail);
	STDMETHOD(SaveToFile)(/*[in]*/ BSTR FilePath);
	STDMETHOD(FreeThumbnail)();
	void ResolvePath();
	STDMETHOD(ExtractThumbnail)(/*[out,retval]*/ long *phBitmap);
	STDMETHOD(SetThumbnailSize)(/*[in]*/ long Width,/*[in]*/ long Height);
	STDMETHOD(SetFile)(/*[in]*/ BSTR File);
	STDMETHOD(SetDir)(/*[in]*/ BSTR Dir);
	STDMETHOD(SetPath)(/*[in]*/ BSTR Path);
	STDMETHOD(GetPath)(/*[out,retval]*/ BSTR* Path);
	STDMETHOD(GetDir)(/*[out,retval]*/ BSTR* Dir);
	STDMETHOD(GetFile)(/*[out,retval]*/ BSTR* File);
};

#endif //__FILETHUMBEXTRACT_H_
