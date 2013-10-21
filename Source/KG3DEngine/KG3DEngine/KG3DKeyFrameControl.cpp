#include "StdAfx.h"
#include ".\kg3dkeyframecontrol.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DKeyFrameControl::KG3DKeyFrameControl(void)
{
}

KG3DKeyFrameControl::~KG3DKeyFrameControl(void)
{
	CleanDataDefine();
}

HRESULT KG3DKeyFrameControl::CleanDataDefine()
{
	for(size_t i=0;i<m_vecDataDefine.size();i++)
	{
		SAFE_DELETE(m_vecDataDefine[i].m_pTimeLine);
	}
	m_vecDataDefine.clear();
	return S_OK;
}

