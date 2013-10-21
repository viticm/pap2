////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DShaderManager.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-29 10:06:30
//  Comment     : 
/*
迭代1
1.1 创建Shader
1.2 自动设备丢失与恢复
1.3 方便的Reload，要求能够记录创建时的参数，并方便的创建
1.4 方便的Shader重用。
1.5 文件不存在的判断。

迭代2
2.1 没用的Shader的垃圾回收
2.2 方便的Include并兼容打包
2.3 错误的文件写Log，并防止多次写同样的Log

迭代3
3.1 可扩展的OnLost和OnReset
3.2 可以保存Handle的Buffer
3.3 Shared Pool
3.4 多Macro


Working
4.1 自动Reload（不是很必要）

KG3DShaderHolder

ShaderHolder的其中一个主要目的是自动的Effect重加载，因为如果出去的是一个指针，那么重加载之后指针就变了，外面的指针
收不回来。而如果出去的是ShaderHolder的话，一切就好办了。注意要每次用GetEffect取指针，在一个函数内部临时
复制这个指针是可以的，但不能保存成成员之类的，因为不能保证什么时候会重加载。

KG3DShaderHolder不能直接保存Effect的指针引用，而是通过动态分配的KG3DShaderPtrHolder来保存指针
如果由KG3DShaderHolder直接保存指针引用，无法实现其行为的改变。而有KG3DShaderPtrHolder的话，是有
办法扩展其行为的。

关于垃圾回收，ShaderManager必然需要保存一份加载的信息用于重加载，这份信息不应该由ShaderHolder保存，
不然重用ShaderHolder的时候就太臃肿了。而ShaderHolder析构的时候应该也释放这部分信息。但ShaderHolder
不应该知道ShaderManager的存在，所以ShaderManager每隔一段时间，检查一次引用计数，然后释放掉Shader
这样的行为是很安全的。另外由于引用计数由ShaderHolder本身自己处理，外部不需要管任何引用计数的Add
Release，所以其行为已经很像智能指针了，而且比较好理解。

KG3DShaderPtrHolder

KG3DShaderHolder本身只是个智能指针类似的管理引用计数的容器，并进行某些总要方法的托管。行为类似C#里面的Ref。所以它是不能
进行扩展的，如果要ReloadShader有些不同的行为，没有什么虚函数可以覆盖。而给予KG3DShaderHolder的职责，它也不应该有任何虚
行为。所以扩展的任务是放在KG3DShaderPtrHolder里面的，它同时也用于Shader的引用计数，而不直接使用D3DXEffect的引用计数。不
直接使用Effect的引用计数是因为随便改变其引用计数无法预测DX的行为，因为我们没有它的代码，而且由此造成的错误也是很难检查的
既然有这个类包裹了Effect，把引用计数做成外覆型的设计就很容易实现了，而且也很容易管理。

KG3DShaderPtrHolder虽然能扩展，但是暂时没有被扩展出去。如果要让ShaderHolder有不同的行为，应该在KG3DShaderPtrHolder那扩展
*/
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DShaderManager.h"
#include "KG3DMessenger.h"
#include "KG3DSystemEventListener.h"
#include "KG3DShaderHolder.h"
#include "KG3DShaderInclude.h"
#include "KG3DGraphicsTool.h"
#include "DummyInterfaces\KGDummyID3DXEffect.h"
#include "KG3DShaderInfrastructure.h"
#include "KG3DShaderIncludeForRC.h"
#include "KG3DShaderRCBridge.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class KG3DShaderManagerSolid;



//////////////////////////////////////////////////////////////////////////

class KG3DShaderPtrHolderSolid : public KG3DShaderPtrHolder
{
	friend KG3DShaderManagerSolid;

protected:
	virtual LPD3DXEFFECT GetEffect()
	{
		_ASSERTE(NULL != m_lpShaderBuildStruct);
		return m_lpShaderBuildStruct->pEffect;	//可以为NULL
	}

    virtual LPVOID GetEffectParamHandleBuffer(size_t *puRetBufferSize)
    {
		_ASSERTE(NULL != m_lpShaderBuildStruct);
        if (NULL != puRetBufferSize)
        {
            *puRetBufferSize = m_lpShaderBuildStruct->vecEffectParamHandleBuffer.size();
        }

        if (0 == m_lpShaderBuildStruct->vecEffectParamHandleBuffer.size())
            return NULL;

        return (void *)&(m_lpShaderBuildStruct->vecEffectParamHandleBuffer[0]);
    }


    explicit KG3DShaderPtrHolderSolid(KG3DShaderBuildStruct& buildStruct)	//会保存指针，注意传入的对象的生命周期
		:m_lpShaderBuildStruct(&buildStruct)
		,m_ulRefCount(1)
	{
		_ASSERTE(NULL != m_lpShaderBuildStruct);
		++buildStruct.uRef;         
	}

	~KG3DShaderPtrHolderSolid()
	{
		_ASSERTE(NULL != m_lpShaderBuildStruct);
		_ASSERTE(m_lpShaderBuildStruct->uRef >= 1);
		--m_lpShaderBuildStruct->uRef;
		m_lpShaderBuildStruct = NULL;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef()
	{
		return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
	}

	virtual	ULONG STDMETHODCALLTYPE Release()
	{
        ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
        if (uNewRefCount == 0)
            delete this; 
        return uNewRefCount;
	}
	
private:
	KG3DShaderPtrHolderSolid(const KG3DShaderPtrHolderSolid&);
	KG3DShaderPtrHolderSolid& operator=(const KG3DShaderPtrHolderSolid& Other);
	
	KG3DShaderBuildStruct*	m_lpShaderBuildStruct;//这里必须要是指针，不然没有法子更新所有的ShaderHolder

	ULONG			m_ulRefCount;
};

//////////////////////////////////////////////////////////////////////////

class KG3DShaderManagerSolid : public KG3DShaderManager, public KG3DMessageSingleSenderReceiver
{
	KG_TEST_DECLARE_CLASS_TEST(KG3DShaderManagerSolid)
public:
	KG3DShaderManagerSolid(KG3DSystemEventListener& List);
	~KG3DShaderManagerSolid();

	virtual KG3DShaderHolder Create(LPDIRECT3DDEVICE9 pDevice,
		LPCTSTR pFilePathOrResourceIDString,
		D3DXMACRO_ARRAY macroArray = D3DXMACRO_ARRAY::GetEmptyMacroArray(),
		LPCTSTR pIncludeFilePath = NULL,
		DWORD Flags = 0,
        size_t uShaderParamHandleSize = 0,
		BOOL bUseEffectPool = FALSE,
		std::tstring* pErrorString = NULL,
		const KG3DShaderEventHandler* pEventHandler = NULL);

	KG3DShaderPtrHolder* CreateShaderHolder(LPDIRECT3DDEVICE9 pDevice,
		LPCTSTR pFilePathOrResourceIDString,
		D3DXMACRO_ARRAY macroArray,
		LPCTSTR pIncludeFilePath,
		DWORD Flags,
        size_t uShaderParamHandleSize,
		BOOL bUseEffectPool,
		std::tstring* pErrorString,
		const KG3DShaderEventHandler* pEventHandler);	//失败返回NULL

	virtual HRESULT Reload(KG3DShaderPtrHolder& Holder);
	virtual HRESULT ReloadAll();
	virtual HRESULT ReloadWithMacroModification(KG3DShaderPtrHolder& shaderPtrHolder, D3DXMACRO_ARRAY macroArrayFull);	//必须是全部Macro定义，如果需要前面的Macro定义，可以用GetShaderBuildParams得到，用MergeDefinition就可以合并两个D3DXMACRO_ARRAY
	virtual HRESULT Find(LPCTSTR shaderName, KG3DShaderHolder& retHolder);

	virtual HRESULT GetShaderBuildParams(KG3DShaderHolder& shaderHolder, KG3DShaderBuildParams* pBuildParams);

	virtual HRESULT SetShaderEventHandler(KG3DShaderHolder& shaderHolder, KG3DShaderEventHandler* pEventHandler);
	virtual void __stdcall OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender );

private:
	HRESULT ReloadEffect(KG3DShaderBuildStruct& Struct, D3DXMACRO_ARRAY* pNewMacroArray);
	virtual HRESULT EffectCreationCore(const KG3DShaderBuildStruct& Struct, D3DXMACRO_ARRAY macroArray
									, LPD3DXEFFECT* ppEffectCreated, std::tstring* pErrorString);
    virtual KG3DShaderPtrHolder* BuildShaderHolder(KG3DShaderBuildStruct& buildStruct);//需要传入内存地址的，小心
	void Clear();
	
	virtual VOID OutputError(const std::tstring& filePathOrID, const std::tstring& ErrorString);
	virtual bool IsTimeToCollectGarbage();
	void GarbageCollection();
	LPD3DXEFFECTPOOL GetEffectPool(LPDIRECT3DDEVICE9 pDevice);
	KG3DShaderPtrHolder* AddStruct(const KG3DShaderBuildStruct &Struct, size_t uShaderParamHandleSize);		

	KG_CUSTOM_HELPERS::KGBitSet<32>	m_bitset;
	enum
	{
		em_is_device_release_message_received,
	};	

	enum
	{
		em_max_macro_def = 50,
		em_max_effect = 100,	//因为ShaderHolder里面放的是Effect指针的引用，所以不能容器不能引起空间重分配，不然很危险。
		em_max_garbage_weight = 1,//太久不重用的Effect就可以删除了
		em_min_garbage_collection_time_gap = 2,//单位是秒
	};

	size_t GetInvalidHandle(){return std::numeric_limits<size_t>::max();}

	typedef std::list<KG3DShaderBuildStruct>	TypeCt;
	TypeCt	m_Effects;

	typedef	std::map<tstring, TypeCt::iterator>	TypeStringEffectMap;
	TypeStringEffectMap m_StringEffectMap;

	LPD3DXEFFECTPOOL	m_pEffectPool;
};

namespace
{
	HRESULT ConvertKG3DShaderBuildStructToParams(const KG3DShaderBuildStruct& buildStruct, KG3DShaderBuildParams& buildParams)
	{
		errno_t errnoTemp = _tcscpy_s(buildParams.strFilePathOrResourceIDString, buildStruct.strFilePathOrResourceID.c_str());
		KG_PROCESS_ERROR(0 == errnoTemp);

		buildStruct.macroDef.ToD3DXMACRO_ARRAY(buildParams.macroArray);

		errnoTemp = _tcscpy_s(buildParams.strIncludePath, buildStruct.strIncludeFilePath.c_str());
		KG_PROCESS_ERROR(0 == errnoTemp);

		buildParams.dwFlag = buildStruct.dwFlags;

		buildParams.uShaderParamHandleSize = buildStruct.vecEffectParamHandleBuffer.size();

		buildParams.bUseEffectPool = buildStruct.bUseEffectPool;

		buildParams.pEventHandler = buildStruct.pEventHandler;	//不加引用计数。这个东东是没有引用计数的。用的时候会Clone出来

		return S_OK;
Exit0:
		return E_FAIL;
	}
};
//////////////////////////////////////////////////////////////////////////
KG3DShaderManagerSolid::KG3DShaderManagerSolid( KG3DSystemEventListener& List )
	:m_pEffectPool(NULL)
{
	bool bRet = List.RegisterReceiver(this);
	_ASSERTE(bRet);
	(void)bRet;
}
KG3DShaderManagerSolid::~KG3DShaderManagerSolid()
{
	Clear();
}

void __stdcall KG3DShaderManagerSolid::OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )
{
	switch(Message.m_uMessage)
	{
#if defined(DEBUG) | defined(_DEBUG)
	case KM_D3DDEVICE_CREATE:
		_ASSERTE(! m_bitset.Is<em_is_device_release_message_received>());
		break;
#endif
	case KM_D3DDEVICE_LOST:
		for (TypeCt::iterator it = m_Effects.begin(); it != m_Effects.end(); ++it)
		{
			KG3DShaderBuildStruct& BS = *it;
			LPD3DXEFFECT p = BS.pEffect;
			if (NULL != p)
			{
				if(NULL != BS.pEventHandler)
					BS.pEventHandler->OnLostDevice(p);

				p->OnLostDevice();
			}
		}
		break;
	case KM_D3DDEVICE_RESET:
		for (TypeCt::iterator it = m_Effects.begin(); it != m_Effects.end(); ++it)
		{
			KG3DShaderBuildStruct& BS = *it;
			LPD3DXEFFECT p = BS.pEffect;
			if (NULL != p)
			{
				p->OnResetDevice();

				if(NULL != BS.pEventHandler)
					BS.pEventHandler->OnResetDevice(p);	
			}
		}
		break;
	case KM_D3DDEVICE_RELEASE:
		{
			m_bitset.SetTrue<em_is_device_release_message_received>();
			Clear();
		}
		break;
	default:
		break;
	}
}

HRESULT KG3DShaderManagerSolid::Reload( KG3DShaderPtrHolder& shaderPtrHolder )
{
	HRESULT hr = E_FAIL;
	KG3DShaderPtrHolderSolid* pHolderSolid = dynamic_cast<KG3DShaderPtrHolderSolid*>(&shaderPtrHolder);
	KG_PROCESS_ERROR(NULL != pHolderSolid);
	{
		_ASSERTE(NULL != pHolderSolid->m_lpShaderBuildStruct);
		
		hr = ReloadEffect(*pHolderSolid->m_lpShaderBuildStruct, NULL);
	}
Exit0:
	return hr;	
}

HRESULT KG3DShaderManagerSolid::ReloadWithMacroModification( KG3DShaderPtrHolder& shaderPtrHolder, D3DXMACRO_ARRAY macroArrayFull)
{
	KG3DShaderPtrHolderSolid* pHolderSolid = dynamic_cast<KG3DShaderPtrHolderSolid*>(&shaderPtrHolder);
	KG_PROCESS_ERROR(NULL != pHolderSolid);
	{
		_ASSERTE(NULL != pHolderSolid->m_lpShaderBuildStruct);

		return ReloadEffect(*pHolderSolid->m_lpShaderBuildStruct, &macroArrayFull);
	}
Exit0:
	return E_FAIL;
}

HRESULT KG3DShaderManagerSolid::ReloadEffect(KG3DShaderBuildStruct& Struct, D3DXMACRO_ARRAY* pNewMacroArray)
{
	HRESULT hr = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	LPD3DXEFFECT	pEffectCreated = NULL;

#if defined(DEBUG) | defined(_DEBUG)
	ULONG uOldRef = Struct.uRef;
#endif

	std::vector<D3DXMACRO> macroBuffer;
	KG_PROCESS_ERROR(NULL != Struct.pDevice);	
	
	hr = EffectCreationCore(Struct
				, NULL == pNewMacroArray ? Struct.macroDef.ToD3DXMACRO_ARRAY(macroBuffer) : *pNewMacroArray
				, &pEffectCreated, &Struct.strErrorString);

	if (FAILED(hr))
	{
		this->OutputError(Struct.strFilePathOrResourceID, Struct.strErrorString);
		goto Exit0;
	}

	{
		if (NULL != pNewMacroArray)
		{
			Struct.macroDef.FromD3DXMACRO_ARRAY(*pNewMacroArray);//改了之后要重新保存
		}

		if (NULL != Struct.pEffect)
		{
			_ASSERTE(1 == KGGetRef(Struct.pEffect));
			KGReleaseUntillDestructed(Struct.pEffect);
		}

		Struct.pEffect = pEffectCreated;
		pEffectCreated = NULL;

		_ASSERTE(1 == KGGetRef(Struct.pEffect));

        if (Struct.vecEffectParamHandleBuffer.size() > 0)
        {
            memset(&Struct.vecEffectParamHandleBuffer[0], 0, Struct.vecEffectParamHandleBuffer.size());//清空里面的Handle，这样可以让外面重新取
        }
		
		if (NULL != Struct.pEventHandler)
			Struct.pEventHandler->OnReload(Struct.pEffect);
	}
	hRetCode = S_OK;
Exit0:
	_ASSERTE(uOldRef == Struct.uRef);
	SAFE_RELEASE(pEffectCreated);
	return hRetCode;
}

HRESULT KG3DShaderManagerSolid::ReloadAll()
{
	for (TypeCt::iterator it = m_Effects.begin(); it != m_Effects.end(); ++it)
	{
		KG3DShaderBuildStruct& Ref = *it;
#if defined(DEBUG) | defined(_DEBUG)
		LPVOID pEffectPointerOld = reinterpret_cast<LPVOID>(Ref.pEffect);
#endif
		HRESULT hr = ReloadEffect(Ref, NULL);
#if defined(DEBUG) | defined(_DEBUG)
		if (FAILED(hr))
		{
			_ASSERTE(pEffectPointerOld == reinterpret_cast<LPVOID>(Ref.pEffect) 
				&& _T("重加载失败的时候，原来的Effects会被保留"));
		}
#endif
		(void)hr;
	}
	return S_OK;
}

std::tstring ParseIncludePathFromFilePath(LPCTSTR FilePath)
{
	LPCTSTR pLastPath = _tcsrchr(FilePath, _T('/'));
	return pLastPath == NULL ? _T("") : std::tstring(FilePath, pLastPath - FilePath);
}

KG3DShaderPtrHolder* KG3DShaderManagerSolid::CreateShaderHolder( LPDIRECT3DDEVICE9 pDevice
												 , LPCTSTR pFilePathOrIDString
												 , D3DXMACRO_ARRAY macroArray
												 , LPCTSTR pIncludeFilePath
												 , DWORD Flags
                                                 , size_t uShaderParamHandleSize
												 , BOOL bUseEffectPool
												 , std::tstring* pErrorString
												 , const KG3DShaderEventHandler* pEventHandler)
{
	LPD3DXEFFECT	pEffectCreated = NULL;
	KG3DShaderPtrHolder* pShaderPtrHolderCreated = NULL;
	KG_PROCESS_ERROR(NULL != pFilePathOrIDString && NULL != pDevice);
	{
		std::tstring filePathOrIDString = pFilePathOrIDString;

#if defined(DEBUG) | defined(_DEBUG)
		_ASSERTE(m_Effects.size() <= em_max_effect);
		_ASSERTE(macroArray.uMacroCount <= em_max_macro_def);
		if (macroArray.uMacroCount > 0)
		{
			const D3DXMACRO& lastMacro = macroArray.pMacroDefs[macroArray.uMacroCount - 1];
			_ASSERTE(NULL == lastMacro.Name && NULL == lastMacro.Definition && _T("d3d要求最后是要以NULL,NULL结尾的"));
		}
#endif
		KG_PROCESS_ERROR(macroArray.uMacroCount <= em_max_macro_def);

		BOOL bIsDiskFileOrResourceFile = ! this->IsResourceString(filePathOrIDString.c_str());
		if(bIsDiskFileOrResourceFile)
		{
			KGCH::StrPathToLowerAndReplaceSlash(filePathOrIDString);
		}

		try
		{
			TypeStringEffectMap::iterator it = m_StringEffectMap.find(filePathOrIDString);
			if (it == m_StringEffectMap.end())
			{
				KG3DShaderBuildStruct Struct;
				Struct.pDevice = pDevice;
				Struct.pEffect = NULL;
				Struct.uRef = 0;
				Struct.strFilePathOrResourceID = filePathOrIDString;

				BOOL bRetTemp = Struct.macroDef.FromD3DXMACRO_ARRAY(macroArray);
				KG_PROCESS_ERROR(bRetTemp);

				Struct.dwFlags = Flags;
				//如果取默认，即Include文件在Shader文件同目录下查找，那么必须分析出那个目录
				//不然打包之后没有办法找到文件
				if (bIsDiskFileOrResourceFile)
				{
					Struct.strIncludeFilePath = (NULL == pIncludeFilePath) ? 
						ParseIncludePathFromFilePath(filePathOrIDString.c_str()) 
						: pIncludeFilePath;
					KGCH::StrPathToLowerAndReplaceSlash(Struct.strIncludeFilePath);
				}
				else
				{
					Struct.strIncludeFilePath = _T("");	//Resource文件不能使用IncludePath，只能是Resource中已经打包的文件
				}
				
				Struct.bUseEffectPool = bUseEffectPool;
				Struct.pEventHandler = (pEventHandler == NULL) ? NULL : pEventHandler->Clone();

				std::vector<D3DXMACRO> macroBuffer;
				HRESULT hr = this->EffectCreationCore(Struct, Struct.macroDef.ToD3DXMACRO_ARRAY(macroBuffer)
									, &pEffectCreated, &Struct.strErrorString);
				if (SUCCEEDED(hr))
				{
					Struct.pEffect = pEffectCreated;
					pEffectCreated = NULL;

					_ASSERTE(NULL != Struct.pEffect);
				}
				else
				{
					_ASSERTE(NULL == pEffectCreated);
					this->OutputError(Struct.strFilePathOrResourceID, Struct.strErrorString);					
					Struct.pEffect = NULL;	//创建失败，但是也记录下来
					pEffectCreated = NULL;
				}
                
				pShaderPtrHolderCreated = AddStruct(Struct, uShaderParamHandleSize);
				_ASSERTE(NULL != pShaderPtrHolderCreated && _T("唯一可能失败的情况是内存用完，不然一定创建成功"));
				//this->GarbageCollection();
			}
			else	//已经存在的文件，从现成的shader中加引用传出去
			{
				TypeCt::iterator itCt = it->second;
				KG3DShaderBuildStruct& StructRef = *itCt;
#if defined(DEBUG) | defined(_DEBUG)
				ULONG uOldRef = StructRef.uRef;
#endif

				StructRef.uGarbageWeight = 0;

				pShaderPtrHolderCreated = BuildShaderHolder(StructRef);
				_ASSERTE(StructRef.uRef == uOldRef + 1);
				_ASSERTE(NULL != pShaderPtrHolderCreated && _T("唯一可能失败的情况是内存用完，不然一定创建成功"));
			}
			return pShaderPtrHolderCreated;
		}
		catch (...)
		{
			
		}
	}
Exit0:
	SAFE_RELEASE(pShaderPtrHolderCreated);
	SAFE_RELEASE(pEffectCreated);
	return NULL;
}

HRESULT KG3DShaderManagerSolid::Find( LPCTSTR strInput, KG3DShaderHolder& retHolder )
{
	KG_PROCESS_ERROR(NULL != strInput);
	{
		std::tstring strInputPath = strInput;
		KGCH::StrPathToLowerAndReplaceSlash(strInputPath);		
		
		for (TypeStringEffectMap::iterator it = m_StringEffectMap.begin(); it != m_StringEffectMap.end(); ++it)
		{
			const std::tstring& strPath = it->first;
			if(NULL != _tcsstr(strPath.c_str(),strInputPath.c_str()))
			{
				TypeCt::iterator itCt = it->second;
				KG3DShaderBuildStruct& StructRef = *itCt;

				KG3DShaderPtrHolder* pHolder = BuildShaderHolder(StructRef);

				_ASSERTE(KGGetRef(pHolder) == 1);

				retHolder = KG3DShaderHolder(pHolder);

				_ASSERTE(KGGetRef(pHolder) == 2);
				SAFE_RELEASE(pHolder);

				_ASSERTE(NULL != retHolder.GetShaderPtrHolder());
				return S_OK;
			}
		}
	}
Exit0:
	return E_FAIL;
}
HRESULT KG3DShaderManagerSolid::EffectCreationCore(const KG3DShaderBuildStruct& Struct, D3DXMACRO_ARRAY macroArray
												   , LPD3DXEFFECT* ppEffectPtrOut, std::tstring* pErrorString)
{
	LPD3DXBUFFER	pBuffer = NULL;
	LPD3DXEFFECT	pEffect = NULL;
	IKG3DShaderLibraryResource* pRCResouce = NULL;
	HRESULT hr = E_FAIL;

	_ASSERTE(NULL != Struct.pDevice && NULL != ppEffectPtrOut && NULL != pErrorString);
	{
		_ASSERTE(D3DXMACRO_ARRAY::IsValid(macroArray));

		LPD3DXEFFECTPOOL pPool = (Struct.bUseEffectPool ? this->GetEffectPool(Struct.pDevice) : NULL);

		LPDIRECT3DDEVICE9	pDevice = const_cast<LPDIRECT3DDEVICE9>(Struct.pDevice);

		std::tstring strResourceString;
		BOOL bIsResource = this->GetPathFromResourceString(Struct.strFilePathOrResourceID.c_str(), &strResourceString);
		
		if (! bIsResource)	//即里面存放的是一个真正的路径，使用路径加载
		{
			KG3DShaderInclude ShaderInclude(Struct.strIncludeFilePath.c_str());	
			hr = KG3DD3DXCreateEffectFromFile(pDevice, Struct.strFilePathOrResourceID.c_str()
				, macroArray.pMacroDefs, &ShaderInclude, Struct.dwFlags, pPool
				, &pEffect, &pBuffer);	//里面用File.h里面的系列函数处理了Shader文件打包等问题，所以不用管打包
		}
		else	//否则，是个资源ID字符串，转换为资源ID，内存加载
		{
			/*注意这个ID虽然的确是shaderLibrary里面的资源ID，但不是真正的可执行文件资源ID。这个
			需要通过shaderLibrary的导出函数来使用。如果是真正的资源ID，需要考虑偏移和Security。
			不要试图直接使用LoadResource之类的函数。
			*/
			KG3DShaderIncludeForRC shaderIncludeForRC;

			hr = KG3DShaderRCBridge::GetShaderLibraryInterface()->GetShaderResourceA(strResourceString.c_str(), &pRCResouce);
			KG_COM_PROCESS_ERROR(hr);

			_ASSERTE(NULL != pRCResouce && NULL != pRCResouce->GetBuffer());
			hr = D3DXCreateEffect(pDevice, pRCResouce->GetBuffer(), pRCResouce->GetBufferSize()
				, macroArray.pMacroDefs, &shaderIncludeForRC, Struct.dwFlags, pPool
				, &pEffect, &pBuffer);	//里面用File.h里面的系列函数处理了Shader文件打包等问题，所以不用管打包
			
		}

		if (FAILED(hr))
		{
			if(NULL != pBuffer)
			{
				size_t BufferSize = (size_t)(pBuffer->GetBufferSize());
				(*pErrorString).assign(reinterpret_cast<LPCTSTR>(pBuffer->GetBufferPointer()), BufferSize);
			}
			else//文件加载不正确的时候，会FAILED但没有pBuffer
			{
				*pErrorString = _T("By kg3dengine: File incorrect or pack file incorrect, read failed");
			}
			goto Exit0;
		}
		SAFE_RELEASE(pRCResouce);
		SAFE_RELEASE(pBuffer);
		*ppEffectPtrOut = pEffect;
		*pErrorString = _T("");
		return S_OK;
	}
Exit0:
	SAFE_RELEASE(pRCResouce);
	SAFE_RELEASE(pEffect);
	SAFE_RELEASE(pBuffer);
	return E_FAIL;
}

void KG3DShaderManagerSolid::Clear()
{

	for (TypeCt::iterator it = m_Effects.begin(); it != m_Effects.end(); ++it)
	{
		KG3DShaderBuildStruct& buildStructRef = *it;
		KGReleaseUntillDestructed(buildStructRef.pEffect);
		buildStructRef.pEffect = NULL;
		buildStructRef.pDevice = NULL;
	}

	//不能释放m_Effects的内存，外部的指针还指在那呢

	SAFE_RELEASE(m_pEffectPool);
}

KG3DShaderPtrHolder* KG3DShaderManagerSolid::BuildShaderHolder(KG3DShaderBuildStruct& buildStruct)
{
	return new KG3DShaderPtrHolderSolid(buildStruct);
}

bool KG3DShaderManagerSolid::IsTimeToCollectGarbage()
{
	static time_t lastTime = ::time(NULL);
	time_t currentTime = ::time(NULL);
	time_t timeGap = currentTime - lastTime;
	lastTime = currentTime;
	return timeGap > em_min_garbage_collection_time_gap;
}
void KG3DShaderManagerSolid::GarbageCollection()
{
	if(! IsTimeToCollectGarbage())
		return;

	for (TypeCt::iterator it = m_Effects.begin(); it != m_Effects.end();)
	{
		KG3DShaderBuildStruct& Ref = *it;
		
		if (Ref.uRef > 1)
		{
			++it;
			continue;
		}
		++Ref.uGarbageWeight;
		//只有ShaderManager拥有这个Effect，表明外部都已经释放了
		if(Ref.uGarbageWeight <= em_max_garbage_weight)
		{
			++it;
			continue;
		}
		TypeStringEffectMap::iterator itStringEffectMap = m_StringEffectMap.find(Ref.strFilePathOrResourceID);
		_ASSERTE(itStringEffectMap != m_StringEffectMap.end());
		m_StringEffectMap.erase(itStringEffectMap);

		_ASSERTE(1 == Ref.uRef && 1== KGGetRef(Ref.pEffect));
		SAFE_RELEASE(Ref.pEffect);

		it = m_Effects.erase(it);
	}
}

HRESULT KG3DShaderManagerSolid::GetShaderBuildParams( KG3DShaderHolder& shaderHolder, KG3DShaderBuildParams* pBuildParams )
{
	KG3DShaderPtrHolderSolid* pHolderSolid = dynamic_cast<KG3DShaderPtrHolderSolid*>(shaderHolder.GetShaderPtrHolder());
	KG_PROCESS_ERROR(NULL != pHolderSolid);
	KG_PROCESS_ERROR(NULL != pBuildParams);
	{
		_ASSERTE(NULL != pHolderSolid->m_lpShaderBuildStruct);

		//内部的BuildStruct不能直接使用BuildParams那种参数，因为那种参数太灵活了，字符串和指针这样子随便放都是很危险的
		//所以内部的BuildStruct不会以BuildParams的形式保存，他们之间还是需要一个转换

		return ConvertKG3DShaderBuildStructToParams(*pHolderSolid->m_lpShaderBuildStruct, *pBuildParams);
	}
Exit0:
	return E_FAIL;
}


HRESULT KG3DShaderManagerSolid::SetShaderEventHandler( KG3DShaderHolder& shaderHolder, KG3DShaderEventHandler* pEventHandler )
{
	KG3DShaderEventHandler* pNewHandler = NULL;
	KG3DShaderPtrHolderSolid* pHolderSolid = dynamic_cast<KG3DShaderPtrHolderSolid*>(shaderHolder.GetShaderPtrHolder());
	KG_PROCESS_ERROR(NULL != pHolderSolid);
	KG_PROCESS_ERROR(NULL != pEventHandler);
	{
		_ASSERTE(NULL != pHolderSolid->m_lpShaderBuildStruct);

		pNewHandler = pEventHandler->Clone();
		KG_PROCESS_ERROR(NULL != pNewHandler);

		KG3DShaderBuildStruct& buildStruct = *pHolderSolid->m_lpShaderBuildStruct;
		SAFE_RELEASE(buildStruct.pEventHandler);

		buildStruct.pEventHandler = pNewHandler;
		
		return S_OK;	
	}
Exit0:
	SAFE_RELEASE(pNewHandler);
	return E_FAIL;
}

VOID KG3DShaderManagerSolid::OutputError( const std::tstring& filePathOrID, const std::tstring& ErrorString )
{
	BOOL bIsResouce = this->IsResourceString(filePathOrID.c_str());
	if (! bIsResouce)
	{
		KGLogPrintf(KGLOG_WARNING, "以下Shader文件加载不成功：%s, 错误为：%s"
			, filePathOrID.c_str(), ErrorString.c_str());
	}
	else
	{
		KGLogPrintf(KGLOG_WARNING, "以下Resource中的Shader文件加载不成功, ID：%u, Path: %s, 错误为：%s"
			, filePathOrID.c_str(), filePathOrID.c_str(), ErrorString.c_str());
	}

	//在Debug模式下面同时输出到Output窗口
#if defined(DEBUG) | defined(_DEBUG)
	_KG3D_DEBUG_OUTPUT2("以下Shader文件加载不成功：%s, 错误为：%s", filePathOrID.c_str(), ErrorString.c_str());
#endif
}

KG3DShaderHolder KG3DShaderManagerSolid::Create( LPDIRECT3DDEVICE9 pDevice, LPCTSTR pFilePathOrResourceIDString
												, D3DXMACRO_ARRAY macroArray /*= D3DXMACRO_ARRAY::GetDefault()*/
												, LPCTSTR pIncludeFilePath /*= NULL*/
												, DWORD Flags /*= 0*/
                                                , size_t uShaderParamHandleSize /* = 0*/
                                                , BOOL bUseEffectPool /*= FALSE*/
												, std::tstring* pErrorString /*= NULL*/
												, const KG3DShaderEventHandler* pEventHandler /*= NULL*/)
{
	KG3DShaderPtrHolder* p = this->CreateShaderHolder(pDevice, pFilePathOrResourceIDString, macroArray, pIncludeFilePath
		, Flags, uShaderParamHandleSize, bUseEffectPool, pErrorString, pEventHandler);
	KG3DShaderHolder Ret(p);
	SAFE_RELEASE(p);
	return Ret;
}

LPD3DXEFFECTPOOL KG3DShaderManagerSolid::GetEffectPool(LPDIRECT3DDEVICE9 pDevice)
{
	_ASSERTE(NULL != pDevice);

	static bool s_bIsEffectPoolAbleToBeCreate = true;

	KG_PROCESS_ERROR(NULL != pDevice);
	if (s_bIsEffectPoolAbleToBeCreate && NULL == m_pEffectPool)
	{
		HRESULT hr = D3DXCreateEffectPool(&m_pEffectPool);
		if (FAILED(hr))
		{
			s_bIsEffectPoolAbleToBeCreate = false;	//避免重复尝试创建EffectPool
		}
		KG_COM_PROCESS_ERROR(hr);
	}
Exit0:
	return m_pEffectPool;
}

KG3DShaderPtrHolder* KG3DShaderManagerSolid::AddStruct( const KG3DShaderBuildStruct &Struct, size_t uShaderParamHandleSize )
{
	m_Effects.push_back(Struct);

	//从插入队列中把对象重新找出来
	_ASSERTE(m_Effects.size() > 0);
	TypeCt::iterator itCt = m_Effects.end();
	--itCt;
	KG3DShaderBuildStruct& InsertedStructRef = *itCt;	//下面要再次用到这个it，所以这里不用back方法
	++InsertedStructRef.uRef;

	_ASSERTE(InsertedStructRef.pEffect == Struct.pEffect && _T("应该是push_back那个Struct"));

	InsertedStructRef.vecEffectParamHandleBuffer.clear();
	if (uShaderParamHandleSize > 0)
	{
		InsertedStructRef.vecEffectParamHandleBuffer.resize(uShaderParamHandleSize, 0);
	}						
	
	KG3DShaderPtrHolder* pRet = BuildShaderHolder(InsertedStructRef);
	if (NULL == pRet)
	{
		m_Effects.pop_back();
		return NULL;
	}

	_ASSERTE(2 == InsertedStructRef.uRef);
	m_StringEffectMap[Struct.strFilePathOrResourceID] = itCt;

	return pRet;
}

namespace Private
{
	struct KG3DShaderManagerDummyEffect : public KGDummyID3DXEffectRefImp
	{
	};

	struct  SingletonDummyEffect : public Private::KG3DShaderManagerDummyEffect
	{
		virtual ULONG STDMETHODCALLTYPE AddRef(){return 1;}
		virtual ULONG STDMETHODCALLTYPE Release(){return 0;}
	};
	
};

HRESULT Private::ReloadShaderHolder( KG3DShaderPtrHolder& Holder )
{
	return g_GetShaderManager().Reload(Holder);
}

bool Private::IsShaderHolderInited( KG3DShaderHolder& Holder )
{
	return Holder.GetShaderPtrHolder() != NULL;	//不要改变语义，PtrHolder的GetEffect可能失败，这样的原因是Effect加载失败了，但是Holder的确已经被初始化了。
}

bool Private::IsDummyEffect( LPD3DXEFFECT p)
{
	_ASSERTE(NULL != p);

	bool bRet =false;
		
	try
	{
		bRet = NULL != dynamic_cast<Private::KG3DShaderManagerDummyEffect*>(p);	//如果Application Verifier 在这里当下来，请忽略掉。这不是一个错误
		return bRet;
	}
	catch (...)
	{
		
	}
	return false;	
}

//LPD3DXEFFECT Private::CreateDummyEffect()
//{
//	return new Private::KG3DShaderManagerDummyEffect();//带引用计数
//}



static Private::SingletonDummyEffect stSingletonDummyEffect;
/*引用计数无效，不需要释放。不要放别的地方。ShaderManager是一种Meyers Singleton，会用到系统函数atexit，如果这里也用
Meyers Singleton的手法，即在Get函数里面返回一个函数内的static变量，这样出事的可能性大。因为
atexit虽然说保证LIFO，但是谁也看不见这个保证，C++标准倒是有，但是编译器通常会忽略其中一些比较
麻烦的部分，而atexit的说明是属于增补说明的。所以如果stEffect是cpp内的static,而ShaderManager是
Meyers Singleton的话，基本能保证不会出现stEffect先于ShaderManager析构的现象。参考《c++设计新思维》
里面关于Singleton的论述。
*/

static KG3DShaderManagerSolid stSM(g_GetSystemEventListener());	//注意这些内存是程序最后的最后才会被释放的，所以能保证ShaderHolder里面的Effect指针的引用有效。而且这里保证了stSingletonDummyEffect的生命周期比较长。Listener是生命周期无关的，会自动反耦合，所以不用理它

LPD3DXEFFECT Private::GetSingletonDummyEffect()
{
	_ASSERTE(KGIsVirtualPointerValid(&stSingletonDummyEffect));
	return &stSingletonDummyEffect;
}

KG3DShaderManager& g_GetShaderManager()
{
	_ASSERTE(KGIsVirtualPointerValid(&stSM));//虚指针不为空
	return stSM;
}
#define RESOURCE_STRING_HEAD	_T("<>")	//这些都是在路径中绝对不会出现的字符，所以可以用于区分路径和资源
std::tstring KG3DShaderManager::GetResourceString(LPCTSTR strPath)
{
	_ASSERTE(NULL != strPath);
	std::tstring strRet = RESOURCE_STRING_HEAD;
	strRet += strPath;
	return strRet;
}

BOOL KG3DShaderManager::GetPathFromResourceString( LPCTSTR strResourceString, std::tstring* pRet)
{
	_ASSERTE(NULL != strResourceString);
	
	if (IsResourceString(strResourceString))
	{
		if (NULL != pRet)
		{
			 *pRet = (strResourceString + (_countof(RESOURCE_STRING_HEAD) - 1));
		}
		return TRUE;
	}
	return FALSE;
}

BOOL KG3DShaderManager::IsResourceString( LPCTSTR strIn )
{
	int nRet = _tcsncmp(strIn, RESOURCE_STRING_HEAD, _countof(RESOURCE_STRING_HEAD) - 1);
	return 0 == nRet;
}

#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DShaderManager)
{
	LPCTSTR strSrc = _T("data/public/source/a.fx");
	LPCTSTR strDest = _T("<>data/public/source/a.fx");

	{
		std::tstring strRet1 = KG3DShaderManager::GetResourceString(strSrc);
		_ASSERTE(strRet1 == strDest);
	}

	{
		std::tstring strRet2;
		BOOL bRet = KG3DShaderManager::GetPathFromResourceString(strDest, &strRet2);
		_ASSERTE(bRet && 0 == _tcscmp(strRet2.c_str(), strSrc));

		_ASSERTE(! KG3DShaderManager::GetPathFromResourceString(strSrc, &strRet2));
	}
}
KG_TEST_END(KG3DShaderManager)
#endif

#if 0
#if defined(KG_ENABLE_TDD)
KG_TEST_CLASS_TEST_BEGIN(KG3DShaderManagerSolid)
{
	static ULONG stEffectInstanceCount = 0; 
	static ULONG stShaderHolderPtrInstanceCount = 0;
	static ULONG  s_ulTotalEffectRef = 0;
	static ULONG  stDeviceLostObj = 0;
	static ULONG  stDeviceResetObj = 0;
	static bool	  stForceGarbageCollection = false;
	static TCHAR  stErrorFile[MAX_PATH] = _T("");
	static TCHAR  stErrorString[MAX_PATH] = _T("");

	struct TestEffect : public KGDummyID3DXEffectRefImp
	{
		TestEffect()
        {
            KG_InterlockedIncrement((long *)&s_ulTotalEffectRef);
            ++stEffectInstanceCount;
        }
		~TestEffect()
        {
            --stEffectInstanceCount;
        }
		STDMETHOD_(ULONG, AddRef)(THIS)
        {
            KG_InterlockedIncrement((long *)&s_ulTotalEffectRef);
            return KGDummyID3DXEffectRefImp::AddRef();
        }
		STDMETHOD_(ULONG, Release)(THIS)
        {
            KG_InterlockedDecrement((long *)&s_ulTotalEffectRef);
            return KGDummyID3DXEffectRefImp::Release();
        }
		STDMETHOD(OnLostDevice)(THIS) {++stDeviceLostObj; return E_NOTIMPL;}
		STDMETHOD(OnResetDevice)(THIS) {++stDeviceResetObj; return E_NOTIMPL;}
	};
	struct KG3DShaderHolderSolidTest : public KG3DShaderPtrHolderSolid
	{
		KG3DShaderHolderSolidTest(KG3DShaderBuildStruct& buildStruct)
			:KG3DShaderPtrHolderSolid(buildStruct)
		{
			++stShaderHolderPtrInstanceCount;
		}
		~KG3DShaderHolderSolidTest(){--stShaderHolderPtrInstanceCount;}
	};

	KG3DSystemEventListenerDummy DummyListener;
	struct ShaderMgrTest : public KG3DShaderManagerSolid 
	{
		BOOL m_bEffectCreationCoreShouldSucceed;
		ShaderMgrTest(KG3DSystemEventListener& List):KG3DShaderManagerSolid(List),m_bEffectCreationCoreShouldSucceed(TRUE){}
		virtual HRESULT EffectCreationCore(const KG3DShaderBuildStruct& Struct, D3DXMACRO_ARRAY macroArray
										, LPD3DXEFFECT* ppEffectCreated, std::tstring* pErrorString)
		{
			if(m_bEffectCreationCoreShouldSucceed)
			{
				*ppEffectCreated = new TestEffect;
				return S_OK;
			}
			*ppEffectCreated = NULL;
			*pErrorString = _T("A Test Error");
			return E_FAIL;
		}
		virtual KG3DShaderPtrHolder* BuildShaderHolder(KG3DShaderBuildStruct& buildStruct)
		{
			return new KG3DShaderHolderSolidTest(buildStruct);
		}
		virtual bool IsTimeToCollectGarbage()
		{
			if(stForceGarbageCollection)
				return true;
			return KG3DShaderManagerSolid::IsTimeToCollectGarbage();
		}
		virtual VOID OutputError(const std::tstring& filePathOrID, const std::tstring& ErrorString)
		{
			_tcscpy_s(stErrorFile, filePathOrID.c_str());
			_tcscpy_s(stErrorString, ErrorString.c_str());
		}
		
	};

	BOOL bInitSingletonDummy = FALSE;
	if (! KGIsVirtualPointerValid(&stSingletonDummyEffect))
	{
		stSingletonDummyEffect.Private::SingletonDummyEffect::SingletonDummyEffect();//这个时候这个Effect是没被初始化的，这里用个野蛮的手法,VC特殊的显式构造函数语法(用于控制静态成员生命周期)。反正是测试程序，以后会放到另外一个工程里面的。
		bInitSingletonDummy = TRUE;
	}
	

	//////////////////////////////////////////////////////////////////////////

	KG3DShaderBuildStruct bs;
	KG3DShaderMacroDef md;
	md.macroDefs.push_back(std::make_pair(std::tstring(_T("Macro")),  std::tstring(_T("MacroDef"))));
	bs.pDevice = (LPDIRECT3DDEVICE9)0x00000001;
	bs.strFilePathOrResourceID = _T("Data/public/Shader.fx");
	TCHAR bsShaderName[] = _T("Shader");
	bs.macroDef = md;
	bs.strIncludeFilePath =  _T("Data/public");
	bs.dwFlags = 0x00000002;
	bs.bUseEffectPool = TRUE;
	bs.strErrorString = _T("");	
	bs.vecEffectParamHandleBuffer.resize(10, 0);

	KG3DSystemEventListenerDummy	DummyLst;

	std::vector<D3DXMACRO>	macroBuffer;

	//KG3DShaderBuildStruct
	{
		KG3DShaderBuildStruct bs2 = bs;
		_ASSERTE(bs2 == bs);
	}
	{
		TCHAR pathTemp[] = _T("data/public/shader.fx");
		std::tstring strTemp = ParseIncludePathFromFilePath(pathTemp);
		_ASSERTE(0 == _tcscmp(strTemp.c_str(), _T("data/public")));
	}
	//错误的创建，会写Log
	{
		_ASSERTE(0 == stEffectInstanceCount && 0 == stShaderHolderPtrInstanceCount && 0 == s_ulTotalEffectRef);
		ShaderMgrTest Mgr(DummyLst);
		_ASSERTE(Mgr.m_bEffectCreationCoreShouldSucceed);
		Mgr.m_bEffectCreationCoreShouldSucceed = FALSE;
		KG3DShaderPtrHolder* p = Mgr.CreateShaderHolder(bs.pDevice, bs.strFilePathOrResourceID.c_str()
			,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str()
			, bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString, NULL);

		_ASSERTE(0 == _tcsicmp(stErrorFile, bs.strFilePathOrResourceID.c_str()) 
			&& 0 != _tcsicmp(stErrorString, bs.strFilePathOrResourceID.c_str()));
		_ASSERTE(NULL != p);
		_ASSERTE(NULL == p->GetEffect());
		SAFE_RELEASE(p);

		KG3DShaderHolder Holder =  Mgr.Create(bs.pDevice, bs.strFilePathOrResourceID.c_str()
			,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str()
			, bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString, NULL);
		_ASSERTE(NULL != Holder.GetShaderPtrHolder());
		_ASSERTE(NULL == Holder.GetShaderPtrHolder()->GetEffect());
		_ASSERTE(NULL != Holder.GetEffect());
	}
	//单一创建
	{
		_ASSERTE(0 == stEffectInstanceCount && 0 == stShaderHolderPtrInstanceCount && 0 == s_ulTotalEffectRef);
		ShaderMgrTest Mgr(DummyLst);
		_ASSERTE(0 == stEffectInstanceCount);
		KG3DShaderPtrHolder* p = Mgr.CreateShaderHolder(bs.pDevice, bs.strFilePathOrResourceID.c_str()
			,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str()
			, bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString, NULL);

		_ASSERTE(NULL != p);
		_ASSERTE(NULL != p->GetEffect());
		_ASSERTE(1 == KGGetRef(p) 
			&& 1 == KGGetRef(p->GetEffect())
			&& 1 == s_ulTotalEffectRef
			&& 1 == stEffectInstanceCount
			&& 1 == stShaderHolderPtrInstanceCount);

		_ASSERTE(2 == Mgr.m_Effects.back().uRef);

		_ASSERTE(dynamic_cast<TestEffect*>(p->GetEffect()) != NULL);

		LPD3DXEFFECT pEffect = p->GetEffect();

		KG3DShaderBuildStruct bsRet = bs;
		bsRet.pEffect = pEffect;
		bsRet.uRef = 2;
		bsRet.strIncludeFilePath = bs.strIncludeFilePath;
		_ASSERTE(Mgr.m_Effects.front() == bsRet);

		std::tstring lowerPath = bs.strFilePathOrResourceID;
		transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), tolower);
		_ASSERTE(Mgr.m_StringEffectMap.find(lowerPath) != Mgr.m_StringEffectMap.end());
		_ASSERTE(1 == Mgr.m_Effects.size());
		_ASSERTE(1 == s_ulTotalEffectRef);

		{
			size_t uHandleBufferSize = 0;
			LPVOID pParam = p->GetEffectParamHandleBuffer(&uHandleBufferSize);
			_ASSERTE(NULL != pParam);
			_ASSERTE(0 < Mgr.m_Effects.front().vecEffectParamHandleBuffer.size());
			_ASSERTE(pParam == (LPVOID)(&Mgr.m_Effects.front().vecEffectParamHandleBuffer[0]));
			_ASSERTE(uHandleBufferSize == bs.vecEffectParamHandleBuffer.size());
		}

		SAFE_RELEASE(p);
		_ASSERTE(1 == stEffectInstanceCount);//内部有引用，
		_ASSERTE(1 == s_ulTotalEffectRef);
	}
	//使用EventHandler创建,确认其引用计数的正确性
	{
		struct TestEvenHandler : public KG3DShaderEventHandler 
		{
			virtual KG3DShaderEventHandler* Clone()const{++m_uRef; return const_cast<TestEvenHandler*>(this);}
			virtual ULONG STDMETHODCALLTYPE Release(){--m_uRef; return 0;}
			mutable UINT m_uRef;
		};
		TestEvenHandler testEventHandler;
		testEventHandler.m_uRef = 1;
		
		{
			ShaderMgrTest Mgr(DummyLst);
			KG3DShaderPtrHolder* p = Mgr.CreateShaderHolder(bs.pDevice, bs.strFilePathOrResourceID.c_str()
				,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str(), bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString, &testEventHandler);

			_ASSERTE(NULL != p);
			_ASSERTE(2 == testEventHandler.m_uRef);

			KG3DShaderPtrHolder* p2 = Mgr.CreateShaderHolder(bs.pDevice, bs.strFilePathOrResourceID.c_str()
				,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str(), bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString, &testEventHandler);

			_ASSERTE(2 == testEventHandler.m_uRef);

			SAFE_RELEASE(p);
			SAFE_RELEASE(p2);
		}
		_ASSERTE(1 == testEventHandler.m_uRef);
	}
	//试验SetEventHandler
	{
		struct TestEvenHandler : public KG3DShaderEventHandler 
		{
			virtual KG3DShaderEventHandler* Clone()const{++m_uRef; return const_cast<TestEvenHandler*>(this);}
			virtual ULONG STDMETHODCALLTYPE Release(){--m_uRef; return 0;}
			mutable UINT m_uRef;
		};
		TestEvenHandler testEventHandler;
		testEventHandler.m_uRef = 1;

		struct TestEvenHandler2 : public KG3DShaderEventHandler 
		{
			virtual KG3DShaderEventHandler* Clone()const{++m_uRef; return const_cast<TestEvenHandler2*>(this);}
			virtual ULONG STDMETHODCALLTYPE Release(){--m_uRef; return 0;}
			mutable UINT m_uRef;
		};
		TestEvenHandler2 testEventHandler2;
		testEventHandler2.m_uRef = 1;

		{
			ShaderMgrTest Mgr(DummyLst);
			KG3DShaderHolder holder = Mgr.Create(bs.pDevice, bs.strFilePathOrResourceID.c_str()
				,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str(), bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString, &testEventHandler);

			_ASSERTE(2 == testEventHandler.m_uRef);

			Mgr.SetShaderEventHandler(holder, &testEventHandler2);
			_ASSERTE(1 == testEventHandler.m_uRef);
			_ASSERTE(2 == testEventHandler2.m_uRef);			
		}
		_ASSERTE(1 == testEventHandler2.m_uRef);
	}
	//用没有设定任何参数的简单形式再创建一次
	{
		_ASSERTE(0 == stEffectInstanceCount && 0 == stShaderHolderPtrInstanceCount && 0 == s_ulTotalEffectRef);
		ShaderMgrTest Mgr(DummyLst);
		_ASSERTE(0 == stEffectInstanceCount);

		KG3DShaderHolder tempHolder = Mgr.Create(bs.pDevice, bs.strFilePathOrResourceID.c_str());
		_ASSERTE(Private::IsShaderHolderInited(tempHolder));
		KG3DShaderPtrHolder* p = tempHolder.GetShaderPtrHolder();
		_ASSERTE(1 == KGGetRef(p) 
			&& 1 == KGGetRef(p->GetEffect())
			&& 1 == stEffectInstanceCount
			&& 1 == stShaderHolderPtrInstanceCount);
		_ASSERTE(2 == Mgr.m_Effects.back().uRef);

		LPD3DXEFFECT pEffect = tempHolder.GetEffect();

		KG3DShaderBuildStruct bsRet;
		bsRet.pEffect = pEffect;
		bsRet.pDevice = bs.pDevice;
		bsRet.strFilePathOrResourceID = bs.strFilePathOrResourceID;
		bsRet.strIncludeFilePath = bs.strIncludeFilePath;

	
		bsRet.strErrorString = _T("");
		bsRet.vecEffectParamHandleBuffer.clear();
		bsRet.uRef = 2;
		
		_ASSERTE(Mgr.m_Effects.front() == bsRet);
	}
	//用同一路径创建两次，应该得出一样的结果
	{
		_ASSERTE(0 == stEffectInstanceCount && 0 == stShaderHolderPtrInstanceCount && 0 == s_ulTotalEffectRef);
		ShaderMgrTest Mgr(DummyLst);
		_ASSERTE(0 == stEffectInstanceCount);
		KG3DShaderPtrHolder* p1 = Mgr.CreateShaderHolder(bs.pDevice, bs.strFilePathOrResourceID.c_str()
			,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str()
			, bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString, NULL);
		_ASSERTE(Mgr.m_Effects.size() == 1 && Mgr.m_StringEffectMap.size() == 1);


		KG3DShaderBuildStruct bsP1 = Mgr.m_Effects.front();		

		KG3DShaderPtrHolder* p2 = Mgr.CreateShaderHolder(bs.pDevice, bs.strFilePathOrResourceID.c_str()
			,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str()
			, bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString, NULL);
		_ASSERTE(p1->GetEffect() == p2->GetEffect());
		_ASSERTE(KGGetRef(p1->GetEffect()) == 1);

		_ASSERTE(3 == Mgr.m_Effects.back().uRef);
		bsP1.uRef = 3;
		_ASSERTE(bsP1 == Mgr.m_Effects.front());
		
		_ASSERTE(1 == stEffectInstanceCount );
		_ASSERTE(1 == s_ulTotalEffectRef);

		SAFE_RELEASE(p1);
		SAFE_RELEASE(p2);
		_ASSERTE(1 == stEffectInstanceCount);
	}
	//GetShaderBuildParams	里面使用了ConvertKG3DShaderBuildStructToParams，也同时验证了这个函数
	{
		_ASSERTE(0 == stEffectInstanceCount && 0 == stShaderHolderPtrInstanceCount && 0 == s_ulTotalEffectRef);
		ShaderMgrTest Mgr(DummyLst);
		_ASSERTE(0 == stEffectInstanceCount);
		KG3DShaderHolder holderTmp = Mgr.Create(bs.pDevice, bs.strFilePathOrResourceID.c_str()
			,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str()
			, bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString);

		KG3DShaderBuildParams buildParam;
		HRESULT hr = Mgr.GetShaderBuildParams(holderTmp, &buildParam);
		_ASSERTE(SUCCEEDED(hr));

		
		_ASSERTE(0 == _tcsicmp(buildParam.strFilePathOrResourceIDString, bs.strFilePathOrResourceID.c_str()));
		_ASSERTE(2 == buildParam.macroArray.size());
		_ASSERTE(0 == _tcsicmp(buildParam.macroArray[0].Name, bs.macroDef.macroDefs[0].first.c_str()));
		_ASSERTE(0 == _tcsicmp(buildParam.macroArray[0].Definition, bs.macroDef.macroDefs[0].second.c_str()));
		_ASSERTE(0 == _tcsicmp(buildParam.strIncludePath, bs.strIncludeFilePath.c_str()));
		_ASSERTE(buildParam.dwFlag == bs.dwFlags);
		_ASSERTE(buildParam.uShaderParamHandleSize == bs.vecEffectParamHandleBuffer.size());
		_ASSERTE(buildParam.pEventHandler == bs.pEventHandler);
	}
	//加载几个同样的，然后Reload，应该一起更新
	{
		_ASSERTE(0 == stEffectInstanceCount && 0 == stShaderHolderPtrInstanceCount && 0 == s_ulTotalEffectRef);
		ShaderMgrTest Mgr(DummyLst);
		_ASSERTE(0 == stEffectInstanceCount);
		KG3DShaderHolder HolderArray[3];
		HolderArray[0] = Mgr.Create(bs.pDevice, bs.strFilePathOrResourceID.c_str()
			,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str()
			, bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString);
		HolderArray[1] = Mgr.Create(bs.pDevice, bs.strFilePathOrResourceID.c_str()
			,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str()
			, bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString);
		HolderArray[2] = HolderArray[0];

		for (size_t i = 0; i < _countof(HolderArray)-1; ++i)
		{
			_ASSERTE(HolderArray[i].GetEffect() == HolderArray[i+1].GetEffect());
		}
		LPD3DXEFFECT pEffectOld = HolderArray[0].GetEffect();
		_ASSERTE(3 == Mgr.m_Effects.front().uRef);	//其中有一个复制的是Holder，增加的是Holder的引用计数	

		HRESULT hr = Mgr.Reload(*HolderArray[0].GetShaderPtrHolder());
		_ASSERTE(SUCCEEDED(hr));
		_ASSERTE(3 == Mgr.m_Effects.front().uRef);
		_ASSERTE(HolderArray[0].GetEffect() != pEffectOld);
		_ASSERTE(1 == s_ulTotalEffectRef);

		_ASSERTE(FAILED(HolderArray[0].GetEffect()->SetTechnique("a")));
	}

	//单一创建失败，文件改正之后应该创建成功
	{
		_ASSERTE(0 == stEffectInstanceCount && 0 == stShaderHolderPtrInstanceCount && 0 == s_ulTotalEffectRef);
		ShaderMgrTest Mgr(DummyLst);
		Mgr.m_bEffectCreationCoreShouldSucceed = FALSE;
		KG3DShaderPtrHolder* p = Mgr.CreateShaderHolder(bs.pDevice, bs.strFilePathOrResourceID.c_str()
			,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str()
			, bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString, NULL);
		_ASSERTE(NULL != p);
		_ASSERTE(0 == stEffectInstanceCount && 1 == stShaderHolderPtrInstanceCount 
			&& 1 == Mgr.m_Effects.size()
			&& 1 == Mgr.m_StringEffectMap.size());
		_ASSERTE(0 == s_ulTotalEffectRef);
		_ASSERTE(0 == stEffectInstanceCount);
		_ASSERTE(2 == Mgr.m_Effects.front().uRef);

		Mgr.m_bEffectCreationCoreShouldSucceed = TRUE;
		HRESULT hr = Mgr.Reload(*p);
		_ASSERTE(SUCCEEDED(hr));
		_ASSERTE(NULL != dynamic_cast<TestEffect*>(p->GetEffect()));
		_ASSERTE(1 == s_ulTotalEffectRef);
		_ASSERTE(1 == stEffectInstanceCount);
		_ASSERTE(2 == Mgr.m_Effects.front().uRef);

		SAFE_RELEASE(p);
	}
	//Reload失败，应该不改变Shader本身
	{
		_ASSERTE(0 == stEffectInstanceCount && 0 == stShaderHolderPtrInstanceCount && 0 == s_ulTotalEffectRef);
		ShaderMgrTest Mgr(DummyLst);
		KG3DShaderPtrHolder* p = Mgr.CreateShaderHolder(bs.pDevice, bs.strFilePathOrResourceID.c_str()
			,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str(), bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString, NULL);
		_ASSERTE(NULL != p);
		_ASSERTE(1 == stEffectInstanceCount && 1 == stShaderHolderPtrInstanceCount 
			&& 1 == Mgr.m_Effects.size()
			&& 1 == Mgr.m_StringEffectMap.size());
		_ASSERTE(stShaderHolderPtrInstanceCount == (ULONG)s_ulTotalEffectRef);
		_ASSERTE(2 == Mgr.m_Effects.front().uRef);
			
		LPD3DXEFFECT pOld = p->GetEffect();

		Mgr.m_bEffectCreationCoreShouldSucceed = FALSE;
		HRESULT hr = Mgr.Reload(*p);
		_ASSERTE(FAILED(hr));
		_ASSERTE(pOld == p->GetEffect());
		_ASSERTE(2 == Mgr.m_Effects.front().uRef);

		Mgr.m_bEffectCreationCoreShouldSucceed = TRUE;
		_ASSERTE(stShaderHolderPtrInstanceCount == (ULONG)s_ulTotalEffectRef);
		
		SAFE_RELEASE(p);
	}
	//全部重新加载
	{
		_ASSERTE(0 == stEffectInstanceCount && 0 == stShaderHolderPtrInstanceCount && 0 == s_ulTotalEffectRef);
		ShaderMgrTest Mgr(DummyLst);
		KG3DShaderHolder HolderArray[3];
		KG3DShaderHolder Holder1;

		//创建出几个不一样的
		for (size_t i = 0; i < _countof(HolderArray); ++i)
		{
			std::tstring FilePath = bs.strFilePathOrResourceID;
			TCHAR	TCharBuffer[MAX_PATH];
			FilePath += _itot((int)i, TCharBuffer, 10);
			HolderArray[i] = Mgr.Create(bs.pDevice, FilePath.c_str()
				,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str()
				, bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString);
		}
		for (size_t i = 0; i < _countof(HolderArray)-1; ++i)
		{
			_ASSERTE(HolderArray[i].GetEffect() != HolderArray[i+1].GetEffect());
		}
		//再创建一个一样的
		{
			std::tstring FilePath = bs.strFilePathOrResourceID;
			TCHAR	TCharBuffer[MAX_PATH];
			FilePath += _itot(0, TCharBuffer, 10);
			Holder1 = Mgr.Create(bs.pDevice, FilePath.c_str()
				,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str()
				, bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString);
		}
		_ASSERTE(3 == Mgr.m_Effects.front().uRef);
		_ASSERTE(_countof(HolderArray) == (ULONG)s_ulTotalEffectRef);
		_ASSERTE(_countof(HolderArray) == stEffectInstanceCount && _countof(HolderArray)+1 == stShaderHolderPtrInstanceCount);

		KG3DShaderManagerSolid::TypeStringEffectMap SEMapOld = Mgr.m_StringEffectMap;
		
		size_t OldSize = Mgr.m_Effects.size();
		LPD3DXEFFECT pOld = Holder1.GetEffect();

		HRESULT hr = Mgr.ReloadAll();
		_ASSERTE(SUCCEEDED(hr));
		_ASSERTE(Mgr.m_Effects.size() == OldSize);
		_ASSERTE(Mgr.m_StringEffectMap == SEMapOld);
		_ASSERTE(_countof(HolderArray) == stEffectInstanceCount && _countof(HolderArray)+1 == stShaderHolderPtrInstanceCount);
		_ASSERTE(Holder1.GetEffect() != pOld);
		_ASSERTE(3 == Mgr.m_Effects.front().uRef);
		_ASSERTE(KGGetRef(HolderArray[0].GetEffect()) == 1);
		_ASSERTE(2 == Mgr.m_Effects.back().uRef);
		_ASSERTE(_countof(HolderArray) == (ULONG)s_ulTotalEffectRef);

		_ASSERTE(FAILED(Holder1.GetEffect()->SetTechnique("a")));
	}
	//自动的设备丢失和恢复
	{
		_ASSERTE(0 == stEffectInstanceCount && 0 == stShaderHolderPtrInstanceCount && 0 == s_ulTotalEffectRef);
		ShaderMgrTest Mgr(DummyLst);
		KG3DShaderPtrHolder* pArray[3] = {NULL, NULL, NULL};
		for (size_t i = 0; i < _countof(pArray); ++i)
		{
			std::tstring FilePath = bs.strFilePathOrResourceID;
			TCHAR	TCharBuffer[MAX_PATH];
			FilePath += _itot((int)i, TCharBuffer, 10);
			pArray[i] = Mgr.CreateShaderHolder(bs.pDevice, FilePath.c_str()
				,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str(), bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString, NULL);
		}

		_ASSERTE(stShaderHolderPtrInstanceCount == s_ulTotalEffectRef);

		_ASSERTE(_countof(pArray) == stEffectInstanceCount && stShaderHolderPtrInstanceCount == _countof(pArray));
		_ASSERTE(0 == stDeviceLostObj);
		Mgr.OnReceiveMessage(KG3DMessage(KM_D3DDEVICE_LOST), NULL);
		_ASSERTE(_countof(pArray) == stDeviceLostObj);
		_ASSERTE(3 == s_ulTotalEffectRef);	//设备丢失的话，Effect是不需要释放的

		_ASSERTE(0 == stDeviceResetObj);
		Mgr.OnReceiveMessage(KG3DMessage(KM_D3DDEVICE_RESET), NULL);
		_ASSERTE(_countof(pArray) == stDeviceResetObj);
		_ASSERTE(_countof(pArray) == s_ulTotalEffectRef);

		_ASSERTE(_countof(pArray) == stEffectInstanceCount);
		Mgr.OnReceiveMessage(KG3DMessage(KM_D3DDEVICE_RELEASE), NULL);
		_ASSERTE(0 == stEffectInstanceCount && 0 == s_ulTotalEffectRef);
		_ASSERTE(_countof(pArray) == Mgr.m_Effects.size());
		_ASSERTE(NULL == Mgr.m_Effects.front().pEffect );
		
		for (size_t i = 0; i < _countof(pArray); ++i)
			SAFE_RELEASE(pArray[i]);
	}
	//垃圾回收
	{
		_ASSERTE(! stForceGarbageCollection);
		_ASSERTE(0 == stEffectInstanceCount && 0 == stShaderHolderPtrInstanceCount && 0 == s_ulTotalEffectRef);
		ShaderMgrTest Mgr(DummyLst);
		_ASSERTE(0 == stEffectInstanceCount);
		time_t firstTime = time(NULL);
		KG3DShaderPtrHolder* p = Mgr.CreateShaderHolder(bs.pDevice, bs.strFilePathOrResourceID.c_str()
			,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str()
			, bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString, NULL);
		_ASSERTE(NULL != p);

		
		_ASSERTE(stEffectInstanceCount == 1);
		_ASSERTE(s_ulTotalEffectRef == 1);

		//连续创建3个，因为时间没有到，所以应该没有垃圾被回收
		KG3DShaderPtrHolder* pArray[3] = {NULL, NULL, NULL};
		for (size_t i = 0; i < _countof(pArray); ++i)
		{
			std::tstring FilePath = bs.strFilePathOrResourceID;
			TCHAR	TCharBuffer[MAX_PATH];
			FilePath += _itot((int)i, TCharBuffer, 10);
			pArray[i] = Mgr.CreateShaderHolder(bs.pDevice, FilePath.c_str()
				,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str(), bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString, NULL);
		}
			
		time_t secondTime = time(NULL);
		_ASSERTE(secondTime - firstTime < KG3DShaderManagerSolid::em_min_garbage_collection_time_gap);
		_ASSERTE(stEffectInstanceCount == _countof(pArray) + 1);
		_ASSERTE(s_ulTotalEffectRef == _countof(pArray) + 1);
		
		stForceGarbageCollection = true;
		for (size_t i = 0; i < em_max_garbage_weight; ++i)
		{
			Mgr.GarbageCollection();
		}
		
		_ASSERTE(stEffectInstanceCount == _countof(pArray) + 1);	//这个时候应该没有任何释放
		
		SAFE_RELEASE(p);
		for (size_t i = 0; i < _countof(pArray); ++i)
			SAFE_RELEASE(pArray[i]);

		for (size_t i = 0; i <= em_max_garbage_weight; ++i)
		{
			Mgr.GarbageCollection();
		}
		
		_ASSERTE(0 == stEffectInstanceCount);
		_ASSERTE(0 == Mgr.m_Effects.size() && 0 == Mgr.m_StringEffectMap.size());
	}
	//设备丢失和恢复的处理器的注册
	{
		_ASSERTE(0 == stEffectInstanceCount && 0 == stShaderHolderPtrInstanceCount && 0 == s_ulTotalEffectRef);
		static bool stLostEventHandled = false;
		static bool stResetEventHandled = false;
		static bool stReloadEventHandled = false;
		static ULONG	stEventHandlerCount = 0;
		ShaderMgrTest Mgr(DummyLst);
		struct  CustomHandler : public KG3DShaderEventHandler
		{
			virtual HRESULT OnLostDevice(LPD3DXEFFECT p){stLostEventHandled = true;return S_OK;}
			virtual	HRESULT OnResetDevice(LPD3DXEFFECT p){stResetEventHandled = true;return S_OK;}
			virtual HRESULT OnReload(LPD3DXEFFECT p){stReloadEventHandled = true;return S_OK;}

			CustomHandler(){++stEventHandlerCount;}
			~CustomHandler(){--stEventHandlerCount;}

			virtual KG3DShaderEventHandler* Clone()const{return new CustomHandler;}
			virtual ULONG STDMETHODCALLTYPE Release(){delete this;return 0;}
		};
		
		KG3DShaderHolder ABottle = Mgr.Create(bs.pDevice, bs.strFilePathOrResourceID.c_str()
			,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str()
			, bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString, NULL);
		_ASSERTE(Private::IsShaderHolderInited(ABottle));

		Mgr.OnReceiveMessage(KG3DMessage(KM_D3DDEVICE_LOST), NULL);
		Mgr.OnReceiveMessage(KG3DMessage(KM_D3DDEVICE_RESET), NULL);
		_ASSERTE(!stLostEventHandled && !stResetEventHandled);
		Mgr.ReloadAll();
		_ASSERTE(!stReloadEventHandled);

		KG3DShaderHolder ABottle2 = Mgr.Create(bs.pDevice, _T("abcdef")
			,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str(), bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString, &CustomHandler());

		_ASSERTE(stEventHandlerCount == 1);

		Mgr.OnReceiveMessage(KG3DMessage(KM_D3DDEVICE_LOST), NULL);
		Mgr.OnReceiveMessage(KG3DMessage(KM_D3DDEVICE_RESET), NULL);
		_ASSERTE(stLostEventHandled && stResetEventHandled);
		Mgr.ReloadAll();
		_ASSERTE(stReloadEventHandled);
		
	}
	//Find
	{
		_ASSERTE(0 == stEffectInstanceCount && 0 == stShaderHolderPtrInstanceCount && 0 == s_ulTotalEffectRef);
		ShaderMgrTest Mgr(DummyLst);
		_ASSERTE(0 == stEffectInstanceCount);
		KG3DShaderHolder holder = Mgr.Create(bs.pDevice, bs.strFilePathOrResourceID.c_str()
			,bs.macroDef.ToD3DXMACRO_ARRAY(macroBuffer), bs.strIncludeFilePath.c_str(), bs.dwFlags, bs.vecEffectParamHandleBuffer.size(), bs.bUseEffectPool, &bs.strErrorString);

		_ASSERTE(NULL != holder.GetShaderPtrHolder() && NULL != holder.GetShaderPtrHolder()->GetEffect());
		
		KG3DShaderHolder holderTemp;
		HRESULT hr = Mgr.Find(_T("Shaderxxx"), holderTemp);
		_ASSERTE(FAILED(hr));
		_ASSERTE(stEffectInstanceCount == 1 && stShaderHolderPtrInstanceCount == 1);

		hr = Mgr.Find(bsShaderName, holderTemp);
		_ASSERTE(SUCCEEDED(hr));
		_ASSERTE(stEffectInstanceCount == 1 && stShaderHolderPtrInstanceCount == 2);
	}
	_ASSERTE(0 == stEffectInstanceCount && 0 == stShaderHolderPtrInstanceCount && 0 == s_ulTotalEffectRef);

	if(bInitSingletonDummy)//要把上面的更改去掉，不然Release版可能会出事也不知道
		ZeroMemory(&stSingletonDummyEffect, sizeof(stSingletonDummyEffect));
}
KG_TEST_CLASS_TEST_END(KG3DShaderManagerSolid)
#endif
#endif

//////////////////////////////////////////////////////////////////////////





