////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRTTIType.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-2-28 10:37:54
//  Comment     : 用于得到类信息，自己定义的RTTI
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DRTTITYPE_H_
#define _INCLUDE_KG3DRTTITYPE_H_
#include "KG3DInterface.h"
////////////////////////////////////////////////////////////////////////////////
class KG3DRTTIType : public IKG3DRTTIType
{
private:
	IKG3DRTTITypeDescStruct m_DescStruct;
public:
	KG3DRTTIType(DWORD dwType, LPCTSTR tczName);
	KG3DRTTIType(const KG3DRTTIType& OtherStruct);
	KG3DRTTIType& operator=(const KG3DRTTIType&);
	virtual ~KG3DRTTIType();

	bool before(const KG3DRTTIType& OtherStruct)const;
	virtual bool operator==(const KG3DRTTIType& OtherStruct)const;
	virtual bool operator!=(const KG3DRTTIType& OtherStruct)const;
	const TCHAR* Name()const;
		
	DWORD GetType()
	{
		return m_DescStruct.m_dwType;
	}
public:///<接口>
	virtual HRESULT GetInfo(IKG3DRTTITypeDescStruct* pDescStruct);
	virtual HRESULT IsEqual(IKG3DRTTIType* pOtherType);
	virtual HRESULT GetName(LPTSTR tczName, INT nBufferSize);
	virtual HRESULT GetType(DWORD* pdwType);
	
private:
	KG3DRTTIType();
	
};
class KG3DRTTITypeManager : protected map<DWORD, KG3DRTTIType>, public IKG3DRTTITypeManager
{
public:
	typedef map<DWORD, KG3DRTTIType> ContainerType;
	typedef ContainerType::value_type value_type;
	using ContainerType::begin;
	using ContainerType::end;
	typedef ContainerType::const_iterator const_iterator;
	using ContainerType::upper_bound;
	using ContainerType::lower_bound;
private:
	INT_PTR m_nEnvEffectTypeCount;
public:
	INT_PTR GetAllEnvEffectTypeCount(){ return m_nEnvEffectTypeCount;}
public://接口
	virtual HRESULT GetRTTIType(DWORD dwType, IKG3DRTTIType** pRetType);
public:
	KG3DRTTITypeManager();
	KG3DRTTIType* GetType(DWORD dwType);
	const TCHAR*  GetName(DWORD dwType);
};

KG3DRTTITypeManager* g_cGetRTTITypeManager();///<RTTI_INTERFACE>
extern KG3DRTTITypeManager g_cKG3DRTTITypeManager;
#endif //_INCLUDE_KG3DRTTITYPE_H_
