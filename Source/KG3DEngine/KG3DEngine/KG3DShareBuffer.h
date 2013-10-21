#pragma once

class KG3DShareBuffer
{
public:
	DWORD dwLastTime;
	PVOID pUser;

	LPDIRECT3DVERTEXBUFFER9 m_lpVertexBuffer;

	KG3DShareBuffer(void);
	~KG3DShareBuffer(void);

	HRESULT Init();
	HRESULT UnInit();
};
