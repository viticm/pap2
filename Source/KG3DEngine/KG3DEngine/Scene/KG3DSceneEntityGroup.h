#pragma once

#include ".\kg3dsceneentity.h"
#include "..\..\..\Include\KG3DEngine\KG3DInterface.h"

using namespace std;

class KG3DScene;

class KG3DSceneEntityGroup : public IKG3DResourceBase
{
protected:
	DWORD   m_dwType;//类型
	DWORD   m_dwID;

	D3DXQUATERNION m_Rotation;
protected:

public:
	KG3DSceneEntityList m_listEntity;
	list<D3DXQUATERNION>m_listRotationBegan;
	ULONG  m_ulRefCount;//物品使用计数
	AABBOX m_BBox;
	string m_scName;
	//////////////////////////////////////////////////////////////////////////
	//引用计数
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

	//////////////////////////////////////////////////////////////////////////
	//初始化,清除数据
	virtual HRESULT Init();
	virtual HRESULT UnInit();
	//////////////////////////////////////////////////////////////////////////
	//加载,保存
	virtual HRESULT LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption);
	virtual HRESULT SaveToFile(const char cszFileName[],   unsigned uOption);
	virtual HRESULT LoadFromFileEx(const char cszFileName[], unsigned uFileNameHash, unsigned uOption, DWORD_PTR WParam, DWORD_PTR LParam)
	{
		return LoadFromFile(cszFileName, uFileNameHash, uOption);
	}
	virtual HRESULT RefreshOption(unsigned uOption);
	//////////////////////////////////////////////////////////////////////////
	//ID,type
	virtual DWORD GetID();
	virtual unsigned GetType();
	void SetID(DWORD dwID) { m_dwID = dwID;}
	void SetType(DWORD dwType){m_dwType = dwType;}
	//////////////////////////////////////////////////////////////////////////
	//内存管理
	virtual HRESULT PrepareRender(unsigned uOption);//压缩数据
	virtual HRESULT PostRender();//恢复数据，对象可以立即被使用
	virtual HRESULT GetDateInformation(int* pnMem,int* pnVedioMem,int* pnConMem,int* pnConVMem);//得到数据信息，参数依次为占用内存大小，现存大小，压缩后内存大小，压缩后内存大小
	virtual unsigned   GetLastUseTime();//查询上次使用的时间
	virtual unsigned   GetUseFrequency();//查询使用频率，一段时间里的使用次数

private:

	virtual HRESULT OnReferenceZero();


public:
	KG3DSceneEntityGroup(void);
public:
	virtual ~KG3DSceneEntityGroup(void);

	HRESULT AddEntity(KG3DSceneEntity& Entity);
	HRESULT RemoveEntity(KG3DSceneEntity& Entity);

	HRESULT Render(DWORD dwOption);
	HRESULT FrameMove();
	HRESULT ComputeBBox();
	HRESULT RenderSelectMark(DWORD dwColor = 0xFFFF0000);


	void SetTranslation(D3DXVECTOR3* pValue);
	void SetRotation(D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
	void SetRotationCenter(D3DXVECTOR3* pValue);
	void SetScaling(D3DXVECTOR3* pValue);
	void SetScalingRotation(D3DXQUATERNION* pValue);
	void SetScalingCenter(D3DXVECTOR3* pValue);

	void GetTranslation(D3DXVECTOR3* pValue);
	void GetRotation(D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
	void GetRotationCenter(D3DXVECTOR3* pValue);
	void GetScaling(D3DXVECTOR3* pValue);
	void GetScalingRotation(D3DXQUATERNION* pValue);
	void GetScalingCenter(D3DXVECTOR3* pValue);
};
