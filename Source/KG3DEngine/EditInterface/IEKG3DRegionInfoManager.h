////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DRegionInfoManager.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-2 10:30:31
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DRegionInfoManager_H_
#define _INCLUDE_IEKG3DRegionInfoManager_H_
#include "KG3DInterface.h"
////////////////////////////////////////////////////////////////////////////////

interface IEKG3DSceneSceneEditor;
struct IEKG3DRegionInfoManager ///: public IKG3DRegionInfoManager，不要继承I接口，省的错误的Release
{
	virtual HRESULT GetRegionInfoData(D3DXVECTOR3 vecPos, DWORD* pdwData) = 0;
	virtual HRESULT IELoadFromFile(const TCHAR cszFileName[], DWORD_PTR uFileNameHash, DWORD_PTR uOption) = 0;
	virtual HRESULT GetRegionTexState(BOOL* pbIsExist, INT* pnID, LPSTR lpTexPathRetBuffer, INT nBufferSize) = 0;
	virtual HRESULT GetDiffArrayCount(INT* pnCount) = 0;
	virtual HRESULT GetDiffArray(DWORD* pBuffer, INT nBufferSize) = 0;
	virtual HRESULT GetDefaultParam(KG3DREGION_INFORMER_LOADFILE_PARAM* pParam) = 0;
	virtual HRESULT GenerateRegionTex(DWORD_PTR dwExtraParam, DWORD* pdwTexID) = 0;
	virtual HRESULT GetPath(LPSTR pPathBuffer, INT nBufferSize) = 0;
	virtual HRESULT Save(const TCHAR cszFileName[], DWORD_PTR uOption) = 0;
	virtual HRESULT IEGetState(KG3DREGION_INFORMER_STATE* pState) = 0;
};

struct IEKG3DRegionInfoManagerTable
{
	virtual HRESULT GetCount(INT* pnRet) = 0;
	virtual HRESULT GetFirstRIM(IEKG3DRegionInfoManager** ppRIM, DWORD* pdwFirstHandle, DWORD* pdwNextHandle) = 0;
	virtual HRESULT GetRIM(DWORD dwHandle, IEKG3DRegionInfoManager** ppRIM, DWORD* pdwNextHandle) = 0;///返回的是下一个的Handle
	virtual HRESULT Add(const TCHAR cszFileName[]
						, unsigned uFileNameHash
						, unsigned uOption
						, DWORD_PTR WParam
						, DWORD_PTR LParam
						, IEKG3DRegionInfoManager** ppRIM
						, DWORD* pdwHandle) = 0;///WParam是KG3DREGION_INFORMER_LOADFILE_PARAM*
	virtual HRESULT Del(DWORD dwHandle) = 0;
};
#endif //_INCLUDE_IEKG3DRegionInfoManager_H_
