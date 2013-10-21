#include "StdAfx.h"
#include "KG3DShareBuffer.h"
#include "KG3DTemplates.h"

TKG3DResourceLimitSharePool<KG3DShareBuffer> g_cShareBufferPool;

KG3DShareBuffer::KG3DShareBuffer(void)
{
	dwLastTime = 0;
	pUser = NULL;
	m_lpVertexBuffer = NULL;
}

KG3DShareBuffer::~KG3DShareBuffer(void)
{
	UnInit();
}

HRESULT KG3DShareBuffer::Init()
{
	UnInit();
	return S_OK;
}

HRESULT KG3DShareBuffer::UnInit()
{
	dwLastTime = 0;
	pUser = NULL;
	SAFE_RELEASE(m_lpVertexBuffer);
	return S_OK;
}