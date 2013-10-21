////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DShaderRCBridge.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-8-4 10:09:41
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSHADERRCBRIDGE_H_
#define _INCLUDE_KG3DSHADERRCBRIDGE_H_

#include "../../ShaderLibrary/ShaderLibrary/ShaderLibraryMain.h"
////////////////////////////////////////////////////////////////////////////////
namespace KG3DShaderRCBridge
{
	enum
	{
		em_max_resource_id_digit = 8,	//最多是8位数的ID
	};
	//HGLOBAL LoadShaderRC(UINT uID, LPCVOID* ppData, DWORD* pSize);	//得到的HGLOBAL如果是空则失败，非空则在返回的内存用完之后，要用FreeResource释放。返回的内存不需要主动释放，FreeResource之后，系统会释放

	//VOID	TryInitResources();	//在其它东西调用之前要调用一次，可以重复调用
	//UINT	GetShaderResourceID(LPCTSTR pFileName);	//0表示失败，传入的字符串必须是小写的
	//LPCTSTR	GetShaderResourcePath(UINT uResourceID);

	//UINT	GetIDFromString(LPCTSTR filePathOrID);	//返回0则失败
	//BOOL IsFilePath(LPCTSTR filePathOrID);
	//BOOL IsIDValid(UINT uID);
	//LPCTSTR	GetStringFromID(UINT uID, TCHAR uBuffer[MAX_PATH]);	//返回uBuffer

	HRESULT InitLibrary();
	HRESULT UninitLibrary();	//主动调用比较好，主动释放资源避免有泄漏报告，提早释放也避免难以发现的析构错误
	IKG3DShaderLibrary* GetShaderLibraryInterface();
};

#endif //_INCLUDE_KG3DSHADERRCBRIDGE_H_
