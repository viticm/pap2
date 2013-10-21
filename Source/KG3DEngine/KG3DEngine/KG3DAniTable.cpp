#include "StdAfx.h"
#include "KG3DAniTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DAniTable g_cAni2Table;
static TypeInfo s_AniTableDefaultType = { "", 0, FALSE };

HRESULT KG3DAniTable::Init()
{
	HRESULT hRetCode = TKG3DResourceManagerBase<KG3DAni>::Init();
	m_pDefaultType = &s_AniTableDefaultType;
	return hRetCode;
}

KG3DAniTable::KG3DAniTable(void)
{
}

KG3DAniTable::~KG3DAniTable(void)
{
}






