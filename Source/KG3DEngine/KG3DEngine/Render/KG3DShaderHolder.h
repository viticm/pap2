////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DShaderHolder.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-12 17:04:06
/*  Comment     : 自动设备丢失，Reload可以重新加载。自动Shader释放，因为事件Listener会通知
D3D的析构事件，所以不用担心来不及释放
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSHADERHOLDER_H_
#define _INCLUDE_KG3DSHADERHOLDER_H_
////////////////////////////////////////////////////////////////////////////////
/*
使用GetEffect：
KG3DShaderHolder SH = g_GetShaderManager().Create(XX);//重复的Shader会自动复制一个出来
LPD3DXEFFECT pEffect = SH.GetEffect();
_ASSERTE(NULL != pEffect);
if(SUCCEEDED(pEffect->Begin(NULL, 0)))
{
	pEffect->End();
}

重新加载：
hr = SH.Reload();
if(FAILED(hr))
{
	除了写Log，啥也不能做。因为出现这种情况的的可能性只有：文件有问题，内存分配失败。
}

如果要重用一个Shader，可以用相同的文件名从ShaderManger中创建，可以直接复制KG3DShaderHolder.

引用计数，设备恢复什么的全都不用管，已经全部接管了。如果想在设备恢复的时候做些特殊操作，可以用g_GetShaderManager().Create的其中一个参数

因为Effect如果SetTexture会加引用计数，所以
*/

struct  D3DXMACRO_ARRAY
{
	enum
	{
		em_max_macro = 20,	//最大只允许这个数目的宏定义
	};
	const D3DXMACRO* pMacroDefs;	//D3DXMACRO数组，最后必须用NULL,NULL的D3DXMACRO来结束，最后那个也计算在uMacroCount里面
	size_t	uMacroCount;	//数组元素个数

	BOOL MergeDefinition(std::vector<D3DXMACRO>& buffer, const D3DXMACRO_ARRAY& other);

	static D3DXMACRO	GetEmptyMacro()
	{
		D3DXMACRO emptyOne = {NULL, NULL};
		return emptyOne;
	}
	static D3DXMACRO_ARRAY GetEmptyMacroArray()
	{
		D3DXMACRO_ARRAY emptyOne = {
			NULL,
			0,
		};
		return emptyOne;
	}
	static bool IsValid(const D3DXMACRO_ARRAY& macroArray);
};

struct KG3DShaderEventHandler 
{
	//不需要调用p->OnLostDevice或者OnResetDevice，ShaderMgr已经帮你做的，你要做的是处理额外的问题
	virtual HRESULT OnLostDevice(LPD3DXEFFECT p){return S_OK;}
	virtual	HRESULT OnResetDevice(LPD3DXEFFECT p){return S_OK;}
	virtual HRESULT OnReload(LPD3DXEFFECT p){return S_OK;}
	virtual KG3DShaderEventHandler* Clone()const = 0;
	virtual ULONG STDMETHODCALLTYPE Release() = 0;
	virtual ~KG3DShaderEventHandler() = 0{}
};

struct KG3DShaderPtrHolder;
class KG3DShaderHolder
{
public:
	LPD3DXEFFECT GetEffect();//保证非空,不需Release
    LPVOID GetEffectParamHandleBuffer(size_t *puRetBufferSize);	//这个Buffer要即用即取，如果存下来，不保证生命周期
	HRESULT		 Reload();
	KG3DShaderPtrHolder* GetShaderPtrHolder(){return m_pShaderHolder;}

	KG3DShaderHolder();
	explicit KG3DShaderHolder(KG3DShaderPtrHolder* pHolder);
	KG3DShaderHolder(const KG3DShaderHolder& Other);
	KG3DShaderHolder& operator=(const KG3DShaderHolder& Other);
	~KG3DShaderHolder();

	KG_TEST_DECLARE_CLASS_TEST(KG3DShaderHolder);
private:	
	KG3DShaderPtrHolder* m_pShaderHolder;
};

struct KG3DShaderPtrHolder
{
	virtual LPD3DXEFFECT GetEffect() = 0;//可能返回NULL
    virtual LPVOID GetEffectParamHandleBuffer(unsigned *puRetBufferSize) = 0;
	virtual ULONG STDMETHODCALLTYPE AddRef() = 0;
	virtual	ULONG STDMETHODCALLTYPE Release() = 0;

	virtual ~KG3DShaderPtrHolder() = 0{}
};

struct KG3DShaderPtrHolderDummy : public KG3DShaderPtrHolder
{
	virtual LPD3DXEFFECT GetEffect(){return NULL;}
    virtual LPVOID GetEffectParamHandleBuffer(size_t *puRetBufferSize) 
    {
        if (NULL != puRetBufferSize)
        {
            *puRetBufferSize = 0;
        }
        return NULL;
    }
	virtual ULONG STDMETHODCALLTYPE AddRef() {return 1;}
	virtual	ULONG STDMETHODCALLTYPE Release() {return 0;}
};

#endif //_INCLUDE_KG3DSHADERHOLDER_H_
