#include "StdAfx.h"
#include "KG3DSceneEntityGroup.h"
#include "kg3dgraphicstool.h"
#include "KG3DSceneEntityGroupTable.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KG3DSceneEntityGroup::KG3DSceneEntityGroup(void)
{
	m_dwID = 0;
	m_dwType = 0;
	m_ulRefCount = 0;
	D3DXQuaternionIdentity(&m_Rotation);
}

KG3DSceneEntityGroup::~KG3DSceneEntityGroup(void)
{
	UnInit();
}

HRESULT KG3DSceneEntityGroup::AddEntity(KG3DSceneEntity& Entity)
{
	m_listEntity.push_back(Entity);
	D3DXQUATERNION Rot;
	D3DXVECTOR3 RotValue;
	Entity.GetRotation(&Rot,&RotValue);
	m_listRotationBegan.push_back(Rot);
	return S_OK;
}

HRESULT KG3DSceneEntityGroup::RemoveEntity(KG3DSceneEntity& Entity)
{
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//引用计数
ULONG STDMETHODCALLTYPE KG3DSceneEntityGroup::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}
ULONG STDMETHODCALLTYPE KG3DSceneEntityGroup::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
        OnReferenceZero(); 
    return uNewRefCount;
}
HRESULT STDMETHODCALLTYPE KG3DSceneEntityGroup::QueryInterface( 
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
{
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
//初始化,清除数据
HRESULT KG3DSceneEntityGroup::Init()
{
	return S_OK;
}
HRESULT KG3DSceneEntityGroup::UnInit()
{
	return S_OK;
}

HRESULT KG3DSceneEntityGroup::FrameMove()
{
	for_each(m_listEntity.begin(), m_listEntity.end(), mem_fun_ref(&KG3DSceneEntity::FrameMove));
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
//加载,保存
HRESULT KG3DSceneEntityGroup::LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption)
{
	return S_OK;
}

HRESULT KG3DSceneEntityGroup::SaveToFile(const char cszFileName[],   unsigned uOption)
{
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
//ID,type
DWORD KG3DSceneEntityGroup::GetID()
{
	return m_dwID;
}
unsigned KG3DSceneEntityGroup::GetType()
{
	return m_dwType;
}
//////////////////////////////////////////////////////////////////////////
//内存管理
HRESULT KG3DSceneEntityGroup::PrepareRender(unsigned uOption)//压缩数据
{
	return S_OK;
}
HRESULT KG3DSceneEntityGroup::PostRender()//恢复数据，对象可以立即被使用
{
	return S_OK;
}
HRESULT KG3DSceneEntityGroup::GetDateInformation(int* pnMem,int* pnVedioMem,int* pnConMem,int* pnConVMem)//得到数据信息，参数依次为占用内存大小，现存大小，压缩后内存大小，压缩后内存大小
{
	return S_OK;
}
unsigned   KG3DSceneEntityGroup::GetLastUseTime()//查询上次使用的时间
{
	return S_OK;
}
unsigned   KG3DSceneEntityGroup::GetUseFrequency()//查询使用频率，一段时间里的使用次数
{
	return S_OK;
}
HRESULT KG3DSceneEntityGroup::OnReferenceZero()
{
	//return g_cModelTable.ReleaseResource(this,RELEASE_ATONCE);
	g_cSceneEntityGroupTable.ReleaseResource(this,RELEASE_ATONCE);
	return S_OK;
}

HRESULT KG3DSceneEntityGroup::Render(DWORD dwOption)
{
	/*KG3DSceneEntityList::iterator i = m_listEntity.begin();
	while(i!=m_listEntity.end())
	{
		KG3DSceneEntity Entity = *i;
		Entity.RenderOption(dwOption);
		i++;
	}*/
	return S_OK;
}

HRESULT KG3DSceneEntityGroup::ComputeBBox()
{
	m_BBox.Clear();
	AABBOX Box;
	KG3DSceneEntityList::iterator i = m_listEntity.begin();
	while(i!=m_listEntity.end())
	{
		KG3DSceneEntity Entity = *i;
		Box.Clear();
		Entity.GetBBox(&Box);
		m_BBox.AddPosition(Box.A);
		m_BBox.AddPosition(Box.B);
		i++;
	}
	return S_OK;
}

HRESULT KG3DSceneEntityGroup::RenderSelectMark(DWORD dwColor)
{
	g_cGraphicsTool.DrawAABBox(&m_BBox, dwColor);
	return S_OK;
}

void KG3DSceneEntityGroup::SetTranslation(D3DXVECTOR3* pValue)
{
	if(m_listEntity.size()==0)
		return;

	D3DXVECTOR3 PosSum(0,0,0);
	KG3DSceneEntityList::iterator i = m_listEntity.begin();
	while(i!=m_listEntity.end())
	{
		KG3DSceneEntity Entity = *i;
		D3DXVECTOR3 Pos(0,0,0);
		Entity.GetTranslation(&Pos);
		PosSum += Pos;
		i++;
	}
	D3DXVECTOR3 PosAvs = PosSum / (float)m_listEntity.size();

	i = m_listEntity.begin();
	while(i!=m_listEntity.end())
	{
		KG3DSceneEntity Entity = *i;
		D3DXVECTOR3 Pos(0,0,0);
		Entity.GetTranslation(&Pos);
		Entity.SetTranslation(&(*pValue-PosAvs+Pos));
		i++;
	}
}

void KG3DSceneEntityGroup::SetRotation(D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ)
{
	if(m_listEntity.size()==0)
		return;

	m_Rotation = (*pValue)/1000;
	D3DXQuaternionNormalize(&m_Rotation,&m_Rotation);

	D3DXVECTOR3 Center(0,0,0);
	GetTranslation(&Center);

	KG3DSceneEntityList::iterator i = m_listEntity.begin();
	list<D3DXQUATERNION>::iterator j = m_listRotationBegan.begin();
	while(i!=m_listEntity.end())
	{
		KG3DSceneEntity Entity = *i;
		D3DXQUATERNION RotEntityBegan = *j;

		D3DXVECTOR3 PosEntity(0,0,0);
		Entity.GetTranslation(&PosEntity);
		D3DXVECTOR3 V = PosEntity - Center;
		D3DXMATRIX mat;
		D3DXMatrixRotationQuaternion(&mat,&m_Rotation);

		D3DXVec3TransformCoord(&V,&V,&mat);

		Entity.SetTranslation(&(Center+V));

		D3DXQUATERNION RotEntity;
		D3DXVECTOR3 RotValue;
		Entity.GetRotation(&RotEntity,&RotValue);

		RotEntity *= m_Rotation;
		D3DXQuaternionNormalize(&RotEntity,&RotEntity);
		Entity.SetRotation(&RotEntity,pValueXYZ);
		i++;
		j++;
	}
}
void KG3DSceneEntityGroup::SetRotationCenter(D3DXVECTOR3* pValue)
{
	if(m_listEntity.size()==0)
		return;

	D3DXVECTOR3 PosSum(0,0,0);
	KG3DSceneEntityList::iterator i = m_listEntity.begin();
	while(i!=m_listEntity.end())
	{
		KG3DSceneEntity Entity = *i;
		D3DXVECTOR3 Pos(0,0,0);
		Entity.SetRotationCenter(pValue);
		PosSum += Pos;
		i++;
	}
}
void KG3DSceneEntityGroup::SetScaling(D3DXVECTOR3* pValue)
{
	;
}
void KG3DSceneEntityGroup::SetScalingRotation(D3DXQUATERNION* pValue)
{
	;
}

void KG3DSceneEntityGroup::SetScalingCenter(D3DXVECTOR3* pValue)
{
	;
}

void KG3DSceneEntityGroup::GetTranslation(D3DXVECTOR3* pValue)
{
	if(m_listEntity.size()==0)
		return;

	D3DXVECTOR3 PosSum(0,0,0);
	KG3DSceneEntityList::iterator i = m_listEntity.begin();
	while(i!=m_listEntity.end())
	{
		KG3DSceneEntity Entity = *i;
		D3DXVECTOR3 Pos(0,0,0);
		Entity.GetTranslation(&Pos);
		PosSum += Pos;
		i++;
	}
	(*pValue) = PosSum / (float)m_listEntity.size();
}

void KG3DSceneEntityGroup::GetRotation(D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ)
{
	(*pValue) = m_Rotation;
	(*pValueXYZ) = D3DXVECTOR3(0,0,0);
}
void KG3DSceneEntityGroup::GetRotationCenter(D3DXVECTOR3* pValue)
{
	(*pValue) = m_BBox.GetCenter();
}
void KG3DSceneEntityGroup::GetScaling(D3DXVECTOR3* pValue)
{
	;
}
void KG3DSceneEntityGroup::GetScalingRotation(D3DXQUATERNION* pValue)
{
	;
}
void KG3DSceneEntityGroup::GetScalingCenter(D3DXVECTOR3* pValue)
{
	;
}

HRESULT KG3DSceneEntityGroup::RefreshOption(unsigned uOption)
{
	return S_OK;
}