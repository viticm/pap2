//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   IKG3DEngineController.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-9-15 10:00
//  Comment     :   封装加载旧的引擎Dll，提供所需的接口
//					代码版本：t_jx3_built_1-0-8-1548
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _IKG3DEngineController_H__
#define _IKG3DEngineController_H__

#include <Windows.h>
#include <vector>
#include <string>

class IKG3DEngineController
{
public:
	virtual int  Init(const char cszClientPath[], const char cszDLLName[]) = 0;
	virtual void UnInit() = 0;
	virtual int  LoadMapInfo(const char cszMapName[]) = 0;
	virtual int  RemoveMap() = 0;
	virtual HRESULT  GetAllTerrainInfoHandle(vector<UINT>* vecHandle) = 0;
	virtual HRESULT  GetAllEntityInfoHandle(vector<UINT>* vecHandle) = 0;
	virtual HRESULT  GetEntityInfoformation(UINT uEntityInfoHandle,DWORD* pdwDataType,DWORD* pdwLength,PVOID pData,DWORD dwMaxLen) = 0;
	virtual HRESULT  GetTerrainInfoformation(UINT uEntityInfoHandle,DWORD* pdwDataType,DWORD* pdwLength,PVOID pData) = 0;

};
#endif