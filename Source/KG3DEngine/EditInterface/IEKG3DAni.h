

#ifndef _INCLUDE_IEKG3DAni_H_
#define _INCLUDE_IEKG3DAni_H_

#include "KG3DInterface.h"

////////////////////////////////////////////////////////////////////////////////
class IEKG3DMesh;
interface IEKG3DAni : public IKG3DResourceBase
{
	virtual HRESULT GetFrameLength(FLOAT *pfRetlength) = 0;
	virtual HRESULT SetFrameLength(FLOAT fFrameLength) = 0;

	virtual HRESULT GetNumFrames(DWORD *pdwRetNum) = 0;

	virtual HRESULT GetName(LPCTSTR *ppszName) = 0;

	virtual HRESULT NormalizeClipData(IEKG3DMesh* pMesh, BOOL bSaveToFile) = 0;
	virtual HRESULT NormalizeClipData(IEKG3DAni* pAni, BOOL bSaveToFile) = 0;

	virtual HRESULT CheckLoadState() = 0;
};

#endif //_INCLUDE_IEKG3DAni_H_
