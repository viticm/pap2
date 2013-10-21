////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DEnvEffStarryNight.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-2-27 16:05:58
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DENVEFFSTARRYNIGHT_H_
#define _INCLUDE_IEKG3DENVEFFSTARRYNIGHT_H_

////////////////////////////////////////////////////////////////////////////////

struct IEKG3DEnvEffDescBase 
{
	DWORD dwType;
};

typedef TCHAR (TypePathBuffer)[MAX_PATH];

struct IEKG3DEnvEffStarryNightKeyDesc : public IEKG3DEnvEffDescBase
{
	FLOAT	fWidth;
	FLOAT	fHeight;
};

struct IEKG3DEnvEffStarryNightDesc : public IEKG3DEnvEffStarryNightKeyDesc
{
	TypePathBuffer strTexture;
};

struct IEKG3DParamDesc 
{
	DWORD dwType;
	LPCTSTR strParamName;
	size_t	uOffset;
	size_t  uSize;
};

typedef KGCH::TFrontAccessProxy<IEKG3DParamDesc>	TypeParamDescEnumer;

struct IEKG3DEnvEffDescHandler 
{
	virtual HRESULT GetDefDesc(IEKG3DEnvEffDescBase* pDesc, size_t uSizeOfDesc) = 0;
	virtual HRESULT GetDescParamTable(TypeParamDescEnumer** pEnumer, size_t* uKeyParamCount) = 0;
	virtual ~IEKG3DEnvEffDescHandler() =0{}
};

struct IEKG3DEnvEffStarryNight 
{
	//virtual HRESULT SetProperty(LPCTSTR, DWORD_PTR propertyValue) = 0;
	//virtual HRESULT GetProperty(LPCTSTR, DWORD_PTR* pPropertyValue) = 0;
	virtual HRESULT GetDescHandler(IEKG3DEnvEffDescHandler** pDescHandler) = 0;
};

#endif //_INCLUDE_IEKG3DENVEFFSTARRYNIGHT_H_
