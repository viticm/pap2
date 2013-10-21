////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DGPS.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-19 11:41:46
//  Comment     : 这个类主要解决如何在引擎中处理玩家位置的问题。原理可以参考
/*KG3DTimeClock。这个类的派生类可以被其它任意的类Accept，然后根据策略调用
注意它使用了KG3DLifeTimeDependedClassBase，必须派生
*/
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DGPS_H_
#define _INCLUDE_KG3DGPS_H_


////////////////////////////////////////////////////////////////////////////////
struct KG3DGPSBase : public KG3D_CUSTOM_HELPERS::KG3DLifeTimeDependedClassBase<1>
{
	virtual ~KG3DGPSBase() = 0{}
	virtual HRESULT Init(D3DXVECTOR3 vecPos) = 0;
	virtual HRESULT AdjustPos(D3DXVECTOR3 vecPos) = 0;
	virtual HRESULT GetPos(D3DXVECTOR3& RefPos) = 0;
	virtual HRESULT	GetRegionIndexByPos(INT& nXIndex, INT& nZIndex)= 0;
	
};
//////////////////////////////////////////////////////////////////////////
class KG3DGPS_Dummy : public KG3DGPSBase
{
private:
	D3DXVECTOR3 m_vecCamLookAtPos;
public:
	KG3DGPS_Dummy();
public:
	virtual HRESULT Init(D3DXVECTOR3 vecPos);
	virtual HRESULT AdjustPos(D3DXVECTOR3 vecPos);
	virtual HRESULT GetPos(D3DXVECTOR3& RefPos);
	virtual HRESULT	GetRegionIndexByPos(INT& nXIndex, INT& nZIndex);
};
#endif //_INCLUDE_KG3DGPS_H_
