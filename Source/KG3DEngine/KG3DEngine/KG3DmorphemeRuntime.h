#pragma once

class KG3DmorphemeRuntime
{
public:
	KG3DmorphemeRuntime(void);
	~KG3DmorphemeRuntime(void);

	HRESULT Init();
	HRESULT UnInit();

	HRESULT LoadNetwork(LPSTR pFileName);

};
