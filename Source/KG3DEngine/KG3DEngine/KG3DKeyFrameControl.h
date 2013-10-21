#pragma once

#include ".\kg3dkeyframedatabase.h"

using namespace std;

class KG3DKeyFrameControl
{
protected:
	struct DataDefine
	{
		char m_Name[64];
		TKG3DTimeLinebase* m_pTimeLine;
	};

	vector<DataDefine>m_vecDataDefine;

public:

	template <class TFormat>
		int AddDataDefine(LPSTR pszName,TFormat Data)
	{
		DataDefine Define;
		wsprintf(Define.m_Name,"%s",pszName);
		Define.m_pTimeLine = new TKG3DTimeLine<TFormat>;
		m_vecDataDefine.push_back(Define);
		return (int)m_vecDataDefine.size()-1;
	}

	HRESULT CleanDataDefine();

	KG3DKeyFrameControl(void);
	virtual ~KG3DKeyFrameControl(void);
};
