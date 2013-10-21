////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSettingPageBase.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-1-30 10:55:09
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESETTINGPAGEBASE_H_
#define _INCLUDE_KSCENESETTINGPAGEBASE_H_
////////////////////////////////////////////////////////////////////////////////
class KSceneSettingPageBase : public CPropertyPage
{

public:
	KSceneSettingPageBase(UINT nIDTemplate, UINT nIDCaption = 0, DWORD dwSize = sizeof(PROPSHEETPAGE))
		:CPropertyPage(nIDTemplate, nIDCaption, dwSize){}
	virtual ~KSceneSettingPageBase(){};
	virtual HRESULT SynFromInnerToInterface(DWORD_PTR dwOption = 0) = 0;	
	virtual HRESULT SynFromInterfaceToInner(DWORD_PTR dwOption = 0) = 0;
};

#endif //_INCLUDE_KSCENESETTINGPAGEBASE_H_
