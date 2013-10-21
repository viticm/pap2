#ifndef _KREGINFO_H_
#define _KREGINFO_H_

class KRegInfo
{
public:
	static int ReadInstallPath(const TCHAR szLang[], TCHAR szPath[], size_t nCount);
	static int WriteInstallPath(const TCHAR szLang[], const TCHAR szInstallPath[]);

	static int ReadCurrentVersion(const TCHAR szLang[], TCHAR szVersion[], size_t nCount);
	static int WriteCurrentVersion(const TCHAR szLang[], const TCHAR szCurrentVersion[]);

protected:
	KRegInfo(void){};
	~KRegInfo(void){};
};

#endif