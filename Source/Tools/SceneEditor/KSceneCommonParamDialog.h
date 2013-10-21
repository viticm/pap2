////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneCommonParamDialog.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-2 20:07:58
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef _INCLUDE_KSCENECOMMONPARAMDIALOG_H_
#define _INCLUDE_KSCENECOMMONPARAMDIALOG_H_
#include "KGStringFunctionsPT.h"
#include "IEClassMemberDescription.h"
#include <vector>
////////////////////////////////////////////////////////////////////////////////



// KSceneCommonParamDialog dialog
class KSceneCommonParamElement;
class KSceneCommonParamDialog : public CDialog
{
	DECLARE_DYNAMIC(KSceneCommonParamDialog)

public:
	KSceneCommonParamDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneCommonParamDialog();

// Dialog Data
	enum { IDD = IDD_DLG_COMMON_PARAM };
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
private:
	enum
	{
		MAX_PARAM_COUNT = 40,
		MAX_VALUE_SIZE = 128,
	};
	
	typedef vector<KSceneCommonParamElement*>	Element_Container_Type;
	typedef vector<CString>						Title_Container_Type;
	typedef vector<INT>							ValueSize_Container_Type;
	Element_Container_Type		m_vecElement;
	BOOL	m_bHasInitDialog;
	//<AddStruct>
	IEClassMemberDescriptionPack	m_LastPack;
	std::vector<INT>				m_DataHandle;
	//</AddStruct>
public:
	INT AddAnParam(DWORD dwType, CString csTitle, const VOID* pValue, INT nValueBufferSize,BOOL bEnable);///dwType是KSCENE_COMMON_PARAM中一种，pValue可选，用于初始值，Return是Param序列号，用于取得Param，返回值小于0就失败
	BOOL GetData(INT nIndex, DWORD dwType, VOID* pValueBuffer, INT nValueBufferSize, BOOL* bHasChanged);
	//////////////////////////////////////////////////////////////////////////
	template<typename _Type>
	inline INT AddAnParam(CString csTitle, const _Type& Value,BOOL bEnable)
	{
		return AddAnParam(KSH::TConvertorDWORDType<_Type>::result, csTitle, &Value, sizeof(_Type),bEnable);
	}
	template<typename _Type>
	inline BOOL GetData(INT nIndex, _Type& Value, BOOL* bHasChanged = NULL)
	{
		return GetData(nIndex, KSH::TConvertorDWORDType<_Type>::result, &Value, sizeof(_Type), bHasChanged);
	}
	template<typename _Type>
	BOOL AddStruct(const _Type& TheStruct)
	{
		IEClassMemberDescriptionPack Pack = TheStruct.GetMemberDescrption();

		if(Pack == m_LastPack || m_bHasInitDialog || NULL == Pack.m_pDesc)
			return FALSE;

		m_DataHandle.clear();
		for (size_t i = 0; i < Pack.m_NumberOfMembers; ++i)
		{
			const IEClassMemberDescription& DescElem = Pack.m_pDesc[i];

			DWORD ConvertedType = KSH::ConverKG3DTYPEToParamType(DescElem.m_Type);
			if(ConvertedType == KSH::KSCENE_COMMON_PARAM_INVALUE_TYPE)
				return FALSE;

			LPVOID lpParamPtr = ((CHAR*)&TheStruct)+DescElem.m_Offset;
			LPCTSTR pTitle = DescElem.m_MemberTitle != NULL ? DescElem.m_MemberTitle : _T("Anonymous");
				
			INT nRet = AddAnParam(ConvertedType, pTitle, lpParamPtr, (INT)DescElem.m_Size,TRUE);
			if (nRet < 0)
				return FALSE;
			m_DataHandle.push_back(nRet);
		}
		if(m_DataHandle.size() != Pack.m_NumberOfMembers)
			return FALSE;

		m_LastPack = Pack;
		return TRUE;
	}
	template<typename _Type>
	BOOL GetStruct(_Type& TheStruct)
	{
		IEClassMemberDescriptionPack Pack = TheStruct.GetMemberDescrption();
		if(Pack != m_LastPack || m_DataHandle.size() != Pack.m_NumberOfMembers)
			return FALSE;
		
		for (size_t i = 0; i < Pack.m_NumberOfMembers; ++i)
		{
			const IEClassMemberDescription& DescElem = Pack.m_pDesc[i];

			DWORD ConvertedType = KSH::ConverKG3DTYPEToParamType(DescElem.m_Type);
			if(ConvertedType == KSH::KSCENE_COMMON_PARAM_INVALUE_TYPE)
				return FALSE;

			LPVOID lpParamPtr = ((CHAR*)&TheStruct)+DescElem.m_Offset;
			LPCTSTR pTitle = DescElem.m_MemberTitle != NULL ? DescElem.m_MemberTitle : _T("Anonymous");

			BOOL bRet = GetData(m_DataHandle[i], ConvertedType, lpParamPtr, (INT)DescElem.m_Size, NULL);
			if (! bRet)
				return FALSE;
			
		}
		return TRUE;
		
	}
	//////////////////////////////////////////////////////////////////////////
	virtual BOOL OnInitDialog();
	virtual void OnOK( );
};


#endif //_INCLUDE_KSCENECOMMONPARAMDIALOG_H_
