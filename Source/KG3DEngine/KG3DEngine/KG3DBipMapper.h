#pragma once

class KG3DBip;

class KG3DBipMapper
{
public:
	struct DirectMaping
	{
		DWORD dwDestBoneIndex;
		DWORD dwSrcBoneIndex;
	};
	struct IKMaping
	{
		DWORD dwDestBoneIndex;
	};

public:
	vector<DirectMaping>m_vecDirectMaping;

	KG3DBip* m_lpBipSrc;
	KG3DBip* m_lpBipDest;

	KG3DBipMapper(void);
	~KG3DBipMapper(void);

	HRESULT Init(KG3DBip* pSrc,KG3DBip* pDest);

	HRESULT GenerateMapping();
};
