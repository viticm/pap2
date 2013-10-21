#pragma once

class KG3DStateDefine
{
	string m_szStateName;
	string m_szAnimationName;
};

class KG3DStateConfig
{
	map<string,DWORD>m_mapStateName;
	map<DWORD,KG3DStateDefine*>m_mapStateDefines;
	HRESULT LoadFromFile(LPSTR pFileName);
};

class KG3DStateConfigTable
{
	map<string,KG3DStateConfig*>m_mapStateConfig;
};

class KG3DStateMachineBase
{
public:
	KG3DStateConfig* m_lpStateConfig;

	HRESULT ChangeState(LPSTR pStateName);

	KG3DStateMachineBase(void);
	~KG3DStateMachineBase(void);
};
