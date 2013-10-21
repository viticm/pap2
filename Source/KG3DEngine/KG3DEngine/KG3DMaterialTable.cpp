#include "StdAfx.h"
#include "kg3dmaterialtable.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DMaterialTable g_cMaterialTable;
static TypeInfo s_MaterialTableDefault = {"", 0, FALSE };
KG3DMaterialTable::KG3DMaterialTable(void)
{
	m_dwReleaseQueueSize = 80;
}

KG3DMaterialTable::~KG3DMaterialTable(void)
{
}

HRESULT KG3DMaterialTable::Init()
{
	HRESULT hRetCode = TKG3DResourceManagerBase<KG3DMaterial>::Init();
	m_RefTable = NULL;
	m_RefTableSize = 0;
	m_pDefaultType = &s_MaterialTableDefault;
	return hRetCode;
}

