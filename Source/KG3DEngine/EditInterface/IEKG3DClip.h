////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DClip.h
//  Version     : 1.0
//  Creator     : Xia Yong
//  Create Date : 2006-11-1 15:51:00
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DCLIP_H_
#define _INCLUDE_IEKG3DCLIP_H_

interface IEKG3DClip : public IUnknown
{
    virtual HRESULT GetFrameLength(FLOAT *pfRetlength) = 0;
    virtual HRESULT SetFrameLength(FLOAT fFrameLength) = 0;

    virtual HRESULT GetNumFrames(DWORD *pdwRetNum) = 0;

    virtual HRESULT GetName(LPCTSTR *ppszName) = 0;
	virtual HRESULT Rename(const TCHAR* strNewName) = 0;

	virtual HRESULT GetPathName(LPCTSTR *ppszPath) = 0;
	virtual HRESULT SetPathName(const TCHAR *strPath) = 0;

	virtual HRESULT SaveToFile(LPCTSTR cszFileName, UINT uOption) = 0;
};

#endif //_INCLUDE_IEKG3DCLIP_H_
