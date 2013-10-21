#pragma once

#include "IEEditor.h"

DWORD GetVariableLengthByType(KG3DDATAFLOWVARIABLETYPE eType);

class KG3DDataFlowDataDefine
{
protected:

	struct VariableDefine 
	{
		KG3DDATAFLOWVARIABLETYPE  eType;//数据成员类型
		string szName;//数据成员名称
		DWORD  dwPostionStart;//数据成员起始位置
		DWORD  dwLength;//数据成员长度

		VariableDefine()
		{
			eType = VARIABLETYPE_INT;
			dwPostionStart = 0;
			dwLength =4;
			szName = "NonameVariable";
		}
	};

	HRESULT LoadFromFile(LPCTSTR pFileName);
	HRESULT SaveToFile(LPCTSTR pFileName);

	HRESULT ComputeDataLength();
	DWORD m_dwDataLength;

	HRESULT FindVariableByName(int& nIndex,LPCTSTR pName);

public:
	DWORD GetDataLength()
	{
		return m_dwDataLength;
	}

	DWORD  m_dwType;//该数据结构的类型编号
	string m_szName;//该数据结构的名称
	vector<VariableDefine>m_vecVariableDefine;//该数据结构的每个成员的定义

	HRESULT SetName(LPCTSTR pName);
	HRESULT GetName(LPCTSTR pName);
	HRESULT SetNumVariable(DWORD dwNum);
	DWORD   GetNumVariable();
	HRESULT SetVariable(DWORD dwIndex,KG3DDATAFLOWVARIABLETYPE eType,LPCTSTR pName);
	HRESULT GetVariable(DWORD dwIndex,KG3DDATAFLOWVARIABLETYPE* peType,LPCTSTR pName);

public:
	KG3DDataFlowDataDefine(void);
	virtual ~KG3DDataFlowDataDefine(void);
};
