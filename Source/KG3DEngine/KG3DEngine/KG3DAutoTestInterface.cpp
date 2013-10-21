////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DAutoTestInterface.cpp
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2009/6/15 9:42:51
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#ifdef KG3D_AUTO_TEST
#include "KG3DAutoTestInterface.h"

extern KG3DMeshTable	 g_cMeshTable;
extern KG3DMaterialTable g_cMaterialTable;

////////////////////////////////////////////////////////////////////////////////
class KG3DAutoTestInterface : public IKG3DAutoTestInterface
{
	ULONG m_ulRefCount;
	~KG3DAutoTestInterface(){}; // only allow delete by self, can't define variable
public:
	KG3DAutoTestInterface() { m_ulRefCount = 1;};

	// -------------------------- interface of IUnknow --------------------------
	virtual ULONG STDMETHODCALLTYPE AddRef()
	{
		return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
	}

	virtual ULONG STDMETHODCALLTYPE Release()
	{
		ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
		if (uNewRefCount == 0)
			delete this; 
		return uNewRefCount;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void **ppvObject)
	{
		return E_FAIL;
	}

	// -------------------------- interface of IKG3DAutoTestInterface --------------------------
	virtual KG3DMeshTable		*GetMeshTable()		{return &g_cMeshTable;		};
	virtual KG3DMaterialTable	*GetMaterialTable() {return &g_cMaterialTable;	};

};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int CreateAutoTestInterface(IKG3DAutoTestInterface **ppiRetAutoTest)
{
	int nResult = false;
	
	KG_ASSERT_EXIT(ppiRetAutoTest);

	*ppiRetAutoTest = new KG3DAutoTestInterface();

	nResult = true;
Exit0:
	return nResult;
}



#endif // #ifdef KG3D_AUTO_TEST
