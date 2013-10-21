// KResourceManager.cpp: implementation of the KResourceManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "ResourceManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*template<class HResourceType> 
KResourceManager<HResourceType>::KResourceManager()
{
	;
}

template<class HResourceType> 
KResourceManager<HResourceType>::~KResourceManager()
{
	;
}*/

/*template<class HResourceType> 
HRESULT KResourceManager<HResourceType>::FindResourceByName(KResourceHandle* pHandle,const char *szFilename)
{
	_ResourceHashMap :: const_iterator i;
	i = m_ResourceHashmap.find(szFilename);
	if( i == m_ResourceHashmap.end() )
		return E_FAIL;
	else
	{
		(*pHandle) = (*i).second;
		// 增加引用计数
		pHandle->IncRefCount();
		return S_OK;
	}
}

template<class HResourceType> 
HRESULT KResourceManager<HResourceType>::GetResource(HResourceType* HResource , const char *szFilename,
													  DWORD Option=0,DWORD Operator=0)
{
	if(FAILED(FindResourceByName(HResource,szFilename)))
	{
		if(SUCCEEDED(LoadResource(HResource,szFilename,Option,Operator)))
		{
			m_ResourceHashmap[szFilename] = *HResource;
		}
	}
	return S_OK;
}

template<class HResourceType> 
HRESULT KResourceManager<HResourceType>::LoadResource(HResourceType* HResource , const char *szFilename,
													 DWORD Option=0,DWORD Operator=0)
{
	HResource = new HResourceType;
	return S_OK;
}*/
