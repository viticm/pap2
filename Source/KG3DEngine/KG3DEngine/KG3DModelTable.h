////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DModelTable.h
//  Version     : 
//  Creator     : 
//  Create Date :  
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DMODELTABLE_H_
#define _INCLUDE_KG3DMODELTABLE_H_

////////////////////////////////////////////////////////////////////////////////

#include "IEKG3DModelTable.h"
#include "kg3dinterface.h"
#include "KG3DTemplates.h"
#include "KG3DModel.h"
#include "kg3dmodelst.h"

class KG3DModelTable : 
    public TKG3DResourceManagerBase<KG3DModel>, public IEKG3DModelTable
{
public:
	KG3DModelTable(void);
	virtual ~KG3DModelTable(void);
	//////////////////////////////////////////////////////////////////////////
	//初始化,清除数据
	virtual HRESULT Init();
	virtual HRESULT UnInit();
	//////////////////////////////////////////////////////////////////////////
//	virtual HRESULT LoadResourceFromFile(IKG3DResourceBase** ppResource, LPSTR pFileName);
	virtual HRESULT Get1NewResourse(IKG3DResourceBase** ppResource, DWORD dwType);
    virtual HRESULT IEGet1NewResourse(IEKG3DModel **ppModel, DWORD dwType);
    virtual HRESULT IELoadResourceFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption, IKG3DResourceBase **ppRetResource);
    virtual HRESULT IELoadModelFromFile(
        const TCHAR cszFileName[],
        UINT uFileNameHash,
        UINT uOption,
        IEKG3DModel **ppRetResource
    );
    virtual HRESULT GetResourse(IKG3DResourceBase** ppResource, DWORD dwID);
    virtual HRESULT Get1NewResourcePointer(KG3DModel** ppT,DWORD Type,DWORD_PTR Option);
    //////////////////////////////////////////////////////////////////////////
	virtual HRESULT SetBalance(DWORD nSizeMemery,DWORD nSzieVideo);
	virtual HRESULT LoadResourceFromFile(const char cszFileName[],
		unsigned uFileNameHash, 
		unsigned uOption, 
		IKG3DResourceBase **ppRetResource);

};

class KG3DModelSTCache
{
	KMutex m_lock;
	static const int s_nMaxCacheSize = 200;
public:
	BOOL CheckMDL(LPCSTR strFileName);
	const MDLFileContent* GetMDLContent(LPCSTR strFileName);
private:
	void OnCompact();
	std::map<DWORD, MDLFileContent> m_Cache;
	std::deque<DWORD> m_LastUsed;
};
//HRESULT g_GetModelTable(IEKG3DModelTable **ppRetModelTable);
extern KG3DModelSTCache g_cModelSTCache;
extern KG3DModelTable g_cModelTable;


#endif //_INCLUDE_KG3DMODELTABLE_H_
