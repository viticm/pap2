////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRepresentObjectDummy.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-7-18 10:05:02
//  Comment     : 
/*用于Observer模式，场景中一个物件，和内部程序之间的交互，用于基类扩展用的
不要和任何具体的类耦合起来，不要加任何和具体逻辑关联的变量。不然派生类就得
忍受一堆没有关系的东东。

Dummy提供一个动态Buffer，用于给用户放些和Dummy联系在一起的数据，用户自己决定怎么用。
Buffer中最好只放些简单数据，Object析构会清空这个Buffer，但不知道里面是些啥东东，也就
没法帮你调用析构函数

如(伪代码)：
INT ObjectOrder = 10;
KG3DRepresentObjectDummy* pObjectDummy = NULL;
HRESULT hr = g_GetObjectTable().Get1NewResouce(&pObjectDummy, REPRESENTOBJECT_DUMMY, 0);
if(SUCCEED(hr))
{
	pObjectDummy->RegisterReceiver(this);
	pObjectDummy->SetBuffer(&ObjectOrder, sizeof(ObjectOrder));
}
hr = m_SceneRef.AddRepresentObject(pObjectDummy, EM_SCENE_ADD_OPTION_SELECTABLE, NULL);


那么在Object移动，选择等进行的时候，Messenger架构会传递消息
那么在OnReceiverMessage中可以写这样的
switch(Message.m_uMessage)
{
case KM_XXX:
{
	INT ObjectOrder = 0;
	HRESULT hr = pObjectDummy->GetBuffer(&ObjectOrder, sizeof(ObjectOrder));
	if(SUCCEED(hr))
		this->DoSomething(ObjectOrder);

	break;
}

}

*/
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DREPRESENTOBJECTDUMMY_H_
#define _INCLUDE_KG3DREPRESENTOBJECTDUMMY_H_

#include "KG3DRepresentObject.h"
#include "KG3DMessenger.h"

////////////////////////////////////////////////////////////////////////////////
class KG3DRepresentObjectDummy : public KG3DRepresentObject, public KG3DMessageSender
{
public:
	static KG3DRepresentObject* Build(DWORD_PTR wParam, DWORD_PTR lParam){return new KG3DRepresentObjectDummy;}
	virtual HRESULT Copy(KG3DRepresentObject* pSrcObject);
	virtual HRESULT Render(DWORD dwOption);
	virtual BOOL RayIntersection(const D3DXVECTOR3& vSrc
		, const D3DXVECTOR3& vNormalizedDir
		, FLOAT* pfRet
		, BOOL bIntersectBoxOnly
		, KG3DIntersectionFilter* pFilter = NULL);


	//Dummy提供一个动态数组用于存储用户数据，1kByte之内这些数据最好不是带析构函数的，Dummy不会知道怎么调用析构
	enum{max_buffer_size = 1024
		, shot_buffer_size = sizeof(DWORD_PTR)*2};

	VOID	ClearBuffer();
	HRESULT	SetBuffer(LPVOID pData, size_t DataSize);
	HRESULT GetBuffer(LPVOID pData, size_t DataSize);

	//覆盖方法，用于发消息
	virtual HRESULT SetTranslation(const D3DXVECTOR3* pTrans);
	virtual HRESULT SetRotation(const D3DXQUATERNION* pRotaion);
	virtual HRESULT SetScaling(const D3DXVECTOR3* pScale);
	virtual HRESULT OnSelect();

	virtual HRESULT ComputeBBox();
	virtual HRESULT	SetSize(FLOAT fSize);
	virtual BOOL	IsSpecial(){return TRUE;}

	KG3DRepresentObjectDummy();
	virtual ~KG3DRepresentObjectDummy();
#if defined(DEBUG) | defined(_DEBUG)
protected:
#else
private:
#endif

	char*	m_pBuffer;
	char	m_ShortBuffer[shot_buffer_size];	
	size_t	m_BufferSize;

	FLOAT	m_fSize;
};

#endif //_INCLUDE_KG3DREPRESENTOBJECTDUMMY_H_
