#pragma once
#include "../../../Include/kg3dengine/kg3dinterface.h"
#include "KG3DTemplates.h"
#include "KG3DMesh.h"

class KG3DMeshTable :
	public TKG3DResourceManagerBase<KG3DMesh>
{
public:
	//////////////////////////////////////////////////////////////////////////
	//初始化,清除数据
	virtual HRESULT Init();
	virtual HRESULT UnInit();
	//////////////////////////////////////////////////////////////////////////
//	virtual HRESULT LoadResourceFromFile(IKG3DResourceBase** ppResource,LPSTR pFileName);
	virtual HRESULT Get1NewResourse(IKG3DResourceBase** ppResource,DWORD dwType);
	virtual HRESULT GetResourse(IKG3DResourceBase** ppResource,DWORD dwID);
	//////////////////////////////////////////////////////////////////////////
	virtual HRESULT SetBalance(DWORD nSizeMemery,DWORD nSzieVideo);
    virtual void ChangeMesh2CpuProcess();

public:
	KG3DMeshTable(void);
	virtual ~KG3DMeshTable(void);

protected:
	HRESULT Get1NewResourcePointer(KG3DMesh** ppT,DWORD Type,DWORD_PTR Option);
};

extern KG3DMeshTable g_cMeshTable;