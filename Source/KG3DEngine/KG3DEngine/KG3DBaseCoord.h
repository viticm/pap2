////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DBaseCoord.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2008-03-5 19:13:23
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG3DBASECOORD_H_
#define _KG3DBASECOORD_H_
#include "KG3DCommonObject.h"

class KG3DMessageReceiverBase;
struct KG3DBaseCoord : public KG3DCommonObjectSimple 
{
	STDMETHOD_(ULONG, GetType)() = 0;

	STDMETHOD(FrameMove)() = 0;
	STDMETHOD(Render)() = 0;

	virtual ULONG	GetCurStateID() = 0;
	virtual HRESULT SetPosition(const D3DXVECTOR3& Pos) = 0;
	virtual HRESULT	Zoom(FLOAT zoomFactor) = 0;//zoomFactor会加到Scale系数里面，而不是乘
	virtual D3DXMATRIX GetTrans() = 0;

#if defined(DEBUG) | defined(_DEBUG)
	virtual KG3DMessageReceiverBase* TestGetReceiverInterface() = 0;
#endif
	
	KG_DECLARE_NO_DELETE_OP();//如果编译提示这里，表明你对这个类错误的用了delete操作，应该用Release
};

#endif