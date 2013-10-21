////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneObjectPlacementTool.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-4-1 11:34:33
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSCENEOBJECTPLACEMENTTOOL_H_
#define _INCLUDE_KG3DSCENEOBJECTPLACEMENTTOOL_H_
#include "KG3DMessageWMGListner.h"
#include "IEKG3DSceneObjectPlacementTool.h"
#include "KG3DCommonObject.h"

class KG3DRepresentObject;
class KG3DRepresentObjectSet;
class KG3DSceneSceneEditor;
////////////////////////////////////////////////////////////////////////////////
class KG3DSceneSceneEditor;
class KG3DSceneObjectPlacementTool : public KG3DCommonObjectSimple, public IEKG3DSceneObjectPlacementTool, public KG3DMessageMSGListner
{
public:
	STDMETHOD_(ULONG, GetType)();
	STDMETHOD_(LPVOID,ToAnotherInterface)(DWORD_PTR Param);
	static KG3DCommonObject* Build(LPCTSTR lpFile, DWORD_PTR pScene, DWORD_PTR);

	virtual HRESULT SetObjectInHandle(IEKG3DRepresentObject* pObject, BOOL bReleaseWhenAnotherObjectIsSettedToHandle);//如果设了第二个参数，那么外部不用管生命周期，否则，外部要管生命周期
	//virtual HRESULT SetObjectSetInHandle(IEKG3DRepresentObjectSet* pObject, BOOL bReleaseWhenAnotherObjectIsSettedToHandle);
	virtual HRESULT GetObjectInHandle(IEKG3DRepresentObject** ppObject/*, IEKG3DRepresentObjectSet** ppObjectSet*/);//返回任意一种
	virtual HRESULT RemoveObjectInHandle();

	virtual void __stdcall OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender );

	KG3DSceneObjectPlacementTool(KG3DSceneSceneEditor* pEditor);
	~KG3DSceneObjectPlacementTool();
	
	HRESULT DoPlaceSpecObject(KG3DRepresentObject *pObject,BOOL& bToFile);	// last update by likan 2009-11-11
public:
	//仅引擎内部使用，如果是要在某个位置放一个物件的话，不要用放置工具倒腾
	//，应该用BuildKG3DRepresentObject+KG3DRepresentObject::SetTranslation
	//+KG3DSceneSceneEditor::AddRepresentObject
	HRESULT DoPlaceObject();	//暂时是public的，应该可以改成private的
	HRESULT DoPlaceObjectByOther(IEKG3DRepresentObject *pInObject, IEKG3DRepresentObject **pOutObj); //在某个obj的位置创建另一个obj,by likan

private:
	STDMETHOD(Render)();
	STDMETHOD(FrameMove)();

	KG3DRepresentObject* m_pObjInHandle;
	//KG3DRepresentObjectSet* m_pObjSetInHandle;
	enum
	{
		EM_RELEASE_WHEN_SET_HANDLE = 0x01,
	};
	KG_CUSTOM_HELPERS::KGBitSet<32>		m_BitSet;
	DWORD	m_dwLastPlaceTime;	//为了防止连续的点击，放置需要有一个间隔
	KG3DSceneSceneEditor* m_pSceneEditor;
};

#endif //_INCLUDE_KG3DSCENEOBJECTPLACEMENTTOOL_H_
