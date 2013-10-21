#pragma once
#include "..\..\..\include\kg3dengine\kg3dinterface.h"

class IKG3DEffect: public IKG3DResourceBase
{
public:
	virtual HRESULT OnLostDevice() = 0;
	virtual HRESULT OnResetDevice() = 0;
};

class KG3DEffect : public IKG3DEffect
{
public:
	
	KG3DEffect(void);
	~KG3DEffect(void);
	
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

    virtual HRESULT LoadFromFile(const char cszFileName[], unsigned uOption = 0,
		                         unsigned uOperator = 0);
	virtual HRESULT SaveToFile(const char cszFileName[], unsigned uOption = 0);
	virtual HRESULT LoadFromFileEx(const char cszFileName[], unsigned uFileNameHash, unsigned uOption, DWORD_PTR WParam, DWORD_PTR LParam);

    virtual HRESULT Init();
    virtual HRESULT UnInit();

	virtual HRESULT OnLostDevice();
	virtual HRESULT OnResetDevice();

	virtual unsigned GetType();

	//////////////////////////////////////////////////////////////////////////
	// memory manager
	virtual HRESULT PrepareRender(unsigned uOption); 
	virtual HRESULT PostRender(); 
	virtual HRESULT GetDateInformation(int *pnMemory, int *pnVideoMemory, int *pnCompressMem, int *pnCompressVideoMem);
	virtual unsigned GetLastUseTime();
	virtual unsigned GetUseFrequency();

	LPD3DXEFFECT GetEffect() { return m_pEffect;}
	HRESULT BegineEffect(unsigned int& uNumPass);
	HRESULT EndEffect();
	const std::string& GetResouceFileName() { return m_szFileName;}

private:
	const static unsigned m_uEffectType = 0xffffffff;

	LPD3DXEFFECT m_pEffect;
	ULONG m_ulRefCount;

public:
	std::string m_szFileName;
	
	
};
