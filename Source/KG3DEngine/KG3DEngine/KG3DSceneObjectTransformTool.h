////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DTransformTool.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
// 
//  Create Date : 2008-10-16 11:32:31
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSCENEOBJECTTRANSFORMTOOL_H_
#define _INCLUDE_KG3DSCENEOBJECTTRANSFORMTOOL_H_
#include "KG3DCommonObject.h"
////////////////////////////////////////////////////////////////////////////////
typedef enum _KG3DCOORD KG3DCOORD;
class IKG3DMessageReceiverBase;
class KG3DMessageSender;
struct KG3DTransformTool : public KG3DCommonObjectSimple
{
	STDMETHOD_(ULONG, GetType)() = 0;
	STDMETHOD(Render)() = 0;
	STDMETHOD(FrameMove)() = 0;

	virtual HRESULT	SetCoordMode(const KG3DCOORD& coordMode) = 0;//设定是Local还是World还是啥的坐标系

	virtual HRESULT TransformDirect(const D3DXVECTOR3& vTransform) = 0;	//直接变换的函数，参数对于移动来说，是xyz，对于旋转是xyz轴旋转，对于放缩，分别是三轴的放缩
	virtual HRESULT TransformDirectCoord(const D3DXMATRIX& matTransform) = 0;	//TransformDirect最后会调用这个，对于D3DXQUATERNION，用D3DXMatrixRotationQuaternion变成D3DXMATRIX再传进来

	virtual HRESULT RegisterEventListener(DWORD messageID, IKG3DMessageReceiverBase& receiver) = 0;

#if defined(DEBUG) | defined(_DEBUG)
	virtual KG3DMessageSender* TestBegin() = 0;
	virtual VOID TestEnd() = 0;
#endif

	virtual ~KG3DTransformTool() = 0{}

	static DWORD GetTypeCorrespondingMessage(DWORD dwToolType);
};

struct KG3DTransformToolDummy : public KG3DTransformTool
{
	STDMETHOD_(ULONG, GetType)(){return KG3DTYPE_NONE;}
	STDMETHOD(Render)(){return E_NOTIMPL;}
	STDMETHOD(FrameMove)(){return E_NOTIMPL;}

	virtual HRESULT	SetCoordMode(const KG3DCOORD& coordMode){return E_NOTIMPL;}//设定是Local还是World还是啥的坐标系

	virtual HRESULT TransformDirect(const D3DXVECTOR3& vTransform){return E_NOTIMPL;}
	virtual HRESULT TransformDirectCoord(const D3DXMATRIX& matTransform){return E_NOTIMPL;}

	virtual HRESULT RegisterEventListener(DWORD messageID, IKG3DMessageReceiverBase& receiver){return E_NOTIMPL;}

#if defined(DEBUG) | defined(_DEBUG)
	virtual KG3DMessageSender* TestBegin(){return NULL;}
	virtual VOID TestEnd(){return;}
#endif
};

KG3DTransformTool& GetStaticDummyTransformTool();

KG3DCommonObject* BuildKG3DRotationTool(LPCTSTR, DWORD_PTR pScene, DWORD_PTR);
KG3DCommonObject* BuildKG3DTranslationTool(LPCTSTR, DWORD_PTR pScene, DWORD_PTR);
KG3DCommonObject* BuildKG3DScalingTool(LPCTSTR, DWORD_PTR pScene, DWORD_PTR);

#endif //_INCLUDE_KG3DSCENEOBJECTTRANSFORMTOOL_H_
