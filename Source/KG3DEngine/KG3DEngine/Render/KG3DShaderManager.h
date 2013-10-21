////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DShaderManager.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-29 10:06:33
//  Comment     : 参考CPP的说明
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSHADERMANAGER_H_
#define _INCLUDE_KG3DSHADERMANAGER_H_
#include "KG3DShaderHolder.h"
////////////////////////////////////////////////////////////////////////////////

struct KG3DShaderVisitor
{
	virtual HRESULT Accept(LPD3DXEFFECT pEffect) = 0;
	virtual KG3DShaderVisitor* Clone()const = 0;
	virtual ~KG3DShaderVisitor() = 0{}
};

struct KG3DShaderBuildParams 
{
	TCHAR strFilePathOrResourceIDString[MAX_PATH];
	std::vector<D3DXMACRO> macroArray;				//里面的Macro的字符指针也是不能保存的
	TCHAR strIncludePath[MAX_PATH];
	DWORD dwFlag;
	size_t uShaderParamHandleSize;
	BOOL	bUseEffectPool;							//如果非0就使用EffectPool，EffectPool是唯一的
	const KG3DShaderEventHandler* pEventHandler;	//得到这个之后，只能临时用，不能保存。内部没有加引用计数

	KG3DShaderBuildParams(){ZeroMemory(this, sizeof(*this));}

	D3DXMACRO_ARRAY GetMacroArray()
	{
		if (macroArray.empty())
		{
			return D3DXMACRO_ARRAY::GetEmptyMacroArray(); 
		}
		D3DXMACRO_ARRAY retArray = {
			&macroArray[0],		//因为vector内存是连续的，所以可以直接放出去
			macroArray.size(),
		};
		return retArray;
	}
};

struct KG3DShaderManager
{
	virtual KG3DShaderHolder Create(LPDIRECT3DDEVICE9 pDevice,
		LPCTSTR pSrcFile,					//会自动转小写
		D3DXMACRO_ARRAY macroArray = D3DXMACRO_ARRAY::GetEmptyMacroArray(),	//需要分大小写
		LPCTSTR pIncludeFilePath = NULL,	//会自动转小写，DX的函数是LPD3DXINCLUDE的，这里已经帮忙包装了，直接传文件名路径就可以了，至于文件名，由Shader内include关键字指定
		DWORD Flags = 0,
        size_t uShaderParamHandleSize = 0,
		BOOL bUseEffectPool = NULL,	//是否使用EffectPool，如果使用那么用的是ShaderManager内部维护的唯一Pool。EffectPool不需要多个
		std::tstring* pErrorString = NULL,
		const KG3DShaderEventHandler* pEventHandler = NULL	//没有特殊需求就不需要这个。里面会Clone，但是是否new一个可以自己在Clone函数中决定，但要保证生命周期
		) = 0;//返回的KG3DShaderHolder拿到就可以用了，Effect加载失败可以用Reload来重新加载，但是失败的Effect可以继续用的
	
	virtual HRESULT ReloadAll() = 0;
	virtual HRESULT Reload(KG3DShaderPtrHolder& shaderPtrHolder) = 0;
	virtual HRESULT ReloadWithMacroModification(KG3DShaderPtrHolder& shaderPtrHolder, D3DXMACRO_ARRAY macroArrayFull) = 0;	//改Macro定义的话，需要重新加载shader
	virtual HRESULT Find(LPCTSTR shaderName, KG3DShaderHolder& retHolder) = 0;	//速度很慢，慎用，不是用来哈希的,只兼容lower字符串

	virtual HRESULT GetShaderBuildParams(KG3DShaderHolder& shaderHolder, KG3DShaderBuildParams* pBuildParams) = 0;	//得到的Param必须在调用的函数中立刻用，不要保存里面的指针，过期不保证有效。也别改动那些指针的引用计数，如果改了就是往死里整了
	//设备恢复自己管理，不用担心
	virtual HRESULT SetShaderEventHandler(KG3DShaderHolder& shaderHolder, KG3DShaderEventHandler* pEventHandler) = 0;//里面会调用Clone

	static std::tstring GetResourceString(LPCTSTR strPath);	//返回的是strBuffer的首字符
	static BOOL GetPathFromResourceString(LPCTSTR strResourceString, std::tstring* pRet);
	static BOOL IsResourceString(LPCTSTR strIn);

	virtual ~KG3DShaderManager() = 0{}
};

KG3DShaderManager& g_GetShaderManager();

namespace Private
{
	HRESULT	ReloadShaderHolder(KG3DShaderPtrHolder& Holder);
	LPD3DXEFFECT GetSingletonDummyEffect();
	//LPD3DXEFFECT CreateDummyEffect();
	bool IsDummyEffect(LPD3DXEFFECT p);
	bool IsShaderHolderInited(KG3DShaderHolder& Holder);
};



#endif //_INCLUDE_KG3DSHADERMANAGER_H_
