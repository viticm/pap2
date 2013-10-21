#ifndef _INI_OPERATOR_H_
#define _INI_OPERATOR_H_

#include <tchar.h>

#define  _OUT

class KIniOperator
{
public:
	KIniOperator(void);
	~KIniOperator(void);

public:
 	int Init(const TCHAR* pszIniName);
	int Uninit();

public:
	int IsSectionExist(const TCHAR* pszSection);

public:
 	int GetInteger(const TCHAR* cpszSection, const TCHAR* cpszKeyName, int nDefault, _OUT int* pnValue);
	int GetString(const TCHAR* cpszSection, const TCHAR* cpszKeyName, const TCHAR* cpszDefault, _OUT TCHAR* pszString, unsigned uSize);
//	int GetFloat(const TCHAR* cpszSection, const TCHAR* cpszKeyName, float fDefault, _OUT float *pfValue);
//	int GetBool(const TCHAR* lpSection, const TCHAR* lpKeyName, _OUT int* pBool);
	
public:
	int WriteInteger(const TCHAR* cpszSection, const TCHAR* cpszKeyName, int nValue);
	int WriteString(const TCHAR* cpszSection, const TCHAR* cpszKeyName, const TCHAR* cpszString);
//	int WriteFloat(const TCHAR* cpszSection, const TCHAR* cpszKeyName, float fValue);
//	int WriteBool(const TCHAR* pszSection, const TCHAR* pszKeyName, bool bValue);

private:

	TCHAR m_szIniFileName[MAX_PATH];
	bool  m_bInit;
};

#endif  /*_INI_OPERATOR_H_*/