////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRegionInformer.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-13 15:12:55
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DREGIONINFORMER_H_
#define _INCLUDE_KG3DREGIONINFORMER_H_
#include "KG3DMessenger.h"
#include "IEKG3DEnvEffDynamicWeather.h"
#include "KG3DRegionInfoManager.h"
#include "KG3DGPS.h"

//////////////////////////////////////////////////////////////////////////

class IIniFile;
struct KG3DRegionInformerDescStruct : public KG3DREGION_INFORMER_DESCSTRUCT
{
	KG3DRegionInformerDescStruct();
	KG3DRegionInformerDescStruct(const KG3DRegionInformerDescStruct& OtherStruct);
	KG3DRegionInformerDescStruct(const KG3DREGION_INFORMER_DESCSTRUCT& OtherStruct);
	void operator=(const KG3DRegionInformerDescStruct& OtherStruct);
	void ToBaseStruct(KG3DREGION_INFORMER_DESCSTRUCT* pBaseStruct);
	bool SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection );
	bool LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection );
};
class KG3DRegionInformer : public KG3DMessageSender, public IEKG3DRegionInformer
{
private:
	KG3DRegionInformerDescStruct m_DescStruct;
public:
	KG3DRegionInformer();
	~KG3DRegionInformer();
	virtual HRESULT RequestModification(const KG3DREGION_INFORMER_DESCSTRUCT* pDescStruct, DWORD dwModificationOption);
	virtual HRESULT GetStruct(KG3DREGION_INFORMER_DESCSTRUCT* pDescStruct);
	HRESULT RequestModification(const KG3DRegionInformerDescStruct& DescStruct, DWORD dwModificationOption);
	
	BOOL BindManager(KG3DRegionInfoManager& InfoManager);
	HRESULT AcceptGPS(KG3DGPSBase& RefGPS);
	INT GetRegionTraitByGPS();///得到当前所在位置的Region的标志
	
	HRESULT SaveMapSetting(IIniFile* pIniFile, LPCTSTR pSection);
	HRESULT LoadMapSetting(IIniFile* pIniFile, LPCTSTR pSection);
	HRESULT FrameMove();

	//用于动态天气调整关键帧的时候暂时屏蔽Region检查
	HRESULT StopRegionChecking(INT StoppedRegion);
	HRESULT ReStartRegionChecking();
private:
	HRESULT CustomInit();
	HRESULT GetRegionIndexByGPS( INT& nXIndex, INT& nZIndex );///从GPS得到当前所在的Region的XZ坐标
private:
	INT m_nGetTimeTick;
	DWORD m_dwLastGainTime;
	INT m_nLastRegionIndex;
	INT m_nEnterRegionCount;
	//bool	m_bGainRegionIndexFromGPS;
	enum
	{
		emIsInit = 0,
		emIsRegionCheckingStopped = 1,
	};
	std::bitset<8>	m_bitset;
	bool	IsInit(){return m_bitset.test(emIsInit);}
	VOID	IsInit(bool bInit){m_bitset.set(emIsInit, bInit);}
	bool	IsRegionCheckingStopped(){return m_bitset.test(emIsRegionCheckingStopped);}
	void	IsRegionCheckingStopped(bool IsStopped){m_bitset.set(emIsRegionCheckingStopped, IsStopped);}
	KG3D_CUSTOM_HELPERS::TKG3DLifeTimeDependedPointer<KG3DRegionInfoManager*>	m_tpRegionInfoManager;
	KG3D_CUSTOM_HELPERS::TKG3DLifeTimeDependedPointer<KG3DGPSBase*>				m_tpGPS;	
};
////////////////////////////////////////////////////////////////////////////////

#endif //_INCLUDE_KG3DREGIONINFORMER_H_
