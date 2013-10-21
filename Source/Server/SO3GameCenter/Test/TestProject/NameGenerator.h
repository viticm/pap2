#pragma once
#include <string>
#include "utility.h"
#include "Engine/KTextFilter.h"
#include "testtools.h"
#include <string>

class NameGenerator
{
public:
	static NameGenerator* Instance();
	bool Init();
	~NameGenerator();
	TestTools::KRoleInfo Generator();
	std::string IllegalName(int nLength);
	std::string LegalName(int nLength);
private:
	NameGenerator(void);
	int m_nIndex;
	std::string m_NameBuff;
	ScopedPtrMalloc<ITextFilter, ComRelease> m_TextFilter;
};
